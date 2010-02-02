/***************************************************************************
    NetLayer - Wrapper for AuroraServerNetLayer.dll and hooks into NWN2 to
	use it.

    Copyright (C) 2009 Skywing (skywing@valhallalegends.com).  This instance
    of NetLayerWindow is licensed under the GPLv2 for the usage of the NWNX4
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

#ifndef _AURORA_AURORASERVERNELAYER_H
#define _AURORA_AURORASERVERNELAYER_H

typedef void * NETLAYER_HANDLE;

#define ASNAPI __stdcall

NETLAYER_HANDLE
ASNAPI
AuroraServerNetLayerCreate(
	__in_opt NETLAYER_HANDLE ExistingInstance,
	__in void * Callbacks
	);

typedef
NETLAYER_HANDLE
(ASNAPI *
AuroraServerNetLayerCreateProc)(
	__in_opt NETLAYER_HANDLE ExistingInstance,
	__in void * Callbacks
	);

BOOL
ASNAPI
AuroraServerNetLayerSend(
	__in NETLAYER_HANDLE Connection,
	__in_bcount( Length ) const unsigned char * Data,
	__in size_t Length,
	__in BOOL HighPriority,
	__in BOOL FlushNagle
	);

typedef
BOOL
(ASNAPI *
AuroraServerNetLayerSendProc)(
	__in NETLAYER_HANDLE Connection,
	__in_bcount( Length ) const unsigned char * Data,
	__in size_t Length,
	__in BOOL HighPriority,
	__in BOOL FlushNagle
	);

BOOL
ASNAPI
AuroraServerNetLayerReceive(
	__in NETLAYER_HANDLE Connection,
	__in_bcount( Length ) const unsigned char * Data,
	__in size_t Length
	);

typedef
BOOL
(ASNAPI *
AuroraServerNetLayerReceiveProc)(
	__in NETLAYER_HANDLE Connection,
	__in_bcount( Length ) const unsigned char * Data,
	__in size_t Length
	);

unsigned long
ASNAPI
AuroraServerNetLayerTimeout(
	__in NETLAYER_HANDLE Connection
	);

typedef
unsigned long
(ASNAPI *
AuroraServerNetLayerTimeoutProc)(
	__in NETLAYER_HANDLE Connection
	);

BOOL
ASNAPI
AuroraServerNetLayerDestroy(
	__in NETLAYER_HANDLE Connection
	);

typedef
BOOL
(ASNAPI *
AuroraServerNetLayerDestroyProc)(
	__in NETLAYER_HANDLE Connection
	);

BOOL
ASNAPI
AuroraServerNetLayerQuery(
	__in NETLAYER_HANDLE Connection,
	__in ULONG InfoClass,
	__in ULONG InfoBufferSize,
	__inout_bcount( InfoBufferSize ) PVOID * InfoBuffer
	);

typedef
BOOL
(ASNAPI *
AuroraServerNetLayerQueryProc)(
	__in NETLAYER_HANDLE Connection,
	__in ULONG InfoClass,
	__in ULONG InfoBufferSize,
	__out_opt PULONG ReturnLength,
	__inout_bcount( InfoBufferSize ) PVOID InfoBuffer
	);

enum
{
	AuroraServerQuerySendQueueDepth = 0,

	LastAuroraServerQuery
};

typedef struct _AURORA_SERVER_QUERY_SEND_QUEUE_DEPTH
{
	ULONG SendQueueDepth; // [out]
} AURORA_SERVER_QUERY_SEND_QUEUE_DEPTH, * PAURORA_SERVER_QUERY_SEND_QUEUE_DEPTH;

typedef const struct _AURORA_SERVER_QUERY_SEND_QUEUE_DEPTH * PCAURORA_SERVER_QUERY_SEND_QUEUE_DEPTH;

#endif
