/***************************************************************************
    NWNX BugFix - NWN2Server bugfixes and patches plugin
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
#if !defined(BUGFIX_H_INCLUDED)
#define BUGFIX_H_INCLUDED

#define DLLEXPORT extern "C" __declspec(dllexport)

#include "windows.h"
#include "../plugin.h"
#include "../../misc/log.h"
#include "wx/tokenzr.h"
#include "wx/hashset.h"
#include <set>
#include "../../nwn2lib/NWN2Lib.h"

/*

.text:006FB5EF                 mov     ecx, [esi+28h]
.text:006FB5F2                 mov     eax, [ecx]
.text:006FB5F4
.text:006FB5F4 loc_6FB5F4:                             ; CODE XREF: sub_6FB1E0+41Ej
.text:006FB5F4                 mov     [eax+4], ecx
.text:006FB5F7                 mov     ecx, eax
.text:006FB5F9                 cmp     ecx, [esi+24h]
.text:006FB5FC                 mov     eax, [eax]
.text:006FB5FE                 jnz     short loc_6FB5F4
.text:006FB600
.text:006FB600 loc_6FB600:                             ; CODE XREF: sub_6FB1E0+19j
.text:006FB600                                         ; sub_6FB1E0+3EEj ...
.text:006FB600                 pop     edi

.text:006FB23D                 mov     ecx, [esi+28h]
.text:006FB240                 mov     eax, [ecx]
.text:006FB242
.text:006FB242 loc_6FB242:                             ; CODE XREF: sub_6FB1E0+6Cj
.text:006FB242                 mov     [eax+4], ecx
.text:006FB245                 mov     ecx, eax
.text:006FB247                 cmp     ecx, [esi+24h]
.text:006FB24A                 mov     eax, [eax]
.text:006FB24C                 jnz     short loc_6FB242
.text:006FB24E
.text:006FB24E loc_6FB24E:                             ; CODE XREF: sub_6FB1E0+53j
.text:006FB24E                 pop     ebp
.text:006FB24F                 mov     dword ptr [esi+58h], 2
*/



class BugFix : public Plugin
{
public:
	BugFix();
	~BugFix();

	bool Init(TCHAR* nwnxhome);  

	int GetInt(char* sFunction, char* sParam1, int nParam2) { return 0; }
	void SetInt(char* sFunction, char* sParam1, int nParam2, int nValue) {};
	float GetFloat(char* sFunction, char* sParam1, int nParam2) { return 0.0; }
	void SetFloat(char* sFunction, char* sParam1, int nParam2, float fValue) {};
	void SetString(char* sFunction, char* sParam1, int nParam2, char* sValue);
	char* GetString(char* sFunction, char* sParam1, int nParam2);
	void GetFunctionClass(TCHAR* fClass);


	static void CalcPositionsLoop0Fix();
	static void CalcPositionsLoop1Fix();
	static void NullDerefCrash0Fix();
	static void NullDerefCrash1Fix();
	static void NullDerefCrash2Fix();
	static void NullDerefCrash3Fix();
	static void NullDerefCrash4Fix();
	static void Crash5Fix();
	static void NullDerefCrash6Fix();
	static void NullDerefCrash7Fix();
	static void NullDerefCrash8Fix();
	static void NullDerefCrash9Fix();
	static void Uncompress0Fix();
	static void Uncompress1Fix();

private:

	bool Check();
	static void __stdcall SafeInitPositionList(NWN2::somestruc *struc);
	static void __stdcall LogNullDerefCrash0();
	static void __stdcall LogNullDerefCrash1();
	static void __stdcall LogNullDerefCrash2();
	static void __stdcall LogNullDerefCrash3();
	static void __stdcall LogNullDerefCrash4();
	static void __stdcall LogCrash5();
	static void __stdcall LogNullDerefCrash6();
	static void __stdcall LogNullDerefCrash7();
	static void __stdcall LogNullDerefCrash8();
	static void __stdcall LogNullDerefCrash9();
	static void __stdcall LogUncompress0Fix();
	static void __stdcall LogUncompress1Fix();

	static void __stdcall FreeNwn2Heap(void *p);

	wxLogNWNX* logger;
	ULONG      lastlog;
	HMODULE    nwn2mm;
};

#endif
