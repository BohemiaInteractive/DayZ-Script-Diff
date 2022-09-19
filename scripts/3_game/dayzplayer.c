/** @file 

  this file is interface DayZPlayer classes 

*/


// *************************************************************************************
// ! DayZPlayerCameraResult - camera result -
// *************************************************************************************
class DayZPlayerCameraResult
{

	vector		m_CameraTM[4];			//!< transformation matrix - pos + orient of the camera
	float 		m_fFovMultiplier;		//!< fov multiplier - 1.0 default - used for modifying fov - 
	float 		m_fFovAbsolute;			//!< fov absolute value override - -1.0 default, if set - overrides absolute fov setting
	float		m_fNearPlane;			//!< nearplane distance
	float 		m_fPositionModelSpace;  //!< 0.0 position is in heading space, 1.0 position is in model space
	float 		m_fDistance;			//!< camera distance (external cameras only)
	float 		m_fUseHeading;			//!< 0..1 (0 uses direct dir returned, 1 uses heading from player)
	
	int 		m_iDirectBone;			//!< -1 no bone, >= 0 - bone index camera is bound to, m_CameraTM is offset to the bone 
	int 		m_iDirectBoneMode;		//! 0 not used, 1 - pos, 2 - rot, 3 - pos+rot applied as a parent to m_CameraTM, 4 as 3 but cam aligned with Y
	float 		m_fInsideCamera;		//!< 0..1 (0 normal lod, 1 inside lod), >0.7 -> inside
	bool		m_bUpdateWhenBlendOut;	//!< true - camera is updated when blending to new camera (Ironsights == false)
	float 		m_fShootFromCamera;		//!< 1(default) - uses shoot from camera (+aiming sway), 0 pure weapon shoot (ironsights == 0)
	float		m_fIgnoreParentRoll;	//!< 1 - resets base transforms roll
	float		m_fIgnoreParentPitch;	//!< 1 - resets base transforms pitch
	float		m_fIgnoreParentYaw;   //!< 1 - resets base transforms yaw
	IEntity		m_CollisionIgnoreEntity;//!< ignore entity in 3rd person camera collision solver

	//! cannot be instanced from script (always created from C++)
	private void DayZPlayerCameraResult()
	{
	}

	//! cannot be instanced from script (always created from C++)
	private void ~DayZPlayerCameraResult()
	{
	}

}

// *************************************************************************************
// ! DayZPlayerCamera - main dayz player camera implement 
// *************************************************************************************
class DayZPlayerCamera 
{

	void DayZPlayerCamera(DayZPlayer pPlayer, HumanInputController pInput)
	{
		m_pPlayer		= pPlayer;
		m_pInput 		= pInput;
	}

	//! this overrides freelok for cameras
	bool		CanFreeLook()
	{
		return true;
	}

	//! virtual callback - called when camera is created
	void 		OnActivate (DayZPlayerCamera pPrevCamera, DayZPlayerCameraResult pPrevCameraResult)
	{
	}

	//!	virtual callback - called each frame
	void 		OnUpdate(float pDt, out DayZPlayerCameraResult pOutResult)
	{
	}

	//!	helper to blend between cameras 
	//! ret[0] - up down angle
	//! ret[1] - left right angle
	//! ret[2] - roll
	vector 		GetBaseAngles()
	{
		return	"0 0 0";
	}
	
	string GetCameraName()
	{
		return "DayZPlayerCamera";
	}
	
	float GetCurrentPitch()
	{
		return -1;
	}
	
	bool IsCamera3rdRaised()
	{
		return false;
	}

	void SpawnCameraShake(float strength = 1, float radius = 2, float smoothness = 5, float radius_decay_speed = 6)
	{
		SpawnCameraShakeProper(strength, radius, smoothness, radius_decay_speed);//done this way in order to avoid duplicating default params which could cause issues under certain circumstances
	}
	
	void SpawnCameraShakeProper(float strength, float radius, float smoothness, float radius_decay_speed);
	
	//! data 
	protected 	DayZPlayer 				m_pPlayer;		//!< player camera is attached to
	protected 	HumanInputController	m_pInput;		//!< human input 
	
	//! m_fCamPosOffsetZ info, used to adjust near plane in DayZPlayerCameraIronsights now
	void SendRecoilOffsetZ(float offset) {};
}


// *************************************************************************************
// ! DayZPlayerTypeStepSoundLookupTable - virtual 
// *************************************************************************************
class DayZPlayerTypeStepSoundLookupTable
{
	SoundObjectBuilder GetSoundBuilder(int eventId, int pMovement, int pSurfaceHash, AnimBootsType pBoots)
	{
		return null;
	}
}

// *************************************************************************************
// ! DayZPlayerTypeVoiceSoundLookupTable - virtual 
// *************************************************************************************
class DayZPlayerTypeVoiceSoundLookupTable
{
	SoundObjectBuilder GetSoundBuilder(int eventId, int parameterHash)
	{
		return null;
	}
	
	NoiseParams GetNoiseParams(int eventId);
}

// *************************************************************************************
// ! DayZPlayerTypeAttachmentSoundLookupTable - virtual 
// *************************************************************************************
class DayZPlayerTypeAttachmentSoundLookupTable
{
	SoundObjectBuilder GetSoundBuilder(int eventId, string slotName, int attachmentHash)
	{
		return null;
	}
}

class DayZPlayerTypeAnimTable
{
	AnimSoundEvent GetSoundEvent(int event_id)
	{
		return null;
	}
}

// *************************************************************************************
// ! VegetationSound
// *************************************************************************************

class VegetationSound
{
	private ref SoundObjectBuilder m_SoundObjectBuilder;
	private ref TIntArray m_AnimEventIds;
	
	void VegetationSound(SoundObjectBuilder soundObjectBuilder, TIntArray animEventIds)
	{
		m_SoundObjectBuilder = soundObjectBuilder;
		m_AnimEventIds = animEventIds;
	}
	
	SoundObjectBuilder GetSoundObjectBuilder()
	{
		return m_SoundObjectBuilder;
	}
	
	TIntArray GetAnimEventIds()
	{
		return m_AnimEventIds;
	}
}

// *************************************************************************************
// ! DayZPlayerType - DayZPlayer configuration 
// *************************************************************************************

//! class that configures static data for DayZPlayer
//! on each mission start DayZPlayerTypeCreate(DayZPlayerType pType) is called - can call methods

class DayZPlayerType
{
	//!----------------------------------------------------
	// item in hands config

	//! 
	proto native 	void SetDefaultItemInHandsProfile(string pAnimInstanceName, HumanItemBehaviorCfg pBehaviorCfg);


	//! reset profiles 
	proto native 	void ResetItemInHandsProfiles();

	//! adds setup profile for items of class pItemClass
	//! when item is in hands - pAnimInstanceName is used for animation sourcing 
	//! pCanRaise is whether weapon / item can be raised
	//! pRHandIK == false -> rhand ik is always turned off
	//! pLHandIK == false -> lhand ik is always turned off

	//! VKOSTIK: DO NOT FORGET !!! HumanItemBehaviorCfg.m_iPerItemCameraUD - per item camera user data - enables you setup different cameras for different items in hands

	//!
	proto native 	int AddItemInHandsProfile(string pItemClass, string pAnimInstanceName, HumanItemBehaviorCfg pBehaviorCfg);

