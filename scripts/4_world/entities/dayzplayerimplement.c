/*
DayZPlayerImplement

this file is implemenation of dayzPlayer in script
- logic of movement
- camera switching logic

*/

class DayZPlayerCommandDeathCallback : HumanCommandDeathCallback
{
	//----------------------------------
	// callbacks

	override void 	OnSimulationEnd()
	{
		if (LogManager.IsSyncLogEnable()) syncDebugPrint("DZPI::OnSimulationEnd - trying to drop item");
		
		if (GetGame().IsServer())
		{
			EntityAI itemInHands = m_pPlayer.GetHumanInventory().GetEntityInHands();
			if (itemInHands)
			{
				int boneIndex = m_pPlayer.GetBoneIndexByName("RightHand_Dummy");
			
				vector m4[4];
				m_pPlayer.GetBoneTransformWS(boneIndex, m4);
			
				m_pPlayer.GetInventory().DropEntityWithTransform(InventoryMode.SERVER, m_pPlayer, itemInHands, m4);
				
				if (GetCEApi())
				{
					int deadBodyLifetime = GetCEApi().GetCEGlobalInt("CleanupLifetimeDeadPlayer");
					if (deadBodyLifetime <= 0)
					{
						deadBodyLifetime = 3600;
					}
					itemInHands.SetLifetime(deadBodyLifetime);
				}
			}	
		}
	}

	override bool ShouldSimulationBeDisabled()
	{
		return m_pPlayer.m_WasInVehicle == false;
	}
	
	PlayerBase m_pPlayer;
}

class DayZPlayerVehicleCommandDeathCallback : HumanCommandDeathCallback
{
	override void OnSimulationEnd()
	{
		m_pPlayer.PhysicsEnableGravity(true);
	}
	
	PlayerBase m_pPlayer;
}

class DeathEffectTimer extends Timer
{
	 override void Stop()
	{
		super.Stop();
		
		PPERequesterBank.GetRequester(PPERequester_DeathDarkening).Stop();
	}
};

#ifdef FEATURE_NETWORK_RECONCILIATION

class DayZPlayerImplementOwnerState : DayZPlayerOwnerState
{
	float m_fLastHeadingDiff;

	protected override event void Write(PawnStateWriter ctx)
	{
		super.Write(ctx);

		ctx.Write(m_fLastHeadingDiff);
	}
	
	protected override event void Read(PawnStateReader ctx)
	{
		super.Read(ctx);

		ctx.Read(m_fLastHeadingDiff);
	}
};

class DayZPlayerImplementMove : DayZPlayerMove
{
	protected override event void Write(PawnMoveWriter ctx, PawnMove prev)
	{
		super.Write(ctx, prev);

	}
	
	protected override event void Read(PawnMoveReader ctx, PawnMove prev)
	{
		super.Read(ctx, prev);

	}
};

#endif

class DayZPlayerImplement extends DayZPlayer
{
	static const float DEFAULT_DYING_TIME = 2.5; 		//! how long does it take to full death screen [s]
	static const float DYING_PROGRESSION_TIME = 0.05; 	//! time step for gradual update of dead screen visibilibty up to full visbility [s]


	ref HumanMovementState 					m_MovementState = new HumanMovementState();	//!< cached ptr for movement state
	protected ref DayZPlayerImplementAiming				m_AimingModel;
	protected ref DayZPlayerImplementMeleeCombat		m_MeleeCombat;
	protected ref DayZPlayerMeleeFightLogic_LightHeavy	m_MeleeFightLogic;
	protected ref DayZPlayerImplementSwimming			m_Swimming;
	protected ref DayZPlayerImplementThrowing			m_Throwing;
	protected ref DayZPlayerImplementJumpClimb			m_JumpClimb;
	protected ref DayZPlayerImplementFallDamage			m_FallDamage;
	protected ref PlayerSoundEventHandler 				m_PlayerSoundEventHandler;
	protected ref ReplaceSoundEventHandler 				m_ReplaceSoundEventHandler;
	protected SHumanCommandMoveSettings 				m_MoveSettings;
	protected float 									m_FallYDiff;
	protected float										m_SprintedTime;
	protected float										m_SprintedTimePerStanceMin;
	protected bool										m_SprintFull;
	protected bool										m_IsRaised;
	protected bool										m_ShouldReload;
	protected bool										m_Camera3rdPerson;
	protected bool										m_CameraZoomToggle;
	protected bool										m_bADS;
	private float 										m_WeaponRaiseTime; //! measures time from the weapon raise start to complete raise (once per)
	protected bool 										m_WeaponRaiseCompleted;
	protected ECameraZoomType							m_CameraEyeZoomLevel;
	protected bool										m_CameraIronsight; // Ironsight NOW!
	protected bool										m_CameraOptics; // Optics NOW!
	protected bool										m_CameraOpticsAimOverride;	// for tracking if aim limit is currently overriden by optics

	protected float 									m_DeathDarkeningCurrentTime;
	protected bool										m_IsTryingHoldBreath;
	protected bool										m_IsShootingFromCamera;
	protected bool										m_PlayerSelected;
	protected bool 										m_Suicide;
	protected bool										m_IsUnconscious;
	protected bool										m_ShouldBeUnconscious;
	protected bool										m_IsUnconsciousFalling;
	bool												m_UnconsciousDebug;
	protected int			 							m_LastCommandBeforeUnconscious;
	ref WeaponDebug										m_WeaponDebug;
	ref DeathEffectTimer 								m_DeathEffectTimer;
	ref Timer 											m_FightEndBlendTimer;
	protected bool										m_ProcessFirearmMeleeHit;
	protected bool										m_ContinueFirearmMelee;
	protected bool 										m_LiftWeapon_player;
	protected float										m_ObstructWeapon_player;
	protected bool 										m_ProcessLiftWeapon;
	protected bool 										m_ProcessLiftWeaponState;
	protected float										m_ProcessObstructWeapon;
	protected int										m_LastSurfaceUnderHash;
	protected Transport									m_TransportCache;
	protected string 									m_ClimbingLadderType;
	bool												m_isFBsymptomPlaying;
	protected bool 										m_HandheldOpticsInUse;
	protected bool 										m_ResetADS;
	protected bool 										m_ProcessResetADS;
	protected int										m_StepCounter;
	protected int 										m_NextVoNNoiseTime;
	protected ref array<ref SyncHitInfo>				m_SyncedHitDataArray;
	protected ref array<AbstractWave>					m_PerformedActionSounds;
	
	// aiming model helpers	
	protected bool										m_RaiseStarted = false;
	protected bool										m_AimingFinisherStarted = false;
	protected bool										m_IsWeapon;
	
	private float										m_CurrentWaterLevel;

	bool												m_WasInVehicle;
	protected bool										m_TriggerPullPlayerOutOfVehicleSynch;
	protected bool										m_PullPlayerOutOfVehicleKeepsInLocalSpace = false;
	protected int										m_PullPlayerOutOfVehicleState = -1;
	int													m_ActionSoundCategoryHash;
	
	protected float										m_dT;

	protected float m_fObstructionSmooth;
	protected float m_fObstructionSmoothVelocity[1];
	
	//! constructor 
	void 	DayZPlayerImplement()
	{
		SetEventMask(EntityEvent.CONTACT);

		m_SprintFull = false;
		m_SprintedTime = 0;
		m_SprintedTimePerStanceMin = PlayerConstants.FULL_SPRINT_DELAY_DEFAULT;
		m_AimingModel = new DayZPlayerImplementAiming(this);
		m_MeleeCombat = new DayZPlayerImplementMeleeCombat(this);
		m_MeleeFightLogic = new DayZPlayerMeleeFightLogic_LightHeavy(this);
		m_Swimming = new DayZPlayerImplementSwimming(this);		
		m_Throwing = new DayZPlayerImplementThrowing(this);
		m_JumpClimb = new DayZPlayerImplementJumpClimb(this);
		m_FallDamage = new DayZPlayerImplementFallDamage(this);
		m_ADSAutomationTimer = new Timer();
		m_bADS = false;
		m_WeaponRaiseCompleted = false;
		SetOptics(false);
		m_IsShootingFromCamera = true;
		m_ProcessFirearmMeleeHit = false;
		m_ContinueFirearmMelee = false;
		#ifdef PLATFORM_CONSOLE
		m_Camera3rdPerson = !GetGame().GetWorld().Is3rdPersonDisabled();
		#endif
		m_LastSurfaceUnderHash = ("cp_gravel").Hash();
		m_NextVoNNoiseTime = 0;
		m_SyncedHitDataArray = new array<ref SyncHitInfo>;
		m_PerformedActionSounds = new array<AbstractWave>();
		m_CurrentWaterLevel = 0;
		m_WeaponRaiseTime = 0;
		
		RegisterNetSyncVariableBoolSignal("m_TriggerPullPlayerOutOfVehicleSynch");
	}

#ifdef FEATURE_NETWORK_RECONCILIATION
	protected override event typename GetOwnerStateType()
	{
		return DayZPlayerImplementOwnerState;
	}
	
	protected override event typename GetMoveType()
	{
		return DayZPlayerImplementMove;
	}

	protected override event void ObtainState(/*inout*/ PawnOwnerState pState)
	{
		super.ObtainState(pState);

		DayZPlayerImplementOwnerState state = DayZPlayerImplementOwnerState.Cast(pState);

		state.m_fLastHeadingDiff = m_fLastHeadingDiff;
	}

	protected override event void RewindState(PawnOwnerState pState, /*inout*/ PawnMove pMove, inout NetworkRewindType pRewindType)
	{
		super.RewindState(pState, pMove, pRewindType);

		DayZPlayerImplementOwnerState state = DayZPlayerImplementOwnerState.Cast(pState);

		if (pRewindType != NetworkRewindType.ADDITIVE)
		{
			m_fLastHeadingDiff = state.m_fLastHeadingDiff;
		}
	}
#endif

	DayZPlayerImplementAiming GetAimingModel()
	{
		return m_AimingModel;
	}

	DayZPlayerImplementMeleeCombat GetMeleeCombat()
	{
		return m_MeleeCombat;
	}
	
	DayZPlayerMeleeFightLogic_LightHeavy GetMeleeFightLogic()
	{
		return m_MeleeFightLogic;
	}
	
	DayZPlayerImplementThrowing GetThrowing()
	{
		return m_Throwing;
	}
	
	DayZPlayerImplementFallDamage GetFallDamage()
	{
		return m_FallDamage;
	}
	
	void ~DayZPlayerImplement()
	{
	}

	//! Legacy
	override bool IsEyeZoom()
	{
		return GetEyeZoomLevel() > 0;
	}
	
	override int GetEyeZoomLevel()
	{
		return m_CameraEyeZoomLevel;
	}
	
	override bool IsShootingFromCamera()
	{
		return m_IsShootingFromCamera;
	}
	
	void OverrideShootFromCamera(bool pState)
	{
		m_IsShootingFromCamera = pState;
	}

	bool IsInIronsights()
	{
		return m_CameraIronsight;
	}
	
	bool IsInOptics()
	{
		return m_CameraOptics;
	}
	
	override bool IsInThirdPerson()
	{
		return m_Camera3rdPerson;
	}
	
	void SetIsInThirdPerson(bool state)
	{
		m_Camera3rdPerson = state;
	}

	bool IsFireWeaponRaised()
	{
		GetMovementState(m_MovementState);
		if (m_MovementState)
			return m_MovementState.IsRaised();
		
		return false;
	}

	bool IsTryingHoldBreath()
	{
		return m_IsTryingHoldBreath;
	}

	bool IsSprintFull()
	{
		return m_SprintFull;
	}

	void ShowWeaponDebug(bool show)
	{
		if (show)
		{
			m_WeaponDebug = new WeaponDebug;
		}
		else
		{
			m_WeaponDebug = null;
		}
	}

	bool IsWeaponDebugEnabled()
	{
		return m_WeaponDebug != null;
	}
	
	void SetFallYDiff(float value)
	{
		m_FallYDiff = value;
	}
	
	override float GetCurrentWaterLevel()
	{
		return m_CurrentWaterLevel;
	}
	
	override void SetCurrentWaterLevel(float pWaterLevel)
	{
		m_CurrentWaterLevel = pWaterLevel;
	}

	void SetIronsights(bool value)
	{
		Weapon_Base weapon = Weapon_Base.Cast(GetHumanInventory().GetEntityInHands());
		if (weapon)
			weapon.SetWasIronSight(m_CameraIronsight);
		
		m_CameraIronsight = value;
		
		if (value)
		{
			HumanCommandWeapons hcw = GetCommandModifier_Weapons();
			if (hcw)
				hcw.SetADS(true);
			
			HumanInputController hic = GetInputController();
			if (hic)
				hic.ResetFreeLookToggle();
		}
	}
	
	void SetOptics(bool value)
	{
		m_CameraOptics = value;
		
		if (value)
		{
			HumanCommandWeapons hcw = GetCommandModifier_Weapons();
			if (hcw)
				hcw.SetADS(!IsHandheldOpticsInUse());
			
			HumanInputController hic = GetInputController();
			if (hic)
				hic.ResetFreeLookToggle();
		}
	}
	
	void ExitSights()
	{
		ItemOptics optics = null;
		EntityAI entityInHands = GetHumanInventory().GetEntityInHands();
		Weapon weapon = Weapon.Cast(entityInHands);
		if (weapon)
		{
			optics = weapon.GetAttachedOptics();
		}
		else
		{
			optics = ItemOptics.Cast(entityInHands);
		}
		
		SetIronsights(false);
		SetOptics(false);
		
		if (optics)
		{
			SwitchOptics(optics,false);
		}
		
		HumanCommandWeapons hcw = GetCommandModifier_Weapons();
		if (hcw)
		{
			hcw.SetADS(false);
		}
	}
	
	void SwitchOptics(ItemOptics optic, bool state)
	{
		if (optic && !optic.IsSightOnly())
		{
			if (state)
			{
				if (optic.HasEnergyManager())
					optic.GetCompEM().SwitchOn();

				optic.EnterOptics();
				optic.OnOpticEnter();
			}
			else
			{
				optic.ExitOptics();
				optic.OnOpticExit();
				if (optic.HasEnergyManager())
					optic.GetCompEM().SwitchOff();
			}
		}
		
		if (m_CameraOptics != state)
		{
			SetOptics(state);
		}
	}

	void SetClimbingLadderType(string value)
	{
		m_ClimbingLadderType = value;
	}

	//! Implementations only! - used on PlayerBase
	bool CanConsumeStamina(EStaminaConsumers consumer) {};
	bool CanStartConsumingStamina(EStaminaConsumers consumer) {};
	void DepleteStaminaEx(EStaminaModifiers modifier, float dT = -1, float coef = 1.0) {};
	
	bool IsInVehicle()
	{
		return m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_VEHICLE || (GetParent() != null && GetParent().IsInherited(Transport));
	}
	
	bool IsSwimming()
	{
		return m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_SWIM;
	}
	
	bool IsClimbingLadder()
	{
		return m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_LADDER;
	}
	
	//! Handle sound event by respective sound handler based on its type
	bool PlaySoundEventType(ESoundEventType soundType, int soundEventID, int param = 0) {};
	bool PlaySoundEvent(EPlayerSoundEventID id, bool from_anim_system = false, bool is_from_server = false) {};
	bool PlaySoundEventEx(EPlayerSoundEventID id, bool from_anim_system = false, bool is_from_server = false, int param = 0) {};

	bool IsFBSymptomPlaying()
	{
		return m_isFBsymptomPlaying;
	}
	
