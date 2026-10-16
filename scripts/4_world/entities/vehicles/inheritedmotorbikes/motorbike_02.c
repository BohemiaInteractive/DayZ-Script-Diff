class Motorbike_02_ColorBase : MotorbikeScript
{
	void Motorbike_02_ColorBase()
	{
		m_VehicleContactDamageCoef = 0.035;
		m_CrewContactDamageCoef = 0.012;
		
		m_EngineStarting 		= "";
		m_EngineStartOK 		= "Motorbike_02_engine_start_SoundSet";
		m_EngineStartBattery 	= "";
		m_EngineStartPlug 		= "";
		m_EngineStartFuel 		= "Motorbike_02_engine_failed_start_fuel_SoundSet";
		m_EngineStop	 		= "Motorbike_02_engine_stop_SoundSet";
		m_EngineStopFuel 		= "Motorbike_02_engine_stop_fuel_SoundSet";
		

		m_LightsComponent.RegisterLight("Front", new VehicleLightData(new Motorbike_02LightProfileFront()));
		m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new Motorbike_02LightProfileRear()));
		
		m_LightsComponent.RegisterSelection("BrakeOn",	{new VehicleLightSelectionData(2, "brakeReflectorMatOn")});
		m_LightsComponent.RegisterSelection("BrakeOff",	{new VehicleLightSelectionData(2, "brakeReflectorMatOff")});

		m_HornComponent.RegisterSound(VehicleHornMode.SHORT, 	"Motorbike_02_Horn_Short_SoundSet");
		m_HornComponent.RegisterSound(VehicleHornMode.LONG, 	"Motorbike_02_Horn_SoundSet");

		SetEnginePos("0 0.5 -0.08");
	}
	
	override bool HasGangedTailAndBrakeLights()
	{
		return true;
	}

	override bool NeedElectricitySourceDevice()
	{
		return false;
	}

	override float GetTransportCameraDistance()
	{
		return 3.0;
	}
	
	override vector GetTransportCameraOffset()
	{
		return "0 1.4 -0.3";
	}
	
	override int GetAnimInstance()
	{
		return VehicleAnimInstances.MOTO2;
	}
	
	override bool HasDirectionalInOutAction()
	{
		return true;
	}
	
	override int GetSeatAnimationTypeDirectional(int posIdx, int directionIndex)
	{
		if (posIdx == 0)
		{
			if (directionIndex == 0)
				return 0;
			else
				return 1;
		}
		
		return 0;
	}
	
	override bool IsVitalGlowPlug()
	{
		return false;
	}
	
	override float GetWeightCoef()
	{
		return 0.75;
	}
	
	override protected void HandleHeatComfortOverrides(string slotName, notnull EntityAI item, bool detach = false)
	{
		MotorbikePart part = MotorbikePart.Cast(item);
		if (part)
		{		
			float addOrRemove = 1;
			if (detach)
				addOrRemove = -1;

			switch (slotName)
			{
				case "Motorbike_02_ShieldFront":
				case "Motorbike_02_ShieldLeft":
				case "Motorbike_02_ShieldRight":
					m_HeatComfortOverride += part.GetHeatComfortOverride() * addOrRemove;
					break;
			}
		}
		
		#ifdef DIAG_DEVELOPER
		if (m_HeatComfortOverride > GameConstants.ENVIRO_HEATCOMFORT_MOTORBIKE_TARGET_MAX)
			ErrorEx("m_HeatComfortOverride value is higher than GameConstants.ENVIRO_HEATCOMFORT_MOTORBIKE_TARGET_MAX, check config");
		#endif
		
		m_HeatComfortOverride = Math.Clamp(m_HeatComfortOverride, 0.0, GameConstants.ENVIRO_HEATCOMFORT_MOTORBIKE_TARGET_MAX);
	}
	
	// --------------------------------------------------------------------------------
	
	override protected void PlayCrashLightSound()
	{
		PlaySoundEx("Motorbike_02_hit_character_SoundSet", m_CrashSoundLight, m_PlayCrashSoundLight);
	}

	// --------------------------------------------------------------------------------

	override protected void PlayCrashHeavySound()
	{
		PlaySoundEx("Motorbike_02_hit_heavy_SoundSet", m_CrashSoundHeavy, m_PlayCrashSoundHeavy);
	}
	
	// --------------------------------------------------------------------------------
	
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();

		float amount = GetFluidCapacity(MotorbikeFluid.FUEL);
		Fill(MotorbikeFluid.FUEL, amount);

		//-----ATTACHMENTS
		GetInventory().CreateInInventory("Motorbike_02_Wheel_1");
		GetInventory().CreateInInventory("Motorbike_02_Wheel_2");
		
		GetInventory().CreateInInventory("Motorbike_02_ShieldFront_Red");
		GetInventory().CreateInInventory("Motorbike_02_ShieldLeft_Red");
		GetInventory().CreateInInventory("Motorbike_02_ShieldRight_Red");
	}
}

class Motorbike_02_Red : Motorbike_02_ColorBase {};