#ifndef _NWNX4_NWN2LIB_NWN2_H
#define _NWNX4_NWN2LIB_NWN2_H

#include "NWN2Common.h"

//
// General header file for reverse engineered NWN2Server information.
//

#define NWN2SERVER_VERSION 0x01211549
//#define NWN2SERVER_VERSION 0x00131409
//#define NWN2SERVER_VERSION 0x00121295

/*
 * Version specific offsets and check data.
 */

#if   NWN2SERVER_VERSION == 0x01211549

/*
 * 1.0.21.1549
 */

#define OFFS_ProcessServerMessage      0x004FFB70 // CNWSMessage::HandlePlayerToServerMessage
#define OFFS_ProcessServerMessageHook  0x0045417F // CServerExoAppInternal::HandleMessage
#define OFFS_CalcPositionLoop0         0x007098DD // NWN2_JStar::SearchStep [left]
#define OFFS_CalcPositionLoop0Ret      0x007098EE // NWN2_JStar::SearchStep [left]
#define OFFS_CalcPositionLoop1         0x00709C8F // NWN2_JStar::SearchStep [right]
#define OFFS_CalcPositionLoop1Ret      0x00709CA0 // NWN2_JStar::SearchStep [right]

// NullDerefCrash0 is fixed in 1.0.21.1549
// #define OFFS_NullDerefCrash0           0x005DB05A // CNetLayerWindow::FrameTimeout
// #define OFFS_NullDerefCrash0RetNormal  0x005DB067 // CNetLayerWindow::FrameTimeout
// #define OFFS_NullDerefCrash0RetSkip    0x005DB1CB // CNetLayerWindow::FrameTimeout

// Still 1.0.13.1409 offsets.  Fixed in 1.0.13.1409.
// #define OFFS_NullDerefCrash1           0x0042F595
// #define OFFS_NullDerefCrash1RetNormal  0x0042F59B
// #define OFFS_NullDerefCrash1RetSkip    0x0042F5C1

#define OFFS_NullDerefCrash2           0x0073271F // NWN2_Collider::UpdateCollider
#define OFFS_NullDerefCrash2RetNormal  0x00732725 // NWN2_Collider::UpdateCollider
#define OFFS_NullDerefCrash2RetSkip    0x00732766 // NWN2_Collider::UpdateCollider

// Fixed in 1.0.21.1549.
// #define OFFS_NullDerefCrash3           0x004F45A4 // CNWSMessage::HandlePlayerToServerDungeonMasterMessage [0x24]
// #define OFFS_NullDerefCrash3RetNormal  0x004F45AA // CNWSMessage::HandlePlayerToServerDungeonMasterMessage [0x24]
// #define OFFS_NullDerefCrash3RetSkip    0x004F45C8 // CNWSMessage::HandlePlayerToServerDungeonMasterMessage [0x24]

#define OFFS_NullDerefCrash4           0x0045C00F // CServerExoAppInternal::LoadCharacterStart
#define OFFS_NullDerefCrash4RetNormal  0x0045C016 // CServerExoAppInternal::LoadCharacterStart 
#define OFFS_NullDerefCrash4RetSkip    0x0045C262 // CServerExoAppInternal::LoadCharacterStart

//
// Actual crash occurs at 0x005D110C <1.0.13.1409> but that is not the right place to hook in
// to fix it.
//
#define OFFS_Crash5                    0x005D6820 // CNetLayerWindow::UnpacketizeFullMessages
#define OFFS_Crash5RetNormal           0x005D6826 // CNetLayerWindow::UnpacketizeFullMessages
#define OFFS_Crash5RetSkip             0x005D6826 // CNetLayerWindow::UnpacketizeFullMessages

// BrianMeyer_Inventory_Msg_BadObjIdCrash
#define OFFS_NullDerefCrash6           0x004FB297 // CNWSMessage::HandlePlayerToServerInventoryMessage+887
#define OFFS_NullDerefCrash6RetNormal  0x004FB29E // CNWSMessage::HandlePlayerToServerInventoryMessage+88E
#define OFFS_NullDerefCrash6RetSkip    0x004FB308 // CNWSMessage::HandlePlayerToServerInventoryMessage+8F6

//
// Actual crash occurs at 004e8c09 <1.0.13.1409>
//

#define OFFS_NullDerefCrash7           0x00561029 // CNWVirtualMachineCommands::ExecuteCommandActionExchangeItem+B9
#define OFFS_NullDerefCrash7RetNormal  0x0056102F // CNWVirtualMachineCommands::ExecuteCommandActionExchangeItem+BF
#define OFFS_NullDerefCrash7RetSkip    0x00561070 // CNWVirtualMachineCommands::ExecuteCommandActionExchangeItem=100

