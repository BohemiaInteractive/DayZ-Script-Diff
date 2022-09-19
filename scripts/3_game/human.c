/** @file 

  this file is interface to Human 

*/





// *************************************************************************************
// ! HumanInputController - what we know about the input - human.GetInputController()
// *************************************************************************************
class HumanInputController
{
	//--------------------------------------------------------------

	//! disables input controller
	proto native void			SetDisabled(bool pState);
	
	//!	returns pSpeed 0,1..2..3 (idle, walk, run, sprint), local normalized direction vector 
	proto void 					GetMovement(out float pSpeed, out vector pLocalDirection);

	//! returns main heading angle (in radians)  -PI .. PI (this is world Y angle player is actualy pointing the camera)
	proto native float 			GetHeadingAngle();

	//! returns per tick aim change (in radians) 
	proto native vector 		GetAimChange();

	//! returns absolute tracking change (in radians) 
	proto native vector 		GetTracking();

	//! 1st/3rd person camera view
	proto native bool			CameraViewChanged();

	//! returns true if freelook is active
	proto native bool			CameraIsFreeLook();
	
	//! reset freelook toggle
	proto native void			ResetFreeLookToggle();

	//! returns if camera is tracking (using IR device)
	proto native bool			CameraIsTracking();

	//! returns true if camera is on right/ false-left shoulder
	proto native bool			Camera3rdIsRightShoulder();

	//--------------------------------------------------------------

	//! jump/climb action was pressed
	proto native bool			IsJumpClimb();


	//--------------------------------------------------------------

	//! returns pressed SHIFT (melee evade)
	proto native bool			IsMeleeEvade();

	//! returns SHIFT down (melee fast/heavy) attack modifier
	proto native bool			IsMeleeFastAttackModifier();

	//! returns 0,1,2 = none,left,right
	proto native int 			IsMeleeLREvade();

	//! return weapon melee attack modifier
	proto native bool			IsMeleeWeaponAttack();

	//--------------------------------------------------------------

	//! returns true if weapon click perfomed recently (before raise specifically)
	proto native bool			WeaponWasRaiseClick();

	//! returns true if weapon is raised
	proto native bool			IsWeaponRaised();

	//! returns true if weapon ADS mode
	proto native bool			WeaponADS();

	//! resets ADS mode to default
	proto native void			ResetADS();
	
	//! returns true if change of throwing mode has been requested
	proto native bool			IsThrowingModeChange();

	//! resets Throwing mode
	proto native void			ResetThrowingMode();
	
	//! returns true if Walk set to toggle
	proto native bool			IsWalkToggled();

	//--------------------------------------------------------------
	
	/**
	*	\brief Deprecated; returns true if Use/Attack button is pressed (== true for multiple ticks). Synced.
	*	\note Returns state of both 'UADefaultAction' and 'UAFire' input actions. Those are now separated in the methods below.
	*/
	proto native bool			IsUseButton();

	/**
	*	\brief Deprecated; returns true if Use/Attack button has just been pressed (== true for 1 tick only). Synced.
	*	\note Returns state of both 'UADefaultAction' and 'UAFire' input actions. Those are now separated in the methods below.
	*/
	proto native bool			IsUseButtonDown();
	
		//! returns true if 'UADefaultAction' button is pressed (== true for multiple ticks). Synced.
		proto native bool			IsUseItemButton();
		//! returns true if 'UADefaultAction' button has just been pressed (== true for 1 tick only). Synced.
		proto native bool			IsUseItemButtonDown();
		
		//! returns true if 'UAFire' button is pressed (== true for multiple ticks). Synced.
		proto native bool			IsAttackButton();
		//! returns true if 'UAFire' button has just been pressed (== true for 1 tick only). Synced.
		proto native bool			IsAttackButtonDown();

	//! single 'UADefaultAction' (== true for 1 tick only) + not raised
	proto native bool			IsSingleUse();

	//! Long click 'UADefaultAction' (== true for multiple ticks) + not raised
	proto native bool			IsContinuousUse();

	//! is start of cont. 'UADefaultAction' (== true for 1 tick only) + not raised
	proto native bool			IsContinuousUseStart();

	//! is end of cont. 'UADefaultAction' (== true for 1 tick only) + not raised
	proto native bool			IsContinuousUseEnd();

	//! is immediate action triggered - 1 tick only
	//! Middle Button + not raised !
	proto native bool			IsImmediateAction(); //TODO: revise, may be obsolete

	//--------------------------------------------------------------
	//! weapon handling 
	
	//! R - reloading / bolting  (== true for 1 tick only)
	proto native bool			IsReloadOrMechanismSingleUse();

	//! R - reloading / bolting - long press (== true for multiple ticks)
	proto native bool			IsReloadOrMechanismContinuousUse();

	//! R - reloading / bolting  (== true for 1 tick only)
	proto native bool			IsReloadOrMechanismContinuousUseStart();

	//! R - reloading / bolting - long press (== true for multiple ticks)
	proto native bool			IsReloadOrMechanismContinuousUseEnd();

	//! zooming 
	proto native bool			IsZoom();

	//! zooming toggle
	proto native bool			IsZoomToggle();

	//! reset zoom toggle
	proto native void			ResetZoomToggle();

	//! sight has been changed (in/out ironsights)
	proto native bool			IsSightChange();

	//!
	proto native bool			IsZoomIn();

	//!
	proto native bool			IsZoomOut();

  	//! fire mode has changed
	proto native bool			IsFireModeChange();

  	//! zeroing up
	proto native bool			IsZeroingUp();

  	//! zeroing down
	proto native bool			IsZeroingDown();

  	//! holding breath
	proto native bool			IsHoldBreath();

  	//! reset hold breath toggle
	proto native void			ResetHoldBreath();

	//--------------------------------------------------------------

	//! returns 1..12 if F1 - F12 us pressed, 0 otherwise
	//! Deprecated; slots no longer used, bindable input actions instead
	proto native int 			IsGestureSlot();

	//! returns true, if player controls other entity (vehicle for example)
	proto native bool 			IsOtherController();

	//--------------------------------------------------------------

	//! returns 1..10 if some quickbar slot is used, 0 otherwise
	proto native int 			IsQuickBarSlot();

	//! single use (== true for 1 tick only)
	//! quickbar + not raised !
	proto native bool			IsQuickBarSingleUse();

	//! Long click use (== true for multiple ticks)
	//! long quickbar + not raised !
	proto native bool			IsQuickBarContinuousUse();

