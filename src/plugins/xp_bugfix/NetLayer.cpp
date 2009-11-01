/***************************************************************************
    NetLayer - Wrapper for AuroraServerNetLayer.dll and hooks into NWN2 to
	use it.

    Copyright (C) 2009 Skywing (skywing@valhallalegends.com).  This instance
    of NetLayerWindow is licensed under the GPLv2 for the usage of the NWNX4
    project, nonwithstanding other licenses granted by the copyright holder.

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

#include "bugfix.h"
#include "..\..\misc\Patch.h"
#include "ServerNetLayer.h"
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#include "NetLayer.h"

bool ReplaceNetLayer();

CNetLayerInternal * NetLayerInternal;

/***************************************************************************
    Debug output to the debugger before we can use wx logging safely.
***************************************************************************/

void
DebugPrintV(
	__in const char *Format,
	__in va_list Ap
	)
{
	CHAR Message[ 4096 ];

	//
	// Let's not clutter up things if a user mode debugger isn't present.
	//

	if (!IsDebuggerPresent())
		return;

	StringCbVPrintfA(
		Message,
		sizeof( Message ),
		Format,
		Ap
		);

	OutputDebugStringA( Message );
}

void
DebugPrint(
	__in const char *Format,
	...
	)
{
	va_list Ap;

	va_start( Ap, Format );

	DebugPrintV( Format, Ap );

	va_end( Ap );
}


bool PlayerIdToConnectionId(unsigned long PlayerId, unsigned long *ConnectionId)
{
	unsigned long idx;

	if (PlayerId > MAX_PLAYERS)
		return false;

	if (!NetLayerInternal->Players[PlayerId].m_bPlayerInUse)
		return false;

	idx = NetLayerInternal->Players[PlayerId].m_nSlidingWindowId;

	if (!NetLayerInternal->Windows[idx].m_WindowInUse)
		return false;

	*ConnectionId = NetLayerInternal->Windows[idx].m_ReceiveConnectionId;

	return true;
}

bool PlayerIdToSlidingWindow(unsigned long PlayerId, unsigned long *SlidingWindowId)
{
	if (PlayerId > MAX_PLAYERS)
		return false;

	for (unsigned long i = 0; i < MAX_PLAYERS; i += 1)
	{
		if (NetLayerInternal->Windows[i].m_WindowInUse != 1)
			continue;

		if (NetLayerInternal->Windows[i].m_PlayerId == PlayerId)
		{
			*SlidingWindowId = i;
			return true;
		}
	}

	return false;
}

/*

0:000> u 004ff8a0 
nwn2server!CNetLayerInternal::PlayerIdToConnectionId:
004ff8a0 8b442404        mov     eax,dword ptr [esp+4]
004ff8a4 8bd0            mov     edx,eax
004ff8a6 c1e204          shl     edx,4
004ff8a9 2bd0            sub     edx,eax
004ff8ab 83bcd19076030000 cmp     dword ptr [ecx+edx*8+37690h],0
004ff8b3 8d04d1          lea     eax,[ecx+edx*8]
004ff8b6 7425            je      nwn2server!CNetLayerInternal::PlayerIdToConnectionId+0x3d (004ff8dd)
004ff8b8 8b80a0760300    mov     eax,dword ptr [eax+376A0h]
0:000> u
nwn2server!CNetLayerInternal::PlayerIdToConnectionId+0x1e:
004ff8be 69c03c090000    imul    eax,eax,93Ch
004ff8c4 03c1            add     eax,ecx
004ff8c6 83781400        cmp     dword ptr [eax+14h],0
004ff8ca 7411            je      nwn2server!CNetLayerInternal::PlayerIdToConnectionId+0x3d (004ff8dd)
004ff8cc 8b401c          mov     eax,dword ptr [eax+1Ch]
004ff8cf 8b4c2408        mov     ecx,dword ptr [esp+8]
004ff8d3 8901            mov     dword ptr [ecx],eax
004ff8d5 b801000000      mov     eax,1
0:000> u
nwn2server!CNetLayerInternal::PlayerIdToConnectionId+0x3a:
004ff8da c20800          ret     8
004ff8dd 33c0            xor     eax,eax
004ff8df c20800          ret     8
004ff8e2 cc              int     3
004ff8e3 cc              int     3
004ff8e4 cc              int     3
004ff8e5 cc              int     3
004ff8e6 cc              int     3
*/


