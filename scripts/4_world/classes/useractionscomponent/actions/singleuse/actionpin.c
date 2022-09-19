class ActionPin extends ActionSingleUseBase
{
	void ActionPin()
	{
		m_Text = "#pin";
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
		/*
		if ( item.IsInherited(Grenade_Base) )
		{
			m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PINGRENAGE;
		}
		else
		*/
		{
			m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		}
		
		Grenade_Base grenade = Grenade_Base.Cast(item);
		if( grenade )
		{
			if( !grenade.IsPinned() && grenade.IsPinnable() )
			{
				return true;
			}
		}

		return false;
	}

	override void OnExecute( ActionData action_data )
	{
		Grenade_Base grenade = Grenade_Base.Cast(action_data.m_MainItem);
		if( grenade )
		{
			grenade.Pin();
		}
	}
}