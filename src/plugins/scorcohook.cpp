/***************************************************************************
scorcohook.cpp - Hooking of StoreCampaingObject and RetrieveCampaignObject
Copyright (C) 2007 virusman (virusman@virusman.ru)

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

#include "scorcohook.h"
#include "dbplugin.h"
#include "mysql/mysql.h" //This is a temporary design flaw workaround: see below

void (*OriginalSCO)();
void (*OriginalRCO)();
//DESIGN FLAW
//Can't access base class here
extern MySQL* plugin;

int __stdcall SCOHookProc(char** database, char** key, char** player, int flags, unsigned char * pData, int size)
{
	int *pThis;
	__asm {mov pThis, ecx}
	if(!*pThis) return 0;

	if (memcmp(*database, "NWNX", 4))
	{
		_asm { leave }
		_asm { jmp OriginalSCO }
	}
	__asm { pushad }
	int lastRet = plugin->WriteScorcoData(pData, size);
	__asm { popad }
	return lastRet;
}

unsigned char * __stdcall RCOHookProc(char** database, char** key, char** player, int* arg4, int* size)
{
	int *pThis;
	__asm {mov pThis, ecx}
	if(!*pThis) return NULL;

	if (memcmp(*database, "NWNX", 4))
	{
		_asm { leave }
		_asm { jmp OriginalRCO }
	}
	__asm { pushad }
	unsigned char * lastRet = plugin->ReadScorcoData(*key, size);
	__asm { popad }
	return lastRet; 
}


DWORD FindHookSCO()
{
	char* ptr = (char*) 0x400000;
	while (ptr < (char*) 0x800000)
	{
		if ((ptr[0] == (char) 0x8b) &&
			(ptr[1] == (char) 0x09) &&
			(ptr[2] == (char) 0x85) &&
			(ptr[3] == (char) 0xc9) &&
			(ptr[4] == (char) 0x74) &&
			(ptr[5] == (char) 0x05) &&
			(ptr[6] == (char) 0xE9) &&
			(ptr[7] == (char) 0xE5) &&
			(ptr[8] == (char) 0x12) &&
			(ptr[9] == (char) 0x00) &&
			(ptr[0xA] == (char) 0x00) &&
			(ptr[0xB] == (char) 0x33) &&
			(ptr[0xC] == (char) 0xC0) &&
			(ptr[0xD] == (char) 0xC2) &&
			(ptr[0xE] == (char) 0x18) &&
			(ptr[0xF] == (char) 0x00)
			)
			return (DWORD) ptr;
		else
			ptr++;
	}
	return NULL;
}

DWORD FindHookRCO()
{
	char* ptr = (char*) 0x400000;
	while (ptr < (char*) 0x800000)
	{
		if ((ptr[0] == (char) 0x8b) &&
			(ptr[1] == (char) 0x09) &&
			(ptr[2] == (char) 0x85) &&
			(ptr[3] == (char) 0xc9) &&
			(ptr[4] == (char) 0x74) &&
			(ptr[6] == (char) 0xE9) &&
			(ptr[7] == (char) 0xB5) &&
			(ptr[8] == (char) 0x0F) &&
			(ptr[9] == (char) 0x00) &&
			(ptr[0xA] == (char) 0x00) &&
			(ptr[0xB] == (char) 0x33) &&
			(ptr[0xC] == (char) 0xC0) &&
			(ptr[0xD] == (char) 0xC2) &&
			(ptr[0xE] == (char) 0x14) &&
			(ptr[0xF] == (char) 0x00)
			)
			return (DWORD) ptr;
		else
			ptr++;
	}
	return NULL;
}

int HookSCORCO()
{
	int sco_success, rco_success;
	DWORD sco = FindHookSCO();
	if (sco)
	{
		wxLogMessage(wxT("o SCO located at %x."), sco);
		//odmbc.Log(0, "o SCO located at %x.\n", sco);
		//sco_success = HookCode((PVOID) sco, SCOHookProc, (PVOID*) &OriginalSCO);
		*(DWORD*)&OriginalSCO = sco;
		sco_success = DetourAttach(&(PVOID&)OriginalSCO, SCOHookProc);
	}
	else
	{
		wxLogMessage(wxT("! SCO locate failed."));
		return 0;
	}

	DWORD rco = FindHookRCO();
	if (rco)
	{
		wxLogMessage(wxT("o RCO located at %x."), rco);
		//rco_success = HookCode((PVOID) rco, RCOHookProc, (PVOID*) &OriginalRCO);
		*(DWORD*)&OriginalRCO = rco;
		rco_success = DetourAttach(&(PVOID&)OriginalRCO, RCOHookProc);
	}
	else
	{
		wxLogMessage(wxT("! RCO locate failed."));
		return 0;
	}
	return sco_success&&rco_success;
}

