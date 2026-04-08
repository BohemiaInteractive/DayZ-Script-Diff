enum EBoatEffects
{
	PTC_FRONT = 0,
	PTC_BACK,
	PTC_SIDE_L,
	PTC_SIDE_R
}

enum EBoatOperationalState
{
	OK = 0,
	RUINED = 1,
	NO_FUEL = 2,
	NO_IGNITER = 4,
}

enum EBoatEngineSoundState
{
	NONE,
	START_OK,
	START_NO_FUEL,
	STOP_OK,
	STOP_NO_FUEL
}

class BoatScriptOwnerState : BoatOwnerState
{
};

class BoatScriptMove : BoatMove
{
};

#ifdef DIAG_DEVELOPER 
BoatScript _boat;
#endif

/*!
	Base script class for boats
*/
class BoatScript : Boat
{
	protected const int DECAY_PLAYER_RANGE = 300;	// when a player is found within this range, decay is deactivated
	protected const int DECAY_FLAG_RANGE = 100; // when a boat is parked around a territoryflag, decay is deactivated
	protected const float DECAY_TICK_FREQUENCY = 10; // seconds
	protected const float DECAY_DAMAGE = 0.017;
	protected const float DECAY_FLIPPED_MULT = 4;
	
	protected const float SOUND_ENGINE_FADE = 0.2;
	protected const float SPLASH_THRESHOLD_CONDITION = 0.08; // diff between sea surface and propeller pos which determines if the splash should happen
	protected const float SPLASH_THRESHOLD = 0.18; // diff between sea surface and propeller pos which determines when the splash happens
	
	private static ref map<typename, ref TInputActionMap> m_BoatTypeActionsMap = new map<typename, ref TInputActionMap>();
	private TInputActionMap m_InputActionMap;
	private bool m_ActionsInitialized;	
	
	protected vector m_VelocityPrevTick;
	protected float m_MomentumPrevTick;
	protected ref VehicleContactData m_ContactData;
	protected ref Timer m_DecayTimer;
	
	// particles
	protected bool m_UpdateParticles;
	protected ref EffectBoatWaterBase m_WaterEffects[4];
	
	// sounds
	protected bool m_SplashIncoming;	// boat is high enough for the splash to trigger when it falls back down to sea level
	protected bool m_PlaySoundEngineStopNoFuel;
	protected bool m_PlaySoundImpactLight;
	protected bool m_PlaySoundImpactHeavy;
	protected bool m_PlaySoundPushBoat;
	protected bool m_IsEngineSoundFading;
	protected bool m_EngineFadeDirection; // true>in | false>out
	protected float m_EngineFadeTime;
	protected string m_SoundImpactLight;
	protected string m_SoundImpactHeavy;
	protected string m_SoundPushBoat;
	protected string m_SoundWaterSplash;
	protected string m_SoundEngineStart;
	protected string m_SoundEngineStop;
	protected string m_SoundEngineStartNoFuel;
	protected string m_SoundEngineStopNoFuel;
	
	// caches to avoid multiple triggers
	protected ref EffectSound m_SoundPushBoatEffect;
	protected ref EffectSound m_SoundWaterSplashEffect;
	protected ref EffectSound m_SoundEngineEffect;
	protected ref EffectSound m_SoundEngineEffectDeletion; // hold engine sound which is being stopped & deleted while another is starting 

