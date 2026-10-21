enum MotorbikeHeadlightState
{
	NONE,
	MAIN
}

enum MotorbikeRearLightType
{
	NONE,
	BRAKES_ONLY,
	REVERSE_ONLY,
	BRAKES_AND_REVERSE
}

enum EMotorbikeOperationalState
{
	OK = 0,
	RUINED = 1,
	NO_FUEL = 2,
	NO_IGNITER = 4,
}

class MotorbikeScriptOwnerState : MotorbikeOwnerState
{
};

class MotorbikeScriptMove : MotorbikeMove
{
};

#ifdef DIAG_DEVELOPER
enum EMotorbikeDebugMode
{
	NONE,
	FORWARD_10,
	FORWARD_50,
};

MotorbikeScript _motorbike;
#endif

enum MotorbikeEngineSoundState
{
	NONE,
	STARTING,
	START_OK,
	START_NO_FUEL,
	START_NO_SPARKPLUG,
	STOP_OK,
	STOP_NO_FUEL
}

class MotorbikeScript : Motorbike
{
	protected static float DROWN_ENGINE_THRESHOLD	= 0.5;
	protected static float DROWN_ENGINE_DAMAGE 		= 150.0;
	protected static float WATER_DEPTH_PLAYER_EJECT	= 1.2;

	// TODO(kumarjac): VERY VERY VERY TEMPORARY, REMOVE THIS!
	bool m_BrakesArePressed;

	protected float m_Time;

	protected float m_EngineHealth;
	protected float m_FuelTankHealth;
	protected float m_BatteryHealth;
	protected float m_PlugHealth;
	
	protected static string m_HeadLightPoint			= "light_1_1";
	protected static string m_HeadLightTargetPoint		= "light_1_1_dir";
	protected static string m_RearLightPoint			= "light_1_2_reverse";
	//! ----------------------------------------
	
	protected ref VehicleLightsComponent m_LightsComponent;
	protected ref VehicleHornComponent m_HornComponent;
	protected ref VehicleVFXComponent m_VFXComponent;
	
	//!
	protected float m_DrownTime;
	protected static vector m_DrownEnginePos;
	
	protected float m_HeatComfortOverride;
	
	protected MotorbikeEngineSoundState m_EngineSoundState;
	protected MotorbikeEngineSoundState m_EngineLastSoundState;

	//!Sounds
	protected string m_EngineStarting 			= "";
	protected string m_EngineStartOK 			= "";
	protected string m_EngineStartBattery 		= "";
	protected string m_EngineStartPlug 			= "";
	protected string m_EngineStartFuel 			= "";
	protected string m_EngineStop	 			= "";
	protected string m_EngineStopFuel 			= "";
	private ref EffectSound m_PreStartSound;
	
#ifdef DIAG_DEVELOPER
	EMotorbikeDebugMode m_DebugMode;

	private const int DEBUG_MESSAGE_CLEAN_TIME_SECONDS = 10;
	private float m_DebugMessageCleanTime;
	private string m_DebugContactDamageMessage;
#endif

