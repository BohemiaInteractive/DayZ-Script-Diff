class ActionStopEngineBoat: ActionSingleUseBase
{
	void ActionStopEngineBoat()
	{
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONMOD_STOPENGINE;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text 			= "#stop_engine";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem		= new CCINone;
		m_ConditionTarget	= new CCTNone;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
		if (!vehCommand)
			return false;
		
		auto vehicle = BoatScript.Cast(vehCommand.GetTransport());
		if (!vehicle)
			return false;

		if (!vehicle.EngineIsOn())
			return false;
		
		return vehicle.CrewDriver() == player;
	}

	override void OnExecute(ActionData action_data)
	{
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (!vehCommand)
			return;

		auto vehicle = BoatScript.Cast(vehCommand.GetTransport());
		if (!vehicle)
			return;

		if (vehicle.GetNetworkMoveStrategy() == NetworkMoveStrategy.PHYSICS)
		{
			// Only perform on clients (or robos), validation is performed in C++
			if (action_data.m_Player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
			{
				return;
			}
		}

		vehicle.EngineStop();
	}
		
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override bool HasTarget()
	{
		return false;
	}
}