	//! is start of cont use (== true for 1 tick only)
	//! long quickbar + not raised !
	proto native bool			IsQuickBarContinuousUseStart();

	//! is end of cont use (== true for 1 tick only)
	//! long quickbar + not raised !	
	proto native bool			IsQuickBarContinuousUseEnd();



	//--------------------------------------------------------------
	// LIMITS 

	//! this disables sprint
	proto native void 			LimitsDisableSprint(bool pDisable);

	//! is sprint disabled
	proto native bool			LimitsIsSprintDisabled();
	

	//--------------------------------------------------------------
	// overrides

	//!
	proto native void 			OverrideMovementSpeed(bool pOverride, float pVal);

	//!
	proto native void 			OverrideMovementAngle(bool pOverride, float pVal);
	
	//!
	proto native void 			OverrideAimChangeX(bool pOverride, float pVal);

	//!
	proto native void 			OverrideAimChangeY(bool pOverride, float pVal);

	//!
	proto native void 			OverrideMeleeEvade(bool pOverride, bool pValue);

	//!
	proto native void 			OverrideRaise(bool pOverride, bool pValue);

	//! 
	proto native void 			Override3rdIsRightShoulder(bool pOverride, bool pValue);


	//--------------------------------------------------------------

	//! never created by script
	private void HumanInputController()
	{
	}

	//! never created by script
	private void ~HumanInputController()
	{
	}	
}


// *************************************************************************************
// ! HumanInputController - what we know about the input - human.GetInputController()
// *************************************************************************************

typedef 	int 	TAnimGraphCommand;
typedef 	int 	TAnimGraphVariable;
typedef 	int 	TAnimGraphTag;
typedef 	int 	TAnimGraphEvent;

//!
class HumanAnimInterface
{
	private void HumanAnimInterface();
	private void ~HumanAnimInterface();
	
	//-----------------------------------------------------
	// Binds, returns -1 when error, otherwise if ok

	//! returns command index - 	
	proto native TAnimGraphCommand		BindCommand(string pCommandName);

	//!
	proto native TAnimGraphVariable 	BindVariableFloat(string pVariable);
	proto native TAnimGraphVariable 	BindVariableInt(string pVariable);
	proto native TAnimGraphVariable 	BindVariableBool(string pVariable);

	//!
	proto native TAnimGraphTag 			BindTag(string pTagName);

	//!
	proto native TAnimGraphEvent		BindEvent(string pEventName);
}


// *************************************************************************************
// ! HumanCommandActionCallback - called as action callback 
// *************************************************************************************
class HumanCommandActionCallback
{
	private void HumanCommandActionCallback();
	private void ~HumanCommandActionCallback();
	
	//! get the human this cb belongs to
	proto native Human 		GetHuman();
	
	//! cancels action 
	proto native 	void 	Cancel();

	//! calls internal command 
	//! pInternalCommandId is one of CMD_ACTIONINT_ ... 
	proto native 	void 	InternalCommand(int pInternalCommandId);	
	
	//! enables character aligning to desired position and direction in world space
	proto native	void	SetAligning(vector pPositionWS, vector pDirectionWS);

	//! disables character aligning
	proto native	void	ResetAligning();

	//! enables calling cancel condition
	//! default is disabled
	//! when enabled - there must be a function "bool CancelCondition()" implemented
	proto native 	void 	EnableCancelCondition(bool pEnable);

	//! system implemented cancel condition (now raise or sprint cancels action)
	proto native 	bool	DefaultCancelCondition();

	//! registers animation event, when event registered and received - it sends OnAnimationEvent with registered id 
	proto native void 		RegisterAnimationEvent(string pAnimationEventStr, int pId);

	//! enables state change callback OnStateChange 
	proto native void 		EnableStateChangeCallback();

	//! example of implement of CancelCondition
	//! returns true it it should CANCEL the action
	//	bool				CancelCondition()	{ return false; }

	//! undef state - not running anymore
	static const int 	STATE_NONE 			= 0;

	//! looped state 
	static const int 	STATE_LOOP_IN 		= 1;
	static const int 	STATE_LOOP_LOOP 	= 2;
	static const int 	STATE_LOOP_END 		= 3;
	static const int 	STATE_LOOP_END2		= 4;
	static const int 	STATE_LOOP_LOOP2	= 5;
	static const int 	STATE_LOOP_ACTION	= 6;

	//! one time state 
	static const int 	STATE_NORMAL 		= 7;

	//! returns one of STATE_...
	proto native int 	GetState();


	//----------------------------------
	// debug helper

	//! returns debug string of current state 
	static string	GetStateString(int pState)
	{
		switch (pState)
		{
		case STATE_NONE:			return "NONE";

		case STATE_LOOP_IN:			return "LOOP_IN";
		case STATE_LOOP_LOOP: 		return "LOOP_LOOP";
		case STATE_LOOP_END:		return "LOOP_END";
		case STATE_LOOP_END2:		return "LOOP_END2";
		case STATE_LOOP_LOOP2:		return "LOOP_LOOP2";
		case STATE_LOOP_ACTION:		return "LOOP_ACTION";

			//! one time state 
		case STATE_NORMAL:			return "ONE TIME";
		}

		return "Undef";
	}


	//! returns debug string of current state 
	string	GetStateString()
	{
		return GetStateString(GetState());
	}



	//----------------------------------
	// callbacks

	//! calls animation event -> must be registered with RegisterAnimationEvent
	void 	OnAnimationEvent(int pEventID)	{};

	//! called ALWAYS! when action is finished (normal finish or cancel -> pCanceled)
	void 	OnFinish(bool pCanceled)	{};		

	//! called when enabled by EnableStateChangeCallback and state changes, call GetState() to get actual state 
	void 	OnStateChange(int pOldState, int pCurrentState)			{};
	
	
	// helps identify type of callback
	bool IsUserActionCallback()
	{
		return false;
	}
	
	bool IsGestureCallback()
	{
		return false;
	}
	
	bool IsSymptomCallback()
	{
		return false;
	}
}


// *************************************************************************************
// ! HumanCommandMove
// *************************************************************************************
class HumanCommandMove
{
	private void HumanCommandMove() {}
	private void ~HumanCommandMove() {}
	
	//! marks command to continue to combo 
	//! -180 -90 0 90 180 angles of current movement 
	proto native float 		GetCurrentMovementAngle();
		
	//! direction held on keyboard/controller
	//! -180 -90 0 90 180 angles of input movement (no filtering, no postprocesses)
	//! return false in idle states
	proto bool				GetCurrentInputAngle(out float pAngle);

