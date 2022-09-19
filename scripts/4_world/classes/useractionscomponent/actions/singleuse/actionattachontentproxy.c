class ActionAttachOnTentProxy: ActionAttachOnProxy
{
	void ActionAttachOnTentProxy() {};
	
	override void CreateConditionComponents() 
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursorParent;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		TentBase tent = TentBase.Cast(target.GetParent());
		if ( !tent )
			return false;
		
		return super.ActionCondition(player,target,item);
	}
}