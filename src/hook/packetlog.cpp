/***************************************************************************
    NWNX Hook - Responsible for the actual hooking
    Copyright (C) 2007 Ingmar Stieger (Papillon, papillon@nwnx.org)
	Copyright (C) 2008 Skywing (skywing@valhallalegends.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

***************************************************************************/

#include "stdwx.h"
#include "hook.h"


/***************************************************************************
    Server packet logging.
***************************************************************************/

/*
 * Assembler wrappers around our functions to account for calling convention
 * differences and provide call filtering support.
 */

__declspec(naked) void ProcessServerMessageHook()
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		sub     esp, 04h
		push    ecx

		lea     edx, dword ptr [ebp-04h]
		push    edx
		push    ecx
		push    dword ptr [ebp+10h]
		push    dword ptr [ebp+0ch]
		call    FilterRecvServerMessage

		;
		; Determine if we are to call the original client handler.  If so then
		; we'll do that now, otherwise we use the return value that was
		; indicated by our internal filter function.
		;

		mov     edx, dword ptr [ebp-04h]

		test    edx, edx

		jz      retpoint

		pop     ecx

		push    dword ptr [ebp+10h]
		push    dword ptr [ebp+0ch]
		push    dword ptr [ebp+08h]
		mov     eax, OFFS_ProcessServerMessage
		call    eax

retpoint:
		mov     esp, ebp
		pop     ebp
		ret     0ch

	}
}

bool
OpenLogPacketFile(
	)
{
	PLOGGED_PACKET View;
	HANDLE         File;
	HANDLE         Section;
	bool           Success;
	WCHAR          FileName[ MAX_PATH + 1 ];

	File    = INVALID_HANDLE_VALUE;
	Section = 0;
	Success = false;

	for (;;)
	{
		//
		// If we have no directory configured, bail out.
		//

		if (g_LogPacketDir == wxT( "" ))
			break;

#ifdef UNICODE
		if (FAILED(StringCbPrintfW(
			FileName,
			sizeof( FileName ),
			L"%s\\packetlog_%lu_%lu_%lu.log",
			logPacketDir.c_str(),
			static_cast< unsigned long >( time( 0 ) ),
			GetCurrentProcessId(),
			GetCurrentThreadId())))
			break;
#else
		if (FAILED(StringCbPrintfW(
			FileName,
			sizeof( FileName ),
			L"%S\\packetlog_%lu_%lu_%lu.log",
			g_LogPacketDir.c_str(),
			static_cast< unsigned long >( time( 0 ) ),
			GetCurrentProcessId(),
			GetCurrentThreadId())))
			break;
#endif

		//
		// Open our log file.
		//

		File = CreateFileW(
			FileName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			0,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			0
			);

		if (File == INVALID_HANDLE_VALUE)
			break;

		//
		// Resize it.
		//

		SetFilePointer(
			File,
			sizeof( LOGGED_PACKET ) * MAX_LOGGED_PACKETS,
			0,
			FILE_BEGIN
			);

		SetEndOfFile( File );

		//
		// Create our section.
		//

		Section = CreateFileMappingW(
			File,
			0,
			PAGE_READWRITE,
			0,
			0,
			0
			);

		if (!Section)
			break;

		//
		// Map a section view.
		//

		View = (PLOGGED_PACKET)MapViewOfFile(
			Section,
			FILE_MAP_WRITE,
			0,
			0,
			0
			);

		if (!View)
			break;

		//
		// We're done.  Initialize the buffer and return success.
		//

		ZeroMemory(
			View,
			sizeof( LOGGED_PACKET ) * MAX_LOGGED_PACKETS
			);

		for (ULONG i = 0;
		     i < MAX_LOGGED_PACKETS;
		     i += 1)
		{
			View[ i ].Length = sizeof( LOGGED_PACKET );
		}

		g_LogPacketView  = View;
		g_LogPacketIndex = 0;

		Success = true;
	}

	if (File != INVALID_HANDLE_VALUE)
		CloseHandle( File );
	if (Section)
		CloseHandle( Section );
	if (!Success && View)
		UnmapViewOfFile( View );

	return Success;
}

void
WritePacketToLog(
	__in const void *PacketData,
	__in unsigned long Length
	)
{
	PLOGGED_PACKET Packet;

	if (!g_LogPacketView)
		return;

	//
	// No synchronization is necessary as packets are always handled in the main thread.
	//

	Packet = &g_LogPacketView[ g_LogPacketIndex ];

	g_LogPacketIndex = (g_LogPacketIndex + 1) % MAX_LOGGED_PACKETS;

	//
	// Write packet attributes out to the section view mapping.
	//

	GetSystemTimeAsFileTime( &Packet->RecvTime );

	Packet->ActualPacketLength = Length;

	if (Length > MAX_LOGGED_PACKET_LENGTH)
		Length = MAX_LOGGED_PACKET_LENGTH;

	Packet->PacketLength = Length;

	//
	// Write the packet data to the buffer.  Note that we truncate packets if
	// they were too large.  We record the original length and the truncated
	// length so that we can determine this fact.
	//

	memcpy( Packet->PacketData, PacketData, Length );
}

