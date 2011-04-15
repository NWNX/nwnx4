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

namespace NWN
{
	typedef unsigned long OBJECTID;
	typedef unsigned long PLAYERID;

	static const OBJECTID INVALIDOBJID          = 0x7F000000;

	static const unsigned long LISTTYPE_MASK    = 0x80000000;

	static const PLAYERID PLAYERID_INVALIDID    = 0xFFFFFFFE;

	struct Vector3
	{
		float x;
		float y;
		float z;
	};

	struct Matrix44
	{
		float m[4][4];
	};

	struct D3DXCOLOR
	{
		float r;
		float g;
		float b;
		float a;
	};

	struct NWN2_TintSet
	{
		D3DXCOLOR Colors[ 3 ];
	};

	C_ASSERT( sizeof( NWN2_TintSet ) == 0x30 );

	typedef enum _OBJECT_TYPE
	{
		OBJECT_TYPE_GUI                = 0x01,
		OBJECT_TYPE_TILE               = 0x02,
		OBJECT_TYPE_MODULE             = 0x03,
		OBJECT_TYPE_AREA               = 0x04,
		OBJECT_TYPE_CREATURE           = 0x05,
		OBJECT_TYPE_ITEM               = 0x06,
		OBJECT_TYPE_TRIGGER            = 0x07,
		OBJECT_TYPE_PROJECTILE         = 0x08,
		OBJECT_TYPE_PLACEABLE          = 0x09,
		OBJECT_TYPE_DOOR               = 0x0A,
		OBJECT_TYPE_AREAOFEFFECTOBJECT = 0x0B,
		OBJECT_TYPE_WAYPOINT           = 0x0C,
		OBJECT_TYPE_ENCOUNTER          = 0x0D,
		OBJECT_TYPE_STORE              = 0x0E,
		OBJECT_TYPE_PORTAL             = 0x0F,
		OBJECT_TYPE_SOUND              = 0x10,
		OBJECT_TYPE_DYNAMIC            = 0x11,
		OBJECT_TYPE_STATIC_CAMERA      = 0x12,
		OBJECT_TYPE_ENVIRONMENT_OBJECT = 0x13,
		OBJECT_TYPE_TREE               = 0x14,
		OBJECT_TYPE_LIGHT              = 0x15,
		OBJECT_TYPE_PLACED_EFFECT      = 0x16,
		OBJECT_TYPE_COUNT              = 0x17
	} OBJECT_TYPE, * POBJECT_TYPE;





	union CGameObjectFlags
	{
		unsigned __int64 Flags;
		struct
		{
			unsigned long NewPosition        : 1;
			unsigned long NewHeading         : 1;
			unsigned long NewOrientation     : 1;
			unsigned long NewScale           : 1;
			unsigned long WorldTransIsDirty  : 1;
			unsigned long Visible            : 1;
			unsigned long MarkedForDeletion  : 1;
			unsigned long Reserved1          : 1;
			unsigned long Reserved2          : 24;
			unsigned long AttachedToPoint    : 9;
			unsigned long AttachedToVia      : 9;
			unsigned long Reserved3          : 14;
		};
	};

	C_ASSERT( sizeof( CGameObjectFlags ) == 8 );

	struct CNWSCreature;
	struct CNWSArea;

	struct CGameObject
	{
		void                                   * vtable[4];
		NWN::Vector3                             m_vUp;
		NWN::Vector3                             m_vHeading;
		NWN::Vector3                             m_vScale;
		unsigned char                            Padding0[ 0x0C ];
		NWN::Matrix44                            m_WorldTrans;
		struct NWN2_Object                     * m_pAttachedTo;
		unsigned char                            m_AttachmentPoints[ 0x0C ];
		CGameObjectFlags                         Flags;
		unsigned long                            m_PrmaryUpdateCycle;
		unsigned long                            m_SecondaryUpdateCycle;
		NWN::OBJECTID                            m_idSelf;
		unsigned char                            m_nObjectType;
		int                                      m_nAnimation;
		float                                    m_fAnimSpeed;
		float                                    m_fAnimDuration;
		struct DialogueData                    * m_pDialogueData;
		unsigned char                            Padding1[ 0x8 ];

