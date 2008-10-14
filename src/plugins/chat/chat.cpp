/***************************************************************************
    NWNX Chat - NWNX4 port
    Copyright (C) 2006-2007 virusman (virusman@virusman.ru)
	Original Chat plugin was written by dumbo

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

#include "chat.h"
#include "hook.h"

/***************************************************************************
    NWNX and DLL specific functions
***************************************************************************/

Chat* plugin;

DLLEXPORT Plugin* GetPluginPointerV2()
{
	return plugin;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		plugin = new Chat();

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
    Implementation of Chat Plugin
***************************************************************************/

Chat::Chat()
{
	header = _T(
		"NWNX4 Chat Plugin V.0.3.6\n" \
		"(c) 2005-2006 by dumbo (dumbo@nm.ru)\n" \
		"(c) 2006-2007 by virusman (virusman@virusman.ru)\n" \
		"visit us at http://www.nwnx.org\n");

	description = _T(
		"This plugin provides chat event hook.");

	subClass = _T("CHAT");
	version = _T("0.3.6");
}

Chat::~Chat()
{

	wxLogMessage(wxT("* Plugin unloaded."));
}

bool Chat::Init(TCHAR* nwnxhome)
{
	assert(GetPluginFileName());

	/* Log file */
	wxString logfile(nwnxhome); 
	logfile.append(wxT("\\"));
	logfile.append(GetPluginFileName());
	logfile.append(wxT(".txt"));
	logger = new wxLogNWNX(logfile, wxString(header.c_str()));

	//strcpy(chatScript, "chat_script");
	//strcpy(servScript, "chat_script");
	//maxMsgLen = 1024;
	//processNPC = 1;

	wxString inifile(nwnxhome); 
	inifile.append(wxT("\\"));
	inifile.append(GetPluginFileName());
	inifile.append(wxT(".ini"));
	wxLogTrace(TRACE_VERBOSE, wxT("* reading inifile %s"), inifile);

	config = new wxFileConfig(wxEmptyString, wxEmptyString, 
		inifile, wxEmptyString, wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_NO_ESCAPE_CHARACTERS);

	if (!config->Read(wxT("chat_script"), &chatScript) )
	{
		wxLogMessage(wxT("* Chat script name not found in ini file"));
		chatScript = wxT("chat_script");
		wxLogMessage(wxT("* Using default chat script '%s'"), chatScript);
	}

	if (!config->Read(wxT("server_script"), &servScript) )
	{
		wxLogMessage(wxT("* Server script name not found in ini file"));
		servScript = wxT("chat_script");
		wxLogMessage(wxT("* Using default server script '%s'"), servScript);
	}
	config->Read(wxT("max_msg_len"), &maxMsgLen, 1024);
	config->Read(wxT("processnpc"), &processNPC, 0);
	config->Read(wxT("ignore_silent"), &ignore_silent, 0);

	config->Read(wxT("loglevel"), &logLevel);
	switch(logLevel)
	{
	case 0: wxLogMessage(wxT("* Log level set to 0 (nothing)")); break;
	case 1: wxLogMessage(wxT("* Log level set to 1 (only errors)")); break;
	case 2: wxLogMessage(wxT("* Log level set to 2 (everything)")); break;
	case 3: wxLogMessage(wxT("* Log level set to 3 (debug)")); break;
	}

	lastMsg = new char[maxMsgLen+13];

	//wxLogMessage(wxT("* Plugin initialized."));
	if(!HookFunctions())
	{
		wxLogMessage(wxT("* Hooking error."));
		return false;
	}

	wxLogMessage(wxT("* Plugin initialized."));
	return true;
}

void Chat::GetFunctionClass(TCHAR* fClass)
{
	_tcsncpy_s(fClass, 128, wxT("CHAT"), 4); 
}



void Chat::SetInt(char * sFunction, char * sParam1, int nParam2, int nValue)
{
	wxString function(sFunction); 
	if (function == wxT("LOGNPC"))
	{
		processNPC = nValue;
	}
	else if (function == wxT("IGNORESILENT"))
	{
		ignore_silent = nValue; 
	}
	else if (function == wxT("SUPRESS"))
	{
		if (!scriptRun) return; // only in chat script
		if (nValue == 1) 
			supressMsg = 1;
	}

}

void Chat::SetString(char * sFunction, char * sParam1, int nParam2, char * value)
{
	wxString function(sFunction);
	if (function == wxT("LOG")) {
		if (!scriptRun ) return; // only in chat script
		wxLogMessage(wxT("%s"), sParam1);
	
	}

}

int Chat::GetInt(char* sFunction, char* sParam1, int nParam2)
{
	wxString function = sFunction;
	if (function == wxT("GETID")) {
		wxLogDebug(wxT("%s - getting ID for %d"), nParam2);
		if (nParam2) {  
			return GetID(nParam2);
		}
		wxLogDebug(wxT("%s - returns -1"));
	} else  if (function == wxT("SPEAK")) { //makes someone say something
		if (logLevel >= 2)
			wxLogMessage(wxT("o SPEAK: %s"), sParam1);
		int oSender, oRecipient, nChannel;
		int nParamLen = strlen(sParam1);
		char *nLastDelimiter = strrchr(sParam1, '�');
		if (!nLastDelimiter || (nLastDelimiter-sParam1)<0)
		{
			if (logLevel >= 1)
				wxLogError("%s - nLastDelimiter error", sFunction);
			return FALSE;
		}
		int nMessageLen = nParamLen-(nLastDelimiter-sParam1)+1;
		char *sMessage = new char[nMessageLen];
		if(sscanf(sParam1, "%x¬%x¬%d¬", &oSender, &oRecipient, &nChannel)<3)
		{
			if (logLevel >= 1)
				wxLogMessage("o sscanf error");
			delete[] sMessage;
			return FALSE;
		}
		strncpy(sMessage, nLastDelimiter+1, nMessageLen-1);
		int nRecipientID = GetID(oRecipient);
		/*if((nChannel==4 || nChannel==20) && oRecipient <= 0x7F000000)
		{
			wxLogMessage("o oRecipient is not a PC");
			delete[] sMessage;
			return FALSE;
		}*/
		if(nChannel!=4 && nChannel!=20) nRecipientID=-1;
		if (logLevel >= 2)
			wxLogMessage("o SendMsg(%d, %08lX, '%s', %d)", nChannel, oSender, sMessage, nRecipientID);
		int nResult = SendMsg(nChannel, oSender, sMessage, nRecipientID);
		if (logLevel >= 3)
			wxLogMessage("o Return value: %d", nResult); //return value for full message delivery acknowledgement
		delete[] sMessage;
		if (nResult) 
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;

}
char* Chat::GetString(char* sFunction, char* sParam1, int nParam2)
{
	wxString function = sFunction;
	returnBuffer[0] = 0;
	if (function == wxT("TEXT") && scriptRun) {
		strncpy(returnBuffer, lastMsg, MAX_BUFFER);
		if (logLevel >= 2)
			wxLogMessage(wxT("TEXT: '%s'"), lastMsg);
	}
	return returnBuffer;

}


int Chat::ChatProc(const int mode, const int id, const char **msg, const int to)
{
	if ( !msg || !*msg ) return 0; // don't process null-string
	int cmode = mode & 0xFF;
	if (logLevel >= 2)
		wxLogMessage(wxT("o CHAT: mode=%lX, from_oID=%08lX, msg='%s', to_ID=%08lX"), cmode, id, *(char **)msg, to);
	//Log("o CHAT: mode=%lX, from_oID=%08lX, msg='%s', to_ID=%08lX\n", cmode, id, (char *)msg, to);
	sprintf(lastMsg, "%02d%10d", cmode, to);
	strncat(lastMsg, (char*)*msg, maxMsgLen);
	if (logLevel >= 3)
		wxLogMessage(wxT("lastMsg: %s"), lastMsg);
	supressMsg = 0;
	if(ignore_silent && (cmode==0xD || cmode==0xE)) return 0;
	if ( (processNPC && id != 0x7F000000) || (!processNPC && (unsigned long)id >> 16 == 0x7FFF) )
	{
		RunScript((char *)chatScript.c_str(), id);
	}
	else if (cmode==5 && id==0x7F000000) {
		RunScript((char *)servScript.c_str(), to);
	}
	return supressMsg;
}