	//! 0,1,2..3 idle, walk, run, sprint 
	proto native float 		GetCurrentMovementSpeed();
	
	//! return true if prone is on back
	proto native bool		IsOnBack();
	
	//! return true if character barrel rolling
	proto native bool		IsInRoll();
	
	//! return true if character transitions out of uncon
	proto native bool		IsLeavingUncon();

	//! return true if prone on back is chaning to crounch/stand
	proto native bool		IsStandingFromBack();

	//! marks command to continue to combo 
	proto native void 		StartMeleeEvade();

	//! marks command to continue to combo, pDirAngle specifies angle
	proto native void 		StartMeleeEvadeA(float pDirAngle);

	//! this is true when in melee evade
	proto native bool		IsMeleeEvade();

	//! this enables/disables block 
	proto native void 		SetMeleeBlock(bool pBlock);

	//!	pStanceIds is one of STANCEIDX_ERECT,STANCEIDX_CROUCH,STANCEIDX_PRONE,STANCEIDX_RAISEDERECT,STANCEIDX_RAISEDCROUCH,STANCEIDX_RAISEDPRONE,
	//! or -1 when release the stance lock 
	proto native void 		ForceStance(int pStanceIdx);


	//!	pStanceIds is one of STANCEIDX_ERECT,STANCEIDX_CROUCH
	//! this forces to stand up to required stance
	proto native void 		ForceStanceUp(int pStanceIdx);


	//! sets the multiplier for SHumanCommandMoveSettings::m_fRunSpringTimeout
	proto native void		SetRunSprintFilterModifier(float value);

	//! sets the multiplier for SHumanCommandMoveSettings::m_fDirFilterTimeout
	proto native void		SetDirectionFilterModifier(float value);

	//! sets the multiplier for SHumanCommandMoveSettings::m_fDirFilterSprintTimeout
	proto native void		SetDirectionSprintFilterModifier(float value);

	//! sets the multiplier for HumanItemBehaviorCfg::m_fMoveHeadingFilterSpan
	proto native void		SetTurnSpanModifier(float value);

	//! sets the multiplier for HumanItemBehaviorCfg::m_fMoveHeadingSprintFilterSpan
	proto native void		SetTurnSpanSprintModifier(float value);
	
	//! sets water level (depth)
	proto native void		SetCurrentWaterLevel(float value);
}



// *************************************************************************************
// ! HumanCommandMelee - actual command melee running on 
// *************************************************************************************
class HumanCommandMelee
{
	private void HumanCommandMelee() {}
	private void ~HumanCommandMelee() {}
	
	//! marks command to continue to combo 
	proto native void 		ContinueCombo();

	//! returns true if hit is in range, where person can continue to combo
	proto native bool		IsInComboRange();

	//! is true only once after hit event 
	proto native bool		WasHit();

	//! cancels command melee and goes to HumanCommandMove
	proto native void		Cancel();
	
	//! is on back in prone stance?
	proto native bool		IsOnBack();
}


// *************************************************************************************
// ! HumanCommandMelee2 - actual command melee running on - Heavy/Light version
// *************************************************************************************
class HumanCommandMelee2
{
	private void HumanCommandMelee2() {}
	private void ~HumanCommandMelee2() {}
	
	static const int HIT_TYPE_LIGHT = 0;
	static const int HIT_TYPE_HEAVY	= 1;
	static const int HIT_TYPE_FINISHER = 2; //liver stab
	static const int HIT_TYPE_FINISHER_NECK = 3;
	
	//! marks command to continue to combo 
	proto native void 		ContinueCombo(bool pHeavyHit, float pComboValue, EntityAI target = null, vector hitPos = vector.Zero);

	//! returns true if hit is in range, where person can continue to combo
	proto native bool		IsInComboRange();

	//! is true only once after hit event 
	proto native bool		WasHit();

	//! cancels command melee and goes to HumanCommandMove
	proto native void		Cancel();

	//! is on back in prone stance?
	proto native bool		IsOnBack();
	
	proto native int		GetComboCount();
	
	proto native int		GetCurrentHitType();
	
	proto native bool		IsFinisher();
}


// *************************************************************************************
// ! HumanCommandFall - actually falling 
// *************************************************************************************
class HumanCommandFall
{
	private void HumanCommandFall() {}
	private void ~HumanCommandFall() {}
	
	static const int LANDTYPE_NONE = 0;
	static const int LANDTYPE_LIGHT	= 1;
	static const int LANDTYPE_MEDIUM = 2;
	static const int LANDTYPE_HEAVY = 3;

	//! this is true when fall has physically landed - need to call Land after this is true
	proto native bool	PhysicsLanded();
	
	//! end fall with land
	proto native void 	Land(int pLandType);
	
	//! returns true if fall is in landing state 
	proto native bool	IsLanding();
}

// *************************************************************************************
// ! HumanCommandDeathCallback - called as action callback 
// *************************************************************************************
class HumanCommandDeathCallback
{
	//----------------------------------
	// callbacks

	void 	OnSimulationEnd()	{};		
}

// *************************************************************************************
// ! HumanCommandDeath - actually dying
// *************************************************************************************
class HumanCommandDeath
{	
	private void HumanCommandDeath() {}
	private void ~HumanCommandDeath() {}
}


// *************************************************************************************
// ! HumanCommandUnconscious - actually not dying
// *************************************************************************************
class HumanCommandUnconscious
{	
	private void HumanCommandUnconscious() {}
	private void ~HumanCommandUnconscious() {}
	
	proto native void 	WakeUp(int targetStance = -1);
	proto native bool	IsOnLand();
	proto native bool	IsInWater();
}


// *************************************************************************************
// ! HumanCommandDamage - additional damage 
// *************************************************************************************
class HumanCommandDamage
{	
	private void HumanCommandDamage() {}
	private void ~HumanCommandDamage() {}
}

// *************************************************************************************
// ! HumanCommandLadder ladder
// *************************************************************************************
class HumanCommandLadder
{	
	private void HumanCommandLadder() {}
	private void ~HumanCommandLadder() {}
	
	//! returns true if on exiting point
	proto native bool				CanExit();

	//!
	proto native void 				Exit();

	//! debug draws any ladder 
	proto native static bool		DebugDrawLadder(Building pBuilding, int pLadderIndex);
	
	//!
	proto native static int 		DebugGetLadderIndex(string pComponentName); 
}



// *************************************************************************************
// ! HumanCommandLadder ladder
// *************************************************************************************
class HumanCommandSwim
{
	private void HumanCommandSwim() {}
	private void ~HumanCommandSwim() {}
	
