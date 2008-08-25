/***************************************************************************
    NWNX SrvAdmin - NWN2Server Administrative GUI Functions
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
#if !defined(SRVADMIN_H_INCLUDED)
#define SRVADMIN_H_INCLUDED

#define DLLEXPORT extern "C" __declspec(dllexport)

#include <windows.h>
#include <windowsx.h>
#include <specstrings.h>
#include "../plugin.h"
#include "../../misc/log.h"

class SrvAdmin : public Plugin
{
public:
	SrvAdmin();
	~SrvAdmin();

	bool Init(TCHAR* nwnxhome);  

	int GetInt(char* sFunction, char* sParam1, int nParam2) { return 0; }
	void SetInt(char* sFunction, char* sParam1, int nParam2, int nValue) {};
	float GetFloat(char* sFunction, char* sParam1, int nParam2) { return 0.0; }
	void SetFloat(char* sFunction, char* sParam1, int nParam2, float fValue) {};
	void SetString(char* sFunction, char* sParam1, int nParam2, char* sValue);
	char* GetString(char* sFunction, char* sParam1, int nParam2);
	void GetFunctionClass(TCHAR* fClass);



private:

	wxLogNWNX* logger;
	HWND       mainGuiWindow;

	static
	BOOL
	CALLBACK
	FindServerGuiWindowEnumProc(
		__in HWND hwnd,
		__in LPARAM lParam
		);

	HWND
	FindServerGuiWindow(
		);

	int
	BroadcastServerMessage(
		__in const char *Message
		);

	bool
	SelectPlayerListBox(
		__in HWND ListBox,
		__in const char *PlayerName
		);

	int
	BootPlayer(
		__in const char *PlayerName
		);

	int
	BanPlayerName(
		__in const char *PlayerName
		);

	int
	BanPlayerIP(
		__in const char *PlayerName
		);

	int
	BanPlayerCDKey(
		__in const char *PlayerName
		);

	int
	ShutdownNwn2server(
		);

	int
	SetELC(
		bool EnableELC
		);

};

#endif
