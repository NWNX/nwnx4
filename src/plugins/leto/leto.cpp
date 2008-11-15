/***************************************************************************
    NWNX Leto - Leto bridge
    Copyright (C) 2007 virusman (virusman@virusman.ru)
	Copyright (C) 2004 David Frauzel (dragonsong), dragon@weathersong.net

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

#include "leto.h"

/***************************************************************************
    NWNX and DLL specific functions
***************************************************************************/

Leto* plugin;
#define INI_FILE ".\\nwnx_leto.ini"

DLLEXPORT LegacyPlugin* GetPluginPointer()
{
	return plugin;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		plugin = new Leto();

		TCHAR szPath[MAX_PATH];
		GetModuleFileName(hModule, szPath, MAX_PATH);
		plugin->SetPluginFullPath(szPath);
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		delete plugin;
	}
    return TRUE;
}


/***************************************************************************
    Implementation of Leto Plugin
***************************************************************************/

Leto::Leto()
{
	header = _T(
		"NWNX Leto Plugin V.0.0.1\n" \
		"(c) 2007 by virusman (virusman@virusman.ru)\n" \
		"(c) 2004 by David Frauzel (dragonsong), dragon@weathersong.net\n" \
		"visit us at http://www.nwnx.org\n");

	description = _T(
		"This plugin is a bridge to LetoScript DLL.");

	subClass = _T("LETO");
	version = _T("0.0.1");
}

Leto::~Leto()
{
	wxLogMessage(wxT("* Unloading the plugin."));
	//wxLogMessage(wxT("* Leto OnRelease..."));
	/*if (lpfnOnRelease)
		lpfnOnRelease();*/
	wxLogMessage(wxT("* Freeing the library..."));
	if (hDLL != NULL)
		FreeLibrary(hDLL);

	wxLogMessage(wxT("* Plugin unloaded."));
}

LPCSTR Leto::sTimeReport(LARGE_INTEGER c1, LARGE_INTEGER c2)
{
	sTime[0] = '\0';
	__int64 ValueNS = ((c2.QuadPart - c1.QuadPart) * 1000000000)/clkFreq.QuadPart;
	if (ValueNS<1000)
		sprintf(sTime,"%I64dns",ValueNS);
	else
		if (ValueNS<10000)
			sprintf(sTime,"%0.3fµs",(float)(ValueNS/1000.0));
		else
			if (ValueNS<100000)
				sprintf(sTime,"%0.2fµs",(float)(ValueNS/1000.0));
			else
				if (ValueNS<1000000)
					sprintf(sTime,"%0.1fµs",(float)(ValueNS/1000.0));
				else
					if (ValueNS<10000000)
						sprintf(sTime,"%0.3fms",(float)(ValueNS/1000000.0));
					else
						if (ValueNS<100000000)
							sprintf(sTime,"%0.2fms",(float)(ValueNS/1000000.0));
						else
							if (ValueNS<1000000000)
								sprintf(sTime,"%0.1fms",(float)(ValueNS/1000000.0));
							else
								if (ValueNS<10000000000)
									sprintf(sTime,"%0.3fs",(float)(ValueNS/1000000000.0));
								else
									if (ValueNS<100000000000)
										sprintf(sTime,"%0.2fs",(float)(ValueNS/1000000000.0));
									else
										sprintf(sTime,"%0.1fs",(float)(ValueNS/1000000000.0));
	return sTime;
}