	void BoatScript()
	{
#ifdef DIAG_DEVELOPER 
		_boat = this;
#endif
		
		SetEventMask(EntityEvent.POSTSIMULATE);
		SetEventMask(EntityEvent.FRAME);
		
		m_PlaySoundImpactLight = false;
		m_PlaySoundImpactHeavy = false;
		
		RegisterNetSyncVariableBool("m_PlaySoundEngineStopNoFuel");
		RegisterNetSyncVariableBool("m_PlaySoundPushBoat");
		RegisterNetSyncVariableBoolSignal("m_PlaySoundImpactLight");
		RegisterNetSyncVariableBoolSignal("m_PlaySoundImpactHeavy");
		
		m_SoundImpactHeavy = "boat_01_hit_light_SoundSet";
		m_SoundImpactLight = "boat_01_hit_heavy_SoundSet";
		m_SoundPushBoat = "boat_01_push_SoundSet";
		m_SoundWaterSplash = "boat_01_splash_SoundSet";
		m_SoundEngineStart = "boat_01_engine_start_SoundSet";
		m_SoundEngineStop = "boat_01_engine_stop_SoundSet";
		m_SoundEngineStartNoFuel = "boat_01_engine_start_no_fuel_SoundSet";
		m_SoundEngineStopNoFuel = "boat_01_engine_stop_no_fuel_SoundSet";
				
		if (g_Game.IsServer())
		{
			m_DecayTimer = new Timer( CALL_CATEGORY_SYSTEM );
			m_DecayTimer.Run(DECAY_TICK_FREQUENCY, this, "DecayHealthTick", NULL, true);
		}
		
		if (g_Game.IsDedicatedServer())
			return;
		
		m_WaterEffects[EBoatEffects.PTC_FRONT] 	= new EffectBoatWaterFront();
		m_WaterEffects[EBoatEffects.PTC_BACK] 	= new EffectBoatWaterBack();
		m_WaterEffects[EBoatEffects.PTC_SIDE_L] = new EffectBoatWaterSide();
		m_WaterEffects[EBoatEffects.PTC_SIDE_R] = new EffectBoatWaterSide();
			
		if (MemoryPointExists("ptcFxFront"))
			m_WaterEffects[EBoatEffects.PTC_FRONT].AttachTo(this, GetMemoryPointPos("ptcFxFront"));
		
		if (MemoryPointExists("ptcFxBack"))
			m_WaterEffects[EBoatEffects.PTC_BACK].AttachTo(this, GetMemoryPointPos("ptcFxBack"));
		
		if (MemoryPointExists("ptcFxSide1"))
			m_WaterEffects[EBoatEffects.PTC_SIDE_L].AttachTo(this, GetMemoryPointPos("ptcFxSide1"));
		
		if (MemoryPointExists("ptcFxSide2"))
			m_WaterEffects[EBoatEffects.PTC_SIDE_R].AttachTo(this, GetMemoryPointPos("ptcFxSide2"));
	}
	
	void ~BoatScript()
	{
		if (m_DecayTimer)
			m_DecayTimer.Stop();
		
		#ifndef SERVER
			CleanupEffects();
		#endif
	}
	
	override void EEDelete(EntityAI parent)
	{
		if (!g_Game.IsDedicatedServer())
			CleanupEffects();
	}

	void InitializeActions()
	{
		m_InputActionMap = m_BoatTypeActionsMap.Get(this.Type());
		if (!m_InputActionMap)
		{
			TInputActionMap iam = new TInputActionMap();
			m_InputActionMap = iam;
			SetActions();
			m_BoatTypeActionsMap.Insert(this.Type(), m_InputActionMap);
		}
	}
	
	override void GetActions(typename action_input_type, out array<ActionBase_Basic> actions)
	{
		if (!m_ActionsInitialized)
		{
			m_ActionsInitialized = true;
			InitializeActions();
		}
		
		actions = m_InputActionMap.Get(action_input_type);
	}
	
	override bool DisableVicinityIcon()
	{
		return true;
	}
	
	override string GetVehicleType()
	{
		return "VehicleTypeBoat";
	}
	
	override bool IsInventoryVisible()
	{
		return (g_Game.GetPlayer() && (!g_Game.GetPlayer().GetCommand_Vehicle() || g_Game.GetPlayer().GetCommand_Vehicle().GetTransport() == this));
	}

	override float GetTransportCameraDistance()
	{
		return 4.5;
	}

