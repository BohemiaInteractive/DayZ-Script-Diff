class Hatchback_02 extends CarScript
{
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaEngine m_UTSLEngine;

	void Hatchback_02()
	{
		m_dmgContactCoef		= 0.070;

		m_EngineStartOK			= "Hatchback_02_engine_start_SoundSet";
		m_EngineStartBattery	= "Hatchback_02_engine_failed_start_battery_SoundSet";
		m_EngineStartPlug		= "Hatchback_02_engine_failed_start_sparkplugs_SoundSet";
		m_EngineStartFuel		= "Hatchback_02_engine_failed_start_fuel_SoundSet";
		m_EngineStopFuel		= "offroad_engine_stop_fuel_SoundSet";
		
		m_CarDoorOpenSound		= "offroad_door_open_SoundSet";
		m_CarDoorCloseSound		= "offroad_door_close_SoundSet";
		
		m_CarHornShortSoundName = "Hatchback_02_Horn_Short_SoundSet";
		m_CarHornLongSoundName	= "Hatchback_02_Horn_SoundSet";
		
		SetEnginePos("0 0.7 1.4");
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
		return VehicleAnimInstances.GOLF;
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
	override CarRearLightBase CreateRearLight()
	{
		return CarRearLightBase.Cast(ScriptedLightBase.CreateLight(Hatchback_02RearLight));
	}
	
	// Override for car-specific light type
	override CarLightBase CreateFrontLight()
	{
		return CarLightBase.Cast(ScriptedLightBase.CreateLight(Hatchback_02FrontLight));
	}

	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (!super.CanReleaseAttachment(attachment))
		{
			return false;
		}
		
		if (EngineIsOn() || GetCarDoorsState("Hatchback_02_Hood") == CarDoorState.DOORS_CLOSED)
		{
			string attType = attachment.GetType();
			if (attType == "CarRadiator" || attType == "CarBattery" || attType == "SparkPlug")
			{
				return false;
			}
		}

		return true;
	}
	
	override protected bool CanManipulateSpareWheel(string slotSelectionName)
	{
		return GetCarDoorsState("Hatchback_02_Trunk") != CarDoorState.DOORS_CLOSED;
	}

	override bool CanDisplayAttachmentCategory( string category_name )
	{
		//super
		if ( !super.CanDisplayAttachmentCategory( category_name ) )
		return false;
		//
	
		category_name.ToLower();		
		if ( category_name.Contains( "engine" ) )
		{
			if ( GetCarDoorsState("Hatchback_02_Hood") == CarDoorState.DOORS_CLOSED )
				return false;
		}
				
		return true;
	}

	override bool CanDisplayCargo()
	{
		if ( !super.CanDisplayCargo() )
			return false;
		
		if ( GetCarDoorsState("Hatchback_02_Trunk") == CarDoorState.DOORS_CLOSED )
			return false;
		
		return true;
	}

	override int GetCarDoorsState(string slotType)
	{
		CarDoor carDoor;

		Class.CastTo(carDoor, FindAttachmentBySlotName(slotType));
		if (!carDoor)
		{
			return CarDoorState.DOORS_MISSING;
		}
	
		switch (slotType)
		{
		case "Hatchback_02_Door_1_1":
			return TranslateAnimationPhaseToCarDoorState("DoorsDriver");
			
		case "Hatchback_02_Door_2_1":
			return TranslateAnimationPhaseToCarDoorState("DoorsCoDriver");
		
		case "Hatchback_02_Door_1_2":
			return TranslateAnimationPhaseToCarDoorState("DoorsCargo1");
		
		case "Hatchback_02_Door_2_2":
			return TranslateAnimationPhaseToCarDoorState("DoorsCargo2");
		
		case "Hatchback_02_Hood":
			return TranslateAnimationPhaseToCarDoorState("DoorsHood");
			
		case "Hatchback_02_Trunk":
			return TranslateAnimationPhaseToCarDoorState("DoorsTrunk");
		}

		return CarDoorState.DOORS_MISSING;
	}
	

	override bool CrewCanGetThrough(int posIdx)
	{
		switch (posIdx)
		{
		case 0:
			if (GetCarDoorsState("Hatchback_02_Door_1_1") == CarDoorState.DOORS_CLOSED)
			{
				return false;
			}

			return true;
		
		case 1:
			if (GetCarDoorsState("Hatchback_02_Door_2_1") == CarDoorState.DOORS_CLOSED)
			{
				return false;
			}

			return true;

		case 2:
			if (GetCarDoorsState("Hatchback_02_Door_1_2") == CarDoorState.DOORS_CLOSED)
			{
				return false;
			}

			return true;

		case 3:
			if (GetCarDoorsState("Hatchback_02_Door_2_2") == CarDoorState.DOORS_CLOSED)
			{
				return false;
			}

			return true;
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
			return "Hatchback_02_Door_1_1";
		break;
		case 1:
			return "Hatchback_02_Door_2_1";
		break;
		case 2:
			return "Hatchback_02_Door_1_2";
		break;
		case 3:
			return "Hatchback_02_Door_2_2";
		break;
		}
		
		return super.GetDoorInvSlotNameFromSeatPos(posIdx);
	}
	
	override float OnSound(CarSoundCtrl ctrl, float oldValue)
	{
		switch (ctrl)
		{
		case CarSoundCtrl.DOORS:
			float newValue = 0;
			if (GetCarDoorsState("Hatchback_02_Door_1_1") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.25;
			}

			if (GetCarDoorsState("Hatchback_02_Door_2_1") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.25;
			}
		
			if (GetCarDoorsState("Hatchback_02_Door_1_2") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.25;
			}

			if (GetCarDoorsState("Hatchback_02_Door_2_2") == CarDoorState.DOORS_CLOSED)
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


	override bool IsVitalTruckBattery()
	{
		return false;
	}

	override bool IsVitalGlowPlug()
	{
		return false;
	}
	
	override bool CanReachSeatFromSeat( int currentSeat, int nextSeat )
	{
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

	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );

			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );

			//entity.GetInventory().CreateInInventory( "Hatchback_02_Door_1_1" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Door_1_2" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Door_2_1" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Door_2_2" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Hood" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Trunk" );

			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
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

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
}

class Hatchback_02_Black extends Hatchback_02
{
	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );

			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );

			//entity.GetInventory().CreateInInventory( "Hatchback_02_Door_1_1_Black" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Door_1_2_Black" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Door_2_1_Black" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Door_2_2_Black" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Hood_Black" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Trunk_Black" );

			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
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

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
};

class Hatchback_02_Blue extends Hatchback_02
{
	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );


			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );

			//entity.GetInventory().CreateInInventory( "Hatchback_02_Door_1_1_Blue" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Door_1_2_Blue" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Door_2_1_Blue" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Door_2_2_Blue" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Hood_Blue" );
			entity.GetInventory().CreateInInventory( "Hatchback_02_Trunk_Blue" );

			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );

			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Hatchback_02_Wheel" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
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

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
};