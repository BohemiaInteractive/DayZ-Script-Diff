class ActionNextCombinationLockDial: ActionSingleUseBase
{
	void ActionNextCombinationLockDial()
	{
		m_Text = "#next_combination_lock_dial";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem		= new CCINonRuined;
		m_ConditionTarget 	= new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool RemoveForceTargetAfterUse()
	{
		return false;
	}
	
	override bool UseAcknowledgment()
	{
		return true;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( item.IsInherited( CombinationLock ) )
		{
			ConstructionActionData construction_action_data = player.GetConstructionActionData();
			construction_action_data.SetCombinationLock( CombinationLock.Cast( item ) );

			return true;
		}
		
		return false;
	}

	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		//set next dial
		CombinationLock combination_lock = CombinationLock.Cast( action_data.m_MainItem );
		if ( combination_lock )
		{
			combination_lock.SetNextDial();
		}
	}
}