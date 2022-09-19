class ActionOpen: ActionSingleUseBase
{
	void ActionOpen()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM_ONCE;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM_ONCE;
		//m_Animation = "open";
		m_Text = "#open";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( item && !item.IsOpen() )
		{ 
			return true;
		}
		return false;
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		return true;
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

	override void OnExecuteServer( ActionData action_data )
	{
		if( !GetGame().IsMultiplayer() )
			ClearInventoryReservationEx(action_data);
		
		action_data.m_MainItem.Open();
	}
};