BOOL
__stdcall
SendMessageToPlayer(
	__in unsigned long PlayerId,
	__in_bcount( Size ) unsigned char * Data,
	__in unsigned long Size,
	__in unsigned long Flags
	);

BOOL
__stdcall
SendMessageToPlayer2(
	__in unsigned long PlayerId,
	__in_bcount( Size ) unsigned char * Data,
	__in unsigned long Size,
	__in unsigned long Flags
	);

BOOL
__stdcall
FrameTimeout2(
	__in unsigned long Unused
	);

BOOL
__stdcall
FrameReceive2(
	__in_bcount( Size ) unsigned char *Data,
	__in unsigned long Size
	);

Patch _patches2[] =
{
	Patch( OFFS_SendMessageToPlayer, "\xe9", 1 ),
	Patch( OFFS_SendMessageToPlayer+1, (relativefunc)SendMessageToPlayer2 ),
	Patch( OFFS_CallFrameTimeout, (relativefunc)FrameTimeout2 ),
	Patch( OFFS_FrameReceive, "\xe9", 1 ),
	Patch( OFFS_FrameReceive+1, (relativefunc)FrameReceive2 ),
	Patch( OFFS_FrameSend, "\xc2\x0c\x00", 3 ), // Disable all outbound sends (from FrameTimeout)

	Patch( )
};

Patch *patches2 = _patches2;

bool
__cdecl
OnNetLayerWindowReceive(
	__in_bcount( Length ) const unsigned char * Data,
	__in size_t Length,
	__in void * Context
	);

bool
__cdecl
OnNetLayerWindowSend(
	__in_bcount( Length ) const unsigned char * Data,
	__in size_t Length,
	__in void * Context
	);

bool
__cdecl
OnNetLayerWindowStreamError(
	__in bool Fatal,
	__in unsigned long ErrorCode,
	__in void * Context
	);

void ResetWindow(unsigned long PlayerId)
{
	CONNECTION_CALLBACKS Callbacks;
	NETLAYER_HANDLE      Handle;

	if (Connections[PlayerId] != NULL)
		AuroraServerNetLayerDestroy_(Connections[PlayerId]);

	Callbacks.Context       = (void *) (ULONG_PTR) PlayerId;
	Callbacks.OnReceive     = OnNetLayerWindowReceive;
	Callbacks.OnSend        = OnNetLayerWindowSend;
	Callbacks.OnStreamError = OnNetLayerWindowStreamError;

	Handle = AuroraServerNetLayerCreate_(Connections[PlayerId], &Callbacks);

	if (Handle == NULL)
		return;

	Connections[PlayerId] = Handle;
}

bool ReplaceNetLayer()
{
	//
	// Wire up the dllimports.
	//

	struct { const char *Name; void **Import; } DllImports[] =
	{
		{ "AuroraServerNetLayerCreate",  (void **) &AuroraServerNetLayerCreate_  },
		{ "AuroraServerNetLayerSend",    (void **) &AuroraServerNetLayerSend_    },
		{ "AuroraServerNetLayerReceive", (void **) &AuroraServerNetLayerReceive_ },
		{ "AuroraServerNetLayerTimeout", (void **) &AuroraServerNetLayerTimeout_ },
		{ "AuroraServerNetLayerDestroy", (void **) &AuroraServerNetLayerDestroy_ }
	};
	AuroraServerNetLayer = LoadLibrary("AuroraServerNetLayer.dll");

	if (!AuroraServerNetLayer)
	{
		wxLogMessage(wxT("* Failed to load AuroraServerNetLayer.dll"));
		return false;
	}

	for (int i = 0; i < sizeof(DllImports)/sizeof(DllImports[0]); i += 1)
	{
		*DllImports[i].Import = (void *)GetProcAddress(AuroraServerNetLayer, DllImports[i].Name);

		if (!*DllImports[i].Import)
		{
			wxLogMessage(wxT("* Unable to resolve AuroraServerNetLayer!%s"), DllImports[i].Name);
			return false;
		}
	}

	int i = 0;
	while(patches2[i].Apply()) {
		i++;
	}

	//
	// Set each window into a good initial state.
	//

	for (unsigned long i = 0; i < MAX_PLAYERS; i += 1)
		ResetWindow( i );

	return false;
}

