class ActionClose: ActionSingleUseBase
{
	void ActionClose()
	{
		m_Text = "#close";
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
		if ( item && item.IsOpen() ) 
		{
			return true;
		}
		return false;
	}

	//setup
	override bool SetupAction( PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL )
	{	
		if( super.SetupAction( player, target, item, action_data, extra_data ) )
		{
			SetCloseAnimation( item );
			
			return true;
		}
		
		return false;
	}
	
	void SetCloseAnimation( ItemBase item )
	{
		switch( item.Type() )
		{
			case Compass:
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_CLOSEITEM_ONCE;
				break;
			case TelescopicBaton:
				m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
				break;
		}
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		action_data.m_MainItem.Close();
	}
};