	float GetDeltaT()
	{
		return m_dT;
	}

	//-------------------------------------------------------------
	//!
	//! HandleDeath
	//! 

	int m_DeathAnimType = -2;
	float m_DeathHitDir = 0;
	bool m_DeathJuctureSent = false;
	
	//! Get the transport that was cached when entering unconsciousness
	Transport GetTransportCache()
	{
		return m_TransportCache;
	}
	
	override string GetDebugText()
	{
		string text = super.GetDebugText();
		text += "Parent: " + Object.GetDebugName(Object.Cast(GetParent())) + "\n";//keep
		text += "IsSimulationDisabled: " + GetIsSimulationDisabled() + "\n";//keep
		/*
		text += "DamageDestroyed: " + IsDamageDestroyed() + "\n";
		text += "DeathProcessed: " + IsDeathProcessed() + "\n";
		text += "DeathConditionMet: " + IsDeathConditionMet() + "\n";
		text += "PullOutOfVehicleState: " + m_PullPlayerOutOfVehicleState + "\n";
		text += "PullOutOfVehicleSynch: " + m_TriggerPullPlayerOutOfVehicleSynch + "\n";
		text += "Position: " + GetPosition() + "\n";*/
		return text;
	}

	void TriggerPullPlayerOutOfVehicle()
	{
		if (!GetGame().IsServer())
		{
			return;
		}
		
		Transport transport;
		if (!Class.CastTo(transport, GetParent()))
		{
			return;
		}
		
		int crewIdx = -1;
		for (int i = 0; i < transport.CrewSize(); ++i)
		{
			if (transport.CrewMember(i) == this)
			{
				crewIdx = i;
				break;
			}
		}
		
		if (crewIdx == -1)
		{
			return;
		}

		transport.CrewGetOut(crewIdx);
		TriggerPullPlayerOutOfVehicleImpl();

		if (crewIdx == DayZPlayerConstants.VEHICLESEAT_DRIVER)
		{
			//! we don't want the vehicle to be haunted, re-possess the dead player so other players can use the vehicle
						
#ifdef FEATURE_NETWORK_RECONCILIATION
			PlayerIdentity identity = GetIdentity();

			if (identity)
			{
				//! And now we want to resume control of the player 
				identity.Possess(this);
			}
#endif
		}
		
		SetSynchDirty();
	}

	void TriggerPullPlayerOutOfVehicleImpl()
	{
		//! Reset the cooldown so simulation doesn't automatically get re-disabled
		ResetDeathCooldown();

		//! Synced to re-enable simulation on client as the death processing sets disable simulation to local only
		DisableSimulation(false);

		switch (GetInstanceType())
		{
			case DayZPlayerInstanceType.INSTANCETYPE_SERVER:
			case DayZPlayerInstanceType.INSTANCETYPE_CLIENT:		//! Handle own client animation freeze
			case DayZPlayerInstanceType.INSTANCETYPE_AI_SERVER:
			case DayZPlayerInstanceType.INSTANCETYPE_AI_SINGLEPLAYER:
				m_PullPlayerOutOfVehicleState = 0;
				m_TriggerPullPlayerOutOfVehicleSynch = true;
				break;
		}
	}
	
	override void OnVariablesSynchronized()
	{
		if (m_TriggerPullPlayerOutOfVehicleSynch && !GetGame().IsDedicatedServer())
		{
			TriggerPullPlayerOutOfVehicleImpl();
		}

		super.OnVariablesSynchronized();
	}

	bool HandleDeath(int pCurrentCommandID)
	{
		if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_DEATH)
		{
			switch (m_PullPlayerOutOfVehicleState)
			{
			case -1:
				break;
			case 0:
				m_PullPlayerOutOfVehicleState = -1;

				PhysicsSetSolid(true);
				
				//! Can't pull a body out of a car that is in water, don't need to handle that
				DayZPlayerVehicleCommandDeathCallback callbackVeh;
				Class.CastTo(callbackVeh, StartCommand_Death(DayZPlayerConstants.DEATH_PULL_OUT_TRANSPORT, 0, DayZPlayerVehicleCommandDeathCallback, m_PullPlayerOutOfVehicleKeepsInLocalSpace));
				Class.CastTo(callbackVeh.m_pPlayer, this);
				break;
			}	

			return true;
		}
		
		if (m_DeathAnimType != -2 && g_Game.GetMissionState() == g_Game.MISSION_STATE_GAME)
		{
			if (!CommitedSuicide()) 	
			{
				int type = m_DeathAnimType;
				if (type == DayZPlayerConstants.DEATH_DEFAULT) 
					type = GetTypeOfDeath(pCurrentCommandID);

				m_WasInVehicle = false;				
				HumanCommandVehicle hcv = GetCommand_Vehicle();
				if (hcv)
				{
					m_TransportCache = hcv.GetTransport();
					m_WasInVehicle = !hcv.IsGettingIn() && !hcv.IsGettingOut();
				}

				int seatIndex = -1;
				if (m_TransportCache)
				{
					seatIndex = m_TransportCache.CrewMemberIndex(this);
				}

				//! player looks to be dead now, not unconscious
				m_ShouldBeUnconscious = false;

				bool keepInLocalSpace = m_PullPlayerOutOfVehicleKeepsInLocalSpace;
				if (m_TransportCache)
				{
					if (IsUnconscious() || m_WasInVehicle)
					{
						keepInLocalSpace = true;
						m_TransportCache.CrewDeath(seatIndex);
					}
					else
					{
						m_TransportCache.CrewGetOut(seatIndex);
					}

					m_TransportCache.MarkCrewMemberDead(seatIndex);
					
					if (seatIndex == DayZPlayerConstants.VEHICLESEAT_DRIVER)
					{
						//! we don't want the vehicle to be haunted, re-possess the dead player so other players can use the vehicle
						
#ifdef FEATURE_NETWORK_RECONCILIATION
						PlayerIdentity identity = GetIdentity();

						if (identity)
						{
							//! And now we want to resume control of the player 
							identity.Possess(this);
						}
#endif
					}
				}
				
				DisableSimulation(false);
				GetItemAccessor().HideItemInHands(false);
				m_TransportCache = null;

				DayZPlayerCommandDeathCallback callback;
				Class.CastTo(callback, StartCommand_Death(type, m_DeathHitDir, DayZPlayerCommandDeathCallback, keepInLocalSpace));
				Class.CastTo(callback.m_pPlayer, this);
			}
			else
			{
				PhysicsSetRagdoll(true);
			}
			
			// disable voice communication
			GetGame().GetWorld().SetVoiceOn(false, false);
			
			return true;
		}

