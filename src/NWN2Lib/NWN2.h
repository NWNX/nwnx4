#ifndef _NWNX4_NWN2LIB_NWN2_H
#define _NWNX4_NWN2LIB_NWN2_H

#include "NWN2Common.h"

//
// General header file for reverse engineered NWN2Server information.
//

#define NWN2SERVER_VERSION 0x01231763
//#define NWN2SERVER_VERSION 0x01221588
//#define NWN2SERVER_VERSION 0x01211549
//#define NWN2SERVER_VERSION 0x00131409
//#define NWN2SERVER_VERSION 0x00121295

/*
 * Version specific offsets and check data.
 */

#if   NWN2SERVER_VERSION == 0x01231763

/*
 * 1.0.23.1763 & 1.0.23.1765
 */

#define OFFS_ProcessServerMessage      0x00635250 // CNWSMessage::HandlePlayerToServerMessage
#define OFFS_ProcessServerMessageHook  0x005895AD // CServerExoAppInternal::HandleMessage
#define OFFS_CalcPositionLoop0         0x004107ED // NWN2_JStar::SearchStep+0x5D [left]
#define OFFS_CalcPositionLoop0Ret      0x004107FE // NWN2_JStar::SearchStep+0x6E [left]
#define OFFS_CalcPositionLoop1         0x00410B9F // NWN2_JStar::SearchStep+0x40F [right]
#define OFFS_CalcPositionLoop1Ret      0x00410BB0 // NWN2_JStar::SearchStep+0x420 [right]

// NullDerefCrash0 is fixed in 1.0.21.1549
// #define OFFS_NullDerefCrash0           0x005DB05A // CNetLayerWindow::FrameTimeout
// #define OFFS_NullDerefCrash0RetNormal  0x005DB067 // CNetLayerWindow::FrameTimeout
// #define OFFS_NullDerefCrash0RetSkip    0x005DB1CB // CNetLayerWindow::FrameTimeout

// Still 1.0.13.1409 offsets.  Fixed in 1.0.13.1409.
// #define OFFS_NullDerefCrash1           0x0042F595
// #define OFFS_NullDerefCrash1RetNormal  0x0042F59B
// #define OFFS_NullDerefCrash1RetSkip    0x0042F5C1

#define OFFS_NullDerefCrash2           0x0074A03F // NWN2_Collider::UpdateCollider+0x4F
#define OFFS_NullDerefCrash2RetNormal  0x0074A045 // NWN2_Collider::UpdateCollider+0x55
#define OFFS_NullDerefCrash2RetSkip    0x0074A086 // NWN2_Collider::UpdateCollider+0x96

// Fixed in 1.0.21.1549.
// #define OFFS_NullDerefCrash3           0x004F45A4 // CNWSMessage::HandlePlayerToServerDungeonMasterMessage [0x24]
// #define OFFS_NullDerefCrash3RetNormal  0x004F45AA // CNWSMessage::HandlePlayerToServerDungeonMasterMessage [0x24]
// #define OFFS_NullDerefCrash3RetSkip    0x004F45C8 // CNWSMessage::HandlePlayerToServerDungeonMasterMessage [0x24]

// Fixed in 1.0.23.1763
// #define OFFS_NullDerefCrash4           0x0045C00F // CServerExoAppInternal::LoadCharacterStart+0x47F
// #define OFFS_NullDerefCrash4RetNormal  0x0045C016 // CServerExoAppInternal::LoadCharacterStart+0x486
// #define OFFS_NullDerefCrash4RetSkip    0x0045C262 // CServerExoAppInternal::LoadCharacterStart+0x6D2

//
// Actual crash occurs at 0x005D110C <1.0.13.1409> but that is not the right place to hook in
// to fix it.
//
// Doesn't appear to be needed anymore in 1.0.23.1763 (??)
// #define OFFS_Crash5                    0x005D6700 // CNetLayerWindow::UnpacketizeFullMessages+0x80
// #define OFFS_Crash5RetNormal           0x005D6706 // CNetLayerWindow::UnpacketizeFullMessages+0x86
// #define OFFS_Crash5RetSkip             0x005D6706 // CNetLayerWindow::UnpacketizeFullMessages+0x86

// BrianMeyer_Inventory_Msg_BadObjIdCrash
#define OFFS_NullDerefCrash6           0x00630F77 // CNWSMessage::HandlePlayerToServerInventoryMessage+0x887
#define OFFS_NullDerefCrash6RetNormal  0x00630F7E // CNWSMessage::HandlePlayerToServerInventoryMessage+0x88E
#define OFFS_NullDerefCrash6RetSkip    0x00630FE8 // CNWSMessage::HandlePlayerToServerInventoryMessage+0x8F8

