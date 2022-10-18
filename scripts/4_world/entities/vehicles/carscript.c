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

enum CarEngineSoundState
{
	NONE,
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

class CarContactData
{
	vector localPos;
	IEntity other;
	float impulse;
	
	void CarContactData(vector _localPos, IEntity _other, float _impulse)
	{
		localPos	= _localPos;
		other		= _other;
		impulse		= _impulse;
	}
}

typedef map<string, ref array<ref CarContactData>> CarContactCache

/*!
	Base script class for all motorized wheeled vehicles.
*/
class CarScript extends Car
{
	static ref map<typename, ref TInputActionMap> m_CarTypeActionsMap = new map<typename, ref TInputActionMap>;
	TInputActionMap m_InputActionMap;
	bool	m_ActionsInitialize;
	
	ref CarContactCache m_ContactCache;

	protected float m_Time;

	static float DROWN_ENGINE_THRESHOLD = 0.5;
	static float DROWN_ENGINE_DAMAGE = 350.0;
	
	//! keeps ammount of each fluid
	protected float m_FuelAmmount;
	protected float m_CoolantAmmount;
	protected float m_OilAmmount;
	protected float m_BrakeAmmount;

	//!
	protected float m_dmgContactCoef;
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
	protected float m_BatteryEnergyStartMin = 5.0;		// Minimal energy level of battery required for start

	//! Particles
	protected ref EffVehicleSmoke m_coolantFx;
	protected ref EffVehicleSmoke m_engineFx;
	protected ref EffVehicleSmoke m_exhaustFx;
		
	protected int m_enginePtcFx;
	protected int m_coolantPtcFx;
	protected int m_exhaustPtcFx;
	
	protected vector m_exhaustPtcPos;
	protected vector m_exhaustPtcDir;
	protected vector m_enginePtcPos;
	protected vector m_coolantPtcPos;
	
	protected vector m_fuelPos;

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
	string m_EngineStopFuel 			= "";
	
	string m_CarDoorOpenSound 			= "";
	string m_CarDoorCloseSound 			= "";
	
	string m_CarHornShortSoundName 		= "";
	string m_CarHornLongSoundName		= "";
	
	ref EffectSound m_CrashSoundLight;
	ref EffectSound m_CrashSoundHeavy;
	ref EffectSound m_WindowSmall;
	ref EffectSound m_WindowLarge;
	protected ref EffectSound m_CarHornSoundEffect;
	protected ref NoiseParams m_NoisePar;
	protected NoiseSystem m_NoiseSystem;
	
	protected bool m_PlayCrashSoundLight;
	protected bool m_PlayCrashSoundHeavy;
	
	protected bool m_HeadlightsOn;
	protected bool m_HeadlightsState;
	protected bool m_BrakesArePressed;
	protected bool m_RearLightType;
	
	protected bool m_ForceUpdateLights;
	protected bool m_EngineStarted;
	protected bool m_EngineDestroyed;
	
	protected int m_CarHornState;
	
	CarLightBase 		m_Headlight;
	CarRearLightBase 	m_RearLight;
	
	// Memory points for lights
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
	
	protected bool m_EngineBeforeStart;
	protected bool m_EngineStartDoOnce;

	protected bool m_EngineZoneReceivedHit;
	
	#ifdef DEVELOPER 
	private const int DEBUG_MESSAGE_CLEAN_TIME_SECONDS = 10;
	private float m_DebugMessageCleanTime;
	private string m_DebugContactDamageMessage;
	#endif
	
	void CarScript()
	{
		SetEventMask(/*EntityEvent.CONTACT |*/ EntityEvent.POSTSIMULATE);
		
		m_ContactCache = new CarContactCache;
		
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

		m_dmgContactCoef = 0;
		m_EnviroHeatComfortOverride = 0;

		m_PlayCrashSoundLight = false;
		m_PlayCrashSoundHeavy = false;
		
		m_CarHornState = ECarHornState.OFF;
		
		RegisterNetSyncVariableBool("m_HeadlightsOn");
		RegisterNetSyncVariableBool("m_BrakesArePressed");
		RegisterNetSyncVariableBool("m_ForceUpdateLights");
		RegisterNetSyncVariableBool("m_EngineZoneReceivedHit");
		RegisterNetSyncVariableBoolSignal("m_PlayCrashSoundLight");
		RegisterNetSyncVariableBoolSignal("m_PlayCrashSoundHeavy");
		RegisterNetSyncVariableInt("m_CarHornState", ECarHornState.OFF, ECarHornState.LONG);
		
		if ( MemoryPointExists("ptcExhaust_end") )
		{
			m_exhaustPtcPos = GetMemoryPointPos("ptcExhaust_end");
			if ( MemoryPointExists("ptcExhaust_start") )
			{
				vector exhaustStart = GetMemoryPointPos("ptcExhaust_start");
				vector tempOri = vector.Direction( exhaustStart, m_exhaustPtcPos);

				m_exhaustPtcDir[0] = -tempOri[2];
				m_exhaustPtcDir[1] = tempOri[1];
				m_exhaustPtcDir[2] = tempOri[0];
				
				m_exhaustPtcDir = m_exhaustPtcDir.Normalized().VectorToAngles();
			}
		}
		else
		{
			m_exhaustPtcPos = "0 0 0";
			m_exhaustPtcDir = "1 1 1";
		}
	
		if ( MemoryPointExists("ptcEnginePos") )
			m_enginePtcPos = GetMemoryPointPos("ptcEnginePos");
		else
			m_enginePtcPos = "0 0 0";		

		if ( MemoryPointExists("ptcCoolantPos") )
			m_coolantPtcPos = GetMemoryPointPos("ptcCoolantPos");
		else
			m_coolantPtcPos = "0 0 0";

		if ( MemoryPointExists("refill") )
			m_fuelPos = GetMemoryPointPos("refill");
		else
			m_fuelPos = "0 0 0";
		
		if ( MemoryPointExists("drown_engine") )
			m_DrownEnginePos = GetMemoryPointPos("drown_engine");
		else
			m_DrownEnginePos = "0 0 0";	

		if ( MemoryPointExists("dmgZone_engine") )
			m_enginePos = GetMemoryPointPos("dmgZone_engine");
		else
			m_enginePos = "0 0 0";

		if ( MemoryPointExists("dmgZone_front") )
			m_frontPos = GetMemoryPointPos("dmgZone_front");
		else
			m_frontPos = "0 0 0";

		if ( MemoryPointExists("dmgZone_back") )
			m_backPos = GetMemoryPointPos("dmgZone_back");
		else
			m_backPos = "0 0 0";		
				
		if ( MemoryPointExists("dmgZone_fender_1_1") )
			m_side_1_1Pos = GetMemoryPointPos("dmgZone_fender_1_1");
		else
			m_side_1_1Pos = "0 0 0";

		if ( MemoryPointExists("dmgZone_fender_1_2") )
			m_side_1_2Pos = GetMemoryPointPos("dmgZone_fender_1_2");
		else
			m_side_1_2Pos = "0 0 0";
		
		if ( MemoryPointExists("dmgZone_fender_2_1") )
			m_side_2_1Pos = GetMemoryPointPos("dmgZone_fender_2_1");
		else
			m_side_2_1Pos = "0 0 0";

		if ( MemoryPointExists("dmgZone_fender_2_2") )
			m_side_2_2Pos = GetMemoryPointPos("dmgZone_fender_2_2");
		else
			m_side_2_2Pos = "0 0 0";
	}
	
