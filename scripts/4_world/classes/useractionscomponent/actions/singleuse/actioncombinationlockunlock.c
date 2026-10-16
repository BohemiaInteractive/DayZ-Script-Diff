class ActionCombinationLockUnlock: ActionInteractBase
{
	void ActionCombinationLockUnlock()
	{
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_Text = "#detach_combination_lock";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
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
	
	//! Only handles single player/offline handling
	override void OnExecute(ActionData action_data)
	{
		if (!GetGame().IsServer() && GetGame().IsMultiplayer())
			return;
		
		TryUnlockLock(action_data);
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		TryUnlockLock(action_data);
	}
	
	void TryUnlockLock(ActionData action_data)
	{
		ConstructionActionData constructionActionData = action_data.m_Player.GetConstructionActionData();
		CombinationLock combinationLock = constructionActionData.GetCombinationLock();
		int facingSide = constructionActionData.GetCombinationLockSide();
		if (!combinationLock || facingSide == -1)
			return;
		
		combinationLock.CheckLockedStateServer(facingSide);
		
		// Check for unlock state
		if (!combinationLock.IsLockedOnParent())
		{
			if (GetGame().IsMultiplayer())
				ClearActionJuncture(action_data);
			else 
				ClearInventoryReservationEx(action_data);
			
			EntityAI targetEntity = EntityAI.Cast(action_data.m_Target.GetObject());
			combinationLock.UnlockOnServer(action_data.m_Player, targetEntity.GetHierarchyParent(), facingSide);
		}
		else
		{
			combinationLock.StartItemSoundServer(SoundConstants.ITEM_COMBINATION_LOCK_WRONG_COMBINATION);
		}
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		ConstructionActionData constructionActionData = action_data.m_Player.GetConstructionActionData();
		CombinationLock combinationLock = constructionActionData.GetCombinationLock();
		if (!combinationLock)
			return;
		
		combinationLock.StopItemSoundServer(SoundConstants.ITEM_COMBINATION_LOCK_WRONG_COMBINATION);
		constructionActionData.SetCombinationLock(null);
	}
}