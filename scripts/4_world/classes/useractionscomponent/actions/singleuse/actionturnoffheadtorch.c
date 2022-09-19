class ActionTurnOffHeadtorch: ActionBase
{
	void ActionTurnOffHeadtorch()
	{
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override typename GetInputType()
	{
		return ToggleLightsActionInput;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Headtorch_ColorBase headtorch;
		headtorch = Headtorch_ColorBase.Cast(target.GetObject());
		if ( !headtorch )
			return false;
		
		if ( headtorch.HasEnergyManager() && headtorch.GetCompEM().CanSwitchOff() ) //TODO review conditions for turning off
		{
			return true;
		}
		
		return false;
	}

	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		Headtorch_ColorBase headtorch;
		headtorch = Headtorch_ColorBase.Cast(action_data.m_Target.GetObject());
		if ( headtorch.HasEnergyManager() )
		{
			if ( headtorch.GetCompEM().IsSwitchedOn() )
			{
				headtorch.GetCompEM().SwitchOff();
			}
		}
	}
};
