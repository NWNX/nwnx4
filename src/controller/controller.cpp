/***************************************************************************
    NWNX Controller - Controls the server process
    Copyright (C) 2006 Ingmar Stieger (Papillon, papillon@nwnx.org)

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
#include "controller.h"

NWNXController::NWNXController(wxFileConfig *config)
{
	this->config = config;

	tick = 0;
	initialized = false;
	processWatchdog = true;
	gamespyWatchdog = true;
	gamespyPort = 5121;
	gamespyInterval = 30;
	restartDelay = 5;
	gamespyRetries = 0;
	gamespyTolerance = 5;
	gamespyDelay = 30;

	config->Read(wxT("restartDelay"), &restartDelay);
	config->Read(wxT("processWatchdog"), &processWatchdog);
	config->Read(wxT("gamespyWatchdog"), &gamespyWatchdog);
	config->Read(wxT("gamespyInterval"), &gamespyInterval);
	config->Read(wxT("gamespyTolerance"), &gamespyTolerance);
	config->Read(wxT("gamespyDelay"), &gamespyDelay);

	if (!config->Read(wxT("parameters"), &parameters) )
	{
		wxLogMessage(wxT("Parameter setting not found in nwnx.ini. Starting server with empty commandline."));
	}
	parameters.Prepend(wxT(" "));

	if (gamespyWatchdog)
	{
		config->Read(wxT("gamespyPort"), &gamespyPort);
		udp = new CUDP("localhost", gamespyPort);
	}

	if (!config->Read(wxT("nwn2"), &nwnhome))
	{
		wxLogMessage(wxT("* NWN2 home directory not found. Check your nwnx.ini file."));
		return;
	}
}

NWNXController::~NWNXController()
{
}


/***************************************************************************
    NWNServer related functions
***************************************************************************/

void NWNXController::startServerProcess()
{
    SHARED_MEMORY shmem;
	wxString nwnexe(wxT("\\nwn2server.exe"));

#ifdef DEBUG
	PCHAR pszHookDLLPath = "NWNX4_Hookd.dll";			// Debug DLL
#else
	PCHAR pszHookDLLPath = "NWNX4_Hook.dll";			// Release DLL
#endif

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	wxLogTrace(TRACE_VERBOSE, wxT("Starting server executable %s in %s"), nwnhome + nwnexe, nwnhome);

	CHAR szDllPath[MAX_PATH];
	PCHAR pszFilePart = NULL;

	if (!GetFullPathName(pszHookDLLPath, arrayof(szDllPath), szDllPath, &pszFilePart)) 
	{
		wxLogMessage(wxT("Error: %s could not be found."), pszHookDLLPath);
		return;
	}

	SECURITY_ATTRIBUTES SecurityAttributes;
	SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	SecurityAttributes.bInheritHandle = TRUE;
	SecurityAttributes.lpSecurityDescriptor = 0;

	shmem.ready_event = CreateEvent(&SecurityAttributes, TRUE, FALSE, 0);
	if(!shmem.ready_event)
	{ 
		wxLogMessage(wxT("CreateEvent failed (%d)"), GetLastError());
		return;
	}

	wxLogMessage(wxT("Starting: `%s'"), nwnhome + nwnexe);
	wxLogMessage(wxT("with `%s'"),  szDllPath);

	DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;
	SetLastError(0);

	if (!DetourCreateProcessWithDll(nwnhome + nwnexe, (LPTSTR)parameters.c_str(),
                                    NULL, NULL, TRUE, dwFlags, NULL, nwnhome,
                                    &si, &pi, szDllPath, NULL))   
	{
		wxLogMessage(wxT("DetourCreateProcessWithDll failed: %d\n"), GetLastError());
		return;
    }

	GUID my_guid =
	{ /* d9ab8a40-f4cc-11d1-b6d7-006097b010e3 */
		0xd9ab8a40,
		0xf4cc,
		0x11d1,
		{0xb6, 0xd7, 0x00, 0x60, 0x97, 0xb0, 0x10, 0xe3}
	};

	GetCurrentDirectory(MAX_PATH, shmem.nwnx_home);
	wxLogMessage(wxT("NWNX home directory set to %s"), shmem.nwnx_home);
	//wxLogTrace(TRACE_VERBOSE, wxT("NWNX home directory set to %s"), shmem.nwnx_home);

	DetourCopyPayloadToProcess(pi.hProcess, my_guid, &shmem, sizeof(SHARED_MEMORY));

	ResumeThread(pi.hThread);
	switch(WaitForSingleObject(shmem.ready_event, 60000))
	{
		case WAIT_TIMEOUT:
			wxLogMessage(wxT("! Error: Server did not initialize properly (timeout).\n"));
			return;
			break;
		case WAIT_FAILED:
			wxLogMessage(wxT("! Error: Server did not initialize properly (wait failed).\n"));
			return;
			break;
		case WAIT_OBJECT_0:
			CloseHandle(shmem.ready_event);
			wxLogMessage(wxT("Success: Server initialized properly.\n"));
			break;
	}

    DWORD dwResult = 0;
    if (!GetExitCodeProcess(pi.hProcess, &dwResult)) 
	{
		wxLogMessage(wxT("GetExitCodeProcess failed: %d\n"), GetLastError());
		return;
    }
	
	wxLogMessage(wxT("* Hook installed and initialized successfully"));
	initialized = true;
}

