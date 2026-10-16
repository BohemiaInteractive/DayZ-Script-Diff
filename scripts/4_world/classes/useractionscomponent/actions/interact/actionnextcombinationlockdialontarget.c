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
		
		CombinationLock lock = CombinationLock.Cast(target.GetObject());
		if (!lock)
			return false;
		
		EntityAI parent = lock.GetHierarchyParent();
		if (!parent)
			return false;
		
		int facingSide = lock.GetLockSideByComponentIndex(target.GetComponentIndex());
		if (facingSide == -1)
			return false;
		
		if (parent.CanUseConstruction())
		{
			ConstructionActionData constructionActionData = player.GetConstructionActionData();
			constructionActionData.SetCombinationLock(lock);
			constructionActionData.SetCombinationLockSide(facingSide);
			return true;
		}
		
		return false;
	}
	
	override void Start(ActionData action_data)
	{
		super.Start(action_data);
		
		//set next dial
		ConstructionActionData constructionActionData = action_data.m_Player.GetConstructionActionData();
		CombinationLock combinationLock = constructionActionData.GetCombinationLock();
		int lockSide = constructionActionData.GetCombinationLockSide();
		if (combinationLock && lockSide != -1)
		{
			combinationLock.SetNextDial(lockSide);
		}
	}
}