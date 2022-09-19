class DayZPlayerCameras
{
	static const int 	DAYZCAMERA_1ST 						= 1;		//!< 1st person camera
	static const int 	DAYZCAMERA_3RD_ERC 					= 2;		//!< 3rd - standing
	static const int 	DAYZCAMERA_3RD_ERC_SPR 				= 3;		//!< 3rd - standing sprint
	static const int 	DAYZCAMERA_3RD_ERC_RAISED 			= 4;		//!< 3rd - standing raised
	static const int 	DAYZCAMERA_3RD_CRO 					= 5;		//!< 3rd - crouch
	static const int 	DAYZCAMERA_3RD_CRO_RAISED 			= 6;		//!< 3rd - crouch
	static const int 	DAYZCAMERA_3RD_PRO 					= 7;		//!< 3rd - laying
	static const int 	DAYZCAMERA_3RD_PRO_RAISED   		= 8; 		//!< 3rd - laying raised
	static const int 	DAYZCAMERA_3RD_ERC_RAISED_MELEE   	= 9; 		//!< 3rd - laying raised
	static const int 	DAYZCAMERA_IRONSIGHTS				= 10;		//!< ironsights camera
	static const int 	DAYZCAMERA_OPTICS					= 11;		//!< optics
	static const int 	DAYZCAMERA_1ST_UNCONSCIOUS			= 12;		//!< unconscious
	static const int	DAYZCAMERA_3RD_JUMP					= 13;		//!< jump
	static const int	DAYZCAMERA_3RD_CLIMB				= 14;		//!< climb / vault
	
	static const int	DAYZCAMERA_1ST_VEHICLE				= 30;		//!< vehicle 1st person
	static const int	DAYZCAMERA_3RD_VEHICLE				= 31;		//!< generic vehicle 3rd person
	
	static const int 	PERITEMUD_INVALID 			= -1;		//! uninitialized / invalid per item camera user data
	static const int 	PERITEMUD_EMPTYHANDED 		= 20;		//! for empty hands
	static const int 	PERITEMUD_ONEHANDED 		= 25;		//! for empty hands
	static const int 	PERITEMUD_TWOHANDED 		= 30;		//! for two handed items
	
	// some times for camera changes
	static const float 	TIME_CAMERACHANGE_01 				= 0.1;
	static const float 	TIME_CAMERACHANGE_02 				= 0.2;
	static const float 	TIME_CAMERACHANGE_03 				= 0.3;
	

	static ref map<int,float>								m_TransitionTimes = new map<int,float>;


	//! called from DayZPlayerTypeCreate() fn	
	// temporary moved out of the 
	static void 	RegisterCameras(DayZPlayerType pType)
	{
		pType.ResetCameraCreators();

		//! 
		//! this registers camera ids for camera script class implementation
		//! which camera is used at the time is specified by DayzPlayerImplement.CamraHandler which returns id 
		//!
		pType.RegisterCameraCreator(DAYZCAMERA_1ST, DayZPlayerCamera1stPerson);
		pType.RegisterCameraCreator(DAYZCAMERA_3RD_ERC, DayZPlayerCamera3rdPersonErc);
		pType.RegisterCameraCreator(DAYZCAMERA_3RD_ERC_SPR, DayZPlayerCamera3rdPersonErcSpr);
		pType.RegisterCameraCreator(DAYZCAMERA_3RD_ERC_RAISED, DayZPlayerCamera3rdPersonErcRaised);
		pType.RegisterCameraCreator(DAYZCAMERA_3RD_CRO, DayZPlayerCamera3rdPersonCrouch);
		pType.RegisterCameraCreator(DAYZCAMERA_3RD_CRO_RAISED, DayZPlayerCamera3rdPersonCrouchRaised);
		pType.RegisterCameraCreator(DAYZCAMERA_3RD_PRO, DayZPlayerCamera3rdPersonProne);
		pType.RegisterCameraCreator(DAYZCAMERA_3RD_PRO_RAISED, DayZPlayerCamera3rdPersonProneRaised);
		pType.RegisterCameraCreator(DAYZCAMERA_IRONSIGHTS, DayZPlayerCameraIronsights);
		pType.RegisterCameraCreator(DAYZCAMERA_OPTICS, DayZPlayerCameraOptics);
		pType.RegisterCameraCreator(DAYZCAMERA_3RD_ERC_RAISED_MELEE, DayZPlayerCamera3rdPersonErcRaisedMelee);
		pType.RegisterCameraCreator(DAYZCAMERA_1ST_UNCONSCIOUS, DayZPlayerCamera1stPersonUnconscious);
		pType.RegisterCameraCreator(DAYZCAMERA_3RD_JUMP, DayZPlayerCamera3rdPersonJump);
		pType.RegisterCameraCreator(DAYZCAMERA_3RD_CLIMB, DayZPlayerCamera3rdPersonClimb);
		
		pType.RegisterCameraCreator(DAYZCAMERA_1ST_VEHICLE, DayZPlayerCamera1stPersonVehicle);
		pType.RegisterCameraCreator(DAYZCAMERA_3RD_VEHICLE, DayZPlayerCamera3rdPersonVehicle);

		
		//! Blend times when changing cameras
		//! 3rd person erected camera transition
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC, DAYZCAMERA_3RD_CRO, 0.4, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC, DAYZCAMERA_3RD_PRO, 0.8, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC, DAYZCAMERA_3RD_PRO_RAISED, 0.8, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC, DAYZCAMERA_3RD_ERC_RAISED, 0.2, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC, DAYZCAMERA_3RD_ERC_RAISED_MELEE, 0.2, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC, DAYZCAMERA_3RD_ERC_SPR, 0.6, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC_RAISED, DAYZCAMERA_3RD_ERC_SPR, 0.7, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC_RAISED, DAYZCAMERA_3RD_PRO_RAISED, 0.8, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC_SPR, DAYZCAMERA_3RD_ERC, 0.3, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC_SPR, DAYZCAMERA_3RD_CRO, 0.3, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC_SPR, DAYZCAMERA_3RD_PRO, 0.5, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC_SPR, DAYZCAMERA_3RD_ERC_RAISED, 0.3, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC_SPR, DAYZCAMERA_3RD_JUMP, 0.1, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_JUMP, DAYZCAMERA_3RD_ERC_SPR, 0.2, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC, DAYZCAMERA_3RD_JUMP, 0.05, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_JUMP, DAYZCAMERA_3RD_ERC, 0.3, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC, DAYZCAMERA_3RD_CLIMB, 0.3, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_CLIMB, DAYZCAMERA_3RD_ERC, 0.3, false);

		//! 3rd person crouch camera transitions
		RegisterTransitionTime(DAYZCAMERA_3RD_CRO, DAYZCAMERA_3RD_ERC, 0.4, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_CRO, DAYZCAMERA_3RD_PRO, 0.6, true);
		RegisterTransitionTime(DAYZCAMERA_3RD_CRO, DAYZCAMERA_3RD_CRO_RAISED, 0.2, true);
		RegisterTransitionTime(DAYZCAMERA_3RD_CRO, DAYZCAMERA_3RD_ERC_SPR, 0.6, false);
		
		//! 3rd person prone camera transitions
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO, DAYZCAMERA_3RD_ERC, 0.9, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO, DAYZCAMERA_3RD_CRO, 0.7, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO, DAYZCAMERA_3RD_PRO_RAISED, 0.2, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO, DAYZCAMERA_3RD_ERC_SPR, 0.6, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO_RAISED, DAYZCAMERA_3RD_CRO, 0.7, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO_RAISED, DAYZCAMERA_3RD_CRO_RAISED, 0.7, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO_RAISED, DAYZCAMERA_3RD_ERC, 0.9, false);
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO_RAISED, DAYZCAMERA_3RD_ERC_RAISED, 0.9, false);

		//! ironsights camera transitions
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC_RAISED, DAYZCAMERA_IRONSIGHTS, TIME_CAMERACHANGE_02, true);
		RegisterTransitionTime(DAYZCAMERA_3RD_CRO_RAISED, DAYZCAMERA_IRONSIGHTS, TIME_CAMERACHANGE_02, true);
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO_RAISED, DAYZCAMERA_IRONSIGHTS, TIME_CAMERACHANGE_02, true);
		RegisterTransitionTime(DAYZCAMERA_1ST, DAYZCAMERA_IRONSIGHTS, TIME_CAMERACHANGE_01, true);
		
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC, DAYZCAMERA_IRONSIGHTS, TIME_CAMERACHANGE_02, true);
		RegisterTransitionTime(DAYZCAMERA_3RD_CRO, DAYZCAMERA_IRONSIGHTS, TIME_CAMERACHANGE_02, true);
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO, DAYZCAMERA_IRONSIGHTS, TIME_CAMERACHANGE_02, true);
		
		//! optics camera transitions
		RegisterTransitionTime(DAYZCAMERA_IRONSIGHTS, DAYZCAMERA_OPTICS, TIME_CAMERACHANGE_02, true);
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC_RAISED, DAYZCAMERA_OPTICS, TIME_CAMERACHANGE_03, true);
		RegisterTransitionTime(DAYZCAMERA_3RD_CRO_RAISED, DAYZCAMERA_OPTICS, TIME_CAMERACHANGE_03, true);
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO_RAISED, DAYZCAMERA_OPTICS, TIME_CAMERACHANGE_03, true);
		
		RegisterTransitionTime(DAYZCAMERA_1ST, DAYZCAMERA_OPTICS, TIME_CAMERACHANGE_02, true); 	//handheld optics only
		RegisterTransitionTime(DAYZCAMERA_3RD_ERC, DAYZCAMERA_OPTICS, TIME_CAMERACHANGE_02, true);	//handheld optics only
		RegisterTransitionTime(DAYZCAMERA_3RD_CRO, DAYZCAMERA_OPTICS, TIME_CAMERACHANGE_02, true);	//handheld optics only
		RegisterTransitionTime(DAYZCAMERA_3RD_PRO, DAYZCAMERA_OPTICS, TIME_CAMERACHANGE_02, true);	//handheld optics only
		
		//Uncon camera transitions
		RegisterTransitionTime(DAYZCAMERA_1ST_UNCONSCIOUS, DAYZCAMERA_3RD_PRO, 1.0, false);
		RegisterTransitionTime(DAYZCAMERA_1ST_UNCONSCIOUS, DAYZCAMERA_1ST, 0.1, false);
		RegisterTransitionTime(DAYZCAMERA_1ST_UNCONSCIOUS, DAYZCAMERA_1ST_VEHICLE, 0.5, false);
		RegisterTransitionTime(DAYZCAMERA_1ST_UNCONSCIOUS, DAYZCAMERA_3RD_VEHICLE, 1.0, false);
		
		//! register function to handle camera changes 
		pType.RegisterCameraOnChangeFnStatic(DayZPlayerCameras, "OnCameraChange");
	}


	//! static function to set time to table
	static void 	RegisterTransitionTime(int pFrom, int pTo, float pTime, bool pTwoWay)
	{
		int 	key = (pFrom * 65536) + pTo;
		m_TransitionTimes.Set(key, pTime);
		if (pTwoWay)
		{
			RegisterTransitionTime(pTo, pFrom, pTime, false);	
		}
	}

	//! static function to get time from table
	static float 	GetTransitionTime(int pFrom, int pTo)
	{
		int 	key = (pFrom * 65536) + pTo;
		float 	val;

		if (m_TransitionTimes.Find(key, val))
		{
			return val;
		}
		else 
		{
			return 0.3;
		}
	}



	static float 	OnCameraChange(DayZPlayer pPlayer, int pFrom, int pTo)
	{
		//Print("---transition change---");
		float transTime = GetTransitionTime(pFrom, pTo);
	    //Print("OnCameraChange:" + pFrom.ToString() + "->" + pTo.ToString() + " time:" + transTime.ToString());
		return transTime;		
	}
}
