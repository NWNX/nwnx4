/***************************************************************************
    NWNX Time - Timer functions plugin
    Copyright (C) 2007 Ingmar Stieger (Papillon, papillon@nwnx.org)

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

#include "time.h"

/***************************************************************************
    NWNX and DLL specific functions
***************************************************************************/

Timer* plugin;

DLLEXPORT Plugin* GetPluginPointerV2()
{
	return plugin;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		plugin = new Timer();

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
    Implementation of Timer Plugin
***************************************************************************/

Timer::Timer()
{
	header = 
		"NWNX Timer Plugin V.0.0.2\n" \
		"(c) 2007 by Ingmar Stieger (Papillon)\n" \
		"visit us at http://www.nwnx.org\n";

	description = 
		"This plugin provides highly accurate timers.";

	subClass = "TIME";
	version = "0.0.2";

	QueryPerformanceFrequency(&liFrequency);
}

Timer::~Timer()
{
	wxLogMessage(wxT("* Plugin unloaded."));
}

bool Timer::Init(TCHAR* nwnxhome)
{
	assert(GetPluginFileName());

	/* Log file */
	wxString logfile(nwnxhome); 
	logfile.append(wxT("\\"));
	logfile.append(GetPluginFileName());
	logfile.append(wxT(".txt"));
	logger = new wxLogNWNX(logfile, wxString(header.c_str()));

	wxLogMessage(wxT("* Plugin initialized."));
	return true;
}

void Timer::GetFunctionClass(TCHAR* fClass)
{
	_tcsncpy_s(fClass, 128, wxT("TIME"), 4); 
}

void Timer::SetString(char* sFunction, char* sParam1, int nParam2, char* sValue)
{
	wxLogTrace(TRACE_VERBOSE, wxT("* Plugin SetString(0x%x, %s, %d, %s)"), 0x0, sParam1, nParam2, sValue);

#ifdef UNICODE
	wxString wxRequest(sFunction, wxConvUTF8);
	wxString function(sFunction, wxConvUTF8);
	wxString timerName(sParam1, wxConvUTF8);
#else
	wxString wxRequest(sFunction);
	wxString function(sFunction);
	wxString timerName(sParam1);
#endif

	if (function == wxT(""))
	{
		wxLogMessage(wxT("* Function not specified."));
		return;
	}

	if (timerName == wxT(""))
	{
		wxLogMessage(wxT("* Timer name not specified."));
		return;
	}

	if (function == wxT("START"))
	{
		wxLogMessage(wxT("o Starting timer %s"), timerName);
		StartTimer(timerName);
	}
	else if (function == wxT("STOP"))
	{
		LONGLONG result = StopTimer(timerName);
		wxLogMessage(wxT("o Stopping timer %s: %I64i µs / %.3f msec / %.3f sec\n"), timerName, result, (float) result / 1000, (float) result / 1000 / 1000);
	}
}

char* Timer::GetString(char* sFunction, char* sParam1, int nParam2)
{
	wxLogTrace(TRACE_VERBOSE, wxT("* Plugin GetString(0x%x, %s, %d)"), 0x0, sParam1, nParam2);

#ifdef UNICODE
	wxString wxRequest(sFunction, wxConvUTF8);
	wxString function(sFunction, wxConvUTF8);
	wxString timerName(sParam1, wxConvUTF8);
#else
	wxString wxRequest(sFunction);
	wxString function(sFunction);
	wxString timerName(sParam1);
#endif

	if (function == wxT(""))
	{
		wxLogMessage(wxT("* Function not specified."));
		return NULL;
	}

	if (timerName == wxT(""))
	{
		wxLogMessage(wxT("* Timer name not specified."));
	}

	if (function == wxT("QUERY"))
	{
		LONGLONG result = PeekTimer(timerName);
		wxLogMessage(wxT("o Elapsed timer %s: %I64i µs / %.3f msec / %.3f sec\n"), timerName, result, (float) result / 1000, (float) result / 1000 / 1000);
		sprintf_s(returnBuffer, MAX_BUFFER, "%I64i", result);
	}
	else if (function == wxT("STOP"))
	{
		LONGLONG result = StopTimer(timerName);
		wxLogMessage(wxT("o Stopping timer %s: %I64i µs / %.3f msec / %.3f sec\n"), timerName, result, (float) result / 1000, (float) result / 1000 / 1000);
		sprintf_s(returnBuffer, MAX_BUFFER, "%I64i", result);
	}
	else
	{
		// Process generic functions
		wxString query = ProcessQueryFunction(function.ToStdString().c_str());
		if (query != wxT(""))
		{
			sprintf_s(returnBuffer, MAX_BUFFER, "%s", query);
		}
		else
		{
			wxLogMessage(wxT("* Unknown function '%s' called."), function);
			return NULL;
		}
	}

	return returnBuffer;
}

void Timer::StartTimer(wxString name)
{
	LARGE_INTEGER liLast;
	QueryPerformanceCounter(&liLast);
	timers[name] = liLast;
}

LONGLONG Timer::StopTimer(wxString name)
{
	LONGLONG result = PeekTimer(name);
	timers.erase(name);
	return result;
}

LONGLONG Timer::PeekTimer(wxString name)
{
	LARGE_INTEGER liCurrent;
	QueryPerformanceCounter(&liCurrent);

	TimerHashMap::iterator it = timers.find(name);
	if (it != timers.end())
		return (((liCurrent.QuadPart - it->second.QuadPart) * 1000000) / liFrequency.QuadPart);
	else
		return 0;
}
