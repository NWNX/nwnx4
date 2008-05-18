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

	config->Read(wxT("restartDelay"), &restartDelay);
	config->Read(wxT("processWatchdog"), &processWatchdog);
	config->Read(wxT("gamespyWatchdog"), &gamespyWatchdog);
	config->Read(wxT("gamespyInterval"), &gamespyInterval);
	config->Read(wxT("gamespyTolerance"), &gamespyTolerance);

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
	wxString nwnexe(wxT("\\nwn2server.exe"));

	ZeroMemory(&si,sizeof(si));
	si.cb = sizeof(si);

	wxLogTrace(TRACE_VERBOSE, wxT("Starting server executable %s in %s"), nwnhome + nwnexe, nwnhome);

#ifdef _DEBUG
	CreateProcessEx(nwnhome + nwnexe, (LPTSTR)parameters.c_str(), NULL, NULL,FALSE, NORMAL_PRIORITY_CLASS, NULL, nwnhome, &si, &pi, _T("NWNX4_Hookd.DLL"));
#else
	CreateProcessEx(nwnhome + nwnexe, (LPTSTR)parameters.c_str(), NULL, NULL,FALSE, NORMAL_PRIORITY_CLASS, NULL, nwnhome, &si, &pi, _T("NWNX4_Hook.DLL"));
#endif

	initializeServerProcess();
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
	shutdownServerProcess();
	TerminateProcess(pi.hProcess, -1);
}

void NWNXController::initializeServerProcess()
{
    nwnx_client = new NWNXClient;

	// Connect via IPC
	while (!connectIPCServer(pi.dwProcessId))
	{
		Sleep(1000);
	}

	// Set home directory
	if (nwnx_client->IsConnected())
	{
		TCHAR* dir = (TCHAR*) malloc(MAX_PATH);
		GetCurrentDirectory(MAX_PATH, dir);
		wxString home(dir);
		free(dir);

		bool success = nwnx_client->GetConnection()->Poke(wxT("set_nwnx_home"), (wxChar*) home.c_str());
		if (success)
			wxLogTrace(TRACE_VERBOSE, wxT("NWNX home directory set to %s"), home.c_str());
		else
			wxLogMessage(wxT("NWNX home directory %s not accepted by server"), home.c_str());
	}

	// Tell the other side to initialize itself
	if (nwnx_client->IsConnected())
	{
		bool success = nwnx_client->GetConnection()->Execute(wxT("init"));
		if (success)
		{
			wxLogMessage(wxT("* Hook installed and initialized successfully"));
		}
		else
			wxLogMessage(wxT("* Hook not installed or error during initialization"));
	}

	initialized = true;
}

void NWNXController::shutdownServerProcess()
{
	delete nwnx_client;
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
		if (gamespyWatchdog && (tick % gamespyInterval == 0))
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


/***************************************************************************
    IPC related functions
***************************************************************************/

bool NWNXController::connectIPCServer(unsigned long pid)
{
    wxString servername;
	servername.Printf(wxT("MyNWNXServer %d"), pid);
	//servername.Printf(wxT("4242"));

	wxString hostname(wxT("localhost"));
    wxString topic(IPC_TOPIC);

	#if wxUSE_DDE_FOR_IPC
		wxLogTrace(TRACE_VERBOSE, wxT("IPC Server %s connecting (DDE)"), servername.c_str());
	#else 
		wxLogTrace(TRACE_VERBOSE, wxT("IPC Server %s connecting (TCP)"), servername.c_str());
	#endif

    bool retval = nwnx_client->Connect(hostname, servername, topic);

    wxLogTrace(TRACE_VERBOSE, _T("Client host=\"%s\" port=\"%s\" topic=\"%s\" %s"),
        hostname.c_str(), servername.c_str(), topic.c_str(),
        retval ? _T("connected") : _T("failed to connect"));

	return retval;
}
