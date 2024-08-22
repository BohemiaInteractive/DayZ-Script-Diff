class GetOutTransportActionData : ActionData
{
	Transport m_Vehicle;
	float m_Speed;
	float m_JumpingOutThreshold;
	bool m_WasJumpingOut 	= false;
	
	// obsolete
	Car m_Car;
	float m_CarSpeed;
	vector m_StartLocation;
	float m_DmgTaken; 
	float m_ShockTaken;
}

class ActionGetOutTransport : ActionBase
{
	//For the two following values -> The HIGHER the value, the LOWER the output
	const int DMG_FACTOR = 60; 				//value used to translate impact strength into actual damage (impact strength -> velocity squared)
	const int SHOCK_FACTOR = 15; 			//Value used to translate impact strength into actual shock

	//Variables used to determine the different speed levels for bleeding checks
	const int LOW_SPEED_VALUE 			= 20;
	const int HIGH_SPEED_VALUE			= 30;
	
	protected const float CAR_JUMPOUT_THRESHOLD 	= 8.0;	// speed in km/h at which leaving the vehicle is considered a jump out
	protected const float BOAT_JUMPOUT_THRESHOLD 	= 1.0;
	
	const int HEALTH_LOW_SPEED_VALUE 	= 30;
	const int HEALTH_HIGH_SPEED_VALUE 	= 70;
	
	void ActionGetOutTransport()
	{
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
		m_Text		 = "#leave_vehicle";
	}
	
	override ActionData CreateActionData()
	{
		ActionData data = new GetOutTransportActionData();
		return data;
	}


	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNone();
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool HasProgress()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			if (trans)
			{
				int crewIndex = trans.CrewMemberIndex(player);
				return crewIndex >= 0 && trans.CrewCanGetThrough(crewIndex) && trans.IsAreaAtDoorFree(crewIndex);
			}
		}

		return false;
	}

	void ProcessGetOutTransportActionData(Transport veh, GetOutTransportActionData got_action_data)
	{
		float speed;
		if (Car.Cast(veh))
		{
			got_action_data.m_JumpingOutThreshold = CAR_JUMPOUT_THRESHOLD;
			speed = Car.Cast(veh).GetSpeedometerAbsolute();
		}
		else if (Boat.Cast(veh))
		{
			got_action_data.m_JumpingOutThreshold = BOAT_JUMPOUT_THRESHOLD;
			vector velocity = dBodyGetVelocityAt(veh, veh.GetPosition());
			speed = velocity.Normalize();
		}

		got_action_data.m_Speed = speed;
		got_action_data.m_WasJumpingOut = speed > got_action_data.m_JumpingOutThreshold;
		got_action_data.m_Vehicle = veh;
	}

	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);

		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			if (trans)
			{
				GetOutTransportActionData gotActionData = GetOutTransportActionData.Cast(action_data);
				ProcessGetOutTransportActionData(trans, gotActionData);

				if (!gotActionData.m_WasJumpingOut)
					vehCommand.GetOutVehicle();
				else
					vehCommand.JumpOutVehicle();
				
				if (Car.Cast(trans))
					GetDayZGame().GetBacklit().OnLeaveCar();

				if (action_data.m_Player.GetInventory())
					action_data.m_Player.GetInventory().LockInventory(LOCK_FROM_SCRIPT);
			}
		}
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport trans = vehCommand.GetTransport();
			if (trans)
			{
				CarScript car;
				if (Class.CastTo(car, trans))
					car.ForceUpdateLightsStart();
			}
		}
	}
	
	void Unhide(PlayerBase player);

	override void OnUpdate(ActionData action_data)
	{
		if (action_data.m_State == UA_START)
		{
			if (!action_data.m_Player.GetCommand_Vehicle())
				End(action_data);
		}
	}
	
	override bool CanBeUsedInRestrain()
	{
		return true;
	}
	
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
	
	override int GetActionCategory()
	{
		return AC_INTERACT;
	}
	
	override void OnEnd(ActionData action_data)
	{
		if (action_data.m_Player.GetInventory()) 
			action_data.m_Player.GetInventory().UnlockInventory(LOCK_FROM_SCRIPT);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		GetOutTransportActionData gotActionData = GetOutTransportActionData.Cast(action_data);
		
		if (gotActionData.m_WasJumpingOut)
		{
			gotActionData.m_Player.OnJumpOutVehicleFinish(gotActionData.m_Speed);
			
			ApplyJumpOutDmg(action_data);
		}

		CarScript car = CarScript.Cast(gotActionData.m_Vehicle);
		if (car)
			car.ForceUpdateLightsEnd();
	}
	
	//Manage all jumping out of vehicle damage logic
	void ApplyJumpOutDmg(ActionData action_data)
	{		
		GetOutTransportActionData gotActionData = GetOutTransportActionData.Cast(action_data);
		
		CarScript car;
		if (Class.CastTo(car, gotActionData.m_Vehicle))
			car.OnVehicleJumpOutServer(gotActionData);
		
		BoatScript boat;
		if (Class.CastTo(boat, gotActionData.m_Vehicle))
			boat.OnVehicleJumpOutServer(gotActionData);
	}
	
	// deprecated
	int m_DmgFactor;
	int m_ShockFactor;
	
	void ProcessGetOutActionData(Car car, GetOutTransportActionData got_action_data);
}
