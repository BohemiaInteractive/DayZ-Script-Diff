class Motorbike_01_ColorBase : MotorbikeScript
{	
	void Motorbike_01_ColorBase()
	{
		m_VehicleContactDamageCoef = 0.03;
		m_CrewContactDamageCoef = 0.018;
		
		m_EngineStarting 		= "";
		m_EngineStartOK 		= "Motorbike_01_engine_start_SoundSet";
		m_EngineStartBattery 	= "";
		m_EngineStartPlug 		= "";
		m_EngineStartFuel 		= "Motorbike_01_engine_failed_start_fuel_SoundSet";
		m_EngineStop	 		= "Motorbike_01_engine_stop_SoundSet";
		m_EngineStopFuel 		= "Motorbike_01_engine_stop_fuel_SoundSet";
		
		m_LightsComponent.RegisterLight("Front", new VehicleLightData(new Motorbike_01LightProfileFront()));
		m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new Motorbike_01LightProfileRear()));
		
		m_LightsComponent.RegisterSelection("FrontOn",	{new VehicleLightSelectionData(3, "frontReflectorMatOn")});
		m_LightsComponent.RegisterSelection("FrontOff",	{new VehicleLightSelectionData(3, "frontReflectorMatOff")});
		m_LightsComponent.RegisterSelection("BrakeOn",	{new VehicleLightSelectionData(2, "brakeReflectorMatOn")});
		m_LightsComponent.RegisterSelection("BrakeOff",	{new VehicleLightSelectionData(2, "brakeReflectorMatOff")});
		
		m_HornComponent.RegisterSound(VehicleHornMode.SHORT, 	"Motorbike_01_Horn_Short_SoundSet");
		m_HornComponent.RegisterSound(VehicleHornMode.LONG, 	"Motorbike_01_Horn_SoundSet");
		
		SetEnginePos("0 0.3 -0.06");
	}
	
	override bool NeedElectricitySourceDevice()
	{
		return false;
	}
	
	override float GetTransportCameraDistance()
	{
		return 2.5;
	}
	
	override vector GetTransportCameraOffset()
	{
		return "0 1.3 -0.25";
	}
	
	override int GetAnimInstance()
	{
		return VehicleAnimInstances.MOTO1;
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
	
	override string GetAnimSourceFromSelection(string selection)
	{
		switch (selection)
		{
			case "seat_driver":
				return "SeatDriver";
		}

		return super.GetAnimSourceFromSelection(selection);
	}
	
	override bool IsVitalGlowPlug()
	{
		return false;
	}

	override float GetWeightCoef()
	{
		return 0.5;
	}

	override bool CanGetIn()
	{
		if (!super.CanGetIn())
			return false;

		return GetAnimationPhase("SeatDriver") == 0.0;
	}

	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionAnimateMotorbikeSeat);
	}
	
	override void OnAnimationPhaseStarted(string animSource, float phase)
	{
		super.OnAnimationPhaseStarted(animSource, phase);

		#ifndef SERVER
		#ifndef NO_GUI
		switch (animSource)
		{
			case "seatdriver":
				EffectSound sound = CreateSoundForAnimationSource(animSource);
				if (phase == 0.0)
				{
					sound.SetSoundSet("Motorbike_01_Seat_FlipUp_SoundSet");
				}
				else
				{
					sound.SetSoundSet("Motorbike_01_Seat_FlipDown_SoundSet");
				}
	
				sound.SetSoundFadeIn(0.1);
				sound.SetSoundFadeOut(0.1);
				
				SEffectManager.EffectRegister(sound);
				sound.SoundPlay();

				break;
		}
		#endif
		#endif
	}
	
	
	// --------------------------------------------------------------------------------
	
	override protected void PlayCrashLightSound()
	{
		PlaySoundEx("Motorbike_01_hit_character_SoundSet", m_CrashSoundLight, m_PlayCrashSoundLight);
	}

	// --------------------------------------------------------------------------------

	override protected void PlayCrashHeavySound()
	{
		PlaySoundEx("Motorbike_01_hit_heavy_SoundSet", m_CrashSoundHeavy, m_PlayCrashSoundHeavy);
	}
	
	// --------------------------------------------------------------------------------

	override void OnDebugSpawn()
	{
		SpawnUniversalParts();

		float amount = GetFluidCapacity(MotorbikeFluid.FUEL);
		Fill(MotorbikeFluid.FUEL, amount);

		//-----ATTACHMENTS
		GetInventory().CreateInInventory("Motorbike_01_Wheel_1");
		GetInventory().CreateInInventory("Motorbike_01_Wheel_2");
	}
}


class Motorbike_01_Blue : Motorbike_01_ColorBase {};
class Motorbike_01_Yellow : Motorbike_01_ColorBase {};
class Motorbike_01_Red : Motorbike_01_ColorBase {};
