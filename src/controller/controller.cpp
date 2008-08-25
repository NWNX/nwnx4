/***************************************************************************
    NWNX Controller - Controls the server process
    Copyright (C) 2006 Ingmar Stieger (Papillon, papillon@nwnx.org)
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

#include "stdwx.h"
#include "controller.h"

#include <dbghelp.h>
#include <strsafe.h>

#ifndef STATUS_POSSIBLE_DEADLOCK
#define STATUS_POSSIBLE_DEADLOCK         ((NTSTATUS)0xC0000194L)
#endif

#define IDC_SENDMESSAGE_EDIT    0x3FC 
#define IDC_SENDMESSAGE_BUTTON  0x400 

NWNXController::NWNXController(wxFileConfig *config)
: initTimeout( 30000 ),
  gracefulShutdownTimeout( 10000 ),
  gracefulShutdownMessageWait( 5 ),
  NtSuspendProcess( NULL ),
  ImagehlpApiVersion( NULL ),
  ImagehlpApiVersionEx( NULL ),
  MiniDumpWriteDump( NULL ),
  udp( NULL ),
  crashReportEvent( NULL ),
  crashAckEvent( NULL ),
  crashDumpSection( NULL ),
  tick( 0 ),
  shuttingDown( false ),
  initialized( false ),
  dbgHelp( NULL )
{
	this->config = config;

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
	config->Read(wxT("initTimeout"), &initTimeout);
	config->Read(wxT("gracefulShutdownTimeout"), &gracefulShutdownTimeout);
	config->Read(wxT("gracefulShutdownMessageWait"), &gracefulShutdownMessageWait);

	if (!config->Read(wxT("parameters"), &parameters) )
	{
		wxLogMessage(wxT("Parameter setting not found in nwnx.ini. Starting server with empty commandline."));
	}

	config->Read(wxT("gracefulShutdownMessage"), &gracefulShutdownMessage);

	parameters.Prepend(wxT(" "));

	if (gamespyWatchdog)
	{
		config->Read(wxT("gamespyPort"), &gamespyPort);

		try
		{
			udp = new CUDP("localhost", gamespyPort);
		}
		catch (std::bad_alloc)
		{
			udp = NULL;
		}
	}

	if (!config->Read(wxT("nwn2"), &nwnhome))
	{
		wxLogMessage(wxT("* NWN2 home directory not found. Check your nwnx.ini file."));
		return;
	}

	if (!config->Read(wxT("crashDumpDir"), &crashDumpDir))
		crashDumpDir = wxT("");

	HMODULE hmod;

	hmod = GetModuleHandleW( L"ntdll.dll" );

	if (hmod)
	{
		NtSuspendProcess = (NtSuspendProcessProc)GetProcAddress(
			hmod,
			"NtSuspendProcess"
			);
	}

	if (crashDumpDir != wxT(""))
	{
		dbgHelp = LoadLibraryW( L"DbgHelp.dll" );

		if (dbgHelp)
		{
			ImagehlpApiVersion   = (ImagehlpApiVersionProc)GetProcAddress(
				dbgHelp,
				"ImagehlpApiVersion"
				);

			ImagehlpApiVersionEx = (ImagehlpApiVersionExProc)GetProcAddress(
				dbgHelp,
				"ImagehlpApiVersionEx"
				);

			MiniDumpWriteDump    = (MiniDumpWriteDumpProc)GetProcAddress(
				dbgHelp,
				"MiniDumpWriteDump"
				);
		}
	}

	//
	// Determine our DbgHelp.dll version, which indicates to us which
	// features we can request when writing crash dumps.  Additionally, we
	// can warn the user if we are using a degraded feature set because
	// the local system has an old version of DbgHelp.dll.
	//

	checkDbgHelpVersion();
}

NWNXController::~NWNXController()
{
	killServerProcess();

	if (dbgHelp)
		FreeLibrary( dbgHelp );

	if (udp)
		delete udp;
}


/***************************************************************************
    NWNServer related functions
***************************************************************************/