	//!
	proto native 	int AddItemInHandsProfileIK(string pItemClass, string pAnimInstanceName, HumanItemBehaviorCfg pBehaviorCfg, string pIkPoseAnim, string pWeaponStates = "");

	//! add bone remap for item class
	//! pBoneRemap has always 2x N members
	//! bone in item's P3D first, bone in Character skeleton second

	//! array<string> remap = { "bolt", "Weapon_Bolt", "magazine", "Weapon_Magazine", "trigger", "Weapon_Trigger" };
	//! AddItemBoneRemap("class", remap);

	proto native 	int AddItemBoneRemap(string pItemClass, array<string> pBoneRemap);


	//! debug / hack - loads all animation instances
	proto native 	void DebugPreloadAnimationInstances();



	//!----------------------------------------------------
	// camera creators

	//! resets camera creators
	proto native 	void ResetCameraCreators();

	//! registers camera creator - camera type => camera type (must be descendant of DayZPlayerCamera)
	proto native 	void RegisterCameraCreator(int pCameraID, typename pCameraType);

	//! registers camera on change function - static function 
	proto native 	void RegisterCameraOnChangeFnStatic(typename pClass, string pFnName);

	//! registers camera on change function - 
	proto native 	void RegisterCameraOnChangeFn(Class pInstance, string pFnName);


	//!----------------------------------------------------
	// global options

	//! sets look limits for a player
	proto native 	void SetLookLimits(float pDown, float pUp, float pLeft, float pRight);

	//! sets aim limits for a player
	proto native 	void SetAimLimits(float pDown, float pUp, float pLeft, float pRight);
	
	//! sets vertical minimum aim limit for a player
	proto native 	void SetVerticalMinimumAimLimit(float value);

	//! sets aim limits for a player
	proto native 	void SetCameraShootParams(float pRayDistance, float pRayRadius, float pMaxAngleCos);

	//!----------------------------------------------------
	// bone indices

	proto native	int  GetHeadBoneIdx();

	//!----------------------------------------------------
	// event handling

	//! calls DayZPlayer.OnStepEvent();
	proto native	void RegisterStepEvent(string pEventType, float pFilterTimeout);

	//! calls DayZPlayer.OnSoundEvent();
	proto native	void RegisterSoundEvent(string pEventType, float pFilterTimeout);

	//! calls DayZPlayer.OnParticleEvent();
	proto native	void RegisterParticleEvent(string pEventType, float pFilterTimeout);


	void 				 RegisterStepSoundLookupTable(DayZPlayerTypeStepSoundLookupTable pSSLUT)
	{
		m_pStepSoundLookupTable = pSSLUT;
	}
	
	DayZPlayerTypeStepSoundLookupTable	GetStepSoundLookupTable()
	{
		return m_pStepSoundLookupTable;
	}
	
	void RegisterAttachmentSoundLookupTable(DayZPlayerTypeAttachmentSoundLookupTable pASLUT)
	{
		m_pAttachmentSoundLookupTable = pASLUT;
	}
	
	DayZPlayerTypeAttachmentSoundLookupTable	GetAttachmentSoundLookupTable()
	{
		return m_pAttachmentSoundLookupTable;
	}
	
	void RegisterVoiceSoundLookupTable(DayZPlayerTypeVoiceSoundLookupTable pASLUT)
	{
		m_pVoiceSoundLookupTable = pASLUT;
	}
	
	DayZPlayerTypeVoiceSoundLookupTable	GetVoiceSoundLookupTable()
	{
		return m_pVoiceSoundLookupTable;
	}
	
	void RegisterSoundTable(DayZPlayerTypeAnimTable pST)
	{
		m_pSoundTable = pST;
	}
	
	DayZPlayerTypeAnimTable GetSoundTable()
	{
		return m_pSoundTable;
	}
	
	/*
	void RegisterSoundVoiceTable(DayZPlayerTypeAnimTable pVST)
	{
		m_pSoundVoiceTable = pVST;
	}
	
	DayZPlayerTypeAnimTable GetSoundVoiceTable()
	{
		return m_pSoundVoiceTable;
	}
	*/
	
	array<ref VegetationSound> GetVegetationSounds()
	{
		return m_pVegetationSounds;
	}
	
	NoiseParams GetNoiseParamsStand()
	{
		return m_pNoiseStepStand;
	}
	
	NoiseParams GetNoiseParamsCrouch()
	{
		return m_pNoiseStepCrouch;
	}
	
	NoiseParams GetNoiseParamsProne()
	{
		return m_pNoiseStepProne;
	}
	
	NoiseParams GetNoiseParamsLandLight()
	{
		return m_pNoiseLandLight;	
	}
	
	NoiseParams GetNoiseParamsLandHeavy()
	{
		return m_pNoiseLandHeavy;
	}
	
	NoiseParams GetNoiseParamsWhisper()
	{
		return m_pNoiseWhisper;	
	}
	
	NoiseParams GetNoiseParamsTalk()
	{
		return m_pNoiseTalk;	
	}
	
	NoiseParams GetNoiseParamsShout()
	{
		return m_pNoiseShout;	
	}
	
	void LoadSoundWeaponEvent()
	{
		string cfgPath = "CfgVehicles SurvivorBase AnimEvents SoundWeapon ";
		
		m_animSoundEventsAttack = new array<ref AnimSoundEvent>;

		int soundCount = GetGame().ConfigGetChildrenCount(cfgPath);
		for(int i = 0; i < soundCount; i++)
		{
			string soundName;
			GetGame().ConfigGetChildName(cfgPath, i, soundName);
			string soundPath = cfgPath + soundName + " ";
			AnimSoundEvent soundEvent = new AnimSoundEvent(soundPath);
			if(soundEvent.IsValid())
				m_animSoundEventsAttack.Insert(soundEvent);
		}
	}
	
	void LoadVegetationSounds()
	{
		m_pVegetationSounds = new array<ref VegetationSound>;

		string vegSoundsCfgPath = "CfgVehicles SurvivorBase VegetationSounds ";
		int vegSoundsCount = GetGame().ConfigGetChildrenCount(vegSoundsCfgPath);

		for (int v = 0; v < vegSoundsCount; ++v)
		{
			string vegSoundParamName;
			GetGame().ConfigGetChildName(vegSoundsCfgPath, v, vegSoundParamName);
			
			string soundSet = "";
			GetGame().ConfigGetText(vegSoundsCfgPath + vegSoundParamName + " soundSet", soundSet);

			TIntArray animEventIds = new TIntArray;
			GetGame().ConfigGetIntArray(vegSoundsCfgPath + vegSoundParamName + " animEventIds", animEventIds);

			SoundParams soundParams = new SoundParams(soundSet);

			if (soundParams.IsValid() && animEventIds.Count() > 0)
			{
				m_pVegetationSounds.Insert(new VegetationSound(new SoundObjectBuilder(soundParams), animEventIds));
			}
		}
	}
	
	AnimSoundEvent GetSoundWeaponEvent(int event_id)
	{
		for(int i = 0; i < m_animSoundEventsAttack.Count(); i++)
		{
			AnimSoundEvent soundEvent = m_animSoundEventsAttack.Get(i);
			if(soundEvent.m_iID == event_id)
			{
				return soundEvent;
			}
		}

		return NULL;
	}
	
