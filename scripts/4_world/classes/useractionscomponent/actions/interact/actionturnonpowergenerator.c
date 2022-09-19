class ActionTurnOnPowerGenerator: ActionInteractBase
{	
	void ActionTurnOnPowerGenerator()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_Text = "#switch_on";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		PowerGenerator pg = PowerGenerator.Cast(target.GetObject());

		if (pg)
		{
			if (pg.HasEnergyManager() && pg.GetCompEM().CanSwitchOn() && pg.HasSparkplug() && pg.GetCompEM().CanWork())
			{
				return true;
			}
		}
		
		return false;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		Object targetObject = action_data.m_Target.GetObject();
		if (targetObject)
		{
			ItemBase target_IB = ItemBase.Cast(targetObject);
			target_IB.GetCompEM().SwitchOn();
		}
	}
};