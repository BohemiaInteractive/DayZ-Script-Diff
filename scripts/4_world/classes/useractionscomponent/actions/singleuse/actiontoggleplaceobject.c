class ActionTogglePlaceObject: ActionSingleUseBase
{

	void ActionTogglePlaceObject()
	{
		m_Text = "#toggle_placing";
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
	
	override bool IsLocal()
	{
		return true;
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool IsDeploymentAction()
	{
		return true;
	}
	
	override bool RemoveForceTargetAfterUse()
	{
		return false;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		// not placeable if liquid is present; exception for barrels, for now
		if ( !item.IsKindOf("Barrel_ColorBase") && item.IsLiquidPresent() )
		{
			return false;
		}
		
		return true;
	}
	
	override void Start( ActionData action_data ) //Setup on start of action
	{
		super.Start( action_data );
		action_data.m_Player.SetLocalProjectionPosition(action_data.m_Target.GetCursorHitPos());
		action_data.m_Player.TogglePlacingLocal();
	}
};