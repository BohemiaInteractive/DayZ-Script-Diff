class ActionTriggerRemotely : ActionSingleUseBase
{	
	void ActionTriggerRemotely()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PRESS_TRIGGER;
		m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_PRESS_TRIGGER;
		m_Text 				= "#STR_Detonate";
	}
	
	override void CreateConditionComponents()
	{	
		m_ConditionTarget	= new CCTNone();
		m_ConditionItem		= new CCINonRuined();
	}

	override bool HasProgress()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool HasProneException()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		RemoteDetonatorTrigger rdt = RemoteDetonatorTrigger.Cast(item);
		if (rdt && rdt.IsConnected())
		{
			ExplosivesBase controlledDevice = ExplosivesBase.Cast(rdt.GetControlledDevice());
			if (controlledDevice && controlledDevice.GetPairDevice() != item)
			{
				rdt.UpdateLED(ERemoteDetonatorLEDState.OFF);

				return false;
			}

			if (controlledDevice && !controlledDevice.IsRuined() && controlledDevice.GetArmed())
			{
				if (vector.DistanceSq(player.GetPosition(), controlledDevice.GetPosition()) <= Math.SqrFloat(UAMaxDistances.EXPLOSIVE_REMOTE_ACTIVATION))
				{
					rdt.UpdateLED(ERemoteDetonatorLEDState.LIT);
					
					return true;
				}
			}
		}
		
		rdt.UpdateLED(ERemoteDetonatorLEDState.OFF);

		return false;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		RemoteDetonatorTrigger rdt = RemoteDetonatorTrigger.Cast(action_data.m_MainItem);
		if (rdt && rdt.IsConnected()) 
		{
			ItemBase controlledDevice = ItemBase.Cast(rdt.GetControlledDevice());
			if (controlledDevice && !controlledDevice.IsRuined())
			{
				controlledDevice.OnActivatedByItem(action_data.m_MainItem);
				action_data.m_MainItem.SetAnimationPhase("trigger", 1);
				action_data.m_MainItem.SetAnimationPhase("lever", 1);
			}
		}
	}
}