	//! register hit components for AI melee (used by attacking AI)
	void RegisterHitComponentsForAI()
	{
		m_HitComponentsForAI = new array<ref DayZAIHitComponent>;

		//! registers default hit compoent for entity
		m_DefaultHitComponent = "dmgZone_torso";
		//! registers default hit position for entity
		m_DefaultHitPositionComponent = "Pelvis";

		//! list of components suitable for melee finisher attacks (used in fight logic)
		m_SuitableFinisherHitComponents = new array<string>;
		m_SuitableFinisherHitComponents.Insert("Head");

		//! register hit components that are selected by probability
		//DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "dmgZone_head", 5); // TMP comment out
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "dmgZone_leftArm", 50);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "dmgZone_torso", 65);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "dmgZone_rightArm", 50);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "dmgZone_leftLeg", 40);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "dmgZone_rightLeg", 40);
	}
	
	string GetHitComponentForAI()
	{
		string hitComp;
		
		if (DayZAIHitComponentHelpers.SelectMostProbableHitComponent(m_HitComponentsForAI, hitComp))
		{
			return hitComp;
		}
		
		return GetDefaultHitComponent();
	}
	
	string GetDefaultHitComponent()
	{
		return m_DefaultHitComponent;
	}
	
	string GetDefaultHitPositionComponent()
	{
		return m_DefaultHitPositionComponent;
	}
	
	array<string> GetSuitableFinisherHitComponents()
	{
		return m_SuitableFinisherHitComponents;
	}
	
	private void DayZPlayerType()
	{
		string cfgPath = "CfgVehicles SurvivorBase ";
	
		m_pNoiseStepStand = new NoiseParams();
		m_pNoiseStepStand.LoadFromPath(cfgPath + "NoiseStepStand");
		
		m_pNoiseStepCrouch = new NoiseParams();
		m_pNoiseStepCrouch.LoadFromPath(cfgPath + "NoiseStepCrouch");
		
		m_pNoiseStepProne = new NoiseParams();
		m_pNoiseStepProne.LoadFromPath(cfgPath + "NoiseStepProne");
		
		m_pNoiseLandLight = new NoiseParams();
		m_pNoiseLandLight.LoadFromPath(cfgPath + "NoiseLandLight");
		
		m_pNoiseLandHeavy = new NoiseParams();
		m_pNoiseLandHeavy.LoadFromPath(cfgPath + "NoiseLandHeavy");
		
		m_pNoiseWhisper = new NoiseParams();
		m_pNoiseWhisper.LoadFromPath(cfgPath + "NoiseWhisper");
		
		m_pNoiseTalk = new NoiseParams();
		m_pNoiseTalk.LoadFromPath(cfgPath + "NoiseTalk");
		
		m_pNoiseShout = new NoiseParams();
		m_pNoiseShout.LoadFromPath(cfgPath + "NoiseShout");
		
		LoadSoundWeaponEvent();
		LoadVegetationSounds();
	}
	
	private void ~DayZPlayerType();

	//!----------------------------------------------------
	// global settings

	//! returns Human global settings for DayZPlayer
	proto native 	SHumanGlobalSettings 		GetGlobalSettingsW();


	//!----------------------------------------------------
	// command configs

	// returns command move setting for write - in init phase
	proto native 	SHumanCommandMoveSettings	CommandMoveSettingsW();

	// returns command swim setting for write - in init phase
	proto native 	SHumanCommandSwimSettings	CommandSwimSettingsW();

	// returns command swim setting for write - in init phase
	proto native 	SHumanCommandClimbSettings	CommandClimbSettingsW();


	//!< this is used for getting appropriate sound effect to play step event on various surfaces && boots
	//!< it's downcasted to StepSoundLookupTable
	ref DayZPlayerTypeStepSoundLookupTable m_pStepSoundLookupTable;
	ref DayZPlayerTypeAttachmentSoundLookupTable m_pAttachmentSoundLookupTable;
	ref DayZPlayerTypeVoiceSoundLookupTable m_pVoiceSoundLookupTable;
	ref DayZPlayerTypeAnimTable m_pSoundTable;
	//ref DayZPlayerTypeAnimTable m_pSoundVoiceTable;
	ref NoiseParams m_pNoiseStepStand;
	ref NoiseParams m_pNoiseStepCrouch;
	ref NoiseParams m_pNoiseStepProne;
	ref NoiseParams m_pNoiseLandLight;
	ref NoiseParams m_pNoiseLandHeavy;
	
	// von noises
	ref NoiseParams m_pNoiseWhisper;
	ref NoiseParams m_pNoiseTalk;
	ref NoiseParams m_pNoiseShout;

	ref array<ref VegetationSound> m_pVegetationSounds;

	//! Melee hit components (AI targeting)	
	protected ref array<ref DayZAIHitComponent> m_HitComponentsForAI;
	protected string m_DefaultHitComponent;
	protected string m_DefaultHitPositionComponent;

	protected ref array<string> m_SuitableFinisherHitComponents;
	
	ref array<ref AnimSoundEvent> m_animSoundEventsAttack;
}


// *************************************************************************************
// ! DayZPlayerConstants - dayz player constants 
// *************************************************************************************
//! defined in C++
enum DayZPlayerConstants
{
    //! ---------------------------------------------------------
    //! ---------------------- movement stances -----------------
    //! ---------------------------------------------------------
	DEBUG_MENU,				//!< dayz player debug menu in enfusion 
	DEBUG_TURNSLIDE,		//!< over turn slides / clamps
	DEBUG_SHOWDEBUGPLUGIN,  //!< menu for showing debug plugin
	DEBUG_SHOWINJURY,		//!< menu for showing injuries
	DEBUG_SHOWEXHAUSTION,	//!< menu for showing exhaustion
	DEBUG_ENABLEJUMP,		//!< menu for showing exhaustion
	DEBUG_ENABLETALKING,	//!< option for showing talk command
	

    //! ---------------------------------------------------------
    //! ---------------------- movement stances -----------------
    //! ---------------------------------------------------------

	STANCEIDX_ERECT,
	STANCEIDX_CROUCH,
	STANCEIDX_PRONE,
	STANCEIDX_RAISEDERECT,
	STANCEIDX_RAISEDCROUCH,
	STANCEIDX_RAISEDPRONE,


	//! stance masks 
	STANCEMASK_ERECT,
	STANCEMASK_CROUCH,
	STANCEMASK_PRONE,
	STANCEMASK_RAISEDERECT,
	STANCEMASK_RAISEDCROUCH,
	STANCEMASK_RAISEDPRONE,
	STANCEMASK_ALL,		// STANCEMASK_ERECT | STANCEMASK_CROUCH | STANCEMASK_PRONE | STANCEMASK_RAISEDERECT | STANCEMASK_RAISEDCROUCH | STANCEMASK_RAISEDPRONE
	STANCEMASK_NOTRAISED 	= STANCEMASK_ERECT | STANCEMASK_CROUCH | STANCEMASK_PRONE,
	STANCEMASK_RAISED 		= STANCEMASK_RAISEDERECT | STANCEMASK_RAISEDCROUCH | STANCEMASK_RAISEDPRONE,


	//! movements - masks 
	MOVEMENT_IDLE,  			//! 0x1
	MOVEMENT_WALK,				//! 0x2
	MOVEMENT_RUN,				//! 0x4
	MOVEMENT_SPRINT,			//! 0x8 

	//! rotation modes
	
	ROTATION_DISABLE,			//!
	ROTATION_ENABLE,			//!
	
