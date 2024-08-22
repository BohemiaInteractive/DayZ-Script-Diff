class ActionOperatePanelPowerStation : ActionOperatePanel
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target) 
			return false;
		
		Land_WarheadStorage_PowerStation powerStation = Land_WarheadStorage_PowerStation.Cast(target.GetObject());
		if (!powerStation)
			return false;
	
		int leverComponent = target.GetComponentIndex();
		string leverName = powerStation.GetActionComponentName(leverComponent);
		int leverIndex = powerStation.GetLeverIndexByComponentName(leverName);

		return (leverIndex != 0);	
	}
	
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		Land_WarheadStorage_PowerStation powerStation = Land_WarheadStorage_PowerStation.Cast(action_data.m_Target.GetObject());
		if (!powerStation)
			return;
		
		int leverComponent = action_data.m_Target.GetComponentIndex();
		string leverName = powerStation.GetActionComponentName(leverComponent);
		int leverIndex = powerStation.GetLeverIndexByComponentName(leverName);
		if (leverIndex)
		{
			if (GetGame().IsServer())
				powerStation.AnimateLever(leverIndex);
		}
	}
}