	//!
	proto native void 				StopSwimming();

	//! returns water level from entity position 
	//! returns in format (totalWaterDepth, characterDepht, 0)
	proto native static vector 		WaterLevelCheck(Human pHuman, vector pPosition);
}



// *************************************************************************************
// ! HumanCommandVehicle vehicle
// *************************************************************************************
class HumanCommandVehicle
{
	private void HumanCommandVehicle() {}
	private void ~HumanCommandVehicle() {}
	
	proto native Transport			GetTransport();
	proto native int				GetVehicleClass();
	proto native int				GetVehicleSeat();
	proto native void				SetVehicleType(int pVehicleType);
	proto native int				GetVehicleType();
	
	proto native void				GetOutVehicle();
	proto native void				KnockedOutVehicle();
	proto native bool				ShouldBeKnockedOut();
	proto native void				JumpOutVehicle();
	proto native void				SwitchSeat(int pTransportPositionIndex, int pVehicleSeat);
	proto native bool				IsGettingIn();
	proto native bool				IsGettingOut();
	proto native bool				IsSwitchSeat();
	proto native bool				WasGearChange();
	proto native void				SetClutchState(bool pState);
	proto native void				KeepInVehicleSpaceAfterLeave(bool pState);
}

// *************************************************************************************
// ! HumanCommandClimb
// *************************************************************************************

//! result from static test
class SHumanCommandClimbResult
{
	bool		m_bIsClimb;	
	bool		m_bIsClimbOver;
	bool		m_bFinishWithFall;
	bool		m_bHasParent;

	float		m_fClimbHeight;
	
	vector 		m_ClimbGrabPoint;		//! grab point for climb && climb over (in local space of it's parent)
	vector 		m_ClimbGrabPointNormal;	//! normal to grabpoint position (used for character orientation)
	vector 		m_ClimbStandPoint;		//! where climb ends (in local space of it's parent)
	vector 		m_ClimbOverStandPoint;	//! where climb over ends (in local space of it's parent)
	
	IEntity		m_GrabPointParent;		//! parent of grabpoint
	IEntity		m_ClimbStandPointParent;
	IEntity		m_ClimbOverStandPointParent;
};


//! state of climb command
enum ClimbStates
{
	STATE_MOVE,
	STATE_TAKEOFF,
	STATE_ONTOP,
	STATE_FALLING,
	STATE_FINISH
};

//! command itself
class HumanCommandClimb
{
	private void HumanCommandClimb() {}
	private void ~HumanCommandClimb() {}
	
	//! returns the state of climb (enum value of ClimbStates);
	proto native int				GetState();
	
	//! returns world space position of climbing grab point
	proto native vector				GetGrabPointWS();
	
	//! returns world space position of landspot after climbing over
	proto native vector				GetClimbOverStandPointWS();
	
	//! debug draws climb heauristics
	//! pDebugDrawLevel viz DebugDrawClimb
	proto native static bool		DoClimbTest(Human pHuman, SHumanCommandClimbResult pResult, int pDebugDrawLevel);

	//! debug draws climb heauristics
	//! int pLevel = 1 result, 2 - capsules (3 - all)
	proto native static bool		DebugDrawClimb(Human pHuman, int pLevel);
}




// *************************************************************************************
// ! HumanCommandFullBodyDamage - full body damages
// *************************************************************************************
class HumanCommandFullBodyDamage
{	
	private void HumanCommandFullBodyDamage() {}
	private void ~HumanCommandFullBodyDamage() {}
}


//! actions
enum WeaponActions
{
	INTERRUPT			= 15,
	NONE                = -1,
	RELOAD 				= 0,
	MECHANISM			= 1,
	CHAMBERING 			= 2,
	CHAMBERING_LOADER 	= 3,
	UNJAMMING			= 4,
	FIRE 				= 5,
	HIDE 				= 6,
	SHOW 				= 7,
	RELOAD_CLIP			= 8,
};
//! action types
/// reload action types
enum WeaponActionReloadTypes
{
	//! reload action types - rifles
	RELOADRIFLE_MAGAZINE_BULLET 			= 0,   		// CMD_Reload_Magazine
	RELOADRIFLE_NOMAGAZINE_BULLET 			= 1,
	RELOADRIFLE_MAGAZINE_NOBULLET 			= 2,
	RELOADRIFLE_NOMAGAZINE_NOBULLET 		= 3,
	RELOADRIFLE_MAGAZINE_NOBULLET_OPEN		= 4,
	RELOADRIFLE_NOMAGAZINE_NOBULLET_OPEN	= 5,

	// reload action types - pistols
	RELOADPISTOL_NOMAGAZINE_NOBULLET_CLOSED_UNCOCKED	= 8, // CMD_Reload_Magazine	
	RELOADPISTOL_MAGAZINE_NOBULLET_CLOSED_UNCOCKED		= 9,
	RELOADPISTOL_MAGAZINE_BULLET_CLOSED					= 10,   		
	RELOADPISTOL_NOMAGAZINE_BULLET_CLOSED 				= 11,
	RELOADPISTOL_MAGAZINE_NOBULLET_CLOSED_COCKED 		= 12,
	RELOADPISTOL_NOMAGAZINE_NOBULLET_CLOSED_COCKED 		= 13,
	RELOADPISTOL_NOMAGAZINE_NOBULLET_OPENED 			= 14,
	RELOADPISTOL_MAGAZINE_NOBULLET_OPENED 				= 15,

	RELOAD_MAGAZINE_DETACH 	= 16,
	RELOADRIFLE_MAGAZINE_DETACH = 17,                    // CMD_Reload_Magazine

	RELOADSRIFLE_MAGAZINE_BULLET 		= 20,   		// CMD_Reload_Magazine
	RELOADSRIFLE_NOMAGAZINE_BULLET 		= 21,
	RELOADSRIFLE_MAGAZINE_NOBULLET 		= 22,
	RELOADSRIFLE_NOMAGAZINE_NOBULLET 	= 23,
};

enum WeaponActionReloadClipTypes
{
	RELOADRIFLE_CLIP_NOBULLET			= 0,
	RELOADRIFLE_CLIP_BULLET				= 1,
}

enum WeaponActionMechanismTypes
{
	//! mechanism action types
	MECHANISM_OPENED 	= 0,		// CMD_Reload_BoltAction
	MECHANISM_CLOSED 	= 1,		
	MECHANISM_SPECIAL 	= 2,	    // crossbow, magnum ... special mechanism action apropriate for custom weapons
    MECHANISM_CLOSED_UNCOCKED 	= 3,	
};