	void MotorbikeScript()
	{
		#ifdef DIAG_DEVELOPER 
		_motorbike = this;
		#endif
		
		SetEventMask(EntityEvent.POSTSIMULATE|EntityEvent.POSTFRAME);
		
		m_LightsComponent = VehicleLightsComponent(this);
		m_LightsComponent.RegisterSelection("FrontOn",	{new VehicleLightSelectionData(0, "frontReflectorMatOn")});
		m_LightsComponent.RegisterSelection("FrontOff",	{new VehicleLightSelectionData(0, "frontReflectorMatOff")});
		m_LightsComponent.RegisterSelection("BrakeOn",	{new VehicleLightSelectionData(1, "brakeReflectorMatOn")});
		m_LightsComponent.RegisterSelection("BrakeOff",	{new VehicleLightSelectionData(1, "brakeReflectorMatOff")});
		m_LightsComponent.RegisterSelection("TailOn",	{new VehicleLightSelectionData(2, "TailReflectorMatOn")});
		m_LightsComponent.RegisterSelection("TailOff",	{new VehicleLightSelectionData(2, "TailReflectorMatOff")});
		
		m_LightsComponent.RegisterSelection("DashboardOn",	{new VehicleLightSelectionData(3, "dashboardMatOn")});
		m_LightsComponent.RegisterSelection("DashboardOff",	{new VehicleLightSelectionData(3, "dashboardMatOff")});

		m_LightsComponent.RegisterLight("Front", new VehicleLightData(new VehicleLightProfileFront()));
		m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new VehicleLightProfileRear()));
		
		m_HornComponent = VehicleHornComponent(this);
		m_HornComponent.SetMode(VehicleHornMode.OFF);

		m_EngineHealth 		= 1;
		m_FuelTankHealth 	= 1;
		m_BatteryHealth 	= -1;
		m_PlugHealth 		= -1;
		
		m_HeatComfortOverride = 0.0;
		
		m_EngineSoundState = MotorbikeEngineSoundState.NONE;
		
		InitializeParticles();

		SetKickstand(1);
		
		RegisterNetSyncVariableBool("m_BrakesArePressed");
		RegisterNetSyncVariableBoolSignal("m_PlayCrashSoundLight");
		RegisterNetSyncVariableBoolSignal("m_PlayCrashSoundHeavy");

		RegisterNetSyncVariableInt("m_HornComponent.m_Mode", VehicleHornMode.OFF, VehicleHornMode.COUNT);
	}
	
	void ~MotorbikeScript()
	{
		#ifndef SERVER
		CleanupEffects();
		#endif
	}
	
	override void EEDelete(EntityAI parent)
	{
		#ifndef SERVER
		CleanupEffects();
		#endif
	}
	
	float GetHeatComfortOverride()
	{
		return m_HeatComfortOverride;
	}
	
	bool HasGangedTailAndBrakeLights()
	{
		return false;
	}

	protected void InitializeParticles()
	{
		if (g_Game.IsDedicatedServer())
			return;

		m_VFXComponent = VehicleVFXComponent(this);
		
		m_DrownEnginePos = m_VFXComponent.ConvertMemPointToPosition("engine");

		vector exhaustPosition = m_VFXComponent.ConvertMemPointToPosition("ptcExhaust_end");
		vector exhaustDirection;
		if (exhaustPosition != vector.Zero)
		{
			vector exhaustStart = m_VFXComponent.ConvertMemPointToPosition("ptcExhaust_start");
			vector tempOri = vector.Direction(exhaustStart, exhaustPosition);

			exhaustDirection[0] = -tempOri[2];
			exhaustDirection[1] = tempOri[1];
			exhaustDirection[2] = tempOri[0];
			
			exhaustDirection = exhaustDirection.Normalized().VectorToAngles();
		}
		else
			exhaustDirection = "1 1 1";
		
		m_VFXComponent.RegisterEffect(EVehicleVFXEffect.DUST_BEHIND, EffVehicleDust, new VehicleVFXDataParams(m_VFXComponent.ConvertMemPointToPosition("ptcDustPos")));
		m_VFXComponent.RegisterEffect(EVehicleVFXEffect.ENGINE_SMOKE, EffEngineSmoke, new VehicleVFXDataParams(m_VFXComponent.ConvertMemPointToPosition("ptcEnginePos")));
		m_VFXComponent.RegisterEffect(EVehicleVFXEffect.EXHAUST_SMOKE, EffExhaustSmoke, new VehicleVFXDataParams(exhaustPosition, exhaustDirection));

		m_VFXComponent.RegisterEffectGroup(EVehicleVFXEffectGroup.WHEEL_SMOKE, EffWheelSmoke, WheelCount(), new VehicleVFXDataParams(vector.Zero, "0 1 -1", 2.0));		
		m_VFXComponent.RegisterEffectGroup(EVehicleVFXEffectGroup.WHEEL_CONTACT, EffWheelContact, WheelCount(), new VehicleVFXDataParams(vector.Zero, vector.Up, 2.0));
	}
	
	override vector GetDefaultHitPosition()
	{
		return vector.Zero;
	}
	
	override VehicleHornComponent GetVehicleHornComponent()
	{
		return m_HornComponent;
	}

	override void OnDriverExit(Human player)
	{
		super.OnDriverExit(player);

		if (GetGear() != GetNeutralGear())
			EngineStop();
	}

	override bool CanGetIn()
	{
		if (!super.CanGetIn())
			return false;

		return !IsFallen();
	}

	override void OnStabilityStart()
	{
		super.OnStabilityStart();
	}

	override void OnStabilityStop()
	{
		super.OnStabilityStop();
	}
	
	override int GetFluidCapacityScript(ETransportFluid fluidType)
	{
		return GetFluidCapacity(fluidType);
	}
	
	override float GetFluidFractionScript(ETransportFluid fluidType)
	{
		return GetFluidFraction(fluidType);
	}
	
	override void FillScript(ETransportFluid fluidType, float amount)
	{
		Fill(fluidType, amount);
	}
	
	/*!
		Gets called every sound simulation step.
		In this callback, user can modify behaviour of sound controllers
		The higher the return value is the more muted sound is.
	*/
	override float OnSound(MotorbikeSoundCtrl ctrl, float oldValue)
	{
		switch (ctrl)
		{
			case MotorbikeSoundCtrl.ENGINE:
				if (!m_EngineStarted)
					return 0.0;

				break;
		}
		
		return oldValue;
	}
	
	override protected EffectSound CreateSoundForAnimationSource(string animSource)
	{
		vector position = vector.Zero;
		int pivotIndex = -1;
		
		string selectionName = GetSelectionFromAnimSource(animSource);
		if (selectionName != "")
		{			
			position = GetSelectionBasePositionLS(selectionName);
			
			int level = GetViewGeometryLevel();
		
			array<int> pivots = new array<int>();
							
			pivots.Clear();
			GetBonePivotsForAnimationSource(level, animSource, pivots);
				
			if (pivots.Count())
			{
				pivotIndex = pivots[0];
			}
		}

		WaveKind waveKind = WaveKind.WAVEEFFECTEX;		
		EffectSound sound = new EffectSound();

		sound.SetAutodestroy(true);
		sound.SetParent(this, pivotIndex);
		sound.SetPosition(ModelToWorld(position));
		sound.SetLocalPosition(position);
		sound.SetSoundWaveKind(waveKind);

		return sound;
	}

	private static ref map<typename, ref TInputActionMap> m_MotorbikeTypeActionsMap = new map<typename, ref TInputActionMap>();
	private TInputActionMap m_InputActionMap;
	private bool m_ActionsInitialized;	
	
	override string GetVehicleType()
	{
		return "VehicleTypeMotorbike";
	}
	
	bool IsVitalRadiator()
	{
		return false;
	}

	bool HasRadiator()
	{
		return false;
	}
	
	vector GetEnginePosWS()
	{
		return ModelToWorld(m_DrownEnginePos);
	}

	EntityAI GetRadiator()
	{
		return null;
	}

	void InitializeActions()
	{
		m_InputActionMap = m_MotorbikeTypeActionsMap.Get(this.Type());
		if (!m_InputActionMap)
		{
			TInputActionMap iam = new TInputActionMap();
			m_InputActionMap = iam;
			SetActions();
			m_MotorbikeTypeActionsMap.Insert(this.Type(), m_InputActionMap);
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

	protected void SetActions()
	{
		AddAction(ActionGetInTransport);
		AddAction(ActionPickUpMotorbike);
		AddAction(ActionSwitchLights);
		AddAction(ActionVehicleHornShort);
		AddAction(ActionVehicleHornLong);
	}
	
	void AddAction(typename actionName)
	{
		ActionBase action = ActionManagerBase.GetAction(actionName);

		if (!action)
		{
			Debug.LogError("Action " + actionName + " doesn't exist!");
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
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		if (newLevel == GameConstants.STATE_RUINED && oldLevel != newLevel)
		{
			switch (zone)
			{
				case "Engine":
					#ifndef SERVER
					if (g_Game.GetWaterDepth(GetEnginePosWS()) <= 0)
					{
						m_VFXComponent.PlayEffect(EVehicleVFXEffect.ENGINE_SMOKE);
						m_VFXComponent.SetEffectParticleState(EVehicleVFXEffect.ENGINE_SMOKE, ParticleList.HATCHBACK_COOLANT_OVERHEATED);
					}
					#endif
				break;
			}
		}
	}
	
	override void EEOnCECreate()
	{

		float maxVolume = GetFluidCapacity( MotorbikeFluid.FUEL );
		float amount = Math.RandomFloat(0.0, maxVolume * 0.35 );

		Fill( MotorbikeFluid.FUEL, amount );
	}

	override int GetSeatAnimationType(int posIdx)
	{
		return DayZPlayerConstants.VEHICLESEAT_DRIVER;
	}

	override int Get3rdPersonCameraType()
	{
		return DayZPlayerCameras.DAYZCAMERA_3RD_VEHICLE_MOTORBIKE;
	}
	
	override bool CrewCanEject(int posIdx)
	{
		return true;
	}
	
	override bool CrewShouldEject(int posIdx)
	{
		return IsFallen();
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
		//! selection name is same as mem point (eg. "seat_driver")
		if (pSeatSelection.Length() > 0)
		{
			if (MemoryPointExists(pSeatSelection))
			{
				vector conditionPointMS = GetMemoryPointPos(pSeatSelection);
				vector conditionPointWS = ModelToWorld(conditionPointMS);
				
				//! skip the height
				conditionPointWS[1] = 0;
				pFromPos[1] = 0;
				
				if (vector.Distance(pFromPos, conditionPointWS) <= pDistance)
					return true;
			}
		}

		return false;
	}

	override bool CanReachDoorsFromSeat(string pDoorsSelection, int pCurrentSeat)
	{
		return true;
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (GetCrashHeavySound())
			PlayCrashHeavySound();
		else if (GetCrashLightSound())
			PlayCrashLightSound();
		
		if (!IsOwner() && m_EngineSoundState != m_EngineLastSoundState)
			HandleEngineSound(m_EngineSoundState);
		
		m_HornComponent.HandleSound();
		
		UpdateLights();
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);

		switch (slot_name)
		{
			case "Reflector_1_1":
				if (g_Game.IsServer())
				{
					SetHealth("Reflector_1_1", "Health", item.GetHealth());
				}
				break;
			case "CarBattery":
			case "TruckBattery":
				if (g_Game.IsServer())
				{
					m_BatteryHealth = item.GetHealth01();
				}
				break;
			case "SparkPlug":
			case "GlowPlug":
				if (g_Game.IsServer())
				{
					m_PlugHealth = item.GetHealth01();
				}
				break;
		}
		
		if (g_Game.IsServer())
		{
			HandleHeatComfortOverrides(slot_name, item);
			Synchronize();
		}

		UpdateHeadlightState();
		UpdateLights();
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		switch (slot_name)
		{
			case "CarBattery":
			case "TruckBattery":
				m_BatteryHealth = -1;
				if (g_Game.IsServer())
				{
					if (EngineIsOn())
						EngineStop();
				}
	
				if (g_Game.IsServer())
				{	
					LightOff();
	
					UpdateBattery(ItemBase.Cast(item));
				}

				break;

			case "SparkPlug":
			case "GlowPlug":
				m_PlugHealth = -1;
				if (g_Game.IsServer() && EngineIsOn())
					EngineStop();
			
				break;
		}
		
		if (g_Game.IsServer())
		{
			HandleHeatComfortOverrides(slot_name, item, true);
			Synchronize();
		}
		
		UpdateHeadlightState();
		UpdateLights();
	}
	
	override void OnAttachmentRuined(EntityAI attachment)
	{
		super.OnAttachmentRuined(attachment);
		
		UpdateHeadlightState();
		UpdateLights();
	}
	
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (!super.CanReleaseAttachment(attachment))
		{
			return false;
		}

		if (EngineIsOn() || HasCrew())
		{
			return false;
		}

		return true;
	}
	
	override void EOnPostFrame(IEntity other, int extra)
	{
		if (!g_Game.IsDedicatedServer())
		{
			float motorbikeSpeed = GetVelocity(this).Length();
			ParticleParamsOverrideData particleParamsData;

			if (EngineIsOn())
			{
				if (!m_VFXComponent.IsEffectPlayed(EVehicleVFXEffect.EXHAUST_SMOKE))
				{
					particleParamsData = m_VFXComponent.GetParticleParamsOverrideData(EVehicleVFXEffect.EXHAUST_SMOKE);
					particleParamsData.m_FloatModifiers.InsertAt(motorbikeSpeed, 0);
					m_VFXComponent.PlayEffect(EVehicleVFXEffect.EXHAUST_SMOKE);
					m_VFXComponent.SetEffectParticleState(EVehicleVFXEffect.EXHAUST_SMOKE, ParticleList.HATCHBACK_EXHAUST_SMOKE);
				}
			}
			else
			{
				if (!g_Game.IsDedicatedServer())
					m_VFXComponent.StopEffect(EVehicleVFXEffect.EXHAUST_SMOKE);
			}
			
			//! dust behind handling
			{
				string surface;
			
				if (motorbikeSpeed > EffVehicleDust.WHEEL_DUST_THRESHOLD)
				{
					if (particleParamsData)
					{
						particleParamsData.ClearAll();
						particleParamsData = m_VFXComponent.GetParticleParamsOverrideData(EVehicleVFXEffect.DUST_BEHIND);
						particleParamsData.m_FloatModifiers.InsertAt(motorbikeSpeed, 0);
						particleParamsData.m_FloatModifiers.InsertAt(GetWeightCoef(), 1);
						m_VFXComponent.PlayEffect(EVehicleVFXEffect.DUST_BEHIND);
					}
				}
				else
					m_VFXComponent.StopEffect(EVehicleVFXEffect.DUST_BEHIND);
				
				
				int wheelCount = WheelCount();
				TVectorArray wheelsPosition = new TVectorArray();
	
				int i;
				for (i = 0; i < wheelCount; ++i)
					wheelsPosition.InsertAt(Vector(-1, -1, -1), i);
	
				for (i = 0; i < wheelCount; ++i)
				{
					CarWheel wheel = CarWheel.Cast(WheelGetEntity(i));
					if (!wheel)
						break;
	
					vector wheelPos;
					if (m_VFXComponent.CanPlayEffectGroup(EVehicleVFXEffectGroup.WHEEL_SMOKE))
					{
						if (WheelHasContact(i))
						{
							float wheelSpeed = WheelGetAngularVelocity(i) * wheel.GetRadius();
							wheelPos = WheelGetContactPosition(i);
							wheelPos = WorldToModel(wheelPos);
							wheelsPosition.Set(i, wheelPos);
							
							vector wheelVel = dBodyGetVelocityAt(this, wheelPos);
		
							vector transform[3];
							transform[2] = WheelGetDirection(i);
							transform[1] = vector.Up;
							transform[0] = transform[2] * transform[1];
		
							wheelVel = wheelVel.InvMultiply3(transform);
		
							float bodySpeed = wheelVel[2];
	
							bool applyWheelSmokeEffect = false;
	
							if ((wheelSpeed > 0 && bodySpeed > 0) || (wheelSpeed < 0 && bodySpeed < 0))
								applyWheelSmokeEffect = Math.AbsFloat(wheelSpeed) > Math.AbsFloat(bodySpeed) + EffWheelSmoke.WHEEL_SMOKE_THRESHOLD;
							else
								applyWheelSmokeEffect = Math.AbsFloat(wheelSpeed) > EffWheelSmoke.WHEEL_SMOKE_THRESHOLD;
							
							if (applyWheelSmokeEffect)
							{							
								m_VFXComponent.SetEffectGroupPositions(EVehicleVFXEffectGroup.WHEEL_SMOKE, wheelsPosition);
								m_VFXComponent.PlayEffectGroup(EVehicleVFXEffectGroup.WHEEL_SMOKE);
							}
							else
								m_VFXComponent.StopEffectGroup(EVehicleVFXEffectGroup.WHEEL_SMOKE);
						}
					}
					
					// actual wheel position
					wheelPos = WheelGetPositionLS(i);
					wheelsPosition.Set(i, wheelPos);
				
					//! WheelContact
					{
						if (m_VFXComponent.CanPlayEffectGroup(EVehicleVFXEffectGroup.WHEEL_CONTACT))
						{
							bool applyWheelContactEffect = Math.AbsFloat(wheelSpeed) > EffWheelContact.SPEED_THRESHOLD;
							if (wheel && applyWheelContactEffect)
							{
								particleParamsData = m_VFXComponent.GetEffectGroupParticleParamsOverrideData(EVehicleVFXEffectGroup.WHEEL_CONTACT);
								if (particleParamsData)
								{
									particleParamsData.m_FloatModifiers.InsertAt(motorbikeSpeed, 0);
									particleParamsData.m_FloatModifiers.InsertAt(GetWeightCoef(), 1);

									m_VFXComponent.SetSurfaceInfo(WheelGetSurface(i));
									m_VFXComponent.SetEffectGroupPositions(EVehicleVFXEffectGroup.WHEEL_CONTACT, wheelsPosition);
											
									m_VFXComponent.PlayEffectGroup(EVehicleVFXEffectGroup.WHEEL_CONTACT);
								}
							}
							else
								m_VFXComponent.StopEffectGroup(EVehicleVFXEffectGroup.WHEEL_CONTACT);
						}
					}
					
					
				}
			}
		}
	}
	
	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		m_Time += timeSlice;
		
		if (g_Game.IsServer())
		{
			#ifdef DIAG_DEVELOPER
			if (DEBUG_OUTPUT_TYPE & EVehicleDebugOutputType.CONTACT)
			{
				if (m_ContactCalled)
				{
					Debug.Log("Momentum delta: " + (GetMomentum() - m_MomentumPrevTick));
					Debug.Log("--------------------------------------------------------------------");
					m_ContactCalled = false;
				}
			}
			#endif
			
			
			CheckContactCache();
			m_VelocityPrevTick = GetVelocity(this);
			m_MomentumPrevTick = GetMomentum();
			#ifdef DIAG_DEVELOPER
			m_DebugMessageCleanTime += timeSlice;
			if (m_DebugMessageCleanTime >= DEBUG_MESSAGE_CLEAN_TIME_SECONDS)
			{
				m_DebugMessageCleanTime = 0;
				m_DebugContactDamageMessage = "";
			}
			#endif
		}
		
		if (m_Time >= GameConstants.VEHICLE_FLUIDS_TICK)
		{
			m_Time = 0;

			PartsHealthCheck();

			if (EngineIsOn())
			{
				bool isServerOrOwner = IsServerOrOwner();

				if (g_Game.IsServer())
				{
					//! Vehicle should stop the engine as first thing
					if (IsDamageDestroyed() || GetFluidFraction(MotorbikeFluid.FUEL) <= 0.0 || m_EngineHealth <= 0.0)
						EngineStop();
					
					float engineRPM = EngineGetRPM();
					if (EngineGetRPM() >= EngineGetRPMRedline())
					{
						if (engineRPM > EngineGetRPMMax())
							AddHealth( "Engine", "Health", -GetMaxHealth("Engine", "") * 0.05 );
	
						// only called on server, don't use deterministic RandomFloat
						float dmg = engineRPM * 0.001 * Math.RandomFloat(0.02, 1.0);
						ProcessDirectDamage(DamageType.CUSTOM, null, "Engine", "EnviroDmg", vector.Zero, dmg);
						SetEngineZoneReceivedHit(true);
					}
					else
						SetEngineZoneReceivedHit(false);
				}
				
				if (isServerOrOwner)
				{
					CheckVitalItemCallback(IsVitalSparkPlug(), SparkPlug.SLOT_ID, this, "EngineStop");
					
					if (GetFluidFraction(MotorbikeFluid.FUEL) > 0.0 && m_FuelTankHealth < GameConstants.DAMAGE_DAMAGED_VALUE)
					{
						float amount = (1 - m_FuelTankHealth) * RandomFloat(0.02, 0.05);
						Leak(MotorbikeFluid.FUEL, amount);				
					}
				}
			}
			
			if (g_Game.IsServer())
			{
				if (IsVitalFuelTank())
				{
					if (m_FuelTankHealth == GameConstants.DAMAGE_RUINED_VALUE && m_EngineHealth > GameConstants.DAMAGE_RUINED_VALUE)
					{
						SetHealth("Engine", "Health", GameConstants.DAMAGE_RUINED_VALUE);
					}
				}
			}	
		}
	}
	
	protected void OnBrakesPressed()
	{
		UpdateLights();
	}
	
	protected void OnBrakesReleased()
	{
		UpdateLights();
	}
	
	override void OnInput(float dt)
	{
		super.OnInput(dt);
		
#ifdef DIAG_DEVELOPER
		if (m_DebugMode == EMotorbikeDebugMode.NONE)
			return;
		
		float currentRPM = EngineGetRPM();
        if (currentRPM < EngineGetRPMIdle())
        {
            if (currentRPM < 1.0 && EngineIsOn())
            {
                //! Stalled
                EngineStop();
            }
            else if (!EngineIsOn())
            {
                EngineStart();
            }
              
            return;
        }
		
		float speed = GetSpeedometerAbsolute();
		
		float thrustWanted = 0.0;
		float steeringWanted = 0.0;
		
		bool attemptSpeed = false;
		float speedWanted = 0;
		
		switch (m_DebugMode)
		{
		case EMotorbikeDebugMode.FORWARD_10:
			attemptSpeed = true;
			speedWanted = 10;
			break;
		case EMotorbikeDebugMode.FORWARD_50:
			attemptSpeed = true;
			speedWanted = 50;
			break;
		}
		
		if (attemptSpeed)
		{
			// very basic and doesn't actually work
			thrustWanted = Math.Clamp(1.0 - Math.InverseLerp(0, speedWanted, speed), 0, 1);
		}
		
		bool isManual = GearboxGetType() == CarGearboxType.MANUAL;
		bool isReverse = GetGear() == CarGear.REVERSE;
		
		if (isManual)
		{
			float thrustWantedAbs = Math.AbsFloat(thrustWanted);
		
			if (currentRPM > EngineGetRPMRedline() * 0.8)
			{
				if (thrustWantedAbs > 0.1)
				{
					ShiftUp();
				}
			}
			else if (GetGear() > CarGear.FIRST && thrustWantedAbs < 0.1)
			{
				ShiftDown();
			}
			else if (GetGear() < CarGear.FIRST || (thrustWanted > 0.0 && speed < 5.0))
			{
				ShiftTo(CarGear.FIRST);
			}
		}
		
		SetThrottle(thrustWanted);
		SetSteering(steeringWanted);
		SetBrake(0.0);
		SetHandbrake(0.0);
		SetKickstand(0.0);
#endif
	}
	
	override void OnUpdate(float dt)
    {
		Human driver = CrewDriver();
		if (driver && !driver.IsControllingVehicle())
		{
			// likely unconscious
			if (driver.IsAlive())
			{
				SetBrake(0.5);
			}
		}

		if (g_Game.IsServer())
		{
			if (NeedElectricitySourceDevice())
			{
				EntityAI battery = GetElectricitySourceDevice();
				if (battery)
				{
					/*
					m_BatteryTimer += dt;
					if (m_BatteryTimer >= BATTERY_UPDATE_DELAY)
					{
						UpdateBattery(battery);
					}
					*/
				}
			}
		}

		float waterDepth = g_Game.GetWaterDepth(GetEnginePosWS());
		if (waterDepth > 0.0)
		{
			TIntArray crewPresenceIndices = new TIntArray();
			if (IsAnyCrewPresent(crewPresenceIndices))
			{
				if (waterDepth >= WATER_DEPTH_PLAYER_EJECT)
				{
					int crewCount = crewPresenceIndices.Count();
					for (int crewPositionIndex = 0; crewPositionIndex < crewCount; ++crewPositionIndex)
						CrewMember(crewPositionIndex).GetCommand_Vehicle().GetOutVehicle();
				}
			}

			if (g_Game.IsServer())
			{
				m_DrownTime += dt;
				if (m_DrownTime > DROWN_ENGINE_THRESHOLD)
				{
					AddHealth( "Engine", "Health", -DROWN_ENGINE_DAMAGE * dt);
					SetEngineZoneReceivedHit(true);
				}
			}
		}
		else
		{
			if (g_Game.IsServer())
				m_DrownTime = 0;
			
		}

		// For visualisation of brake lights for all players
		float brakeCoef = GetBrake();
		if (brakeCoef > 0)
		{
			if (!m_BrakesArePressed)
			{
				m_BrakesArePressed = true;
				SetSynchDirty();
				OnBrakesPressed();
			}
		}
		else
		{
			if (m_BrakesArePressed)
			{
				m_BrakesArePressed = false;
				SetSynchDirty();
				OnBrakesReleased();
			}
		}
    }
	
	//! Gets called everytime the engine starts.
	override void OnEngineStart()
	{
		if (NeedElectricitySourceDevice())
		{
			EntityAI battery = GetElectricitySourceDevice();
			if (g_Game.IsServer() && battery)
			{
				//! TODO@RR - battery
				/*
				float batteryConsume = GetBatteryConsumption();
				battery.GetCompEM().ConsumeEnergy(batteryConsume);
	
				UpdateBattery(battery);
				*/
			}
		}

		UpdateHeadlightState();		
		UpdateLights();
		
		HandleEngineSound(MotorbikeEngineSoundState.START_OK);
	}

	//! Gets called everytime the engine stops.
	override void OnEngineStop()
	{
		if (NeedElectricitySourceDevice())
		{
			EntityAI battery = GetElectricitySourceDevice();
			if (g_Game.IsServer() && battery)
				UpdateBattery(battery);
		}

		UpdateHeadlightState();
		UpdateLights();
		
		MotorbikeEngineSoundState stopSoundState = MotorbikeEngineSoundState.STOP_OK;
		if (GetFluidFraction(MotorbikeFluid.FUEL) <= 0)
			stopSoundState = MotorbikeEngineSoundState.STOP_NO_FUEL;

		HandleEngineSound(stopSoundState);
		
		SetEngineZoneReceivedHit(false);
	}
	
	// Updates state of attached headlight bulbs for faster access
	protected void UpdateHeadlightState()
	{
		if (!NeedElectricitySourceDevice() && !EngineIsOn())
		{
			m_HeadlightsState = MotorbikeHeadlightState.NONE;
			return;
		}

		EntityAI bulb = FindAttachmentBySlotName("Reflector_1_1");
		
		if (bulb && !bulb.IsRuined())
			m_HeadlightsState = MotorbikeHeadlightState.MAIN;
		else if ((!bulb || bulb.IsRuined()))
			m_HeadlightsState = MotorbikeHeadlightState.NONE;
	}

	override bool OnBeforeLightOn()
	{
		/*
		ItemBase neededItem = GetBattery();

		if (neededItem && !neededItem.IsRuined())
		{
			return neededItem.GetCompEM() && neededItem.GetCompEM().GetEnergy() > 0.0;
		}

		return false;
		*/
		return true;
	}
	
	override void UpdateLights(int new_gear = -1)
	{
		#ifndef SERVER
		UpdateLightsClient(new_gear);
		#endif
		UpdateLightsServer(new_gear);
	}
	
	protected void UpdateLightsClient(int newGear = -1)
	{
		int gear;
		
		if (newGear == -1)
			gear = GetGear();
		else
			gear = newGear;
		
		if (LightIsOn())
		{
			if (!m_LightsComponent.LightExist("Front") && m_HeadlightsState != MotorbikeHeadlightState.NONE)
				m_LightsComponent.EnableLight("Front");
			
			if (m_LightsComponent.LightExist("Front"))
			{
				switch (m_HeadlightsState)
				{
				case MotorbikeHeadlightState.MAIN:
					int driveWheelIndex = GetBoneIndex("drivewheel");
					//m_LightsComponent.AttachLightOnPoint("Front", m_HeadLightPoint, m_HeadLightTargetPoint, VehicleLightMode.SEGREGATED);
					m_LightsComponent.AttachLightOnObject("Front", GetMemoryPointPos(m_HeadLightPoint), vector.Zero, VehicleLightMode.SEGREGATED, driveWheelIndex);
					break;

				default:
					m_LightsComponent.DisableLight("Front");
				}
			}
		}
		else
		{
			if (m_LightsComponent.LightExist("Front"))
				m_LightsComponent.DisableLight("Front");
		}
			
		// brakes & reverse
		switch (gear)
		{
			if (m_BrakesArePressed)
				m_RearLightType = MotorbikeRearLightType.BRAKES_ONLY;
			else
				m_RearLightType = MotorbikeRearLightType.NONE;
		}

		//Debug.Log(string.Format("m_BrakesArePressed=%1, m_RearLightType=%2", m_BrakesArePressed.ToString(), EnumTools.EnumToString(MotorbikeRearLightType, m_RearLightType)));

		if (!m_LightsComponent.LightExist("Rear") && m_RearLightType != MotorbikeRearLightType.NONE && m_HeadlightsState != MotorbikeHeadlightState.NONE)
		{
			if (EngineIsOn() || m_RearLightType == MotorbikeRearLightType.BRAKES_ONLY)
			{
				m_LightsComponent.EnableLight("Rear");
				if (MemoryPointExists(m_RearLightPoint))
					m_LightsComponent.AttachLightOnObject("Rear", GetMemoryPointPos(m_RearLightPoint), "180 0 0", VehicleLightMode.SEGREGATED);
				#ifdef DIAG_DEVELOPER
				else
					ErrorEx(string.Format("Missing '%1' memory point, check memory LOD", m_RearLightPoint), ErrorExSeverity.INFO);
				#endif
			}
		}

		// rear lights
		if (m_LightsComponent.LightExist("Rear") && m_RearLightType != MotorbikeRearLightType.NONE && m_HeadlightsState != MotorbikeHeadlightState.NONE)
		{
			switch (m_RearLightType)
			{
				case MotorbikeRearLightType.BRAKES_ONLY:
					m_LightsComponent.SetLightTypeAndMode("Rear", VehicleLightType.BRAKE, VehicleLightMode.SEGREGATED);
					break;

				default:
					m_LightsComponent.DisableLight("Rear");
			}
		}
		else
		{
			if (m_LightsComponent.LightExist("Rear"))
				m_LightsComponent.DisableLight("Rear");
		}
	}
	
	protected void UpdateLightsServer(int newGear = -1)
	{
		int gear;
		
		if (newGear == -1)
		{
			gear = GetGear();
			if (GearboxGetType() == CarGearboxType.AUTOMATIC)
				gear = GearboxGetMode();
		}
		else
		{
			gear = newGear;
		}
		
		if (LightIsOn())
		{
			m_LightsComponent.SetSelectionMaterial("DashboardOn");
			m_LightsComponent.SetSelectionMaterial("TailOn");
			
			switch (m_HeadlightsState)
			{
				case MotorbikeHeadlightState.MAIN:
					m_LightsComponent.SetSelectionMaterial("FrontOn");
	
					break;
	
				default:
					m_LightsComponent.SetSelectionMaterial("FrontOff");
			}
			
			//Debug.Log(string.Format("LightIsOn=%1, m_HeadlightsState=%2", LightIsOn().ToString(), EnumTools.EnumToString(CarHeadlightBulbsState, m_HeadlightsState)));
		}
		else
		{
			m_LightsComponent.SetSelectionMaterial("TailOff");
			m_LightsComponent.SetSelectionMaterial("DashboardOff");
			m_LightsComponent.SetSelectionMaterial("FrontOff");
		}
			

		// brakes & reverse
		switch (gear)
		{
			if (m_BrakesArePressed)
				m_RearLightType = MotorbikeRearLightType.BRAKES_ONLY;
			else
				m_RearLightType = MotorbikeRearLightType.NONE;
		}

		//Debug.Log(string.Format("m_BrakesArePressed=%1, m_RearLightType=%2", m_BrakesArePressed.ToString(), EnumTools.EnumToString(MotorbikeRearLightType, m_RearLightType)));
			
		
		// rear lights
		if (m_RearLightType != MotorbikeRearLightType.NONE && m_HeadlightsState != CarHeadlightBulbsState.NONE)
		{
			switch (m_RearLightType)
			{
				case MotorbikeRearLightType.BRAKES_ONLY:
					m_LightsComponent.SetSelectionMaterial("BrakeOn");

					break;

				default:
					if (!HasGangedTailAndBrakeLights())
						m_LightsComponent.SetSelectionMaterial("BrakeOff");
			}
		}
		else
		{
			if (!HasGangedTailAndBrakeLights())
				m_LightsComponent.SetSelectionMaterial("BrakeOff");
		}
	}
	
	override protected void PartsHealthCheck()
	{
		int engineHealthLevel = GetHealthLevel("Engine");
		m_EngineHealth = GetHealthLevelValue(engineHealthLevel, "Engine");

		int fuelTankHealthLevel = GetHealthLevel("FuelTank");
		m_FuelTankHealth = GetHealthLevelValue(fuelTankHealthLevel, "FuelTank");
	}
	
	protected void CheckContactCache()
	{
		int contactZonesCount = m_ContactCache.Count();
		if (contactZonesCount == 0)
			return;
		
		for (int i = 0; i < contactZonesCount; ++i)
		{
			string zoneName = m_ContactCache.GetKey(i);
			array<ref VehicleContactData> data = m_ContactCache[zoneName];

			float dmg = Math.AbsInt(data[0].m_Impulse * m_VehicleContactDamageCoef);
			float crewDmgBase = Math.AbsInt((data[0].m_Impulse / dBodyGetMass(this)) * 500 * m_CrewContactDamageCoef);// calculates damage as if the object's weight was 500kg instead of its actual weight

			#ifdef DIAG_DEVELOPER
			CrashDebugData.m_CrashDataPoint = new CrashDebugData();
			CrashDebugData.m_CrashDataPoint.m_VehicleType = GetDisplayName();
			CrashDebugData.m_CrashDataPoint.m_Damage = dmg;
			CrashDebugData.m_CrashDataPoint.m_Zone = zoneName;
			CrashDebugData.m_CrashDataPoint.m_MomentumCurr = GetMomentum();
			CrashDebugData.m_CrashDataPoint.m_MomentumPrev = m_MomentumPrevTick;
			CrashDebugData.m_CrashDataPoint.m_MomentumDelta = data[0].m_Impulse;
			CrashDebugData.m_CrashDataPoint.m_SpeedWorld = GetVelocity(this).Length() * 3.6;
			CrashDebugData.m_CrashDataPoint.m_SpeedWorldPrev = m_VelocityPrevTick.Length() * 3.6;
			CrashDebugData.m_CrashDataPoint.m_SpeedWorldDelta = (m_VelocityPrevTick.Length() - GetVelocity(this).Length()) * 3.6;
			CrashDebugData.m_CrashDataPoint.m_VelocityCur = GetVelocity(this);
			CrashDebugData.m_CrashDataPoint.m_VelocityPrev = m_VelocityPrevTick;
			CrashDebugData.m_CrashDataPoint.m_VelocityDot = vector.Dot(m_VelocityPrevTick.Normalized(), GetVelocity(this).Normalized());
			CrashDebugData.m_CrashDataPoint.m_Time = g_Game.GetTime();
			
			if (DEBUG_OUTPUT_TYPE & EVehicleDebugOutputType.DAMAGE_CONSIDERED)
			{
				Debug.Log("--------------------------------------------------");
				Debug.Log("Vehicle:" + GetDisplayName());
				Debug.Log("DMG: " + dmg);
				Debug.Log("zoneName : "+ zoneName);
				Debug.Log("momentumCurr : "+ GetMomentum());
				Debug.Log("momentumPrev : "+ m_MomentumPrevTick);
				Debug.Log("momentumDelta : "+ data[0].m_Impulse);
				Debug.Log("speed(km/h): "+ GetVelocity(this).Length() * 3.6);
				Debug.Log("speedPrev(km/h): "+ m_VelocityPrevTick.Length() * 3.6);
				Debug.Log("speedDelta(km/h) : "+ (m_VelocityPrevTick.Length() - GetVelocity(this).Length()) * 3.6);
				Debug.Log("velocityCur.): "+ GetVelocity(this));
				Debug.Log("velocityPrev.): "+ m_VelocityPrevTick);
				Debug.Log("velocityDot): "+ vector.Dot(m_VelocityPrevTick.Normalized(), GetVelocity(this).Normalized()));
				Debug.Log("g_Game.GetTime(): "+ g_Game.GetTime());
				Debug.Log("--------------------------------------------------");
			}
			#endif
			if ( dmg < GameConstants.MOTORBIKES_CONTACT_DMG_MIN )
				continue;

			int pddfFlags;
			#ifdef DIAG_DEVELOPER
			CrashDebugData.m_CrashData.Insert(CrashDebugData.m_CrashDataPoint);
			CrashDebugData.m_CrashDataPoint.m_Speedometer =  GetSpeedometer();
			//Print("Crash data recorded");
			#endif
			if (dmg < GameConstants.MOTORBIKES_CONTACT_DMG_THRESHOLD_SMALL)
			{			
				#ifdef DIAG_DEVELOPER
				CrashDebugData.m_CrashDataPoint.m_DamageType = "Small";
				if (DEBUG_OUTPUT_TYPE & EVehicleDebugOutputType.DAMAGE_APPLIED)
					Debug.Log(string.Format("[Vehiles:Damage]:: DMG %1 to the %2 zone is SMALL (threshold: %3), SPEEDOMETER: %4, TIME: %5", dmg, zoneName, GameConstants.MOTORBIKES_CONTACT_DMG_MIN, GetSpeedometer(), g_Game.GetTime() ));
				#endif

				SynchCrashLightSound(true);
				pddfFlags = ProcessDirectDamageFlags.NO_TRANSFER;
			}
			else if (dmg < GameConstants.MOTORBIKES_CONTACT_DMG_THRESHOLD_MID)
			{
				#ifdef DIAG_DEVELOPER
				CrashDebugData.m_CrashDataPoint.m_DamageType = "Medium";
				if (DEBUG_OUTPUT_TYPE & EVehicleDebugOutputType.DAMAGE_APPLIED)
					Debug.Log(string.Format("[Vehiles:Damage]:: DMG %1 to the %2 zone is SMALL (threshold: %3), SPEEDOMETER: %4, TIME: %5", dmg, zoneName, GameConstants.MOTORBIKES_CONTACT_DMG_MIN, GetSpeedometer(), g_Game.GetTime() ));
				#endif

				FallOver();
				SynchCrashLightSound(true);
			}
			else
			{
				#ifdef DIAG_DEVELOPER
				CrashDebugData.m_CrashDataPoint.m_DamageType = "Big";
				if (DEBUG_OUTPUT_TYPE & EVehicleDebugOutputType.DAMAGE_APPLIED)
					Debug.Log(string.Format("[Vehiles:Damage]:: DMG %1 to the %2 zone is BIG (threshold: %3), SPEED: %4, TIME: %5", dmg, zoneName, GameConstants.MOTORBIKES_CONTACT_DMG_MIN, GetSpeedometer(), g_Game.GetTime() ));
				#endif

				DamageCrew(crewDmgBase);
				FallOver();
				SynchCrashHeavySound(true);
				pddfFlags = 0;
			}
			
			#ifdef DEVELOPER
			m_DebugContactDamageMessage += string.Format("%1: %2\n", zoneName, dmg);
			#endif
			
			ProcessDirectDamage(DamageType.CUSTOM, null, zoneName, "EnviroDmg", "0 0 0", dmg, pddfFlags);
		}
		
		UpdateHeadlightState();
		UpdateLights();
		
		m_ContactCache.Clear();
	}
	
	//! Responsible for damaging crew in a car crash
	protected void DamageCrew(float dmg)
	{
		int crewSize = CrewSize();
		for (int c = 0; c < crewSize; ++c)
		{
			Human crew = CrewMember(c);
			if (!crew)
				continue;

			if (dmg > GameConstants.MOTORBIKES_CONTACT_DMG_KILLCREW)
			{		
				#ifdef DIAG_DEVELOPER
				CrashDebugData.m_CrashDataPoint.m_CrewDamageBase = dmg;
				CrashDebugData.m_CrashDataPoint.m_DMGHealth = -100;
				if (DEBUG_OUTPUT_TYPE & EVehicleDebugOutputType.DAMAGE_APPLIED)
				{
					Debug.Log("--------------------------------------------------");
					Debug.Log("Killing the player");
					Debug.Log("Crew DMG Base: " + dmg);
					Debug.Log("--------------------------------------------------");

				}
				#endif
				crew.SetHealth(0.0);
			}
			else
			{
				float shockTemp = Math.InverseLerp(GameConstants.MOTORBIKES_CONTACT_DMG_THRESHOLD_SMALL, GameConstants.MOTORBIKES_CONTACT_DMG_KILLCREW, dmg);
				shockTemp = Math.Clamp(shockTemp,0,1);
				float shock = Math.Lerp(50, 150, shockTemp);
				float hp = Math.Lerp(2, 100, shockTemp);

				#ifdef DIAG_DEVELOPER
				CrashDebugData.m_CrashDataPoint.m_CrewDamageBase = dmg;
				CrashDebugData.m_CrashDataPoint.m_ShockTemp = shockTemp;
				CrashDebugData.m_CrashDataPoint.m_DMGHealth = hp;
				CrashDebugData.m_CrashDataPoint.m_DMGShock = shock;
				if (DEBUG_OUTPUT_TYPE & EVehicleDebugOutputType.DAMAGE_APPLIED)
				{
					Debug.Log("--------------------------------------------------");
					Debug.Log("Crew DMG Base: " + dmg);
					Debug.Log("Crew shockTemp: " + shockTemp);
					Debug.Log("Crew DMG Health: " + hp);
					Debug.Log("Crew DMG Shock: " + shock);
					Debug.Log("--------------------------------------------------");
				}
				#endif
				
				crew.AddHealth("", "Shock", -shock);
				crew.AddHealth("", "Health", -hp);
			}
		}
	}
	
	override void CleanupEffects()
	{
		super.CleanupEffects();

		if (m_LightsComponent)
		{
			m_LightsComponent.DestroyLight("Front");
			m_LightsComponent.DestroyLight("Rear");
		}
	}
	
	protected void SpawnUniversalParts()
	{
		GetInventory().CreateInInventory("HeadlightH7");

		if (NeedElectricitySourceDevice())
			GetInventory().CreateInInventory(GetElectricitySourceDeviceType());

		if (IsVitalSparkPlug())
		{
			GetInventory().CreateInInventory("SparkPlug");
			GetInventory().CreateInInventory("SparkPlug");
		}
	}

	protected override event typename GetOwnerStateType()
	{
		return MotorbikeScriptOwnerState;
	}

	protected override event typename GetMoveType()
	{
		return MotorbikeScriptMove;
	}
	
	protected void SetEngineStarted(bool started)
	{
		m_EngineStarted = started;
	}
	
	// Only used for sound states which happen before engine start
	protected void SetEngineSoundState(MotorbikeEngineSoundState pState)
	{
		m_EngineSoundState = pState;
		SetSynchDirty();
	}
	
	protected void HandleEngineSound(MotorbikeEngineSoundState state)
	{
		SetEngineSoundState(state);
		m_EngineLastSoundState 	= state;

		#ifndef SERVER
		#ifndef NO_GUI
		PlayerBase player = null;
		EffectSound sound = null;
		WaveKind waveKind = WaveKind.WAVEEFFECTEX;
		
		bool doInside = false;

		m_EngineLastSoundState = state;
		
		switch (state)
		{
			case MotorbikeEngineSoundState.STARTING:
				sound = new EffectSound();
				sound.SetSoundSet(m_EngineStarting);
				sound.SetSoundFadeOut(0.15);

				m_PreStartSound = sound;
				break;
			case MotorbikeEngineSoundState.START_OK:
				doInside = true;
		
				sound = new EffectSound();
				sound.SetSoundSet(m_EngineStartOK);
				sound.SetAutodestroy(true);
				
				//! postpone the engine sound played from c++ on soundcontroller (via OnSound override)
				g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SetEngineStarted, 1000, false, true);
				break;
				
			case MotorbikeEngineSoundState.START_NO_FUEL:
				sound = new EffectSound();
				sound.SetSoundSet(m_EngineStartFuel);
				sound.SetAutodestroy(true);
				break;

			case MotorbikeEngineSoundState.START_NO_SPARKPLUG:
				sound = new EffectSound();
				sound.SetSoundSet(m_EngineStartPlug);
				sound.SetAutodestroy(true);
				break;
				
			case MotorbikeEngineSoundState.STOP_OK:
				doInside = true;
					
				sound = new EffectSound();
				sound.SetSoundSet(m_EngineStop);
				sound.SetAutodestroy(true);
				break;
			case MotorbikeEngineSoundState.STOP_NO_FUEL:
				doInside = true;
					
				sound = new EffectSound();
				sound.SetSoundSet(m_EngineStopFuel);
				sound.SetAutodestroy(true);
				break;
			
			default: 
				break;
		}
		
		// play different sound based on selected camera
		if (doInside && Class.CastTo(player, g_Game.GetPlayer()))
		{
			if (player.IsCameraInsideVehicle())
			{
				waveKind = WaveKind.WAVEEFFECT;
			}
		}

		if (sound)
		{
			vector enginePos = GetEnginePos();
			
			sound.SetParent(this);
			sound.SetPosition(ModelToWorld(enginePos));
			sound.SetLocalPosition(enginePos);
			sound.SetSoundWaveKind(waveKind);
			SEffectManager.EffectRegister(sound);
			
			sound.SoundPlay();
		}

		#endif
		#endif
	}
	
	/*!
		Gets called everytime the game wants to start the engine.

		\return true if the engine can start, false otherwise.
	*/
	override bool OnBeforeEngineStart()
	{
		SetEngineSoundState(MotorbikeEngineSoundState.NONE);

		EMotorbikeOperationalState state = CheckOperationalRequirements();
		return state == EMotorbikeOperationalState.OK;
	}
	
	// Whether the car engine can be started
	int CheckOperationalRequirements()
	{
		int state = EMotorbikeOperationalState.OK;

		EntityAI item = null;

		if (IsDamageDestroyed() || GetHealthLevel("Engine") >= GameConstants.STATE_RUINED)
		{
			state |= EMotorbikeOperationalState.RUINED;
		}
		
		if (GetFluidFraction(MotorbikeFluid.FUEL) <= 0)
		{
			state |= EMotorbikeOperationalState.NO_FUEL;
		}

		if (IsVitalSparkPlug())
		{
			item = FindAttachmentBySlotName("SparkPlug");
			if (!item || (item && item.IsRuined()))
				state |= EMotorbikeOperationalState.NO_IGNITER;
		}
		
		return state;
	}

	bool CheckOperationalState()
	{
		return CheckOperationalRequirements() == EMotorbikeOperationalState.OK;
	}

	void OnIgnition()
	{
		EMotorbikeOperationalState state = CheckOperationalRequirements();

		if (state == EMotorbikeOperationalState.RUINED)
		{
			return;
		}

		if (state & EMotorbikeOperationalState.NO_IGNITER)
		{
			HandleEngineSound(MotorbikeEngineSoundState.START_NO_SPARKPLUG);
			return;
		}

		if (state & EMotorbikeOperationalState.NO_FUEL)
		{
			HandleEngineSound(MotorbikeEngineSoundState.START_NO_FUEL);
			return;
		}

		HandleEngineSound(MotorbikeEngineSoundState.STARTING);
	}
	
	private bool IsServerOrOwner()
	{
		return g_Game.IsServer() || IsOwner();
	}
	
	protected void HandleHeatComfortOverrides(string slotName, notnull EntityAI item, bool detach = false);

	#ifdef DIAG_DEVELOPER
	override void GetDebugActions(out TSelectableActionInfoArrayEx outputList)
	{
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.VEHICLE_HORN_START_LONG, "Horn Start Short", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.VEHICLE_HORN_START_SHORT, "Horn Start Long", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.VEHICLE_HORN_STOP, "Horn Stop", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));

		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "Fuel", FadeColors.RED));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.VEHICLE_FUEL_FULL, "Full", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.VEHICLE_FUEL_EMPTY, "Empty", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.VEHICLE_FUEL_INCREASE, "10% increase", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.VEHICLE_FUEL_DECREASE, "10% decrease", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));

		typename e = EMotorbikeDebugMode;
		
		int i;
			
		int cnt = e.GetVariableCount();
		int val;
	
		for (i = 0; i < cnt; i++)
		{
			if (!e.GetVariableValue(null, i, val))
				continue;
			
			val = val + EActions.PLAYER_BOT_INTERNAL_START;
				
			string name = e.GetVariableName(i);
			
			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, val, name, FadeColors.LIGHT_GREY));
		}

		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
						
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
			case EActions.VEHICLE_HORN_START_SHORT:
				m_HornComponent.SetMode(VehicleHornMode.SHORT);
				return true;
			case EActions.VEHICLE_HORN_START_LONG:
				m_HornComponent.SetMode(VehicleHornMode.LONG);
				return true;
			case EActions.VEHICLE_HORN_STOP:
				m_HornComponent.SetMode(VehicleHornMode.OFF);
				return true;

			case EActions.VEHICLE_FUEL_FULL:
				Fill(MotorbikeFluid.FUEL, GetFluidCapacity(MotorbikeFluid.FUEL));
				return true;
			case EActions.VEHICLE_FUEL_EMPTY:
				LeakAll(MotorbikeFluid.FUEL);
				return true;
			case EActions.VEHICLE_FUEL_INCREASE:
				Fill(MotorbikeFluid.FUEL, GetFluidCapacity(MotorbikeFluid.FUEL) * 0.1);
				return true;
			case EActions.VEHICLE_FUEL_DECREASE:
				Leak(MotorbikeFluid.FUEL, GetFluidCapacity(MotorbikeFluid.FUEL) * 0.1);
				return true;
		}

		typename e = EMotorbikeDebugMode;
		
		int i;
			
		int cnt = e.GetVariableCount();
		int val;
	
		for (i = 0; i < cnt; i++)
		{
			if (!e.GetVariableValue(null, i, val))
				continue;
			
			if ((val + EActions.PLAYER_BOT_INTERNAL_START) == action_id)
			{
				m_DebugMode = val;
				dBodyActive(this, ActiveState.ACTIVE);
			}
		}
	
		return false;
	}

	override protected string GetDebugText()
	{
		string debug_output = super.GetDebugText();
		if (g_Game.IsServer())
		{
			debug_output += m_DebugContactDamageMessage + "\n";
		}
		debug_output += "Entity momentum: " + GetMomentum();
		int capacity = GetFluidCapacity(MotorbikeFluid.FUEL);
		debug_output += string.Format("\nFuel:%1/%2", capacity * GetFluidFraction(MotorbikeFluid.FUEL), capacity);
		debug_output += "\nIsEngineON: " + EngineIsOn();
		debug_output += "\nHeatComfort override: " + GetHeatComfortOverride();

		return debug_output;	
	}

	override void FixEntity()
	{
		super.FixEntity();

		if (g_Game.IsServer())
		{
			Fill(MotorbikeFluid.FUEL, GetFluidCapacity(MotorbikeFluid.FUEL));
			
			for (int i = 5; i > 0; i--)//there is a problem with wheels when performed only once, this solves it
				super.FixEntity();
			if (!g_Game.IsMultiplayer())
			{
				//single
				m_VFXComponent.StopEffect(EVehicleVFXEffect.ENGINE_SMOKE);
			}
		}
		else
		{
			//MP client
			m_VFXComponent.StopEffect(EVehicleVFXEffect.ENGINE_SMOKE);
		}
	}
	#endif
}
