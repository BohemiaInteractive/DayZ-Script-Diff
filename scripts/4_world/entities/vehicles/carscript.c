enum CarDoorState
{
	DOORS_MISSING,
	DOORS_OPEN,
	DOORS_CLOSED
}

enum CarHeadlightBulbsState
{
	NONE,
	LEFT,
	RIGHT,
	BOTH
}

enum CarRearLightType
{
	NONE,
	BRAKES_ONLY,
	REVERSE_ONLY,
	BRAKES_AND_REVERSE
}

enum ECarOperationalState
{
	OK = 0,
	RUINED = 1,
	NO_FUEL = 2,
	NO_IGNITER = 4,
	NO_BATTERY = 8,
}

enum CarEngineSoundState
{
	NONE,
	STARTING,
	START_OK,
	START_NO_FUEL,
	START_NO_BATTERY,
	START_NO_SPARKPLUG,
	STOP_OK,
	STOP_NO_FUEL
}

enum ECarHornState
{
	OFF 	= 0,
	SHORT	= 1,
	LONG	= 2
}

//! for backward compatibility
class CarContactData : VehicleContactData {}
typedef VehicleContactCache CarContactCache;


class CarScriptOwnerState : CarOwnerState
{
	float m_fTime;
	
	protected override event void Write(PawnStateWriter ctx)
	{
		ctx.Write(m_fTime);
	}
	
	protected override event void Read(PawnStateReader ctx)
	{
		ctx.Read(m_fTime);
	}
};

class CarScriptMove : CarMove
{
};

#ifdef DIAG_DEVELOPER
enum ECarDebugMode
{
	NONE,
	FORWARD_10,
	FORWARD_50,
	RANDOM_DRIVING,
	PREDEFINED_PATH,
};
#endif

#ifdef DIAG_DEVELOPER 
CarScript _car;
#endif

/*!
	Base script class for cars
*/
class CarScript extends Car
{
	static ref map<typename, ref TInputActionMap> m_CarTypeActionsMap = new map<typename, ref TInputActionMap>;
	TInputActionMap m_InputActionMap;
	bool	m_ActionsInitialize;

	protected float m_Time;

	static float DROWN_ENGINE_THRESHOLD = 0.5;
	static float DROWN_ENGINE_DAMAGE = 350.0;
	
	static const string MEMORY_POINT_NAME_CAR_HORN 	= "pos_carHorn";
	
	//! keeps ammount of each fluid
	protected float m_FuelAmmount;
	protected float m_CoolantAmmount;
	protected float m_OilAmmount;
	protected float m_BrakeAmmount;

	//!
	//protected float m_dmgContactCoef = 0.023;
	protected float m_EnviroHeatComfortOverride;

	//!
	protected float m_DrownTime;
	static vector m_DrownEnginePos;
	
	//!
	protected float m_EngineHealth;
	protected float m_RadiatorHealth;
	protected float m_FuelTankHealth;
	protected float m_BatteryHealth;
	protected float m_PlugHealth;
	
	protected EntityAI m_Radiator;
	
	protected float	m_BatteryConsume = 15; 				//Battery energy consumption upon engine start
	protected float m_BatteryContinuousConsume = 0.25; 	//Battery consumption with lights on and engine is off
	protected float m_BatteryRecharge = 0.15; 			//Battery recharge rate when engine is on
	private	  float m_BatteryTimer = 0; 				//Used to factor energy consumption / recharging
	private const float BATTERY_UPDATE_DELAY = 100;

	//! Particles
	protected vector m_exhaustPtcDir;

	protected vector m_enginePos;
	protected vector m_frontPos;
	protected vector m_backPos;
	protected vector m_side_1_1Pos;
	protected vector m_side_1_2Pos;
	protected vector m_side_2_1Pos;
	protected vector m_side_2_2Pos;	
	
	//!Sounds
	string m_EngineStartOK 				= "";
	string m_EngineStartBattery 		= "";
	string m_EngineStartPlug 			= "";
	string m_EngineStartFuel 			= "";
	string m_EngineStop	 				= "";
	string m_EngineStopFuel 			= "";
	
	string m_CarDoorOpenSound 			= "";
	string m_CarDoorCloseSound 			= "";
	string m_CarSeatShiftInSound 		= "";
	string m_CarSeatShiftOutSound 		= "";

	ref EffectSound m_WindowSmall;
	ref EffectSound m_WindowLarge;
	private ref EffectSound m_PreStartSound;
	
	// Memory points
	static string m_ReverseLightPoint				= "light_reverse";
	static string m_LeftHeadlightPoint				= "light_left";
	static string m_RightHeadlightPoint				= "light_right";
	static string m_LeftHeadlightTargetPoint		= "light_left_dir";
	static string m_RightHeadlightTargetPoint		= "light_right_dir";
	static string m_DrownEnginePoint 				= "drown_engine";
	
	// Model selection IDs for texture/material changes
	// If each car needs different IDs, then feel free to remove the 'static' flag and overwrite these numbers down the hierarchy
	static const int SELECTION_ID_FRONT_LIGHT_L 	= 0;
	static const int SELECTION_ID_FRONT_LIGHT_R 	= 1;
	static const int SELECTION_ID_BRAKE_LIGHT_L 	= 2;
	static const int SELECTION_ID_BRAKE_LIGHT_R 	= 3;
	static const int SELECTION_ID_REVERSE_LIGHT_L 	= 4;
	static const int SELECTION_ID_REVERSE_LIGHT_R 	= 5;
	static const int SELECTION_ID_TAIL_LIGHT_L 		= 6;
	static const int SELECTION_ID_TAIL_LIGHT_R 		= 7;
	static const int SELECTION_ID_DASHBOARD_LIGHT 	= 8;

	protected int m_CarEngineSoundState;
	protected int m_CarEngineLastSoundState;
	
	protected ref VehicleLightsComponent m_LightsComponent;
	protected ref VehicleHornComponent m_HornComponent;
	protected ref VehicleVFXComponent m_VFXComponent;
	
	#ifdef DIAG_DEVELOPER 
	private const int DEBUG_MESSAGE_CLEAN_TIME_SECONDS = 10;
	private float m_DebugMessageCleanTime;
	private string m_DebugContactDamageMessage;
	#endif
	
#ifdef DIAG_DEVELOPER
	ECarDebugMode m_eDebugMode;
#endif
	
