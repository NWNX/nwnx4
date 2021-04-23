#ifndef _SOURCE_PROGRAMS_SKYWINGUTILS_CRYPTOGRAPHY_MD5_H
#define _SOURCE_PROGRAMS_SKYWINGUTILS_CRYPTOGRAPHY_MD5_H

#ifdef _MSC_VER
#pragma once
#endif

namespace swutil
{


	typedef unsigned char md5_byte_t; /* 8-bit byte */
	typedef unsigned int md5_word_t; /* 32-bit word */

	/* Define the state of the MD5 Algorithm. */
	typedef struct md5_state_s {
		md5_word_t count[2];	/* message length in bits, lsw first */
		md5_word_t abcd[4];		/* digest buffer */
		md5_byte_t buf[64];		/* accumulate block */
	} md5_state_t;


	typedef md5_state_t MD5_CTX, * PMD5_CTX;

	void
	MD5Init(
		__out PMD5_CTX Ctx
		);

	void
	MD5Update(
		__inout PMD5_CTX Ctx,
		__in const void *Data,
		__in size_t Length
		);

	void
	MD5Final(
		__out unsigned char Digest[ 16 ],
		__inout PMD5_CTX Ctx
		);


}

#endif