//
// Actual crash occurs at 004e8c09 <1.0.13.1409>
//

#define OFFS_NullDerefCrash7           0x00696829 // CNWVirtualMachineCommands::ExecuteCommandActionExchangeItem+0xB9
#define OFFS_NullDerefCrash7RetNormal  0x0069682F // CNWVirtualMachineCommands::ExecuteCommandActionExchangeItem+0xBF
#define OFFS_NullDerefCrash7RetSkip    0x00696870 // CNWVirtualMachineCommands::ExecuteCommandActionExchangeItem+100

//
// Actual crash occurs at 004a2256 <1.0.13.1409>

#define OFFS_NullDerefCrash8           0x005D9B5C // CNWSItem::AcquireItem+0x8C
#define OFFS_NullDerefCrash8RetNormal  0x005D9B62 // CNWSItem::AcquireItem+0x92
#define OFFS_NullDerefCrash8RetSkip    0x005D9CD8 // CNWSItem::AcquireItem+0x208

#define OFFS_CheckUncompress0          0x00504476 // CNetLayerWindow::UnpacketizeFullMessages+0x126 [right]
#define OFFS_CheckUncompress0RetNormal 0x00504484 // CNetLayerWindow::UnpacketizeFullMessages+0x134 [right]
#define OFFS_CheckUncompress0RetSkip   0x00504415 // CNetLayerWindow::UnpacketizeFullMessages+0xC5  [left]

#define OFFS_CheckUncompress1          0x005046BE // CNetLayerWindow::UnpacketizeFullMessages+0x36E [bottom]
#define OFFS_CheckUncompress1RetNormal 0x005046CC // CNetLayerWindow::UnpacketizeFullMessages+0x37C [bottom]
#define OFFS_CheckUncompress1RetSkip   OFFS_CheckUncompress0RetSkip // Same, presently

#define OFFS_UncompressMessage         0x004FF470 // CNetLayerInternal::UncompressMessage+0

#define OFFS_NullDerefCrash9           0x005D405D // CItemRepository::GetItemPtrInRepository+0x5D
#define OFFS_NullDerefCrash9RetNormal  0x005D4062 // CItemRepository::GetItemPtrInRepository+0x62
#define OFFS_NullDerefCrash9RetSkip    0x005D40BB // CItemRepository::GetItemPtrInRepository+0xBB

#define OFFS_NullDerefCrash10          0x005B6AED // CNWSCreatureStats::ValidateLevelUp+0x3FD
#define OFFS_NullDerefCrash10RetNormal 0x005B6AF3 // CNWSCreatureStats::ValidateLevelUp+0x403
#define OFFS_NullDerefCrash10RetSkip   0x005B6730 // CNWSCreatureStats::ValidateLevelUp+0x40

// Not needed for 1.0.22.1588
// #define OFFS_CGameEffectDtor           0x00521B30 // CGameEffect::~CGameEffect
// #define OFFS_ms_iGameEffectCount       0x0085D5A8 // CGameEffect::ms_iGameEffectCount
// #define OFFS_CGameEffectDtorRet        0x00521B38 // CGameEffect::~CGameEffect+8

#define CHECK_ProcessServerMessageHook 0x000ABC9F

#define OFFS_NullDerefCrash11          0x005724fa // CNWSPlayer::DropTURD+0x2da
#define OFFS_NullDerefCrash11RetNormal 0x00572501 // CNWSPlayer::DropTURD+0x2e1
#define OFFS_NullDerefCrash11RetSkip   0x0057250a // CNWSPlayer::DropTURD+0x2ea

#define OFFS_SendCompressionHook       0x00508DE2 // CNetLayerInternal::SendMessageToPlayer+0x52
#define OFFS_SendCompressionHookDoZlib 0x00508de8 // CNetLayerInternal::SendMessageToPlayer+0x58
#define OFFS_SendCompressionHookNoZlib 0x0050902f // CNetLayerInternal::SendMessageToPlayer+0x29f