		return false;
	}
	
	void ResetDeathStartTime()
	{
		ResetDeathCooldown();
	}
	
	int	GetTypeOfDeath(int pCurrentCommandID)
	{
		switch (pCurrentCommandID)
		{
			case DayZPlayerConstants.COMMANDID_SWIM:
				return DayZPlayerConstants.DEATH_WATER;
			case DayZPlayerConstants.COMMANDID_FALL:
				return DayZPlayerConstants.DEATH_FALL;
			case DayZPlayerConstants.COMMANDID_UNCONSCIOUS:
				HumanCommandUnconscious hcu = GetCommand_Unconscious();
				if (hcu)
				{
					if (hcu.IsOnLand())
						return DayZPlayerConstants.DEATH_UNCON_ON_LAND;
					if (hcu.IsInWater())
						return DayZPlayerConstants.DEATH_UNCON_IN_WATER;
				}

				break;
		}
		
		return DayZPlayerConstants.DEATH_BODY;
	}
	
	void SendDeathJuncture(int pAnimTypeDeath, float pAnimHitDirDeath)
	{
		if (m_DeathJuctureSent)
			return;
		
		DayZPlayerSyncJunctures.SendDeath(this, pAnimTypeDeath, pAnimHitDirDeath);
		m_DeathJuctureSent = true;
	}
	
	override void EEKilled(Object killer)
	{
		SendDeathJuncture(-1, 0);
		
		super.EEKilled(killer);
	}
	
	void ShowDeadScreen(bool show, float duration)
	{		
		#ifndef NO_GUI
		if (show && IsPlayerSelected())
		{
			#ifdef PLATFORM_PS4
			OnlineServices.SetMultiplayState(false);
			#endif		
			
			string message = "";
			if (!GetGame().GetMission().IsPlayerRespawning())
			{
				message = "#dayz_implement_dead";
			}

			#ifdef PLATFORM_CONSOLE
			GetGame().GetUIManager().ScreenFadeIn(duration, message, FadeColors.DARK_RED, FadeColors.WHITE);
			#else
			GetGame().GetUIManager().ScreenFadeIn(duration, message, FadeColors.BLACK, FadeColors.WHITE);
			#endif
		}
		else
		{
			GetGame().GetUIManager().ScreenFadeOut(duration);

			if (duration > 0)
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(StopDeathDarkeningEffect, duration*1000, false);
			else
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(StopDeathDarkeningEffect);
		}
		#endif
	}
	
	override void StopDeathDarkeningEffect()
	{
		if (m_DeathEffectTimer && m_DeathEffectTimer.IsRunning())
		{
			m_DeathEffectTimer.Stop();
			m_DeathEffectTimer = null;
		}
	}
	
	void SimulateDeath(bool state)
	{
		if (g_Game.GetMissionState() != DayZGame.MISSION_STATE_GAME)
			return;
		
		LockControls(state);
		
		if (state)
		{
			float duration = DEFAULT_DYING_TIME;
			if (m_KilledByHeadshot)
			{
				duration = 0;
			}
			
			if (duration > DYING_PROGRESSION_TIME && !m_IsUnconscious)
			{
				if (!m_DeathEffectTimer)
					m_DeathEffectTimer = new DeathEffectTimer();
				
				Param2<float,float> par = new Param2<float,float>(duration, DYING_PROGRESSION_TIME);
				m_DeathEffectTimer.Run(DYING_PROGRESSION_TIME, this, "SetDeathDarknessLevel", par, true);
			}
			
			ShowDeadScreen(state, duration);
			
			GetGame().GetSoundScene().SetSoundVolume(0,0);
			GetGame().GetSoundScene().SetSpeechExVolume(0,0);
			GetGame().GetSoundScene().SetMusicVolume(0,0);
			GetGame().GetSoundScene().SetVOIPVolume(0,0);
			GetGame().GetSoundScene().SetRadioVolume(0,0);
		}
		else
		{
			StopDeathDarkeningEffect();
			ShowDeadScreen(state, 0);
		}
	}
	
	void LockControls(bool state)
	{
		if (state == true)
		{
			GetGame().GetInput().ChangeGameFocus(1, INPUT_DEVICE_MOUSE);
			GetGame().GetInput().ChangeGameFocus(1, INPUT_DEVICE_KEYBOARD);
			GetGame().GetInput().ChangeGameFocus(1, INPUT_DEVICE_GAMEPAD);
			
			if (GetGame().GetUIManager()) 	
			{
				GetGame().GetUIManager().ShowUICursor(true);
				if (GetGame().GetUIManager().IsDialogVisible())
					GetGame().GetUIManager().CloseDialog();
			}
		}
		else
		{
			GetGame().GetInput().ChangeGameFocus(-1, INPUT_DEVICE_MOUSE);
			GetGame().GetInput().ChangeGameFocus(-1, INPUT_DEVICE_KEYBOARD);
			GetGame().GetInput().ChangeGameFocus(-1, INPUT_DEVICE_GAMEPAD);
			
			if (GetGame().GetUIManager())
			{
				if (GetGame().GetUIManager().GetMenu())
				{
					GetGame().GetUIManager().ShowUICursor(true);
				}
				else
				{
					GetGame().GetUIManager().ShowUICursor(false);
				}
			}
		}
	}
	
	void SetDeathDarknessLevel(float duration, float tick_time);
	
	void CompleteWeaponRaise()
	{
		m_WeaponRaiseTime = 0;
		m_WeaponRaiseCompleted = true;
		HumanInputController hic = GetInputController();
		if (hic)
			hic.ResetFreeLookToggle();
	}
	
	void ResetWeaponRaiseProgress()
	{
		m_WeaponRaiseTime = 0;
		m_WeaponRaiseCompleted = false;
	}
	
	bool IsWeaponRaiseCompleted()
	{
		return m_WeaponRaiseCompleted;
	}
	
	DayZPlayerInventory GetDayZPlayerInventory()
	{
		DayZPlayerInventory inv = DayZPlayerInventory.Cast(GetInventory());
		return inv;
	}

	override void OnInputForRemote(ParamsReadContext ctx)
	{
		int userDataType = 0;
		if (!ctx.Read(userDataType))
		{
			Error("DayZPlayer: OnInputForRemote - cannot read input type");
			return;
		}
		
		switch (userDataType)
		{
			case INPUT_UDT_WEAPON_REMOTE_EVENT:
				GetDayZPlayerInventory().OnEventForRemoteWeapon(ctx);
				break;
			case INPUT_UDT_HAND_REMOTE_EVENT:
				GetDayZPlayerInventory().OnHandEventForRemote(ctx);
				break;
			case INPUT_UDT_INVENTORY:
				GetDayZPlayerInventory().OnInputUserDataForRemote(ctx);
				break;
			default:
				Error("OnInputForRemote - unknown userDataType=" + userDataType);
				break;
		}
	}

	override void OnInputFromServer (ParamsReadContext ctx)
	{
		int userDataType = 0;
		if (!ctx.Read(userDataType))
		{
			Error("DayZPlayer: OnInputFromServer - cannot read input type");
			return;
		}

		switch (userDataType)
		{
			case INPUT_UDT_INVENTORY:
				GetDayZPlayerInventory().OnServerInventoryCommand(ctx);
				break;
			default:
				Error("OnInputFromServer - unknown userDataType=" + userDataType);
				break;
		}
	}
	void RequestSoundEvent(EPlayerSoundEventID id, bool from_server_and_client = false);
	void RequestSoundEventEx(EPlayerSoundEventID id, bool from_server_and_client = false, int param = 0);
	protected void SendSoundEvent(EPlayerSoundEventID id);
	protected void SendSoundEventEx(EPlayerSoundEventID id, int param = 0);
	
	override void OnItemInHandsChanged()
	{
		//Print("inv: DayZPlayerImplement::OnItemInHandsChanged: "/* + GetHumanInventory().GetEntityInHands()*/);
		StopWeaponEvent();
		GetItemAccessor().OnItemInHandsChanged();
		
		if (!IsAlive())
		{
			//Print("inv: DayZPlayerImplement::OnItemInHandsChanged - human not alive! calling ResetWeaponInHands");
			GetItemAccessor().ResetWeaponInHands();
		}
		
		HumanInventoryWithFSM.Cast(GetHumanInventory()).CheckFSMState();
	}

	WeaponManager GetWeaponManager() { return null; }

	//-------------------------------------------------------------
	//!
	//! HandleWeapons
	//!
	void HandleWeapons(float pDt, Entity pInHands, HumanInputController pInputs, out bool pExitIronSights)
	{
		HumanCommandWeapons		hcw = GetCommandModifier_Weapons();
    	GetDayZPlayerInventory().HandleWeaponEvents(pDt, pExitIronSights);

		Weapon_Base weapon;
		Class.CastTo(weapon, pInHands);
		ItemOptics optic = weapon.GetAttachedOptics();
		
		CheckLiftWeapon();
		ProcessLiftWeapon();
		
		GetMovementState(m_MovementState);
		
		// obstruction
		bool 	wantedLift 			= m_LiftWeapon_player;
		bool 	currentLift 		= hcw.IsWeaponLifted();
		if (wantedLift != currentLift) 
		{
			hcw.LiftWeapon(wantedLift);
			// Reset the velocity, cause at this point we might need to start moving in the opposite direction rather rapidly,
			// to prevent the obstruction from smashing the player's head while lifting
			m_fObstructionSmoothVelocity[0] = 0;
		}		
		
		float 	wantedObstruction 	= m_ObstructWeapon_player;
		float 	currentObstruction 	= hcw.GetWeaponObstruction();
		if (wantedObstruction != currentObstruction)
		{
			// The following times are times chosen by observation; the following values produced
			// consistent and relatively nicely looking results and were OK'd by the animators.
			const float outTime    = 0.150;   // duration when smoothing hi -> lo
			const float inTime     = 0.010;   // duration when smoothing lo -> hi
			const float inTimeLift = 0.100;   // duration when smoothing in during lift
			
			float smoothTime;
			if (wantedLift) // When lifting always transition rather quickly
			{
				wantedObstruction = 0.0;
				smoothTime = inTimeLift;
			}
			else // Otherwise we can take our time based on the delta
			{
				float t = Math.Clamp(Math.AbsFloat(wantedObstruction-currentObstruction), 0, 1);
				smoothTime = Math.Lerp(outTime, inTime, t);
			}
			
			// Do the interpolation and clamp to wanted value if the change is below certain threshold, to prevent uneccessary interpolation
			m_fObstructionSmooth = Math.SmoothCD(m_fObstructionSmooth, wantedObstruction, m_fObstructionSmoothVelocity, smoothTime, 6.0, pDt);
			if (Math.AbsFloat(m_fObstructionSmooth-wantedObstruction) < 0.0001) 
			{ 
				m_fObstructionSmooth = wantedObstruction;
			}
			
			#ifdef DIAG_DEVELOPER
			if (DiagMenu.GetValue(DiagMenuIDs.WEAPON_DISABLE_OBSTRUCTION_INTERPOLATION))
				m_fObstructionSmooth = wantedObstruction;
			#endif
				
			hcw.ObstructWeapon(m_fObstructionSmooth);
			
			#ifndef SERVER
			#ifdef DIAG_DEVELOPER
			PluginDiagMenuClient.GetWeaponLiftDiag().Data().SetInterpolation( inTime, inTimeLift, outTime, smoothTime, m_fObstructionSmooth, currentObstruction, wantedObstruction );
			#endif
			#endif
		}
		// !obstruction
		
		// hold breath
		if (pInputs.IsHoldBreath() && m_MovementState.IsRaised() && (IsInIronsights() || IsInOptics()))
		{
			m_IsTryingHoldBreath = true;
		}
		else
		{
			m_IsTryingHoldBreath = false;
		}
		
		if (pInputs.IsFireModeChange())
		{
			GetWeaponManager().SetNextMuzzleMode();
		}
		if (pInputs.IsZeroingUp())
		{
			if (optic && (optic.IsInOptics() || optic.IsUsingWeaponIronsightsOverride()))
			{
				optic.StepZeroingUp();
			}
			else
			{
				weapon.StepZeroingUpAllMuzzles();
			}
		}
		if (pInputs.IsZeroingDown())
		{
			if (optic && (optic.IsInOptics() || optic.IsUsingWeaponIronsightsOverride()))
			{
				optic.StepZeroingDown();
			}
			else
			{
				weapon.StepZeroingDownAllMuzzles();
			}
		}
		
		if (!m_LiftWeapon_player && (m_CameraIronsight || !weapon.CanEnterIronsights() || m_CameraOptics/*m_ForceHandleOptics*/)) 	// HACK straight to optics, if ironsights not allowed
		{
			if (optic)
				HandleOptic(optic, false, pInputs, pExitIronSights);
		}
		
		if (!m_MovementState.IsRaised())
		{
			m_IsFireWeaponRaised = false; //legacy reasons
			if (weapon && weapon.IsInOptics())
			{
				weapon.ExitOptics();
			}
			
			ResetWeaponRaiseProgress();

			return; // if not raised => return
		}
		else
		{
			m_IsFireWeaponRaised = true; //legacy reasons
			if (!IsWeaponRaiseCompleted())
			{
				m_WeaponRaiseTime += pDt;
			}
			
			if (m_WeaponRaiseTime >= PlayerConstants.WEAPON_RAISE_BLEND_DELAY)
			{
				CompleteWeaponRaise();
			}
		}
		
		//! fire
		if (GetWeaponManager().CanFire(weapon))
		{
			bool autofire = weapon.GetCurrentModeAutoFire(weapon.GetCurrentMuzzle()) && weapon.IsChamberEjectable(weapon.GetCurrentMuzzle());
			int burst = weapon.GetCurrentModeBurstSize(weapon.GetCurrentMuzzle());
			int burst_count = weapon.GetBurstCount();
			if (!autofire && (burst < 2 || burst_count < 1))
			{
				if (pInputs.IsAttackButtonDown() && GetInputInterface().SyncedValue("UAWeaponMeleeAttack") == 0 && GetInputInterface().SyncedValue("UAHeavyMeleeAttack") == 0)
				{
					GetWeaponManager().Fire(weapon);
				}
			}
			else if (autofire || burst > 1)
			{
#ifdef DIAG_DEVELOPER
				int burst_option = GetWeaponManager().GetBurstOption();
				if (burst_option == 0)
				{
#endif
					if (pInputs.IsAttackButton() && GetInputInterface().SyncedValue("UAWeaponMeleeAttack") == 0 && GetInputInterface().SyncedValue("UAHeavyMeleeAttack") == 0)
					{
						if (autofire || burst_count < burst)
						{
							GetWeaponManager().Fire(weapon);
						}
					}
					else
					{
						weapon.ResetBurstCount();	
					}
#ifdef DIAG_DEVELOPER
				}
				else if (burst_option == 1)
				{
					if (burst > 1 && burst_count == burst)
					{
						weapon.ResetBurstCount();	
					}
					else if (burst > 1 && burst_count < burst)
					{
						GetWeaponManager().Fire(weapon);	
					}
					else
					{
						//Autofire
						if (pInputs.IsAttackButton())
						{
							GetWeaponManager().Fire(weapon);	
						}						
					}	
				}
#endif
			}
		}

		#ifdef PLATFORM_CONSOLE
		if (GetGame().GetInput().LocalRelease("UAFire", false) || m_ShouldReload)
		{
			if (!weapon.IsWaitingForActionFinish() && !IsFighting())
			{
				int muzzle_index = weapon.GetCurrentMuzzle();
			
				if (weapon.IsChamberFiredOut(muzzle_index))
				{
					if (weapon.CanProcessWeaponEvents())
					{
						if (GetWeaponManager().CanEjectBullet(weapon))
						{
							GetWeaponManager().EjectBullet();
							pExitIronSights = true;
							m_ShouldReload = false;
						}
					}
				}
			}
			else
			{
				m_ShouldReload = true;
			}
		}
		#endif
	}
	
	void HandleOptic(notnull ItemOptics optic, bool inHands, HumanInputController pInputs, out bool pExitOptics)
	{
		UAInterface input = GetInputInterface();
		if (!input)
		{
			return;
		}

		Weapon_Base weapon;
		int FOVcount;
		bool controllerPressIn;
		bool controllerPressOut;
		
		if (input.SyncedPress_ID(UAZoomInOptics))
		{
			weapon = Weapon_Base.Cast(optic.GetHierarchyParent());
			FOVcount = optic.GetStepFOVCount();
			controllerPressIn = input.SyncedPress_ID(UAZoomInOpticsControllerHelper);
		
			if (weapon && m_CameraIronsight && !m_CameraOptics)
			{
				SetIronsights(false);
				SwitchOptics(optic,true);
				optic.SetStepFOVIndex(0);
			}
			else if (m_CameraOptics)
			{
				if (controllerPressIn) //controller
				{
					if (!optic.StepFOVUp())
					{
						if (FOVcount > 0)
						{
							optic.SetStepFOVIndex(0); //loop to minimum magnification
						}
						
						if (weapon && weapon.CanEnterIronsights()) //loop into ironsights
						{
							SwitchOptics(optic,false);
							SetIronsights(true);
						}
					}
				}
				else //m&k
				{
					optic.StepFOVUp();
				}
			}
		}
		
		if (input.SyncedPress_ID(UAZoomOutOptics))
		{
			weapon = Weapon_Base.Cast(optic.GetHierarchyParent());
			FOVcount = optic.GetStepFOVCount();
			controllerPressOut = input.SyncedPress_ID(UAZoomOutOpticsControllerHelper);
			if (m_CameraOptics)
			{
				if (!optic.StepFOVDown())
				{
					if (controllerPressOut)
					{
						if (FOVcount > 0 && (!weapon || !weapon.CanEnterIronsights()))
						{
							optic.SetStepFOVIndex(FOVcount - 1); //loop to maximum magnification
						}
					}
					
					if (weapon && weapon.CanEnterIronsights())
					{
						SwitchOptics(optic,false);
						SetIronsights(true);
					}
				}
			}
			else if (controllerPressOut && weapon && m_CameraIronsight)
			{
				SwitchOptics(optic,true);
				if (FOVcount > 0)
				{
					optic.SetStepFOVIndex(FOVcount - 1); //loop to maximum magnification
				}
			}
		}
	}

	//-------------------------------------------------------------
	//!
	//! HandleDamageHit
	//! 

	bool m_DamageHitFullbody = false;
	int m_DamageHitAnimType = -1.0;
	float m_DamageHitDir = 0.0;
	float m_DamageHealth = 0.0;
	
	const float HIT_INTERVAL_MIN = 0.3; 		// Minimum time in seconds before a COMMANDID_HIT to COMMANDID_HIT transition is allowed
	float m_HitElapsedTime = HIT_INTERVAL_MIN;

	bool IsInFullbodyDamageAnimation()
	{
		return (m_DamageHitFullbody && m_DamageHitAnimType != -1) || GetCommand_Damage() != null;
	}

	//! Must be ran at the start of CommandHandler before Jump is triggered
	void EvaluateDamageHit(int pCurrentCommandID)
	{
		//! Reset damage hit to prevent animation from playing unnecessarily when it is already cancelled out
		m_DamageHitAnimType = -1;

		if (!m_SyncedHitDataArray || m_SyncedHitDataArray.Count() == 0)
		{
			return;
		}
		
		//evaluate all hit data
		SyncHitInfo data;
		SyncHitInfo greatest_hit;
		for (int i = 0; i < m_SyncedHitDataArray.Count(); i++)
		{
			data = m_SyncedHitDataArray[i];
			m_DamageHitDir = data.m_HitDir;
			
			//client-side effects
			#ifndef SERVER
			if (IsAlive() && !IsUnconscious() && data.m_HasSource && GetGame().GetMission().GetHud() && GetGame().GetPlayer() == this) //only for controlled players
			{
				m_DamageHealth = data.m_HealthDamage;
				if (m_DamageHealth > 0.0)
				{
					float rel = m_DamageHealth / (GetMaxHealth("","Health") * PlayerConstants.HEAVY_HIT_THRESHOLD);
					GetGame().GetMission().GetHud().SpawnHitDirEffect(this,m_DamageHitDir,rel);
				}
			}
			#endif
			
			if (!greatest_hit)
			{
				greatest_hit = data;
			}
			//else if (data.m_HealthDamage > greatest_hit.m_HealthDamage || (data.m_Fullbody && !greatest_hit.m_Fullbody)) //TODO - revisit cumulative calculation for animation evaluation purposes (stagger on shotgun to the chest)
			else if (data.m_Fullbody) //temporary solution; last fullbody hit is used for animation purposes, some light hit otherwise
			{
				greatest_hit = data;
			}
		}
		
		m_SyncedHitDataArray.Clear();
		m_DamageHealth = 0.0;

		//! ignore hit impacts in prone (for now)
		if (m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE || m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE || IsUnconscious())
		{
			return;
		}
		
		//general effects
		m_DamageHitFullbody = greatest_hit.m_Fullbody;
		m_DamageHitAnimType = greatest_hit.m_AnimType;
		m_DamageHitDir = greatest_hit.m_HitDir;
	}

	bool HandleDamageHit(int pCurrentCommandID)
	{
		// Update elapsed time since hit first
		if ( pCurrentCommandID == DayZPlayerConstants.COMMANDID_DAMAGE )
		{
			// Throttle heavy hit command up to a fixed rate
			if ( m_HitElapsedTime < HIT_INTERVAL_MIN )
			{
				m_HitElapsedTime += m_dT;
				ResetDamageHitState(false);
				return false;
			}
		}
		
		// If transportcache is not null, player is unconscious inside of a car and should not head into a damage command
		if (m_DamageHitAnimType != -1 && m_TransportCache == null && !CommitedSuicide())
		{
			HumanCommandVehicle vehCommand = GetCommand_Vehicle();		
			if (m_DamageHitFullbody && (!vehCommand || vehCommand.IsGettingIn() || vehCommand.IsGettingOut()))
			{
				StartCommand_Damage(m_DamageHitAnimType, m_DamageHitDir);
				ResetDamageHitState(true);
				return true;
			}
			else
			{
				AddCommandModifier_Damage(m_DamageHitAnimType, m_DamageHitDir);
				ResetDamageHitState(false);
				return false;
			}
		}
		
		return pCurrentCommandID == DayZPlayerConstants.COMMANDID_DAMAGE;
	}
	
	void ResetDamageHitState(bool resetTimer)
	{
		m_DamageHitAnimType = -1;
		m_DamageHitFullbody = false;
		
		if (resetTimer)
		{
			m_HitElapsedTime = 0;
		}
	}

	//! selects animation type and direction based on damage system data
	bool EvaluateDeathAnimation(int pDamageType, EntityAI pSource, string pAmmoType, out int pAnimType, out float pAnimHitDir)
	{
		bool doPhxImpulse = GetGame().ConfigGetInt("cfgAmmo " + pAmmoType + " doPhxImpulse") > 0;

		pAnimType = DayZPlayerConstants.DEATH_DEFAULT;		
		if (doPhxImpulse)
			pAnimType = DayZPlayerConstants.DEATH_FAST;

		//! direction
		vector targetDirection = GetDirection();
		vector toSourceDirection = (pSource.GetPosition() - GetPosition());

		targetDirection[1] = 0;
		toSourceDirection[1] = 0;

		targetDirection.Normalize();
		toSourceDirection.Normalize();

		float cosFi = vector.Dot(targetDirection, toSourceDirection);
		vector cross = targetDirection * toSourceDirection;

		pAnimHitDir = Math.Acos(cosFi) * Math.RAD2DEG;
		if (cross[1] < 0)
			pAnimHitDir = -pAnimHitDir;
		
		return true;
	}

	//! selects animation type and direction based on damage system data
	bool EvaluateDamageHitAnimation(TotalDamageResult pDamageResult, int pDamageType, EntityAI pSource, string pComponent, string pAmmoType, vector pModelPos, out int pAnimType, out float pAnimHitDir, out bool pAnimHitFullbody)
	{
		int invertHitDir = 0; //Used to flip the heavy hit animation direction
		
		pAnimType = 0;
		pAnimHitFullbody = false; // additive anm
		GetMovementState(m_MovementState);

		switch (pDamageType)
		{
			case DamageType.CLOSE_COMBAT:
				//! impact from infected attack will be light only
			 	if (pSource.IsInherited(DayZInfected))
					break;

				pAnimType = GetGame().ConfigGetInt("cfgAmmo " + pAmmoType + " hitAnimation");
				invertHitDir = GetGame().ConfigGetInt("cfgAmmo " + pAmmoType + " invertHitDir");
				if (!IsUnconscious() && pAnimType == 1 && !m_MeleeFightLogic.IsInBlock())
					pAnimHitFullbody = true;
				break;

			case DamageType.FIRE_ARM:
				int impactBehaviour = 0;

				if (!IsUnconscious() && GetHealth("", "Shock") > 25)
				{
					//! play full body when these coponents were hit
					if (pComponent == "Torso" || pComponent == "Head")
					{
						impactBehaviour = GetGame().ConfigGetInt("cfgAmmo " + pAmmoType + " impactBehaviour");
						float fireDamage = pDamageResult.GetHighestDamage("Health");
						float shockDamage = pDamageResult.GetHighestDamage("Shock");
						if ((fireDamage > 80.0 || shockDamage > 40.0) && impactBehaviour == 1)
							pAnimHitFullbody = true;
					}
				}

				break;

			case DamageType.EXPLOSION:
				break;

			case DamageType.CUSTOM:
				pAnimType = GetGame().ConfigGetInt("cfgAmmo " + pAmmoType + " hitAnimation");
				if (pAnimType == 1)
					pAnimHitFullbody = true;
				else if (pAmmoType != "HeatDamage" || IsSwimming() || IsInVehicle())
					return false; //! skip evaluation of dmg hit animation

				break;
		}

		//! direction
		vector targetDirection = GetDirection();
		vector toSourceDirection = (pSource.GetPosition() - GetPosition());

		targetDirection[1] = 0;
		toSourceDirection[1] = 0;

		targetDirection.Normalize();
		toSourceDirection.Normalize();

		float cosFi = vector.Dot(targetDirection, toSourceDirection);
		vector cross = targetDirection * toSourceDirection;

		pAnimHitDir = Math.Acos(cosFi) * Math.RAD2DEG;
		
		// We will invert direction of the hit
		if (invertHitDir > 0)
			pAnimHitDir -= 180;
		
		if (cross[1] < 0)
			pAnimHitDir = -pAnimHitDir;
		
		return true;
	}

	//! event from damage system
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		
		m_TransportHitRegistered = false;
		
		if (!IsAlive())
		{
			int animTypeDeath;
			float animHitDirDeath;
			if (EvaluateDeathAnimation(damageType, source, ammo, animTypeDeath, animHitDirDeath))
			{
				SendDeathJuncture(animTypeDeath, animHitDirDeath);
			}			
			
			if (!m_DeathSyncSent) //checked until the death is evaluated by 'OnCommandHandlerTick' higher up the road
			{
				Man killer = source.GetHierarchyRootPlayer();
				
				if (!m_KillerData) //only one player is considered killer in the event of crossfire
				{
					m_KillerData = new KillerData();
					m_KillerData.m_Killer = killer;
					m_KillerData.m_MurderWeapon = source;
				}
				
				if (killer && killer.IsPlayer())
				{
					// was player killed by headshot?
					if (dmgZone == "Brain")
					{
						m_KilledByHeadshot = true;
						if (m_KillerData.m_Killer == killer)
							m_KillerData.m_KillerHiTheBrain = true;
					}
				}
			}
		}
		else
		{
			int animType;
			float animHitDir;
			bool animHitFullbody;
			if (EvaluateDamageHitAnimation(damageResult, damageType, source, dmgZone, ammo, modelPos, animType, animHitDir, animHitFullbody))
				DayZPlayerSyncJunctures.SendDamageHitEx(this, animType, animHitDir, animHitFullbody, damageResult, damageType, source, dmgZone, ammo, modelPos);
			else
			{
				bool skipSoundRequest = false;
				if (damageType == DamageType.CUSTOM && GetCommand_Fall())
					skipSoundRequest = GetFallDamage().GetLandType() < HumanCommandFall.LANDTYPE_MEDIUM;

				if (!skipSoundRequest)
					RequestSoundEvent(EPlayerSoundEventID.TAKING_DMG_LIGHT);
			}
		}

		// interupt melee for non-blocked hit or heavy hit
		if (animHitFullbody)
		{
			HumanCommandMelee2 hcm = GetCommand_Melee2();
			if (hcm)
				hcm.Cancel();
		}
	}

	//-------------------------------------------------------------
	//!
	//! This HeadingModel
	//! 
	float 	m_fLastHeadingDiff = 0;

	//! is replayed after correction when 'NetworkRewindType.REPLAY' is used
	override bool	HeadingModel(float pDt, SDayZPlayerHeadingModel pModel)
	{
		if (!IsAlive())
		{
			return false;
		}
		
		GetMovementState(m_MovementState);
		
		//! on ladder - do nothing 
		if (m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_LADDER)
		{
			m_fLastHeadingDiff = 0;
			return false;
		}
		if (m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_VEHICLE)
		{
			HumanCommandVehicle hmv = GetCommand_Vehicle();
			if (hmv.IsGettingOut() || hmv.IsGettingIn())
			{
				return DayZPlayerImplementHeading.NoHeading(pDt, pModel, m_fLastHeadingDiff);
			}
			
			m_fLastHeadingDiff = 0;
			return false;
		}
		if (m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_UNCONSCIOUS)
		{
			HumanCommandUnconscious	hcu = GetCommand_Unconscious();
			if (!hcu.IsWakingUp())
			{
				m_fLastHeadingDiff = 0;
				//pModel.m_iCamMode = DayZPlayerConstants.CAMERAMODE_HEAD;
				return false;
			}
		}
		
		if (m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_CLIMB)
		{
			m_fLastHeadingDiff = 0;
			return false;
		}
		
#ifdef DEVELOPER
		int	actMenuValue = DiagMenu.GetEngineValue(DayZPlayerConstants.DEBUG_TURNSLIDE);
		if (actMenuValue != 0)
		{
			//! this clamps heading 
			return DayZPlayerImplementHeading.ClampHeading(pDt, pModel, m_fLastHeadingDiff);
		}
#endif
		
		if (m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
		{
			m_fLastHeadingDiff = 0;
			return false;
		}

		//! if it's standing from prone on back  
		if (m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_MOVE)
		{
			HumanCommandMove hcm = GetCommand_Move();
			if (hcm.IsStandingFromBack())
			{
				m_fLastHeadingDiff = 0;
				return false;
			}
		}
		
		HumanItemAccessor hia = GetItemAccessor();
		HumanItemBehaviorCfg hibcfg = hia.GetItemInHandsBehaviourCfg();
		if (hibcfg && hibcfg.m_StanceRotation[m_MovementState.m_iStanceIdx] == DayZPlayerConstants.ROTATION_DISABLE)
		{
			return DayZPlayerImplementHeading.NoHeading(pDt, pModel, m_fLastHeadingDiff);
		}
		if (IsUnconscious() || (GetCommand_Move() && GetCommand_Move().IsLeavingUncon()))
		{
			return DayZPlayerImplementHeading.NoHeading(pDt, pModel, m_fLastHeadingDiff);
		}

		return DayZPlayerImplementHeading.RotateOrient(pDt, pModel, m_fLastHeadingDiff);	
	}


	//-------------------------------------------------------------
	//!
	//! This Aiming Model
	//! 
	override bool	AimingModel(float pDt, SDayZPlayerAimingModel pModel)
	{
		GetMovementState(m_MovementState);
		if (m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_MELEE2)
		{
			HumanCommandMelee2 hcm = GetCommand_Melee2();
			if (hcm.IsFinisher())
			{
				if (!m_AimingFinisherStarted)
				{
					m_AimingModel.OnFinisherBegin(pModel.m_fCurrentAimY);
					m_AimingFinisherStarted = true;
				}
				m_AimingModel.ProcessStealthFilters(pDt, pModel);
			}
			
			return true;
		}

		if (m_MovementState.IsRaised())
		{
			if (!m_RaiseStarted)
			{
				m_AimingModel.OnRaiseBegin(this);
				m_RaiseStarted = true;
			}
			m_AimingModel.ProcessAimFilters(pDt, pModel, m_MovementState.m_iStanceIdx);
			
			return true;
		}

		m_RaiseStarted = false;
		m_AimingFinisherStarted = false;

		return true;
	}

	//-------------------------------------------------------------
	//!
	//! Jump / Climb
	//! 
	
	//!
	bool ProcessJumpOrClimb(float pDt, int pCurrentCommandID)
	{
		HumanInputController hic = GetInputController();
		if (!hic || !hic.IsJumpClimb())
		{
			return false;
		}
		
		m_JumpClimb.JumpOrClimb();
	
		if (!m_JumpClimb.WasSuccessful())
		{
			return false;
		}
		
		return true;
	}
	
	//!
	bool CanJump()
	{
		if (IsFBSymptomPlaying() || IsRestrained() || IsUnconscious() || IsInFBEmoteState())
			return false;
		
		if (m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE || m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
			return false;
		
		if (IsRaised() && GetInputInterface().SyncedPress("UAGetOverControllerHelper")) //no raised jump on cotroller
		{
			return false;
		}
		
		HumanItemBehaviorCfg hibcfg = GetItemAccessor().GetItemInHandsBehaviourCfg();
		if (!hibcfg.m_bJumpAllowed)
			return false;
		
		if (!DayZPlayerUtils.PlayerCanChangeStance(this, DayZPlayerConstants.STANCEIDX_ERECT) || !DayZPlayerUtils.PlayerCanChangeStance(this, DayZPlayerConstants.STANCEIDX_RAISEDERECT))
			return false;
		
		HumanCommandMove hcm = GetCommand_Move();
		if (hcm)
		{
			if (hcm.IsChangingStance())
				return false;
		}
		
		HumanCommandSwim hcs = GetCommand_Swim();
		if (hcs)
		{
			return false;
		}
		
		return true;
	}

	bool CanClimb(int climbType, SHumanCommandClimbResult climbRes)
	{
		if (IsFBSymptomPlaying() || IsRestrained() || IsUnconscious() || IsInFBEmoteState())
			return false;
		
		if (m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE || m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
			return false;
		
		if (IsRaised() && GetInputInterface().SyncedPress("UAGetOverControllerHelper")) //no raised climb on cotroller
		{
			return false;
		}
		
		HumanItemBehaviorCfg hibcfg = GetItemAccessor().GetItemInHandsBehaviourCfg();
		if (!hibcfg.m_bJumpAllowed)
			return false;
		
		if (climbRes)
		{
			EntityAI entity;
			if (Class.CastTo(entity,climbRes.m_GrabPointParent) && entity.IsHologram())
				return false;
			if (Class.CastTo(entity,climbRes.m_ClimbStandPointParent) && entity.IsHologram())
				return false;
			if (Class.CastTo(entity,climbRes.m_ClimbOverStandPointParent) && entity.IsHologram())
				return false;
		}

		return true;
	}
	

	void OnJumpStart()
	{
	}
	
	void OnJumpEnd(int pLandType = 0)
	{
	}
	
	void StopHandEvent()
	{
		GetDayZPlayerInventory().CancelHandEvent();
	}
	
	void StopWeaponEvent()
	{
		GetDayZPlayerInventory().CancelWeaponEvent();
	}
	
	void AbortWeaponEvent()
	{
		GetDayZPlayerInventory().AbortWeaponEvent();
	}

	//-------------------------------------------------------------
	//!
	//! This is main command logic
	//! 
	void HandleADS()
	{
		if (!IsAlive())
		{
			if (m_CameraIronsight || m_CameraOptics)
			{
				ExitSights();
			}
			return;
		}
		
		bool bADSToggle = false;
		bool exitSights = false;
		HumanInputController hic = GetInputController();
		HumanItemAccessor 	hia = GetItemAccessor();
		HumanCommandWeapons	hcw = GetCommandModifier_Weapons();
		PlayerBase playerPB = PlayerBase.Cast(this);
		GetMovementState(m_MovementState);
		
		if (playerPB.IsRolling())
		{
			exitSights = true;
		}
		
		if (m_ResetADS || (!hia.IsItemInHandsWeapon() && hic.WeaponADS()))
		{
			hic.ResetADS();
			m_ResetADS = false;
		}
		
		if (!m_MovementState.IsRaised() || m_LiftWeapon_player)
		{
			ResetWeaponRaiseProgress();
			exitSights = true;
		}
		else
		{
			if (m_bADS != hic.WeaponADS())
			{
				m_bADS = hic.WeaponADS();
				bADSToggle = true;
			}
			
			if (m_bADS && !m_CameraIronsight && !m_CameraOptics)
			{
				//!artificial Auto-ADS delay
				if (!IsWeaponRaiseCompleted())
				{
					bADSToggle = false;
					exitSights = true;
				}
				else
				{
					bADSToggle = true;
				}
			}
		}
	
		if (bADSToggle && !GetCommand_Melee2() && !GetThrowing().IsThrowingModeEnabled())
		{
			if (hia.IsItemInHandsWeapon() && playerPB.GetItemInHands() && playerPB.GetItemInHands().IsWeapon() && playerPB.GetWeaponManager() && !playerPB.GetWeaponManager().IsRunning())
			{
				Weapon_Base weapon = Weapon_Base.Cast(GetHumanInventory().GetEntityInHands());
				ItemOptics optic = weapon.GetAttachedOptics();
				bool switchToADS = false;
				bool canUseIronsights = weapon.CanEnterIronsights();
				bool canUseOptics = optic != NULL;
				
				if (canUseIronsights || canUseOptics)
					switchToADS = m_bADS;
				
				// go to ironsights - disable ironsights when
				//! if !raised
				//! if sprinting
				if (!m_MovementState.IsRaised() || m_MovementState.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT)
				{
					switchToADS = false;
				}
				
				// fixes camera switching during item transitions
				if (IsLiftWeapon() || IsWeaponObstructionBlockingADS() || !IsRaised() || GetDayZPlayerInventory().IsProcessing() || !IsWeaponRaiseCompleted() || IsFighting())
				{
					exitSights = true;
				}
				else if (switchToADS) // enter ironsights or optics
				{
					// filter by usability
					if (weapon.GetWasIronSight() && !canUseIronsights)
					{
						weapon.SetWasIronSight(false);
					}
					else if (!weapon.GetWasIronSight() && !canUseOptics)
					{
						weapon.SetWasIronSight(true);
					}
					
					if (weapon.GetWasIronSight())
					{
						SwitchOptics(optic,false);
						SetIronsights(true);
					}
					else if (!weapon.GetWasIronSight() || (!canUseIronsights && canUseOptics))
					{
						SetIronsights(false);
						SwitchOptics(optic,true);
					}
					else
					{
						exitSights = true;
					}
					
					if (hcw && (m_CameraOptics/* || m_CameraIronsight*/))
					{
						hcw.SetADS(true);
					}
				}
				else
				{
					exitSights = true;
				}
			}
		}
		
		// leave ironsight and/ or optics
		if (exitSights && !IsHandheldOpticsInUse() && (m_CameraIronsight || m_CameraOptics) || GetCommand_Melee2())
		{
			ExitSights();
		}
	}
	
	void HandleView()
	{
		if (!IsAlive())
		{
			if (m_CameraIronsight || m_CameraOptics)
			{
				ExitSights();
			}
			return;
		}
		
		HumanInputController hic = GetInputController();
	
		bool camChange = hic.CameraViewChanged();
		
		if (IsRaised())
		{
			if (m_IsWeapon)
			{
				camChange = false;
			}
		}
		
		//! 3rd person camera
		if (GetGame().GetWorld().Is3rdPersonDisabled())
		{
			m_Camera3rdPerson = false;
		}
		else if (camChange)
		{
			m_Camera3rdPerson = !m_Camera3rdPerson;
		}
		
		// exits optics completely, comment to return to ADS
		if ((m_LiftWeapon_player || IsWeaponObstructionBlockingADS()) && (m_CameraOptics || m_CameraIronsight))
			ExitSights();
		
		if (IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDPRONE) && (m_CameraOptics || m_CameraIronsight))
		{
			SetVerticalMinimumAimLimit(DayZPlayerCameraIronsights.CONST_UD_MIN_BACK);
			m_CameraOpticsAimOverride = true;
		}
		else if (m_CameraOpticsAimOverride)
		{
			SetVerticalMinimumAimLimit(DayZPlayerCameraIronsights.CONST_UD_MIN);
			m_CameraOpticsAimOverride = false;
		}
		
		if (hic.IsZoomToggle() && !m_MovementState.IsRaised())        
		{
			m_CameraEyeZoomLevel = ECameraZoomType.NORMAL;
			if ((IsClimbingLadder() && Math.AbsInt(m_MovementState.m_iMovement) == 2) || (IsSwimming() && m_MovementState.m_iMovement == 3))
			{
				m_CameraEyeZoomLevel = ECameraZoomType.SHALLOW;
			}
			else if (!IsClimbingLadder() && !IsSwimming() && !IsInVehicle())
			{
				float pSpeed;
				vector pLocalDirection;
				hic.GetMovement(pSpeed ,pLocalDirection);
				
				if (m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_ERECT && pSpeed == 3)
				{
					m_CameraEyeZoomLevel = ECameraZoomType.SHALLOW;
				}
			}
		}
		else
		{
			m_CameraEyeZoomLevel = ECameraZoomType.NONE;
		}
		
		if (m_MovementState.IsRaisedInProne())
		{
			float headingAngle = MiscGameplayFunctions.GetHeadingAngle(this);
			float headingAngleDiffBlock = Math.AbsFloat(m_LastHeadingAngleBlock - headingAngle);
			float headingAngleDiffBlock2 = Math.AbsFloat(m_LastHeadingAngleBlock2 - headingAngle);
			float headingAngleDiff = 0.0;
			
			if (m_LastHeadingAngle * headingAngle < 0.0)
			{
				headingAngleDiff = Math.AbsFloat(m_LastHeadingAngle + headingAngle);
			}
			else
			{
				headingAngleDiff = Math.AbsFloat(m_LastHeadingAngle - headingAngle);
			}
			
			if (headingAngleDiff > 0.2)
			{
				int time = GetGame().GetTime();
				
				float timefilterconstant = 400 - (headingAngleDiff * 100);
				
				if (headingAngleDiffBlock > 0.8 && time - m_LastBackSoundTime > timefilterconstant)
				{
					float volume = headingAngleDiff / 0.5;
					if (volume > 1)
					{
						volume = 1;
					}
					if (volume < 0.25)
					{
						volume = 0.25;
					}

					m_SoundOffset = headingAngleDiff / 2;
					if (m_SoundOffset > 0.25)
					{
						m_SoundOffset = 0.25;
					}

					string soundSetName = "Cloth_Body_longmove_TShirt_Soundset";
					
					string bodyClothName = "";
					EntityAI attachment = GetInventory().FindAttachment(InventorySlots.BODY);
					if (attachment)
					{
						bodyClothName = attachment.GetAttachmentSoundType();
					}

					if (bodyClothName != "")
					{
						string path = "CfgSoundTables CfgAttachmentSoundTables Cloth_Body_Longmove_LookupTable";
						int soundCount = GetGame().ConfigGetChildrenCount(path);

						for (int i = 0; i < soundCount; i++)
						{
							string name = "";
							GetGame().ConfigGetChildName(path, i, name);
							
							if (name == bodyClothName)
							{
								TStringArray stringArray = new TStringArray();
								GetGame().ConfigGetTextArray(path + " " + name + " soundSets", stringArray);
								soundSetName = stringArray.Get(0);

								delete stringArray;
							}
						}
					}

					SoundParams soundParams = new SoundParams(soundSetName);
					SoundObjectBuilder soundObjectBuilder = new SoundObjectBuilder(soundParams);
					
					if (soundObjectBuilder != NULL)
					{
						SoundObject soundObject = soundObjectBuilder.BuildSoundObject();
						
						if (soundObject != NULL)
						{
							soundObject.SetPosition(GetPosition());

							AbstractWave wave = GetGame().GetSoundScene().Play3D(soundObject, soundObjectBuilder);
							wave.SetStartOffset(m_SoundOffset);
							wave.SetVolumeRelative(volume);

							m_LastBackSoundTime = GetGame().GetTime();
							m_LastHeadingAngleBlock = headingAngle;
						}
					}
				}

				if (headingAngleDiffBlock2 > 1.5 && time - m_LastBackSoundTime2 > (timefilterconstant * 2))
				{
					float volume2 = headingAngleDiff * 2;
					if (volume2 > 1)
					{
						volume2 = 1;
					}

					m_SoundOffset = headingAngleDiff / 1.8;
					if (m_SoundOffset < 0.1)
					{
						m_SoundOffset = 0.1;
					}
					if (m_SoundOffset > 0.3)
					{
						m_SoundOffset = 0.3;
					}

					string soundSetName2 = "walkProne_noHS_asphalt_ext_Char_SoundSet";
					string surfaceType = GetSurfaceType();

					if (surfaceType != "")
					{
						string movementSurfaceType = "walkProne_" + surfaceType;
						
						string path2 = "CfgSoundTables CfgStepSoundTables walkProne_noHS_Char_LookupTable";
						int soundCount2 = GetGame().ConfigGetChildrenCount(path2);

						for (int i2 = 0; i2 < soundCount2; i2++)
						{
							string name2 = "";
							GetGame().ConfigGetChildName(path2, i2, name2);
							
							if (name2 == movementSurfaceType)
							{
								TStringArray stringArray2 = new TStringArray();
								GetGame().ConfigGetTextArray(path2 + " " + name2 + " soundSets", stringArray2);
								soundSetName2 = stringArray2.Get(0);

								delete stringArray2;
							}
						}
					}

					SoundParams soundParams2 = new SoundParams(soundSetName2);
					SoundObjectBuilder soundObjectBuilder2 = new SoundObjectBuilder(soundParams2);
					
					if (soundObjectBuilder2 != NULL)
					{
						SoundObject soundObject2 = soundObjectBuilder2.BuildSoundObject();
						
						if (soundObject2 != NULL)
						{
							soundObject2.SetPosition(GetPosition());

							AbstractWave wave2 = GetGame().GetSoundScene().Play3D(soundObject2, soundObjectBuilder2);
							wave2.SetStartOffset(m_SoundOffset);
							wave2.SetVolumeRelative(volume2);            

							m_LastBackSoundTime2 = GetGame().GetTime();
							m_LastHeadingAngleBlock2 = headingAngle;
						}
					}
				}

				m_LastHeadingAngle = headingAngle;
			}
		}
	}

	float m_LastHeadingAngleBlock;
	float m_LastHeadingAngleBlock2;
	float m_LastHeadingAngle;
	int m_LastBackSoundTime;
	int m_LastBackSoundTime2;
	float m_SoundOffset;


	//-------------------------------------------------------------
	//!
	//! ModOverrides
	//! 
	// these functions are for modded overide in script command mods 

	bool	ModCommandHandlerBefore(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		return false;
	}

	bool	ModCommandHandlerInside(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		return false;
	}
	
	bool	ModCommandHandlerAfter(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		return false;
	}

	//-------------------------------------------------------------
	//!
	//! This is main command logic
	//! 

	
	//! 
	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		m_dT = pDt;

		vector playerPosition = PhysicsGetPositionWS();
		
		GetDayZPlayerInventory().HandleInventory(pDt);
		GetHumanInventory().Update(pDt);
		
		if (ModCommandHandlerBefore(pDt, pCurrentCommandID, pCurrentCommandFinished))
		{
			return;
		}

		HumanInputController hic = GetInputController();
		EntityAI entityInHands = GetHumanInventory().GetEntityInHands();
		GetMovementState(m_MovementState);
		
		bool isRaisedNow = m_MovementState.IsRaised();
		bool isWeapon		= entityInHands	&& entityInHands.IsInherited(Weapon);
		
		bool updateAimingMdfr = false;
		if (isWeapon != m_IsWeapon)
		{
			m_IsWeapon = isWeapon;
			updateAimingMdfr = true;
		}

		if (isRaisedNow != m_IsRaised)
		{
			updateAimingMdfr = true;
			m_IsRaised = m_MovementState.IsRaised();
		}
		
		if (updateAimingMdfr)
		{
			if (isRaisedNow && isWeapon)
				GetUApi().ActivateModificator("aiming");
			else
				GetUApi().DeactivateModificator("aiming");
		}
		
		// handle ADS ironsights/optics transition logic
		HandleADS();
		
		// handle weapons
		if (hic)
		{
			ItemOptics opticInHands = ItemOptics.Cast(entityInHands);
			if (isWeapon && (!m_ProcessFirearmMeleeHit || !m_ContinueFirearmMelee))
			{
				m_ProcessFirearmMeleeHit = false;
				bool exitIronSights = false;
				HandleWeapons(pDt, entityInHands, hic, exitIronSights);
			}
			else if (IsHandheldOpticsInUse() && m_CameraOptics && opticInHands)
			{
				bool exitOptic = false;
				HandleOptic(opticInHands, true, hic, exitOptic);
			}
		}
		
		//! handle all Camera changes, switch + zoom
		HandleView();
		
		if (m_MovementState.m_iMovement != m_MovementState.m_LocalMovement)
		{
			OnMovementChanged();
			m_MovementState.m_LocalMovement = m_MovementState.m_iMovement;
		}
		
		//! handle death with high priority
		if (HandleDeath(pCurrentCommandID))
		{
			return;
		}
		
		//! check for water depth while getting out and start swimming if necessary
		//! this also handles the last frame of the vehicle command hence it doens't need separate check on pCurrentCommandFinished
		if ( pCurrentCommandID == DayZPlayerConstants.COMMANDID_VEHICLE )
		{
			HumanCommandVehicle cmdVehicle = GetCommand_Vehicle();
			if ( cmdVehicle && cmdVehicle.IsGettingOut() )
			{
				vector waterLevel;
				if ( m_Swimming.CheckSwimmingStart( waterLevel ) )
				{
					// not all the events may have been called as getting out could have occurred while under water
					cmdVehicle.ProcessLeaveEvents();

					StartCommand_Swim();
					return;
				}
			}
		}
		
		//! handle finished commands
		if (pCurrentCommandFinished)
		{
			if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_UNCONSCIOUS)
			{
				if ((m_LastCommandBeforeUnconscious == DayZPlayerConstants.COMMANDID_VEHICLE) && (m_TransportCache != null))
				{
					int crew_index = m_TransportCache.CrewMemberIndex(this);
					int seat = m_TransportCache.GetSeatAnimationType(crew_index);
					StartCommand_Vehicle(m_TransportCache, crew_index, seat, true);
					m_TransportCache = null;
					return;
				}
			}
			// start moving

			// start falling ? (could happen after climbing)
			if (PhysicsIsFalling(true))
			{
				StartCommand_Fall(0);
				SetFallYDiff(playerPosition[1]);
				return;
			}

			//! default behaviour after finish is to start move
			if (m_Swimming.m_bWasSwimming)
			{
				StartCommand_Swim();
				return;
			}

			StartCommand_Move();
			
			if (GetHumanInventory().GetEntityInHands())
				ForceStandUpForHeavyItems(GetHumanInventory().GetEntityInHands());
			
			return;
		}
	

		//--------------------------------------------
		//! debug test script command !PSOVIS

		if (ModCommandHandlerInside(pDt, pCurrentCommandID, pCurrentCommandFinished))
		{
			return;
		}
		

		//--------------------------------------------
		// vehicle handling
		if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_VEHICLE)
		{
			HumanCommandVehicle hcv = GetCommand_Vehicle();
			if (hcv.WasGearChange())
			{
				GearChangeActionCallback cb = GearChangeActionCallback.Cast(AddCommandModifier_Action(DayZPlayerConstants.CMD_ACTIONMOD_SHIFTGEAR, GearChangeActionCallback));
				cb.SetVehicleCommand(hcv);
			}
			
			return;
		}
		
		//! Sprint attack limiting - player has to be in full sprint for at least 0.5s
		//--------------------------------------------
		HumanCommandMove hcm = GetCommand_Move();
		if (hcm && hcm.GetCurrentMovementSpeed() > 2.99 && m_MovementState.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT)
		{
			m_SprintedTime += pDt;
			if (m_SprintedTime > m_SprintedTimePerStanceMin)
			{
				m_SprintFull = true;
				m_SprintedTimePerStanceMin = PlayerConstants.FULL_SPRINT_DELAY_DEFAULT;
			}
			else
				m_SprintFull = false;
		}
		else
		{
			m_SprintedTime = 0.0;
			m_SprintFull = false;
		}

		//--------------------------------------------
		// swimming handling
		if (m_Swimming.HandleSwimming(pCurrentCommandID, hcm, m_MovementState))
		{
			if (ProcessJumpOrClimb(pDt, pCurrentCommandID))
			{
				return;
			}
			
			m_JumpClimb.CheckAndFinishJump();
			return;			
		}
		
		//--------------------------------------------
		// ladder handling
		if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_LADDER)
		{
			OnLadder(pDt, m_MovementState);
			return;
		}

		//--------------------------------------------
		// climb handling
		if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_CLIMB)
		{
			return;
		}

		//--------------------------------------------
		// fall handling

		if (IsAlreadyInFallingCommand(pCurrentCommandID))
		{
			if (IsLanded(pCurrentCommandID))
			{
				DayZPlayerType type = GetDayZPlayerType();
				NoiseParams npar;

				FallDamageData fallDamageData = new FallDamageData();
				fallDamageData.m_Height = m_FallYDiff - playerPosition[1];

				// land
				if (fallDamageData.m_Height < 0.5)
				{
					fallDamageData.m_LandType = HumanCommandFall.LANDTYPE_NONE; 
					OnLand(pCurrentCommandID, fallDamageData);
					npar = type.GetNoiseParamsLandLight();
					AddNoise(npar, NoiseAIEvaluate.GetNoiseReduction(GetGame().GetWeather()));
				}
				else if (fallDamageData.m_Height < 3.0)
				{
					if (m_MovementState.IsInProne() || m_MovementState.IsInRaisedProne())
						fallDamageData.m_LandType = HumanCommandFall.LANDTYPE_NONE;
					else
						fallDamageData.m_LandType = HumanCommandFall.LANDTYPE_LIGHT;
					
					OnLand(pCurrentCommandID, fallDamageData);
					npar = type.GetNoiseParamsLandLight();
					AddNoise(npar, NoiseAIEvaluate.GetNoiseReduction(GetGame().GetWeather()));
				}
				else if (fallDamageData.m_Height < 5.0)
				{
					fallDamageData.m_LandType = HumanCommandFall.LANDTYPE_MEDIUM;
					OnLand(pCurrentCommandID, fallDamageData);
					npar = type.GetNoiseParamsLandHeavy();
					AddNoise(npar, NoiseAIEvaluate.GetNoiseReduction(GetGame().GetWeather()));
				}
				else
				{
					fallDamageData.m_LandType = HumanCommandFall.LANDTYPE_HEAVY;
					OnLand(pCurrentCommandID, fallDamageData);
					npar = type.GetNoiseParamsLandHeavy();
					AddNoise(npar, NoiseAIEvaluate.GetNoiseReduction(GetGame().GetWeather()));
				}
				
				if (fallDamageData.m_Height >= DayZPlayerImplementFallDamage.HEALTH_HEIGHT_LOW && GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
				{
					OnPlayerRecievedHit();
				}

				m_FallDamage.HandleFallDamage(fallDamageData);
				m_JumpClimb.CheckAndFinishJump(fallDamageData.m_LandType);
			}

			//! If we are not in fall command then something else is currently running and we can safely continue here, otherwise terminate here
			if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_FALL)
			{
				return;
			}
		}
		else if (PhysicsIsFalling(false)) 
		{
			// Not in a falling command but the controller is falling, start default fall
			StartCommand_Fall(0);
			SetFallYDiff(playerPosition[1]);
			return;
		}

		//--------------------------------------------
		// handle jumping