	//! movement idx
	MOVEMENTIDX_SLIDE	= -2,
	MOVEMENTIDX_IDLE	= 0,
	MOVEMENTIDX_WALK	= 1,
	MOVEMENTIDX_RUN		= 2,
	MOVEMENTIDX_SPRINT  = 3,
	MOVEMENTIDX_CROUCH_RUN = 4,
	
	//! melee hit type
	MELEE_LIGHT = 0,
	MELEE_HEAVY = 1,
	

	//! vehicle classes
	VEHICLECLASS_CAR,
	VEHICLECLASS_HELI,

	//! vehicle seats
	VEHICLESEAT_DRIVER,
	VEHICLESEAT_CODRIVER,
	VEHICLESEAT_PASSENGER_L,
	VEHICLESEAT_PASSENGER_R,

    //! ---------------------------------------------------------
    //! ---------------------- COMMAND IDs ----------------------
    //! ---------------------------------------------------------
	
	//! command ids - main movement commands 
	COMMANDID_NONE,    		// type is int - no command - invalid state 
	COMMANDID_MOVE,			// type is int (overridden from C++) - normal movement (idle, walk, run, sprint, ... )
	COMMANDID_ACTION,		// type is int (overridden from C++) - full body action
	COMMANDID_MELEE,		// type is int (overridden from C++) - melee attacks
	COMMANDID_MELEE2,		// type is int (overridden from C++) - melee attacks
	COMMANDID_FALL,			// type is int (overridden from C++) - falling
	COMMANDID_DEATH,		// type is int (overridden from C++) - dead 
	COMMANDID_DAMAGE,		// type is int (overridden from C++) - fullbody damage
	COMMANDID_LADDER,		// type is int (overridden from C++) - ladder
	COMMANDID_UNCONSCIOUS,	// type is int (overridden from C++) - unconscious
	COMMANDID_SWIM,			// type is int (overridden from C++) - swimming
	COMMANDID_VEHICLE,		// type is int (overridden from C++) - vehicle
	COMMANDID_CLIMB,		// type is int (overridden from C++) - climb
	COMMANDID_SCRIPT,		// type is int (overridden from C++) - all scripted commands


	//! modifier commands - additive behaviour 
	COMMANDID_MOD_LOOKAT,    	// look at - always on 
	COMMANDID_MOD_WEAPONS,   	// weapons - always on 
    COMMANDID_MOD_ACTION,		// action - additive action 
	COMMANDID_MOD_DAMAGE,		// damage - additive damage 




    //! ---------------------------------------------------------
    //! ---------------------- ACTIONS --------------------------
    //! ---------------------------------------------------------

	//! internal action commands used in HumanCommandActionCallback.InternalCommand()
	CMD_ACTIONINT_INTERRUPT				= -2,		//!< totally interrupt action (no animation, just blend)
	CMD_ACTIONINT_FINISH				= -1,		//!< secondary ending (finishing action, eg running out of water while drinking, not all actions have this)
	CMD_ACTIONINT_END					= 0,		//!< end action (stopping action, without finish, all actions have this)
	CMD_ACTIONINT_ACTION				= 1,		//!< one time secondary action within an action
	CMD_ACTIONINT_ACTIONLOOP			= 2,		//!< loop secondary action within an action


	//! animations constants 

	//! note: (erc - standing, cro - crouching, pne - prone)    	anim pose it can start from
	//! note: (end, end2, action, loop2)							internal command which can be send in loop state 

	//! looping 	
		
    //! --------------------------
    //! modifier (differential animation)
    CMD_ACTIONMOD_DRINK					= 0,		// erc,cro    		[end, end2]
	CMD_ACTIONMOD_EAT					= 1,		// erc,cro    		[end, end2]
	CMD_ACTIONMOD_EMPTY_VESSEL			= 2,		// erc,cro          [end]
	CMD_ACTIONMOD_CATCHRAIN				= 3,		// erc,cro			[end]
	CMD_ACTIONMOD_VIEWCOMPASS			= 7,		// erc,cro          [end]
	CMD_ACTIONMOD_ITEM_TUNE				= 9,		// erc,cro			[end]
	CMD_ACTIONMOD_GIVEL					= 10,		// erc,cro			[end]
	CMD_ACTIONMOD_GIVER					= 11,		// erc,cro			[end]
	CMD_ACTIONMOD_SHAVE					= 12,		// erc,cro			[end]
	CMD_ACTIONMOD_FILLMAG				= 13,		// erc,cro			[end]
	CMD_ACTIONMOD_EMPTYMAG				= 14,		// erc,cro			[end]
	CMD_ACTIONMOD_OPENITEM				= 15,		// erc,cro			[end]
	CMD_ACTIONMOD_TAKETEMPSELF			= 18,		// erc,cro			[end]
	CMD_ACTIONMOD_VIEWMAP				= 19,		// erc,cro			[end]
	CMD_ACTIONMOD_RAISEITEM				= 20,		// erc,cro			[end]
	CMD_ACTIONMOD_SEARCHINVENTORY		= 21,		// erc,cro			[end]
	CMD_ACTIONMOD_CRAFTING				= 22,		// erc,cro			[end]
	CMD_ACTIONMOD_RESTRAINEDSTRUGGLE	= 23,		// erc,cro 			[end, end2]
	CMD_ACTIONMOD_COVERHEAD_SELF		= 24,		// erc,cro 			[end, end2]
	CMD_ACTIONMOD_COVERHEAD_TARGET		= 25,		// erc,cro 			[end, end2]
	CMD_ACTIONMOD_SET_ALARM				= 250,
	CMD_ACTIONMOD_SET_KITCHENTIMER		= 252,
	CMD_ACTIONMOD_RESET_KITCHENTIMER	= 253,
	
	// onetime 
	CMD_ACTIONMOD_PICKUP_HANDS			= 500,		// erc,cro
	CMD_ACTIONMOD_PICKUP_INVENTORY		= 501,		// erc,cro
	CMD_ACTIONMOD_LICKBATTERY			= 502,		// erc,cro
	CMD_ACTIONMOD_LIGHTFLARE			= 503,		// erc,cro
	CMD_ACTIONMOD_LITCHEMLIGHT			= 504,		// erc,cro
	CMD_ACTIONMOD_UNPINGRENAGE			= 505,		// erc,cro
	CMD_ACTIONMOD_OPENDOORFW			= 506,		// erc,cro
	CMD_ACTIONMOD_OPENLID				= 507,		// erc,cro 
	CMD_ACTIONMOD_CLOSELID				= 508,		// erc,cro 	
	CMD_ACTIONMOD_ITEM_ON				= 509,		// erc,cro
	CMD_ACTIONMOD_ITEM_OFF				= 510,		// erc,cro
	CMD_ACTIONMOD_BATONEXTEND			= 511,		// erc,cro
	CMD_ACTIONMOD_BATONRETRACT			= 512,		// erc,cro
	CMD_ACTIONMOD_UNLOCKHANDCUFFTARGET	= 513,		// erc,cro
	CMD_ACTIONMOD_FISHINGRODEXTEND		= 514,		// erc,cro
	CMD_ACTIONMOD_FISHINGRODRETRACT		= 515,		// erc,cro
	CMD_ACTIONMOD_CLEANHANDSBOTTLE		= 516,		// erc,cro
	CMD_ACTIONMOD_OPENITEM_ONCE			= 517,		// erc,cro
	CMD_ACTIONMOD_ATTACHSCOPE			= 518,		// erc,cro
	CMD_ACTIONMOD_ATTACHBARREL			= 519,		// erc,cro
	CMD_ACTIONMOD_EMPTYSEEDSPACK		= 520,		// erc,cro
	CMD_ACTIONMOD_INTERACTONCE			= 521,		// erc,cro
	CMD_ACTIONMOD_ATTACHITEM			= 522,		// erc,cro
	CMD_ACTIONMOD_CLOSEITEM_ONCE		= 523,		// erc,cro,pne
	CMD_ACTIONMOD_FOLDITEM_ONCE			= 524,		// erc,cro
	CMD_ACTIONMOD_UNFOLDITEM_ONCE		= 525,		// erc,cro
	CMD_ACTIONMOD_PRESS_TRIGGER			= 526,		// erc,cro
	CMD_ACTIONMOD_STOP_ALARM			= 251,