#define OFFS_SendMessageToPlayer       0x00508D90 // CNetLayerInternal::SendMessageToPlayer+0x0
#define OFFS_FrameReceive              0x00507740 // CNetLayerWindow::FrameRecieve+0x0
#define OFFS_SetInFrameTimer           0x004FEE80 // CNetLayerWindow::SetInFrameTimer+0x0
#define OFFS_CallFrameTimeout          0x0050918A // CNetLayerInternal::UpdateStatusLoop+140 (+139 for raw instr)
#define OFFS_FrameTimeout              0x00508B80 // CNetLayerWindow::FrameTimeout+0x0
#define OFFS_FrameSend                 0x00504110 // CNetLayerWindow::FrameSend+0x0
#define OFFS_HandleMessage             0x0054a390 // CServerExoApp::HandleMessage+0x0

#define OFFS_NullDerefCrash12          0x00632963 // CNWSMessage::HandlePlayerToServerLevelUpMessage+0x443
#define OFFS_NullDerefCrash12Ret       0x00632972 // CNWSMessage::HandlePlayerToServerLevelUpMessage+0x452

#define OFFS_GameObjectUpdateTime1     0x00589E26 // CServerExoAppInternal::UpdateGameObjectsForPlayer+0xc6
#define OFFS_GameObjUpdateSizeLimit1   0x0056EEFC // CNWSMessage::SendServerToPlayerGameObjUpdate+0x7c

#elif NWN2SERVER_VERSION == 0x01221588

/*
 * 1.0.22.1588
 */

#define OFFS_ProcessServerMessage      0x004FFA70 // CNWSMessage::HandlePlayerToServerMessage
#define OFFS_ProcessServerMessageHook  0x0045418D // CServerExoAppInternal::HandleMessage
#define OFFS_CalcPositionLoop0         0x007096DD // NWN2_JStar::SearchStep+0x5D [left]
#define OFFS_CalcPositionLoop0Ret      0x007096EE // NWN2_JStar::SearchStep+0x6E [left]
#define OFFS_CalcPositionLoop1         0x00709A8F // NWN2_JStar::SearchStep+0x40F [right]
#define OFFS_CalcPositionLoop1Ret      0x00709AA0 // NWN2_JStar::SearchStep+0x420 [right]

// NullDerefCrash0 is fixed in 1.0.21.1549
// #define OFFS_NullDerefCrash0           0x005DB05A // CNetLayerWindow::FrameTimeout
// #define OFFS_NullDerefCrash0RetNormal  0x005DB067 // CNetLayerWindow::FrameTimeout
// #define OFFS_NullDerefCrash0RetSkip    0x005DB1CB // CNetLayerWindow::FrameTimeout

// Still 1.0.13.1409 offsets.  Fixed in 1.0.13.1409.
// #define OFFS_NullDerefCrash1           0x0042F595
// #define OFFS_NullDerefCrash1RetNormal  0x0042F59B
// #define OFFS_NullDerefCrash1RetSkip    0x0042F5C1

#define OFFS_NullDerefCrash2           0x007324FF // NWN2_Collider::UpdateCollider+0x4F
#define OFFS_NullDerefCrash2RetNormal  0x00732505 // NWN2_Collider::UpdateCollider+0x55
#define OFFS_NullDerefCrash2RetSkip    0x00732546 // NWN2_Collider::UpdateCollider+0x96

// Fixed in 1.0.21.1549.
// #define OFFS_NullDerefCrash3           0x004F45A4 // CNWSMessage::HandlePlayerToServerDungeonMasterMessage [0x24]
// #define OFFS_NullDerefCrash3RetNormal  0x004F45AA // CNWSMessage::HandlePlayerToServerDungeonMasterMessage [0x24]
// #define OFFS_NullDerefCrash3RetSkip    0x004F45C8 // CNWSMessage::HandlePlayerToServerDungeonMasterMessage [0x24]

#define OFFS_NullDerefCrash4           0x0045C00F // CServerExoAppInternal::LoadCharacterStart+0x47F
#define OFFS_NullDerefCrash4RetNormal  0x0045C016 // CServerExoAppInternal::LoadCharacterStart+0x486
#define OFFS_NullDerefCrash4RetSkip    0x0045C262 // CServerExoAppInternal::LoadCharacterStart+0x6D2

//
// Actual crash occurs at 0x005D110C <1.0.13.1409> but that is not the right place to hook in
// to fix it.
//
#define OFFS_Crash5                    0x005D6700 // CNetLayerWindow::UnpacketizeFullMessages+0x80
#define OFFS_Crash5RetNormal           0x005D6706 // CNetLayerWindow::UnpacketizeFullMessages+0x86
#define OFFS_Crash5RetSkip             0x005D6706 // CNetLayerWindow::UnpacketizeFullMessages+0x86