#ifndef NO_GUI
#ifdef DEVELOPER
		//! enable this later for everything

		if (DiagMenu.GetEngineValue(DayZPlayerConstants.DEBUG_ENABLEJUMP))
		{
			SHumanCommandClimbSettings hcls = GetDayZPlayerType().CommandClimbSettingsW();
			
			if (m_MovementState.m_iMovement != DayZPlayerConstants.MOVEMENTIDX_IDLE)
				hcls.m_fFwMaxDistance = 3;
			else
				hcls.m_fFwMaxDistance = 1.2;

			SHumanCommandClimbResult 	ret;

			//! debug draw climb heuristics
			HumanCommandClimb.DoClimbTest(this, ret, 0x3);
		}

#endif
#endif 

		// start jump 
		if (ProcessJumpOrClimb(pDt, pCurrentCommandID))
		{
			return;
		}

		HumanCommandAdditives ad = GetCommandModifier_Additives();
	
		bool force = false;
#ifndef NO_GUI
#ifdef DEVELOPER
		//! force speaking for debug
		force = DiagMenu.GetEngineValue(DayZPlayerConstants.DEBUG_ENABLETALKING);
#endif
#endif 
		float amplitude = IsPlayerSpeaking();
		
		if (amplitude > 0.1 || force)
		{	
			if (ad)
				ad.SetTalking(true);
			
			// add noises on server
			if (GetGame().IsServer())
			{
				int now = GetGame().GetTime();
				if (now >= m_NextVoNNoiseTime)
				{
					m_NextVoNNoiseTime = now + 1000;
					
					int voiceLevel = GetGame().GetVoiceLevel(this);
					DayZPlayerType pt = GetDayZPlayerType();
					
					// add noise every second while talking 
					NoiseParams vonpar;
					switch (voiceLevel)
					{
						case VoiceLevelWhisper:
							vonpar = pt.GetNoiseParamsWhisper();
						break;
						case VoiceLevelTalk:
							vonpar = pt.GetNoiseParamsTalk();
						break;
						case VoiceLevelShout:
							vonpar = pt.GetNoiseParamsShout();
						break; 
					}
					AddNoise(vonpar, NoiseAIEvaluate.GetNoiseReduction(GetGame().GetWeather()));
				}
			}
		}
		else
		{
			if (ad)
				ad.SetTalking(false);
		}
		
		//--------------------------------------------
		// anything whats handled by InputController

		//! 

		if (hic)
		{
			//! throwing handling
			HumanCommandWeapons hcw = GetCommandModifier_Weapons();
			m_Throwing.HandleThrowing(hic, hcw, entityInHands, pDt);
			
			//! melee
			if (m_MeleeFightLogic.CanFight())
			{
				if (m_MeleeFightLogic.HandleFightLogic(pCurrentCommandID, hic, entityInHands, m_MovementState, m_ContinueFirearmMelee))
				{
					m_ProcessFirearmMeleeHit = isWeapon && (hic.IsMeleeWeaponAttack() || m_ProcessFirearmMeleeHit) || m_ContinueFirearmMelee;
					return;
				}
			}
		}


		//!
		if (ModCommandHandlerAfter(pDt, pCurrentCommandID, pCurrentCommandFinished))
		{
			return;
		}
	}
	
	void HideClothing(ItemOptics optic, bool state) {}

	//-------------------------------------------------------------
	//!
	//! This is Debug functionality 
	//! 

	float 	m_TestDamageCounter 	= -1;	//! VKOSTIK - tady dat 0 misto -1
	float 	m_DebugDirectionVal 	= -180;
	bool	m_DebugTypeVal 			= false;

	int 		m_DebugWeaponChangeStage = 0;
	string	m_DebugWeaponChangeItem;
	int			m_DebugWeaponChangeShowSlot;

	//! 
	void  CommandHandlerDebug(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		if (GetPluginManager())
		{
			PluginDayzPlayerDebug playerDebug = PluginDayzPlayerDebug.Cast(GetPluginManager().GetPluginByType(PluginDayzPlayerDebug));
			if (playerDebug)
			{
				playerDebug.CommandHandler();

				//! debug handling of adding commands 
				if (m_DebugWeaponChangeStage == 0)
				{

					// debug weapon change
					int 		hideSlot;

					if (playerDebug.IsWeaponChange(m_DebugWeaponChangeItem, hideSlot, m_DebugWeaponChangeShowSlot))
					{
						//Print("Change Weapon started: " + m_DebugWeaponChangeItem + "," + hideSlot.ToString() + "," + m_DebugWeaponChangeShowSlot.ToString());

						HumanCommandWeapons	w = GetCommandModifier_Weapons();
						w.StartAction(WeaponActions.HIDE, hideSlot);

						m_DebugWeaponChangeStage = 1;
					}
				}
				else if (m_DebugWeaponChangeStage == 1)
				{
					// wait for weapon hide to meet event 
					HumanCommandWeapons	w2 = GetCommandModifier_Weapons();
					if (w2 && w2.GetRunningAction() == WeaponActions.HIDE)
					{
						if (w2.IsEvent() == WeaponEvents.CHANGE_HIDE)
						{
							//! set item in hands
							//Print("Change Weapon - hidden: ");

							//! fuj
							PlayerBase player = PlayerBase.Cast(this);
							EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();
							if (item_in_hands != NULL && player.CanDropEntity(item_in_hands) && GetGame().GetPlayer().GetHumanInventory().CanRemoveEntityInHands())
							{
								player.PredictiveDropEntity(item_in_hands);
							}

							if (m_DebugWeaponChangeItem != "")
							{
								InventoryLocation dst = new InventoryLocation;
								dst.SetHands(this, NULL);
								EntityAI item = SpawnEntity(m_DebugWeaponChangeItem, dst,ECE_IN_INVENTORY,RF_DEFAULT);
							}

							//! start show
							w2.StartAction(WeaponActions.SHOW, 3);

							m_DebugWeaponChangeStage = 2;
						}
					}
				}
				else if (m_DebugWeaponChangeStage == 2)
				{
					HumanCommandWeapons	w3 = GetCommandModifier_Weapons();
					if (w3 && w3.IsActionFinished())
					{
						m_DebugWeaponChangeStage = 0;
					}
				}
			}
		}
	

		//! damage test
		if (m_TestDamageCounter >= 0)
		{
			if (m_TestDamageCounter > 3)
			{
				m_DebugDirectionVal = m_DebugDirectionVal + 90;
				m_TestDamageCounter = 0;

				if (m_DebugDirectionVal > 90)
				{
					m_DebugDirectionVal = -180;
					m_DebugTypeVal		= !m_DebugTypeVal;
				}

				if (m_DebugTypeVal)
				{
					AddCommandModifier_Damage(0, m_DebugDirectionVal); // starts light
				}
				else
				{
					StartCommand_Damage(0, m_DebugDirectionVal); // starts heavy (full body)
				}
				// 0.. types
				// from: -180 back, -90 left, 0 front, 90 right, 180 back 
				//AddCommandModifier_Damage(0, directionVal); // starts light
				//StartCommand_Damage(0, 0); // starts heavy (full body)
			}

			m_TestDamageCounter += pDt;
		}


#ifndef NO_GUI
#ifdef DEVELOPER

		// injury / exhaustion
		{
			HumanCommandAdditives ad = GetCommandModifier_Additives();

			//! diag menu - injured
			float v = DiagMenu.GetEngineRangeValue(DayZPlayerConstants.DEBUG_SHOWINJURY);
			if (v > 0)
			{
				ad.SetInjured(v, true);
			}

			//! diag menu - injured
			v = DiagMenu.GetEngineRangeValue(DayZPlayerConstants.DEBUG_SHOWEXHAUSTION);
			if (v > 0)
			{
				ad.SetExhaustion(v, true);
			}
		}
#endif
#endif 

	}



	//-------------------------------------------------------------
	//!
	//! This is main camera selection logic
	//! 
	override int 	CameraHandler(int pCameraMode)
	{
		//! ironsights
		if (!m_LiftWeapon_player && !IsWeaponObstructionBlockingADS())
		{
			ItemOptics optics = null;
			EntityAI entityInHands = GetHumanInventory().GetEntityInHands();
			Weapon weapon = Weapon.Cast(entityInHands);
			if (weapon)
				optics = weapon.GetAttachedOptics();
			else if (entityInHands)
				Class.CastTo(optics,entityInHands);
			
			if (m_CameraOptics && optics)
			{
				return DayZPlayerCameras.DAYZCAMERA_OPTICS;
			}
			else if (m_CameraIronsight && weapon)
			{
				return DayZPlayerCameras.DAYZCAMERA_IRONSIGHTS;
			}
		}	
		
		// If the logic above doesn't reach optics nor ironsights camera,
		// yet weapon is still raised and ads is wanted, enforce 1pv camera
		// as this will reduce erratic behaviour of camera switching in 3pv
		HumanInputController hic = GetInputController();
		if (m_Camera3rdPerson && m_IsWeapon && m_IsRaised && hic && hic.WeaponADS())
		{
			return DayZPlayerCameras.DAYZCAMERA_1ST;
		}
		
		//uncon
		if ( GetCurrentCommandID() == DayZPlayerConstants.COMMANDID_UNCONSCIOUS )
		{
			return DayZPlayerCameras.DAYZCAMERA_1ST_UNCONSCIOUS;
		}
		
		HumanCommandVehicle vehicleCommand = GetCommand_Vehicle();
		
		if (!m_Camera3rdPerson)
		{
			if (vehicleCommand)
			{
				return DayZPlayerCameras.DAYZCAMERA_1ST_VEHICLE;
			}
			
			return DayZPlayerCameras.DAYZCAMERA_1ST;
		}
		else
		{
			if (vehicleCommand)
			{
				Transport transport = vehicleCommand.GetTransport();
				if (transport && GetParent())
				{
					return transport.Get3rdPersonCameraType();
				}
			}
			
			//! get movement state 
			GetMovementState(m_MovementState);
			
			//! VKOSTIK: v ostatnich commandech mimo COMMANDID_MOVE je 
			//! m_MovementState.m_iStanceIdx - DayZPlayerConstants.STANCEIDX_ERECT
			//! m_MovementState.m_iMovement = 0 (idle)

			//! COMMANDID_ACTION returns stance right - ERECT/CROUCH

			//! melee camera - not melee - stays in stance camera

			if (m_MovementState.m_CommandTypeId == DayZPlayerConstants.COMMANDID_CLIMB)
			{
				return DayZPlayerCameras.DAYZCAMERA_3RD_CLIMB;
			}

			if (m_JumpClimb.m_bIsJumpInProgress)
			{
				return DayZPlayerCameras.DAYZCAMERA_3RD_JUMP;
			}
			
			int targetStanceIndex = m_MovementState.m_iStanceIdx;
			
			//! special handling to allow the camera to fully blend before transitioning to the raised one
			HumanCommandMove commandMove = GetCommand_Move();
			if (commandMove && commandMove.IsChangingStance() && m_MovementState.IsRaised())
			{
				targetStanceIndex = m_MovementState.m_iStanceIdx - DayZPlayerConstants.STANCEIDX_RAISED;
			}
			
			//		
			// normal movement cameras			
			if (targetStanceIndex == DayZPlayerConstants.STANCEIDX_CROUCH)
			{
				return DayZPlayerCameras.DAYZCAMERA_3RD_CRO;
			}
			else if (targetStanceIndex == DayZPlayerConstants.STANCEIDX_RAISEDCROUCH)
			{
				return DayZPlayerCameras.DAYZCAMERA_3RD_CRO_RAISED;
			}
			else if (targetStanceIndex == DayZPlayerConstants.STANCEIDX_PRONE)
			{
				return DayZPlayerCameras.DAYZCAMERA_3RD_PRO;
			}
			else if (targetStanceIndex == DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
			{
				return DayZPlayerCameras.DAYZCAMERA_3RD_PRO_RAISED;
			}

			if (targetStanceIndex == DayZPlayerConstants.STANCEIDX_RAISEDERECT)
			{
				if (m_IsWeapon) 
				{
					return DayZPlayerCameras.DAYZCAMERA_3RD_ERC_RAISED;
				}
				else
				{
					return DayZPlayerCameras.DAYZCAMERA_3RD_ERC_RAISED_MELEE;
				}
			}
			else if (targetStanceIndex == DayZPlayerConstants.MOVEMENTIDX_SPRINT)
			{
				return DayZPlayerCameras.DAYZCAMERA_3RD_ERC_SPR;
			}

			// DayZPlayerConstants.STANCEIDX_ERECT
			return DayZPlayerCameras.DAYZCAMERA_3RD_ERC;
		}
		
		return DayZPlayerCameras.DAYZCAMERA_1ST;
	}
	
	//! Event, called when HumanMovementState.m_iMovement changes...walk, run, sprint, etc..
	void OnMovementChanged()//(int state_old, int state_new)
	{
		switch (m_MovementState.m_LocalMovement)
		{
			case DayZPlayerConstants.MOVEMENTIDX_CROUCH_RUN:
			{
				//OnCrouchRunEnd();
				break;
			}
			case DayZPlayerConstants.MOVEMENTIDX_SPRINT:
			{
				OnSprintEnd();
				break;
			}
			case DayZPlayerConstants.MOVEMENTIDX_RUN:
			{
				//OnRunEnd();
				break;
			}
			case DayZPlayerConstants.MOVEMENTIDX_WALK:
			{
				//OnWalkEnd();
				break;
			}
			case DayZPlayerConstants.MOVEMENTIDX_IDLE:
			{
				//OnIdleEnd();
				break;
			}
			case DayZPlayerConstants.MOVEMENTIDX_SLIDE:
			{
				//OnSlideEnd();
				break;
			}
		}
		
		switch (m_MovementState.m_iMovement)
		{
			case DayZPlayerConstants.MOVEMENTIDX_CROUCH_RUN:
			{
				//OnCrouchRunStart();
				break;
			}
			case DayZPlayerConstants.MOVEMENTIDX_SPRINT:
			{
				OnSprintStart();
				break;
			}
			case DayZPlayerConstants.MOVEMENTIDX_RUN:
			{
				//OnRunStart();
				break;
			}
			case DayZPlayerConstants.MOVEMENTIDX_WALK:
			{
				//OnWalkStart();
				break;
			}
			case DayZPlayerConstants.MOVEMENTIDX_IDLE:
			{
				//OnIdleStart();
				break;
			}
			case DayZPlayerConstants.MOVEMENTIDX_SLIDE:
			{
				//OnSlideStart();
				break;
			}
		}
	}
	
	void OnSprintStart()
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			GetUApi().GetInputByID(UALeanLeft).Lock();
			GetUApi().GetInputByID(UALeanRight).Lock();
		}
	}
	
	void OnSprintEnd()
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			#ifdef PLATFORM_CONSOLE
			if (GetUApi().GetInputByID(UAVoiceModifierHelper).LocalValue() == 0)
			#endif
			{
				GetUApi().GetInputByID(UALeanLeft).Unlock();
				GetUApi().GetInputByID(UALeanRight).Unlock();
			}
		}
	}
	//-------------------------------------------------------------
	//!
	//! Input userdata
	//! 

	void OnInputUserDataReceived(ParamsReadContext ctx)
	{
		int userDataTypeParam = 0;
		if (!ctx.Read(userDataTypeParam))
		{
			Error("DayZPlayerImplement: cannot read input type");
			return;
		}

		OnInputUserDataProcess(userDataTypeParam, ctx);
	}

	bool OnInputUserDataProcess(int userDataType, ParamsReadContext ctx)
	{
		m_MeleeCombat.SetTargetObject(null);
		m_MeleeCombat.SetHitPos(vector.Zero);
		m_MeleeCombat.SetHitZoneIdx(-1);
		m_MeleeCombat.SetFinisherType(-1);

		if (userDataType == INPUT_UDT_MELEE_TARGET)
		{
			EntityAI target = null;
			if (ctx.Read(target))
			{
				m_MeleeCombat.SetTargetObject(target);
			}

			vector hitPos = vector.Zero;
			if (ctx.Read(hitPos))
			{
				m_MeleeCombat.SetHitPos(hitPos);
			}

			int hitZoneIdx = -1;
			if (ctx.Read(hitZoneIdx))
			{
				m_MeleeCombat.SetHitZoneIdx(hitZoneIdx);
			}

			int finisherType = -1;
			if (ctx.Read(finisherType))
			{
				m_MeleeCombat.SetFinisherType(finisherType);
			}
			
			return true;
		}

		return false;
	}

	//-------------------------------------------------------------
	//!
	//! SyncJunctures
	//! 
	void OnSyncJuncture(int pJunctureID, ParamsReadContext pCtx)
	{
		switch (pJunctureID)
		{
			case DayZPlayerSyncJunctures.SJ_DEATH:
				if (m_DeathAnimType == -2)
					DayZPlayerSyncJunctures.ReadDeathParams(pCtx, m_DeathAnimType, m_DeathHitDir);
				break;
				
			case DayZPlayerSyncJunctures.SJ_DAMAGE_HIT:
				SyncHitInfo data = new SyncHitInfo;
				if (DayZPlayerSyncJunctures.ReadDamageHitParamsEx(pCtx, data) && data != null)
				{
					m_SyncedHitDataArray.Insert(data);
				}
				break;
		}
	}

	AnimBootsType GetBootsType()
	{
		EntityAI boots = GetInventory().FindAttachment(InventorySlots.FEET);
		if (boots)
		{
			switch (boots.GetAttachmentSoundType())
			{
				case "Sneakers":
					return AnimBootsType.Sneakers;
				case "Boots":
					return AnimBootsType.Boots;
			}
		}

		return AnimBootsType.None;
	}
	
	//Get top body clothes
	AnimUpperBodyType GetBodyAttachmentType()
	{
		EntityAI attachment = GetInventory().FindAttachment(InventorySlots.BODY);
		if (attachment)
		{
			switch (attachment.GetAttachmentSoundType())
			{
				case "NylonJacket":
					return AnimUpperBodyType.NylonJacket;
				case "TShirt":
					return AnimUpperBodyType.TShirt;
				case "WoolShirt":
					return AnimUpperBodyType.WoolShirt;
				case "HeavyJacket":
					return AnimUpperBodyType.HeavyJacket; 
				case "LeatherJacket":
					return AnimUpperBodyType.LeatherJacket;
				case "Coat":
					return AnimUpperBodyType.Coat;
				case "ChemlonDress":
					return AnimUpperBodyType.ChemlonDress;
				case "Ghillie":
					return AnimUpperBodyType.Ghillie;
				case "Chainmail":
					return AnimUpperBodyType.Chainmail;
			}
		}
		
		return AnimUpperBodyType.None;
	}
	
	AnimBackType GetBackAttachmentType()
	{
		EntityAI back = GetInventory().FindAttachment(InventorySlots.BACK);
		if (back)
		{
			switch (back.GetAttachmentSoundType())
			{
				case "Small":
					return AnimBackType.Small;
				case "Military":
					return AnimBackType.Military;
				case "Outdoor":
					return AnimBackType.Outdoor;
				case "Ghillie":
					return AnimBackType.Ghillie; 
			}
		}
		
		return AnimBackType.None;
	}
	
	//Get weapon on back
	AnimRangedWeaponType GetShoulderAttachmentType()
	{
		EntityAI shoulderAttachment = GetInventory().FindAttachment(InventorySlots.SHOULDER);
		EntityAI meleeAttachment = GetInventory().FindAttachment(InventorySlots.MELEE);
		
		AnimRangedWeaponType shoulderAttType = AnimRangedWeaponType.None;
		AnimRangedWeaponType meleeAttType = AnimRangedWeaponType.None;
		
		if (shoulderAttachment)
		{
			switch (shoulderAttachment.GetAttachmentSoundType())
			{
				case "Shotgun":
				{
					shoulderAttType = AnimRangedWeaponType.Shotgun;
					break;
				}	
				case "Rifle":
				{
					shoulderAttType = AnimRangedWeaponType.Rifle;
					break;
				}
			}
		}
		if (meleeAttachment)
		{
			switch (meleeAttachment.GetAttachmentSoundType())
			{
				case "Shotgun":
				{
					meleeAttType = AnimRangedWeaponType.Shotgun;
					break;
				}
				case "Rifle":
				{
					meleeAttType = AnimRangedWeaponType.Rifle;
					break;
				}
			}
		}
		
		if (shoulderAttType == AnimRangedWeaponType.Shotgun || meleeAttType == AnimRangedWeaponType.Shotgun)
			return AnimRangedWeaponType.Shotgun;
		
		if (shoulderAttType == AnimRangedWeaponType.Rifle || meleeAttType == AnimRangedWeaponType.Rifle)
			return AnimRangedWeaponType.Rifle;

		return AnimRangedWeaponType.None;
	}
	
	string GetSurfaceType(SurfaceAnimationBone limbType)
	{
		string surfaceType;
		int liquidType;

		g_Game.SurfaceUnderObjectByBoneCorrectedLiquid(this, limbType, surfaceType, liquidType);
		
		return surfaceType;
	}

	//-------------------------------------------------------------
	//!
	//! Sounds
	//! 

	AbstractWave PlaySound(SoundObject so, SoundObjectBuilder sob)
	{
		if (so == null)
			return null;

		so.SetPosition(GetPosition());
		AbstractWave wave = GetGame().GetSoundScene().Play3D(so, sob);
		return wave;
	}

	void AddNoise(NoiseParams noisePar, float noiseMultiplier = 1.0)
	{
		if (noisePar != null)
			GetGame().GetNoiseSystem().AddNoise(this, noisePar, noiseMultiplier);
	}

	//-------------------------------------------------------------
	//!
	//! Event Processing
	//! 

	void OnStepEvent(string pEventType, string pUserString, int pUserInt)
	{
		vector orientation;
		DayZPlayerType type = GetDayZPlayerType();
		HumanMovementState	state = new HumanMovementState();
		GetMovementState(state);
		
		#ifndef SERVER
		DayZPlayerTypeStepSoundLookupTable table = type.GetStepSoundLookupTable();
		SoundObjectBuilder soundBuilder = NULL;
		if (pUserInt < 100)
		{
			string surface;
			if (pUserInt % 2 == 1)
			{
				surface = GetSurfaceType(SurfaceAnimationBone.LeftBackLimb);
				if (surface.Length() == 0)//if no surface found, try other leg
					surface = GetSurfaceType(SurfaceAnimationBone.RightBackLimb);
			}
			else
			{
				surface = GetSurfaceType(SurfaceAnimationBone.RightBackLimb);
				if (surface.Length() == 0)//if no surface found, try other leg
					surface = GetSurfaceType(SurfaceAnimationBone.LeftBackLimb);
			}
			
			if (surface.Length() != 0)
				m_LastSurfaceUnderHash = surface.Hash();
			
			if (state.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT)
			{
				orientation = GetOrientation() * (-1);
				int stepParticleID = Surface.GetStepsParticleID(surface);
			
				if (stepParticleID > 0)
				{
					ParticleManager.GetInstance().PlayInWorldEx(stepParticleID, null, GetPosition(), orientation);
				}
			}
			
			soundBuilder = table.GetSoundBuilder(pUserInt, state.m_iMovement, m_LastSurfaceUnderHash, GetBootsType());
		}
		else
		{
			string surface2 = GetSurfaceType();
			if (surface2.Length() != 0)
				m_LastSurfaceUnderHash = surface2.Hash();
			
			soundBuilder = table.GetSoundBuilder(pUserInt, state.m_iMovement, m_LastSurfaceUnderHash, GetBootsType());
		}

		if (soundBuilder != NULL && GetGame().GetPlayer())
		{
			orientation = Vector(0, 0, 0);
			vector edgeLength = Vector(1.5, 3.0, 1.5);
			array<Object> excludedObjects = new array<Object>;
			excludedObjects.Insert(this);
			array<Object> collidedObjects = new array<Object>;
			
			if (GetGame().IsBoxCollidingGeometry(GetPosition(), orientation, edgeLength, ObjIntersectView, ObjIntersectNone, excludedObjects, collidedObjects))
			{
				for (int i = 0; i < collidedObjects.Count(); ++i)
				{
					WoodBase colObject = WoodBase.Cast(collidedObjects.Get(i));
					if (colObject && colObject.HasPlayerCollisionParticle() && state.m_iMovement > DayZPlayerConstants.MOVEMENTIDX_WALK)
						MiscEffects.PlayVegetationCollideParticles(colObject, this);
					
					if (colObject && colObject.HasPlayerCollisionSound())
					{
						for (int j = 0; j < type.GetVegetationSounds().Count(); ++j)
						{
							VegetationSound vegetationSound = type.GetVegetationSounds().Get(j);
							
							if (vegetationSound.GetAnimEventIds().Find(pUserInt) >= 0)
							{
								SoundObjectBuilder vegSoundObjectBuilder = vegetationSound.GetSoundObjectBuilder();
								SoundObject vegSoundObject = vegetationSound.GetSoundObjectBuilder().BuildSoundObject();
								
								if (vegSoundObject != NULL)
								{
									AttenuateSoundIfNecessary(vegSoundObject);
									PlaySound(vegSoundObject, vegSoundObjectBuilder);
								}
								
								break;
							}
						}
						
						break;
					}
				}
			}
			
			SoundObject soundObject = soundBuilder.BuildSoundObject();
			if (soundObject != NULL)
			{
				AttenuateSoundIfNecessary(soundObject);
				PlaySound(soundObject, soundBuilder);
			}
		}
		#endif
		
		if (GetGame().IsServer())
		{
			m_StepCounter++;//move outside of server condition if needed on client as well

			float noiseMultiplier = 0;
			
			NoiseParams noiseParams;
			if (state.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_ERECT || state.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDERECT)
				noiseParams = type.GetNoiseParamsStand();
			else if (state.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_CROUCH || state.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDCROUCH)
				noiseParams = type.GetNoiseParamsCrouch();
			else if (state.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE || state.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
				noiseParams = type.GetNoiseParamsProne();
			else
			{
				Debug.Log(string.Format("Wrong stance, id: %1 using backup with stand stance (id: 0)", state.m_iStanceIdx));
				noiseParams = type.GetNoiseParamsStand();
			}
			
			noiseMultiplier = NoiseAIEvaluate.GetNoiseMultiplier(this) * NoiseAIEvaluate.GetNoiseReduction(GetGame().GetWeather());
									
			AddNoise(noiseParams, noiseMultiplier);
			
			if ((m_StepCounter % PlayerConstants.CHECK_EVERY_N_STEP) == 0)
			{
				ProcessFeetDamageServer(pUserInt);
			}
		}
	}

	void OnSoundEvent(string pEventType, string pUserString, int pUserInt)
	{
		int eventReplaceID;
		
		if (pEventType == "Sound")
		{
			if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT || GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_REMOTE)
			{
				eventReplaceID = m_ReplaceSoundEventHandler.GetSoundEventID(pUserInt, ESoundEventType.SOUND_COMMON);
				if (eventReplaceID > 0)
				{
					PlaySoundEventType(ESoundEventType.SOUND_COMMON, eventReplaceID);
					return;
				}
			}
			
			ProcessSoundEvent(pEventType, pUserString, pUserInt);
		}
		else if (pEventType == "SoundWeapon")
		{
			if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT || GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_REMOTE)
			{
				eventReplaceID = m_ReplaceSoundEventHandler.GetSoundEventID(pUserInt, ESoundEventType.SOUND_WEAPON);
				if (eventReplaceID > 0)
				{
					PlaySoundEventType(ESoundEventType.SOUND_WEAPON, eventReplaceID);
					return;
				}
			}
			
			ProcessWeaponEvent(pEventType, pUserString, pUserInt);
		}
		else if (pEventType == "SoundAttachment")
		{
			ProcessAttachmentEvent(pEventType, pUserString, pUserInt);
		}
		else if (pEventType == "SoundVoice")
		{
			if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT || GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_REMOTE)
			{
				int event_id = m_PlayerSoundEventHandler.ConvertAnimIDtoEventID(pUserInt);
				if (event_id > 0)
				{
					PlaySoundEvent(event_id);
					return;
				}
			}
			ProcessVoiceEvent(pEventType, pUserString, pUserInt);
		}
		else
		{
			Debug.Log("OnSoundEvent: Unknown sound event \"" + pEventType + "\"");
		}
	}

	void OnParticleEvent(string pEventType, string pUserString, int pUserInt)
	{
		
	}

	
	void ProcessWeaponEvent(string pEventType, string pUserString, int pUserInt)
	{
		DayZPlayerType type = GetDayZPlayerType();
		AnimSoundEvent soundEvent = null;

		float quantity = 0;
		
		EntityAI entityInHands = GetHumanInventory().GetEntityInHands();
		if (entityInHands && entityInHands.IsInherited(ItemBase))
		{
			ItemBase item;
			Class.CastTo(item, entityInHands);
			if (item.HasQuantity())
				quantity = (float)item.GetQuantity() / (item.GetQuantityMax() - item.GetQuantityMin());
			InventoryItemType invItemType = item.GetInventoryItemType();
			soundEvent = invItemType.GetSoundEvent(pUserInt);
		}

		if (soundEvent == null)
		{
			quantity = 0;
			soundEvent = type.GetSoundWeaponEvent(pUserInt);
		}

		if (soundEvent != null)
		{
			if (!GetGame().IsDedicatedServer())
			{
				SoundObjectBuilder builder = soundEvent.GetSoundBuilderEx(m_ActionSoundCategoryHash);
				if (builder)
				{
					builder.AddVariable("quantity", quantity);
					builder.AddVariable("interior", IsSoundInsideBuilding());
					
					SoundObject soundObject = builder.BuildSoundObject();
					if (soundObject != NULL)
					{
						AttenuateSoundIfNecessary(soundObject);
						AbstractWave sound = PlaySound(soundObject, builder);
						
						if (pUserString == "StopOnAnimEnd")
							m_PerformedActionSounds.Insert(sound);
					}
				}
				else
				{
					//TODO:Userful error message
				}
			}
			
			if (GetGame().IsServer())
			{
				AddNoise(soundEvent.m_NoiseParams, NoiseAIEvaluate.GetNoiseReduction(GetGame().GetWeather()));
			}
		}
	}

	void ProcessAttachmentEvent(string pEventType, string pUserString, int pUserInt)
	{
		DayZPlayerType type = GetDayZPlayerType();
		DayZPlayerTypeAttachmentSoundLookupTable table = type.GetAttachmentSoundLookupTable();
			
		array<string> attachments = new array<string>();
		pUserString.Split(",", attachments);
		for (int i = 0; i < attachments.Count(); i++)
		{
			int attachmentHash = -1;
			if (attachments[i] == "shoulder")
				attachmentHash = GetShoulderAttachmentType();
			else if (attachments[i] == "body")
				attachmentHash = GetBodyAttachmentType();
			else if (attachments[i] == "back")
				attachmentHash = GetBackAttachmentType();
			
			SoundObjectBuilder soundBuilder = table.GetSoundBuilder(pUserInt, attachments[i], attachmentHash);
			
			if (soundBuilder != NULL)
			{
				SoundObject soundObject = soundBuilder.BuildSoundObject();
				if (soundObject != NULL)
				{
					AttenuateSoundIfNecessary(soundObject);
					PlaySound(soundObject, soundBuilder);
				}
			}
		}
	}

	void SetVariablesLadderSoundObjectBuilder(SoundObjectBuilder soundObjectBuilder)
	{
		if (m_ClimbingLadderType == "wood")
		{
			soundObjectBuilder.AddVariable("laddertype", 1);
		}
		else
		{
			soundObjectBuilder.AddVariable("laddertype", 0);
		}
		
		AnimBootsType pBoots = GetBootsType();
		
		if (pBoots == AnimBootsType.None)
		{
			soundObjectBuilder.AddVariable("bare", 1);
			soundObjectBuilder.AddVariable("sneakers", 0);
			soundObjectBuilder.AddVariable("boots", 0);
		}
		else if (pBoots == AnimBootsType.Sneakers)
		{
			soundObjectBuilder.AddVariable("bare", 0);
			soundObjectBuilder.AddVariable("sneakers", 1);
			soundObjectBuilder.AddVariable("boots", 0);
		}
		else if (pBoots == AnimBootsType.Boots)
		{
			soundObjectBuilder.AddVariable("bare", 0);
			soundObjectBuilder.AddVariable("sneakers", 0);
			soundObjectBuilder.AddVariable("boots", 1);
		}
	}

	void ProcessSoundEvent(string pEventType, string pUserString, int pUserInt)
	{
		DayZPlayerType type = GetDayZPlayerType();
		DayZPlayerTypeAnimTable table = type.GetSoundTable();
		AnimSoundEvent soundEvent;
		if (table)
		{
			soundEvent = table.GetSoundEvent(pUserInt);
		}
		
		if (soundEvent != NULL)
		{
			if (!GetGame().IsDedicatedServer())
			{
				SoundObjectBuilder objectBuilder = soundEvent.GetSoundBuilder();
				if (GetCommand_Ladder())
				{
					SetVariablesLadderSoundObjectBuilder(objectBuilder);
				}
				objectBuilder.AddEnvSoundVariables(GetPosition());

				SoundObject soundObject = objectBuilder.BuildSoundObject();
				if (soundObject != NULL)
				{
					AttenuateSoundIfNecessary(soundObject);
					AbstractWave sound = PlaySound(soundObject, objectBuilder);
					
					if (pUserString == "StopOnAnimEnd")
						m_PerformedActionSounds.Insert(sound);
				}
			}
			
			if (GetGame().IsServer())
			{
				if (soundEvent.m_NoiseParams != NULL)
					GetGame().GetNoiseSystem().AddNoise(this, soundEvent.m_NoiseParams, NoiseAIEvaluate.GetNoiseReduction(GetGame().GetWeather()));
			}
		}
	}
	
	

	AbstractWave ProcessVoiceEvent(string pEventType, string pUserString, int pUserInt)
	{
		#ifdef SERVER//none of this code needs to run on server, ifdef put here rather than before the method call to maintain mod compatibility
		return null;
		#endif
		DayZPlayerType type = GetDayZPlayerType();
		DayZPlayerTypeVoiceSoundLookupTable table = type.GetVoiceSoundLookupTable();
		if (!table)
			return null;
		AbstractWave wave;
		EntityAI mask = GetInventory().FindAttachment(InventorySlots.MASK);
		EntityAI head_gear = GetInventory().FindAttachment(InventorySlots.HEADGEAR);
		
		string category;
		if (mask || head_gear)
		{
			string category_mask;
			string category_headgear;
			
			int priority_mask;
			int priority_headgear;
			
			if (mask)
			{
				category_mask = mask.ConfigGetString("soundVoiceType");
				priority_mask = mask.ConfigGetInt("soundVoicePriority");
			}
			if (head_gear)
			{
				category_headgear = head_gear.ConfigGetString("soundVoiceType");
				priority_headgear = head_gear.ConfigGetInt("soundVoicePriority");
			}
			
			if (priority_headgear >= priority_mask && category_headgear != "")
			{
				category = category_headgear;
			}
			else
			{
				category = category_mask;
			}
		}

		if (category == "")
		{
			category = "none";
		}
		
		SoundObjectBuilder soundBuilder = table.GetSoundBuilder(pUserInt, category.Hash());
		
		if (soundBuilder)
		{
			PlayerBase player = PlayerBase.Cast(this);
			//this code block looks super shady, but it's the only way the sounds play correctly, god knows what's going on on c++ side
			int maleVoiceType = 0;
			int femaleVoiceType = 0;
			if (player.IsMale())
			{
				maleVoiceType = player.GetVoiceType();
			}
			else
			{
				femaleVoiceType = player.GetVoiceType();
			}

			soundBuilder.AddVariable("male", maleVoiceType);
			soundBuilder.AddVariable("female", femaleVoiceType);
			
			// end of weirdness
			SoundObject soundObject = soundBuilder.BuildSoundObject();
			if (soundObject != NULL)
			{
				AttenuateSoundIfNecessary(soundObject);
				wave = PlaySound(soundObject, soundBuilder);
			}

		}
		
		return wave;
	}
	
	void ProcessFeetDamageServer(int pUserInt);
	

	//-------------------------------------------------------------
	//!
	//! anti-cheat condition
	//! 

	bool CheckForTakeItem(EntityAI item)
	{
		bool restrained = IsRestrained(); // IsRestrained()
		bool unconscious = IsUnconscious(); // IsRestrained()

		ItemBase ib = ItemBase.Cast(item);
		if (ib && (PlayerBase.DEBUG_INVENTORY_ACCESS || !IsAlive() || restrained || unconscious || ib.CanBeMovedOverride()))
			return true;
		return false;
	}
	
	bool CheckForDropItem(EntityAI item)
	{
		/*
		bool restrained = IsRestrained(); // IsRestrained()
		bool unconscious = IsUnconscious(); // IsRestrained()
		
		ItemBase ib = ItemBase.Cast(item);
		if (!IsAlive() || restrained || unconscious || ib.CanBeMovedOverride())
			return true;
		return false;
		*/
		return CheckForTakeItem(item); //the two functions are currently identical
	}
	
	//-------------------------------------------------------------
	//!
	//! Player respawn
	//! 
	bool CheckForRespawn(EntityAI item)
	{
		PlayerBase player = PlayerBase.Cast(this);
		if (!player)
			return true;
		
		if (!IsAlive() || player.IsUnconscious())
			return true;
		return false;
	}

