class ActionTurnOnWhileInHands : ActionSingleUseBase
{
	void ActionTurnOnWhileInHands()
	{
		m_Text = "#switch_on";
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
			m_CommandUID		= DayZPlayerConstants.CMD_ACTIONMOD_LIGHTFLARE;
			m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_LITCHEMLIGHT;
		}
		else if (item.IsInherited(Chemlight_ColorBase))
		{
			m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_LITCHEMLIGHT;
			m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_LITCHEMLIGHT;
		}
		else if (item.IsInherited(GPSReceiver))
		{
			m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_PRESS_TRIGGER;
			m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_PRESS_TRIGGER;
		}
		else
		{
			m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
			m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
		}
		
		return item.HasEnergyManager() && item.GetCompEM().CanSwitchOn() && item.GetCompEM().CanWork();
	}

	override void OnExecuteServer(ActionData action_data)
	{
		if ( action_data.m_MainItem.HasEnergyManager() )
		{
			if (action_data.m_MainItem.GetCompEM().CanWork())
			{
				action_data.m_MainItem.GetCompEM().SwitchOn();
			}
		}
	}
	
	override void OnStartClient(ActionData action_data)
	{
		if (action_data.m_MainItem && action_data.m_MainItem.IsInherited(Roadflare))
		{
			Roadflare flare = Roadflare.Cast(action_data.m_MainItem);
			flare.SetModelState(RoadflareModelStates.UNCAPPED_UNIGNITED);
		}
	}
	
	override void OnStartServer( ActionData action_data )
	{
		if (action_data.m_MainItem && action_data.m_MainItem.IsInherited(Roadflare))
		{
			Roadflare flare = Roadflare.Cast(action_data.m_MainItem);
			flare.SetModelState(RoadflareModelStates.UNCAPPED_UNIGNITED);
		}
	}
}
