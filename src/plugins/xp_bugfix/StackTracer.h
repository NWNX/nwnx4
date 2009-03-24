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

#ifndef _STACKTRACER_H
#define _STACKTRACER_H

#ifdef XP_BUGFIX_USE_SYMBOLS

enum { NUM_TRACE_FRAMES = 32 };

typedef struct _STACK_TRACE
{
	ULONG64   Sequence;
	ULONG_PTR TraceContext;
	ULONG_PTR Frames[ NUM_TRACE_FRAMES ];
} STACK_TRACE, * PSTACK_TRACE;


typedef struct _STACK_TRACE_LOG
{
	STACK_TRACE Traces[ 1 ]; /* Variable size */
} STACK_TRACE_LOG, * PSTACK_TRACE_LOG;

class StackTracer
{

public:

	StackTracer();
	virtual ~StackTracer();

	bool Initialize(
		__in size_t TraceCount,
		__in const std::wstring & TraceLogFileName
		);

	void LogTrace(
		__in const CONTEXT * Context,
		__in ULONG_PTR TraceContext
		);

private:

	static
	BOOL
	CALLBACK
	ReadProcessMemory64(
		__in HANDLE hProcess,
		__in DWORD64 lpBaseAddress,
		__out PVOID lpBuffer,
		__in DWORD nSize,
		__out LPDWORD lpNumberOfBytesRead
		);

	bool             m_SymInitialized;
	HANDLE           m_Process;
	PSTACK_TRACE_LOG m_TraceLog;
	ULONG64          m_CurSequence;
	size_t           m_CurTrace;
	size_t           m_NumTraces;

};

#endif

#endif