void NWNXController::startServerProcess()
{
	killServerProcess( false );

	while (!shuttingDown && !startServerProcessInternal())
	{
		killServerProcess( false );

		wxLogMessage( wxT( "! Error: Failed to start server process, retrying in 5000ms..." ) );

		Sleep( 5000 );
	}
}

void NWNXController::notifyServiceShutdown()
{
	shuttingDown = true;
}

bool NWNXController::startServerProcessInternal()
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
		return false;
	}

	SECURITY_ATTRIBUTES SecurityAttributes;
	SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	SecurityAttributes.bInheritHandle = TRUE;
	SecurityAttributes.lpSecurityDescriptor = 0;

	shmem.ready_event = CreateEvent(&SecurityAttributes, TRUE, FALSE, 0);
	if(!shmem.ready_event)
	{ 
		wxLogMessage(wxT("CreateEvent failed (%d)"), GetLastError());
		return false;
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
		CloseHandle( shmem.ready_event );
		ZeroMemory( &pi, sizeof( PROCESS_INFORMATION ) );
		return false;
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

	//
	// Start the main thread running and wait for it to signal that it has read
	// configuration data and started up it's end of any IPC mechanisms that we
	// might rely on.
	//

	ResumeThread(pi.hThread);

	switch(WaitForSingleObject(shmem.ready_event, 60000))
	{
		case WAIT_TIMEOUT:
			wxLogMessage(wxT("! Error: Server did not initialize properly (timeout).\n"));
			CloseHandle( shmem.ready_event );
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
			ZeroMemory( &pi, sizeof( PROCESS_INFORMATION ) );
			return false;
			break;
		case WAIT_FAILED:
			wxLogMessage(wxT("! Error: Server did not initialize properly (wait failed).\n"));
			CloseHandle( shmem.ready_event );
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
			ZeroMemory( &pi, sizeof( PROCESS_INFORMATION ) );
			return false;
			break;
		case WAIT_OBJECT_0:
			CloseHandle( shmem.ready_event );
			wxLogMessage(wxT("Success: Server initialized properly.\n"));
			break;
	}

	//
	// Connect the crash dump server.
	//

	if (!connectCrashDumpServer(
		pi.dwProcessId,
		pi.hProcess))
	{
		wxLogMessage( wxT( "* WARNING: Crash dump support will not be enabled." ) );

		//
		// Not fatal, keep on going.
		//
	}

	//
	// Wait for the server message loop to start.
	//

	if (!waitForNWN2ServerMessageLoop( pi.dwProcessId ))
	{
		wxLogMessage( wxT("! Error: Server did not initialize message loop within timeout period." ) );
		killServerProcess( false );
		return false;
	}

	//
	// Reset the count of ping probes to zero for purposes of initial load time
	// GameSpy ping allowances.
	//

	tick = 0;

	//
	// Reset GameSpy failed response count.
	//

	gamespyRetries = 0;

	wxLogMessage(wxT("* Hook installed and initialized successfully"));
	initialized = true;

	return true;
}