BOOL
__stdcall
SendMessageToPlayer(
	__in unsigned long PlayerId,
	__in_bcount( Size ) unsigned char * Data,
	__in unsigned long Size,
	__in unsigned long Flags
	)
{
	DebugPrint( "SendMessageToPlayer(%08X, %p, %08X, %08X)\n", PlayerId, Data, Size, Flags);

	//
	// Enqueue the outbound message to each applicable player's internal
	// NetLayerWindow object.
	//

	for (unsigned Player = 0; Player < MAX_PLAYERS; Player += 1)
	{
		//
		// Ignore players that are not present.
		//

		if (!NetLayerInternal->Players[Player].m_bPlayerInUse)
			continue;

		//
		// Check if this player matches the given filter.  We will continue the
		// next loop iteration if it does not.
		//

		switch (PlayerId)
		{

		case PLAYERID_SERVER:
			DebugPrint("Send to server...\n");
			return TRUE;

		case PLAYERID_ALL_PLAYERS:
			if (!NetLayerInternal->Players[Player].m_bPlayerPrivileges)
				continue;
			break;

		case PLAYERID_ALL_GAMEMASTERS:
			if (!NetLayerInternal->Players[Player].m_bGameMasterPrivileges)
				continue;
			break;

		case PLAYERID_ALL_SERVERADMINS:
			if (!NetLayerInternal->Players[Player].m_bServerAdminPrivileges)
				continue;
			break;

		default: // Specific player id and not a symbolic filter
			if (PlayerId != Player)
				continue;

			//
			// Fallthrough.
			//

		case PLAYERID_ALL_CLIENTS: // Anyone, even if they are not a player yet
			break;

		}

		if (Size >= 0x03 && Data[0] == 0x50)
			DebugPrint("Send %s.%02X to player %lu (%lu bytes)\n", GetMsgName(Data[1]), Data[2], PlayerId, Size);

		AuroraServerNetLayerSend_(
			Connections[PlayerId],
			Data,
			Size,
			FALSE,
			(Flags & SEND_FLUSH_CACHE) ? TRUE : FALSE);

	}

	return TRUE;
}

__declspec(naked)
BOOL
__stdcall
SendMessageToPlayer2(
	__in unsigned long PlayerId,
	__in_bcount( Size ) unsigned char * Data,
	__in unsigned long Size,
	__in unsigned long Flags
	)
{
	__asm
	{
		mov  [NetLayerInternal], ecx
		jmp  SendMessageToPlayer
	}
}

__declspec(naked)
void
__stdcall
SetFrameInTimer(
	__in SlidingWindow * Winfo
	)
{
	__asm
	{
		mov   ecx, dword ptr [esp+04h]
		mov   eax, OFFS_SetInFrameTimer
		jmp   dword ptr [eax]
	}
}

__declspec(naked)
void
__stdcall
CallFrameTimeout(
	__in unsigned long Unused,
	__in SlidingWindow * Winfo
	)
{
	__asm
	{
		push    ebp
		mov     ebp, esp

		push    dword ptr [ebp+08h]
		mov     ecx, dword ptr [ebp+0ch]
		mov     eax, OFFS_FrameTimeout
		call    dword ptr [eax]

		mov     esp, ebp
		pop     ebp
		ret     08h
	}
}

BOOL
__stdcall
FrameReceive(
	__in_bcount( Size ) unsigned char *Data,
	__in unsigned long Size,
	__in SlidingWindow *Winfo
	)
{
	DebugPrint(
		"FrameReceive: Recv from SlidingWindow %p player %lu\n",
		Winfo,
		Winfo->m_PlayerId);

	//
	// Update timeouts so that the server doesn't drop this player for timeout.
	//

	SetFrameInTimer( Winfo );

	//
	// Pass it on to the internal NetLayerWindow implementation to deal with.
	//

	return AuroraServerNetLayerReceive_(
		Connections[Winfo->m_PlayerId],
		Data,
		(size_t) Size);
}

__declspec(naked)
BOOL
__stdcall
FrameReceive2(
	__in_bcount( Size ) unsigned char *Data,
	__in unsigned long Size
	)
{
	__asm
	{
		push    ebp
		mov     ebp, esp

		push    ecx
		push    dword ptr [ebp+0ch]
		push    dword ptr [ebp+08h]
		call    FrameReceive

		mov     esp, ebp
		pop     ebp
		ret     08h
	}
}