	CMD_ACTIONMOD_DROPITEM_HANDS		= 900,		// erc, cro
	CMD_ACTIONMOD_DROPITEM_INVENTORY	= 901,		// erc, cro
	
	

    //! --------------------------
    //! fb (full body)
    CMD_ACTIONFB_DRINK					= 0,		// pne				[end, end2]
	CMD_ACTIONFB_EAT					= 1,		// pne				[end, end2]
	CMD_ACTIONFB_CATCHRAIN				= 3,		// pne				[end]
	CMD_ACTIONFB_ITEM_TUNE				= 9,		// pne				[end]
	CMD_ACTIONFB_GIVEL					= 10,		// pne				[end]
	CMD_ACTIONFB_GIVER					= 11,		// pne				[end]
	CMD_ACTIONFB_FILLMAG				= 13,		// pne				[end]
	CMD_ACTIONFB_EMPTYMAG				= 14,		// pne				[end]
	CMD_ACTIONFB_DRINKPOND				= 50,		// cro 	      	    [end]
	CMD_ACTIONFB_DRINKWELL				= 51,		// cro 	            [end]
	CMD_ACTIONFB_FILLBOTTLEWELL			= 52,		// cro	            [end]
	CMD_ACTIONFB_FIREESTINGUISHER		= 53,		// erc              [end]
	CMD_ACTIONFB_WRING					= 54,		// cro 		        [end]
	CMD_ACTIONFB_FISHING				= 56,		// cro			    [action (check fish), end(catch fish), end2(not catching anything ]
	CMD_ACTIONFB_CPR					= 57,		// cro     		    [end]
	CMD_ACTIONFB_BANDAGE				= 58,		// cro		        [end]
	CMD_ACTIONFB_CRAFTING				= 59,		// cro		        [end]
	CMD_ACTIONFB_INTERACT				= 60,		// erc,cro			[end]
	CMD_ACTIONFB_FORCEFEED				= 62,		// erc,cro          [end]
	CMD_ACTIONFB_BANDAGETARGET			= 63,		// erc,cro          [end]
	CMD_ACTIONFB_SPRAYPLANT				= 64,		// cro     		    [end]
	CMD_ACTIONFB_STARTFIRE				= 65,		// cro			    [end]
	CMD_ACTIONFB_ANIMALSKINNING			= 66,		// cro				[end]
	CMD_ACTIONFB_WASHHANDSWELL			= 67,		// cro				[end]
	CMD_ACTIONFB_WASHHANDSPOND			= 68,		// cro				[end]
	CMD_ACTIONFB_SALINEBLOODBAGTARGET	= 69,		// erc,cro			[end]
	CMD_ACTIONFB_SALINEBLOODBAG			= 70,		// erc,cro			[end]
	CMD_ACTIONFB_STITCHUPSELF			= 71,		// erc,cro			[end]
	CMD_ACTIONFB_VOMIT					= 72,		// cro				[end]
	CMD_ACTIONFB_UNRESTRAINTARGET		= 73,		// erc,cro			[end (finish cutting), end2 (cancel cutting)]
	CMD_ACTIONFB_RESTRAINTARGET			= 74,		// erc,cro			[end (finish tying up), end2 (cancel tying up)]
	CMD_ACTIONFB_CHECKPULSE				= 76,		// cro				[end]
	CMD_ACTIONFB_CLEANWOUNDSTARGET		= 78,		// erc, cro			[end]
	CMD_ACTIONFB_COLLECTBLOODSELF		= 81,		// erc, cro			[end]
	CMD_ACTIONFB_EMPTY_VESSEL			= 82,		// erc, cro			[end]
	CMD_ACTIONFB_OPENITEM				= 83,		// pne				[end]
	CMD_ACTIONFB_HACKBUSH				= 85,		// erc				[end, end2]
	CMD_ACTIONFB_HACKTREE				= 86,		// erc				[end, end2]
	CMD_ACTIONFB_TAKETEMPSELF			= 87,		// pne				[end]
	CMD_ACTIONFB_DIG					= 88,		// erc				[end, end2]
	CMD_ACTIONFB_DIGUPCACHE				= 89,		// erc				[end, end2]
	CMD_ACTIONFB_DIGMANIPULATE			= 90,		// erc				[end, end2]
	CMD_ACTIONFB_DEPLOY_HEAVY			= 95,		// erc				[end, end2]
	CMD_ACTIONFB_DEPLOY_2HD				= 96,		// cro				[end, end2]
	CMD_ACTIONFB_DEPLOY_1HD				= 97,		// cro				[end, end2]
	CMD_ACTIONFB_BLOWFIREPLACE			= 98,		// cro				[end]
	CMD_ACTIONFB_VIEWMAP				= 99,		// pne				[end]
	CMD_ACTIONFB_VIEWCOMPASS			= 100,		// pne				[end]
	CMD_ACTIONFB_FILLBOTTLEPOND			= 101,		// erc, cro			[end]
	CMD_ACTIONFB_PLACING_HEAVY			= 102,		// erc				[end, end2]
	CMD_ACTIONFB_PLACING_2HD			= 103,		// cro				[end, end2]
	CMD_ACTIONFB_PLACING_1HD			= 104,		// cro				[end, end2]
	CMD_ACTIONFB_CUTBARK				= 105,		// erc,cro			[end, end2]
	CMD_ACTIONFB_VIEWNOTE				= 106,		// erc,cro,pne		[end]
	CMD_ACTIONFB_SEARCHINVENTORY		= 107,		// pne				[end]
	CMD_GESTUREFB_LOOKOPTICS			= 108,		// erc,cro,pne		[end]
	CMD_ACTIONFB_MINEROCK				= 109,		// erc				[end, end2]
	CMD_ACTIONFB_RAISEITEM				= 110,		// pne 				[end]
	CMD_ACTIONFB_RESTRAINEDSTRUGGLE		= 111,		// ,pne 			[end, end2]
	CMD_ACTIONFB_RESTRAINSELF			= 112,		// erc,cro 			[end]
	CMD_ACTIONFB_ASSEMBLE				= 113,		// erc,cro			[end, end2]
	CMD_ACTIONFB_DISASSEMBLE			= 114,		// erc,cro			[end, end2]
	CMD_ACTIONFB_FLAME_REPAIR			= 115,		// erc, cro	
	CMD_ACTIONFB_TURN_VALVE				= 116,		// erc
	CMD_ACTIONFB_SET_ALARM				= 250,		// erc,cro			??not sure
	