//
// Actual crash occurs at 004a2256 <1.0.13.1409>

#define OFFS_NullDerefCrash8           0x004A405A // CNWSItem::AcquireItem+8A
#define OFFS_NullDerefCrash8RetNormal  0x004A4060 // CNWSItem::AcquireItem+90
#define OFFS_NullDerefCrash8RetSkip    0x004A41D8 // CNWSItem::AcquireItem+208

#define OFFS_CheckUncompress0          0x005D68C6 // CNetLayerWindow::UnpacketizeFullMessages+126 [right]
#define OFFS_CheckUncompress0RetNormal 0x005D68D4 // CNetLayerWindow::UnpacketizeFullMessages+134 [right]
#define OFFS_CheckUncompress0RetSkip   0x005D6865 // CNetLayerWindow::UnpacketizeFullMessages+C5  [left]

#define OFFS_CheckUncompress1          0x005D6B0E // CNetLayerWindow::UnpacketizeFullMessages+370 [bottom]
#define OFFS_CheckUncompress1RetNormal 0x005D6B1C // CNetLayerWindow::UnpacketizeFullMessages+37C [bottom]
#define OFFS_CheckUncompress1RetSkip   OFFS_CheckUncompress0RetSkip // Same, presently

#define OFFS_UncompressMessage         0x005D1910 // CNetLayerInternal::UncompressMessage+0

#define OFFS_NullDerefCrash9           0x0049E56D // CItemRepository::GetItemPtrInRepository+5D
#define OFFS_NullDerefCrash9RetNormal  0x0049E572 // CItemRepository::GetItemPtrInRepository+62
#define OFFS_NullDerefCrash9RetSkip    0x0049E5CB // CItemRepository::GetItemPtrInRepository+BB

#define CHECK_ProcessServerMessageHook 0x000AB9ED

#elif NWN2SERVER_VERSION == 0x00131409

/*
 * 1.0.13.1409
 */

#define OFFS_ProcessServerMessage      0x004FD860
#define OFFS_ProcessServerMessageHook  0x0045336F
#define OFFS_CalcPositionLoop0         0x007042AD
#define OFFS_CalcPositionLoop0Ret      0x007042BE
#define OFFS_CalcPositionLoop1         0x0070465F
#define OFFS_CalcPositionLoop1Ret      0x00704670
#define OFFS_NullDerefCrash0           0x005D58DA
#define OFFS_NullDerefCrash0RetNormal  0x005D58E3
#define OFFS_NullDerefCrash0RetSkip    0x005D5A3B
#define OFFS_NullDerefCrash1           0x0042F595
#define OFFS_NullDerefCrash1RetNormal  0x0042F59B
#define OFFS_NullDerefCrash1RetSkip    0x0042F5C1
#define OFFS_NullDerefCrash2           0x0072C41F
#define OFFS_NullDerefCrash2RetNormal  0x0072C425
#define OFFS_NullDerefCrash2RetSkip    0x0072C466
#define OFFS_NullDerefCrash3           0x004F2327
#define OFFS_NullDerefCrash3RetNormal  0x004F232D
#define OFFS_NullDerefCrash3RetSkip    0x004F234B
#define OFFS_NullDerefCrash4           0x0045B1FF
#define OFFS_NullDerefCrash4RetNormal  0x0045B206
#define OFFS_NullDerefCrash4RetSkip    0x0045B452
//
// Actual crash occurs at 0x005D110C but that is not the right place to hook in
// to fix it.
//
#define OFFS_Crash5                    0x005D10A0
#define OFFS_Crash5RetNormal           0x005D10A6
#define OFFS_Crash5RetSkip             0x005D10A6

// BrianMeyer_Inventory_Msg_BadObjIdCrash
#define OFFS_NullDerefCrash6           0x004F9027
#define OFFS_NullDerefCrash6RetNormal  0x004F902E
#define OFFS_NullDerefCrash6RetSkip    0x004F9098

//
// Actual crash occurs at 004e8c09 
//

#define OFFS_NullDerefCrash7           0x0055D3A9
#define OFFS_NullDerefCrash7RetNormal  0x0055D3AF
#define OFFS_NullDerefCrash7RetSkip    0x0055D3F0

//
// Actual crash occurs at 004a2256

#define OFFS_NullDerefCrash8           0x004A287A
#define OFFS_NullDerefCrash8RetNormal  0x004A2880
#define OFFS_NullDerefCrash8RetSkip    0x004A29F8