void NWNXController::restartServerProcess()
{
	// Run maintenance command
	wxString restartCmd;
	config->Read(wxT("restartCmd"), &restartCmd);

	if (restartCmd != wxT(""))
	{
		PROCESS_INFORMATION pi;
		STARTUPINFO si;

		//restartCmd.Prepend(wxT("/c "));
		ZeroMemory(&si,sizeof(si));
		si.cb = sizeof(si);
		wxLogMessage(wxT("* Starting maintenance file %s"), restartCmd);
		restartCmd.Prepend(wxT("cmd.exe /c \""));
		restartCmd.Append( wxT( "\"" ) );

		if (CreateProcess(NULL, (LPTSTR)restartCmd.c_str(), NULL, NULL,FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
		{
			WaitForSingleObject( pi.hProcess, INFINITE );
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
		}
	}

	// Finally restart the server
	wxLogMessage(wxT("* Waiting %d seconds before restarting the server."), restartDelay);
	Sleep(restartDelay * 1000);
	wxLogMessage(wxT("* Restarting server."));

	startServerProcess();
}

void NWNXController::killServerProcess(bool graceful /* = true */)
{
	if (pi.hProcess)
	{
		//
		// If we are doing a graceful shutdown, then let's try to poke the
		// server GUI closed so that players are cleanly saved and logged out of
		// the server.
		//

		if (graceful)
		{
			if (!performGracefulShutdown( getGracefulShutdownTimeout() ))
			{
				wxLogMessage(
					wxT( "* WARNING: Failed to gracefully shutdown the server process." )
					);
			}
		}

		//
		// Mark us as not initialized.
		//

		shutdownServerProcess();

		TerminateProcess( pi.hProcess, -1 );

		CloseHandle( pi.hProcess );

		if (pi.hThread)
			CloseHandle( pi.hThread );

		ZeroMemory( &pi, sizeof( PROCESS_INFORMATION ) );
	}

	//
	// Clean up the crash dump server.
	//

	cleanupCrashDumpServer();
}

void NWNXController::shutdownServerProcess()
{
	initialized = false;
}

bool NWNXController::waitForNWN2ServerMessageLoop(unsigned long pid)
{
	HWND hwnd;
	DWORD_PTR res;
	wxString className, windowName;

	/*
	 * Locate the notification window.
	 */

	className.Printf(wxT("NWNXServerClass %lu"), pid);
	windowName.Printf(wxT("NWNXServerWindow %lu"), pid);

	hwnd = FindWindow( className.c_str(), windowName.c_str() );

	if (!hwnd)
	{
		wxLogMessage(wxT( "* Server window %s class %s does not exist?" ), windowName.c_str(), className.c_str());
		return false;
	}

	/*
	 * Send a WM_NULL to it and wait for a reply to come back.  This allows us
	 * to block until the message loop is up and running with a configurable
	 * timeout.  We need to ensure that the message loop is really functional
	 * or we may fail to connect to DDE.
	 */

	if (!SendMessageTimeout(
		hwnd,
		WM_NULL,
		0,
		0,
		SMTO_NORMAL,
		static_cast< ULONG >( initTimeout ),
		&res))
	{
		wxLogMessage(wxT( "* Couldn't receive response from server confirming message loop running (%lu)." ), GetLastError() );
		return false;
	}

	wxLogMessage(wxT( "* Server successfully started message loop (%lu)." ), pi.dwThreadId );

	return true;
}

BOOL
CALLBACK
NWNXController::findServerGuiWindowEnumProc(
	__in HWND hwnd,
	__in LPARAM lParam
	)
{
	DWORD                         pid;
	WCHAR                         className[ 256 ];
	PFIND_SERVER_GUI_WINDOW_PARAM param;

	param = reinterpret_cast< PFIND_SERVER_GUI_WINDOW_PARAM >( lParam );

	//
	// Ignore windows that do not match the right nwn2server process id.
	//

	GetWindowThreadProcessId( hwnd, &pid );

	if (pid != param->processId)
		return TRUE;

	//
	// Ignore windows that are not of the class of the main server window GUI.
	//

	if (GetClassNameW( hwnd, className, 256 ))
	{
		if (!wcscmp( className, L"Exo - BioWare Corp., (c) 1999 - Generic Blank Application"))
		{
			param->hwnd = hwnd;
			return FALSE;
		}
	}

	return TRUE;
}

HWND
NWNXController::findServerGuiWindow(
	__in ULONG processId
	)
{
	FIND_SERVER_GUI_WINDOW_PARAM param;

	param.hwnd      = 0;
	param.processId = processId;

	EnumWindows( findServerGuiWindowEnumProc, (LPARAM)&param );

	return param.hwnd;
}

bool NWNXController::performGracefulShutdown(
	__in ULONG timeout
	)
{
	HWND serverGuiWindow;

	//
	// Can't perform a graceful shutdown without a process ID.
	//

	if (!pi.dwProcessId || !pi.hProcess)
		return false;

	//
	// Try and locate the server's administrative GUI window.
	//
	
	serverGuiWindow = findServerGuiWindow( pi.dwProcessId );

	if (!serverGuiWindow)
		return false;

	//
	// If we have a graceful shutdown message then transmit it before we
	// initiate shutdown.
	//

	if (gracefulShutdownMessage != wxT( "" ))
	{
		wxLogMessage(wxT( "* Sending shutdown server message and waiting %d seconds."), gracefulShutdownMessageWait);
		broadcastServerMessage(gracefulShutdownMessage.c_str());
		Sleep(gracefulShutdownMessageWait * 1000);
	}

	//
	// Post a WM_CLOSE to the admin interface window, which initiates a clean
	// server shutdown without the blocking confirmation message box (if there
	// were any players present).
	//

	if (!PostMessage( serverGuiWindow, WM_CLOSE, 0, 0 ))
		return false;

	//
	// Wait for the server process to exit in the timeout interval.
	//

	return (WaitForSingleObject( pi.hProcess, timeout ) == WAIT_OBJECT_0);
}

bool NWNXController::broadcastServerMessage( __in const TCHAR *message )
{
	HWND      srvWnd;
	HWND      sendMsgEdit;
	HWND      sendMsgButton;
	DWORD_PTR result;

	if (!pi.dwProcessId)
		return false;

	srvWnd = findServerGuiWindow( pi.dwProcessId );

	if (!srvWnd)
		return false;

	sendMsgEdit   = GetDlgItem( srvWnd, IDC_SENDMESSAGE_EDIT );
	sendMsgButton = GetDlgItem( srvWnd, IDC_SENDMESSAGE_BUTTON );

	if (!sendMsgEdit)
		return false;

	if (!sendMsgButton)
		return false;

	SendMessageTimeout(
		sendMsgEdit,
		WM_SETTEXT,
		0,
		reinterpret_cast< LPARAM >( message ),
		SMTO_NORMAL,
		1000,
		&result
		);
	SendMessageTimeout(
		sendMsgButton,
		BM_CLICK,
		0,
		0,
		SMTO_NORMAL,
		1000,
		&result
		);
	SendMessageTimeoutA(
		sendMsgEdit,
		WM_SETTEXT,
		0,
		reinterpret_cast< LPARAM >( "" ),
		SMTO_NORMAL,
		1000,
		&result
		);

	return true;
}

/***************************************************************************
    Watchdog related functions
***************************************************************************/

void NWNXController::ping()
{
	if (initialized)
	{
		if (crashDumpSection)
			runCrashDumpWaiter( 0 );
		if (processWatchdog)
			runProcessWatchdog();
		if (gamespyWatchdog && (tick % gamespyInterval == 0) && (tick > (unsigned long) gamespyDelay))
			runGamespyWatchdog();
		tick++;
	}
}

bool NWNXController::checkProcessActive()
{
	if (!pi.hProcess)
		return false;

	return (WaitForSingleObject( pi.hProcess, 0 ) == WAIT_TIMEOUT);
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
		writeNwn2serverHangDump();
		restartServerProcess();
	}
}

void NWNXController::runCrashDumpWaiter(ULONG timeout)
{
	//
	// If we've not got a crash dump section, then we're done here and now.
	//

	if (!crashDumpSection)
		return;

	//
	// Wait for a timeout or crash report event.
	//

	if (WaitForSingleObject( crashReportEvent, timeout ) != WAIT_OBJECT_0)
		return;

	//
	// Okay, we've got a crash.  Let's perform our analysis and then kill the
	// process.
	//

	wxLogMessage(
		wxT( "* Server process reported a crash on thread %lu." ),
		crashDumpSection->ThreadId
		);

	//
	// First, write some basic analysis to the log file.
	//

	for (;;)
	{
		EXCEPTION_POINTERS ExceptionPointers;
		EXCEPTION_RECORD   ExceptionRecord;
		CONTEXT            ContextRecord;
		std::wstring       DumpFileName;
		SIZE_T             Transferred;
		ULONG_PTR          ExceptionParameters[ 2 ];

		if (!ReadProcessMemory(
			pi.hProcess,
			crashDumpSection->ExceptionPointers.ExceptionRecord,
			&ExceptionRecord,
			sizeof( EXCEPTION_RECORD ),
			&Transferred) || (Transferred != sizeof( EXCEPTION_RECORD )))
			break;

		if (ExceptionRecord.NumberParameters > 0 )
			ExceptionParameters[ 0 ] = ExceptionRecord.ExceptionInformation[ 0 ];
		else
			ExceptionParameters[ 0 ] = 0;

		if (ExceptionRecord.NumberParameters > 1 )
			ExceptionParameters[ 1 ] = ExceptionRecord.ExceptionInformation[ 1 ];
		else
			ExceptionParameters[ 1 ] = 0;

		wxLogMessage(
			wxT( "* Exception %08X occured at address %p (%p, %p) on thread %lu." ),
			ExceptionRecord.ExceptionCode,
			ExceptionRecord.ExceptionAddress,
			ExceptionParameters[ 0 ],
			ExceptionParameters[ 1 ],
			crashDumpSection->ThreadId
			);

		//
		// Next, we'll write the minidump out.
		//

		if (!ReadProcessMemory(
			pi.hProcess,
			crashDumpSection->ExceptionPointers.ContextRecord,
			&ContextRecord,
			sizeof( CONTEXT ),
			&Transferred) || (Transferred != sizeof( CONTEXT )))
			break;

		ExceptionPointers.ExceptionRecord = &ExceptionRecord;
		ExceptionPointers.ContextRecord   = &ContextRecord;

		writeCrashDump(
			pi.hProcess,
			pi.dwProcessId,
			crashDumpSection->ThreadId,
			&ExceptionPointers,
			FALSE,
			L"crash",
			DumpFileName
			);

		break;
	}

	//
	// Kill the process.
	//

	restartServerProcess();
}

/***************************************************************************
    Crash dump support related functions
***************************************************************************/

bool NWNXController::checkDbgHelpVersion()
{
	bool          usingOldVersion = true;
	LPAPI_VERSION imagehlpApiVersion;
	API_VERSION   appDbgHelpVersion;

	appDbgHelpVersion.MajorVersion = 6;
	appDbgHelpVersion.MinorVersion = 2;
	appDbgHelpVersion.Revision     = 0;
	appDbgHelpVersion.Reserved     = 0;

	unsigned long dumpTypeMask =
		MiniDumpWithHandleData                 |
		MiniDumpWithDataSegs                   |
		MiniDumpWithFullMemory                 |
		MiniDumpWithPrivateReadWriteMemory     |
		MiniDumpWithIndirectlyReferencedMemory |
		MiniDumpWithUnloadedModules            |
		MiniDumpWithProcessThreadData          |
		MiniDumpWithFullMemoryInfo             |
		MiniDumpWithThreadInfo;

	if (ImagehlpApiVersionEx)
		imagehlpApiVersion = ImagehlpApiVersionEx( &appDbgHelpVersion );
	else if (ImagehlpApiVersion)
		imagehlpApiVersion = ImagehlpApiVersion();
	else
		imagehlpApiVersion = 0;

	if (!imagehlpApiVersion)
	{
		dumpType = static_cast< MINIDUMP_TYPE >( dumpTypeMask );
		return !usingOldVersion;
	}

	if ((imagehlpApiVersion->MajorVersion < 6) ||
	    ((imagehlpApiVersion->MajorVersion == 6) &&
	     (imagehlpApiVersion->MinorVersion <= 1)))
	{
		wxLogMessage(
			wxT( "* WARNING: Using DbgHelp version %lu.%lu.  A DbgHelp version above 6.1 is required for full crash dump support." ),
			imagehlpApiVersion->MajorVersion,
			imagehlpApiVersion->MinorVersion
			);

		wxLogMessage(
			wxT( "* To install the latest DbgHelp.dll, visit: http://go.microsoft.com/fwlink/?linkid=8708, download the 32-bit Debugging Tools for Windows distribution, and copy DbgHelp.dll from the Debugging Tools for Windows installation directory to the directory containing the NWNX4 installation (e.g. the directory with NWNX4_Controller.exe or NWNX4_GUI.exe)." )
			);

		if ((imagehlpApiVersion->MajorVersion < 6) ||
			((imagehlpApiVersion->MajorVersion == 6) &&
			 (imagehlpApiVersion->MinorVersion <= 1)))
		{
			dumpTypeMask &= ~(MiniDumpWithFullMemoryInfo |
			                  MiniDumpWithThreadInfo);
		}

		if ((imagehlpApiVersion->MajorVersion < 5) ||
			((imagehlpApiVersion->MajorVersion == 5) &&
			 (imagehlpApiVersion->MinorVersion <= 1)))
		{
			dumpTypeMask &= ~(MiniDumpWithIndirectlyReferencedMemory |
			                  MiniDumpWithUnloadedModules            |
			                  MiniDumpWithPrivateReadWriteMemory);
		}
	}

	dumpType = static_cast< MINIDUMP_TYPE >( dumpTypeMask );

	return !usingOldVersion;
}

bool NWNXController::connectCrashDumpServer(
	__in ULONG pid,
	__in HANDLE process
	)
{
	HANDLE              Section;
	WCHAR               SectionName[ 128 ];
	PCRASH_DUMP_SECTION View;
	HANDLE              Events[ 2 ];
	bool                Success;

	ZeroMemory( Events, sizeof( Events ) );
	View    = 0;
	Success = false;

	for (;;)
	{
		StringCbPrintfW(
			SectionName,
			sizeof( SectionName ),
			L"NWN2ServerCrashDumpSection-%lu",
			pid
			);

		Section = OpenFileMappingW(
			FILE_MAP_WRITE,
			FALSE,
			SectionName
			);

		if (!Section)
		{
#ifdef UNICODE
			wxLogMessage(
				wxT( "* WARNING: connectCrashDumpServer(): Failed to open section '%s', %lu." ),
				SectionName,
				GetLastError()
				);
#else
			wxLogMessage(
				wxT( "* WARNING: connectCrashDumpServer(): Failed to open section '%S', %lu." ),
				SectionName,
				GetLastError()
				);
#endif
			break;
		}

		View = (PCRASH_DUMP_SECTION)MapViewOfFile(
			Section,
			SECTION_MAP_WRITE,
			0,
			0,
			0
			);

		if (!View)
		{
			wxLogMessage(
				wxT( "* WARNING: connectCrashDumpServer(): Failed to map view of section, %lu." ),
				GetLastError()
				);

			break;
		}

		Events[ 0 ] = CreateEvent( 0, TRUE, FALSE, 0 );
		Events[ 1 ] = CreateEvent( 0, TRUE, FALSE, 0 );

		if (!Events[ 0 ] || !Events[ 1 ])
		{
			wxLogMessage(
				wxT( "* WARNING: connectCrashDumpServer(): Failed to create event." )
				);

			break;
		}

		if (!DuplicateHandle(
			GetCurrentProcess(),
			Events[ 0 ],
			process,
			&View->CrashReportEvent,
			0,
			FALSE,
			DUPLICATE_SAME_ACCESS))
		{
			wxLogMessage(
				wxT( "* WARNING: connectCrashDumpServer(): Failed to duplicate handle, %lu." ),
				GetLastError()
				);

			break;
		}

		if (!DuplicateHandle(
			GetCurrentProcess(),
			Events[ 1 ],
			process,
			&View->CrashAckEvent,
			0,
			FALSE,
			DUPLICATE_SAME_ACCESS))
		{
			wxLogMessage(
				wxT( "* WARNING: connectCrashDumpServer(): Failed to duplicate handle, %lu." ),
				GetLastError()
				);

			break;
		}

		//
		// Okay, we're all set now.  Let's save the information we need for our
		// crash writing away.
		//

		crashReportEvent = Events[ 0 ];
		crashAckEvent    = Events[ 1 ];
		Events[ 0 ]      = 0;
		Events[ 1 ]      = 0;
		crashDumpSection = View;
		Success          = true;

		break;
	}

	//
	// Perform any cleanup necessary.
	//

	if (Events[ 0 ])
		CloseHandle( Events[ 0 ] );
	if (Events[ 1 ])
		CloseHandle( Events[ 1 ] );

	if (!Success && View)
		UnmapViewOfFile( View );

	if (Section)
		CloseHandle( Section );

	return Success;
}

void NWNXController::cleanupCrashDumpServer()
{
	if (crashReportEvent)
	{
		CloseHandle( crashReportEvent );
		crashReportEvent = 0;
	}

	if (crashAckEvent)
	{
		CloseHandle( crashAckEvent );
		crashAckEvent = 0;
	}

	if (crashDumpSection)
	{
		UnmapViewOfFile( crashDumpSection );
		crashDumpSection = 0;
	}
}

#define __TOUNICODE2_(x) L##x
#define __TOUNICODE_(x) __TOUNICODE2_(x)
#define UNICODE_DATE __TOUNICODE_(__DATE__)
#define UNICODE_TIME __TOUNICODE_(__TIME__)

bool NWNXController::writeCrashDump(
	__in HANDLE Process,
	__in ULONG ProcessId,
	__in ULONG ThreadId,
	__in PEXCEPTION_POINTERS ExceptionPointers,
	__in BOOL ClientPointers,
	__in CONST WCHAR *Comment,
	__out std::wstring &DumpFileName
	)
{
	HANDLE                           MiniDumpFile;
	WCHAR                            FileName[ MAX_PATH + 1 ];
	WCHAR                            DumpComment[ 1024 ];
	wxString                         CrashDumpDir;
	MINIDUMP_USER_STREAM             UserStream[ 1 ];
	MINIDUMP_USER_STREAM_INFORMATION UserStreamInfo;
	MINIDUMP_EXCEPTION_INFORMATION   ExceptionInfo;
	bool                             Success;

	DumpFileName.clear();

	MiniDumpFile  = INVALID_HANDLE_VALUE;
	FileName[ 0 ] = L'\0';
	Success       = false;

	ExceptionInfo.ThreadId          = ThreadId;
	ExceptionInfo.ExceptionPointers = ExceptionPointers;
	ExceptionInfo.ClientPointers    = FALSE;

	for (;;)
	{
		if (crashDumpDir == wxT(""))
		{
			wxLogMessage( wxT( "* Crash dumps are not enabled.  Set crashDumpDir in config file to enable them." ) );
			break;
		}

		if (!MiniDumpWriteDump)
		{
			wxLogMessage( wxT( "* DbgHelp.dll or DbgHelp!MiniDumpWriteDump was not found, crash dump writing aborted." ) );
			break;
		}

		//
		// Try and suspend the process first, so that it doesn't time out while
		// waiting for us or continue editing the process VM space.
		//

		if (NtSuspendProcess)
			NtSuspendProcess( Process );

		StringCbPrintfW(
			DumpComment,
			sizeof( DumpComment ),
			L"Generated by nwnx4 built on "
			UNICODE_DATE
			L" "
			UNICODE_TIME
			L", (%s).",
			Comment
			);

#ifdef UNICODE
		if (FAILED(StringCbPrintfW(
			FileName,
			sizeof( FileName ),
			L"%s\\NWN2Server_%lu_%lu_%lu.dmp",
			crashDumpDir.c_str(),
			static_cast< unsigned long >( time( 0 ) ),
			ProcessId,
			ThreadId)))
		{
			FileName[ 0 ] = L'\0';
			break;
		}
#else
		if (FAILED(StringCbPrintfW(
			FileName,
			sizeof( FileName ),
			L"%S\\NWN2Server_%lu_%lu_%lu.dmp",
			crashDumpDir.c_str(),
			static_cast< unsigned long >( time( 0 ) ),
			ProcessId,
			ThreadId)))
		{
			FileName[ 0 ] = L'\0';
			break;
		}
#endif

		try
		{
			DumpFileName = FileName;
		}
		catch (std::bad_alloc)
		{
			break;
		}

		MiniDumpFile = CreateFileW(
			FileName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			0,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			0
			);

		if (MiniDumpFile == INVALID_HANDLE_VALUE)
		{
#ifdef UNICODE
			wxLogMessage(
				wxT( "* Failed to create minidump file %s (%lu)" ),
				FileName,
				GetLastError()
				);
#else
			wxLogMessage(
				wxT( "* Failed to create minidump file %S (%lu)" ),
				FileName,
				GetLastError()
				);
#endif
			FileName[ 0 ] = L'\0';
			break;
		}

		UserStreamInfo.UserStreamCount = 1;
		UserStreamInfo.UserStreamArray = UserStream;

		UserStream[ 0 ].Type       = CommentStreamW;
		UserStream[ 0 ].Buffer     = DumpComment;
		UserStream[ 0 ].BufferSize = static_cast< ULONG >( wcslen( DumpComment ) + 1 ) * sizeof( wchar_t );

		if (!MiniDumpWriteDump(
			Process,
			ProcessId,
			MiniDumpFile,
			dumpType,
			&ExceptionInfo,
			&UserStreamInfo,
			0))
		{
			wxLogMessage(
				wxT( "* MiniDumpWriteDump fails, %lu (%x)." ),
				GetLastError(),
				GetLastError()
				);
			break;
		}

#ifdef UNICODE
		wxLogMessage(
			wxT( "* Dump written to \"%s\" (%s)." ),
			FileName,
			Comment
			);
#else
		wxLogMessage(
			wxT( "* Dump written to \"%S\" (%S)." ),
			FileName,
			Comment
			);
#endif

		Success = true;
		break;
	}

	//
	// Clean up now.
	//

	if (MiniDumpFile)
		CloseHandle( MiniDumpFile );

	//
	// If we failed and we built a file, then we'll remove it.
	//

	if (!Success && FileName[ 0 ] != L'\0')
		DeleteFileW( FileName );

	return Success;
}

bool NWNXController::writeNwn2serverHangDump()
{
	EXCEPTION_POINTERS ExceptionPointers;
	EXCEPTION_RECORD   ExceptionRecord;
	CONTEXT            Context;
	std::wstring       DumpFileName;

	//
	// Capture the thread context of the initial thread, just so we have a
	// thread context.
	//

	ZeroMemory( &Context, sizeof( CONTEXT ) );

	if (!GetThreadContext(
		pi.hThread,
		&Context))
		return false;

	//
	// Build a fake exception record.  We're not really writing out an
	// exception, but we want a snapshot of the process address space so
	// that we can check out what the hang might have been about.
	//

	ExceptionRecord.ExceptionCode     = EXCEPTION_POSSIBLE_DEADLOCK;
	ExceptionRecord.ExceptionFlags    = EXCEPTION_NONCONTINUABLE;
	ExceptionRecord.ExceptionRecord   = 0;
	ExceptionRecord.ExceptionAddress  = reinterpret_cast< PVOID >( Context.Eip );
	ExceptionRecord.NumberParameters  = 0;

	ExceptionPointers.ExceptionRecord = &ExceptionRecord;
	ExceptionPointers.ContextRecord   = &Context;

	//
	// Write a "crash" dump for the seemingly hung server process.
	//

	return writeCrashDump(
		pi.hProcess,
		pi.dwProcessId,
		pi.dwThreadId,
		&ExceptionPointers,
		FALSE,
		L"hang",
		DumpFileName
		);
}
