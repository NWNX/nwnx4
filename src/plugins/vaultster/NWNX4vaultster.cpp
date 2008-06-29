/***************************************************************************
    NWNX4 Vaultster - Vaultster functions plugin
    Copyright (C) 2004 Jeroen Broekhuizen (nwnx@jengine.nl)
	2008 Modified by Patrice Torguet (torguet@gmail.com) for NWNX4

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

#include "NWNX4vaultster.h"

/***************************************************************************
    NWNX and DLL specific functions
***************************************************************************/

NWNX4Vaultster* plugin;

DLLEXPORT Plugin* GetPluginPointerV2()
{
	return plugin;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		plugin = new NWNX4Vaultster();

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

NWNX4Vaultster::NWNX4Vaultster()
{
	header = _T(
		"NWNX4 Vaultster - Vaultster functions plugin V 0.0.1\n" \
		"(c) 2004 Jeroen Broekhuizen (nwnx@jengine.nl)\n" \
		"2008 Modified by Patrice Torguet (torguet@gmail.com) for NWNX4\n" \
		"visit us at http://www.nwnx.org\n");

	description = _T(
		"This plugin provides portalling functionnalities.");

	subClass = _T("VAULTSTER");
	version = _T("0.0.1");

	//QueryPerformanceFrequency(&liFrequency);
}

NWNX4Vaultster::~NWNX4Vaultster()
{
	wxLogMessage(wxT("* Plugin unloaded."));
}

bool NWNX4Vaultster::SetupLogAndIniFile(TCHAR* nwnxhome)
{
	assert(GetPluginFileName());

	// Log file
	wxString logfile(nwnxhome); 
	logfile.append(wxT("\\"));
	logfile.append(GetPluginFileName());
	logfile.append(wxT(".txt"));
	logger = new wxLogNWNX(logfile, wxString(header.c_str()));

	// Ini file
	wxString inifile(nwnxhome); 
	inifile.append(wxT("\\"));
	inifile.append(GetPluginFileName());
	inifile.append(wxT(".ini"));
	wxLogTrace(TRACE_VERBOSE, wxT("* reading inifile %s"), inifile);

	config = new wxFileConfig(wxEmptyString, wxEmptyString, 
		inifile, wxEmptyString, wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
	
	config->Read(wxT("loglevel"), &logLevel);
	switch(logLevel)
	{
		case 0: wxLogMessage(wxT("* Log level set to 0 (nothing)")); break;
		case 1: wxLogMessage(wxT("* Log level set to 1 (only errors)")); break;
		case 2: wxLogMessage(wxT("* Log level set to 2 (everything)")); break;
	}
	return true;
}

bool NWNX4Vaultster::Init(TCHAR* nwnxhome)
{
	wxString buf;
	wxString key;
	bool startServer;
	bool validateclient;
	int port;
	DWORD serverId;
	int lowprior;
	int serverCount;

	SetupLogAndIniFile(nwnxhome);

	// start up WSA (winsock library)
	if (!CSock::StartWSA ()) {
		return false;
	}

	// load in the settings
	if (config->Read(wxT("startserver"), &startServer) )
	{
		wxLogMessage(wxT("* read startserver: %d"), startServer);
	}
	else
	{
		wxLogMessage(wxT("* startserver not found in ini file"));
		startServer = 1;
		wxLogMessage(wxT("* Defaulting to 1"));
	}
	if (config->Read(wxT("validateclient"), &validateclient) )
	{
		wxLogMessage(wxT("* read validateclient: %d"), validateclient);
	}
	else
	{
		wxLogMessage(wxT("* validateclient not found in ini file"));
		validateclient = 1;
		wxLogMessage(wxT("* Defaulting to 1"));
	}
	server.setValidate (validateclient);
	if (config->Read(wxT("MaxClients"), &maxClients) )
	{
		wxLogMessage(wxT("* read MaxClients: %d"), maxClients);
	}
	else
	{
		wxLogMessage(wxT("* MaxClients not found in ini file"));
		maxClients = 10;
		wxLogMessage(wxT("* Defaulting to 10"));
	}
	server.setMaxClients (maxClients);
	if (config->Read(wxT("Path"), &buf) )
	{
		wxLogMessage(wxT("* read Path: %s"), buf);
	}
	else
	{
		wxLogMessage(wxT("* Path not found in ini file"));
		buf = wxT(".");
		wxLogMessage(wxT("* Defaulting to ."));
	}
	// remove trailing backspace
	if (buf.EndsWith(wxT("\\")))
		buf = buf.RemoveLast(1);
	wxLogMessage(wxT("* Path is now: %s"), buf);

	CClient::setServervault((char*)buf.c_str());
	server.setServervault((char*)buf.c_str());

	if (config->Read(wxT("Key"), &key) )
	{
		wxLogMessage(wxT("* read Key: %s"), key);
	}
	else
	{
		wxLogMessage(wxT("* Key not found in ini file"));
		key = wxT("*invalid");
		wxLogMessage(wxT("* Defaulting to *invalid"));
	}
	CClient::setCryptoKey ((char*)key.c_str());
	server.setCryptoKey ((char*)key.c_str());

	if (config->Read(wxT("Password"), &buf) )
	{
		wxLogMessage(wxT("* read Password"));
	}
	else
	{
		wxLogMessage(wxT("* Password not found in ini file"));
		buf = wxT("PWD!@");
		wxLogMessage(wxT("* Defaulting to PWD!@"));
	}
	CClient::setPassword ((char*)buf.c_str());
	server.setPassword ((char*)buf.c_str());

	if (config->Read(wxT("Port"), &port) )
	{
		wxLogMessage(wxT("* read Port %d"), port);
	}
	else
	{
		wxLogMessage(wxT("* Port not found in ini file"));
		port = 5100;
		wxLogMessage(wxT("* Defaulting to 5100"));
	}
	CClient::setPort (port);
	server.setPort (port);

	if (config->Read(wxT("Low prior"), &lowprior) )
	{
		wxLogMessage(wxT("* read Low prior %d"), lowprior);
	}
	else
	{
		wxLogMessage(wxT("* Low prior not found in ini file"));
		lowprior = 0;
		wxLogMessage(wxT("* Defaulting to 0"));
	}

	// load in the known servers
	if (validateclient) {
		if (!config->Read(wxT("Count"), &serverCount) )
		{
			serverCount = 0;
		}
		wxLogMessage(wxT("o Loading in %d known servers.\n"), serverCount);
		for (int i = 0; i < serverCount; i++) {
			key.Format("Server%d", i+1);
			if (config->Read(key, &buf) )
			{
				if (!buf.empty()) server.addKnownServer((char*)buf.c_str());
				wxLogMessage(wxT("o Loaded: %s\n"), buf);
			}
		}
	}

	// initialize the clients
	clients = new CClient[maxClients];

	// start up the server
	if (startServer) {
		hServer = CreateThread (NULL, 0, CServer::thread, &server, 0, &serverId);
		if (hServer == NULL) {
			// Failing starting up the server should not end
			// VaultSTER, the client part can still run.
			wxLogMessage(wxT("o Failed to start up the server.\n"));
			return TRUE;
		}
		else {
			wxLogMessage(wxT("o Server started on port %d.\n"), port);
			// set the server thread priority
			if (lowprior == 1)
				SetThreadPriority (hServer, THREAD_PRIORITY_BELOW_NORMAL);
			else if (lowprior == 2)
				SetThreadPriority (hServer, THREAD_PRIORITY_LOWEST);
		}
	}
	wxLogMessage(wxT("* Plugin initialized."));

	return true;
}

void NWNX4Vaultster::GetFunctionClass(TCHAR* fClass)
{
	_tcsncpy_s(fClass, 128, wxT("VAULTSTER"), 9); 
}


int NWNX4Vaultster::GetInt(char* sFunction, char* sParam1, int nParam2)
{
	wxLogTrace(TRACE_VERBOSE, wxT("* Plugin GetInt(0x%x, %s, %d)"), 0x0, sParam1, nParam2);
	int returnInt = 0;

#ifdef UNICODE
	wxString wxRequest(sFunction, wxConvUTF8);
	wxString function(sFunction, wxConvUTF8);
	wxString parameters(sParam1, wxConvUTF8);
#else
	wxString wxRequest(sFunction);
	wxString function(sFunction);
	wxString parameters(sParam1);
#endif

	if (function == wxT(""))
	{
		wxLogMessage(wxT("* Function not specified."));
		return NULL;
	}
	else if ((function == wxT("GET")) || (function == wxT("SEND")))
	{
		char* pos[2];
		DWORD id;
		int i;

		// find a not busy client spot in the array
		for (i = 0; i < maxClients; i++) {
			if (clients[i].getStatus() == STATUS_OK)
				break;
		}

		if (i == maxClients) {
			wxLogMessage(wxT("o Too many clients already.\n"));
			// can not help this client yet
			return -2;
		}
		else
			clients[i].setStatus (STATUS_BUSY);

		pos[0] = (char*)strchr (parameters.c_str(), '|');
		pos[1] = strchr (&pos[0][1], '|');
		if (!pos[0] || !pos[1]) {
			wxLogMessage(wxT("o Invalid parameter (%s)!\n"), parameters);
			return -4;
		}

		// set up the client for running
		memset (clients[i].server, 0, 128);
		memset (clients[i].gamespy, 0, 128);
		memset (clients[i].character, 0, 32);
		strncpy_s (clients[i].server, 128, parameters, pos[0] - parameters);
		strncpy_s (clients[i].gamespy, 128, &pos[0][1], pos[1] - pos[0] - 1);
		strcpy_s  (clients[i].character, 32, &pos[1][1]);

		Functions cmd = (_stricmp((char*)function.c_str(),"GET") == 0 ? Get : (_stricmp((char*)function.c_str(),"SEND") == 0 ? Put : Status));
		clients[i].setCommand (cmd);

		// start up the client thread
		clients[i].hThread = CreateThread (NULL, 0, CClient::thread, &clients[i], 0, &id);
		if (clients[i].hThread == NULL) {
			wxLogMessage(wxT("o Failed to start client thread!\n"));
			returnInt = -1;
		}
		else {
			returnInt = i;
		}
		return returnInt;
	} else {
		// return status about given player
		int job = nParam2;
		int status = clients[job].getStatus ();
		wxLogMessage(wxT("o Retreived status for %s\\%s = %d\n"), clients[job].gamespy, clients[job].character, status);
		if (status == STATUS_ERROR)
			clients[job].setStatus (STATUS_OK);
		return status;
	}
	return 0;
}

void NWNX4Vaultster::Log(const char *pcMsg, ...)
{
	va_list argList;
	char acBuffer[1024];

	// build up the string
	va_start(argList, pcMsg);
	vsprintf_s(acBuffer, 1024, pcMsg, argList);
	va_end(argList);

	// log string in file
	wxLogMessage(wxT(acBuffer));
}
