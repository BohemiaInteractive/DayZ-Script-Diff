class ActionTurnOnHeatpack : ActionTurnOnWhileInHands
{
	void ActionTurnOnHeatpack()
	{
		m_Text = "#activate";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( item.HasEnergyManager()  &&  item.GetCompEM().CanSwitchOn() )
		{
			return true;
		}
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		action_data.m_MainItem.GetCompEM().SwitchOn();
	}
};