	override vector GetTransportCameraOffset()
	{
		return "0 1.4 0";
	}
	
	override int GetAnimInstance()
	{
		return VehicleAnimInstances.ZODIAC;
	}

	override int Get3rdPersonCameraType()
	{
		return DayZPlayerCameras.DAYZCAMERA_3RD_VEHICLE;
	}

	override bool CrewCanGetThrough(int posIdx)
	{
		return true;
	}
	
	override bool CanReachSeatFromSeat(int currentSeat, int nextSeat)
	{
		return true;
	}
	
	override bool CanReachSeatFromDoors(string pSeatSelection, vector pFromPos, float pDistance = 1.0)
	{
		return true;
	}

	override bool CanReachDoorsFromSeat(string pDoorsSelection, int pCurrentSeat)
	{
		return true;
	}
	
	override bool IsAreaAtDoorFree(int currentSeat, float maxAllowedObjHeight, inout vector extents, out vector transform[4])
	{
		return super.IsAreaAtDoorFree(currentSeat, maxAllowedObjHeight, extents, transform);
	}
	
	/*!
		Gets called everytime the game wants to start the engine.

		\return true if the engine can start, false otherwise.
	*/
	override bool OnBeforeEngineStart()
	{
		EBoatOperationalState state = CheckOperationalRequirements();
		return state == EBoatOperationalState.OK;
	}
	
	// Whether the car engine can be started
	int CheckOperationalRequirements()
	{
		int state = EBoatOperationalState.OK;

		EntityAI item = null;

		if (GetHealthLevel("") >= GameConstants.STATE_RUINED || GetHealthLevel("Engine") >= GameConstants.STATE_RUINED)
		{
			state |= EBoatOperationalState.RUINED;
		}
		
		if (GetFluidFraction(BoatFluid.FUEL) <= 0)
		{
			state |= EBoatOperationalState.NO_FUEL;
		}

		if (IsVitalSparkPlug())
		{
			item = FindAttachmentBySlotName("SparkPlug");
			if (!item || (item && item.IsRuined()))
				state |= EBoatOperationalState.NO_IGNITER;
		}
		
		return state;
	}

	void OnIgnition()
	{
		EBoatOperationalState state = CheckOperationalRequirements();

		if (state == EBoatOperationalState.RUINED)
		{
			return;
		}

		if (state & EBoatOperationalState.NO_IGNITER)
		{
			HandleEngineSound(EBoatEngineSoundState.START_NO_FUEL);
			return;
		}

		if (state & EBoatOperationalState.NO_FUEL)
		{
			HandleEngineSound(EBoatEngineSoundState.START_NO_FUEL);
			return;
		}
	}
	
	override void OnEngineStart()
	{
		super.OnEngineStart();

		if (g_Game.IsDedicatedServer())
			return;
		
		FadeEngineSound(true);
		HandleEngineSound(EBoatEngineSoundState.START_OK);
		ClearWaterEffects(); // in case they are still active
	}
	
	override void OnEngineStop()
	{
		super.OnEngineStop();
		
		if (g_Game.IsDedicatedServer())
			return;
		
		FadeEngineSound(false);
		HandleEngineSound(EBoatEngineSoundState.STOP_OK);
		
		vector mat[4];
		dBodyGetWorldTransform(this, mat);
		vector pos = mat[3] + VectorToParent(PropellerGetPosition());
		
		if (g_Game.GetWaterDepth(pos) < 0) // stop instantly
			StopParticleUpdate();
		else
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(StopParticleUpdate, 3000);
	}
	
	override void EEOnCECreate()
	{
		float maxVolume = GetFluidCapacity(BoatFluid.FUEL);
		float amount = Math.RandomFloat(0.0, maxVolume * 0.35);

		Fill(BoatFluid.FUEL, amount);
	}

