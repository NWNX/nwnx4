/***************************************************************************
    StackTracer - Runtime stack trace wrapper classes
    Copyright (C) 2009 Skywing (skywing@valhallalegends.com).  This instance
    of StackTracer is licensed under the GPLv2 for the usage of the NWNX4
    project, nonwithstanding other licenses granted by the copyright holder.

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

#include "bugfix.h"
#include "StackTracer.h"
#include <dbghelp.h>

#ifdef XP_BUGFIX_USE_SYMBOLS

StackTracer::StackTracer()
: m_SymInitialized(false),
  m_Process( GetCurrentProcess() ),
  m_TraceLog( NULL ),
  m_CurSequence( 1 ),
  m_CurTrace( 0 )
{
}

StackTracer::~StackTracer()
{
	if (m_TraceLog)
		UnmapViewOfFile( m_TraceLog );
	if (m_SymInitialized)
		SymCleanup( GetCurrentProcess() );
}

bool StackTracer::Initialize(
	__in size_t TraceCount,
	__in const std::wstring & TraceLogFileName
	)
{
	HANDLE           File = INVALID_HANDLE_VALUE;
	HANDLE           Section = NULL;
	PSTACK_TRACE_LOG TraceLog = NULL;
	LARGE_INTEGER    FilePointer;

	for (;;)
	{
		//
		// Create and initialize the trace log.
		//

		File = CreateFileW(
			TraceLogFileName.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);

		if (File == INVALID_HANDLE_VALUE)
			break;

		FilePointer.QuadPart = (ULONGLONG)TraceCount * sizeof( STACK_TRACE ) + sizeof( STACK_TRACE_LOG ) - sizeof( STACK_TRACE );

		if (!SetFilePointerEx( File, FilePointer, NULL, FILE_BEGIN ))
			break;

		if (!SetEndOfFile( File ))
			break;

		//
		// Map a section describing the file contents.
		//

		Section = CreateFileMapping(
			File,
			NULL,
			PAGE_READWRITE,
			0,
			0,
			NULL
			);

		if (!Section)
			break;

		TraceLog = (PSTACK_TRACE_LOG)MapViewOfFile(
			Section,
			FILE_MAP_WRITE,
			0,
			0,
			0
			);

		if (!TraceLog)
			break;

		//
		// Setup DbgHelp.
		//

		if (!SymInitialize( m_Process, NULL, FALSE ))
			break;

		SymSetOptions( SymGetOptions() | SYMOPT_DEBUG );

		SymRefreshModuleList( m_Process );

		//
		// All done.
		//

		m_TraceLog = TraceLog;

		return true;
	}

	if (TraceLog)
		UnmapViewOfFile( TraceLog );
	if (Section)
		CloseHandle( Section );
	if (File != INVALID_HANDLE_VALUE)
		CloseHandle( File );

	return false;
}

void StackTracer::LogTrace(
	__in const CONTEXT * Context,
	__in ULONG_PTR TraceContext
	)
{
	CONTEXT      FrameContext;
	ULONG        i;
	ULONG        Machine;
	PSTACK_TRACE Trace;
	STACKFRAME64 FrameData;

	if (!m_TraceLog)
		return;

	RtlCopyMemory( &FrameContext, Context, sizeof( CONTEXT ) );

	//
	// Get the current trace entry in the ring buffer so that we can record the
	// current stack trace.
	//

	Trace = &m_TraceLog->Traces[ m_CurTrace++ ];

	ZeroMemory( Trace->Frames, sizeof( Trace->Frames ) );
	ZeroMemory( &FrameData, sizeof( FrameData ) );

	//
	// The first entry in the frame log is the current PC.
	//

#ifdef _M_IX86

	Machine                      = IMAGE_FILE_MACHINE_I386;
	FrameData.AddrPC.Offset      = FrameContext.Eip;
	FrameData.AddrPC.Segment     = FrameContext.SegCs;
	FrameData.AddrPC.Mode        = AddrModeFlat;
	FrameData.AddrFrame.Offset   = FrameContext.Ebp;
	FrameData.AddrFrame.Segment  = FrameContext.SegSs;
	FrameData.AddrFrame.Mode     = AddrModeFlat;
	FrameData.AddrStack.Offset   = FrameContext.Esp;
	FrameData.AddrStack.Segment  = FrameContext.SegSs;
	FrameData.AddrStack.Mode     = AddrModeFlat;
	FrameData.AddrBStore.Offset  = FrameContext.Ebp;
	FrameData.AddrBStore.Segment = FrameContext.SegSs;
	FrameData.AddrBStore.Mode    = AddrModeFlat;

#elif defined(_M_AMD64)

	Machine                      = IMAGE_FILE_MACHINE_AMD64;
	FrameData.AddrPC.Offset      = FrameContext.Rip;
	FrameData.AddrPC.Segment     = FrameContext.SegCs;
	FrameData.AddrPC.Mode        = AddrModeFlat;
	FrameData.AddrFrame.Offset   = FrameContext.Rbp;
	FrameData.AddrFrame.Segment  = FrameContext.SegSs;
	FrameData.AddrFrame.Mode     = AddrModeFlat;
	FrameData.AddrStack.Offset   = FrameContext.Rsp;
	FrameData.AddrStack.Segment  = FrameContext.SegSs;
	FrameData.AddrStack.Mode     = AddrModeFlat;
	FrameData.AddrBStore.Offset  = FrameContext.Rbp;
	FrameData.AddrBStore.Segment = FrameContext.SegSs;
	FrameData.AddrBStore.Mode    = AddrModeFlat;

#else

#error Unsupported architecture.

#endif

	//
	// Save the current sequence number so that we can order our traces in the
	// event that it becomes necessary.
	//

	Trace->Sequence = m_CurSequence++;

	if (m_CurSequence == 0)
		m_CurSequence = 1;

	//
	// Wrap the current trace write pointer if need be.
	//

	if (m_CurTrace == m_NumTraces)
		m_CurTrace = 0;

	//
	// Save the user-defined per-trace context away.
	//

	Trace->TraceContext = TraceContext;

	for (i = 0; i < NUM_TRACE_FRAMES; i += 1)
	{
		//
		// Capture frame data for the current frame.
		//

		if (!StackWalk64(
			Machine,
			m_Process,
			GetCurrentThread(),
			&FrameData,
			&FrameContext,
			ReadProcessMemory64,
			SymFunctionTableAccess64,
			SymGetModuleBase64,
			NULL))
		{
			break;
		}

		//
		// Save the current frame away.
		//

		Trace->Frames[ i ] = (ULONG_PTR)FrameData.AddrPC.Offset;

		//
		// Continue on to the next frame.
		//
	}
}

BOOL
CALLBACK
StackTracer::ReadProcessMemory64(
	__in HANDLE hProcess,
	__in DWORD64 lpBaseAddress,
	__out PVOID lpBuffer,
	__in DWORD nSize,
	__out LPDWORD lpNumberOfBytesRead
	)
{
	BOOL   Status;
	SIZE_T Transferred;

	Status = ReadProcessMemory(
		hProcess,
		(PVOID)lpBaseAddress,
		lpBuffer,
		(SIZE_T)nSize,
		&Transferred
		);

	*lpNumberOfBytesRead = (DWORD)Transferred;

	return Status;
}

#endif
