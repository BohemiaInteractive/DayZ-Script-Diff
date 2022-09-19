class ActionNextCombinationLockDialOnTarget: ActionInteractBase
{
	void ActionNextCombinationLockDialOnTarget()
	{
		m_Text = "#next_combination_lock_dial";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.DEFAULT );
	}

	override bool IsInstant()
	{
		return true;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if (!target.GetObject())
			return false;
		
		CombinationLock lock = CombinationLock.Cast( target.GetObject() );
		if ( lock && lock.GetHierarchyParent() && Fence.Cast(lock.GetHierarchyParent()) )
		{
			ConstructionActionData construction_action_data = player.GetConstructionActionData();
			construction_action_data.SetCombinationLock( lock );
			
			return true;
		}
		
		return false;
	}
	
	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		//set next dial
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock combination_lock = construction_action_data.GetCombinationLock();
		if ( combination_lock )
		{
			combination_lock.SetNextDial();
		}
	}
}