enum WeaponActionChamberingTypes
{
	CHAMBERING_END = -1,
	//! chambering action types
	CHAMBERING_ONEBULLET_OPENED 				= 0,		// CMD_Reload_Chambering
	CHAMBERING_ONEBULLET_CLOSED  				= 1,		//
	CHAMBERING_ONEBULLET_CLOSED_UNCOCKED 		= 2,
	CHAMBERING_ONEBULLET_UNIQUE_OPENED 			= 3,		//
	CHAMBERING_ONEBULLET_UNIQUE_CLOSED			= 4,		//
	CHAMBERING_TWOBULLETS_START					= 6,		//  plays one bullet, then second, then ends, when CHAMBERING_TWOBULLETS_END arise, it's canceled
	CHAMBERING_TWOBULLETS_END					= 7,		//  - one bullet reload with closed mechanism		
	CHAMBERING_STARTLOOPABLE_CLOSED				= 10,		// start loop chambering
	CHAMBERING_ENDLOOPABLE						= 11,		// end loop chambering
	CHAMBERING_STARTLOOPABLE_CLOSED_KEEP		= 12,		// start loop chambering and keep last bullet
	CHAMBERING_STARTLOOPABLE_OPENED				= 13,		// 
	
	CHAMBERING_STARTLOOPABLE_SHOTGUN_UNCOCKED	= 15,
	CHAMBERING_STARTLOOPABLE_SHOTGUN_COCKED		= 16, 
	
	CHAMBERING_DOUBLE_1				= 17,
	CHAMBERING_DOUBLE_2				= 18,
	CHAMBERING_DOUBLE_3				= 19,
	CHAMBERING_DOUBLE_4				= 20,
	

	CHAMBERING_CROSSBOW_OPENED			= 21,		// chambering crossbow
	CHAMBERING_CROSSBOW_CLOSED			= 22,		// chambering crossbow
	CHAMBERING_CROSSBOW_FULL			= 23,		// chambering crossbow
};

enum WeaponActionChamberingLoaderTypes
{
	CHAMBERINGLOADER_OPENED		= 0,		// CMD_Reload_ChamberingFast - one bullet reload with open mechanism
	CHAMBERINGLOADER_CLOSED 	= 1,		//  - one bullet reload with closed mechanism
};

enum WeaponActionUnjammingTypes
{
	//! unjam action types
	UNJAMMING_START 					= 1,		// CMD_Weapon_Jam - 0
	UNJAMMING_END 						= 0,		// 1
	UNJAMMING_INTERRUPT					= -1,
};

enum WeaponActionFireTypes
{
	FIRE_NORMAL 						= 0,
	FIRE_LAST 							= 1,
	FIRE_COCKED 						= 2,
	FIRE_UNCOCKED 						= 3,
	FIRE_DRY							= 4,
	FIRE_JAM							= 5
};

enum WeaponHideShowTypes
{ 
	HIDESHOW_SLOT_2HDLEFTBACK = 0,
	HIDESHOW_SLOT_RFLLEFTBACK = 1,
	HIDESHOW_SLOT_1HDLEFTBACK = 2,
	HIDESHOW_SLOT_2HDRIGHTBACK = 3,
	HIDESHOW_SLOT_RFLRIGHTBACK = 4,
	HIDESHOW_SLOT_1HDRIGHTBACK = 5,
	HIDESHOW_SLOT_PISTOLBELT = 6,
	HIDESHOW_SLOT_PISTOLCHEST = 7,
	HIDESHOW_SLOT_KNIFEBACK = 8,
	HIDESHOW_SLOT_INVENTORY = 9,
};

string WeaponActionTypeToString (int A, int AT)
{
	switch (A)
	{
		case WeaponActions.INTERRUPT: return "Weapon interrupt";
		case WeaponActions.NONE: return "---";
		case WeaponActions.RELOAD: return typename.EnumToString(WeaponActionReloadTypes, AT);
		case WeaponActions.MECHANISM: return typename.EnumToString(WeaponActionMechanismTypes, AT);
		case WeaponActions.CHAMBERING: return typename.EnumToString(WeaponActionChamberingTypes, AT);
		case WeaponActions.CHAMBERING_LOADER: return typename.EnumToString(WeaponActionChamberingLoaderTypes, AT);
		case WeaponActions.UNJAMMING: return typename.EnumToString(WeaponActionUnjammingTypes, AT);
		case WeaponActions.FIRE: return typename.EnumToString(WeaponActionFireTypes, AT);
		case WeaponActions.HIDE: return typename.EnumToString(WeaponHideShowTypes, AT);
		case WeaponActions.SHOW: return typename.EnumToString(WeaponHideShowTypes, AT);
	}
	return "---";
}

//! events
enum WeaponEvents
{
	ATTACHMENT_HIDE,
	ATTACHMENT_SHOW,
	BULLET_EJECT,
	BULLET_HIDE,
	BULLET_HIDE2,
	BULLET_IN_CHAMBER,
	BULLET_IN_MAGAZINE,
	BULLET_SHOW,
	BULLET_SHOW2,
	CANUNJAM_END,
	CANUNJAM_START,
	COCKED,
	MAGAZINE_ATTACHED,
	MAGAZINE_DETACHED,
	MAGAZINE_HIDE,
	MAGAZINE_SHOW,
	SLIDER_OPEN,
	UNJAMMED,
	HAMMER_UNCOCKED,
	HAMMER_COCKED
	CHANGE_HIDE,
	CHANGE_SHOW,
	CYLINDER_ROTATE,
};


// *************************************************************************************
// ! HumanCommandWeapons - weapon handling
// *************************************************************************************
class HumanCommandWeapons
{
	private void HumanCommandWeapons() {}
	private void ~HumanCommandWeapons() {}
	
	//!
    proto native 	bool		IsActionFinished();
	
	//! returns -1 when no action is running or RELOAD,MECHANISM, .... 
    proto native 	int 		GetRunningAction();

	//! returns -1 when no action is running or appropriate action type
    proto native 	int 		GetRunningActionType();

	//! sets start and end animation position - f.e. for reload clip action
	proto native 	void		SetActionProgressParams(float pStart, float pEnd);

	//! start reload,mechanism,chambering,unjam ... 
	proto native 	bool		StartAction(WeaponActions pAction, int pActionType);

	//! register events 
	proto native    void    	RegisterEvent(string pName, int pId);

	//! return -1 when there is no event, otherwise it returns pId of event from animation 
	proto native 	int 		IsEvent();
	
