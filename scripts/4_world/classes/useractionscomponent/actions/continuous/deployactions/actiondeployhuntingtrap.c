class ActionDeployHuntingTrap : ActionDeployBase
{
	void ActionDeployHuntingTrap()
	{
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Sound				= "craft_universal_0";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		// Client
		if ( !GetGame().IsDedicatedServer() )
			return item.CanBePlaced( player, player.GetPosition() );
		
		// Server
		return true;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		return GetGame().IsClient() || action_data.m_MainItem.CanBePlaced( action_data.m_Player, action_data.m_Player.GetPosition() );
	}
}