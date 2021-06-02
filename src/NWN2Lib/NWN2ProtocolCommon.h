#ifndef _NWNX4_NWN2LIB_NWN2PROTOCOLCOMMON_H
#define _NWNX4_NWN2LIB_NWN2PROTOCOLCOMMON_H

namespace NWN2
{

	//
	// Message class.
	//

	struct NWMSGCLS
	{
		enum CLSENUM
		{
			Server = 0x50,
			Client = 0x70,

			MaximumNwnCls
		};
	};

	//
	// Major function codes.
	//

	struct NWMSG
	{
		enum MSGENUM
		{
			ServerStatus              = 0x01,
			Login                     = 0x02,
			Module                    = 0x03,
			Area                      = 0x04,
			GameObjUpdate             = 0x05,
			Input                     = 0x06,
			Store                     = 0x07,
			Gold                      = 0x08,
			Chat                      = 0x09,
			PlayerList                = 0x0A,
			UnusedB                   = 0x0B,
			Inventory                 = 0x0C,
			GuiInventory              = 0x0D,
			Party                     = 0x0E,
			Cheat                     = 0x0F,
			Camera                    = 0x10,
			CharList                  = 0x11,
			ClientSideMessage         = 0x12,
			Combat_Round              = 0x13,
			Dialog                    = 0x14,
			GuiCharacterSheet         = 0x15,
			QuickChat                 = 0x16,
			Sound                     = 0x17,
			Item_Property             = 0x18,
			GuiContainer              = 0x19,
			VoiceChat                 = 0x1A,
			GuiInfoPopup              = 0x1B,
			Journal                   = 0x1C,
			LevelUp                   = 0x1D, // Um?
			GuiHotBar                 = 0x1E,
			DungeonMaster             = 0x1F,
			MapPin                    = 0x20,
			DebugInfo                 = 0x21,
			SafeProjectile            = 0x22,
			Barter                    = 0x23,
			PopUpGUIPanel             = 0x24,
			Death                     = 0x25,
			GroupInput                = 0x26,
			DungeonMasterGroup        = 0x27,
			Ambient                   = 0x28,
			UnknownNameMsg            = 0x29, // They used the function pointer instead of the string in the debug print...
			Portal                    = 0x2A,
			Character_Download        = 0x2B,
			LoadBar                   = 0x2C,
			SaveLoad                  = 0x2D,
			GUIPartyBar               = 0x2E,
			ShutDownServer            = 0x2F,
			LevelUp2                  = 0x30, // Um?
			PlayModuleCharacterList   = 0x31,
			CustomToken               = 0x32,
			Cutscene                  = 0x33,
			GuiRoster                 = 0x34,
			WorldMap                  = 0x35,
			CustomAnim                = 0x36,



			MaximumNwnMsg
		};
	};

	//
	// Minor function codes, dependent upon the major function code.
	//

	//
	// Input
	//

	struct MSGINPUT
	{
		enum MSGENUM
		{
			Attack                   = 0x02,
			Use                      = 0x03,
			Examine                  = 0x05,
			PerformAction            = 0x07,
			ToggleMode               = 0x0A,
			LearnSpell               = 0x10,
			CastSpell                = 0x12,
			GuiAction                = 0x19,
			SelectObject             = 0x27,
			RunScript                = 0x30,

			MaximumSubfunction
		};
	};

	//
	// GameObjUpdate
	//

	struct MSGGAMEOBJUPDATE
	{
		enum MSGENUM
		{
		};
	};

	//
	// CustomAnim
	//

	struct MSGCUSTOMANIM
	{
		enum MSGENUM
		{
			Play                    = 0x01,

			MaximumSubfunction
		};
	};

	//
	// PlayerList
	//

	struct MSGPLAYERLIST
	{
		enum MSGENUM
		{
			PlayerEnum               = 0x01,
			PlayerJoin               = 0x02,
			PlayerLeave              = 0x03,

			MaximumSubfunction
		};
	};

	//
	// ClientSideMessage
	//

