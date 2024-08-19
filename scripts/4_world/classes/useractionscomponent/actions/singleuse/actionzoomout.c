class ActionZoomOut: ActionSingleUseBase
{
	void ActionZoomOut()
	{
		m_Text = "#zoom_out";
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
		ItemOptics optic;
		if( Class.CastTo(optic, item) && optic.IsInOptics() )
		{ 
			return true;
		}
		return false;
	}

	override void OnEndServer( ActionData action_data )
	{
		ItemOptics optic;
		if( Class.CastTo(optic, action_data.m_MainItem) )
		{
			optic.ExitOptics(); 
		}
	}
};