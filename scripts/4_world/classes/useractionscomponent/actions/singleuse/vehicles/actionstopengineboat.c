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
		HumanCommandVehicle vehCmd = player.GetCommand_Vehicle();			
		if (vehCmd && vehCmd.GetVehicleSeat() == DayZPlayerConstants.VEHICLESEAT_DRIVER)
		{
			Transport trans = vehCmd.GetTransport();
			if (!trans)
				return false;
			
			Boat boat = Boat.Cast(trans);
			if (boat && boat.EngineIsOn())
				return true;		
		}

		return false;
	}

	override void OnExecute(ActionData action_data)
	{
		HumanCommandVehicle vehCmd = action_data.m_Player.GetCommand_Vehicle();
		if (!vehCmd)
			return;
		
		Boat boat = Boat.Cast(vehCmd.GetTransport());
		if (boat)
			boat.EngineStop();
		
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
};