		inline
		NWN::OBJECT_TYPE
		GetObjectType(
			) const
		{
			return (NWN::OBJECT_TYPE) m_nObjectType;
		}

		inline
		NWN::OBJECTID
		GetObjectId(
			) const
		{
			return m_idSelf;
		}

		inline
		CNWSCreature *
		AsCreature(
			)
		{
			if (GetObjectType( ) != NWN::OBJECT_TYPE_CREATURE)
				return NULL;

			return (CNWSCreature *) this;
		}

		inline
		CNWSArea *
		AsArea(
			)
		{
			if (GetObjectType( ) != NWN::OBJECT_TYPE_AREA)
				return NULL;

			return (CNWSArea *) ((char *) this - ArGameObjectBaseClassOffset);
		}
	};

	C_ASSERT( sizeof( CGameObject ) == 0xC0 );
	C_ASSERT( offsetof( CGameObject, m_pDialogueData ) == 0xB4 );
	C_ASSERT( offsetof( CGameObject, m_idSelf ) == 0xa0);
	C_ASSERT( offsetof( CGameObject, m_pAttachedTo ) == 0x80);
	C_ASSERT( offsetof( CGameObject, m_WorldTrans ) == 0x40);
	C_ASSERT( offsetof( CGameObject, m_vUp ) == 0x10 );
	C_ASSERT( offsetof( CGameObject, m_vHeading ) == 0x1C );
	C_ASSERT( offsetof( CGameObject, m_vScale ) == 0x28 );


	enum TintSetType
	{
		CreatureTintSetBody,
		CreatureTintSetHead,
		CreatureTintSetHair,

		LastCreatureTintSet
	};


	struct CNWSCreatureStatsCore
	{
		inline
		void
		SetHeadVariation(
			__in unsigned char Variation
			)
		{
			*(unsigned char *) ((char *) this + CsHeadVariation) = Variation;
		}

		inline
		void
		SetHairVariation(
			__in unsigned char Variation
			)
		{
			*(unsigned char *) ((char *) this + CsHairVariation) = Variation;
		}

		inline
		void
		SetTailVariation(
			__in unsigned char Variation
			)
		{
			*(unsigned char *) ((char *) this + CsTailVariation) = Variation;
		}

		inline
		void
		SetWingVariation(
			__in unsigned char Variation
			)
		{
			*(unsigned char *) ((char *) this + CsWingVariation) = Variation;
		}

		inline
		void
		SetFacialHairVariation(
			__in unsigned char Variation
			)
		{
			*(unsigned char *) ((char *) this + CsFacialHairVariation) = Variation;
		}

		inline
		void
		SetTintSet(
			__in TintSetType WhichTintSet,
			__in const NWN::NWN2_TintSet * TintSet
			)
		{
			NWN::NWN2_TintSet * ObjTintSet;

			switch (WhichTintSet)
			{

			case CreatureTintSetBody:
				ObjTintSet = (NWN::NWN2_TintSet *) ((char *) this + CsTint);
				break;

			case CreatureTintSetHead:
				ObjTintSet = (NWN::NWN2_TintSet *) ((char *) this + CsHeadTint);
				break;

			case CreatureTintSetHair:
				ObjTintSet = (NWN::NWN2_TintSet *) ((char *) this + CsHairTint);
				break;

			default:
				return;

			}

			memcpy( ObjTintSet, &TintSet, sizeof( *TintSet ) );
		}
	};

	struct CNWSCreature : public CGameObject
	{

		inline
		CNWSCreatureStatsCore *
		GetStats(
			)
		{
			return *(CNWSCreatureStatsCore **) ((char *) this + CrStats);
		}

		inline
		void
		SetHeadVariation(
			__in unsigned char Variation
			)
		{
			*(unsigned char *) ((char *) this + CrAppearance + CaHeadVariation) = Variation;

			GetStats( )->SetHeadVariation( Variation );
		}

