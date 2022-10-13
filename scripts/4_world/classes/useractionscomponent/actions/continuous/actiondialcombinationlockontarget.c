class ActionDialCombinationLockOnTargetCB : ActionContinuousBaseCB
{
	private const float REPEAT_AFTER_SEC = 0.5;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(REPEAT_AFTER_SEC);
	}
}

class ActionDialCombinationLockOnTarget: ActionContinuousBase
{
	void ActionDialCombinationLockOnTarget()
	{
		m_CallbackClass		= ActionDialCombinationLockOnTargetCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;
		m_SpecialtyWeight 	= UASoftSkillsWeight.ROUGH_LOW;
		m_Text 				= "#dial_combination_lock";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override bool HasProneException()
	{
		return true;
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override void OnActionInfoUpdate(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ConstructionActionData constructionActionData = player.GetConstructionActionData();
		m_Text = "#dial_combination_lock " + constructionActionData.GetDialNumberText();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target.GetObject())
			return false;
		
		CombinationLock lock = CombinationLock.Cast(target.GetObject());
		if (lock && lock.GetHierarchyParent() && Fence.Cast(lock.GetHierarchyParent()))
		{
			ConstructionActionData constructionActionData = player.GetConstructionActionData();
			constructionActionData.SetCombinationLock(lock);
			
			return true;
		}
		
		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{	
		//set dialed number
		ConstructionActionData constructionActionData = action_data.m_Player.GetConstructionActionData();
		CombinationLock combination_lock = constructionActionData.GetCombinationLock();
		if (combination_lock)
		{
			combination_lock.DialNextNumber();
	
			//check for unlock state
			if (!combination_lock.IsLockedOnGate())
			{
				EntityAI target_entity = EntityAI.Cast(action_data.m_Target.GetObject());
				combination_lock.UnlockServer(action_data.m_Player, target_entity.GetHierarchyParent());
			}
		}
	}
}
