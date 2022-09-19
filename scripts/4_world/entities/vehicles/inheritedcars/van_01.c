class Van_01 extends CarScript
{
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaEngine m_UTSLEngine;

	void Van_01()
	{
		m_dmgContactCoef = 0.070; //TODO::Set proper value
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
	
	//-----------------------------------------------------------
	override int GetAnimInstance()
	{
		return VehicleAnimInstances.CIVVAN;
	}

	//-----------------------------------------------------------
	override int GetSeatAnimationType( int posIdx )
	{
		switch( posIdx )
		{
		case 0:
			return DayZPlayerConstants.VEHICLESEAT_DRIVER;
		case 1:
			return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
		case 2:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		case 3:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		case 4:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		}

		return 0;
	}

	//-----------------------------------------------------------	
	override int GetCarDoorsState( string slotType )
	{
		CarDoor carDoor;

		Class.CastTo( carDoor, FindAttachmentBySlotName( slotType ) );
		if ( !carDoor )
			return CarDoorState.DOORS_MISSING;
	
		switch( slotType )
		{
			case "Van_01_Door_1_1":
				if ( GetAnimationPhase("DoorsDriver") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;

			break;
			
			case "Van_01_Door_2_1":
				if ( GetAnimationPhase("DoorsCoDriver") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;

			break;
			
			case "Van_01_Door_2_2":
				if ( GetAnimationPhase("DoorsCargo1") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;

			break;
			
			case "Van_01_Trunk_1":
				if ( GetAnimationPhase("DoorsCargo3") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;
			break;
			
			case "Van_01_Trunk_2":
				if ( GetAnimationPhase("DoorsCargo2") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;

			break;
		}

		return CarDoorState.DOORS_MISSING;
	}

	//-----------------------------------------------------------
	override bool CrewCanGetThrough( int posIdx )
	{
		switch( posIdx )
		{
			case 0:
				if ( GetCarDoorsState( "Van_01_Door_1_1" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
			
			case 1:
				if ( GetCarDoorsState( "Van_01_Door_2_1" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;

			case 2:
				if ( GetCarDoorsState( "Van_01_Door_2_2" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;

			case 3:
				if ( GetCarDoorsState( "Van_01_Door_2_2" ) == CarDoorState.DOORS_CLOSED )
					return false;
			
			case 4:
				if ( GetCarDoorsState( "Van_01_Door_2_2" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		}

		return false;
	}

	//-----------------------------------------------------------
	override float OnSound( CarSoundCtrl ctrl, float oldValue )
	{
		switch ( ctrl )
		{
			case CarSoundCtrl.DOORS:
				float newValue = 0;
				
				//-----
				if ( GetCarDoorsState( "Van_01_Door_1_1" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;

				//-----
				if ( GetCarDoorsState( "Van_01_Door_2_1" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;
			
				//-----
				if ( GetCarDoorsState( "Van_01_Door_2_2" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;

				//-----
				if ( GetCarDoorsState( "Van_01_Trunk_1" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.125;

				//-----
				if ( GetCarDoorsState( "Van_01_Trunk_2" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.125;

				if ( newValue > 1 )
					newValue = 1;
			
				return newValue;
			break;
		}

		return oldValue;
	}

	//-----------------------------------------------------------
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
		case "doors_cargo3":
			return "DoorsCargo3";
		case "doors_hood":
			return "DoorsHood";
		}

		return "";
	}

	//-----------------------------------------------------------
	override string GetDoorConditionPointFromSelection( string selection )
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
			case "seat_cargo2":
			case "seat_cargo3":
				return "seat_con_1_2";
			break;
		}

		return "";
	};

	//-----------------------------------------------------------
	override bool IsVitalTruckBattery()
	{
		return false;
	}

	//-----------------------------------------------------------
	override bool IsVitalGlowPlug()
	{
		return false;
	}

	//-----------------------------------------------------------
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

	//-----------------------------------------------------------
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
}