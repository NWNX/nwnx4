/***************************************************************************
    NWNX Profiler - Profiler plugin
	Copyright (C) 2003 Ingmar Stieger (Papillon, papillon@blackdagger.com)
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

#include "profiler.h"
#include "hook.h"

/***************************************************************************
    NWNX and DLL specific functions
***************************************************************************/

Profiler* plugin;

DLLEXPORT Plugin* GetPluginPointerV2()
{
	return plugin;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		plugin = new Profiler();

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
    Implementation of Functions Plugin
***************************************************************************/

Profiler::Profiler()
{
	header = _T(
		"NWNX Profiler Plugin V.1.0.0\n" \
		"(c) 2003 Ingmar Stieger (papillon@blackdagger.com)\n" \
		"(c) 2007 virusman (virusman@virusman.ru)\n" \
		"visit us at http://www.nwnx.org\n");

	description = _T(
		"This plugin provides script profiling functionality.");

	subClass = _T("PROFILER");
	version = _T("1.0.0");
}

Profiler::~Profiler()
{
	wxLogMessage(wxT("* Plugin unloaded."));
}

bool Profiler::Init(TCHAR* nwnxhome)
{
	assert(GetPluginFileName());

	/* Log file */
	wxString logfile(nwnxhome); 
	logfile.append(wxT("\\"));
	logfile.append(GetPluginFileName());
	logfile.append(wxT(".txt"));
	logger = new wxLogNWNX(logfile, wxString(header.c_str()));

	LoadConfiguration(nwnxhome);
	HookRunScript();

	wxLogMessage(wxT("* Plugin initialized."));
	return true;
}

void Profiler::GetFunctionClass(TCHAR* fClass)
{
	_tcsncpy_s(fClass, 128, wxT("PROFILER"), 9); 
}

void Profiler::LoadConfiguration(TCHAR* nwnxhome)
{
	wxString inifile(nwnxhome); 
	inifile.append(wxT("\\"));
	inifile.append(GetPluginFileName());
	inifile.append(wxT(".ini"));
	wxLogTrace(TRACE_VERBOSE, wxT("* reading inifile %s"), inifile);

	config = new wxFileConfig(wxEmptyString, wxEmptyString, 
		inifile, wxEmptyString, wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_NO_ESCAPE_CHARACTERS);

	config->Read(wxT("LogLevel"), &m_LogLevel, 1);
	config->Read(wxT("scriptparts"), &log_scriptparts, 1);

	wxLogMessage(wxT("* Log level: "));
	switch (m_LogLevel)
	{
	case logStats:
		wxLogMessage(wxT("Only overall statistics will be logged."));
		break;
	case logCallstack:
		wxLogMessage(wxT("Script callstack will be logged."));
		break;
	}
	wxLogMessage(wxT("* scriptparts = %d"), log_scriptparts);

}