enum EBoatEffects
{
	PTC_FRONT = 0,
	PTC_BACK,
	PTC_SIDE_L,
	PTC_SIDE_R
}

class BoatScriptOwnerState : BoatOwnerState
{
};

class BoatScriptMove : BoatMove
{
};

class BoatScript : Boat
{
	private static ref map<typename, ref TInputActionMap> m_BoatTypeActionsMap = new map<typename, ref TInputActionMap>();
	private TInputActionMap m_InputActionMap;
	private bool m_ActionsInitialized;	
	
	protected vector m_VelocityPrevTick;
	protected float m_MomentumPrevTick;
	protected ref VehicleContactData m_ContactData;
	
	// particles
	protected bool m_UpdateParticles;
	protected ref EffectBoatWaterBase m_WaterEffects[4];
	
	// sounds
	protected bool m_PlaySoundImpactLight;
	protected bool m_PlaySoundImpactHeavy;
	protected bool m_PlaySoundPushBoat;
	protected string m_SoundImpactLight;
	protected string m_SoundImpactHeavy;
	protected string m_SoundPushBoat;
	protected string m_SoundWaterSplash;
	protected ref EffectSound m_SoundImpactLightEffect;
	protected ref EffectSound m_SoundImpactHeavyEffect;
	protected ref EffectSound m_SoundPushBoatEffect;
	protected ref SoundBoatWaterBack m_SoundWaterSplashEffect;

	void BoatScript()
	{
		SetEventMask(EntityEvent.POSTSIMULATE);
		SetEventMask(EntityEvent.FRAME);
		
		m_PlaySoundImpactLight = false;
		m_PlaySoundImpactHeavy = false;
		
		RegisterNetSyncVariableBool("m_PlaySoundPushBoat");
		RegisterNetSyncVariableBoolSignal("m_PlaySoundImpactLight");
		RegisterNetSyncVariableBoolSignal("m_PlaySoundImpactHeavy");
		
		m_SoundImpactHeavy = "offroad_hit_light_SoundSet";
		m_SoundImpactLight = "offroad_hit_heavy_SoundSet";
		m_SoundPushBoat = "Offroad_02_Horn_SoundSet";
		m_SoundWaterSplash = "Fishing_splash_SoundSet";
				
		if (GetGame().IsDedicatedServer())
			return;
		
		m_WaterEffects[EBoatEffects.PTC_FRONT] 	= new EffectBoatWaterFront();
		m_WaterEffects[EBoatEffects.PTC_BACK] 	= new EffectBoatWaterBack();
		m_WaterEffects[EBoatEffects.PTC_SIDE_L] = new EffectBoatWaterSide();
		m_WaterEffects[EBoatEffects.PTC_SIDE_R] = new EffectBoatWaterSide();
		m_SoundWaterSplashEffect = new SoundBoatWaterBack();
			
		if (MemoryPointExists("ptcFxFront"))
			m_WaterEffects[EBoatEffects.PTC_FRONT].AttachTo(this, GetMemoryPointPos("ptcFxFront"));
		
		if (MemoryPointExists("ptcFxBack"))
		{
			m_WaterEffects[EBoatEffects.PTC_BACK].AttachTo(this, GetMemoryPointPos("ptcFxBack"));
			m_SoundWaterSplashEffect.AttachSound(this, m_SoundWaterSplash, GetMemoryPointPos("ptcFxBack"));
		}
		
		if (MemoryPointExists("ptcFxSide1"))
			m_WaterEffects[EBoatEffects.PTC_SIDE_L].AttachTo(this, GetMemoryPointPos("ptcFxSide1"));
		
		if (MemoryPointExists("ptcFxSide2"))
			m_WaterEffects[EBoatEffects.PTC_SIDE_R].AttachTo(this, GetMemoryPointPos("ptcFxSide2"));
	}
	
	void ~BoatScript()
	{
		#ifndef SERVER
			CleanupEffects();
		#endif
	}
	
	override void EEDelete(EntityAI parent)
	{
		if (!GetGame().IsDedicatedServer())
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
		return (GetGame().GetPlayer() && (!GetGame().GetPlayer().GetCommand_Vehicle() || GetGame().GetPlayer().GetCommand_Vehicle().GetTransport() == this));
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
		return true;
	}

	override bool OnBeforeEngineStart()
	{		
		if (GetFluidFraction(BoatFluid.FUEL) <= 0)
			return false;
		
		if (IsVitalSparkPlug())
		{
			EntityAI item = FindAttachmentBySlotName("SparkPlug");
			if (!item || (item && item.IsRuined()))
				return false;
		}
		
		return true;
	}
	
	override void OnEngineStart()
	{
		super.OnEngineStart();

		if (GetGame().IsDedicatedServer())
			return;
		
		m_UpdateParticles = true;
		ClearWaterEffects(); // in case they are still active
	}
	
	override void OnEngineStop()
	{
		super.OnEngineStop();

		if (GetGame().IsDedicatedServer())
			return;
		
		// if beached, stop right away
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(StopParticleUpdate, 3000);
	}
	
	override void EEOnCECreate()
	{
		float maxVolume = GetFluidCapacity(BoatFluid.FUEL);
		float amount = Math.RandomFloat(0.0, maxVolume * 0.35);

		Fill(BoatFluid.FUEL, amount);
	}
	
