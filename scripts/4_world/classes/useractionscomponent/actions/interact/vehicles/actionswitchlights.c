class ActionSwitchLights: ActionInteractBase
{
	void ActionSwitchLights()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_HEADLIGHT;
		m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text 				= "#switch_lights";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNone();
	}

	override typename GetInputType()
	{
		return ToggleVehicleLightsActionInput;
	}
	
	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport transport = vehCommand.GetTransport();
			if (transport)
			{
				if (transport.CrewMemberIndex(player) == DayZPlayerConstants.VEHICLESEAT_DRIVER)
				{
					if (!transport.LightIsOn())
					{
						if (transport.NeedElectricitySourceDevice())
						{
							EntityAI neededItem = transport.GetElectricitySourceDevice();
							
							if (neededItem && !neededItem.IsRuined())
								return neededItem.GetCompEM() && neededItem.GetCompEM().GetEnergy() > 0;
						}
						else
							return true;
					}
					else
						return true;
				}
			}
		}

		return false;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if (vehCommand)
		{
			Transport transport = vehCommand.GetTransport();
			if (transport)
				transport.LightToggle();
		}
	}
	
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
}