	void CarScript()
	{
#ifdef DIAG_DEVELOPER 
		_car = this;
#endif

		SetEventMask(EntityEvent.POSTSIMULATE);
		SetEventMask(EntityEvent.POSTFRAME);
		
		m_Time = 0;
		// sets max health for all components at init
		m_EngineHealth = 1;
		m_FuelTankHealth = 1;
		m_RadiatorHealth = -1;
		m_BatteryHealth = -1;
		m_PlugHealth = -1;

		m_enginePtcFx = -1;
		m_coolantPtcFx = -1;
		m_exhaustPtcFx = -1;

		m_EnviroHeatComfortOverride = 0;

		m_dmgContactCoef = 0.058;
		
		m_CarEngineSoundState = CarEngineSoundState.NONE;

		// TODO(kumarjac): MOVE THIS ALL TO 'CarScriptType' ONCE THAT EXISTS! re:Server Performance
		m_LightsComponent = VehicleLightsComponent(this);
		m_LightsComponent.RegisterSelection("DashboardOn",	{new VehicleLightSelectionData(SELECTION_ID_DASHBOARD_LIGHT, "dashboardMatOn")});
		m_LightsComponent.RegisterSelection("DashboardOff",	{new VehicleLightSelectionData(SELECTION_ID_DASHBOARD_LIGHT, "dashboardMatOff")});
		m_LightsComponent.RegisterSelection("FrontLeftOn",	{new VehicleLightSelectionData(SELECTION_ID_FRONT_LIGHT_L, "frontReflectorMatOn")});
		m_LightsComponent.RegisterSelection("FrontLeftOff",	{new VehicleLightSelectionData(SELECTION_ID_FRONT_LIGHT_L, "frontReflectorMatOff")});
		m_LightsComponent.RegisterSelection("FrontRightOn",	{new VehicleLightSelectionData(SELECTION_ID_FRONT_LIGHT_R, "frontReflectorMatOn")});
		m_LightsComponent.RegisterSelection("FrontRightOff",	{new VehicleLightSelectionData(SELECTION_ID_FRONT_LIGHT_R, "frontReflectorMatOff")});
		
		m_LightsComponent.RegisterSelection("TailOn",	{
			new VehicleLightSelectionData(SELECTION_ID_TAIL_LIGHT_L, "TailReflectorMatOn"),
			new VehicleLightSelectionData(SELECTION_ID_TAIL_LIGHT_R, "TailReflectorMatOn"),
		});
		m_LightsComponent.RegisterSelection("TailOff",	{
			new VehicleLightSelectionData(SELECTION_ID_TAIL_LIGHT_L, "TailReflectorMatOff"),
			new VehicleLightSelectionData(SELECTION_ID_TAIL_LIGHT_R, "TailReflectorMatOff"),
		});
		m_LightsComponent.RegisterSelection("TailLeftOnly",	{
			new VehicleLightSelectionData(SELECTION_ID_TAIL_LIGHT_L, "TailReflectorMatOn"),
			new VehicleLightSelectionData(SELECTION_ID_TAIL_LIGHT_R, "TailReflectorMatOff"),
		});
		m_LightsComponent.RegisterSelection("TailRightOnly",	{
			new VehicleLightSelectionData(SELECTION_ID_TAIL_LIGHT_L, "TailReflectorMatOff"),
			new VehicleLightSelectionData(SELECTION_ID_TAIL_LIGHT_R, "TailReflectorMatOn"),
		});
		m_LightsComponent.RegisterSelection("BrakesOn",	{
			new VehicleLightSelectionData(SELECTION_ID_BRAKE_LIGHT_L, "brakeReflectorMatOn"),
			new VehicleLightSelectionData(SELECTION_ID_BRAKE_LIGHT_R, "brakeReflectorMatOn"),
		});
		m_LightsComponent.RegisterSelection("BrakesOff",	{
			new VehicleLightSelectionData(SELECTION_ID_BRAKE_LIGHT_L, "brakeReflectorMatOff"),
			new VehicleLightSelectionData(SELECTION_ID_BRAKE_LIGHT_R, "brakeReflectorMatOff"),
		});
		m_LightsComponent.RegisterSelection("ReverseOn",	{
			new VehicleLightSelectionData(SELECTION_ID_REVERSE_LIGHT_L, "ReverseReflectorMatOn"),
			new VehicleLightSelectionData(SELECTION_ID_REVERSE_LIGHT_R, "ReverseReflectorMatOn"),
		});
		m_LightsComponent.RegisterSelection("ReverseOff",	{
			new VehicleLightSelectionData(SELECTION_ID_REVERSE_LIGHT_L, "ReverseReflectorMatOff"),
			new VehicleLightSelectionData(SELECTION_ID_REVERSE_LIGHT_R, "ReverseReflectorMatOff"),
		});

		m_LightsComponent.RegisterLight("Front", new VehicleLightData(new VehicleLightProfileBase()));
		m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new VehicleLightProfileBase()));
		
		m_HornComponent = VehicleHornComponent(this);
		m_HornComponent.SetMode(VehicleHornMode.OFF);
		
		if (MemoryPointExists("ptcCoolantPos"))
			m_coolantPtcPos = GetMemoryPointPos("ptcCoolantPos");
		
		RegisterNetSyncVariableBool("m_BrakesArePressed");
		RegisterNetSyncVariableBoolSignal("m_PlayCrashSoundLight");
		RegisterNetSyncVariableBoolSignal("m_PlayCrashSoundHeavy");

		RegisterNetSyncVariableInt("m_HornComponent.m_Mode", VehicleHornMode.OFF, VehicleHornMode.COUNT);
		
		RegisterNetSyncVariableInt("m_CarEngineSoundState", CarEngineSoundState.NONE, CarEngineSoundState.STOP_NO_FUEL);
		
		InitializeParticles();

		//! obsolete handling (since 1.30) - kept for backward compatibility
		m_CarHornState = ECarHornState.OFF;
		RegisterNetSyncVariableInt("m_CarHornState", ECarHornState.OFF, ECarHornState.LONG);
	}
	
	override string GetVehicleType()
	{
		return "VehicleTypeCar";
	}
	
	override VehicleHornComponent GetVehicleHornComponent()
	{
		return m_HornComponent;
	}
	
	vector GetEnginePosWS()
	{
		return ModelToWorld( m_DrownEnginePos );
	}
	
	vector GetCoolantPtcPosWS()
	{
		return ModelToWorld( m_coolantPtcPos );
	}

	vector GetEnginePointPosWS()
	{	
		return ModelToWorld( m_enginePos );
	}
	vector GetFrontPointPosWS()
	{	
		return ModelToWorld( m_frontPos );
	}
	vector GetBackPointPosWS()
	{	
		return ModelToWorld( m_backPos );
	}
	vector Get_1_1PointPosWS()
	{	
		return ModelToWorld( m_side_1_1Pos );
	}
	vector Get_1_2PointPosWS()
	{	
		return ModelToWorld( m_side_1_2Pos );
	}
	vector Get_2_1PointPosWS()
	{	
		return ModelToWorld( m_side_2_1Pos );
	}
	vector Get_2_2PointPosWS()
	{	
		return ModelToWorld( m_side_2_2Pos );
	}
	
	protected void InitializeParticles()
	{
		if (GetGame().IsHeadlessOrDedicatedServer())
			return;

		m_VFXComponent = VehicleVFXComponent(this);

		// OBSOLETE (not used directly; backward compatibility only)
		m_exhaustPtcPos	= m_VFXComponent.ConvertMemPointToPosition("ptcExhaust_end");
		if (m_exhaustPtcPos != vector.Zero)
		{
			vector exhaustStart = m_VFXComponent.ConvertMemPointToPosition("ptcExhaust_start");
			vector tempOri = vector.Direction(exhaustStart, m_exhaustPtcPos);

			m_exhaustPtcDir[0] = -tempOri[2];
			m_exhaustPtcDir[1] = tempOri[1];
			m_exhaustPtcDir[2] = tempOri[0];
			
			m_exhaustPtcDir = m_exhaustPtcDir.Normalized().VectorToAngles();
		}
		else
			m_exhaustPtcDir = "1 1 1";

		m_enginePtcPos 	= m_VFXComponent.ConvertMemPointToPosition("ptcEnginePos");
		
		m_DrownEnginePos = m_VFXComponent.ConvertMemPointToPosition("drown_engine");
		m_enginePos = m_VFXComponent.ConvertMemPointToPosition("dmgZone_engine");
		m_frontPos = m_VFXComponent.ConvertMemPointToPosition("dmgZone_front");
		m_backPos = m_VFXComponent.ConvertMemPointToPosition("dmgZone_back");
		m_side_1_1Pos = m_VFXComponent.ConvertMemPointToPosition("dmgZone_fender_1_1");
		m_side_1_2Pos = m_VFXComponent.ConvertMemPointToPosition("dmgZone_fender_1_2");
		m_side_2_1Pos = m_VFXComponent.ConvertMemPointToPosition("dmgZone_fender_2_1");
		m_side_2_2Pos = m_VFXComponent.ConvertMemPointToPosition("dmgZone_fender_2_2");

		m_VFXComponent.RegisterEffect(EVehicleVFXEffect.DUST_BEHIND, EffVehicleDust, new VehicleVFXDataParams(m_VFXComponent.ConvertMemPointToPosition("ptcDustPos")));
		m_VFXComponent.RegisterEffect(EVehicleVFXEffect.ENGINE_SMOKE, EffEngineSmoke, new VehicleVFXDataParams(m_VFXComponent.ConvertMemPointToPosition("ptcEnginePos")));
		m_VFXComponent.RegisterEffect(EVehicleVFXEffect.COOLANT_STEAM, EffCoolantSteam, new VehicleVFXDataParams(m_VFXComponent.ConvertMemPointToPosition("ptcCoolantPos")));
		m_VFXComponent.RegisterEffect(EVehicleVFXEffect.EXHAUST_SMOKE, EffExhaustSmoke, new VehicleVFXDataParams(m_exhaustPtcPos, m_exhaustPtcDir));
		m_VFXComponent.RegisterEffectGroup(EVehicleVFXEffectGroup.WHEEL_SMOKE, EffWheelSmoke, WheelCount(), new VehicleVFXDataParams(vector.Zero, "0 1 -1", 5.0));
		m_VFXComponent.RegisterEffectGroup(EVehicleVFXEffectGroup.WHEEL_CONTACT, EffWheelContact, WheelCount(), new VehicleVFXDataParams(vector.Zero, vector.Up, 5.0));
	}
	
	override float GetLiquidThroughputCoef()
	{
		return LIQUID_THROUGHPUT_CAR_DEFAULT;
	}
	
	override void CleanupEffects()
	{
		super.CleanupEffects();
	
		if (m_VFXComponent)
			m_VFXComponent.StopAllEffects();
			
		if (m_LightsComponent)
		{
			m_LightsComponent.DestroyLight("Front");
			m_LightsComponent.DestroyLight("Rear");
		}

		//! backward compatibility
		if (m_Headlight)
			m_Headlight.Destroy();
		//! backward compatibility
		if (m_RearLight)
			m_RearLight.Destroy();
			
		SEffectManager.DestroyEffect(m_WindowSmall);
		SEffectManager.DestroyEffect(m_WindowLarge);
		CleanupSound(m_CarHornSoundEffect);
	}
	
	void CleanupSound(EffectSound sound)
	{
		SEffectManager.DestroyEffect(sound);
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

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
				
		if (GetCrashHeavySound())
			PlayCrashHeavySound();
		else if (GetCrashLightSound())
			PlayCrashLightSound();
		
		HandleCarHornSound(m_CarHornState);
		
		if (!IsOwner() && m_CarEngineSoundState != m_CarEngineLastSoundState)
			HandleEngineSound(m_CarEngineSoundState);

		UpdateLights();
	}
	
	// --------------------------------------------------------------------------------
	
	override protected void PlayCrashLightSound()
	{
		PlaySoundEx("offroad_hit_light_SoundSet", m_CrashSoundLight, m_PlayCrashSoundLight);
	}

	// --------------------------------------------------------------------------------

	override protected void PlayCrashHeavySound()
	{
		PlaySoundEx("offroad_hit_heavy_SoundSet", m_CrashSoundHeavy, m_PlayCrashSoundHeavy);
	}
	
	// --------------------------------------------------------------------------------
	
	bool HasGangedTailAndBrakeLights()
	{
		return false;
	}
	
	void CreateCarDestroyedEffect()
	{
		if (g_Game.GetWaterDepth(GetEnginePosWS()) <= 0)
		{
			m_VFXComponent.PlayEffect(EVehicleVFXEffect.ENGINE_SMOKE);
			m_VFXComponent.SetEffectParticleState(EVehicleVFXEffect.ENGINE_SMOKE, ParticleList.HATCHBACK_COOLANT_OVERHEATED);
		}
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);

		switch (slot_name)
		{
			case "Reflector_1_1":
				if (g_Game.IsServer())
					SetHealth("Reflector_1_1", "Health", item.GetHealth());
				break;
			case "Reflector_2_1":
				if (g_Game.IsServer())
					SetHealth("Reflector_2_1", "Health", item.GetHealth());
				break;
			case "CarBattery":
			case "TruckBattery":
				if (g_Game.IsServer())
					m_BatteryHealth = item.GetHealth01();
				break;
			case "SparkPlug":
			case "GlowPlug":
				if (g_Game.IsServer())
					m_PlugHealth = item.GetHealth01();
				break;
			case "CarRadiator":
				if (g_Game.IsServer())
					m_RadiatorHealth = item.GetHealth01();
				
				m_Radiator = item;
				break;
		}
		
		if (g_Game.IsServer())
			Synchronize();

		UpdateHeadlightState();
		UpdateLights();
	}
	
	// Updates state of attached headlight bulbs for faster access
	void UpdateHeadlightState()
	{
		EntityAI bulb_L = FindAttachmentBySlotName("Reflector_1_1");
		EntityAI bulb_R = FindAttachmentBySlotName("Reflector_2_1");
		
		if (bulb_L && !bulb_L.IsRuined() && bulb_R && !bulb_R.IsRuined())
		{
			m_HeadlightsState = CarHeadlightBulbsState.BOTH;
		}
		else if (bulb_L && !bulb_L.IsRuined())
		{
			m_HeadlightsState = CarHeadlightBulbsState.LEFT;
		}
		else if (bulb_R && !bulb_R.IsRuined())
		{
			m_HeadlightsState = CarHeadlightBulbsState.RIGHT;
		}
		else if ((!bulb_L || bulb_L.IsRuined()) && (!bulb_R || bulb_R.IsRuined()))
		{
			m_HeadlightsState = CarHeadlightBulbsState.NONE;
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		switch (slot_name)
		{
		case "CarBattery":
		case "TruckBattery":
			m_BatteryHealth = -1;
			if (IsServerOrOwner())
			{
				if (EngineIsOn())
				{
					EngineStop();
				}
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
			{
				EngineStop();
			}
		break;
		case "CarRadiator":
			m_Radiator = null;
			if (IsServerOrOwner())
			{
				LeakAll(CarFluid.COOLANT);
			}

			if (g_Game.IsServer())
			{
				if (m_DamageZoneMap.Contains("Radiator"))
				{
					SetHealth("Radiator", "Health", 0);
				}
			}
		break;
		}
		
		if (g_Game.IsServer())
		{
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
	
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if (!super.CanReceiveAttachment(attachment, slotId))
			return false;
		
		InventoryLocation attachmentInventoryLocation = new InventoryLocation();
		attachment.GetInventory().GetCurrentInventoryLocation(attachmentInventoryLocation);
		if (attachmentInventoryLocation.GetParent() == null)
		{
			return true;
		}
		
		if (attachment && attachment.Type().IsInherited(CarWheel))
		{
			string slotSelectionName;
			InventorySlots.GetSelectionForSlotId(slotId, slotSelectionName);

			switch (slotSelectionName)
			{
				case "wheel_spare_1":
				case "wheel_spare_2":
					return CanManipulateSpareWheel(slotSelectionName);
					break;
			}
		}
		
		return true;
	}

	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (!super.CanReleaseAttachment(attachment))
		{
			return false;
		}

		if (EngineIsOn() && IsMoving())
		{
			return false;
		}

		if (attachment && attachment.Type().IsInherited(CarWheel))
		{
			InventoryLocation attachmentInventoryLocation = new InventoryLocation();
			attachment.GetInventory().GetCurrentInventoryLocation(attachmentInventoryLocation);

			string slotSelectionName;
			InventorySlots.GetSelectionForSlotId(attachmentInventoryLocation.GetSlot(), slotSelectionName);

			switch (slotSelectionName)
			{
				case "wheel_spare_1":
				case "wheel_spare_2":
					return CanManipulateSpareWheel(slotSelectionName);
					break;
			}
		}

		return true;
	}
	
	protected bool CanManipulateSpareWheel(string slotSelectionName)
	{
		return false;
	}
	
	override void EOnPostFrame(IEntity other, int extra)
	{
		float carSpeed = GetVelocity(this).Length();
		ParticleParamsOverrideData particleParamsData;

		//FX only on Client and in Single
		if ( !g_Game.IsHeadlessOrDedicatedServer() )
		{
			if (EngineIsOn())
			{
				if (!m_VFXComponent.IsEffectPlayed(EVehicleVFXEffect.EXHAUST_SMOKE))
				{
					particleParamsData = m_VFXComponent.GetParticleParamsOverrideData(EVehicleVFXEffect.EXHAUST_SMOKE);
					particleParamsData.m_FloatModifiers.InsertAt(carSpeed, 0);
					m_VFXComponent.PlayEffect(EVehicleVFXEffect.EXHAUST_SMOKE);
					m_VFXComponent.SetEffectParticleState(EVehicleVFXEffect.EXHAUST_SMOKE, ParticleList.HATCHBACK_EXHAUST_SMOKE);
				}
			
				if (IsVitalRadiator() && GetFluidFraction(CarFluid.COOLANT) < 0.5)
				{
					if (!m_VFXComponent.IsEffectPlayed(EVehicleVFXEffect.COOLANT_STEAM))
						m_VFXComponent.PlayEffect(EVehicleVFXEffect.COOLANT_STEAM);
					else
					{
						if (GetFluidFraction(CarFluid.COOLANT) > 0)
							m_VFXComponent.SetEffectParticleState(EVehicleVFXEffect.COOLANT_STEAM, ParticleList.HATCHBACK_COOLANT_OVERHEATING);
						else
							m_VFXComponent.SetEffectParticleState(EVehicleVFXEffect.COOLANT_STEAM, ParticleList.HATCHBACK_COOLANT_OVERHEATED);
					}
				}
				else
					m_VFXComponent.StopEffect(EVehicleVFXEffect.COOLANT_STEAM);
			}
			else
			{
				m_VFXComponent.StopEffect(EVehicleVFXEffect.EXHAUST_SMOKE);
				m_VFXComponent.StopEffect(EVehicleVFXEffect.COOLANT_STEAM);
			}

			string surface;
		
			if (carSpeed > EffVehicleDust.WHEEL_DUST_THRESHOLD)
			{
				particleParamsData = m_VFXComponent.GetParticleParamsOverrideData(EVehicleVFXEffect.DUST_BEHIND);
				particleParamsData.m_FloatModifiers.InsertAt(carSpeed, 0);
				particleParamsData.m_FloatModifiers.InsertAt(GetWeightCoef(), 1);
				m_VFXComponent.PlayEffect(EVehicleVFXEffect.DUST_BEHIND);
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
							particleParamsData.m_FloatModifiers.InsertAt(carSpeed, 0);
							particleParamsData.m_FloatModifiers.InsertAt(GetWeightCoef(), 1);
							m_VFXComponent.SetSurfaceInfo(WheelGetSurface(i));
							m_VFXComponent.SetEffectGroupPositions(EVehicleVFXEffectGroup.WHEEL_CONTACT, wheelsPosition);
									
							m_VFXComponent.PlayEffectGroup(EVehicleVFXEffectGroup.WHEEL_CONTACT);
						}
						else
							m_VFXComponent.StopEffectGroup(EVehicleVFXEffectGroup.WHEEL_CONTACT);
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
		
		if (m_Time >= GameConstants.CARS_FLUIDS_TICK)
		{
			m_Time = 0;

			PartsHealthCheck();

			bool isServerOrOwner = IsServerOrOwner();

			//First of all check if the car should stop the engine
			if (isServerOrOwner && EngineIsOn())
			{
				if (IsDamageDestroyed() || GetFluidFraction(CarFluid.FUEL) <= 0 || m_EngineHealth <= 0)
					EngineStop();

				CheckVitalItemCallback(IsVitalSparkPlug(), SparkPlug.SLOT_ID, this, "EngineStop");
				CheckVitalItemCallback(IsVitalGlowPlug(), GlowPlug.SLOT_ID, this, "EngineStop");
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

			//! actions runned when the engine on
			if (EngineIsOn())
			{
				if (g_Game.IsServer())
				{
					float dmg;

					if (EngineGetRPM() >= EngineGetRPMRedline())
					{
						if (EngineGetRPM() > EngineGetRPMMax())
							AddHealth( "Engine", "Health", -GetMaxHealth("Engine", "") * 0.05 ); //CAR_RPM_DMG
							
						// only called on server, don't use deterministic RandomFloat
						dmg = EngineGetRPM() * 0.001 * Math.RandomFloat( 0.02, 1.0 );  //CARS_TICK_DMG_MIN; //CARS_TICK_DMG_MAX
						ProcessDirectDamage(DamageType.CUSTOM, null, "Engine", "EnviroDmg", vector.Zero, dmg);
						SetEngineZoneReceivedHit(true);
					}
					else
					{
						SetEngineZoneReceivedHit(false);
					}
				}

				if (isServerOrOwner)
				{
					//! leaking of coolant from radiator when damaged
					if ( IsVitalRadiator() )
					{
						if ( GetFluidFraction(CarFluid.COOLANT) > 0 && m_RadiatorHealth < 0.5 ) //CARS_LEAK_THRESHOLD
							LeakFluid( CarFluid.COOLANT );
					}

					if ( GetFluidFraction(CarFluid.FUEL) > 0 && m_FuelTankHealth < GameConstants.DAMAGE_DAMAGED_VALUE )
						LeakFluid( CarFluid.FUEL );

					if ( GetFluidFraction(CarFluid.BRAKE) > 0 && m_EngineHealth < GameConstants.DAMAGE_DAMAGED_VALUE )
						LeakFluid( CarFluid.BRAKE );

					if ( GetFluidFraction(CarFluid.OIL) > 0 && m_EngineHealth < GameConstants.DAMAGE_DAMAGED_VALUE )
						LeakFluid( CarFluid.OIL );

					if ( m_EngineHealth < 0.25 )
						LeakFluid( CarFluid.OIL );
				}
				
				if ( g_Game.IsServer() )
				{
					if ( IsVitalRadiator() )
					{
						if ( GetFluidFraction( CarFluid.COOLANT ) < 0.5 && GetFluidFraction( CarFluid.COOLANT ) >= 0 )
						{
							// only called on server, don't use deterministic RandomFloat
							dmg = ( 1 - GetFluidFraction(CarFluid.COOLANT) ) * Math.RandomFloat( 0.02, 10.00 );  //CARS_DMG_TICK_MIN_COOLANT; //CARS_DMG_TICK_MAX_COOLANT
							AddHealth( "Engine", "Health", -dmg );
							SetEngineZoneReceivedHit(true);
						}
					}
				}
			}
		}
	}
	
	void OnBrakesPressed()
	{
		UpdateLights();
	}
	
	void OnBrakesReleased()
	{
		UpdateLights();
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
	void OnVehicleJumpOutServer(GetOutTransportActionData gotActionData)
	{
		PlayerBase player = gotActionData.m_Player;
			
		array<ClothingBase> equippedClothes = new array<ClothingBase>;
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("LEGS")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("BACK")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("VEST")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("HeadGear")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("Mask")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("BODY")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("FEET")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("GLOVES")));

		// -----------------------------------------------
		float shockTaken = (gotActionData.m_Speed * gotActionData.m_Speed) / ActionGetOutTransport.DMG_FACTOR;
		
		//Lower shock taken if player uses a helmet
		ItemBase headGear = ClothingBase.Cast(player.GetItemOnHead());
		HelmetBase helmet;
		if (Class.CastTo(helmet, headGear))
			shockTaken *= 0.5;

		// -----------------------------------------------
		
		int randNum; //value used for probability evaluation
		randNum = Math.RandomInt(0, 100);
		if (gotActionData.m_Speed < ActionGetOutTransport.LOW_SPEED_VALUE)
		{
			if (randNum < 20)
				player.GiveShock(-shockTaken); //To inflict shock, a negative value must be passed

			randNum = Math.RandomIntInclusive(0, PlayerBase.m_BleedingSourcesLow.Count() - 1);
			
			if (player.m_BleedingManagerServer)
				player.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesLow[randNum]);
		}
		else if (gotActionData.m_Speed >= ActionGetOutTransport.LOW_SPEED_VALUE && gotActionData.m_Speed < ActionGetOutTransport.HIGH_SPEED_VALUE)
		{
			if (randNum < 50)
				player.GiveShock(-shockTaken);

			randNum = Math.RandomInt(0, PlayerBase.m_BleedingSourcesUp.Count() - 1);
			
			if (player.m_BleedingManagerServer)
				player.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection(PlayerBase.m_BleedingSourcesUp[randNum]);
		}
		else if (gotActionData.m_Speed >= ActionGetOutTransport.HIGH_SPEED_VALUE)
		{
			if (!headGear && player.m_BleedingManagerServer)
				player.m_BleedingManagerServer.AttemptAddBleedingSourceBySelection("Head");

			if (randNum < 75)
				player.GiveShock(-shockTaken);
		}
		
		float dmgTaken = (gotActionData.m_Speed * gotActionData.m_Speed) / ActionGetOutTransport.SHOCK_FACTOR;
		
		//Damage all currently equipped clothes
		foreach (ClothingBase cloth : equippedClothes)
		{
			//If no item is equipped on slot, slot is ignored
			if (cloth == null)
				continue;

			cloth.DecreaseHealth(dmgTaken, false);
		}
		
		vector posMS = gotActionData.m_Player.WorldToModel(gotActionData.m_Player.GetPosition());
		gotActionData.m_Player.DamageAllLegs(dmgTaken); //Additionnal leg specific damage dealing
		
		float healthCoef = Math.InverseLerp(ActionGetOutTransport.HEALTH_LOW_SPEED_VALUE, ActionGetOutTransport.HEALTH_HIGH_SPEED_VALUE, gotActionData.m_Speed);
		healthCoef = Math.Clamp(healthCoef, 0.0, 1.0);
		gotActionData.m_Player.ProcessDirectDamage(DamageType.CUSTOM, gotActionData.m_Player, "", "FallDamageHealth", posMS, healthCoef);
	}
	
	protected override bool DetectFlipped(VehicleFlippedContext ctx)
	{
		if (!DetectFlippedUsingWheels(ctx, GameConstants.VEHICLE_FLIP_WHEELS_LIMITED))
			return false;
		if (!DetectFlippedUsingSurface(ctx, GameConstants.VEHICLE_FLIP_ANGLE_TOLERANCE))
			return false;
		return true;
	}
	
#ifdef DIAG_DEVELOPER
	ref DebugTextWorldSpace m_DebugText;
	ref Param8<float, float, float, float, float, float, float, float> m_DebugParams = new Param8<float, float, float, float, float, float, float, float>(0,0,0,0,0,0,0,0);
#endif
	
	override void OnInput(float dt)
	{
		super.OnInput(dt);
		
#ifdef DIAG_DEVELOPER
		if (m_eDebugMode == ECarDebugMode.NONE)
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
		
		float currentSpeed = GetSpeedometer();
		
		float longitudinalWanted = 0.0;
		float throttleWanted = 0.0;
		float steeringWanted = 0.0;
		
		bool attemptSpeed = false;
		float speedWanted = 0;
		
		string text = "";
		
		switch (m_eDebugMode)
		{
		case ECarDebugMode.FORWARD_10:
			attemptSpeed = true;
			speedWanted = 3;
			break;
		case ECarDebugMode.FORWARD_50:
			attemptSpeed = true;
			speedWanted = 50;
			break;
		case ECarDebugMode.RANDOM_DRIVING:
			attemptSpeed = true;
			
			m_DebugParams.param1 -= dt;
			if (m_DebugParams.param1 <= 0)
			{
				m_DebugParams.param1 = Math.RandomFloatInclusive(0.01, 1.0);
								
				speedWanted = speedWanted + Math.RandomFloatInclusive(-25, 25);
				
				speedWanted = Math.Clamp(speedWanted, -20, 100);
				
				steeringWanted = Math.RandomFloatInclusive(-1, 1);
				
				float steeringWantedSign = (steeringWanted > 0) - (steeringWanted < 0);
				steeringWanted = steeringWantedSign * Math.Pow(Math.AbsFloat(steeringWanted), 0.5);
				
				if (Math.AbsFloat(speedWanted - currentSpeed) > 15)
				{
					steeringWanted *= 0.1;
				}
				
				m_DebugParams.param2 = steeringWanted;
				m_DebugParams.param3 = speedWanted;
			}
			
			steeringWanted = m_DebugParams.param2;
			speedWanted = m_DebugParams.param3;
		
			text += "\nTarget Steering: " + steeringWanted;
			text += "\nTarget Speed: " + speedWanted;
			text += "\nCurrent Speed: " + currentSpeed;

			break;
		case ECarDebugMode.PREDEFINED_PATH:
			attemptSpeed = true;
			speedWanted = 50;
			break;
		}
		
		if (!GetGame().IsDedicatedServer())
		{
			vector transform[4];
			GetTransform(transform);
			m_DebugText = DebugTextWorldSpace.CreateInWorld(text, DebugTextFlags.DEFAULT | DebugTextFlags.FACE_CAMERA, transform, 1.0);
		}

		if (attemptSpeed)
		{
			longitudinalWanted = speedWanted - currentSpeed;
	
			if (speedWanted > 0.1 && currentSpeed > 0.1)
			{
				longitudinalWanted = Math.Max(longitudinalWanted, 0.1);
			}
			else if (speedWanted < 0.1 && currentSpeed < 0.1)
			{
				longitudinalWanted = Math.Min(longitudinalWanted, -0.1);
			}
		}
		
		//! Use the pitch to increase the throttle if the car is going up a hill
		float hillFactor = GetOrientation()[1] / 10.0;
		hillFactor += 1.0;
		hillFactor = Math.Clamp(hillFactor, 0.5, 5.0);
		
		//! Prevent the throttle from redlining the engine
		throttleWanted = (EngineGetRPMMin() - EngineGetRPM()) / (EngineGetRPMMin() - EngineGetRPMRedline());
		throttleWanted = 1.0 - throttleWanted;
		throttleWanted *= Math.AbsFloat(longitudinalWanted);
		throttleWanted *= hillFactor;
		throttleWanted = Math.Clamp(throttleWanted, 0.0, 1.0); 
				
		bool isManual = GearboxGetType() == CarGearboxType.MANUAL;
		bool isReverse = GetGear() == CarGear.REVERSE;
		
		if (isManual)
		{
			float thrustWantedAbs = Math.AbsFloat(throttleWanted);
			
			/*
			wtf enforce script?
			bool override1 = false;
			
			if (attemptSpeed)
			{
				if (speedWanted < 0 && currentSpeed < 10)
				{
					ShiftTo(CarGear.REVERSE);
					override1 = true;
				}
				else if (speedWanted <= 1)
				{
					ShiftTo(CarGear.NEUTRAL);
					override1 = true;
				}
			}
			*/

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
			else if (GetGear() < CarGear.FIRST || (throttleWanted > 0.0 && Math.AbsFloat(currentSpeed) < 5.0))
			{
				ShiftTo(CarGear.FIRST);
			}
		}
		
		SetThrottle(throttleWanted);
		SetSteering(steeringWanted);
		SetBrake(0.1);
		SetHandbrake(0.0);
		SetBrakesActivateWithoutDriver(false);
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
			ItemBase battery = GetBattery();
			if (battery)
			{
				m_BatteryTimer += dt;
				if (m_BatteryTimer >= BATTERY_UPDATE_DELAY)
				{
					UpdateBattery(battery);
				}
			}
			
			if ( g_Game.GetWaterDepth( GetEnginePosWS() ) > 0 )
			{
				m_DrownTime += dt;
				if ( m_DrownTime > DROWN_ENGINE_THRESHOLD )
				{
					// *dt to get damage per second
					AddHealth( "Engine", "Health", -DROWN_ENGINE_DAMAGE * dt);
					SetEngineZoneReceivedHit(true);
				}
			}
			else
			{
				m_DrownTime = 0;
			}
		}

		// For visualisation of brake lights for all players
		float brake_coef = GetBrake();
		if ( brake_coef > 0 )
		{
			if ( !m_BrakesArePressed )
			{
				m_BrakesArePressed = true;
				SetSynchDirty();
				OnBrakesPressed();
			}
		}
		else
		{
			if ( m_BrakesArePressed )
			{
				m_BrakesArePressed = false;
				SetSynchDirty();
				OnBrakesReleased();
			}
		}
    }
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		m_EngineDestroyed = true;
	}
	
	//! Responsible for damaging the car according to contact events from OnContact
	void CheckContactCache()
	{
		int contactZonesCount = m_ContactCache.Count();
		
		if (contactZonesCount == 0)
			return;
		
		for (int i = 0; i < contactZonesCount; ++i)
		{
			string zoneName = m_ContactCache.GetKey(i);
			array<ref VehicleContactData> data = m_ContactCache[zoneName];

			float dmg = Math.AbsInt(data[0].m_Impulse * m_dmgContactCoef);
			float crewDmgBase = Math.AbsInt((data[0].m_Impulse / dBodyGetMass(this)) * 1000 * m_dmgContactCoef);// calculates damage as if the object's weight was 1000kg instead of its actual weight

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
			if ( dmg < GameConstants.CARS_CONTACT_DMG_MIN )
				continue;

			int pddfFlags;
			#ifdef DIAG_DEVELOPER
			CrashDebugData.m_CrashData.Insert(CrashDebugData.m_CrashDataPoint);
			CrashDebugData.m_CrashDataPoint.m_Speedometer =  GetSpeedometer();
			//Print("Crash data recorded");
			#endif
			if (dmg < GameConstants.CARS_CONTACT_DMG_THRESHOLD)
			{			
				#ifdef DIAG_DEVELOPER
				CrashDebugData.m_CrashDataPoint.m_DamageType = "Small";
				if (DEBUG_OUTPUT_TYPE & EVehicleDebugOutputType.DAMAGE_APPLIED)
					Debug.Log(string.Format("[Vehiles:Damage]:: DMG %1 to the %2 zone is SMALL (threshold: %3), SPEEDOMETER: %4, TIME: %5", dmg, zoneName, GameConstants.CARS_CONTACT_DMG_MIN, GetSpeedometer(), g_Game.GetTime() ));
				#endif
				SynchCrashLightSound(true);
				pddfFlags = ProcessDirectDamageFlags.NO_TRANSFER;
			}
			else
			{
				#ifdef DIAG_DEVELOPER
				CrashDebugData.m_CrashDataPoint.m_DamageType = "Big";
				if (DEBUG_OUTPUT_TYPE & EVehicleDebugOutputType.DAMAGE_APPLIED)
					Debug.Log(string.Format("[Vehiles:Damage]:: DMG %1 to the %2 zone is BIG (threshold: %3), SPEED: %4, TIME: %5", dmg, zoneName, GameConstants.CARS_CONTACT_DMG_THRESHOLD, GetSpeedometer(), g_Game.GetTime() ));
				#endif
				DamageCrew(crewDmgBase);
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
	void DamageCrew(float dmg)
	{
		int crewSize = CrewSize();
		for (int c = 0; c < crewSize; ++c)
		{
			Human crew = CrewMember(c);
			if (!crew)
				continue;

			if (dmg > GameConstants.CARS_CONTACT_DMG_KILLCREW)
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
				float shockTemp = Math.InverseLerp(GameConstants.CARS_CONTACT_DMG_THRESHOLD, GameConstants.CARS_CONTACT_DMG_KILLCREW, dmg);
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

	/*!
		Gets called every sound simulation step.
		In this callback, user can modify behaviour of sound controllers
		The higher the return value is the more muted sound is.
	*/
	override float OnSound( CarSoundCtrl ctrl, float oldValue )
	{
		switch (ctrl)
		{
		case CarSoundCtrl.ENGINE:
			if (!m_EngineStarted)
			{
				return 0.0;
			}
		break;
		}
		
		return oldValue;
	}
	
	override void OnAnimationPhaseStarted(string animSource, float phase)
	{
		#ifndef SERVER
		#ifndef NO_GUI
		HandleDoorsSound(animSource, phase);
		HandleSeatAdjustmentSound(animSource, phase);
		#endif
		#endif
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
		
		PlayerBase player;
		if (Class.CastTo(player, g_Game.GetPlayer()))
		{
			if (player.IsCameraInsideVehicle())
			{
				waveKind = WaveKind.WAVEEFFECT;
			}
		}

		sound.SetAutodestroy(true);
		sound.SetParent(this, pivotIndex);
		sound.SetPosition(ModelToWorld(position));
		sound.SetLocalPosition(position);
		sound.SetSoundWaveKind(waveKind);

		return sound;
	}
	
	protected void HandleDoorsSound(string animSource, float phase)
	{
		switch (animSource)
		{
			case "doorsdriver":
			case "doorscodriver":
			case "doorscargo1":
			case "doorscargo2":
			case "doorshood":
			case "doorstrunk":
				EffectSound sound = CreateSoundForAnimationSource(animSource);
				
				if (phase == 0)
					sound.SetSoundSet(m_CarDoorOpenSound);
				else
					sound.SetSoundSet(m_CarDoorCloseSound);

				SEffectManager.EffectRegister(sound);
				sound.SoundPlay();

				break;
		}
	}
	
	protected void HandleSeatAdjustmentSound(string animSource, float phase)
	{
		switch (animSource)
		{
			case "seatdriver":
			case "seatcodriver":
				EffectSound sound = CreateSoundForAnimationSource(animSource);

				if (phase == 0)
					sound.SetSoundSet(m_CarSeatShiftInSound);
				else
					sound.SetSoundSet(m_CarSeatShiftOutSound);
				
				SEffectManager.EffectRegister(sound);
				sound.SoundPlay();

				break;
		}
	}
	
	
	protected void HandleCarHornSound(ECarHornState pState)
	{
		m_HornComponent.HandleSound();
	}
	
	// Only used for sound states which happen before engine start
	void SetCarEngineSoundState(CarEngineSoundState pState)
	{
		m_CarEngineSoundState = pState;
		SetSynchDirty();
	}
	
	void HandleEngineSound(CarEngineSoundState state)
	{
		SetCarEngineSoundState(state);
		m_CarEngineLastSoundState 	= state;

		#ifndef SERVER
		#ifndef NO_GUI
		PlayerBase player = null;
		EffectSound sound = null;
		WaveKind waveKind = WaveKind.WAVEEFFECTEX;
		
		bool doInside = false;

		m_CarEngineLastSoundState = state;
		
		switch (state)
		{
			case CarEngineSoundState.STARTING:
				sound = new EffectSound();
				sound.SetSoundSet("Offroad_02_Starting_SoundSet");
				sound.SetSoundFadeOut(0.15);

				m_PreStartSound = sound;
				break;
			case CarEngineSoundState.START_OK:
				doInside = true;
		
				sound = new EffectSound();
				sound.SetSoundSet(m_EngineStartOK);
				sound.SetAutodestroy(true);
				
				//! postpone the engine sound played from c++ on soundcontroller (via OnSound override)
				g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SetEngineStarted, 1000, false, true);
				break;
				
			case CarEngineSoundState.START_NO_FUEL:
				sound = new EffectSound();
				sound.SetSoundSet("offroad_engine_failed_start_fuel_SoundSet");
				sound.SetAutodestroy(true);
				break;
				
			case CarEngineSoundState.START_NO_BATTERY:
				sound = new EffectSound();
				sound.SetSoundSet("offroad_engine_failed_start_battery_SoundSet");
				sound.SetAutodestroy(true);
				break;
				
			case CarEngineSoundState.START_NO_SPARKPLUG:
				sound = new EffectSound();
				sound.SetSoundSet("offroad_engine_failed_start_sparkplugs_SoundSet");
				sound.SetAutodestroy(true);
				break;
				
			case CarEngineSoundState.STOP_OK:
				doInside = true;
					
				sound = new EffectSound();
				sound.SetSoundSet(m_EngineStop);
				sound.SetAutodestroy(true);
				break;
			case CarEngineSoundState.STOP_NO_FUEL:
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

	/*!
		Gets called everytime when the specified vehicle's fluid
		changes its current value eg. when vehicle is consuming fuel.

		This callback is called on server.
	*/
	override void OnFluidChanged(CarFluid fluid, float newValue, float oldValue)
	{
		switch ( fluid )
		{
		case CarFluid.FUEL:
			m_FuelAmmount = newValue;
			break;

		case CarFluid.OIL:
			m_OilAmmount = newValue;
			break;

		case CarFluid.BRAKE:
			m_BrakeAmmount = newValue;
			break;

		case CarFluid.COOLANT:
			m_CoolantAmmount = newValue;
			break;
		}
	}
	
	/*!
		Gets called everytime the game wants to start the engine.

		\return true if the engine can start, false otherwise.
	*/
	override bool OnBeforeEngineStart()
	{
		SetCarEngineSoundState(CarEngineSoundState.NONE);

		ECarOperationalState state = CheckOperationalRequirements();
		return state == ECarOperationalState.OK;
	}

	void OnIgnition()
	{
		ECarOperationalState state = CheckOperationalRequirements();

		if (state == ECarOperationalState.RUINED)
		{
			return;
		}

		if (state & ECarOperationalState.NO_BATTERY)
		{
			HandleEngineSound(CarEngineSoundState.START_NO_BATTERY);
			return;
		}

		if (state & ECarOperationalState.NO_IGNITER)
		{
			HandleEngineSound(CarEngineSoundState.START_NO_SPARKPLUG);
			return;
		}

		if (state & ECarOperationalState.NO_FUEL)
		{
			HandleEngineSound(CarEngineSoundState.START_NO_FUEL);
			return;
		}

		HandleEngineSound(CarEngineSoundState.STARTING);
	}
	
	// Whether the car engine can be started
	int CheckOperationalRequirements()
	{
		int state = ECarOperationalState.OK;

		EntityAI item = null;

		if (IsDamageDestroyed() || GetHealthLevel("Engine") >= GameConstants.STATE_RUINED)
		{
			state |= ECarOperationalState.RUINED;
		}
		
		if (GetFluidFraction(CarFluid.FUEL) <= 0)
		{
			state |= ECarOperationalState.NO_FUEL;
		}

		if (NeedElectricitySourceDevice())
		{
			item = GetBattery();
			float batteryConsume = GetBatteryConsumption();
			if (!item || (item && (item.IsRuined() || item.GetCompEM().GetEnergy() < batteryConsume)))
				state |= ECarOperationalState.NO_BATTERY;
		}

		if (IsVitalSparkPlug())
		{
			item = FindAttachmentBySlotName("SparkPlug");
			if (!item || (item && item.IsRuined()))
				state |= ECarOperationalState.NO_IGNITER;
		}
		
		if (IsVitalGlowPlug())
		{
			item = FindAttachmentBySlotName("GlowPlug");
			if (!item || (item && item.IsRuined()))
				state |= ECarOperationalState.NO_IGNITER;
		}
		
		return state;
	}

	bool CheckOperationalState()
	{
		return CheckOperationalRequirements() == ECarOperationalState.OK;
	}

	override void OnGearChanged(int newGear, int oldGear)
	{
		//Debug.Log(string.Format("OnGearChanged newGear=%1,oldGear=%2", newGear, oldGear));
		UpdateLights(newGear);
	}
	
	//! Gets called everytime the engine starts.
	override void OnEngineStart()
	{
		ItemBase battery = GetBattery();
		if (g_Game.IsServer() && battery)
		{
			float batteryConsume = GetBatteryConsumption();
			battery.GetCompEM().ConsumeEnergy(batteryConsume);

			UpdateBattery(battery);
		}
		
		UpdateLights();
		
		HandleEngineSound(CarEngineSoundState.START_OK);
	}

	//! Gets called everytime the engine stops.
	override void OnEngineStop()
	{
		ItemBase battery = GetBattery();
		if (g_Game.IsServer() && battery)
		{
			UpdateBattery(battery);
		}

		UpdateLights();
		
		CarEngineSoundState stopSoundState = CarEngineSoundState.STOP_OK;
		if (GetFluidFraction(CarFluid.FUEL) <= 0)
			stopSoundState = CarEngineSoundState.STOP_NO_FUEL;

		HandleEngineSound(stopSoundState);
		
		SetEngineZoneReceivedHit(false);
	}

	override bool OnBeforeLightOn()
	{
		ItemBase neededItem = GetBattery();

		if (neededItem && !neededItem.IsRuined())
		{
			return neededItem.GetCompEM() && neededItem.GetCompEM().GetEnergy() > 0.0;
		}

		return false;
	}
	
	override void UpdateLights(int new_gear = -1)
	{
		#ifndef SERVER
		UpdateLightsClient(new_gear);
		#endif
		UpdateLightsServer(new_gear);
	}
	
	void UpdateLightsClient(int newGear = -1)
	{
		int gear;
		
		if (newGear == -1)
		{
			gear = GetGear();
		}
		else
		{
			gear = newGear;
		}
		
		if (LightIsOn())
		{
			if (!m_LightsComponent.LightExist("Front") && m_HeadlightsState != CarHeadlightBulbsState.NONE)
				m_LightsComponent.EnableLight("Front");
			
			if (m_LightsComponent.LightExist("Front"))
			{
				switch (m_HeadlightsState)
				{
					case CarHeadlightBulbsState.LEFT:
						m_LightsComponent.AttachLightOnPoint("Front", m_LeftHeadlightPoint, m_LeftHeadlightTargetPoint, VehicleLightMode.SEGREGATED);
						break;

					case CarHeadlightBulbsState.RIGHT:
						m_LightsComponent.AttachLightOnPoint("Front", m_RightHeadlightPoint, m_RightHeadlightTargetPoint, VehicleLightMode.SEGREGATED);
						break;

					case CarHeadlightBulbsState.BOTH:
						vector local_pos_left = GetMemoryPointPos(m_LeftHeadlightPoint);
						vector local_pos_right = GetMemoryPointPos(m_RightHeadlightPoint);
						
						vector local_pos_middle = (local_pos_left + local_pos_right) * 0.5;
						m_LightsComponent.AttachLightOnObject("Front", local_pos_middle, vector.Zero, VehicleLightMode.AGGREGATED);
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
			case CarGear.REVERSE:
			case CarAutomaticGearboxMode.R:
				if (m_BrakesArePressed)
					m_RearLightType = CarRearLightType.BRAKES_AND_REVERSE;
				else
					m_RearLightType = CarRearLightType.REVERSE_ONLY;
				break;
			default:
				if (m_BrakesArePressed)
					m_RearLightType = CarRearLightType.BRAKES_ONLY;
				else
					m_RearLightType = CarRearLightType.NONE;
		}

		//Debug.Log(string.Format("m_BrakesArePressed=%1, m_RearLightType=%2", m_BrakesArePressed.ToString(), EnumTools.EnumToString(CarRearLightType, m_RearLightType)));

		if (!m_LightsComponent.LightExist("Rear") && m_RearLightType != CarRearLightType.NONE && m_HeadlightsState != CarHeadlightBulbsState.NONE)
		{
			if (EngineIsOn() || m_RearLightType == CarRearLightType.BRAKES_ONLY || m_RearLightType == CarRearLightType.BRAKES_AND_REVERSE)
			{	
				m_LightsComponent.EnableLight("Rear");
				m_LightsComponent.AttachLightOnObject("Rear", GetMemoryPointPos(m_ReverseLightPoint), "180 0 0", VehicleLightMode.SEGREGATED);
			}
		}

		// rear lights
		if (m_LightsComponent.LightExist("Rear") && m_RearLightType != CarRearLightType.NONE && m_HeadlightsState != CarHeadlightBulbsState.NONE)
		{
			switch (m_RearLightType)
			{
				case CarRearLightType.BRAKES_ONLY:
					m_LightsComponent.SetLightTypeAndMode("Rear", VehicleLightType.BRAKE, VehicleLightMode.SEGREGATED);
					break;

				case CarRearLightType.REVERSE_ONLY:
					if (EngineIsOn())
						m_LightsComponent.SetLightTypeAndMode("Rear", VehicleLightType.REVERSE, VehicleLightMode.SEGREGATED);
					else
						m_LightsComponent.DisableLight("Rear");

					break;

				case CarRearLightType.BRAKES_AND_REVERSE:
					if (EngineIsOn())
						m_LightsComponent.SetLightTypeAndMode("Rear", VehicleLightType.REVERSE_BRAKE, VehicleLightMode.AGGREGATED);
					else
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
	
	void UpdateLightsServer(int newGear = -1)
	{
		int gear;
		
		if (newGear == -1)
		{
			gear = GetGear();
			if (GearboxGetType() == CarGearboxType.AUTOMATIC)
			{
				gear = GearboxGetMode();
			}
		}
		else
		{
			gear = newGear;
		}
		
		if (LightIsOn())
		{
			m_LightsComponent.SetSelectionMaterial("DashboardOn");
			
			switch (m_HeadlightsState)
			{
				case CarHeadlightBulbsState.LEFT:
					m_LightsComponent.SetSelectionMaterial("TailLeftOnly");
					m_LightsComponent.SetSelectionMaterial("FrontLeftOn");
					m_LightsComponent.SetSelectionMaterial("FrontRightOff");
					break;
				case CarHeadlightBulbsState.RIGHT:
					m_LightsComponent.SetSelectionMaterial("TailRightOnly");
					m_LightsComponent.SetSelectionMaterial("FrontLeftOff");
					m_LightsComponent.SetSelectionMaterial("FrontRightOn");
					break;
				case CarHeadlightBulbsState.BOTH:
					m_LightsComponent.SetSelectionMaterial("TailOn");
					m_LightsComponent.SetSelectionMaterial("FrontLeftOn");
					m_LightsComponent.SetSelectionMaterial("FrontRightOn");
					break;
				default:
					m_LightsComponent.SetSelectionMaterial("FrontLeftOff");
					m_LightsComponent.SetSelectionMaterial("FrontRightOff");
			}
			
			//Debug.Log(string.Format("LightIsOn=%1, m_HeadlightsState=%2", LightIsOn().ToString(), EnumTools.EnumToString(CarHeadlightBulbsState, m_HeadlightsState)));
		}
		else
		{
			m_LightsComponent.SetSelectionMaterial("TailOff");
			m_LightsComponent.SetSelectionMaterial("DashboardOff");
			m_LightsComponent.SetSelectionMaterial("FrontLeftOff");
			m_LightsComponent.SetSelectionMaterial("FrontRightOff");
		}
			

		// brakes & reverse
		switch (gear)
		{
			case CarGear.REVERSE:
			case CarAutomaticGearboxMode.R:
				if (m_BrakesArePressed)
					m_RearLightType = CarRearLightType.BRAKES_AND_REVERSE;
				else
					m_RearLightType = CarRearLightType.REVERSE_ONLY;
				break;
			default:
				if (m_BrakesArePressed)
					m_RearLightType = CarRearLightType.BRAKES_ONLY;
				else
					m_RearLightType = CarRearLightType.NONE;
		}

		//Debug.Log(string.Format("m_BrakesArePressed=%1, m_RearLightType=%2", m_BrakesArePressed.ToString(), EnumTools.EnumToString(CarRearLightType, m_RearLightType)));
			
		
		// rear lights
		if (m_RearLightType != CarRearLightType.NONE && m_HeadlightsState != CarHeadlightBulbsState.NONE)
		{
			switch (m_RearLightType)
			{
				case CarRearLightType.BRAKES_ONLY:
					m_LightsComponent.SetSelectionMaterial("ReverseOff");
					m_LightsComponent.SetSelectionMaterial("BrakesOn");
					break;
				case CarRearLightType.REVERSE_ONLY:
					if (EngineIsOn())
					{
						m_LightsComponent.SetSelectionMaterial("ReverseOn");
						if (!HasGangedTailAndBrakeLights())
						 m_LightsComponent.SetSelectionMaterial("BrakesOff");
					}
					else
					{
						m_LightsComponent.SetSelectionMaterial("ReverseOff");
						m_LightsComponent.SetSelectionMaterial("BrakesOff");
					}
					break;
				case CarRearLightType.BRAKES_AND_REVERSE:
					if (EngineIsOn())
					{
						m_LightsComponent.SetSelectionMaterial("BrakesOn");
						m_LightsComponent.SetSelectionMaterial("ReverseOn");
					}
					else
					{
						m_LightsComponent.SetSelectionMaterial("ReverseOff");
						m_LightsComponent.SetSelectionMaterial("BrakesOn");
					}
					break;
				default:
					m_LightsComponent.SetSelectionMaterial("ReverseOff");

					if (!HasGangedTailAndBrakeLights())
						m_LightsComponent.SetSelectionMaterial("BrakesOff");
			}
		}
		else
		{
			m_LightsComponent.SetSelectionMaterial("ReverseOff");
			
			if (!HasGangedTailAndBrakeLights())
				m_LightsComponent.SetSelectionMaterial("BrakesOff");
		}
	}

	protected void LeakFluid(CarFluid fluid)
	{
		float amount = 0;
		
		switch (fluid)
		{
			case CarFluid.COOLANT:
				amount = (1 - m_RadiatorHealth) * RandomFloat(0.02, 0.05);//CARS_LEAK_TICK_MIN; CARS_LEAK_TICK_MAX
				Leak(fluid, amount);
			break;
			
			case CarFluid.OIL:
				amount = (1 - m_EngineHealth) * RandomFloat(0.02, 1.0);//CARS_LEAK_OIL_MIN; CARS_LEAK_OIL_MAX
				Leak(fluid, amount);
			break;
			
			case CarFluid.FUEL:
				amount = (1 - m_FuelTankHealth) * RandomFloat(0.02, 0.05);//CARS_LEAK_TICK_MIN; CARS_LEAK_TICK_MAX
				Leak(fluid, amount);
			break;
		}
	}
	
	override protected void PartsHealthCheck()
	{
		if (HasRadiator())
		{
			EntityAI radiator = GetRadiator();
			int radiatorHealthLevel = radiator.GetHealthLevel("");
			m_RadiatorHealth = GetHealthLevelValue(radiatorHealthLevel, "");
		}
		else
		{
			m_RadiatorHealth = 0.0;
		}

		int engineHealthLevel = GetHealthLevel("Engine");
		m_EngineHealth = GetHealthLevelValue(engineHealthLevel, "Engine");

		int fuelTankHealthLevel = GetHealthLevel("FuelTank");
		m_FuelTankHealth = GetHealthLevelValue(fuelTankHealthLevel, "FuelTank");
	}
	
	string GetDoorConditionPointFromSelection( string selection )
	{
		switch( selection )
		{
			case "seat_driver":
				return "seat_con_1_1";
			break;
			case "seat_codriver":
				return "seat_con_2_1";
			break;
			case "seat_cargo1":
				return "seat_con_1_2";
			break;
			case "seat_cargo2":
				return "seat_con_2_2";
			break;
		}
		
		return "";
	}
	
	string GetDoorSelectionNameFromSeatPos(int posIdx)
	{
		return "";
	}
	
	string GetDoorInvSlotNameFromSeatPos(int posIdx)
	{
		return "";
	}
	
	int GetCrewIndex( string selection )
	{
		return -1;
	}

	override bool CanReachSeatFromDoors( string pSeatSelection, vector pFromPos, float pDistance = 1.0 )
	{
		string conPointName = GetDoorConditionPointFromSelection(pSeatSelection);
		if (conPointName.Length() > 0)
		{
			if ( MemoryPointExists(conPointName) )
			{
				vector conPointMS = GetMemoryPointPos(conPointName);
				vector conPointWS = ModelToWorld(conPointMS);
				
				//! skip the height for now
				conPointWS[1] = 0;
				pFromPos[1] = 0;
				
				if (vector.Distance(pFromPos, conPointWS) <= pDistance)
				{
					return true;
				}
			}
		}

		return false;		
	}
	
	bool IsVitalEngineBelt()
	{
		return true;
	}
	
	bool IsVitalRadiator()
	{
		return true;
	}
	
	bool HasRadiator()
	{
		return m_Radiator != null;
	}
	
	EntityAI GetRadiator()
	{
		return m_Radiator;
	}
	
	bool IsMoving()
	{
		return GetSpeedometerAbsolute() > 3.5;
	}
	
	bool IsHandbrakeActive()
	{
		return GetHandbrake() > 0.0;
	}

	//! camera type
	override int Get3rdPersonCameraType()
	{
		return DayZPlayerCameras.DAYZCAMERA_3RD_VEHICLE;

	}

	void SetEngineStarted(bool started)
	{
		m_EngineStarted = started;
	}

	int GetCarDoorsState(string slotType)
	{
		return CarDoorState.DOORS_MISSING;
	}
	
	CarDoorState TranslateAnimationPhaseToCarDoorState(string animation)
	{
		if (GetAnimationPhase(animation) > 0.5)
		{
			return CarDoorState.DOORS_OPEN;
		}
		else
		{
			return CarDoorState.DOORS_CLOSED;
		}
	}

	string GetActionCompNameCoolant()
	{
		return "radiator";
	}

	float GetActionDistanceCoolant()
	{
		return 2.0;
	}
	
	string GetActionCompNameOil()
	{
		return "carradiator";
	}

	float GetActionDistanceOil()
	{
		return 2.0;
	}
	
	string GetActionCompNameBrakes()
	{
		return "carradiator";
	}

	float GetActionDistanceBrakes()
	{
		return 2.0;
	}
	
	void InitializeActions()
	{
		m_InputActionMap = m_CarTypeActionsMap.Get( this.Type() );
		if (!m_InputActionMap)
		{
			TInputActionMap iam = new TInputActionMap;
			m_InputActionMap = iam;
			SetActions();
			m_CarTypeActionsMap.Insert(this.Type(), m_InputActionMap);
		}
	}
	
	override void GetActions(typename action_input_type, out array<ActionBase_Basic> actions)
	{
		if (!m_ActionsInitialize)
		{
			m_ActionsInitialize = true;
			InitializeActions();
		}
		
		actions = m_InputActionMap.Get(action_input_type);
	}
	
	void SetActions()
	{
		AddAction(ActionOpenCarDoorsOutside);
		AddAction(ActionCloseCarDoorsOutside);
		AddAction(ActionGetInTransport);
		AddAction(ActionSwitchLights);
		AddAction(ActionVehicleHornShort);
		AddAction(ActionVehicleHornLong);
		AddAction(ActionPushCar);
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
			m_ActionsInitialize = false;
			return;
		}
		array<ActionBase_Basic> action_array = m_InputActionMap.Get(ai);
		
		if (!action_array)
		{
			action_array = new array<ActionBase_Basic>;
			m_InputActionMap.Insert(ai, action_array);
		}
#ifdef ENABLE_LOGGING
		if ( LogManager.IsActionLogEnable() )
		{
			Debug.ActionLog(action.ToString() + " -> " + ai, this.ToString() , "n/a", "Add action" );
		}
#endif
		action_array.Insert(action);
	}
	
	void RemoveAction(typename actionName)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		ActionBase action = player.GetActionManager().GetAction(actionName);
		typename ai = action.GetInputType();
		array<ActionBase_Basic> action_array = m_InputActionMap.Get(ai);
		
		if (action_array)
		{
			action_array.RemoveItem(action);
		}
	}
	
	override bool IsInventoryVisible()
	{
		return ( g_Game.GetPlayer() && ( !g_Game.GetPlayer().GetCommand_Vehicle() || g_Game.GetPlayer().GetCommand_Vehicle().GetTransport() == this ) );
	}

	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		if (newLevel == GameConstants.STATE_RUINED && oldLevel != newLevel)
		{
			bool dummy;
			switch ( zone )
			{
			case "WindowLR":
			case "WindowRR":
				if (m_Initialized)
				{
					PlaySoundEx("offroad_hit_window_small_SoundSet", m_WindowSmall, dummy);
				}
			break;
			
			case "WindowFront":
			case "WindowBack":
			case "WindowFrontLeft":
			case "WindowFrontRight":
				if (m_Initialized)
				{
					PlaySoundEx("offroad_hit_window_large_SoundSet", m_WindowLarge, dummy);
				}
			break;

			case "Engine":
				#ifndef SERVER
				#ifndef NO_GUI
				CreateCarDestroyedEffect();
				#endif
				#endif
			break;
			}
		}
	}
	
	override void EEOnCECreate()
	{

		float maxVolume = GetFluidCapacity( CarFluid.FUEL );
		float amount = Math.RandomFloat(0.0, maxVolume * 0.35 );

		Fill( CarFluid.FUEL, amount );
	}
	
	//Get the engine start battery consumption
	float GetBatteryConsumption()
	{
		return m_BatteryConsume;
	}
	
	float GetBatteryRuntimeConsumption()
	{
		return m_BatteryContinuousConsume;
	}

	float GetBatteryRechargeRate()
	{
		return -m_BatteryRecharge;
	}

	ItemBase GetBattery()
	{
		return ItemBase.Cast(GetElectricitySourceDevice());
	}

	protected void UpdateBattery(ItemBase battery)
	{
		if (!battery)
		{
			m_BatteryTimer = 0;
			return;
		}
		
		// unlikely
		if (m_BatteryTimer < 0)
		{
			m_BatteryTimer = 0;
		}

		bool engineOn = EngineIsOn();
		bool lightsOn = LightIsOn();

		if (engineOn)
		{
			// alternator
			battery.GetCompEM().ConsumeEnergy(GetBatteryRechargeRate() * m_BatteryTimer);
		}
		else if (!engineOn && lightsOn)
		{
			battery.GetCompEM().ConsumeEnergy(GetBatteryRuntimeConsumption() * m_BatteryTimer);
		}

		if (lightsOn && battery.GetCompEM().GetEnergy() <= 0)
		{
			// lights currently don't automatically turn back on if the headlight was last turned on, so we just keep them on if the engine is on
			if (!engineOn)
			{
				LightOff();
			}
		}

		m_BatteryTimer = 0;
	}
	
	void SetCarHornState(int pState)
	{
		m_HornComponent.SetMode(pState);

		//! old usage (pre 1.30)
		m_CarHornState = pState;
		SetSynchDirty();
	}

	override vector GetDefaultHitPosition()
	{
		return vector.Zero;
	}
	
	float GetPushForceCoefficientMultiplier()
	{
		return 1.0;
	}

	protected void SpawnUniversalParts()
	{
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory("HeadlightH7");
		inventory.CreateInInventory("HeadlightH7");
		inventory.CreateInInventory("HeadlightH7");
		inventory.CreateInInventory("HeadlightH7");

		if (NeedElectricitySourceDevice())
			GetInventory().CreateInInventory(GetElectricitySourceDeviceType());

		if (IsVitalRadiator())
		{
			inventory.CreateInInventory("CarRadiator");
			inventory.CreateInInventory("CarRadiator");
		}

		if (IsVitalSparkPlug())
		{
			inventory.CreateInInventory("SparkPlug");
			inventory.CreateInInventory("SparkPlug");
		}

		if (IsVitalGlowPlug())
		{
			inventory.CreateInInventory("GlowPlug");
			inventory.CreateInInventory("GlowPlug");
		}
	}

	protected void SpawnAdditionalItems()
	{
		GameInventory inventory = GetInventory();
		inventory.CreateInInventory("Wrench");
		inventory.CreateInInventory("LugWrench");
		inventory.CreateInInventory("Screwdriver");
		inventory.CreateInInventory("EpoxyPutty");

		inventory.CreateInInventory("CanisterGasoline");

		EntityAI ent;
		ItemBase container;
		ent = inventory.CreateInInventory("CanisterGasoline");
		if (Class.CastTo(container, ent))
		{
			container.SetLiquidType(LIQUID_WATER, true);
		}

		ent = inventory.CreateInInventory("Blowtorch");
		if (ent)
		{
			ent.GetInventory().CreateInInventory("LargeGasCanister");
		}

		ent = inventory.CreateInInventory("Blowtorch");
		if (ent)
		{
			ent.GetInventory().CreateInInventory("LargeGasCanister");
		}
	}

	protected void FillUpCarFluids()
	{
		Fill(CarFluid.FUEL, 200.0);
		Fill(CarFluid.COOLANT, 200.0);
		Fill(CarFluid.OIL, 200.0);
	}

	protected override event typename GetOwnerStateType()
	{
		return CarScriptOwnerState;
	}

	protected override event typename GetMoveType()
	{
		return CarScriptMove;
	}

	protected override event void ObtainState(/*inout*/ PawnOwnerState pState)
	{
		auto state = CarScriptOwnerState.Cast(pState);
		state.m_fTime = m_Time;
	}

	protected override event void RewindState(PawnOwnerState pState, /*inout*/ PawnMove pMove, inout NetworkRewindType pRewindType)
	{
		auto state = CarScriptOwnerState.Cast(pState);
		m_Time = state.m_fTime;
	}

	// Cars that use the old networking only perform this part of the simulation on the server and not the clients
	// TODO(kumarjac): Obsolete this function once new networking is permanently enabled
	bool IsServerOrOwner()
	{
		bool isServer = g_Game.IsServer();
		if (isServer || GetNetworkMoveStrategy() != NetworkMoveStrategy.PHYSICS)
		{
			return isServer;
		}
		
		return IsOwner();
	}
	
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

		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "Cooler", FadeColors.RED));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.CAR_COOLANT_FULL, "Full", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.CAR_COOLANT_EMPTY, "Empty", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.CAR_COOLANT_INCREASE, "10% increase", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.CAR_COOLANT_DECREASE, "10% decrease", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		
		typename e = ECarDebugMode;
		
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

		if (!GetGame().IsServer())
		{
			return false;
		}

		switch (action_id)
		{
			case EActions.VEHICLE_HORN_START_SHORT:
				SetCarHornState(ECarHornState.SHORT);
				return true;
			case EActions.VEHICLE_HORN_START_LONG:
				SetCarHornState(ECarHornState.LONG);
				return true;
			case EActions.VEHICLE_HORN_STOP:
				SetCarHornState(ECarHornState.OFF);
				return true;

			case EActions.VEHICLE_FUEL_FULL:
				Fill(CarFluid.FUEL, GetFluidCapacity(CarFluid.FUEL));
				return true;
			case EActions.VEHICLE_FUEL_EMPTY:
				LeakAll(CarFluid.FUEL);
				return true;
			case EActions.VEHICLE_FUEL_INCREASE:
				Fill(CarFluid.FUEL, GetFluidCapacity(CarFluid.FUEL) * 0.1);
				return true;
			case EActions.VEHICLE_FUEL_DECREASE:
				Leak(CarFluid.FUEL, GetFluidCapacity(CarFluid.FUEL) * 0.1);
				return true;

			case EActions.CAR_COOLANT_FULL:
				Fill(CarFluid.COOLANT, GetFluidCapacity(CarFluid.COOLANT));
				return true;
			case EActions.CAR_COOLANT_EMPTY:
				LeakAll(CarFluid.COOLANT);
				return true;
			case EActions.CAR_COOLANT_INCREASE:
				Fill(CarFluid.COOLANT, GetFluidCapacity(CarFluid.COOLANT) * 0.1);
				return true;
			case EActions.CAR_COOLANT_DECREASE:
				Leak(CarFluid.COOLANT, GetFluidCapacity(CarFluid.COOLANT) * 0.1);
				return true;
		}

		typename e = ECarDebugMode;
		
		int i;
			
		int cnt = e.GetVariableCount();
		int val;
	
		for (i = 0; i < cnt; i++)
		{
			if (!e.GetVariableValue(null, i, val))
				continue;
			
			if ((val + EActions.PLAYER_BOT_INTERNAL_START) == action_id)
			{
				m_eDebugMode = val;
				dBodyActive(this, ActiveState.ACTIVE);
			}
		}
	
		return false;
	}
	
	override void FixEntity()
	{
		if (GetGame().IsServer())
		{
			FillUpCarFluids();
			//server and single
			
			for (int i = 5; i > 0; i--)//there is a problem with wheels when performed only once, this solves it
				super.FixEntity();
			if (!GetGame().IsMultiplayer())
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
	
	#ifdef DEVELOPER
	override protected string GetDebugText()
	{
		string debug_output = super.GetDebugText();
		if (GetGame().IsServer())
		{
			debug_output += m_DebugContactDamageMessage + "\n";
		}
		debug_output += "Entity momentum: " + GetMomentum();
		int capacity = GetFluidCapacity(CarFluid.FUEL);
		debug_output += string.Format("\nFuel:%1/%2", capacity * GetFluidFraction(CarFluid.FUEL), capacity);
		debug_output += "\nIsEngineON: " + EngineIsOn();

		return debug_output;	
	}
	#endif
		
	//! DEPRECATED
		
	CarLightBase 		m_Headlight;					//! VehicleLightsComponent
	CarRearLightBase 	m_RearLight;					//! VehicleLightsComponent
	
	string m_CarHornShortSoundName 		= "";			//! VehicleHornComponent
	string m_CarHornLongSoundName		= "";			//! VehicleHornComponent
	protected int m_CarHornState;						//! VehicleHornComponent
	protected ref EffectSound m_CarHornSoundEffect;		//! VehicleHornComponent
	protected ref NoiseParams m_NoisePar;				//! VehicleHornComponent
	protected NoiseSystem m_NoiseSystem;				//! VehicleHornComponent
	
	protected float m_BatteryEnergyStartMin = 5.0;
	protected bool m_HeadlightsOn;
	protected bool m_BrakesArePressed; // not yet actually removed, but functionality will be moved to native to prevent requiring updating of lights every game tick

	//! old particles
	protected ref EffVehicleSmoke m_coolantFx;
	protected ref EffVehicleSmoke m_engineFx;
	protected ref EffVehicleSmoke m_exhaustFx;
	
	protected ref array<ref EffWheelSmoke> m_WheelSmokeFx;
	protected ref array<int> m_WheelSmokePtcFx;
		
	protected int m_enginePtcFx;
	protected int m_coolantPtcFx;
	protected int m_exhaustPtcFx;
	
	protected vector m_exhaustPtcPos;
	protected vector m_enginePtcPos;
	protected vector m_coolantPtcPos;

	/*!
		DEPRECATED Gets called everytime the game wants to switch the lights.
		\return true when lights can be switched, false otherwise.
	*/
	[Obsolete("1.30: no replacement")]
	bool OnBeforeSwitchLights(bool toOn)
	{
		return true;
	}

	[Obsolete("1.30: no replacement")]
	float GetEnviroHeatComfortOverride();
	
	[Obsolete("1.30: no replacement")]
	void ForceUpdateLightsStart();
	
	[Obsolete("1.30: no replacement")]
	void ForceUpdateLightsEnd();

	[Obsolete("1.30: use Transport.NeedElectricitySourceDevice")]
	bool IsVitalCarBattery()
	{
		return true;
	}
	
	[Obsolete("1.30: use Transport.NeedElectricitySourceDevice")]
	bool IsVitalTruckBattery()
	{
		return true;
	}
	
	[Obsolete("1.30: use Transport.LightIsOn")]
	bool IsScriptedLightsOn()
	{
		return LightIsOn();
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void BrakesRearLight()
	{
		m_LightsComponent.SetLightTypeAndMode("Rear", VehicleLightType.BRAKE, VehicleLightMode.SEGREGATED);
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void ReverseRearLight()
	{
		m_LightsComponent.SetLightTypeAndMode("Rear", VehicleLightType.REVERSE, VehicleLightMode.SEGREGATED);
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void BrakeAndReverseRearLight()
	{
		m_LightsComponent.SetLightTypeAndMode("Rear", VehicleLightType.REVERSE_BRAKE, VehicleLightMode.AGGREGATED);
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void NoRearLight()
	{
		m_LightsComponent.DisableLight("Rear");
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void LeftFrontLightShineOn()
	{
		m_LightsComponent.SetSelectionMaterial("FrontLeftOn");
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void RightFrontLightShineOn()
	{
		m_LightsComponent.SetSelectionMaterial("FrontRightOn");
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void LeftFrontLightShineOff()
	{
		m_LightsComponent.SetSelectionMaterial("FrontLeftOff");
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void RightFrontLightShineOff()
	{
		m_LightsComponent.SetSelectionMaterial("FrontRightOff");
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void ReverseLightsShineOn()
	{
		m_LightsComponent.SetSelectionMaterial("ReverseOn");
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void ReverseLightsShineOff()
	{
		m_LightsComponent.SetSelectionMaterial("ReverseOff");
	}
	
	[Obsolete("1.30:  VehicleLightsComponent instead")]
	protected void BrakeLightsShineOn()
	{
		m_LightsComponent.SetSelectionMaterial("BrakesOn");
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void BrakeLightsShineOff()
	{
		m_LightsComponent.SetSelectionMaterial("BrakesOff");
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void TailLightsShineOn()
	{
		if (m_HeadlightsState == CarHeadlightBulbsState.LEFT)
		{
			m_LightsComponent.SetSelectionMaterial("TailLeftOnly");
		}
		else if (m_HeadlightsState == CarHeadlightBulbsState.RIGHT)
		{
			m_LightsComponent.SetSelectionMaterial("TailRighttOnly");
		}
		else if (m_HeadlightsState == CarHeadlightBulbsState.BOTH)
		{
			m_LightsComponent.SetSelectionMaterial("TailOn");
		}
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void TailLightsShineOff()
	{
		m_LightsComponent.SetSelectionMaterial("TailOff");;
	}
	
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void DashboardShineOn()
	{
		m_LightsComponent.SetSelectionMaterial("DashboardOn");
	}

	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected void DashboardShineOff()
	{
		m_LightsComponent.SetSelectionMaterial("DashboardOff");
	}
	
	// Override this for a car-specific light type
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected CarRearLightBase CreateRearLight()
	{
		return CarRearLightBase.Cast(ScriptedLightBase.CreateLight(OffroadHatchbackFrontLight));
	}
	
	// Override this for a car-specific light type
	[Obsolete("1.30: use VehicleLightsComponent instead")]
	protected CarLightBase CreateFrontLight()
	{
		return CarLightBase.Cast(ScriptedLightBase.CreateLight(OffroadHatchbackFrontLight));
	}
	
	
	[Obsolete("1.30: use Transport::CheckVitalItemCallback instead")]
	protected void CheckVitalItem(bool isVital, int slotId)
	{
		GameInventory inventory = GetInventory();
		if ( !isVital || !inventory )
			return;

		EntityAI item = inventory.FindAttachment(slotId);

		if (!item || item.IsRuined())
		{
			EngineStop();
		}
	}
	
	[Obsolete("1.30: use Transport::CheckVitalItemCallback instead")]
	protected void CheckVitalItem(bool isVital, string slot_name)
	{
		if ( !isVital )
			return;

		int slotId = InventorySlots.GetSlotIdFromString(slot_name);
		CheckVitalItem(isVital, slotId);
	}

	[Obsolete("1.30: use Transport::PartsHealthCheck instead")]
	protected void CarPartsHealthCheck()
	{
		PartsHealthCheck();
	}
}