	override void EEInit()
	{		
		super.EEInit();

		if (GetGame().IsServer())
		{
			m_NoiseSystem = GetGame().GetNoiseSystem();
			if (m_NoiseSystem && !m_NoisePar)
			{
				m_NoisePar = new NoiseParams();
				m_NoisePar.LoadFromPath("cfgVehicles " + GetType() + " NoiseCarHorn");
			}
		}
	}
	
	vector GetEnginePosWS()
	{
		return ModelToWorld( m_DrownEnginePos );
	}
	
	vector GetCoolantPtcPosWS()
	{
		return ModelToWorld( m_coolantPtcPos );
	}

	vector GetRefillPointPosWS()
	{
		return ModelToWorld( m_fuelPos );
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
	
	override float GetLiquidThroughputCoef()
	{
		return LIQUID_THROUGHPUT_CAR_DEFAULT;
	}

	//here we should handle the damage dealt in OnContact event, but maybe we will react even in that event 
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		ForceUpdateLightsStart();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ForceUpdateLightsEnd, 100, false);

		SetEngineZoneReceivedHit(dmgZone == "Engine");
	}

	override void EEDelete(EntityAI parent)
	{
		#ifndef SERVER
		CleanupEffects();
		#endif
	}
	
	void ~CarScript()
	{
		#ifndef SERVER
		CleanupEffects();
		#endif
	}
	
	void CleanupEffects()
	{
		SEffectManager.DestroyEffect(m_coolantFx);
		SEffectManager.DestroyEffect(m_exhaustFx);
		SEffectManager.DestroyEffect(m_engineFx);
			
		if (m_Headlight)
			m_Headlight.Destroy();
			
		if (m_RearLight)
			m_RearLight.Destroy();
			
		SEffectManager.DestroyEffect(m_CrashSoundLight);
		SEffectManager.DestroyEffect(m_CrashSoundHeavy);
		SEffectManager.DestroyEffect(m_WindowSmall);
		SEffectManager.DestroyEffect(m_WindowLarge);
		CleanupSound(m_CarHornSoundEffect);
	}
	
	void CleanupSound(EffectSound sound)
	{
		SEffectManager.DestroyEffect(sound);
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
				
		if (GetCrashHeavySound())
		{
			PlayCrashHeavySound();
		}
		else if (GetCrashLightSound())
		{
			PlayCrashLightSound();
		}
		
		HandleCarHornSound(m_CarHornState);
		
		UpdateLights();
	}
	
	void CreateCarDestroyedEffect()
	{
		if ( !SEffectManager.IsEffectExist( m_enginePtcFx ) && GetGame().GetWaterDepth( GetEnginePosWS() ) <= 0 )
		{
			m_engineFx = new EffEngineSmoke();
			m_engineFx.SetParticleStateHeavy();
			m_enginePtcFx = SEffectManager.PlayOnObject( m_engineFx, this, m_enginePtcPos, Vector(0,0,0));
		}
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);

		switch (slot_name)
		{
		case "Reflector_1_1":
			if (GetGame().IsServer())
			{
				SetHealth("Reflector_1_1", "Health", item.GetHealth());
			}
		break;
		case "Reflector_2_1":
			if (GetGame().IsServer())
			{
				SetHealth("Reflector_2_1", "Health", item.GetHealth());
			}
		break;
		case "CarBattery":
		case "TruckBattery":
			if (GetGame().IsServer())
			{
				m_BatteryHealth = item.GetHealth01();
			}
		break;
		case "SparkPlug":
		case "GlowPlug":
			if (GetGame().IsServer())
			{
				m_PlugHealth = item.GetHealth01();
			}
		break;
		case "CarRadiator":
			if (GetGame().IsServer())
			{
				m_RadiatorHealth = item.GetHealth01();
			}
			
			m_Radiator = item;
		break;
		}
		
