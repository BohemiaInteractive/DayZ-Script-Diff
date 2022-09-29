class PowerGenerator extends ItemBase
{
	float						m_Fuel;
	private static float		m_FuelTankCapacity; // Capacity in ml.
	private static float		m_FuelToEnergyRatio; // Conversion ratio of 1 ml of fuel to X Energy
	private int					m_FuelPercentage;
	
	static const string			START_SOUND = "powerGeneratorTurnOn_SoundSet";
	static const string			LOOP_SOUND = "powerGeneratorLoop_SoundSet";
	static const string			STOP_SOUND = "powerGeneratorTurnOff_SoundSet";
	static const string 		SPARKPLUG_ATTACH_SOUND = "sparkplug_attach_SoundSet";
	static const string 		SPARKPLUG_DETACH_SOUND = "sparkplug_detach_SoundSet";
	
	protected EffectSound 		m_EngineLoop;
	protected EffectSound 		m_EngineStart;
	protected EffectSound 		m_EngineStop;
	ref Timer 					m_SoundLoopStartTimer;
	ref protected Effect 		m_Smoke;
	
	ItemBase	m_SparkPlug; 	//! DEPRECATED Attached spark plug item
	
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaEngine m_UTSLEngine;
	
	// Constructor
	void PowerGenerator()	
	{
		SetEventMask(EntityEvent.INIT); // Enable EOnInit event
		
		m_FuelPercentage = 50;
		RegisterNetSyncVariableInt("m_FuelPercentage");
		RegisterNetSyncVariableBool("m_IsSoundSynchRemote");
		RegisterNetSyncVariableBool("m_IsPlaceSound");
	}
	
	void ~PowerGenerator()
	{
		SEffectManager.DestroyEffect(m_Smoke);
	}
	
	override void EEInit()
	{		
		super.EEInit();
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
 			m_UTSSettings 					= new UniversalTemperatureSourceSettings();
			m_UTSSettings.m_ManualUpdate	= true;
			m_UTSSettings.m_TemperatureMin	= 0;
			m_UTSSettings.m_TemperatureMax	= 80;
			m_UTSSettings.m_RangeFull		= 1;
			m_UTSSettings.m_RangeMax		= 2.5;
			m_UTSSettings.m_TemperatureCap	= 8;
			
			m_UTSLEngine					= new UniversalTemperatureSourceLambdaEngine();
			m_UTSource						= new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLEngine);
		}		
	}
	
	override void EOnInit(IEntity other, int extra)
	{
		if (GetGame().IsServer())
		{
			m_FuelPercentage = GetCompEM().GetEnergy0To100();
			SetSynchDirty();
		}

		UpdateFuelMeter();
	}
	
	override float GetLiquidThroughputCoef()
	{
		return LIQUID_THROUGHPUT_GENERATOR;
	}
	
	// Play the loop sound
	void StartLoopSound()
	{
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
		{
			if (GetCompEM().IsWorking())
			{
				PlaySoundSetLoop(m_EngineLoop, LOOP_SOUND, 0, 0);
				
				// Particle
				vector local_pos = "0.3 0.21 0.4";
				vector local_ori = "270 0 0";
				m_Smoke = new EffGeneratorSmoke();
				SEffectManager.PlayOnObject(m_Smoke, this, local_pos, local_ori);
			}
		}
	}
	
	// Taking item into inventory
	override bool CanPutInCargo( EntityAI parent )
	{
		if (!super.CanPutInCargo(parent))
		{
			return false;
		}

		return CanManipulate();
	}

	// Taking item into inventory
	override bool CanPutIntoHands(EntityAI player)
	{
		if(!super.CanPutIntoHands(parent))
		{
			return false;
		}
		return CanManipulate();
	}

	// Returns true/false if this item can be moved into inventory/hands
	bool CanManipulate()
	{
		return GetCompEM().GetPluggedDevicesCount() == 0 && !GetCompEM().IsWorking();
	}
	
	/*===================================
					EVENTS
	===================================*/
	
	// Init
	override void OnInitEnergy()
	{
		m_FuelTankCapacity = GetGame().ConfigGetFloat ("CfgVehicles " + GetType() + " fuelTankCapacity");
		m_FuelToEnergyRatio = GetCompEM().GetEnergyMax() / m_FuelTankCapacity; // Conversion ratio of 1 ml of fuel to X Energy
		
		UpdateFuelMeter();
	}
	
	// Generator is working
	override void OnWorkStart()
	{
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
		{
			if (IsInitialized())
			{
				PlaySoundSet(m_EngineStart, START_SOUND, 0, 0);
			}
			
			if (!m_SoundLoopStartTimer)
			{
				m_SoundLoopStartTimer = new Timer(CALL_CATEGORY_SYSTEM);
			}
			
			if (!m_SoundLoopStartTimer.IsRunning()) // Makes sure the timer is NOT running already
			{
				m_SoundLoopStartTimer.Run(1.5, this, "StartLoopSound", NULL, false);
			}
		}
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.SetDefferedActive(true, 20.0);
		}
	}

	// Do work
	override void OnWork(float consumed_energy)
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.Update(m_UTSSettings, m_UTSLEngine);
		}

		if (GetGame().IsServer())
		{
			m_FuelPercentage = GetCompEM().GetEnergy0To100();
			SetSynchDirty();
		}
		
		UpdateFuelMeter();
	}

	// Turn off when this runs out of fuel
	override void OnWorkStop()
	{
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
		{
			// Sound
			PlaySoundSet(m_EngineStop, STOP_SOUND, 0, 0);
			StopSoundSet(m_EngineLoop);
			
			// particle
			SEffectManager.DestroyEffect(m_Smoke);
			
			// Fuel meter
			UpdateFuelMeter();
		}
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.SetDefferedActive(false, 20.0);
		}
	}
	
	// Called when this generator is picked up
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner) 
	{
		super.OnItemLocationChanged(old_owner, new_owner);
		UpdateFuelMeter();
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		ItemBase item_IB = ItemBase.Cast(item);
		
		if (item_IB.IsKindOf("Sparkplug") && IsInitialized())
		{
			ShowSelection("sparkplug_installed");
			
			#ifndef SERVER
			EffectSound sound = SEffectManager.PlaySound(SPARKPLUG_ATTACH_SOUND, GetPosition());
			sound.SetAutodestroy( true );
			#endif
		}
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		ItemBase item_IB = ItemBase.Cast(item);
		
		if (item_IB.IsKindOf("Sparkplug"))
		{
			HideSelection("sparkplug_installed");
			GetCompEM().SwitchOff();
			
			#ifndef SERVER
			EffectSound sound = SEffectManager.PlaySound(SPARKPLUG_DETACH_SOUND, GetPosition());
			sound.SetAutodestroy(true);
			#endif
		}
	}
	
	/*================================
				FUNCTIONS
	================================*/
	
	void UpdateFuelMeter()
	{
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
		{
			SetAnimationPhase("dial_fuel", m_FuelPercentage * 0.01);
		}
	}
	
	// Adds energy to the generator
	void SetFuel(float fuel_amount)
	{
		if (m_FuelTankCapacity > 0)
		{
			m_FuelToEnergyRatio = GetCompEM().GetEnergyMax() / m_FuelTankCapacity;
			GetCompEM().SetEnergy(fuel_amount * m_FuelToEnergyRatio);
			m_FuelPercentage = GetCompEM().GetEnergy0To100();
			SetSynchDirty();
			UpdateFuelMeter();
		}
		else
		{
			string error = string.Format("ERROR! Item %1 has fuel tank with 0 capacity! Add parameter 'fuelTankCapacity' to its config and set it to more than 0!", this.GetType());
			DPrint(error);
		}
	}

	// Adds fuel (energy) to the generator
	// Returns how much fuel was accepted
	float AddFuel(float available_fuel)
	{
		if (available_fuel == 0)
		{
			return 0;
		}
		
		float needed_fuel = GetMaxFuel() - GetFuel();
		
		if (needed_fuel > available_fuel)
		{
			SetFuel(GetFuel() + available_fuel);
			return available_fuel; // Return used fuel amount
		}
		else
		{
			SetFuel(GetMaxFuel());
			return needed_fuel;
		}
	}

	// Check the bottle if it can be used to fill the tank
	bool CanAddFuel(ItemBase container)
	{
		if (container)
		{
			// Get the liquid
			int liquid_type	= container.GetLiquidType();
			
			// Do all checks
			if ( container.GetQuantity() > 0 && GetCompEM().GetEnergy() < GetCompEM().GetEnergyMax() && (liquid_type & LIQUID_GASOLINE))
			{
				return true;
			}
		}
		
		return false;
	}

	// Returns fuel amount
	float GetFuel()
	{
		return GetCompEM().GetEnergy() / m_FuelToEnergyRatio;
	}

	// Returns max fuel amount
	float GetMaxFuel()
	{
		return m_FuelTankCapacity;
	}

	// Checks sparkplug
	bool HasSparkplug()
	{
		int slot = InventorySlots.GetSlotIdFromString("SparkPlug");
		EntityAI ent = GetInventory().FindAttachment(slot);

		return ent && !ent.IsRuined();
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
				
		UpdateFuelMeter();
		
		if (IsPlaceSound())
		{
			PlayPlaceSound();
		}
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
			
		SetIsPlaceSound(true);
	}
	
	override string GetPlaceSoundset()
	{
		return "placePowerGenerator_SoundSet";
	}
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionTogglePlaceObject);
		AddAction(ActionPullOutPlug);
		AddAction(ActionTurnOnPowerGenerator);
		AddAction(ActionTurnOffPowerGenerator);
		AddAction(ActionPlaceObject);
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		EntityAI entity;
		if (Class.CastTo(entity, this))
		{
			entity.GetInventory().CreateInInventory("SparkPlug");
		}
		
		SetFuel(GetMaxFuel());
	}
}