bool Leto::Init(TCHAR* nwnxhome)
{
	assert(GetPluginFileName());

	/* Log file */
	wxString logfile(nwnxhome); 
	logfile.append(wxT("\\"));
	logfile.append(GetPluginFileName());
	logfile.append(wxT(".txt"));
	logger = new wxLogNWNX(logfile, wxString(header.c_str()));

	iDebug = GetPrivateProfileInt(	"Leto", "Debug", 2, INI_FILE);
	if (iDebug==2)
		WritePrivateProfileString(	"Leto", "Debug", "2", INI_FILE );
	//
	QueryPerformanceFrequency(&clkFreq); 
	QueryPerformanceCounter(&clkStart);

	wxLogMessage(wxT("* Loading LetoScript.dll..."));
	// attempt to load LetoScript.Dll
	hDLL = LoadLibrary("LetoScript.dll");
	if (hDLL == NULL) {
		wxLogMessage(wxT("* Failed loading LetoScript.dll! Our bridges are burned! Run away, run away!\n"));
		return FALSE;
	}
	// Stop! Who approacheth the Bridge of DLL
	// must GetProcAddress me these function pointers three,
	// ere the other side he see.

	// OnCreate
	lpfnOnCreate = (LPFNONCREATE)GetProcAddress(hDLL, "OnCreate");
	if (!lpfnOnCreate)
	{
		wxLogMessage(wxT("* Failed GetProcAddress of OnCreate. You are cast into the Gorge of Eternal Peril!\n"));
		return FALSE;
	}

	// OnRelease
	lpfnOnRelease = (LPFNONRELEASE)GetProcAddress(hDLL, "OnRelease");
	if (!lpfnOnRelease)
	{
		wxLogMessage(wxT("* Failed GetProcAddress of OnRelease. You are cast into the Gorge of Eternal Peril!\n"));
		return FALSE;
	}

	// OnRequest
	lpfnOnRequest = (LPFNONREQUEST)GetProcAddress(hDLL, "OnRequest");
	if (!lpfnOnRequest)
	{
		wxLogMessage(wxT("* Failed GetProcAddress of OnRequest. You are cast into the Gorge of Eternal Peril!\n"));
		return FALSE;
	}

	wxLogMessage(wxT("nwnxhome=%s"), nwnxhome);
	// Now call OnCreate (LetoScript.dll initialization)
	lpfnOnCreate(nwnxhome);

	if (iDebug)
	{
		QueryPerformanceCounter(&clkEnd);
		wxLogMessage(wxT("* LetoScript metamodule activated (%s).\n"),sTimeReport(clkStart,clkEnd));
	}
	else
		wxLogMessage(wxT("* LetoScript metamodule activated.\n"));


	wxLogMessage(wxT("* Plugin initialized."));
	return true;
}

void Leto::GetFunctionClass(TCHAR* fClass)
{
	_tcsncpy_s(fClass, 128, wxT("LETO"), 9); 
}



const char* Leto::DoRequest(char *gameObject, char* request, char* parameters)
{
	wxLogTrace(TRACE_VERBOSE, wxT("* Plugin DoRequest(0x%x, %s, %s)"), gameObject, request, parameters);

#ifdef UNICODE
	wxString wxRequest(request, wxConvUTF8);
#else
	wxString wxRequest(request);
#endif
	wxString function;

	wxStringTokenizer tkz(wxRequest, wxT("!"));
	
	if (tkz.HasMoreTokens())
	{
		function = tkz.GetNextToken();
		wxLogTrace(TRACE_VERBOSE, wxT("* function=%s"), function);
	}
	else
	{
		wxLogMessage(wxT("* Function not specified."));
		return NULL;
	}

	if (!lpfnOnRequest)
		return NULL;
	if (iDebug==1)
		wxLogMessage(wxT("Req:\"%s\", Param:\"%s\""),request, parameters);
	if (iDebug<=1) return lpfnOnRequest(gameObject, request, parameters);
	QueryPerformanceCounter(&clkStart);
	char *sRes = lpfnOnRequest(gameObject, request, parameters);
	QueryPerformanceCounter(&clkEnd);
	wxLogMessage(wxT("<%s> Req:\"%s\", Param:\"%s\""),sTimeReport(clkStart,clkEnd), request, parameters);
	wxLogMessage(wxT("Returned:\"%s\""), sRes);

	return sRes;
	// Pass the request along to LetoScript.dll


	ProcessQueryFunction(function.c_str(), parameters);

	return NULL;
}

