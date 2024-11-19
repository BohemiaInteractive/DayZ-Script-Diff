class GetOutTransportActionData : ActionData
{
	Transport m_Vehicle;
	float m_Speed;
	float m_JumpingOutThreshold;
	bool m_WasJumpingOut 	= false;
	bool m_WasJumpingOutAnim= false;
	
	// obsolete
	Car m_Car;
	float m_CarSpeed;
	vector m_StartLocation;
	float m_DmgTaken; 
	float m_ShockTaken;
}

// client determines animation, server will still deal damage
class GetOutTransportActionReciveData : ActionReciveData
{
	bool m_WasJumpingOutAnim = false;
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
	protected const float BOAT_JUMPOUT_THRESHOLD 	= 5.0;
	
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

	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		if (super.SetupAction(player, target, item, action_data, extra_data))
		{
			HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
			if (vehCommand)
			{
				Transport trans = vehCommand.GetTransport();
				if (trans)
				{
					GetOutTransportActionData gotActionData = GetOutTransportActionData.Cast(action_data);
					ProcessGetOutTransportActionData(trans, gotActionData);
					return true;
				}
			}
		}
		return false;
	}

	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		GetOutTransportActionData gotActionData = GetOutTransportActionData.Cast(action_data);

		ctx.Write(gotActionData.m_WasJumpingOutAnim);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		if (!action_recive_data)
			action_recive_data = new GetOutTransportActionReciveData;

		super.ReadFromContext(ctx, action_recive_data);
		GetOutTransportActionReciveData action_data_got = GetOutTransportActionReciveData.Cast(action_recive_data);
		
		if (!ctx.Read(action_data_got.m_WasJumpingOutAnim))
			return false;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		GetOutTransportActionReciveData recive_data_got = GetOutTransportActionReciveData.Cast(action_recive_data);
		GetOutTransportActionData action_data_got = GetOutTransportActionData.Cast(action_data);
		
		action_data_got.m_WasJumpingOutAnim = recive_data_got.m_WasJumpingOutAnim;
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
			vector playerPosition = got_action_data.m_Player.PhysicsGetPositionWS();
			vector velocity = dBodyGetVelocityAt(veh, playerPosition);
			velocity[1] = 0; // sliding check doesn't use vertical velocity
			speed = velocity.Normalize();
		}

		got_action_data.m_Speed = speed;
		got_action_data.m_Vehicle = veh;

		got_action_data.m_WasJumpingOut = got_action_data.m_Speed > got_action_data.m_JumpingOutThreshold;
		if (got_action_data.m_Player.IsOwner())
			got_action_data.m_WasJumpingOutAnim = got_action_data.m_WasJumpingOut;
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

				if (!gotActionData.m_WasJumpingOutAnim)
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
	
	override bool HasTarget()
	{
		return false;
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
		
		// if server determined it was a jump out, or client played jump out animation, deal damage
		if (gotActionData.m_WasJumpingOut || gotActionData.m_WasJumpingOutAnim)
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
