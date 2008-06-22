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
	long restartDelay;
	long gamespyDelay;

private:

	typedef LONG NTSTATUS;

#define NTAPI __stdcall

	typedef NTSTATUS (NTAPI * NtSuspendProcessProc)(__in HANDLE Process);

	typedef BOOL (WINAPI * MiniDumpWriteDumpProc)(
		IN HANDLE hProcess,
		IN DWORD ProcessId,
		IN HANDLE hFile,
		IN MINIDUMP_TYPE DumpType,
		IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, OPTIONAL
		IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, OPTIONAL
		IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL
		);

	typedef LPAPI_VERSION (__stdcall * ImagehlpApiVersionProc)( VOID );
	typedef LPAPI_VERSION (__stdcall * ImagehlpApiVersionExProc)( __in LPAPI_VERSION AppVersion );

	typedef struct _CRASH_DUMP_SECTION
	{
		HANDLE              CrashReportEvent;
		HANDLE              CrashAckEvent;
		EXCEPTION_POINTERS  ExceptionPointers;
		PEXCEPTION_POINTERS ClientExceptionPointers;
		ULONG               ThreadId;
	} CRASH_DUMP_SECTION, * PCRASH_DUMP_SECTION;

	typedef struct _FIND_SERVER_GUI_WINDOW_PARAM
	{
		HWND hwnd;
		ULONG processId;
	} FIND_SERVER_GUI_WINDOW_PARAM, * PFIND_SERVER_GUI_WINDOW_PARAM;

	NtSuspendProcessProc     NtSuspendProcess;
	MiniDumpWriteDumpProc    MiniDumpWriteDump;
	ImagehlpApiVersionProc   ImagehlpApiVersion;
	ImagehlpApiVersionExProc ImagehlpApiVersionEx;

	wxFileConfig            *config;

	CUDP                    *udp;
	STARTUPINFO              si;
	PROCESS_INFORMATION      pi;
	HANDLE                   crashReportEvent;
	HANDLE                   crashAckEvent;
	PCRASH_DUMP_SECTION      crashDumpSection;

	unsigned long            tick;
	int                      initTimeout;
	wxString                 nwnhome;
	wxString                 crashDumpDir;
	wxString                 gracefulShutdownMessage;
	bool                     initialized;
	bool                     shuttingDown;
	MINIDUMP_TYPE            dumpType;
	HMODULE                  dbgHelp;

	bool startServerProcessInternal();
	bool checkProcessActive();
	void runProcessWatchdog();
	void runGamespyWatchdog();
	void runCrashDumpWaiter(__in ULONG timeout);
	bool waitForNWN2ServerMessageLoop(unsigned long pid);
	static BOOL CALLBACK findServerGuiWindowEnumProc(
		__in HWND hwnd,
		__in LPARAM lParam
		);
	static HWND findServerGuiWindow( __in ULONG processId );
	bool performGracefulShutdown( __in ULONG timeout );
	bool broadcastServerMessage( __in const TCHAR *message );

	bool checkDbgHelpVersion();
	bool connectCrashDumpServer(__in ULONG pid, __in HANDLE process);
	void cleanupCrashDumpServer();
	bool writeCrashDump(
		__in HANDLE Process,
		__in ULONG ProcessId,
		__in ULONG ThreadId,
		__in PEXCEPTION_POINTERS ExceptionPointers,
		__in BOOL ClientPointers,
		__in CONST WCHAR *Comment,
		__out std::wstring &DumpFileName
		);
	bool writeNwn2serverHangDump();

};

#endif