	// onetime 
	CMD_ACTIONFB_PICKUP_HANDS			= 500,		// pne
	CMD_ACTIONFB_PICKUP_INVENTORY		= 501,		// pne
	CMD_ACTIONFB_LICKBATTERY			= 502,		// pne
	CMD_ACTIONFB_LIGHTFLARE				= 503,		// pne
	CMD_ACTIONFB_LITCHEMLIGHT			= 504,		// pne
	CMD_ACTIONFB_UNPINGRENAGE			= 505,		// pne
	CMD_ACTIONFB_ITEM_ON				= 506,		// pne
	CMD_ACTIONFB_ITEM_OFF				= 507,		// pne
	CMD_ACTIONFB_HANDCUFFTARGET			= 508,		// erc,cro
	CMD_ACTIONFB_MORPHINE				= 509,		// cro
	CMD_ACTIONFB_INJECTION				= 510,		// cro
	CMD_ACTIONFB_INJECTIONTARGET		= 511,		// erc,cro
	CMD_ACTIONFB_DRINKSIP				= 512,		// cro
	CMD_ACTIONFB_CLEANHANDSBOTTLE		= 513,		// pne
	CMD_ACTIONFB_OPENITEM_ONCE			= 514,		// pne
	CMD_ACTIONFB_POKE					= 515,		// cro
	CMD_ACTIONFB_ATTACHSCOPE			= 516,		// pne
	CMD_ACTIONFB_ATTACHBARREL			= 517,		// pne
	CMD_ACTIONFB_RESTRAIN				= 518,		// erc,cro,pne
	CMD_ACTIONFB_PICKUP_HEAVY			= 519,		// erc
	CMD_ACTIONFB_PRESS_TRIGGER			= 526,		// pne
	CMD_ACTIONFB_RAISE_FLAG				= 600,		// erc?
	CMD_ACTIONFB_LOWER_FLAG				= 601,		// erc?
	CMD_ACTIONFB_STOP_ALARM				= 251,

	CMD_ACTIONFB_DROPITEM_HANDS		= 900,			// pne, pne back
	
    
    //! ---------------------------------------------------------
    //! ---------------------- VEHICLES -------------------------
    //! ---------------------------------------------------------
	
	// looping actions
	CMD_ACTIONMOD_STARTENGINE			= 300,    		// [end]
	CMD_ACTIONMOD_TOOTHORN				= 301,    		// [end]
	
	// onetime actions
	CMD_ACTIONMOD_DRIVER_DOOR_OPEN		= 400,
	CMD_ACTIONMOD_DRIVER_DOOR_CLOSE		= 401,
	CMD_ACTIONMOD_CODRIVER_DOOROPEN		= 402,
	CMD_ACTIONMOD_CODRIVER_DOORCLOSE	= 403,
	CMD_ACTIONMOD_STOPENGINE			= 404,
	CMD_ACTIONMOD_SHIFTGEAR				= 405,
	CMD_ACTIONMOD_HEADLIGHT				= 406,
	
	
	
    
    //! ---------------------------------------------------------
    //! ---------------------- GESTURES -------------------------
    //! ---------------------------------------------------------
    //! modifier (differential animation)
    CMD_GESTUREMOD_GREETING				= 1000,			// erc,cro	 	    [end]
    CMD_GESTUREMOD_POINT				= 1001,			// erc,cro	 	    [end]
    CMD_GESTUREMOD_THUMB				= 1002,			// erc,cro	 	    [end, CMD_ACTIONINT_ACTION to switch to THUMB DOWN]
    CMD_GESTUREMOD_THUMBDOWN			= 1003,			// erc,cro	 	    [end, CMD_ACTIONINT_ACTION to switch to THUMB UP]
    CMD_GESTUREMOD_SILENCE				= 1004,			// erc,cro	 	    [end]
    CMD_GESTUREMOD_TAUNT				= 1005,			// erc,cro	 	    [end]
    CMD_GESTUREMOD_TIMEOUT				= 1006,			// erc,cro	 	    [end]
    CMD_GESTUREMOD_HEART				= 1007,			// erc,cro	 	    [end]
    CMD_GESTUREMOD_FACEPALM				= 1008,			// erc,cro   	    [end]
    CMD_GESTUREMOD_WATCHING				= 1009,			// erc,cro	 	    [end]
    CMD_GESTUREMOD_HOLD					= 1010,			// erc,cro	 	    [end]
    CMD_GESTUREMOD_LISTENING			= 1011,			// erc,cro	 	    [end]
    CMD_GESTUREMOD_POINTSELF			= 1012,			// erc,cro	 	    [end]
    CMD_GESTUREMOD_LOOKATME				= 1013,			// erc,cro	 	    [end]
    CMD_GESTUREMOD_OKAY					= 1014,			// erc,cro	 	    [end]

    CMD_GESTUREMOD_RPS					= 10000,		// erc 	            [SPECIAL, SEE BELOW]

    CMD_ACTIONINT_RPS_ROCK				= 10,	    	// RPS Internal Gesture
    CMD_ACTIONINT_RPS_PAPER				= 11,			// RPS Internal Gesture
    CMD_ACTIONINT_RPS_SCISSORS			= 12,			// RPS Internal Gesture
	
	
    // onetime 		
    CMD_GESTUREMOD_THROAT				= 1100,			// erc,cro
    CMD_GESTUREMOD_CLAP					= 1101,			// erc,cro
    CMD_GESTUREMOD_DABBING				= 1102,			// erc,cro
    CMD_GESTUREMOD_MOVE					= 1103,			// erc,cro
    CMD_GESTUREMOD_DOWN					= 1104,			// erc,cro
    CMD_GESTUREMOD_COME					= 1105,			// erc,cro
    CMD_GESTUREMOD_TAUNTKISS			= 1106,			// erc,cro
    CMD_GESTUREMOD_TAUNTELBOW			= 1107,			// erc,cro
    CMD_GESTUREMOD_TAUNTTHINK			= 1108,			// erc,cro
    CMD_GESTUREMOD_NODHEAD				= 1109,			// erc,cro
    CMD_GESTUREMOD_SHAKEHEAD			= 1110,			// erc,cro
    CMD_GESTUREMOD_SHRUG				= 1111,			// erc,cro
    CMD_GESTUREMOD_SURRENDER			= 1112,			// erc
    CMD_GESTUREMOD_SURRENDERIN			= 1112,			// erc
    CMD_GESTUREMOD_SURRENDEROUT			= 1113,			// erc
	
    //! --------------------------
    //! fb (full body)
    CMD_GESTUREFB_GREETING				= 1000,			// pne 	    [end]
    CMD_GESTUREFB_POINT					= 1001,			// pne 	    [end]
    CMD_GESTUREFB_THUMB					= 1002,			// pne 	    [end, CMD_ACTIONINT_ACTION to switch to THUMB DOWN]
    CMD_GESTUREFB_THUMBDOWN				= 1003,			// pne 	    [end, CMD_ACTIONINT_ACTION to switch to THUMB UP]
    CMD_GESTUREFB_SILENCE				= 1004,			// pne 	    [end]
    CMD_GESTUREFB_TAUNT					= 1005,			// pne 	    [end]
    CMD_GESTUREFB_TIMEOUT				= 1006,			// pne 	    [end]
    CMD_GESTUREFB_HEART					= 1007,			// pne 	    [end]
    CMD_GESTUREFB_WATCHING				= 1009,			// pne 	    [end]
    CMD_GESTUREFB_HOLD					= 1010,			// pne 	    [end]
    CMD_GESTUREFB_LISTENING				= 1011,			// pne 	    [end]
    CMD_GESTUREFB_POINTSELF				= 1012,			// pne 	    [end]
    CMD_GESTUREFB_LOOKATME				= 1013,			// pne 	    [end]
    CMD_GESTUREFB_OKAY					= 1014,			// pne 	    [end]
	