	override void EOnPostSimulate(IEntity other, float timeSlice)
	{		
		if (GetGame().IsServer())
		{
			if (EngineIsOn() && GetFluidFraction(BoatFluid.FUEL) <= 0)
				EngineStop();
			
			CheckContactCache();
			m_VelocityPrevTick = GetVelocity(this);
			m_MomentumPrevTick = GetMomentum();
		}
		
		if (!GetGame().IsDedicatedServer() && m_UpdateParticles)
			m_SoundWaterSplashEffect.Update();
	}
	
	override void EOnFrame(IEntity other, float timeSlice)
	{
		if (!GetGame().IsDedicatedServer())
		{
			if (m_UpdateParticles)
				UpdateParticles();
		}
	}
	
	override void EOnContact(IEntity other, Contact extra)
	{
		if (GetGame().IsServer())
		{
			if (m_ContactData)
				return;
		
			float momentumDelta = GetMomentum() - m_MomentumPrevTick;
			float dot = vector.Dot(m_VelocityPrevTick.Normalized(), GetVelocity(this).Normalized());
			if (dot < 0)
				momentumDelta = m_MomentumPrevTick;
			
			m_ContactData = new VehicleContactData();
			m_ContactData.SetData(extra.Position, other, momentumDelta); // change to local pos
		}
	}
		
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
				
		if (m_PlaySoundImpactHeavy)
		{
			PlaySound(m_SoundImpactHeavy, m_SoundImpactHeavyEffect);
			m_PlaySoundImpactHeavy = false;
		}
		
		if (m_PlaySoundImpactLight)
		{
			PlaySound(m_SoundImpactLight, m_SoundImpactLightEffect);
			m_PlaySoundImpactLight = false;
		}
		
		if (m_PlaySoundPushBoat)
			PlaySound(m_SoundPushBoat, m_SoundPushBoatEffect);
		else if (m_SoundPushBoatEffect && m_SoundPushBoatEffect.IsPlaying())
			m_SoundPushBoatEffect.Stop();
			
	}
	
	// Server side event for jump out processing 
	void OnVehicleJumpOutServer(GetOutTransportActionData data)
	{
		vector posMS = data.m_Player.WorldToModel(data.m_Player.GetPosition());
		float healthCoef = Math.InverseLerp(ActionGetOutTransport.HEALTH_LOW_SPEED_VALUE, ActionGetOutTransport.HEALTH_HIGH_SPEED_VALUE, data.m_Speed) * 0.5;
		healthCoef = Math.Clamp(healthCoef, 0.0, 1.0);
		data.m_Player.ProcessDirectDamage(DamageType.CUSTOM, data.m_Player, "", "FallDamageHealth", posMS, healthCoef);
	}
	
	protected void PlaySound(string soundset, inout EffectSound sound)
	{
		#ifndef SERVER
		//Print(this.GetPosition().ToString() + " playing " + soundset + " using "+  sound);
		
		if (!sound)
		{
			sound =	SEffectManager.PlaySoundCachedParams(soundset, GetPosition());
			sound.SetAutodestroy(true);	// SoundWaveObjects tend to null themselves for unknown reasons, breaking the effect in the process
		}
		else
		{
			if (!sound.IsSoundPlaying())
			{
				sound.SetCurrentPosition(GetPosition());
				sound.SoundPlay();
			}
		}
		#endif
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
	
	protected void AddAction(typename actionName)
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
	
	protected void RemoveAction(typename actionName)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		ActionBase action = player.GetActionManager().GetAction(actionName);
		typename ai = action.GetInputType();
		array<ActionBase_Basic> actionArray = m_InputActionMap.Get(ai);
		
		if (actionArray)
		{
			actionArray.RemoveItem(action);
		}
	}
	
	protected void UpdateParticles()
	{		
		for (int i; i < 4; i++)
		{
			m_WaterEffects[i].Update();
		}
	}
	
	protected void StopParticleUpdate()
	{
		m_UpdateParticles = false;
		ClearWaterEffects();
	}
	
	protected void ClearWaterEffects()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(StopParticleUpdate);
		
		for (int i; i < 4; i++)
		{
			if (m_WaterEffects[i].IsPlaying())
				m_WaterEffects[i].Stop();
		}
		
		m_SoundWaterSplashEffect.Clear();
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
		
		SEffectManager.DestroyEffect(m_SoundImpactLightEffect);
		SEffectManager.DestroyEffect(m_SoundImpactHeavyEffect);
		SEffectManager.DestroyEffect(m_SoundPushBoatEffect);
		SEffectManager.DestroyEffect(m_SoundWaterSplashEffect);
	}

	override void GetDebugActions(out TSelectableActionInfoArrayEx outputList)
	{
		super.GetDebugActions(outputList);
		
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.DELETE, "Delete", FadeColors.RED));
	}
	
	override bool OnAction(int action_id, Man player, ParamsReadContext ctx)
	{
		if (super.OnAction(action_id, player, ctx))
			return true;

		if (!GetGame().IsServer())
		{
			return false;
		}

		switch (action_id)
		{
			case EActions.DELETE:
				Delete();
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
}