	override void OnInput(float dt)
	{
		super.OnInput(dt);
	}
	
	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		if (!IsProxy())
		{
			if (EngineIsOn())
			{
				if (GetFluidFraction(BoatFluid.FUEL) <= 0)
				{	
					if (g_Game.IsServer())
					{	
						m_PlaySoundEngineStopNoFuel = true;
						SetSynchDirty();
						m_PlaySoundEngineStopNoFuel = false;
					}
					
					EngineStop();
				}
			}
		}
		
		if (g_Game.IsServer())
		{
			CheckContactCache();
			m_VelocityPrevTick = GetVelocity(this);
			m_MomentumPrevTick = GetMomentum();
		}
		
		if (!g_Game.IsDedicatedServer())
		{
			if (EngineIsOn())
				m_UpdateParticles = true;
			
			if (m_UpdateParticles)
				HandleBoatSplashSound();
			
		}
	}
	
	override void EOnSimulate(IEntity other, float dt)
	{
		if (!IsProxy())
		{
			if (EngineIsOn())
			{
				vector mat[4];
				dBodyGetWorldTransform(this, mat);
				vector pos = mat[3] + VectorToParent(PropellerGetPosition());
				
				if (g_Game.GetWaterDepth(pos) < -0.2)
					EngineStop();
			}
		}
	}
	
	override void EOnFrame(IEntity other, float timeSlice)
	{
		if (!g_Game.IsDedicatedServer())
		{
			if (m_UpdateParticles)
				UpdateParticles(timeSlice);
			
			if (m_IsEngineSoundFading)
			{
				m_EngineFadeTime -= timeSlice;
				if (m_EngineFadeTime < 0)
					m_IsEngineSoundFading = false;
			}
		}
	}
	
	//! WARNING: Can be called very frequently in one frame, use with caution
	override void OnContact(string zoneName, vector localPos, IEntity other, Contact data)
	{
		if (g_Game.IsServer())
		{
			if (m_ContactData)
				return;
		
			float momentumDelta = GetMomentum() - m_MomentumPrevTick;
			float dot = vector.Dot(m_VelocityPrevTick.Normalized(), GetVelocity(this).Normalized());
			if (dot < 0)
				momentumDelta = m_MomentumPrevTick;
			
			m_ContactData = new VehicleContactData();
			m_ContactData.SetData(localPos, other, momentumDelta); // change to local pos
		}
		
		if (!IsProxy())
		{
			if (EngineIsOn() && !CheckOperationalState())
				EngineStop();
		}
	}
	
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		
		if (!IsProxy())
		{
			if (EngineIsOn() && !CheckOperationalState())
				EngineStop();
		}
	}
		
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
				
		if (m_PlaySoundImpactHeavy)
		{
			EffectSound impactHeavy;
			PlaySound(m_SoundImpactHeavy, impactHeavy, GetPosition());
			m_PlaySoundImpactHeavy = false;
		}
		
		if (m_PlaySoundImpactLight)
		{
			EffectSound impactLight;
			PlaySound(m_SoundImpactLight, impactLight, GetPosition());
			m_PlaySoundImpactLight = false;
		}
		
		if (m_PlaySoundPushBoat)
			PlaySound(m_SoundPushBoat, m_SoundPushBoatEffect, GetPosition());
		else if (m_SoundPushBoatEffect && m_SoundPushBoatEffect.IsPlaying())
			m_SoundPushBoatEffect.Stop();
		
		if (m_PlaySoundEngineStopNoFuel)
		{
			HandleEngineSound(EBoatEngineSoundState.STOP_NO_FUEL);
			m_PlaySoundEngineStopNoFuel = false;
		}
	}
	
	protected override bool DetectFlipped(VehicleFlippedContext ctx)
	{
		if (!DetectFlippedUsingSurface(ctx, GameConstants.VEHICLE_FLIP_ANGLE_TOLERANCE))
			return false;
		return true;
	}
	
	override float OnSound(BoatSoundCtrl ctrl, float oldValue)
	{
		if (m_IsEngineSoundFading)
		{
			if (m_EngineFadeDirection)
				oldValue = Math.InverseLerp(SOUND_ENGINE_FADE, 0, m_EngineFadeTime);
			else 
				oldValue = Math.InverseLerp(0, SOUND_ENGINE_FADE, m_EngineFadeTime);
			
			return oldValue;
		}
		
		return super.OnSound(ctrl, oldValue);
	}

	override void MarkCrewMemberUnconscious(int crewMemberIndex)
	{
		if (!IsAuthority())
			return;

		if (crewMemberIndex == DayZPlayerConstants.VEHICLESEAT_DRIVER)
		{
			EngineStop();
		}
	}

	override void MarkCrewMemberDead(int crewMemberIndex)
	{
		if (!IsAuthority())
			return;

		if (crewMemberIndex == DayZPlayerConstants.VEHICLESEAT_DRIVER)
		{
			EngineStop();
		}
	}
	
	protected void DecayHealthTick()
	{
		if ( (GetCEApi() && !GetCEApi().AvoidPlayer(GetPosition(), DECAY_PLAYER_RANGE)) || CfgGameplayHandler.GetBoatDecayMultiplier() <= 0 ) 
			return;
				
		float damage = DECAY_DAMAGE * CfgGameplayHandler.GetBoatDecayMultiplier();
		
		if (IsFlipped())
			damage *= DECAY_FLIPPED_MULT;
		
		if (IsInFlagRange())
			damage *= 0.3;
		
		AddHealth("Engine","Health", -damage);
	}
	
	protected bool IsInFlagRange()
	{
		array<vector> locations = g_Game.GetMission().GetActiveRefresherLocations();
		if (!locations)
			return false;
		
		int count = locations.Count();
		if (count > 0)
		{
			vector pos = GetWorldPosition();
			for (int i = 0; i < count; i++)
			{
				if (vector.Distance(pos, locations.Get(i)) < DECAY_FLAG_RANGE)
					return true;
			}
			
			return false;
		}
		else
			return false;
	}
	
	bool CheckOperationalState()
	{
		if (GetHealthLevel("") >= GameConstants.STATE_RUINED || GetHealthLevel("Engine") >= GameConstants.STATE_RUINED)
			return false;
		
		if (IsVitalSparkPlug())
		{
			EntityAI item = FindAttachmentBySlotName("SparkPlug");
			if (!item || (item && item.IsRuined()))
				return false;
		}
		
		return true;
	}

	override void OnDriverExit(Human player)
	{
		super.OnDriverExit(player);
		
		if (GetGear() != GetNeutralGear())
		{
			EngineStop();
		}
	}
	
	// Server side event for jump out processing 
	void OnVehicleJumpOutServer(GetOutTransportActionData data)
	{
		vector posMS = data.m_Player.WorldToModel(data.m_Player.GetPosition());
		float healthCoef = Math.InverseLerp(ActionGetOutTransport.HEALTH_LOW_SPEED_VALUE, ActionGetOutTransport.HEALTH_HIGH_SPEED_VALUE, data.m_Speed) * 0.5;
		healthCoef = Math.Clamp(healthCoef, 0.0, 1.0);
		data.m_Player.ProcessDirectDamage(DamageType.CUSTOM, data.m_Player, "", "FallDamageHealth", posMS, healthCoef);
	}
	
	protected void HandleEngineSound(EBoatEngineSoundState state)
	{
		if (g_Game.IsDedicatedServer())
			return;
		
		string soundset;
		
		switch (state)
		{
			case EBoatEngineSoundState.START_OK:
				soundset = m_SoundEngineStart;
				break;
			case EBoatEngineSoundState.STOP_OK:
				soundset = m_SoundEngineStop;
				break;
			case EBoatEngineSoundState.START_NO_FUEL:
				soundset = m_SoundEngineStartNoFuel;
				break;
			case EBoatEngineSoundState.STOP_NO_FUEL:
				soundset = m_SoundEngineStopNoFuel;
				break;
		}
		
		// already playing same type of sound
		if (m_SoundEngineEffect)
		{
			if (m_SoundEngineEffect.GetSoundSet() == soundset) // already playing same type of sound
				return;
			else 
			{
				m_SoundEngineEffectDeletion = m_SoundEngineEffect;
				m_SoundEngineEffectDeletion.SoundStop(); // stop the existing sound
				
				m_SoundEngineEffect = null;
			}

		}
		
		PlaySound(soundset, m_SoundEngineEffect, ModelToWorld(PropellerGetPosition()));
	}
	
	protected void PlaySound(string soundset, inout EffectSound sound, vector position = vector.Zero)
	{
		#ifndef SERVER
		if (position == vector.Zero)
			position = GetPosition();
		
		if (!sound)
		{
			sound =	SEffectManager.PlaySoundCachedParams(soundset, position);
			sound.SetAttachmentParent(this);
			sound.SetAutodestroy(true);	// SoundWaveObjects tend to null themselves for unknown reasons, breaking the effect in the process
		}
		else
		{
			if (!sound.IsSoundPlaying())
			{
				sound.SetCurrentPosition(position);
				sound.SoundPlay();
			}
		}
		#endif
	}
	
	protected void HandleBoatSplashSound()
	{
		float propPosRelative = g_Game.SurfaceGetSeaLevel() - CoordToParent(PropellerGetPosition())[1];
		if (propPosRelative < SPLASH_THRESHOLD_CONDITION)
			m_SplashIncoming = true;
		
		if (m_SplashIncoming && propPosRelative > SPLASH_THRESHOLD)
		{
			m_SoundWaterSplashEffect = SEffectManager.CreateSound(m_SoundWaterSplash, GetPosition());
			m_SoundWaterSplashEffect.SetAttachmentParent(this);
			m_SoundWaterSplashEffect.SetLocalPosition(PropellerGetPosition());
			m_SoundWaterSplashEffect.SetAutodestroy(true);
			
			vector velocity = dBodyGetVelocityAt(this, CoordToParent(PropellerGetPosition()));
			float speed = velocity.Normalize() * 3.6; // to km/h
			float lerp = Math.InverseLerp(0, 30, speed);
			m_SoundWaterSplashEffect.SetSoundVolume(lerp);
			
			m_SoundWaterSplashEffect.Start();
					
			m_SplashIncoming = false;
		}
	}
	
	protected void SyncSoundImpactLight()
	{
		if (!m_PlaySoundImpactLight)
		{
			m_PlaySoundImpactLight = true;
			SetSynchDirty();
		}
	}
	
	protected void SyncSoundImpactHeavy()
	{
		if (!m_PlaySoundImpactHeavy)
		{
			m_PlaySoundImpactHeavy = true;
			SetSynchDirty();
		}
	}
	
	void SyncSoundPushBoat(bool play)
	{
		if (m_PlaySoundPushBoat != play)
		{
			m_PlaySoundPushBoat = play;
			SetSynchDirty();
		}
	}
	
	// Set engine sound to fade on start/stop
	protected void FadeEngineSound(bool fadeIn)
	{
		m_IsEngineSoundFading = true;
		m_EngineFadeDirection = fadeIn;
		m_EngineFadeTime = SOUND_ENGINE_FADE;
	}
	
	protected void FlipVehicle()
	{
		vector orient = GetOrientation();
		orient[2] = orient[2] + 180;
		SetOrientation(orient);
		
		vector pos = GetPosition();
		pos[1] = pos[1] + 0.5;
		SetPosition(pos);
	}
	
	protected void CheckContactCache()
	{
		if (!m_ContactData)
			return;
		
		float impulse = Math.AbsInt(m_ContactData.m_Impulse);
		m_ContactData = null;
		
		if (impulse < GameConstants.CARS_CONTACT_DMG_MIN)
			return;
		else if (impulse < GameConstants.CARS_CONTACT_DMG_THRESHOLD)
			SyncSoundImpactLight();
		else
			SyncSoundImpactHeavy();
	}
	
	protected void SetActions()
	{
		AddAction(ActionGetInTransport);
		AddAction(ActionPushBoat);
	}
	
	void AddAction(typename actionName)
	{
		ActionBase action = ActionManagerBase.GetAction(actionName);

		if (!action)
		{
			Debug.LogError("Action " + actionName + " dosn't exist!");
			return;
		}		
		
		typename ai = action.GetInputType();
		if (!ai)
		{
			m_ActionsInitialized = false;
			return;
		}

		array<ActionBase_Basic> actionArray = m_InputActionMap.Get(ai);
		
		if (!actionArray)
		{
			actionArray = new array<ActionBase_Basic>;
			m_InputActionMap.Insert(ai, actionArray);
		}
		
		if (LogManager.IsActionLogEnable())
		{
			Debug.ActionLog(action.ToString() + " -> " + ai, this.ToString() , "n/a", "Add action" );
		}
		
		actionArray.Insert(action);
	}
	
	void RemoveAction(typename actionName)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		ActionBase action = player.GetActionManager().GetAction(actionName);
		typename ai = action.GetInputType();
		array<ActionBase_Basic> actionArray = m_InputActionMap.Get(ai);
		
		if (actionArray)
		{
			actionArray.RemoveItem(action);
		}
	}
	
	protected void UpdateParticles(float timeSlice = 0)
	{		
		for (int i; i < 4; i++)
		{
			m_WaterEffects[i].Update(timeSlice);
		}
	}
	
	protected void StopParticleUpdate()
	{
		m_UpdateParticles = false;
		ClearWaterEffects();
	}
	
	protected void ClearWaterEffects()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(StopParticleUpdate);
		
		for (int i; i < 4; i++)
		{
			if (m_WaterEffects[i].IsPlaying())
			{
				if (m_WaterEffects[i].GetParticle())
				{
					m_WaterEffects[i].GetParticle().SetParticleParam(EmitorParam.BIRTH_RATE, 0);
					m_WaterEffects[i].GetParticle().SetParticleParam(EmitorParam.BIRTH_RATE_RND, 0);
				}
				m_WaterEffects[i].Stop();
			}
		}
	}
	
	// Called on destroy/stream out
	protected void CleanupEffects()
	{		
		for (int i; i < 4; i++)
		{
			if (m_WaterEffects[i].IsRegistered())
			{
				m_WaterEffects[i].Stop();
				SEffectManager.EffectUnregisterEx(m_WaterEffects[i]);
			}
		}
		
		SEffectManager.DestroyEffect(m_SoundPushBoatEffect);
		SEffectManager.DestroyEffect(m_SoundWaterSplashEffect);
	}

	override void GetDebugActions(out TSelectableActionInfoArrayEx outputList)
	{
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.FLIP_ENTITY, "Flip vehicle", FadeColors.LIGHT_GREY));
		
		super.GetDebugActions(outputList);
	}
	
	override bool OnAction(int action_id, Man player, ParamsReadContext ctx)
	{
		if (super.OnAction(action_id, player, ctx))
			return true;

		if (!g_Game.IsServer())
		{
			return false;
		}

		switch (action_id)
		{			
			case EActions.FLIP_ENTITY:
				FlipVehicle();
				return true;
		}
	
		return false;
	}

	protected override event typename GetOwnerStateType()
	{
		return BoatScriptOwnerState;
	}

	protected override event typename GetMoveType()
	{
		return BoatScriptMove;
	}
	
	#ifdef DIAG_DEVELOPER
	override void FixEntity()
	{
		super.FixEntity();

		if (g_Game.IsServer())
			Fill(BoatFluid.FUEL, GetFluidCapacity(BoatFluid.FUEL));
		
		
	}
	#endif
}