    CMD_GESTUREFB_SALUTE				= 1050,			// erc              [end]
    CMD_GESTUREFB_CAMPFIRE				= 1051,			// cro              [end]
    CMD_GESTUREFB_LYINGDOWN				= 1052,			// cro              [end]
    CMD_GESTUREFB_SOS					= 1053,			// erc              [end]
    CMD_GESTUREFB_SITA					= 1054,			// cro              [end]
    CMD_GESTUREFB_SITB					= 1055,			// cro              [end]
    CMD_GESTUREFB_DABBING				= 1056,			// erc, cro         [end]
    CMD_GESTUREFB_KNEEL					= 1057,			// cro   			[end]

    // onetime 
    CMD_GESTUREFB_THROAT				= 1100,			// pne
    CMD_GESTUREFB_MOVE					= 1103,			// pne
    CMD_GESTUREFB_DOWN					= 1104,			// pne
    CMD_GESTUREFB_COME					= 1105,			// pne
    CMD_GESTUREFB_TAUNTKISS				= 1106,			// pne
    CMD_GESTUREFB_TAUNTTHINK			= 1108,			// pne
    CMD_GESTUREFB_DANCE					= 1109,			// erc
	CMD_GESTUREFB_NODHEAD				= 1110,			// pne
	CMD_GESTUREFB_SHAKEHEAD				= 1111,			// pne
    CMD_GESTUREFB_SHRUG					= 1112,			// pne
    CMD_GESTUREFB_SURRENDER				= 1113,			// cro,pne
    CMD_GESTUREFB_SURRENDERIN			= 1113,			// cro,pne
    CMD_GESTUREFB_SURRENDEROUT			= 1114,			// cro,pne
	

    //! ---------------------------------------------------------
    //! ---------------------- TRAILER --------------------------
    //! ---------------------------------------------------------
	CMD_TRAILER_WAKE_UP 				= 3000,
	CMD_TRAILER_DEAD_BODY 				= 3001,
	CMD_TRAILER_WALKIE_TALKIE 			= 3002,
	CMD_TRAILER_WOUNDED 				= 3003,
	CMD_TRAILER_WALK_AWAY 				= 3004,
	CMD_TRAILER_DEAD 	 				= 3005,
	
    
    //! ---------------------------------------------------------
    //! ---------------------- SUICIDE --------------------------
    //! ---------------------------------------------------------
    //! fb (full body)
    CMD_SUICIDEFB_1HD					= 2000,			// cro              [end (cancel), end2 (commit)]
    CMD_SUICIDEFB_FIREAXE				= 2001,			// erc              [end (cancel), end2 (commit)]
    CMD_SUICIDEFB_PITCHFORK				= 2002,			// erc              [end (cancel), end2 (commit)]
    CMD_SUICIDEFB_PISTOL				= 2003,			// cro              [end (cancel), end2 (commit)]
    CMD_SUICIDEFB_RIFLE					= 2004,			// cro              [end (cancel), end2 (commit)]
    CMD_SUICIDEFB_SWORD					= 2005,			// erc              [end (cancel), end2 (commit)]
    CMD_SUICIDEFB_UNARMED				= 2006,			// cro              [end (cancel), end2 (commit)]
    CMD_SUICIDEFB_WOODAXE				= 2007,			// erc              [end (cancel), end2 (commit)]
    CMD_SUICIDEFB_SPEAR					= 2008,			// erc              [end (cancel), end2 (commit)]
	CMD_SUICIDEFB_SICKLE				= 2009,			// cro              [end (cancel), end2 (commit)]
	CMD_SUICIDEFB_HOE					= 2010,			// erc              [end (cancel), end2 (commit)]

	// 32767 is totaly MAXIMAL index !!!!!

};

//! defined in C++
enum DayZPlayerInstanceType
{
	INSTANCETYPE_SERVER,		//! server instance 
	INSTANCETYPE_CLIENT,		//! client instance && controlled
	INSTANCETYPE_REMOTE,		//! client instance && other player 
	
	INSTANCETYPE_AI_SERVER,				//! temporary!
	INSTANCETYPE_AI_REMOTE,				//! temporary!
	INSTANCETYPE_AI_SINGLEPLAYER,	//! temporary!
}

// *************************************************************************************
// ! DayZPlayer - mainly for logic and camera 
// *************************************************************************************
class SDayZPlayerHeadingModel
{
	int 	m_iCamMode;				//[in]		- DayZPlayerConstants.CAMERAMODE_...
	int 	m_iCurrentCommandID;    //[in]		- Current Main Command ID 
	float 	m_fOrientationAngle;	//[in/out] 	- horizontal model orientation (where you face) - in rad
	float 	m_fHeadingAngle;		//[in/out] 	- horizontal aim angle (where you aim) - in rad

	//! cannot be created from script	
	private void SDayZPlayerHeadingModel() {}
	private void ~SDayZPlayerHeadingModel() {}
}


// *************************************************************************************
// ! DayZPlayer - mainly for logic and camera 
// *************************************************************************************
class SDayZPlayerAimingModel
{
	int 	m_iCamMode;				//[in]		- DayZPlayerConstants.CAMERAMODE_... HEAD,EXTERNAL,WEAPON...
	int 	m_iCurrentCommandID;    //[in]		- Current Main Command ID 
	float	m_fCurrentAimX;			//[in]		- horizontal aim angle - in degrees
	float	m_fCurrentAimY;			//[in]		- vertical aim angle - in degrees

	float	m_fAimXCamOffset;		//[out] 	- camera (angle) offset modifier
	float	m_fAimYCamOffset;		//[out] 	- camera (angle) offset modifier
	float	m_fAimXHandsOffset;		//[out] 	- hands offset modifier
	float	m_fAimYHandsOffset;		//[out] 	- hands offset modifier
	float	m_fAimXMouseShift;		//[out]		- shift like mouse does
	float	m_fAimYMouseShift;		//[out]		- shift like mouse does
	float 	m_fCamPosOffsetX;		//[out]		- camera (position) offset modifier
	float 	m_fCamPosOffsetY;		//[out]		- camera (position) offset modifier
	float 	m_fCamPosOffsetZ;		//[out]		- camera (position) offset modifier

	//! cannot be created from script
	private void SDayZPlayerAimingModel() {}
	private void ~SDayZPlayerAimingModel() {}
}


// *************************************************************************************
// ! DayZPlayer - mainly for logic and camera 
// *************************************************************************************
class DayZPlayer extends Human
{
	const int 						SIMPLE_SELECTION_MELEE_RIFLE = 0;
	const int 						SIMPLE_SELECTION_MELEE_MELEE = 1;
	const int 						SIMPLE_SELECTION_SHOULDER_RIFLE = 2;
	const int 						SIMPLE_SELECTION_SHOULDER_MELEE = 3;
	
