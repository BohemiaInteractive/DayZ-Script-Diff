class ActionAttachOnProxy: ActionAttach
{
	void ActionAttachOnProxy()
	{
		m_Text = "#attach";
	}
	
	override void CreateConditionComponents() 
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursor;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		Object targetObject = target.GetObject();
		Object targetParent = target.GetParent();
		EntityAI target_entity = EntityAI.Cast( targetParent );
		EntityAI item_entity = item;
		
		if ( targetParent )
		{
			if ( target_entity && item_entity )
			{
				if ( target_entity.GetInventory() && target_entity.GetInventory().CanAddAttachment( item_entity ) )
				{
					return true;
				}
			}	
		}
		
		return false;
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		super.OnExecuteClient( action_data );
		
		if ( action_data.m_Player.IsPlacingLocal() )
		{
			action_data.m_Player.TogglePlacingLocal();
		}
	}
}