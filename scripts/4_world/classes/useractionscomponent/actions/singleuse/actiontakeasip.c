class ActionTakeASip: ActionTakeABite
{
	void ActionTakeASip()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_DRINK;
		m_Text = "#take_a_sip";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		return false;
	}
};