	//! returns true when clip visual change is needed for reload clip action
	proto native 	bool 		IsInWeaponReloadBulletSwitchState();

	//! sets head tilt to optics
	proto native	void		SetADS(bool pState);

	//! command for lifting weapon near obstacled (works only when weapon is raised)
	proto native	void		LiftWeapon(bool pState);

	//!
	void 	RegisterDefaultEvents()
	{
		RegisterEvent("Weapon_AttachmentHide", WeaponEvents.ATTACHMENT_HIDE);
		RegisterEvent("Weapon_AttachmentShow", WeaponEvents.ATTACHMENT_SHOW);
		RegisterEvent("Weapon_BulletEject", WeaponEvents.BULLET_EJECT);
		RegisterEvent("Weapon_BulletHide", WeaponEvents.BULLET_HIDE);
		RegisterEvent("Weapon_BulletHide2", WeaponEvents.BULLET_HIDE2);
		RegisterEvent("Weapon_BulletInChamber", WeaponEvents.BULLET_IN_CHAMBER);
		RegisterEvent("Weapon_BulletInMagazine", WeaponEvents.BULLET_IN_MAGAZINE);
		RegisterEvent("Weapon_BulletShow", WeaponEvents.BULLET_SHOW);
		RegisterEvent("Weapon_BulletShow2", WeaponEvents.BULLET_SHOW2);
		RegisterEvent("Weapon_CanUnjam_End", WeaponEvents.CANUNJAM_END);
		RegisterEvent("Weapon_CanUnjam_Start", WeaponEvents.CANUNJAM_START);
		RegisterEvent("Weapon_Cocked", WeaponEvents.COCKED);
		RegisterEvent("Weapon_MagazineAttached", WeaponEvents.MAGAZINE_ATTACHED);
		RegisterEvent("Weapon_MagazineDetached", WeaponEvents.MAGAZINE_DETACHED);
		RegisterEvent("Weapon_MagazineHide", WeaponEvents.MAGAZINE_HIDE);
		RegisterEvent("Weapon_MagazineShow", WeaponEvents.MAGAZINE_SHOW);
		RegisterEvent("Weapon_SliderOpen", WeaponEvents.SLIDER_OPEN);
		RegisterEvent("Weapon_Unjammed", WeaponEvents.UNJAMMED);
		RegisterEvent("Weapon_Hammer_Uncocked", WeaponEvents.HAMMER_UNCOCKED);
		RegisterEvent("Weapon_Hammer_Cocked", WeaponEvents.HAMMER_COCKED);
		RegisterEvent("Weapon_Change_Hide", WeaponEvents.CHANGE_HIDE);
		RegisterEvent("Weapon_Change_Show", WeaponEvents.CHANGE_SHOW);
		RegisterEvent("Weapon_CylinderRotate", WeaponEvents.CYLINDER_ROTATE);
	}

	//----------------------------------------------------
	// weapon init states


	//! this sets to a weapon init state 
	//! each weapon has a different weapon state 
	//! those states are defined by animation set by AddItemInHandsProfileIK - in DayZPlayerCfgBase.c
	//! each frame of animation is one init state 
	proto native 	void 		SetInitState(int pFrameIndex);

	//! just static version of SetInitState
	proto native static void 	StaticSetInitState(Human pHuman, int pFrameIdx);


	//----------------------------------------------------
	// 

	//! returns base aiming angle UD - without sway/offsets/...
	proto native float 			GetBaseAimingAngleUD();

	//! returns base aiming angle LR - without sway/offsets/...
	proto native float 			GetBaseAimingAngleLR();


	//----------------------------------------------------
	// throwing
	proto native void			SetThrowingMode(bool pState);
	
	proto native bool			IsThrowingMode();
	
	proto native void			ThrowItem(int throwType);
	
	proto native bool			WasItemLeaveHandsEvent();
	
	//----------------------------------------------------
	// debug copy 

	//! return -1 when there is no event, otherwise it returns pId of event from animation 
	proto native 	int 		DebugIsEvent();

	//! 
	proto native 	void 		DebugResetEvents();
}


// *************************************************************************************
// ! HumanCommandAdditives - additional damage 
// *************************************************************************************
class HumanCommandAdditives
{
	private void HumanCommandAdditives() {}
	private void ~HumanCommandAdditives() {}

	//! sets injury level 0..1, interpolate == false -> resets the value, otherwise it's interpolating towards the new value
	proto native void 	SetInjured(float pValue, bool pInterpolate);

	//! sets exhaustion level 0..1, interpolate == false -> resets the value, otherwise it's interpolating towards the new value
	proto native void 	SetExhaustion(float pValue, bool pInterpolate);

	//! sets talking
	proto native void 	SetTalking(bool pValue);

	//! starts modifier
	proto native void	StartModifier(int pType);

	//! cancels modifier
	proto native void	CancelModifier();

	//! is modifier active
	proto native bool	IsModifierActive();
}


// *************************************************************************************
// ! HumanMovementState - movement state, command, stance, movement, human.GetMovementState(movementState)
// *************************************************************************************
class HumanMovementState
{
	int 		m_CommandTypeId;	//! current command's id 
	int 		m_iStanceIdx;		//! current stance (DayZPlayer.STANCEIDX_ERECT, ...), only if the command has a stance
	int 		m_iMovement;		//! current movement (0 idle, 1 walk, 2-run, 3-sprint), only if the command has a movement 
	float		m_fLeaning;			//! leaning state (not all commands need to have all movements)
	
	int			m_LocalMovement = -1;
	
	//! 
	bool		IsRaised()
	{
		return m_iStanceIdx >= DayZPlayerConstants.STANCEIDX_RAISEDERECT;
	}
	
	//! 
	bool		IsRaisedInProne()
	{
		return m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE;
	}
	
	//! 
	bool		IsInProne()
	{
		return m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE;
	}

	//! 
	bool		IsInRaisedProne()
	{
		return m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE;
	}	
		
	//! 
	bool		IsLeaning()
	{
		return m_fLeaning != 0;
	}
}


/**
*\brief HumanCommandScript fully scriptable command
*	\warning NON-MANAGED, will be managed by C++ once it is sent to the CommandHandler through Human.StartCommand_Script
*	\note So ideally, it is best to set up the HumanCommandScript, not create any instances and start it through Human.StartCommand_ScriptInst
*			In case an instance needs to be created, it needs manual deletion if not sent to the CommandHandler
*			But deleting it while it is in the CommandHandler will cause crashes
*/
class HumanCommandScript
{
	//! constructor must have 1st parameter to be Human
	// HumanCommandScript(Human pHuman);