BOOL
__stdcall
FrameTimeout(
	__in unsigned long Unused,
	__in SlidingWindow *Winfo
	)
{
	DebugPrint(
		"FrameTimeout: Do timer processing for SlidingWindow %p player %lu\n",
		Winfo,
		Winfo->m_PlayerId);

	//
	// Perform timeout handling in the replacement NetLayerWindow.
	//

	AuroraServerNetLayerTimeout_(Connections[Winfo->m_PlayerId]);

	//
	// Let the server do its internal timeout handling too, so that it may drop
	// a player that has become unresponsive.  N.B.  We've neutered the
	// FrameSend API, so FrameTimeout's attempts to send a NAK/ACK will not be
	// harmful.  It will still be able to call DisconnectPlayer as appropriate.
	//

	CallFrameTimeout( Unused, Winfo );

	return TRUE;
}

__declspec(naked)
BOOL
__stdcall
FrameTimeout2(
	__in unsigned long Unused
	)
{
	__asm
	{
		push    ebp
		mov     ebp, esp

		push    ecx
		push    dword ptr [ebp+08h]
		call    FrameTimeout

		mov     ebp, esp
		ret     04h
	}
}



__declspec(naked)
BOOL
__stdcall
CallHandleMessage(
	__in_bcount( Length ) const unsigned char * Data,
	__in size_t Length,
	__in unsigned long PlayerId
	)
{
	__asm
	{
		;
		; BOOL
		; CServerExoApp::HandleMessage(
		;   __in unsigned long PlayerId,
		;   __in_bcount( Length ) const unsigned char * Data,
		;   __in size_t Size,
		;   __in bool NonClientMessage
		;   );
		;

		push    ebp
		mov     ebp, esp

		push    0h
		push    dword ptr [ebp+10h]
		push    dword ptr [ebp+0ch]
		push    dword ptr [ebp+08h]

		mov     ecx, dword ptr [NetLayerInternal]
		mov     ecx, dword ptr [ecx]
		mov     edx, dword ptr [ecx+10h]
		call    edx

		mov     esp, ebp
		pop     ebp
		ret     0ch
	}
}

bool
__cdecl
OnNetLayerWindowReceive(
	__in_bcount( Length ) const unsigned char * Data,
	__in size_t Length,
	__in void * Context
	)
{
	unsigned long     PlayerId = (unsigned long) (ULONG_PTR) Context;
	PlayerInfo      * Pinfo;
	SlidingWindow   * Winfo;

	Pinfo = &NetLayerInternal->Players[ PlayerId ];
	Winfo = &NetLayerInternal->Windows[ Pinfo->m_nSlidingWindowId ];

	if (Length >= 0x03)
	{
		DebugPrint(
			"OnNetLayerWindowReceive: Recv %s.%02X from player %lu WindowId %lu\n",
			GetMsgName(Data[1]),
			Data[2],
			PlayerId,
			Pinfo->m_nSlidingWindowId);
	}

	if (!Pinfo->m_bPlayerInUse)
	{
		DebugPrint( "OnNetLayerWindowReceive: Player not active!\n" );
		return false;
	}

	if (Winfo->m_PlayerId != PlayerId)
	{
		wxLogMessage(wxT("OnNetLayerWindowReceive: *** SLIDING WINDOW PLAYER ID MISMATCH !! %lu, %lu, %lu (%p, %p)\n"),
			PlayerId,
			Winfo->m_PlayerId,
			Pinfo->m_nSlidingWindowId,
			Pinfo,
			Winfo);
		DebugPrint(
			"OnNetLayerWindowReceive: *** SLIDING WINDOW PLAYER ID MISMATCH !! %lu, %lu, %lu (%p, %p)\n",
			PlayerId,
			Winfo->m_PlayerId,
			Pinfo->m_nSlidingWindowId,
			Pinfo,
			Winfo);

		if (IsDebuggerPresent( ))
			__debugbreak( );

		return false;
	}

	//
	// Pass the packet on to the actual handler, indicating it up to the game
	// for completion handling.
	//

	return CallHandleMessage( Data, Length, PlayerId ) ? TRUE : FALSE;
}

