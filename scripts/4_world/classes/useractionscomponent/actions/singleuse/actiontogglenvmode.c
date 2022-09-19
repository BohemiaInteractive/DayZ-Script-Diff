
class ActionToggleNVMode: ActionSingleUseBase
{
	void ActionToggleNVMode()
	{
		//m_CallbackClass = ActionTurnOnTransmitterCB;
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
		m_Text = "#switch_mode";
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
		ItemOptics optics;
		if ( Class.CastTo(optics,item) && optics.IsNVOptic() )
		{
			return true;
		}
		
		return false;
	}
		
	override void OnExecuteServer( ActionData action_data )
	{
		SwitchMode(action_data);
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		SwitchMode(action_data);
	}
	
	void SwitchMode( ActionData action_data )
	{
		ItemOptics optics = ItemOptics.Cast(action_data.m_MainItem);
		switch (optics.GetCurrentOpticMode())
		{
			case GameConstants.OPTICS_STATE_DAY:
				optics.SetCurrentOpticMode(GameConstants.OPTICS_STATE_NIGHTVISION);
				//TODO: animate
			break;
			
			case GameConstants.OPTICS_STATE_NIGHTVISION:
				optics.SetCurrentOpticMode(GameConstants.OPTICS_STATE_DAY);
				//TODO: animate
			break;
		}
	}
}