	//! virtual to be overridden
	//! called when command starts
	void 	OnActivate()	{ };

	//! called when command ends
	void 	OnDeactivate()	{ };


	//---------------------------------------------------------------
	// usable everywhere

	//! this terminates human command script and shows CommandHandler(  ... pCurrentCommandFinished == true );
	proto native void 	SetFlagFinished(bool pFinished);


	//---------------------------------------------------------------
	// PreAnim Update 

	//! override this !
	//! called before any animation is processed
	//! here change animation values, add animation commands	
	void 	PreAnimUpdate(float pDt);

	//! function usable in PreAnimUpdate or in !!! OnActivate !!!
	proto native 	void	PreAnim_CallCommand(int pCommand, int pParamInt, float pParamFloat);
	proto native 	void	PreAnim_SetFloat(int pVar, float pFlt);
	proto native 	void	PreAnim_SetInt(int pVar, int pInt);
	proto native 	void	PreAnim_SetBool(int pVar, bool pBool);

	//! sets character rotation (heading)
	proto native 	void 	PreAnim_SetFilteredHeading(float pYawAngle, float pFilterDt, float pMaxYawSpeed);

	//---------------------------------------------------------------
	// PrePhys Update 

	//! override this !
	//! after animation is processed, before physics is processed
	void 	PrePhysUpdate(float pDt);

	//! script function usable in PrePhysUpdate
	proto native 	bool	PrePhys_IsEvent(int pEvent);
	proto native 	bool	PrePhys_IsTag(int pTag);
	proto native 	bool	PrePhys_GetTranslation(out vector pOutTransl);		// vec3 in local space !
	proto native 	bool	PrePhys_GetRotation(out float pOutRot[4]);         	// quaternion in local space !
	proto native 	void	PrePhys_SetTranslation(vector pInTransl); 			// vec3 in local space !
	proto native 	void	PrePhys_SetRotation(float pInRot[4]);				// quaternion in local space !

	//---------------------------------------------------------------
	// PostPhys Update 

	//! override this !
	//! final adjustment of physics state (after physics was applied)
	//! returns true if command continues running / false if command should end (or you can use SetFlagFinished(true))
	bool	PostPhysUpdate(float pDt);

	//! script function usable in PostPhysUpdate
	proto native 	void	PostPhys_GetPosition(out vector pOutTransl);		//! vec3 in world space
	proto native 	void	PostPhys_GetRotation(out float pOutRot[4]);        	//! quaternion in world space
	proto native 	void	PostPhys_SetPosition(vector pInTransl);				//! vec3 in world space
	proto native 	void	PostPhys_SetRotation(float pInRot[4]);				//! quaternion in world space
	proto native 	void	PostPhys_LockRotation();							//! do not process rotations !
}

//! used in script-side identification of animation commands on human (swim, fall, move...)
enum HumanMoveCommandID
{
	None = 0,
	CommandMove = 1,
	CommandMelee = 2,
	CommandMelee2 = 4,
	CommandFall = 8,
	CommandClimb = 16,
	CommandDeath = 32,
	CommandUnconscious = 64,
	CommandDamageFullbody = 128,
	CommandDamageAdditive = 256,
	CommandLadder = 512,
	CommandSwim = 1024,
	CommandVehicle = 2048,
	CommandActionFullbody = 4096,
	CommandActionAdditive = 8192
}

// *************************************************************************************
// ! Human - human script interface 
// *************************************************************************************
class Human extends Man
{

	//! gets human transform in World Space
	proto native	void 		GetTransformWS(out vector pTm[4]);


	//---------------------------------------------------------
	// link/unlink to/from local space
	proto native	void		LinkToLocalSpaceOf(notnull IEntity child, vector pLocalSpaceMatrix[4]);
	proto native	void		UnlinkFromLocalSpace();

	//---------------------------------------------------------
	// bone transforms 

	//! returns bone index for a name (-1 if pBoneName doesn't exist)
	proto native 	int 		GetBoneIndexByName(string pBoneName);

	//! returns local space, model space, world space position of the bone 
	proto native	vector		GetBonePositionLS(int pBoneIndex);
	proto native 	vector		GetBonePositionMS(int pBoneIndex);
	proto native 	vector		GetBonePositionWS(int pBoneIndex);

	//! returns local space, model space, world space orientation (quaternion) of a bone 
	proto native	void 		GetBoneRotationLS(int pBoneIndex, out float pQuat[4]);
	proto native 	void 		GetBoneRotationMS(int pBoneIndex, out float pQuat[4]);
	proto native 	void 		GetBoneRotationWS(int pBoneIndex, out float pQuat[4]);

	//! returns local space, model space, world space orientation (quaternion) of a bone 
	proto native	void 		GetBoneTransformLS(int pBoneIndex, out vector pTm[4]);
	proto native 	void 		GetBoneTransformMS(int pBoneIndex, out vector pTm[4]);
	proto native 	void 		GetBoneTransformWS(int pBoneIndex, out vector pTm[4]);

	
	//! returns animation interface - usable in HumanCommandScript implementations 
	proto native 	HumanAnimInterface 	GetAnimInterface();

	//---------------------------------------------------------
	// physic props
	
	//! returns true if physics controller is falling 
	proto native 	bool		PhysicsIsFalling(bool pValidate);

	//! outs pVelocity - linear velocity of PHYSICS CONTROLLER 
	proto native 	void 		PhysicsGetVelocity(out vector pVelocity);

	//---------------------------------------------------------
	// controller 
			
	//! returns human input controller
	proto native 	HumanInputController	GetInputController();

	//---------------------------------------------------------
	// item accessor

	proto native 	HumanItemAccessor		GetItemAccessor();


	//---------------------------------------------------------
	// commands 

	//! returns movement state (current command id, )
	proto native 	void 	GetMovementState(HumanMovementState pState);

	//! returns current command ID
	// proto native 	int 	GetCurrentCommandID();


	//!----- MOVE -----

	//! starts command - Move
	proto native 	HumanCommandMove			StartCommand_Move();

	proto native 	HumanCommandMove			GetCommand_Move();

	//!----- MELEE -----

	//! starts command - melee
	proto native 	HumanCommandMelee			StartCommand_Melee(EntityAI pTarget);

	proto native 	HumanCommandMelee			GetCommand_Melee();

	//! starts command - melee2
	proto native 	HumanCommandMelee2			StartCommand_Melee2(EntityAI pTarget, int pHitType, float pComboValue, vector hitPos = vector.Zero);
	