// BrianMeyer_Inventory_Msg_BadObjIdCrash
#define OFFS_NullDerefCrash6           0x004FB197 // CNWSMessage::HandlePlayerToServerInventoryMessage+0x887
#define OFFS_NullDerefCrash6RetNormal  0x004FB19E // CNWSMessage::HandlePlayerToServerInventoryMessage+0x88E
#define OFFS_NullDerefCrash6RetSkip    0x004FB208 // CNWSMessage::HandlePlayerToServerInventoryMessage+0x8F8

//
// Actual crash occurs at 004e8c09 <1.0.13.1409>
//

#define OFFS_NullDerefCrash7           0x00560ED9 // CNWVirtualMachineCommands::ExecuteCommandActionExchangeItem+0xB9
#define OFFS_NullDerefCrash7RetNormal  0x00560EDF // CNWVirtualMachineCommands::ExecuteCommandActionExchangeItem+0xBF
#define OFFS_NullDerefCrash7RetSkip    0x00560F20 // CNWVirtualMachineCommands::ExecuteCommandActionExchangeItem+100

//
// Actual crash occurs at 004a2256 <1.0.13.1409>

#define OFFS_NullDerefCrash8           0x004A3FEA // CNWSItem::AcquireItem+0x8A
#define OFFS_NullDerefCrash8RetNormal  0x004A3FF0 // CNWSItem::AcquireItem+0x90
#define OFFS_NullDerefCrash8RetSkip    0x004A4168 // CNWSItem::AcquireItem+0x208

#define OFFS_CheckUncompress0          0x005D67A6 // CNetLayerWindow::UnpacketizeFullMessages+0x126 [right]
#define OFFS_CheckUncompress0RetNormal 0x005D67B4 // CNetLayerWindow::UnpacketizeFullMessages+0x134 [right]
#define OFFS_CheckUncompress0RetSkip   0x005D6745 // CNetLayerWindow::UnpacketizeFullMessages+0xC5  [left]

#define OFFS_CheckUncompress1          0x005D69EE // CNetLayerWindow::UnpacketizeFullMessages+0x36E [bottom]
#define OFFS_CheckUncompress1RetNormal 0x005D69FC // CNetLayerWindow::UnpacketizeFullMessages+0x37C [bottom]
#define OFFS_CheckUncompress1RetSkip   OFFS_CheckUncompress0RetSkip // Same, presently

#define OFFS_UncompressMessage         0x005D17F0 // CNetLayerInternal::UncompressMessage+0

#define OFFS_NullDerefCrash9           0x0049E50D // CItemRepository::GetItemPtrInRepository+0x5D
#define OFFS_NullDerefCrash9RetNormal  0x0049E512 // CItemRepository::GetItemPtrInRepository+0x62
#define OFFS_NullDerefCrash9RetSkip    0x0049E56B // CItemRepository::GetItemPtrInRepository+0xBB

#define OFFS_NullDerefCrash10          0x00480C5D // CNWSCreatureStats::ValidateLevelUp+0x3FD
#define OFFS_NullDerefCrash10RetNormal 0x00480C63 // CNWSCreatureStats::ValidateLevelUp+0x403
#define OFFS_NullDerefCrash10RetSkip   0x004808A0 // CNWSCreatureStats::ValidateLevelUp+0x40

// Not needed for 1.0.22.1588
// #define OFFS_CGameEffectDtor           0x00521B30 // CGameEffect::~CGameEffect
// #define OFFS_ms_iGameEffectCount       0x0085D5A8 // CGameEffect::ms_iGameEffectCount
// #define OFFS_CGameEffectDtorRet        0x00521B38 // CGameEffect::~CGameEffect+8

#define CHECK_ProcessServerMessageHook 0x000AB8DF

#elif NWN2SERVER_VERSION == 0x01211549

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

#define OFFS_NullDerefCrash10          0x00480C7D // CNWSCreatureStats::ValidateLevelUp
#define OFFS_NullDerefCrash10RetNormal 0x00480C83 // CNWSCreatureStats::ValidateLevelUp
#define OFFS_NullDerefCrash10RetSkip   0x004808C0 // CNWSCreatureStats::ValidateLevelUp

#define OFFS_CGameEffectDtor           0x00521B30 // CGameEffect::~CGameEffect
#define OFFS_ms_iGameEffectCount       0x0085D5A8 // CGameEffect::ms_iGameEffectCount
#define OFFS_CGameEffectDtorRet        0x00521B38 // CGameEffect::~CGameEffect+8

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
