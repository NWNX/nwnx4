/***************************************************************************
    NWNX Controller - Controls the server process
    Copyright (C) 2006 Ingmar Stieger (Papillon, papillon@nwnx.org)
	Copyright (C) 2008 Skywing

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

#if !defined(CONTROLLER_H_INCLUDED)
#define CONTROLLER_H_INCLUDED

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <dbghelp.h>
#include "detours.h"
#include "wx/fileconf.h"
#include "udp.h"
#include "../misc/log.h"
#include "../misc/shmem.h"

#define arrayof(x)		(sizeof(x)/sizeof(x[0]))

class NWNXController
{
public:
    NWNXController(wxFileConfig *config);
    ~NWNXController();

	inline ULONG getGracefulShutdownTimeout() const
	{
		return static_cast< ULONG >( gracefulShutdownTimeout );
	}

	void startServerProcess();
	void notifyServiceShutdown();
	void killServerProcess(bool graceful = true);
	void restartServerProcess();
	void shutdownServerProcess();
	void ping();

	wxString parameters;
	bool processWatchdog;
	bool gamespyWatchdog;
	int gamespyPort;
	int gamespyInterval;
	int gamespyRetries;
	int gamespyTolerance;
	int gracefulShutdownTimeout;
	int gracefulShutdownMessageWait;
	long restartDelay;
	long gamespyDelay;

private:

	typedef struct _FIND_SERVER_GUI_WINDOW_PARAM
	{
		HWND hwnd;
		ULONG processId;
	} FIND_SERVER_GUI_WINDOW_PARAM, * PFIND_SERVER_GUI_WINDOW_PARAM;

	wxFileConfig            *config;

	CUDP                    *udp;
	STARTUPINFO              si;
	PROCESS_INFORMATION      pi;
	HANDLE                   crashReportEvent;
	HANDLE                   crashAckEvent;
	PCRASH_DUMP_SECTION      crashDumpSection;

	unsigned long            tick;
	wxString                 nwnhome;
	wxString                 crashDumpDir;
	wxString                 gracefulShutdownMessage;
	bool                     initialized;
	bool                     shuttingDown;

	bool startServerProcessInternal();
	bool checkProcessActive();
	void runProcessWatchdog();
	void runGamespyWatchdog();
	void runCrashDumpWaiter(__in ULONG timeout);
	static BOOL CALLBACK findServerGuiWindowEnumProc(
		__in HWND hwnd,
		__in LPARAM lParam
		);
	static HWND findServerGuiWindow( __in ULONG processId );
	bool performGracefulShutdown( __in ULONG timeout );
	bool broadcastServerMessage( __in const TCHAR *message );

};

#endif
