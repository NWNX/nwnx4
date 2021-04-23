#pragma once

const UINT8 ENCAP_MAGIC = 'E';
const UINT32 FRAME_DATA_SIZE = 0x400;  // 0x3C0 + encapsulation header


int
__stdcall
ReflectorSendto(
	__in SOCKET s,
	__in const char *buf,
	__in int len,
	__in int flags,
	__in struct sockaddr_in *to,
	__in int tolen
	);

int
__stdcall
ReflectorRecvfrom(
	__in SOCKET s,
	__out char *buf,
	__in int len,
	__in int flags,
	__out struct sockaddr *from,
	__inout_opt int *fromlen
	);

VOID
ReflectorSetTargetSecret(
	__in CONST CHAR * TargetSecretIn
	);

BOOLEAN
ReflectorIsEnabled(
	VOID
	);

#define INADDR_LOOPBACK_NET 0x0100007f