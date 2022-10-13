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
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}

	override typename GetInputType()
	{
		return ToggleLightsActionInput;
	}
	
	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();

		if ( vehCommand )
		{
			Transport trans = vehCommand.GetTransport();
			if ( trans )
			{
				CarScript car;
				if ( Class.CastTo(car, trans) )
				{
					if ( car.CrewMemberIndex( player ) == DayZPlayerConstants.VEHICLESEAT_DRIVER )
					{
						if ( !car.IsScriptedLightsOn() )
						{
							EntityAI neededItem = null;

							if ( car.IsVitalCarBattery() ) neededItem = car.FindAttachmentBySlotName("CarBattery");
							if ( car.IsVitalTruckBattery() ) neededItem = car.FindAttachmentBySlotName("TruckBattery");
							
							if (neededItem && !neededItem.IsRuined())
							{
								return neededItem.GetCompEM() && neededItem.GetCompEM().GetEnergy() > 0;
							}
						}
						else
						{
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if ( vehCommand )
		{
			Transport trans = vehCommand.GetTransport();
			if ( trans )
			{
				CarScript car;
				if ( Class.CastTo(car, trans) )
				{
					car.ToggleHeadlights();
				}
			}
		}
	}
	
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
};