void NWNXController::restartServerProcess()
{
	// Kill any leftovers
	if (checkProcessActive())
		killServerProcess();
	
	// Run maintenance command
	wxString restartCmd;
	config->Read(wxT("restartCmd"), &restartCmd);
	if (restartCmd != wxT(""))
	{
		//restartCmd.Prepend(wxT("/c "));
		ZeroMemory(&si,sizeof(si));
		si.cb = sizeof(si);
		wxLogMessage(wxT("* Starting maintenance file %s"), restartCmd);
		restartCmd.Prepend(wxT("cmd.exe /c "));
		CreateProcess(NULL, (LPTSTR)restartCmd.c_str(), NULL, NULL,FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

		// Give the maintenance program time to finish
		while(checkProcessActive())
			Sleep(1000);
	}

	// Finally restart the server
	wxLogMessage(wxT("* Waiting %d seconds before restarting the server."), restartDelay);
	Sleep(restartDelay * 1000);
	wxLogMessage(wxT("* Restarting server."));
	startServerProcess();
}

void NWNXController::killServerProcess()
{
	if (!pi.hProcess)
		return;

	shutdownServerProcess();
	TerminateProcess(pi.hProcess, -1);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	pi.hProcess = 0;
	pi.hThread = 0;
}

void NWNXController::shutdownServerProcess()
{
	initialized = false;
}


/***************************************************************************
    Watchdog related functions
***************************************************************************/

void NWNXController::ping()
{
	if (initialized)
	{
		if (processWatchdog)
			runProcessWatchdog();
		if (gamespyWatchdog && (tick % gamespyInterval == 0) && (tick > (unsigned long) gamespyDelay))
			runGamespyWatchdog();
		tick++;
	}
}

bool NWNXController::checkProcessActive()
{
	DWORD lpExitCode;
	if (GetExitCodeProcess(pi.hProcess, &lpExitCode))
		if (lpExitCode == STILL_ACTIVE)
			return true;
	return false;
}

void NWNXController::runProcessWatchdog()
{
	if (checkProcessActive() == false)
	{
		wxLogMessage(wxT("* Server process has gone away."));
		restartServerProcess();
	}
}

void NWNXController::runGamespyWatchdog()
{	
	int ret;
	char buffer[2048];

	udp->sendMessage("BNLM");
	Sleep(UDP_WAIT);
	ret = udp->getMessage(buffer, 2048);

	if (ret == 0)
	{
		// No reply from server
		wxLogMessage(wxT("* Warning: Server did not answer gamespy query. %d retries left."),
			gamespyTolerance - gamespyRetries);
		gamespyRetries++;
	}
	else
	{
		// Server answered
		gamespyRetries = 0;
	}

	if (gamespyRetries > gamespyTolerance)
	{
		// Restart server
		wxLogMessage(wxT("* Server did not answer the last %d gamespy queries."), gamespyTolerance);
		gamespyRetries = 0;
		restartServerProcess();
	}
}