bool
RegisterLogPacketHook(
	)
{
	ULONG  OldProt;
	ULONG  Patch;

	//
	// First, make sure that we're patching the right nwn2server.exe version.
	//

	__try
	{
		if (*reinterpret_cast< unsigned long * >( OFFS_ProcessServerMessageHook ) != CHECK_ProcessServerMessageHook)
		{
			DebugPrint(
				"RegisterLogPacketHook(): Wrong nwn2server version, not logging packets.\n"
				);
			return false;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return false;
	}

	//
	// Reprotect memory so we can edit it.  Note that as we are not yet
	// initialized we don't really have to worry about the server trying to
	// receive a packet while we do surgery.
	//

	if (!VirtualProtect(
		reinterpret_cast< PVOID >( OFFS_ProcessServerMessageHook ),
		4,
		PAGE_READWRITE,
		&OldProt))
		return false;

	//
	// Calculate the relative delta from the next instruction EIP to our
	// desired call destination address, and write it in.
	//

	Patch = reinterpret_cast< unsigned long >( ProcessServerMessageHook ) - (OFFS_ProcessServerMessageHook + 4);

	*reinterpret_cast< unsigned long * >( OFFS_ProcessServerMessageHook ) = Patch;

	//
	// Restore old page protection.
	//

	VirtualProtect(
		reinterpret_cast< PVOID >( OFFS_ProcessServerMessageHook ),
		4,
		OldProt,
		&OldProt
		);

	DebugPrint( "RegisterLogPacketHook(): Packet log hook enabled.\n" );

	return true;
}

struct NWMSGCLS
{
	enum CLSENUM
	{
		Server = 0x50,
		Client = 0x70,

		MaximumNwnCls
	};
};

struct NWMSG
{
	enum MSGENUM
	{
		ServerStatus              = 0x01,
		Login                     = 0x02,
		Module                    = 0x03,
		Area                      = 0x04,
		GameObjUpdate             = 0x05,
		Input                     = 0x06,
		Store                     = 0x07,
		Gold                      = 0x08,
		Chat                      = 0x09,
		PlayerList                = 0x0A,
		UnusedB                   = 0x0B,
		Inventory                 = 0x0C,
		GuiInventory              = 0x0D,
		Party                     = 0x0E,
		Cheat                     = 0x0F,
		Camera                    = 0x10,
		CharList                  = 0x11,
		ClientSideMessage         = 0x12,
		Combat_Round              = 0x13,
		Dialog                    = 0x14,
		GuiCharacterSheet         = 0x15,
		QuickChat                 = 0x16,
		Sound                     = 0x17,
		Item_Property             = 0x18,
		GuiContainer              = 0x19,
		VoiceChat                 = 0x1A,
		GuiInfoPopup              = 0x1B,
		Journal                   = 0x1C,
		LevelUp                   = 0x1D, // Um?
		GuiHotBar                 = 0x1E,
		DungeonMaster             = 0x1F,
		MapPin                    = 0x20,
		DebugInfo                 = 0x21,
		SafeProjectile            = 0x22,
		Barter                    = 0x23,
		PopUpGUIPanel             = 0x24,
		Death                     = 0x25,
		GroupInput                = 0x26,
		DungeonMasterGroup        = 0x27,
		Ambient                   = 0x28,
		UnknownNameMsg            = 0x29, // They used the function pointer instead of the string in the debug print...
		Portal                    = 0x2A,
		Character_Download        = 0x2B,
		LoadBar                   = 0x2C,
		SaveLoad                  = 0x2D,
		GUIPartyBar               = 0x2E,
		ShutDownServer            = 0x2F,
		LevelUp2                  = 0x30, // Um?
		PlayModuleCharacterList   = 0x31,
		CustomToken               = 0x32,
		Cutscene                  = 0x33,
		GuiRoster                 = 0x34,
		WorldMap                  = 0x35,
		CustomAnim                = 0x36,



		MaximumNwnMsg
	};
};


#pragma warning(push)
#pragma warning(disable:4200) // non-standard extension used: zero length array
#include <pshpack1.h>
struct message
{
	unsigned char cls;
	unsigned char function;
	unsigned char subfunction;
	unsigned long length; // includes all but cls
	unsigned char data[];
};
#include <poppack.h>
#pragma warning(pop)


/*
 * Helper/wrapper functions and hook trampoline functions.
 */

#define _MSGN(n) case NWMSG::##n: return #n;

const char *
GetMsgName(
	__in UCHAR MsgId
	)
{
	switch (MsgId)
	{

	_MSGN(ServerStatus)
	_MSGN(Login)
	_MSGN(Module)
	_MSGN(Area)
	_MSGN(GameObjUpdate)
	_MSGN(Input)
	_MSGN(Store)
	_MSGN(Gold)
	_MSGN(Chat)
	_MSGN(PlayerList)
	_MSGN(UnusedB)
	_MSGN(Inventory)
	_MSGN(GuiInventory)
	_MSGN(Party)
	_MSGN(Cheat)
	_MSGN(Camera)
	_MSGN(CharList)
	_MSGN(ClientSideMessage)
	_MSGN(Combat_Round)
	_MSGN(Dialog)
	_MSGN(GuiCharacterSheet)
	_MSGN(QuickChat)
	_MSGN(Sound)
	_MSGN(Item_Property)
	_MSGN(GuiContainer)
	_MSGN(VoiceChat)
	_MSGN(GuiInfoPopup)
	_MSGN(Journal)
	_MSGN(LevelUp)
	_MSGN(GuiHotBar)
	_MSGN(DungeonMaster)
	_MSGN(MapPin)
	_MSGN(DebugInfo)
	_MSGN(SafeProjectile)
	_MSGN(Barter)
	_MSGN(PopUpGUIPanel)
	_MSGN(Death)
	_MSGN(GroupInput)
	_MSGN(DungeonMasterGroup)
	_MSGN(Ambient)
	_MSGN(UnknownNameMsg)
	_MSGN(Portal)
	_MSGN(Character_Download)
	_MSGN(LoadBar)
	_MSGN(SaveLoad)
	_MSGN(GUIPartyBar)
	_MSGN(ShutDownServer)
	_MSGN(LevelUp2)
	_MSGN(PlayModuleCharacterList)
	_MSGN(CustomToken)
	_MSGN(Cutscene)
	_MSGN(GuiRoster)
	_MSGN(WorldMap)
	_MSGN(CustomAnim)

	default:
		{
			static char UnkBuf[ 16 ];

			StringCbPrintfA(UnkBuf, sizeof( UnkBuf ), "%02X", MsgId);

			return UnkBuf;
		}
		break;

	}
}

#undef _MSGN

/*
 * Called when the server is about to receive a game-impacting message.  If the
 * ``CallServerHandler'' value is set to FALSE, then the return value is
 * indicated to the caller.  Otherwise, the original server message handler is
 * called and the message will be processed normally.
 */

// Dumphex output procedure definition
typedef void (__cdecl DumphexOutputProc)(void *Param, const char *pszFormat, ...);


void dumphex(const char *buf, int len, int pos, DumphexOutputProc *pOutputProc, void *Param)
{
	int i, j;
	for(j = 0; j < len; j += 16) {
		for(i = 0; i < 16; i++) {
			if(i + j < len)
				pOutputProc(Param, "%02x%c", (unsigned char)buf[i + j], j + i + 1 == pos ? '*' : ' ');
			else 
				pOutputProc(Param, "   ");
		}
		for(i = 0; i < 16; i++) {
			if(i + j < len)
				pOutputProc(Param, "%c", buf[i + j] >= ' ' ? buf[i + j] : '.');
			else 
				pOutputProc(Param, " ");
		}
		pOutputProc(Param, "\n");
	}
}

void __cdecl DebugPrintDumphexOutput(void *Param, const char *pszFormat, ...)
{
	va_list argptr;
	va_start(argptr, pszFormat);
	DebugPrintV(pszFormat, argptr);
	va_end(argptr);
}

void dumphex(CONST PVOID P, ULONG Length, ULONG Mark /* = 0xFFFFFFFF */)
{
	dumphex((char*)P, (int)Length, (int)Mark, DebugPrintDumphexOutput, 0);
}

BOOL
__stdcall
FilterRecvServerMessage(
	__in char *Buf,
	__in unsigned long Length,
	__inout void *Server,
	__out PBOOL CallServerHandler
	)
{
	message *msg = (message *)Buf;

	*CallServerHandler = TRUE;

	//
	// Write the packet to our log.
	//

	WritePacketToLog(
		Buf,
		Length
		);

	//
	// Skip all of this if we're not being debugged, for speed.
	//

	if (!IsDebuggerPresent())
		return TRUE;

	if (Length < 1)
	{
		DebugPrint( "FilterRecvServerMessage: Got zero length message\n" );
		return TRUE;
	}

	if (msg->cls != NWMSGCLS::Client)
		return TRUE;

	if (Length < sizeof( message ))
	{
		DebugPrint(
			"FilterRecvServerMessage: Got too small client message\n"
			);
		dumphex( Buf, Length, -1 );
		return TRUE;
	}

	if (msg->length+1 > Length)
	{
		DebugPrint(
			"FilterRecvServerMessage: Got too long message length %lu for actual length %lu\n",
			msg->length,
			Length
			);
		return TRUE;
	}

	DebugPrint(
		"FilterRecvServerMessage: RECV %s.%02x - %lu:\n",
		GetMsgName( msg->function ),
		msg->subfunction,
		msg->length-6,
		(Length-7) - (msg->length-6));

	dumphex(msg->data, Length-7 > 256 ? 256 : Length-7, (Length-7 == msg->length-6) ? -1 : msg->length-6);

	return TRUE;
}