#ifdef DEVELOPER

	// -------------------------------------------------------------------------
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type == ERPCs.RPC_DAYZPLAYER_DEBUGSERVERWALK)
		{
			Print("ERPCs.RPC_DAYZPLAYER_DEBUGSERVERWALK");

			Param1<bool> rp = new Param1<bool>(false);
			ctx.Read(rp);

			GetInputController().OverrideMovementSpeed(rp.param1, 1);
			GetInputController().OverrideAimChangeX(rp.param1, 0.01);

		}
	}

#endif

	bool IsLanded(int pCurrentCommandID)
	{
		if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_FALL)
		{
			HumanCommandFall fall = GetCommand_Fall();

			if (fall)
			{
				return fall.PhysicsLanded();
			}
		}
		
		return false;
	}

	bool OnLand(int pCurrentCommandID, FallDamageData fallDamageData)
	{
		if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_FALL)
		{
			HumanCommandFall fall = GetCommand_Fall();

			if (fall)
			{
				fall.Land(fallDamageData.m_LandType);
				return true;
			}
		}

		return false;
	}

	bool IsAlreadyInFallingCommand(int pCurrentCommandID)
	{
		return pCurrentCommandID == DayZPlayerConstants.COMMANDID_FALL;
	}
	
	//! movement sliding override, originally for FB gestures
	void OverrideSlidePoseAngle(float value)
	{
		if (!m_MoveSettings)
			m_MoveSettings = GetDayZPlayerType().CommandMoveSettingsW();
		if (m_MoveSettings.m_fSlidingPoseAngle != value)
		{
			m_MoveSettings.m_fSlidingPoseAngle = value;
			StartCommand_Move(); //nescessary, re-initializes with adjusted values
		}
	}
	
	float GetSlidePoseAngle()
	{
		return GetDayZPlayerType().CommandMoveSettingsW().m_fSlidingPoseAngle;
	}
	
	void CheckAnimationOverrides()
	{
	}
	
	bool IsPlayerSelected()
	{
		return m_PlayerSelected;
	}
	
	bool IsRestrained();
	
	//! Checks if fullbody animation or specific locked state is active in emote manager
	bool IsInFBEmoteState();
	
	void SetSuicide(bool state)
	{
		m_Suicide = state;
	}
	
	bool CommitedSuicide()
	{
		return m_Suicide;
	}
	
	//!called every command handler tick when player is on ladder
	void OnLadder(float delta_time, HumanMovementState pState);
	
	//-------------------------------------------------------------
	//!
	//! Phx contact event
	//! 
	
	override protected void EOnContact(IEntity other, Contact extra)
	{
		if (!IsAlive())
			return;
		
		if (GetParent() == other)
			return;

		Transport transport = Transport.Cast(other);
		if (transport)
		{
			if (GetGame().IsServer())
			{
				RegisterTransportHit(transport);
			}
		}
	}
	
	protected void AttenuateSoundIfNecessary(SoundObject soundObject)
	{
		if (GetGame().GetPlayer() != NULL && (IsSoundInsideBuilding() != GetGame().GetPlayer().IsSoundInsideBuilding() || IsCameraInsideVehicle() != GetGame().GetPlayer().IsCameraInsideVehicle()))
		{
			soundObject.SetKind(WaveKind.WAVEATTALWAYS);
		}
		else
		{
			soundObject.SetKind(WaveKind.WAVEEFFECTEX);
		}
	}
	
	void CheckLiftWeapon();
	void ProcessLiftWeapon();
	
	void SetHandheldOpticsInUse(bool state)
	{
		m_HandheldOpticsInUse = state;
	}
	
	bool IsHandheldOpticsInUse()
	{
		return m_HandheldOpticsInUse;
	}
	
	bool IsLiftWeapon();
	float GetWeaponObstruction();
	bool IsWeaponObstructionBlockingADS();
	
	bool IsRaised()
	{
		return m_IsRaised;
	}

	bool IsFighting();
	
	//! ---------------- Checks if player can pick up heavy item -------------------------
	// Will return true when the item is not heavy as well
	bool CanPickupHeavyItem(notnull EntityAI item)
	{
		return !item.IsHeavyBehaviour() || (item.IsHeavyBehaviour() && DayZPlayerUtils.PlayerCanChangeStance(this, DayZPlayerConstants.STANCEIDX_ERECT));
	}
	
	bool CanPickupHeavyItemSwap(notnull EntityAI item1, notnull EntityAI item2)
	{
		return CanPickupHeavyItem(item1) && CanPickupHeavyItem(item2);
	}
	
	/*!
		Called when 2D optics are about to be drawn.
		\return Collection of optics to be drawn. 
		        Draws the provided optics when of `ItemOptics` type.
		        Draws optic of current muzzle when of `Weapon_Base` type.
	*/
	protected override array<InventoryItem> OnDrawOptics2D()
	{
		array<InventoryItem> optics;
		
		// Find preferred optics, i.e. active on in hands (if any are held) or the ones attached to weapon (if any is held)
		ItemOptics preferredOptics;
		{
			HumanInventory inventory = GetHumanInventory();
			EntityAI itemInHands = inventory.GetEntityInHands();
			
			ItemOptics opticsInHands = ItemOptics.Cast(itemInHands);
			if (opticsInHands)
			{
				preferredOptics = opticsInHands;
			}
			else
			{
				Weapon_Base weaponInHands = Weapon_Base.Cast(itemInHands);
				if (weaponInHands)
				{
					preferredOptics = weaponInHands.GetAttachedOptics();
				}
			}
		}
		
		// Draw selected optics when the player is viewing through them
		if (preferredOptics && preferredOptics.IsInOptics() && preferredOptics.IsUsingOptics2DModel())
		{
			optics = {}; // only allocate the array when necessary
			optics.Insert(preferredOptics);
		}
		
		// Draw equipped NVGs; employ better finding!
		EntityAI nvAttachment = GetNVEntityAttached();
		if (nvAttachment)
		{
			bool blockedByOptics = preferredOptics && preferredOptics.IsInOptics() && !preferredOptics.IsUsingOptics2DModel();
			if (!blockedByOptics && PlayerBase.Cast(this).IsNVGWorking())
			{
				NVGoggles nvg = NVGoggles.Cast(nvAttachment);
				if (nvg)
				{
					if (!optics) 
						optics = {};
					
					optics.Insert(nvg);
				}
			}
		}
		
		return optics;
	}
	
	//!
	//! TODO: transitional change - will be cleaned in near future
	//! Do NOT mod this method - will be removed
	private EntityAI GetNVEntityAttached()
	{
		EntityAI entity;
		
		if (FindAttachmentBySlotName("Eyewear") && FindAttachmentBySlotName("Eyewear").FindAttachmentBySlotName("NVG"))
		{
			entity = FindAttachmentBySlotName("Eyewear").FindAttachmentBySlotName("NVG");
		}
		else if (FindAttachmentBySlotName("Headgear") && FindAttachmentBySlotName("Headgear").FindAttachmentBySlotName("NVG"))
		{
			entity = FindAttachmentBySlotName("Headgear").FindAttachmentBySlotName("NVG");
		}

		return entity;
	}

#ifdef DIAG_DEVELOPER
	Weapon_Base SpawnWeaponInHands(string type)
	{
		return Weapon_Base.Cast(GetHumanInventory().CreateInHands(type));
	}
#endif
	
///////////////////////////////////
//Obsolete stuff below this point//
///////////////////////////////////
	
	static const int 									DEAD_SCREEN_DELAY = 1000; //! DEPRECATED
	protected bool										m_CameraEyeZoom; //DEPRECATED
	ref Timer 											m_ADSAutomationTimer; //DEPRECATED
	protected bool 										m_ADSTimerLaunched; //DEPRECATED
	protected bool 										m_ProcessWeaponRaiseCompleted; //DEPRECATED
	protected bool										m_IsFireWeaponRaised; //DEPRECATED, value still set for legacy reasons
	protected bool										m_WasIronSight;	// moved from player to weapon
	void SetReturnToOptics(bool state); //DEPRECATED
	void RunADSTimer(); //DEPRECATED
	void StopADSTimer(); //DEPRECATED
	void SendCompleteWeaponRaiseJuncture(); //DEPRECATED
	void DepleteStamina(EStaminaModifiers modifier, float dT = -1) {};
}
