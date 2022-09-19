class ActionWashHandsItem: ActionSingleUseBase
{
	protected const float WASH_HANDS_AMOUNT = 250; //ml
	
	void ActionWashHandsItem()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_CLEANHANDSBOTTLE;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_CLEANHANDSBOTTLE;
		m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#wash_hands";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINotRuinedAndEmpty;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool HasTarget()
	{
		return false;
	}

	override bool HasProneException()
	{
		return true;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		//Print(item.GetQuantity());
		if ( player.GetItemOnSlot("Gloves") )
			return false;
		 
		return player.HasBloodyHands() && ( item.GetQuantity() >= item.GetDisinfectQuantity() );
	}

	override void OnEndServer( ActionData action_data )
	{
		if (action_data.m_State != UA_INTERRUPT)
		{
			PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
			module_lifespan.UpdateBloodyHandsVisibility( action_data.m_Player, false );
			action_data.m_MainItem.AddQuantity( -action_data.m_MainItem.GetDisinfectQuantity(), false );
		}
	}
};