	struct MSGCLIENTSIDEMESSAGE
	{
		enum MSGENUM
		{
			TextString               = 0x0B,

			MaximumSubfunction
		};
	};

	//
	// Chat
	//

	struct MSGCHAT
	{
		enum MSGENUM
		{
			Say                      = 0x01,
			Whisper                  = 0x03,
			Tell                     = 0x04,

			MaximumSubfunction
		};
	};

	//
	// Protocol message structures, generic to all message functions.
	//

	enum { nullptr_OBJID  = 0x7F000000 };

	typedef unsigned long OBJECT_ID;
	typedef unsigned long PLAYER_ID;

#include <pshpack1.h>
	typedef struct _POSITION
	{
		float x;
		float y;
		float z;
	} POSITION, * PPOSITION;
#include <poppack.h>

#include <pshpack1.h>
#pragma warning(push)
#pragma warning(disable:4200) // non-standard extension used: zero length array
	typedef struct _CSTRING
	{
		ULONG Length;
		CHAR  Data[ 0 ];
	} CSTRING, *PCSTRING;
#pragma warning(pop)
#include <poppack.h>

	//
	// Generic message header used by all protocol messages.
	//

#include <pshpack1.h>
#pragma warning(push)
#pragma warning(disable:4200) // non-standard extension used: zero length array
#include <pshpack1.h>
	struct message
	{
		unsigned char cls;
		unsigned char function;
		unsigned char subfunction;
		unsigned long length; // includes all but cls
		unsigned char data[];
	};
#include <poppack.h>
#pragma warning(pop)

	//
	// Specific datatypes.
	//
	
	//
	// Mode selection, for Input.ToggleMode
	//

	struct MODE
	{
		enum MODEENUM
		{
			Detect                   = 0x00,
			Stealth                  = 0x01,
			Parry                    = 0x02,
			PowerAttack              = 0x03,
			CombatExpertise          = 0x08,
			DefensiveCasting         = 0x0A,
			Track                    = 0x0E,
			Darkvision               = 0x18,

			MaximumMode
		};
	};

	typedef unsigned char COMBAT_MODE;

	//
	// Spell identifiers.
	//

	struct SPELL
	{
		enum SPELLENUM
		{
			BurningHands              = 0x0000000a,
			DominatePerson            = 0x0000002d,
			Identify                  = 0x00000056,
			GreaterInvisibility       = 0x00000058,
			InvisibilitySphere        = 0x0000005c,
			Light                     = 0x00000064,
			MagicMissile              = 0x0000006b,
			PolymorphSelf             = 0x00000082,
			Stoneskin                 = 0x000000ac,
			Shield                    = 0x000001a1,
			PolymorphSelfBladeSpider  = 0x00000183,
			AcidSplash                = 0x000001a8,
			BigbysInterposingHand     = 0x000001cb,
			ProtectionFromArrows      = 0x00000355,
			PowerWordWeaken           = 0x000003f1,
			WallOfDispelMagic         = 0x00000409,
			GlassDoppleganger         = 0x0000046d,

			MaximumSpell
		};
	};

	typedef unsigned long SPELL_ID;

	//
	// Metamagic (spell modifier) identifiers (bit vector).
	//

	struct METAMAGIC
	{
		enum METAMAGICENUM
		{
			Normal                    = 0x00000000,
			Empower                   = 0x00000001,
			Maximize                  = 0x00000004,

			MaximumMetamagic
		};
	};

	typedef unsigned long METAMAGIC_FLAGS;

	//
	// "GUI" actions, for Input.GuiAction.
	//

	struct GUIACTION
	{
		enum GUIACTIONENUM
		{
			CloseMain                = 0x80,
			OpenMainMenu             = 0x90,

			MaximumGuiAction
		};
	};

	//
	// "Use Object" actions, for Input.Use.
	//

	struct USEACTION
	{
		enum USEACTIONENUM
		{
			Close3                   = 0x03,
			Close4                   = 0x04,
			CloseDoor                = 0x15,
			OpenDoor                 = 0x16,

			LastUseAction
		};
	};



}

#endif