		if (GetGame().IsServer())
		{
			Synchronize();
		}

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
			if (GetGame().IsServer())
			{
				if (EngineIsOn())
				{
					EngineStop();
				}
				
				if (IsScriptedLightsOn())
				{
					ToggleHeadlights();
				}
			}
		break;
		case "SparkPlug":
		case "GlowPlug":
			m_PlugHealth = -1;
			if (GetGame().IsServer() && EngineIsOn())
			{
				EngineStop();
			}
		break;
		case "CarRadiator":
			m_Radiator = null;
			if (GetGame().IsServer())
			{
				LeakAll(CarFluid.COOLANT);

				if (m_DamageZoneMap.Contains("Radiator"))
				{
					SetHealth("Radiator", "Health", 0);
				}
			}
		break;
		}
		
		if (GetGame().IsServer())
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
				
			default:
				return false;
			}
		}

		return true;
	}
	
	protected bool CanManipulateSpareWheel(string slotSelectionName)
	{
		return false;
	}

	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		m_Time += timeSlice;

		if (GetGame().IsServer())
		{
			CheckContactCache();
			#ifdef DEVELOPER
			m_DebugMessageCleanTime += timeSlice;
			if (m_DebugMessageCleanTime >= DEBUG_MESSAGE_CLEAN_TIME_SECONDS)
			{
				m_DebugMessageCleanTime = 0;
				m_DebugContactDamageMessage = "";
			}
			#endif
		}
		
		if ( m_Time >= GameConstants.CARS_FLUIDS_TICK )
		{
			m_Time = 0;

			CarPartsHealthCheck();

			//First of all check if the car should stop the engine
			if (GetGame().IsServer() && EngineIsOn())
			{
				if ( GetFluidFraction(CarFluid.FUEL) <= 0 || m_EngineHealth <= 0 )
					EngineStop();

				CheckVitalItem(IsVitalCarBattery(), "CarBattery");
				CheckVitalItem(IsVitalTruckBattery(), "TruckBattery");
				CheckVitalItem(IsVitalSparkPlug(), "SparkPlug");
				CheckVitalItem(IsVitalGlowPlug(), "GlowPlug");
			}
			
			if (GetGame().IsServer())
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
			if ( EngineIsOn() )
			{
				if ( GetGame().IsServer() )
				{
					float dmg;

					if ( EngineGetRPM() >= EngineGetRPMRedline() )
					{
						if (EngineGetRPM() > EngineGetRPMMax())
							AddHealth( "Engine", "Health", -GetMaxHealth("Engine", "") * 0.05 ); //CAR_RPM_DMG
							
						dmg = EngineGetRPM() * 0.001 * Math.RandomFloat( 0.02, 1.0 );  //CARS_TICK_DMG_MIN; //CARS_TICK_DMG_MAX
						ProcessDirectDamage(DamageType.CUSTOM, null, "Engine", "EnviroDmg", vector.Zero, dmg);
						SetEngineZoneReceivedHit(true);
					}
					else
					{
						SetEngineZoneReceivedHit(false);
					}

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

					if ( IsVitalRadiator() )
					{
						if ( GetFluidFraction( CarFluid.COOLANT ) < 0.5 && GetFluidFraction( CarFluid.COOLANT ) >= 0 )
						{
							dmg = ( 1 - GetFluidFraction(CarFluid.COOLANT) ) * Math.RandomFloat( 0.02, 10.00 );  //CARS_DMG_TICK_MIN_COOLANT; //CARS_DMG_TICK_MAX_COOLANT
							AddHealth( "Engine", "Health", -dmg );
							SetEngineZoneReceivedHit(true);
						}
					}
				}
				
				//FX only on Client and in Single
				if (!GetGame().IsDedicatedServer())
				{
					if (!SEffectManager.IsEffectExist(m_exhaustPtcFx))
					{
						m_exhaustFx = new EffExhaustSmoke();
						m_exhaustPtcFx = SEffectManager.PlayOnObject( m_exhaustFx, this, m_exhaustPtcPos, m_exhaustPtcDir );
						m_exhaustFx.SetParticleStateLight();
					}
				
					if (IsVitalRadiator() && GetFluidFraction(CarFluid.COOLANT) < 0.5)
					{
						if (!SEffectManager.IsEffectExist(m_coolantPtcFx))
						{
							m_coolantFx = new EffCoolantSteam();
							m_coolantPtcFx = SEffectManager.PlayOnObject(m_coolantFx, this, m_coolantPtcPos, vector.Zero);
						}

						if (m_coolantFx)
						{
							if (GetFluidFraction(CarFluid.COOLANT) > 0)
								m_coolantFx.SetParticleStateLight();
							else
								m_coolantFx.SetParticleStateHeavy();
						}
					}
					else
					{
						if (SEffectManager.IsEffectExist(m_coolantPtcFx))
							SEffectManager.Stop(m_coolantPtcFx);
					}
				}
			}
			else
			{
				//FX only on Client and in Single
				if ( !GetGame().IsDedicatedServer() )
				{
					if (SEffectManager.IsEffectExist(m_exhaustPtcFx))
					{
						SEffectManager.Stop(m_exhaustPtcFx);
						m_exhaustPtcFx = -1;
					}
					
					if (SEffectManager.IsEffectExist(m_coolantPtcFx))
					{
						SEffectManager.Stop(m_coolantPtcFx);
						m_coolantPtcFx = -1;
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
	
	override void OnUpdate( float dt )
    {
		if ( GetGame().IsServer() )
		{
			ItemBase battery = GetBattery();
			if ( battery )
			{
				if ( EngineIsOn() )
				{
					m_BatteryTimer += dt;
					if ( m_BatteryTimer >= BATTERY_UPDATE_DELAY )
					{
						battery.GetCompEM().ConsumeEnergy(GetBatteryRechargeRate() * m_BatteryTimer);
						m_BatteryTimer = 0;
					}
				}
				else if ( !EngineIsOn() && IsScriptedLightsOn() )
				{
					m_BatteryTimer += dt;
					if ( m_BatteryTimer >= BATTERY_UPDATE_DELAY )
					{
						battery.GetCompEM().ConsumeEnergy(GetBatteryRuntimeConsumption() * m_BatteryTimer);
						m_BatteryTimer = 0;
						
						if ( battery.GetCompEM().GetEnergy() <= 0 )
						{
							ToggleHeadlights();
						}
					}
				}
			}
			
			if ( GetGame().GetWaterDepth( GetEnginePosWS() ) > 0 )
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
		
		if ( (!GetGame().IsDedicatedServer()) && m_ForceUpdateLights )
		{
			UpdateLights();
			m_ForceUpdateLights = false;
		}
    }
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		m_EngineDestroyed = true;
	}
	
	//! WARNING: Can be called very frequently in one frame, use with caution
	override void OnContact( string zoneName, vector localPos, IEntity other, Contact data )
	{
		if (GetGame().IsServer())
		{
			array<ref CarContactData> ccd;
			if (!m_ContactCache.Find(zoneName, ccd))
			{
				ccd = new array<ref CarContactData>;
				m_ContactCache.Insert(zoneName, ccd);
			}
			
			ccd.Insert(new CarContactData(localPos, other, data.Impulse));
		}
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
			array<ref CarContactData> data = m_ContactCache[zoneName];

			float dmg;
			
			int contactCount = data.Count();
			for (int j = 0; j < contactCount; ++j)
				dmg = data[j].impulse * m_dmgContactCoef;
		
			if ( dmg < GameConstants.CARS_CONTACT_DMG_MIN )
				continue;

			int pddfFlags;

			if (dmg < GameConstants.CARS_CONTACT_DMG_THRESHOLD)
			{				
				//Print(string.Format("[Vehiles:Damage]:: DMG %1 to the %2 zone is SMALL (threshold: %3), SPEED: %4", dmg, zoneName, GameConstants.CARS_CONTACT_DMG_THRESHOLD, GetSpeedometer() ));

				SynchCrashLightSound(true);
				pddfFlags = ProcessDirectDamageFlags.NO_TRANSFER;
			}
			else
			{
				//Print(string.Format("[Vehiles:Damage]:: DMG %1 to the %2 zone is BIG (threshold: %3), SPEED: %4", dmg, zoneName, GameConstants.CARS_CONTACT_DMG_THRESHOLD, GetSpeedometer() ));
				DamageCrew(dmg);
				SynchCrashHeavySound(true);
				pddfFlags = 0;
			}
			
			#ifdef DEVELOPER
			m_DebugContactDamageMessage += string.Format("%1: %2\n", zoneName, dmg);
			#endif
			
			ProcessDirectDamage(DT_CUSTOM, null, zoneName, "EnviroDmg", "0 0 0", dmg, pddfFlags);
		}
		
		UpdateHeadlightState();
		UpdateLights();
		
		m_ContactCache.Clear();
	}
	
	//! Responsible for damaging crew in a car crash
	void DamageCrew(float dmg)
	{
		for ( int c = 0; c < CrewSize(); ++c )
		{
			Human crew = CrewMember( c );
			if ( !crew )
				continue;

			PlayerBase player;
			if ( Class.CastTo(player, crew ) )
			{
				if ( dmg > GameConstants.CARS_CONTACT_DMG_KILLCREW )
				{		
					//Print(string.Format("KILL player: %1", dmg));
					player.SetHealth(0.0);
				}
				else
				{
					float shockTemp = Math.InverseLerp(GameConstants.CARS_CONTACT_DMG_THRESHOLD, GameConstants.CARS_CONTACT_DMG_KILLCREW, dmg);
					float shock = Math.Lerp( 50, 110, shockTemp );
					float hp = Math.Lerp( 2, 99, shockTemp );

					//Print(string.Format("[Vehiles:Damage]:: Damage to player: %1", hp));
					//Print(string.Format("[Vehiles:Damage]:: Shock to player: %1", shock));
					// These should ideally be ProcessDirectDamage...
					player.AddHealth("", "Shock", -shock );
					player.AddHealth("", "Health", -hp );
				}
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
		HandleDoorsSound(animSource, phase);
		#endif
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
			EffectSound sound;
			if (phase == 0)
			{
				sound = SEffectManager.PlaySound(m_CarDoorOpenSound, GetPosition());
			}
			else
			{
				sound = SEffectManager.PlaySound(m_CarDoorCloseSound, GetPosition());
			}
			
			if (sound)
			{
				sound.SetAutodestroy(true);
			}
		break;
		}
	}
	
	
	protected void HandleCarHornSound(ECarHornState pState)
	{
		switch (pState)
		{
		case ECarHornState.SHORT:
			PlaySoundSet(m_CarHornSoundEffect, m_CarHornShortSoundName, 0, 0);
		break;
		case ECarHornState.LONG:
			PlaySoundSet(m_CarHornSoundEffect, m_CarHornLongSoundName, 0, 0, true);
		break;
		default:
			CleanupSound(m_CarHornSoundEffect);
		break;
		}
	}
	
	void HandleEngineSound(CarEngineSoundState state)
	{
		#ifndef SERVER
		PlayerBase player = null;
		EffectSound sound = null;
		WaveKind waveKind = WaveKind.WAVEEFFECT;

		switch (state)
		{
		case CarEngineSoundState.START_OK:
			// play different sound based on selected camera
			if (Class.CastTo(player, CrewMember(0)))
			{
				if (!player.IsCameraInsideVehicle())
				{
					waveKind = WaveKind.WAVEEFFECTEX;
				}
	
				sound = SEffectManager.CreateSound(m_EngineStartOK, ModelToWorld(GetEnginePos()));
				if (sound)
				{
					sound.SetSoundWaveKind(waveKind);
					sound.SoundPlay();
					sound.SetAutodestroy(true);
				}
			}
			
			//! postpone the engine sound played from c++ on soundcontroller (via OnSound override)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SetEngineStarted, 1000, false, true);
		break;
			
		case CarEngineSoundState.START_NO_FUEL:
			SEffectManager.PlaySound("offroad_engine_failed_start_fuel_SoundSet", ModelToWorld(GetEnginePos()));
		break;
			
		case CarEngineSoundState.START_NO_BATTERY:
			SEffectManager.PlaySound("offroad_engine_failed_start_battery_SoundSet", ModelToWorld(GetEnginePos()));
		break;
			
		case CarEngineSoundState.START_NO_SPARKPLUG:
			SEffectManager.PlaySound("offroad_engine_failed_start_sparkplugs_SoundSet", ModelToWorld(GetEnginePos()));
		break;
			
		case CarEngineSoundState.STOP_OK:
		case CarEngineSoundState.STOP_NO_FUEL:
			// play different sound based on selected camera
			if (Class.CastTo(player, CrewMember(0)))
			{
				if (!player.IsCameraInsideVehicle())
				{
					waveKind = WaveKind.WAVEEFFECTEX;
				}
				
				sound = SEffectManager.CreateSound(m_EngineStopFuel, ModelToWorld(GetEnginePos()));
				if (sound)
				{
					sound.SetSoundWaveKind(waveKind);
					sound.SoundPlay();
					sound.SetAutodestroy(true);
				}
			}
		
			SetEngineStarted(false);
		break;
		}
		#endif
	}

	//! DEPRECATED	
	float GetEnviroHeatComfortOverride();

	/*!
		Gets called everytime when the specified vehicle's fluid
		changes its current value eg. when car is consuming fuel.

		This callback is called on both server and client.
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
		This callback is called on server only.

		\return true if the engine can start, false otherwise.
	*/
	override bool OnBeforeEngineStart()
	{
		EntityAI item = null;
		
		if (m_EngineStartDoOnce)
		{
			m_EngineStartDoOnce = false;
			return m_EngineBeforeStart;
		}
		
		if (GetFluidFraction(CarFluid.FUEL) <= 0)
		{
			HandleEngineSound(CarEngineSoundState.START_NO_FUEL);
			return false;
		}
		
		if (IsVitalCarBattery() || IsVitalTruckBattery())
		{
			item = GetBattery();
			if (!item || (item && (item.IsRuined() || item.GetCompEM().GetEnergy() < m_BatteryEnergyStartMin)))
			{
				m_EngineStartDoOnce = true;
				HandleEngineSound(CarEngineSoundState.START_NO_BATTERY);
				return false;
			}
		}

		if (IsVitalSparkPlug())
		{
			item = FindAttachmentBySlotName("SparkPlug");
			if (!item || (item && item.IsRuined()))
			{
				m_EngineStartDoOnce = true;
				HandleEngineSound(CarEngineSoundState.START_NO_SPARKPLUG);
				return false;
			}
		}
	
		if (IsVitalGlowPlug())
		{
			item = FindAttachmentBySlotName("GlowPlug");
			if (!item || (item && item.IsRuined()))
			{
				m_EngineStartDoOnce = true;
				HandleEngineSound(CarEngineSoundState.START_NO_SPARKPLUG);
				return false;
			}
		}
		
		/*
		if (!m_EngineBeforeStart)
		{
			m_EngineBeforeStart = true;
			m_EngineStartDoOnce = true;
			HandleEngineSound(CarEngineSoundState.NONE);
		}
		*/

		return true;
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
		if (GetGame().IsServer() && battery)
		{
			battery.GetCompEM().ConsumeEnergy(GetBatteryConsumption());
		}
		
		UpdateLights();
		
		HandleEngineSound(CarEngineSoundState.START_OK);
		
		m_EngineBeforeStart = false;
	}

	//! Gets called everytime the engine stops.
	override void OnEngineStop()
	{
		UpdateLights();
		
		HandleEngineSound(CarEngineSoundState.STOP_OK);
		SetEngineZoneReceivedHit(false);
		
		m_EngineBeforeStart = false;
	}

	/*!
		DEPRECATED Gets called everytime the game wants to switch the lights.
		\return true when lights can be switched, false otherwise.
	*/
	bool OnBeforeSwitchLights( bool toOn )
	{
		return true;
	}
	
	//! Propper way to get if light is swiched on. Use instead of IsLightsOn().
	bool IsScriptedLightsOn()
	{
		return m_HeadlightsOn;
	}
	
	//! Switches headlights on/off, including the illumination of the control panel and synchronizes this change to all clients.
	void ToggleHeadlights()
	{
		m_HeadlightsOn = !m_HeadlightsOn;
		SetSynchDirty();
	}
	
	void UpdateLights(int new_gear = -1)
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
		
		if (m_HeadlightsOn)
		{
			if (!m_Headlight && m_HeadlightsState != CarHeadlightBulbsState.NONE)
			{
				m_Headlight = CreateFrontLight();
			}
			
			if (m_Headlight)
			{
				switch (m_HeadlightsState)
				{
				case CarHeadlightBulbsState.LEFT:
					m_Headlight.AttachOnMemoryPoint(this, m_LeftHeadlightPoint, m_LeftHeadlightTargetPoint);
					m_Headlight.SegregateLight();
					break;
				case CarHeadlightBulbsState.RIGHT:
					m_Headlight.AttachOnMemoryPoint(this, m_RightHeadlightPoint, m_RightHeadlightTargetPoint);
					m_Headlight.SegregateLight();
					break;
				case CarHeadlightBulbsState.BOTH:
					vector local_pos_left = GetMemoryPointPos(m_LeftHeadlightPoint);
					vector local_pos_right = GetMemoryPointPos(m_RightHeadlightPoint);
					
					vector local_pos_middle = (local_pos_left + local_pos_right) * 0.5;
					m_Headlight.AttachOnObject(this, local_pos_middle);
					m_Headlight.AggregateLight();
					break;
				default:
					m_Headlight.FadeOut();
					m_Headlight = null;
				}
			}
		}
		else
		{
			if (m_Headlight)
			{
				m_Headlight.FadeOut();
				m_Headlight = null;
			}
		}
			
		// brakes
		if (m_BrakesArePressed)
		{
			switch (gear)
			{
			case CarGear.REVERSE:
			case CarAutomaticGearboxMode.R:
				m_RearLightType = CarRearLightType.BRAKES_AND_REVERSE;
				break;
			default:
				m_RearLightType = CarRearLightType.BRAKES_ONLY;
			}
			//Debug.Log(string.Format("m_BrakesArePressed=%1, m_RearLightType=%2", m_BrakesArePressed.ToString(), EnumTools.EnumToString(CarRearLightType, m_RearLightType)));
		}
		else
		{
			switch (gear)
			{
			case CarGear.REVERSE:
			case CarAutomaticGearboxMode.R:
				m_RearLightType = CarRearLightType.REVERSE_ONLY;
				break;
			default:
				m_RearLightType = CarRearLightType.NONE;
			}
			//Debug.Log(string.Format("m_BrakesArePressed=%1, m_RearLightType=%2", m_BrakesArePressed.ToString(), EnumTools.EnumToString(CarRearLightType, m_RearLightType)));
		}

		if (!m_RearLight && m_RearLightType != CarRearLightType.NONE && m_HeadlightsState != CarHeadlightBulbsState.NONE)
		{
			m_RearLight = CreateRearLight();
			vector local_pos = GetMemoryPointPos(m_ReverseLightPoint);
			m_RearLight.AttachOnObject(this, local_pos, "180 0 0");
		}

		// rear lights
		if (m_RearLight && m_RearLightType != CarRearLightType.NONE && m_HeadlightsState != CarHeadlightBulbsState.NONE)
		{
			switch (m_RearLightType)
			{
			case CarRearLightType.BRAKES_ONLY:
				m_RearLight.SetAsSegregatedBrakeLight();
				break;
			case CarRearLightType.REVERSE_ONLY:
				m_RearLight.SetAsSegregatedReverseLight();
				break;
			case CarRearLightType.BRAKES_AND_REVERSE:
				m_RearLight.AggregateLight();
				m_RearLight.SetFadeOutTime(1);
				break;
			default:
				m_RearLight.FadeOut();
				m_RearLight = null;
			}
		}
		else
		{
			if (m_RearLight)
			{
				m_RearLight.FadeOut();
				m_RearLight = null;
			}
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
		
		if (m_HeadlightsOn)
		{
			DashboardShineOn();
			TailLightsShineOn();
			
			switch (m_HeadlightsState)
			{
			case CarHeadlightBulbsState.LEFT:
				LeftFrontLightShineOn();
				RightFrontLightShineOff();
				break;
			case CarHeadlightBulbsState.RIGHT:
				LeftFrontLightShineOff();
				RightFrontLightShineOn();
				break;
			case CarHeadlightBulbsState.BOTH:
				LeftFrontLightShineOn();
				RightFrontLightShineOn();
				break;
			default:
				LeftFrontLightShineOff();
				RightFrontLightShineOff();
			}
			
			//Debug.Log(string.Format("m_HeadlightsOn=%1, m_HeadlightsState=%2", m_HeadlightsOn.ToString(), EnumTools.EnumToString(CarHeadlightBulbsState, m_HeadlightsState)));
		}
		else
		{
			TailLightsShineOff();
			DashboardShineOff();
			LeftFrontLightShineOff();
			RightFrontLightShineOff();
		}
			
		// brakes
		if (m_BrakesArePressed)
		{
			switch (gear)
			{
			case CarGear.REVERSE:
			case CarAutomaticGearboxMode.R:
				m_RearLightType = CarRearLightType.BRAKES_AND_REVERSE;
				break;
			default:
				m_RearLightType = CarRearLightType.BRAKES_ONLY;
			}

			//Debug.Log(string.Format("m_BrakesArePressed=%1, m_RearLightType=%2", m_BrakesArePressed.ToString(), EnumTools.EnumToString(CarRearLightType, m_RearLightType)));
		}
		else
		{
			switch (gear)
			{
			case CarGear.REVERSE:
			case CarAutomaticGearboxMode.R:
				m_RearLightType = CarRearLightType.REVERSE_ONLY;
				break;
			default:
				m_RearLightType = CarRearLightType.NONE;
			}

			//Debug.Log(string.Format("m_BrakesArePressed=%1, m_RearLightType=%2", m_BrakesArePressed.ToString(), EnumTools.EnumToString(CarRearLightType, m_RearLightType)));
		}
			
		
		// rear lights
		if (m_RearLightType != CarRearLightType.NONE && m_HeadlightsState != CarHeadlightBulbsState.NONE)
		{
			switch (m_RearLightType)
			{
			case CarRearLightType.BRAKES_ONLY:
				ReverseLightsShineOff();
				BrakeLightsShineOn();
				break;
			case CarRearLightType.REVERSE_ONLY:
				ReverseLightsShineOn();
				BrakeLightsShineOff();
				break;
			case CarRearLightType.BRAKES_AND_REVERSE:
				BrakeLightsShineOn();
				ReverseLightsShineOn();
				break;
			default:
				ReverseLightsShineOff();
			}
		}
		else
		{
			ReverseLightsShineOff();
			BrakeLightsShineOff();
		}
	}
	
	void LeftFrontLightShineOn()
	{
		string material = ConfigGetString("frontReflectorMatOn");
		
		if (material != "")
		{
			SetObjectMaterial(SELECTION_ID_FRONT_LIGHT_L, material);
		}
	}
	
	void RightFrontLightShineOn()
	{
		string material = ConfigGetString("frontReflectorMatOn");
		
		if (material != "")
		{
			SetObjectMaterial(SELECTION_ID_FRONT_LIGHT_R, material);
		}
	}
	
	void LeftFrontLightShineOff()
	{
		string material = ConfigGetString("frontReflectorMatOff");
		
		if (material != "")
		{
			SetObjectMaterial(SELECTION_ID_FRONT_LIGHT_L, material);
		}
	}
	
	void RightFrontLightShineOff()
	{
		string material = ConfigGetString("frontReflectorMatOff");
		
		if (material != "")
		{
			SetObjectMaterial(SELECTION_ID_FRONT_LIGHT_R, material);
		}
	}
	
	void ReverseLightsShineOn()
	{
		string material = ConfigGetString("ReverseReflectorMatOn");
		
		if (material != "")
		{
			SetObjectMaterial(SELECTION_ID_REVERSE_LIGHT_L, material);
			SetObjectMaterial(SELECTION_ID_REVERSE_LIGHT_R, material);
		}
	}
	
	void ReverseLightsShineOff()
	{
		string material = ConfigGetString("ReverseReflectorMatOff");
		
		if (material != "")
		{
			SetObjectMaterial(SELECTION_ID_REVERSE_LIGHT_L, material);
			SetObjectMaterial(SELECTION_ID_REVERSE_LIGHT_R, material);
		}
	}
	
	void BrakeLightsShineOn()
	{
		string material = ConfigGetString("brakeReflectorMatOn");
		
		if (material != "")
		{
			SetObjectMaterial(SELECTION_ID_BRAKE_LIGHT_L, material);
			SetObjectMaterial(SELECTION_ID_BRAKE_LIGHT_R, material);
		}
	}
	
	void BrakeLightsShineOff()
	{
		string material = ConfigGetString("brakeReflectorMatOff");
		
		if (material != "")
		{
			SetObjectMaterial(SELECTION_ID_BRAKE_LIGHT_L, material);
			SetObjectMaterial(SELECTION_ID_BRAKE_LIGHT_R, material);
		}
	}
	
	void TailLightsShineOn()
	{
		string material = ConfigGetString("TailReflectorMatOn");
		string material_off = ConfigGetString("TailReflectorMatOff");
		
		if (material != "" && material_off != "")
		{
			if (m_HeadlightsState == CarHeadlightBulbsState.LEFT)
			{
				SetObjectMaterial(SELECTION_ID_TAIL_LIGHT_L, material);
				SetObjectMaterial(SELECTION_ID_TAIL_LIGHT_R, material_off);
			}
			else if (m_HeadlightsState == CarHeadlightBulbsState.RIGHT)
			{
				SetObjectMaterial(SELECTION_ID_TAIL_LIGHT_L, material_off);
				SetObjectMaterial(SELECTION_ID_TAIL_LIGHT_R, material);
			}
			else if (m_HeadlightsState == CarHeadlightBulbsState.BOTH)
			{
				SetObjectMaterial(SELECTION_ID_TAIL_LIGHT_L, material);
				SetObjectMaterial(SELECTION_ID_TAIL_LIGHT_R, material);
			}
		}
	}
	
	void TailLightsShineOff()
	{
		string material = ConfigGetString("TailReflectorMatOff");
		
		if (material != "")
		{
			SetObjectMaterial(SELECTION_ID_TAIL_LIGHT_L, material);
			SetObjectMaterial(SELECTION_ID_TAIL_LIGHT_R, material);
		}
	}
	
	void DashboardShineOn()
	{
		string material = ConfigGetString("dashboardMatOn");
		
		if (material != "")
		{
			SetObjectMaterial(SELECTION_ID_DASHBOARD_LIGHT, material);
		}
	}
	
	void DashboardShineOff()
	{
		string material = ConfigGetString("dashboardMatOff");
		
		if (material != "")
		{
			SetObjectMaterial(SELECTION_ID_DASHBOARD_LIGHT, material);
		}
	}
	
	// Override this for a car-specific light type
	CarRearLightBase CreateRearLight()
	{
		return CarRearLightBase.Cast( ScriptedLightBase.CreateLight(OffroadHatchbackFrontLight) ); // just a default value
	}
	
	// Override this for a car-specific light type
	CarLightBase CreateFrontLight()
	{
		return CarLightBase.Cast( ScriptedLightBase.CreateLight(OffroadHatchbackFrontLight) ); // just a default value
	}
	
	protected void CheckVitalItem( bool isVital, string itemName )
	{
		if ( !isVital )
			return;

		EntityAI item = FindAttachmentBySlotName(itemName);

		if ( !item )
			EngineStop();
		else if ( item.IsRuined() )
			EngineStop();
	}

	protected void LeakFluid(CarFluid fluid)
	{
		float ammount = 0;
		
		switch (fluid)
		{
			case CarFluid.COOLANT:
				ammount = (1- m_RadiatorHealth) * Math.RandomFloat(0.02, 0.05);//CARS_LEAK_TICK_MIN; CARS_LEAK_TICK_MAX
				Leak(fluid, ammount);
			break;
			
			case CarFluid.OIL:
				ammount =  ( 1 - m_EngineHealth ) * Math.RandomFloat(0.02, 1.0);//CARS_LEAK_OIL_MIN; CARS_LEAK_OIL_MAX
				Leak(fluid, ammount);
			break;
			
			case CarFluid.FUEL:
				ammount = ( 1 - m_FuelTankHealth ) * Math.RandomFloat(0.02, 0.05);//CARS_LEAK_TICK_MIN; CARS_LEAK_TICK_MAX
				Leak(fluid, ammount);
			break;
		}
	}

	protected void CarPartsHealthCheck()
	{
		if (GetGame().IsServer())
		{
			if (HasRadiator())
			{
				m_RadiatorHealth = GetRadiator().GetHealth01("", "");
			}
			else
			{
				m_RadiatorHealth = 0;
			}
			
			m_EngineHealth 		= GetHealth01("Engine", "");
			m_FuelTankHealth 	= GetHealth01("FuelTank", "");
		}
	}
	
	bool GetCrashLightSound()
	{
		return m_PlayCrashSoundLight;
	}
	
	void SynchCrashLightSound(bool play)
	{
		if (m_PlayCrashSoundLight != play)
		{
			m_PlayCrashSoundLight = play;
			SetSynchDirty();
		}
	}

	void PlayCrashLightSound()
	{
		PlaySoundEx("offroad_hit_light_SoundSet", m_CrashSoundLight, m_PlayCrashSoundLight);
	}

	bool GetCrashHeavySound()
	{
		return m_PlayCrashSoundHeavy;
	}

	void SynchCrashHeavySound(bool play)
	{
		if (m_PlayCrashSoundHeavy != play)
		{
			m_PlayCrashSoundHeavy = play;
			SetSynchDirty();
		}
	}
	
	void PlayCrashHeavySound()
	{
		PlaySoundEx("offroad_hit_heavy_SoundSet", m_CrashSoundHeavy, m_PlayCrashSoundHeavy);
	}
	
	void PlaySoundEx(string soundset, EffectSound sound, out bool soundbool)
	{
		#ifndef SERVER
		if (!sound)
		{
			sound =	SEffectManager.PlaySoundCachedParams(soundset, GetPosition());
			if( sound )
			{
				sound.SetAutodestroy(true);
			}
		}
		else
		{
			if (!sound.IsSoundPlaying())
			{
				sound.SetPosition(GetPosition());
				sound.SoundPlay();
			}
		}
		
		soundbool = false;
		#endif
	}
	
	void PlaySound(string soundset, ref EffectSound sound, out bool soundbool)
	{
		PlaySoundEx(soundset, sound, soundbool);
	}
	
	string GetAnimSourceFromSelection( string selection )
	{
		return "";
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

	bool IsVitalCarBattery()
	{
		return true;
	}
	
	bool IsVitalTruckBattery()
	{
		return true;
	}
	
	bool IsVitalSparkPlug()
	{
		return true;
	}
	
	bool IsVitalGlowPlug()
	{
		return true;
	}
	
	bool IsVitalEngineBelt()
	{
		return true;
	}
	
	bool IsVitalRadiator()
	{
		return true;
	}
	
	bool IsVitalFuelTank()
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
		return -1;
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

	string GetActionCompNameFuel()
	{
		return "refill";
	}

	float GetActionDistanceFuel()
	{
		return 1.5;
	}
	
	string GetActionCompNameOil()
	{
		//return "refill";
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
		AddAction(ActionCarHornShort);
		AddAction(ActionCarHornLong);
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
			m_ActionsInitialize = false;
			return;
		}
		array<ActionBase_Basic> action_array = m_InputActionMap.Get(ai);
		
		if (!action_array)
		{
			action_array = new array<ActionBase_Basic>;
			m_InputActionMap.Insert(ai, action_array);
		}
		
		if ( LogManager.IsActionLogEnable() )
		{
			Debug.ActionLog(action.ToString() + " -> " + ai, this.ToString() , "n/a", "Add action" );
		}
		action_array.Insert(action);
	}
	
	void RemoveAction(typename actionName)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
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
		return ( GetGame().GetPlayer() && ( !GetGame().GetPlayer().GetCommand_Vehicle() || GetGame().GetPlayer().GetCommand_Vehicle().GetTransport() == this ) );
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
				CreateCarDestroyedEffect();
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
	
	void ForceUpdateLightsStart()
	{
		if (!m_ForceUpdateLights)
		{
			m_ForceUpdateLights = true;
			SetSynchDirty();
		}
	}
	
	void ForceUpdateLightsEnd()
	{
		if (m_ForceUpdateLights)
		{
			m_ForceUpdateLights = false;
			SetSynchDirty();
		}
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
		if (IsVitalCarBattery()) 
		{
			return ItemBase.Cast(FindAttachmentBySlotName("CarBattery"));
		}
		else if (IsVitalTruckBattery())
		{
			return ItemBase.Cast(FindAttachmentBySlotName("TruckBattery"));
		}
		
		return null;
	}
	
	void SetCarHornState(int pState)
	{
		m_CarHornState = pState;
		SetSynchDirty();
		
		if (GetGame().IsServer())
		{
			GenerateCarHornAINoise(pState);
		}
	}
	
	protected void GenerateCarHornAINoise(int pState)
	{
		if (pState != ECarHornState.OFF)
		{
			if (m_NoiseSystem && m_NoisePar)
			{
				float noiseMultiplier = 1.0;
				if (pState == ECarHornState.LONG)
					noiseMultiplier = 2.0;

				m_NoiseSystem.AddNoiseTarget(GetPosition(), 5, m_NoisePar, noiseMultiplier);
			}
		}
	}

	override vector GetDefaultHitPosition()
	{
		return vector.Zero;
	}

	void SetEngineZoneReceivedHit(bool pState)
	{
		m_EngineZoneReceivedHit = pState;
		SetSynchDirty();
	}
	
	bool HasEngineZoneReceivedHit()
	{
		return m_EngineZoneReceivedHit;
	}

#ifdef DEVELOPER
	override protected string GetDebugText()
	{
		string debug_output = "";
		if (GetGame().IsServer())
		{
			debug_output = m_DebugContactDamageMessage;
		}

		return debug_output;	
	}
#endif
}
