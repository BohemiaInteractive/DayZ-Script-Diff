class ActionLightTorchVideo : ActionBase
{
	void ActionLightTorchVideo()
	{
		m_Text = "#switch_on";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNone();
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (item.GetCompEM() && !item.GetCompEM().IsSwitchedOn())
			return true;
		
		return false;
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override void OnStartServer( ActionData action_data )
	{
		ClearActionJuncture(action_data);
		ItemBase item = action_data.m_MainItem;

		item.GetCompEM().SwitchOn();
	}
}