class ActionOperatePanel: ActionInteractBase
{
	ref NoiseParams m_NoisePar;

	void ActionOperatePanel()
	{
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text 			= "SWITCH";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTCursor();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target) 
			return false;
		Land_WarheadStorage_PowerStation building;
		if (Class.CastTo(building, target.GetObject()))
		{
			int leverComponent = target.GetComponentIndex();
			string leverName = building.GetActionComponentName(leverComponent);
			int leverIndex = GetLeverIndexByComponentName(leverName);
			return (leverIndex != 0);
		}

		return false;
	}
	
	
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		Land_WarheadStorage_PowerStation building;
		if (Class.CastTo(building, action_data.m_Target.GetObject()))
		{
			int leverComponent = action_data.m_Target.GetComponentIndex();
			string leverName = building.GetActionComponentName(leverComponent);
			int leverIndex = GetLeverIndexByComponentName(leverName);
			if (leverIndex)
			{
				if (GetGame().IsServer())
					building.AnimateLever(leverIndex);
				
				if (!GetGame().IsDedicatedServer())
					building.PlayLeverSound(leverIndex);	// temporary, wont work for remotes
			}
		}
	}
	
	protected int GetLeverIndexByComponentName(string name)
	{
		name.ToLower();
		switch (name)
		{
			case "storagedoor1":
				return 1;
			case "storagedoor2":
				return 2;
			case "storagedoor3":
				return 3;
			case "storagedoor4":
				return 4;
		}
		return 0;
	}
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
}
