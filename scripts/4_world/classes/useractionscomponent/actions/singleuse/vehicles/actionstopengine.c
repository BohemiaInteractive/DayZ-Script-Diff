class ActionStopEngine: ActionSingleUseBase
{
	void ActionStopEngine()
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
		HumanCommandVehicle vehCmd = player.GetCommand_Vehicle();			
		if (vehCmd && vehCmd.GetVehicleSeat() == DayZPlayerConstants.VEHICLESEAT_DRIVER)
		{
			Transport trans = vehCmd.GetTransport();
			if (trans)
			{
				Car car;
				if (Class.CastTo(car, trans) && car.EngineIsOn())
				{
					return car.GetSpeedometer() <= 8;
				}
			}
		}

		return false;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		HumanCommandVehicle vehCmd = action_data.m_Player.GetCommand_Vehicle();
		if (vehCmd)
		{
			Transport trans = vehCmd.GetTransport();
			if (trans)
			{
				CarScript car;
				if (Class.CastTo(car, trans))
				{
					car.EngineStop();
				}
			}
		}	
	}
	
	override void OnExecuteClient(ActionData action_data) {}
	
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
};