bool
__cdecl
OnNetLayerWindowSend(
	__in_bcount( Length ) const unsigned char * Data,
	__in size_t Length,
	__in void * Context
	)
{
	unsigned long     PlayerId = (unsigned long) (ULONG_PTR) Context;
	PlayerInfo      * Pinfo;
	SlidingWindow   * Winfo;
	CExoNetInternal * NetI;
	sockaddr_in       sin;
	int               slen;

	Pinfo = &NetLayerInternal->Players[ PlayerId ];
	Winfo = &NetLayerInternal->Windows[ Pinfo->m_nSlidingWindowId ];

	DebugPrint( "OnNetLayerWindowSend: Send to player %lu WindowId %lu\n", PlayerId, Pinfo->m_nSlidingWindowId );

	if (!Pinfo->m_bPlayerInUse)
	{
		DebugPrint( "OnNetLayerWindowSend: Player not active!\n" );
		return false;
	}

	if (Winfo->m_PlayerId != PlayerId)
	{
		wxLogMessage(wxT("OnNetLayerWindowSend: *** SLIDING WINDOW PLAYER ID MISMATCH !! %lu, %lu, %lu (%p, %p)\n"),
			PlayerId,
			Winfo->m_PlayerId,
			Pinfo->m_nSlidingWindowId,
			Pinfo,
			Winfo);
		DebugPrint(
			"OnNetLayerWindowSend: *** SLIDING WINDOW PLAYER ID MISMATCH !! %lu, %lu, %lu (%p, %p)\n",
			PlayerId,
			Winfo->m_PlayerId,
			Pinfo->m_nSlidingWindowId,
			Pinfo,
			Winfo);

		if (IsDebuggerPresent( ))
			__debugbreak( );

		return false;
	}

	NetI = NetLayerInternal->Net->Internal;

	if (Winfo->m_ConnectionId >= NetI->NumConnections)
	{
		wxLogMessage(wxT("OnNetLayerWindowSend: *** ConnectionId for player %lu window %lu out of range !! %lu >= %lu (NetI %p)\n"),
			PlayerId,
			Pinfo->m_nSlidingWindowId,
			Winfo->m_ConnectionId,
			NetI->NumConnections);
		DebugPrint("OnNetLayerWindowSend: *** ConnectionId for player %lu window %lu out of range !! %lu >= %lu (NetI %p)\n",
			PlayerId,
			Pinfo->m_nSlidingWindowId,
			Winfo->m_ConnectionId,
			NetI->NumConnections);

		if (IsDebuggerPresent( ))
			__debugbreak( );

		return false;
	}

	//
	// Write the raw message out using the game's sendto socket and the address
	// information for this player.
	//

	ZeroMemory( sin.sin_zero, sizeof( sin.sin_zero ) );
	sin.sin_family      = AF_INET;
	sin.sin_port        = htons( NetI->ConnectionAddresses[ Winfo->m_ConnectionId ].Port );
	sin.sin_addr.s_addr = NetI->ConnectionAddresses[ Winfo->m_ConnectionId ].Address;

	DebugPrint( "Send to %08X:%lu\n", NetI->ConnectionAddresses[ Winfo->m_ConnectionId ].Port, sin.sin_addr.s_addr );

	slen = sendto(
		NetI->Socket,
		(const char *) Data,
		(int) Length,
		0,
		(sockaddr *) &sin,
		sizeof( sin ));

	if (slen < 0)
	{
		DebugPrint( "sendto fails - %lu\n", WSAGetLastError( ) );
		return false;
	}

	return true;
}

bool
__cdecl
OnNetLayerWindowStreamError(
	__in bool Fatal,
	__in unsigned long ErrorCode,
	__in void * Context
	)
{
	unsigned long PlayerId;

	PlayerId = (unsigned long) (ULONG_PTR) Context;

	//
	// Just log a warning that something is broken.  We could disconnect the
	// player here too, but synchronizing that with the rest of the internal
	// state would be painful.  We could also just write a disconnect packet
	// out to the player and then let the server's timeout handling eventually
	// drop the server's player state for that player.
	//

	wxLogMessage(wxT("OnNetLayerWindowStreamError: Stream error %lu for player %lu...\n"),
		PlayerId,
		ErrorCode);
	DebugPrint("OnNetLayerWindowStreamError: Stream error %lu for player %lu...\n",
		PlayerId,
		ErrorCode);

	return true;
}


