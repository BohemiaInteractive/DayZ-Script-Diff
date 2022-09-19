class ActionTurnOffWhileInHands : ActionSingleUseBase
{
	void ActionTurnOffWhileInHands()
	{
		m_Text = "#switch_off";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNone();
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
		if (item.IsInherited(Roadflare))
		{
			m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_LIGHTFLARE;
			m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_LIGHTFLARE;
		}
		else if (item.IsInherited(Chemlight_ColorBase))
		{
			m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_LITCHEMLIGHT;
			m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_LITCHEMLIGHT;
		}
		else if (item.IsInherited(GPSReceiver))
		{
			m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_PRESS_TRIGGER;
			m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_PRESS_TRIGGER;
		}
		else
		{
			m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_ITEM_OFF;
			m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_ITEM_OFF;
		}
		
		return item.HasEnergyManager() && item.GetCompEM().CanSwitchOff();
	}

	override void OnExecuteServer(ActionData action_data)
	{
		action_data.m_MainItem.GetCompEM().SwitchOff();
	}
}