	proto native 	HumanCommandMelee2			GetCommand_Melee2();


	//!----- FALL -----

	//! starts command - fall
	//! pYVelocity <= 0 -> fall
	//! pYVelocity > 0 -> jump
	proto native 	HumanCommandFall			StartCommand_Fall(float pYVelocity);

	//! 
	proto native 	HumanCommandFall			GetCommand_Fall();


	//!----- LADDER -----

	//! starts command - fall
	proto native 	HumanCommandLadder 			StartCommand_Ladder(Building pBuilding, int pLadderIndex);

	proto native 	HumanCommandLadder 			GetCommand_Ladder();


	//!----- LADDER -----

	//! starts command - Swim
	proto native 	HumanCommandSwim			StartCommand_Swim();

	proto native 	HumanCommandSwim 			GetCommand_Swim();


	//!----- VEHICLE -----

	//! starts command - vehicle
	proto native 	HumanCommandVehicle			StartCommand_Vehicle(Transport pTransport, int pTransportPositionIndex, int pVehicleSeat, bool fromUnconscious = false);

	proto native 	HumanCommandVehicle			GetCommand_Vehicle();


	//!----- CLIMB -----

	//! starts command - climb
	proto native 	HumanCommandClimb			StartCommand_Climb(SHumanCommandClimbResult pClimbResult, int pType);

	proto native 	HumanCommandClimb			GetCommand_Climb();

	//!----- Death -----

	//! starts command - death
	proto native 	HumanCommandDeathCallback	StartCommand_Death(int pType, float pDirection, typename pCallbackClass);

	proto native 	HumanCommandDeathCallback	GetCommand_Death();


	//! starts command - unconscious
	proto native 	HumanCommandUnconscious 	StartCommand_Unconscious(float pType);

	proto native 	HumanCommandUnconscious		GetCommand_Unconscious();


	//!----- FullBody Damages -----

	//! starts fullbody damage 
	// -> of type 0,...
	// -> from direction (lback -180,left -90,0,righ 90,rightback 180)
	proto native 	HumanCommandFullBodyDamage	StartCommand_Damage(int pType, float pDirection);

	proto native 	HumanCommandFullBodyDamage	GetCommand_Damage();



	//!----- ACTIONS ----- 

	//! starts command - Action 
	//! pStanceMask is mix of flags STANCEMASK_ERECT, ... 
	proto native 	HumanCommandActionCallback 	StartCommand_Action(int pActionID, typename pCallbackClass, int pStanceMask);

	//! is human is in command action - returns its callback, if current command is action 
	proto native	HumanCommandActionCallback 	GetCommand_Action();

	
	//---------------------------------------------------------
	// command modifiers

	//!
	proto native 	int 		GetCommandModifierCount();

	//! returns COMMANDID_ .. type id of command modifier on index pIndex
	proto native 	int 		GetCommandModifierID(int pIndex);	


	//! default (always-on modifiers)

	//! returns modifier for additives / small behaviours on human 
	proto native 	HumanCommandAdditives		GetCommandModifier_Additives();

	//! returns interface for handling weapons 
	proto native 	HumanCommandWeapons			GetCommandModifier_Weapons();




	// --- modifier/additive actions - played on COMMANDID_MOVE command

	//! adds action command modifier, creates callback instance for you 
	proto native 	HumanCommandActionCallback	AddCommandModifier_Action(int pActionID, typename pCallbackClass);

	//! force remove - normally if action is ended or interrupted - this is not needed to call 
	proto native 	void 						DeleteCommandModifier_Action(HumanCommandActionCallback pCallback);

	//! returns callback for action if any is active, null if nothing runs 
	proto native 	HumanCommandActionCallback	GetCommandModifier_Action();



	//!--- modifier for light Damages 

	//! starts additive damage 
	// -> of type 0,...
	// -> from direction (lback -180,left -90,0,righ 90,rightback 180)
	proto native 	HumanCommandDamage			AddCommandModifier_Damage(int pType, float pDirection);

	proto native 	void 						DeleteCommandModifier_Damage(HumanCommandDamage pDamage);

	proto native 	HumanCommandDamage			GetCommandModifier_Damage();


	//!--- SCRIPTED COMMANDS 

	//! starts command - Action 
	//! pStanceMask is mix of flags STANCEMASK_ERECT, ... 
	proto native 	HumanCommandScript			StartCommand_Script(HumanCommandScript pHumanCommand);
	proto native 	HumanCommandScript			StartCommand_ScriptInst(typename pCallbackClass);

	//! is human is in command action - returns its callback, if current command is action 
	proto native	HumanCommandScript			GetCommand_Script();



	//---------------------------------------------------------
	// debug info for Item Accessor


	//! returns current item's class name
	proto native   owned string  						DebugGetItemClass();

	//! returns current item's class that is found in config
	proto native   owned string  						DebugGetItemSuperClass();

	//! returns current item's animation instance 
	proto native   owned string  						DebugGetItemAnimInstance();


	//--------------------------------------------------------
	// commands start/finish events
	
	void	OnCommandMoveStart();
	void	OnCommandMoveFinish();

	void	OnCommandMeleeStart();
	void	OnCommandMeleeFinish();

	void	OnCommandMelee2Start();
	void	OnCommandMelee2Finish();

	void	OnCommandFallStart();
	void	OnCommandFallFinish();

	void	OnCommandClimbStart();
	void	OnCommandClimbFinish();

	void	OnCommandDeathStart();
	void	OnCommandDeathFinish();

	void	OnCommandUnconsciousStart();
	void	OnCommandUnconsciousFinish();

	void	OnCommandDamageFullbodyStart();
	void	OnCommandDamageFullbodyFinish();

	void	OnCommandDamageAdditiveStart();
	void	OnCommandDamageAdditiveFinish();

	void	OnCommandLadderStart();
	void	OnCommandLadderFinish();

	void	OnCommandSwimStart();
	void	OnCommandSwimFinish();

	void	OnCommandVehicleStart();
	void	OnCommandVehicleFinish();

	void	OnCommandActionFullbodyStart();
	void	OnCommandActionFullbodyFinish();

	void	OnCommandActionAdditiveStart();
	void	OnCommandActionAdditiveFinish();
	
	//! gets called on stance change
	void	OnStanceChange(int previousStance, int newStance);
	
	//! Called by code to see if it can
	bool	CanChangeStance(int previousStance, int newStance);
	
	//--------------------------------------------------------
	// rolling API
	
	bool	CanRoll();
	void	OnRollStart(bool isToTheRight);
	void	OnRollFinish();
}