#define OFFS_CheckUncompress0          0x005D1146
#define OFFS_CheckUncompress0RetNormal 0x005D1154
#define OFFS_CheckUncompress0RetSkip   0x005D10E5

#define OFFS_CheckUncompress1          0x005D138E
#define OFFS_CheckUncompress1RetNormal 0x005D139C
#define OFFS_CheckUncompress1RetSkip   0x005D10E5

#define OFFS_UncompressMessage         0x005CC190

#define OFFS_NullDerefCrash9           0x0049CDDD
#define OFFS_NullDerefCrash9RetNormal  0x0049CDE2
#define OFFS_NullDerefCrash9RetSkip    0x0049CE3B

#define CHECK_ProcessServerMessageHook 0x000AA4ED



#elif NWN2SERVER_VERSION == 0x00121295

/*
 * 1.0.12.1295
 */

#define OFFS_ProcessServerMessage      0x004FC320
#define OFFS_ProcessServerMessageHook  0x0045295F
#define OFFS_CalcPositionLoop0         0x006FB23D
#define OFFS_CalcPositionLoop0Ret      0x006FB24E
#define OFFS_CalcPositionLoop1         0x006FB5EF
#define OFFS_CalcPositionLoop1Ret      0x006FB600
#define OFFS_NullDerefCrash0           0x005D1F9A
#define OFFS_NullDerefCrash0RetNormal  0x005D1FA3
#define OFFS_NullDerefCrash0RetSkip    0x005D20FB
#define OFFS_NullDerefCrash1           0x0042EF85
#define OFFS_NullDerefCrash1RetNormal  0x0042EF8B
#define OFFS_NullDerefCrash1RetSkip    0x0042EFB1
#define OFFS_NullDerefCrash2           0x0072331F
#define OFFS_NullDerefCrash2RetNormal  0x00723325
#define OFFS_NullDerefCrash2RetSkip    0x00723366
#define OFFS_NullDerefCrash3           0x004F1127
#define OFFS_NullDerefCrash3RetNormal  0x004F112D
#define OFFS_NullDerefCrash3RetSkip    0x004F114B
#define OFFS_NullDerefCrash4           0x0045A81F
#define OFFS_NullDerefCrash4RetNormal  0x0045A826
#define OFFS_NullDerefCrash4RetSkip    0x0045AA72
//
// Actual crash occurs at 0x005CD7CC but that is not the right place to hook in
// to fix it.
//
#define OFFS_Crash5                    0x005CD760
#define OFFS_Crash5RetNormal           0x005CD766
#define OFFS_Crash5RetSkip             0x005CD766

// BrianMeyer_Inventory_Msg_BadObjIdCrash
#define OFFS_NullDerefCrash6           0x004F7E27
#define OFFS_NullDerefCrash6RetNormal  0x004F7E2E
#define OFFS_NullDerefCrash6RetSkip    0x004F7E98

//
// Actual crash occurs at 004e7a49 
//

#define OFFS_NullDerefCrash7           0x00558949
#define OFFS_NullDerefCrash7RetNormal  0x0055894F
#define OFFS_NullDerefCrash7RetSkip    0x00558990

//
// Actual crash occurs at 004a1586

#define OFFS_NullDerefCrash8           0x004A18AA
#define OFFS_NullDerefCrash8RetNormal  0x004A18B0
#define OFFS_NullDerefCrash8RetSkip    0x004A1A28

#define OFFS_CheckUncompress0          0x005CD806
#define OFFS_CheckUncompress0RetNormal 0x005CD814
#define OFFS_CheckUncompress0RetSkip   0x005CD7A5

#define OFFS_CheckUncompress1          0x005CDA4E
#define OFFS_CheckUncompress1RetNormal 0x005CDA5C
#define OFFS_CheckUncompress1RetSkip   0x005CD7A5

#define OFFS_UncompressMessage         0x005C8850

#define OFFS_NullDerefCrash9           0x0049BE0D
#define OFFS_NullDerefCrash9RetNormal  0x0049BE12
#define OFFS_NullDerefCrash9RetSkip    0x0049BE6B


#define CHECK_ProcessServerMessageHook 0x000A99BD

#elif (NWN2SERVER_VERSION == 0x0111153)

/*
 * 1.0.11.1153
 */

#define OFFS_ProcessServerMessage      0x004FC070
#define OFFS_ProcessServerMessageHook  0x0045276F

#define CHECK_ProcessServerMessageHook 0x000A98FD

#else

#error Unsupported nwn2server version!

#endif


#endif
