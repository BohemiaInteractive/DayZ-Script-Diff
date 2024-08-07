class Boat_01_ColorBase : BoatScript
{
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaEngine m_UTSLEngine;

	void Boat_01()
	{
		//m_dmgContactCoef		= 0.0790;

		//m_EngineStartOK			= "Boat_01_engine_start_SoundSet";
		//m_EngineStartBattery	= "";
		//m_EngineStartPlug		= "Boat_01_engine_failed_start_sparkplugs_SoundSet";
		//m_EngineStartFuel		= "Boat_01_engine_failed_start_fuel_SoundSet";
		//m_EngineStopFuel		= "Boat_01_engine_stop_fuel_SoundSet";

		//SetEnginePos("0 0.7 -1.7");
	}
	
	override void EEInit()
	{		
		super.EEInit();
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
 			m_UTSSettings 						= new UniversalTemperatureSourceSettings();
			m_UTSSettings.m_ManualUpdate 		= true;
			m_UTSSettings.m_TemperatureMin		= 0;
			m_UTSSettings.m_TemperatureMax		= 30;
			m_UTSSettings.m_TemperatureItemCap 	= GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE;
			m_UTSSettings.m_TemperatureCap		= 0;
			m_UTSSettings.m_RangeFull			= 0.5;
			m_UTSSettings.m_RangeMax			= 2;
			
			m_UTSLEngine						= new UniversalTemperatureSourceLambdaEngine();
			m_UTSource							= new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLEngine);
		}
		
		SetAnimationPhase("ShowDamage",0);
		SetAnimationPhase("HideDamage",1);
	}
/*
	override void OnEngineStart()
	{
		super.OnEngineStart();

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.SetDefferedActive(true, 20.0);
		}
	}
	
	override void OnEngineStop()
	{
		super.OnEngineStop();

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.SetDefferedActive(false, 10.0);
		}
	}
*/
	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			if (m_UTSource.IsActive())
			{
				m_UTSource.Update(m_UTSSettings, m_UTSLEngine);
			}
		}
	}

	override int GetAnimInstance()
	{
		return VehicleAnimInstances.ZODIAC;
	}

	override int GetSeatAnimationType(int posIdx)
	{
		switch (posIdx)
		{
		case 0:
			return DayZPlayerConstants.VEHICLESEAT_DRIVER;
		case 1:
			return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
		case 2:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		case 3:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_R;
		}

		return 0;
	}

	
	override bool CanReleaseAttachment( EntityAI attachment )
	{
		if (!super.CanReleaseAttachment(attachment))
		{
			return false;
		}

		return true;
	}

	override bool CanDisplayAttachmentCategory(string category_name)
	{
		if ( !super.CanDisplayAttachmentCategory(category_name))
		{
			return false;
		}

		return true;
	}
	
	override bool CanDisplayCargo()
	{
		if ( !super.CanDisplayCargo() )
			return false;

		return true;
	}
/*
	override float OnSound(CarSoundCtrl ctrl, float oldValue)
	{
		return super.OnSound(ctrl, oldValue);
	}
*/
	override bool CanReachSeatFromSeat(int currentSeat, int nextSeat)
	{
		return true;
	}

	override void OnDamageDestroyed(int oldLevel)
	{
		SetAnimationPhase("ShowDamage",1);
		SetAnimationPhase("HideDamage",0);
	}

	override void OnDebugSpawn()
	{
		float amount = GetFluidCapacity(BoatFluid.FUEL);
		Fill(BoatFluid.FUEL, amount);

		//-----ATTACHMENTS
		GetInventory().CreateInInventory("Sparkplug");

	}
};

class Boat_01_Blue : Boat_01_ColorBase {};
class Boat_01_Orange : Boat_01_ColorBase {};
class Boat_01_Black : Boat_01_ColorBase {};
class Boat_01_Camo : Boat_01_ColorBase {};