		inline
		void
		SetHairVariation(
			__in unsigned char Variation
			)
		{
			*(unsigned char *) ((char *) this + CrAppearance + CaHairVariation) = Variation;

			GetStats( )->SetHairVariation( Variation );
		}

		inline
		void
		SetTailVariation(
			__in unsigned char Variation
			)
		{
			*(unsigned char *) ((char *) this + CrAppearance + CaTailVariation) = Variation;

			GetStats( )->SetTailVariation( Variation );
		}

		inline
		void
		SetWingVariation(
			__in unsigned char Variation
			)
		{
			*(unsigned char *) ((char *) this + CrAppearance + CaWingVariation) = Variation;

			GetStats( )->SetWingVariation( Variation );
		}

		inline
		void
		SetFacialHairVariation(
			__in unsigned char Variation
			)
		{
			*(unsigned char *) ((char *) this + CrAppearance + CaFacialHairVariation) = Variation;

			GetStats( )->SetFacialHairVariation( Variation );
		}

		inline
		void
		SetTintSet(
			__in TintSetType WhichTintSet,
			__in const NWN::NWN2_TintSet * TintSet
			)
		{
			NWN::NWN2_TintSet * ObjTintSet;

			switch (WhichTintSet)
			{

			case CreatureTintSetBody:
				ObjTintSet = (NWN::NWN2_TintSet *) ((char *) this + CrAppearance + CaTint);
				break;

			case CreatureTintSetHead:
				ObjTintSet = (NWN::NWN2_TintSet *) ((char *) this + CrAppearance + CaHeadTint);
				break;

			case CreatureTintSetHair:
				ObjTintSet = (NWN::NWN2_TintSet *) ((char *) this + CrAppearance + CaHairTint);
				break;

			default:
				return;

			}

			memcpy( ObjTintSet, &TintSet, sizeof( *TintSet ) );

			GetStats( )->SetTintSet( WhichTintSet, TintSet );
		}
	};

	C_ASSERT( offsetof( CNWSCreature, vtable ) == 0 );

	struct CNWSArea_Bases
	{
		unsigned char AreaObject__BaseClass_Data[ ArGameObjectBaseClassOffset ];
	};

	struct CNWSArea : public CNWSArea_Bases,
	                  public CGameObject
	{
	};

	C_ASSERT( offsetof( CNWSArea, vtable ) == ArGameObjectBaseClassOffset );

	struct CNetLayer
	{
		static const PLAYERID PLAYERID_INVALIDID = NWN::PLAYERID_INVALIDID;
	};
}


//
// Compatibility with NWN2GameLib.
//

typedef NWN::CNWSCreature CreatureObject;
typedef NWN::CNWSArea AreaObject;
typedef NWN::CGameObject GameObject;

//
// Define a (roughly) NWN2GameLib-compatible object management system.
//

class GameObjectManager
{

public:

	//
	// Return a game object by object id.  Returns NULL if there was no match.
	//

	inline
	GameObject *
	GetGameObject(
		__in NWN::OBJECTID ObjectId
		)
	{
		typedef
		NWN::CGameObject *
		(__cdecl * NWServer_GetGameObjectProc)(
			__in NWN::OBJECTID ObjectId
			);

		static NWServer_GetGameObjectProc NWServer_GetGameObject = (NWServer_GetGameObjectProc) OFFS_NWServerGetGameObject;

		return NWServer_GetGameObject( ObjectId );
	}

	//
	// Return a creature object by object id.  Returns NULL if there was no
	// such object, or if there was an object type mismatch.
	//

	inline
	CreatureObject *
	GetCreature(
		__in NWN::OBJECTID ObjectId
		)
	{
		GameObject * Object;

		if ((Object = GetGameObject( ObjectId )) == NULL)
			return NULL;

		return Object->AsCreature( );
	}

	//
	// Return an area object by object id.  Returns NULL if there was no
	// such object, or if there was an object type mismatch.
	//

	inline
	AreaObject *
	GetArea(
		__in NWN::OBJECTID ObjectId
		)
	{
		GameObject * Object;

		if ((Object = GetGameObject( ObjectId )) == NULL)
			return NULL;

		return Object->AsArea( );
	}

};


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