	//! returns appropriate DayZPlayerType
	proto native DayZPlayerType		GetDayZPlayerType();
	
	proto native DayZPlayerInstanceType 	GetInstanceType();

	//! ---------------- heading model -------------------------
	//! updated each tick
	//! this can limit / extend player's heading (orientation) behaviour in game 
	
	bool			HeadingModel(float pDt, SDayZPlayerHeadingModel pModel);	


	//! ---------------- aiming model -------------------------

	//! updated each tick - this takes care about aiming 
	
	bool			AimingModel(float pDt, SDayZPlayerAimingModel pModel);	

	
	
	//! ---------------- command handling -------------------------

	//! updated each tick 
	//! handles: starting new commands, canceling, interruption 
	//! pDt 						- delta time 
	//! pCurrentCommandID 			- actually running command's ID 
	//! pCurrentCommandFinished		- is current command finished 
	
	void 			CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished);	

	//! ---------------- camera handling -------------------------

	//! virtual 
	//! returns registered camera type
	//! pCameraMode - DayZPlayerConstants.CAMERAMODE_ ... 
	int 			CameraHandler(int pCameraMode);


	//! ---------------- camera additional functions -------------------------

	//! gets current camera 
	proto native 	DayZPlayerCamera		GetCurrentCamera();
	
	//! gets transform of current player camera
	proto native 	void 					GetCurrentCameraTransform(out vector position, out vector direction, out vector rotation);

	//! gets current camera 
	proto native 	int 					GetCurrentPerItemCameraUD();
	
	proto native	bool					IsCameraBlending();


	//! ---------------- animation graph functions -------------------------

	//! functions usable only from CommandHandler
	proto native void AnimCallCommand(int pCommand, int pParamInt, float pParamFloat);
	proto native void AnimSetFloat(int pVar, float pFlt);
	proto native void AnimSetInt(int pVar, int pInt);
	proto native void AnimSetBool(int pVar, bool pBool);

	//! ---------------- deterministic random numbers ------------------------

	/**
	\brief Random number in range of <0,0xffffffff> - !!! use this only during deterministic simulation (CommandHandler)
	\return int value in range of <0,0xffffffff>
	*/
	proto native	int						Random();
	
	/**
	\brief Random number in range of <0,pRange-1> - !!! use this only during deterministic simulation (CommandHandler)
	@param pRange upper bounds of random number
	\return int value in range of <0,pRange-1>
	*/
	proto native	float					RandomRange(int pRange);

	/**
	\brief Random number in range of <0,1> - !!! use this only during deterministic simulation (CommandHandler)
	\return float value in range of <0,1>
	*/
	proto native	float					Random01();
	
	//! returns true if player is using EyeZoom, otherwise false
	bool									IsEyeZoom();
	//! return true if shots are fired from camere, otherwise false
	bool									IsShootingFromCamera();
	//! return true if player is trying to hold breah, otherwise false
	bool									IsHoldingBreath();
	//! return true if player is currently performing FB gesture, otherwise false
	bool									IsPerformingFBGesture();
	//! return true if player is currently in 3pp, otherwise false
	bool									IsInThirdPerson();

	//! processes melee hit
	proto native	MeleeCombatData			GetMeleeCombatData();

	//! processes melee hit (uses component index)
	proto native	void					ProcessMeleeHit(InventoryItem pMeleeWeapon, int pMeleeModeIndex, Object pTarget, int pComponentIndex, vector pHitWorldPos);
	//! processes melee hit (uses component name)
	proto native	void					ProcessMeleeHitName(InventoryItem pMeleeWeapon, int pMeleeModeIndex, Object pTarget, string pComponentName, vector pHitWorldPos);
	
	//! ---------------- release controls -------------------------
	proto native	void					ReleaseNetworkControls();
	
	//! ---------------- sync stuff -------------------------
	
	proto native	void					SendSyncJuncture(int pJunctureID, ParamsWriteContext ctx);
	proto native	void					StoreInputForRemotes(ParamsWriteContext ctx);
	
	//! ---------------- sync shadow -------------------------


	//! test if entity is DayZPlayer 
	//static proto native bool				IsEntityDayZPlayer(IEntity pEntity);

	//! 
	proto native 	bool					DebugSyncShadowSetup(DayZPlayer pPlayer);



	//! ---------------- camera additiona functions -------------------------
	
	
	//! returns true if player is currently in one of the stances specified by stance mask 
	//! IsPlayerInStance(STANCEMASK_ERECT | STANCEMASK_CROUCH) returns true if player is standing or crouching and not raised (aimed)
	//! IsPlayerInStance(STANCEMASK_PRONE | STANCEIDX_RAISEDPRONE) returns true if player is in or in prone (both raised or nonraised)
	//! IsPlayerInStance(STANCEMASK_ALL) returns true always 
	//! IsPlayerInStance(STANCEMASK_RAISEDERECT | STANCEMASK_RAISEDCROUCH | STANCEMASK_RAISEDPRONE) returns true if player has raised hands
	
	// PSOVIS - move to native
	bool		IsPlayerInStance(int pStanceMask)		// STANCEMASK_ERECT | STANCEMASK_CROUCH 
	{
		HumanMovementState		state = new HumanMovementState;
		GetMovementState(state);
		
		bool ret = ((1 << state.m_iStanceIdx) & pStanceMask) != 0;
		return ret;
	}
	
	void OnInputForRemote (ParamsReadContext ctx) { }
	void OnInputFromServer (ParamsReadContext ctx) { }
	
	//! ---------------- speaking anim -------------------------
	
	//! Check if player is using VoN to speak and return max amplitude from current samples
	proto native	float	IsPlayerSpeaking();
	
	void UpdateDummyPlayerProxyVisibility(EntityAI shoulder, EntityAI melee)
	{
		/*EntityAI shoulder_item = FindAttachmentBySlotName("Shoulder");
		EntityAI melee_item = FindAttachmentBySlotName("Melee");*/
		bool boo;
		
		if ( melee )
		{
			boo = melee.IsWeapon();
			SetSimpleHiddenSelectionState(SIMPLE_SELECTION_MELEE_RIFLE,boo);
			SetSimpleHiddenSelectionState(SIMPLE_SELECTION_MELEE_MELEE,!boo);
		}
		if ( shoulder )
		{
			boo = shoulder.IsWeapon();
			SetSimpleHiddenSelectionState(SIMPLE_SELECTION_SHOULDER_RIFLE,boo);
			SetSimpleHiddenSelectionState(SIMPLE_SELECTION_SHOULDER_MELEE,!boo);
		}
	}
	
	//! ---------------- Forces player to stand up when swapping to heavy item -------------------------
	void ForceStandUpForHeavyItems(notnull EntityAI item)
	{
		InventoryLocation il = new InventoryLocation;
		if ( item.IsHeavyBehaviour() && IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_CROUCH))
		{
			HumanCommandMove cm = GetCommand_Move();
			if (cm)
			{
				cm.ForceStance(DayZPlayerConstants.STANCEIDX_ERECT);
			}
		}
	}
	
	void ForceStandUpForHeavyItemsSwap(notnull EntityAI item1, notnull EntityAI item2)
	{
		ForceStandUpForHeavyItems(item1);
		ForceStandUpForHeavyItems(item2);
	}
	
	void OnThrowingModeChange(bool change_to_enabled){}
}

