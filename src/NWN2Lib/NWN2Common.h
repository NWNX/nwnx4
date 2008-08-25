#ifndef _NWNX4_NWN2LIB_NWN2COMMON_H
#define _NWNX4_NWN2LIB_NWN2COMMON_H

//
// General header file for reverse engineered NWN2 information, not specific to
// the client or server binary.
//

namespace NWN2
{

	//
	// Position list for pathing information.
	//

	struct position
	{
		float x;
		float y;
		float z;
		unsigned long idx; // 16-bits with padding perhaps
	};

	struct position_list
	{
		struct position_list *next;
		struct position_list *prev;
		position              pos; // some large array
	};

	struct somestruc
	{
		// positions sorted in ascending order from tail by y
		char                  unk[0x24];     // +0x00
		struct position_list *head;  // +0x24
		struct position_list *tail;  // +0x28
		char                  unk2[0x2c];    // +0x2c
		int                   flag;           // +0x58
	};

}

//
// Pull in common protocol definitions.
//

#include "NWN2ProtocolCommon.h"

//
// Pull in client to server protocol definitions
//

//#include "NWN2ProtocolClientToServer.h"

//
// Pull in server to client protocol definitions.
//

//#include "NWN2ProtocolServerToClient.h"



#endif
