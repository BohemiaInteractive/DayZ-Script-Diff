class Offroad_02 extends CarScript
{
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaEngine m_UTSLEngine;

	void Offroad_02()
	{
		m_dmgContactCoef		= 0.0365;

		m_EngineStartOK			= "Offroad_02_engine_start_SoundSet";
		m_EngineStartBattery	= "Offroad_02_engine_failed_start_battery_SoundSet";
		m_EngineStartPlug		= "Offroad_02_engine_failed_start_sparkplugs_SoundSet";
		m_EngineStartFuel		= "Offroad_02_engine_failed_start_fuel_SoundSet";
		m_EngineStopFuel		= "offroad_engine_stop_fuel_SoundSet";

		m_CarDoorOpenSound		= "offroad_02_door_open_SoundSet";
		m_CarDoorCloseSound		= "offroad_02_door_close_SoundSet";
		
		m_CarHornShortSoundName = "Offroad_02_Horn_Short_SoundSet";
		m_CarHornLongSoundName	= "Offroad_02_Horn_SoundSet";
		
		SetEnginePos("0 0.7 1.7");
	}
	
	override void EEInit()
	{		
		super.EEInit();
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
 			m_UTSSettings 					= new UniversalTemperatureSourceSettings();
			m_UTSSettings.m_ManualUpdate 	= true;
			m_UTSSettings.m_TemperatureMin	= 0;
			m_UTSSettings.m_TemperatureMax	= 30;
			m_UTSSettings.m_RangeFull		= 0.5;
			m_UTSSettings.m_RangeMax		= 2;
			m_UTSSettings.m_TemperatureCap	= 25;
			
			m_UTSLEngine					= new UniversalTemperatureSourceLambdaEngine();
			m_UTSource						= new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLEngine);
		}		
	}
	
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
	
	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		super.EOnPostSimulate(other, timeSlice);
		
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
		return VehicleAnimInstances.HMMWV;
	}
	
	override float GetTransportCameraDistance()
	{
		return 5.1;
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

	// Override for car-specific light type
	override CarLightBase CreateFrontLight()
	{
		return CarLightBase.Cast(ScriptedLightBase.CreateLight(Offroad_02FrontLight));
	}
	
	// Override for car-specific light type
	override CarRearLightBase CreateRearLight()
	{
		return CarRearLightBase.Cast(ScriptedLightBase.CreateLight(Offroad_02RearLight));
	}
	
	override bool CanReleaseAttachment( EntityAI attachment )
	{
		if (!super.CanReleaseAttachment(attachment))
		{
			return false;
		}
		/*
		string attType = attachment.GetType();
		switch (attType)
		{
		case "CarBattery": 
			if (GetCarDoorsState("Offroad_02_Hood") == CarDoorState.DOORS_CLOSED || EngineIsOn())
			{
				return false;
			}
			break;

		case "GlowPlug":
			if (GetCarDoorsState("Offroad_02_Hood") == CarDoorState.DOORS_CLOSED || EngineIsOn())
			{
				return false;
			}
			break;

		case "CarRadiator":
			if (GetCarDoorsState("Offroad_02_Hood") == CarDoorState.DOORS_CLOSED || EngineIsOn())
			{
				return false;
			}
			break;
		}
		*/
		return true;
	}

	override protected bool CanManipulateSpareWheel(string slotSelectionName)
	{
		return GetCarDoorsState("Offroad_02_Trunk") != CarDoorState.DOORS_CLOSED;
	}

	override bool CanDisplayAttachmentCategory(string category_name)
	{
		if ( !super.CanDisplayAttachmentCategory(category_name))
		{
			return false;
		}
	
		/*
		category_name.ToLower();
		if (category_name.Contains("engine"))
		{
			if (GetCarDoorsState("Offroad_02_Hood") == CarDoorState.DOORS_CLOSED)
			{
				return false;
			}
		}
		*/

		return true;
	}
	
	override bool CanDisplayCargo()
	{
		if ( !super.CanDisplayCargo() )
			return false;
		
		if ( GetCarDoorsState("Offroad_02_Trunk") == CarDoorState.DOORS_CLOSED )
			return false;
		
		return true;
	}

	override int GetCarDoorsState(string slotType)
	{
		CarDoor carDoor;

		Class.CastTo( carDoor, FindAttachmentBySlotName( slotType ) );
		if (!carDoor)
		{
			return CarDoorState.DOORS_MISSING;
		}
	
		switch (slotType)
		{
		case "Offroad_02_Door_1_1":
			return TranslateAnimationPhaseToCarDoorState("DoorsDriver");
			
		case "Offroad_02_Door_2_1":
			return TranslateAnimationPhaseToCarDoorState("DoorsCoDriver");
		
		case "Offroad_02_Door_1_2":
			return TranslateAnimationPhaseToCarDoorState("DoorsCargo1");
		
		case "Offroad_02_Door_2_2":
			return TranslateAnimationPhaseToCarDoorState("DoorsCargo2");
		
		case "Offroad_02_Hood":
			return TranslateAnimationPhaseToCarDoorState("DoorsHood");

		case "Offroad_02_Trunk":
			return TranslateAnimationPhaseToCarDoorState("DoorsTrunk");
		}

		return CarDoorState.DOORS_MISSING;
	}
	

	override bool CrewCanGetThrough( int posIdx )
	{
		switch( posIdx )
		{
			case 0:
				if ( GetCarDoorsState( "Offroad_02_Door_1_1" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
			
			case 1:
				if ( GetCarDoorsState( "Offroad_02_Door_2_1" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;

			case 2:
				if ( GetCarDoorsState( "Offroad_02_Door_1_2" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;

			case 3:
				if ( GetCarDoorsState( "Offroad_02_Door_2_2" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		}

		return false;
	}

	override string GetDoorSelectionNameFromSeatPos(int posIdx)
	{
		switch( posIdx )
		{
		case 0:
			return "doors_driver";
		break;
		case 1:
			return "doors_codriver";
		break;
		case 2:
			return "doors_cargo1";
		break;
		case 3:
			return "doors_cargo2";
		break;
		}
		
		return super.GetDoorSelectionNameFromSeatPos(posIdx);
	}

	override string GetDoorInvSlotNameFromSeatPos(int posIdx)
	{
		switch( posIdx )
		{
		case 0:
			return "Offroad_02_Door_1_1";
		break;
		case 1:
			return "Offroad_02_Door_2_1";
		break;
		case 2:
			return "Offroad_02_Door_1_2";
		break;
		case 3:
			return "Offroad_02_Door_2_2";
		break;
		}
		
		return super.GetDoorInvSlotNameFromSeatPos(posIdx);
	}
	
	override float OnSound(CarSoundCtrl ctrl, float oldValue)
	{
		switch ( ctrl )
		{
		case CarSoundCtrl.DOORS:
			float newValue = 0;
			if (GetCarDoorsState("Offroad_02_Door_1_1") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.25;
			}

			if (GetCarDoorsState("Offroad_02_Door_2_1") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.25;
			}
		
			if (GetCarDoorsState("Offroad_02_Door_1_2") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.25;
			}

			if (GetCarDoorsState("Offroad_02_Door_2_2") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.25;
			}
			
			return Math.Clamp(newValue, 0, 1);
		break;
		}

		return super.OnSound(ctrl, oldValue);
	}
	
	override string GetAnimSourceFromSelection( string selection )
	{
		switch( selection )
		{
		case "doors_driver":
			return "DoorsDriver";
		case "doors_codriver":
			return "DoorsCoDriver";
		case "doors_cargo1":
			return "DoorsCargo1";
		case "doors_cargo2":
			return "DoorsCargo2";
		case "doors_hood":
			return "DoorsHood";
		case "doors_trunk":
			return "DoorsTrunk";
		}

		return "";
	}

	override bool CanReachSeatFromSeat( int currentSeat, int nextSeat )
	{
		//return false;
		switch( currentSeat )
		{
		case 0:
			if ( nextSeat == 1 )
				return true;
			break;
		case 1:
			if ( nextSeat == 0 )
				return true;
			break;
		case 2:
			if ( nextSeat == 3 )
				return true;
			break;
		case 3:
			if ( nextSeat == 2 )
				return true;
			break;
		}
		
		return false;
	}

	override bool CanReachDoorsFromSeat( string pDoorsSelection, int pCurrentSeat )
	{
		switch( pCurrentSeat )
		{
		case 0:
			if (pDoorsSelection == "DoorsDriver")
			{
				return true;
			}
		break;
		case 1:
			if (pDoorsSelection == "DoorsCoDriver")
			{
				return true;
			}
		break;
		case 2:
			if (pDoorsSelection == "DoorsCargo1")
			{
				return true;
			}
		break;
		case 3:
			if (pDoorsSelection == "DoorsCargo2")
			{
				return true;
			}
		break;
		}
		
		return false;		
	}
	
	override bool IsVitalTruckBattery()
	{
		return false;
	}

	override bool IsVitalSparkPlug()
	{
		return false;
	}

	override bool IsVitalRadiator()
	{
		return false;
	}

	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Offroad_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Offroad_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Offroad_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Offroad_02_Wheel" );

			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "GlowPlug" );

			//entity.GetInventory().CreateInInventory( "Offroad_02_Door_1_1" );
			entity.GetInventory().CreateInInventory( "Offroad_02_Door_1_2" );
			entity.GetInventory().CreateInInventory( "Offroad_02_Door_2_1" );
			entity.GetInventory().CreateInInventory( "Offroad_02_Door_2_2" );
			entity.GetInventory().CreateInInventory( "Offroad_02_Hood" );
			entity.GetInventory().CreateInInventory( "Offroad_02_Trunk" );

			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "GlowPlug" );
			
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Offroad_02_Wheel" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "GlowkPlug" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			//--
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			//--
			entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			ent = entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			if ( Class.CastTo(container, ent) )
			{
				container.SetLiquidType(LIQUID_WATER, true);
			}
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			if ( ent )
			{
				entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			}
		}

		Fill( CarFluid.FUEL, 120 );
	}
}
