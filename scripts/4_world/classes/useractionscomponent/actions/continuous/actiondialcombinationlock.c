class ActionDialCombinationLockCB : ActionContinuousBaseCB
{
	private const float REPEAT_AFTER_SEC = 0.5;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(REPEAT_AFTER_SEC);
	}
}

class ActionDialCombinationLock: ActionContinuousBase
{
	void ActionDialCombinationLock()
	{
		m_CallbackClass 	= ActionDialCombinationLockCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;		
		m_SpecialtyWeight 	= UASoftSkillsWeight.ROUGH_LOW;
		m_Text 				= "#dial_combination_lock";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget 	= new CCTNone();
		m_ConditionItem 	= new CCINonRuined();
	}
	
	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override void OnActionInfoUpdate(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ConstructionActionData constructionActionData = player.GetConstructionActionData();
		m_Text = "#dial_combination_lock " + constructionActionData.GetDialNumberText();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		if (item.IsInherited(CombinationLock))
		{
			ConstructionActionData constructionActionData = player.GetConstructionActionData();
			constructionActionData.SetCombinationLock(CombinationLock.Cast(item));
	
			return true;
		}
		
		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{	
		//set dialed number
		ConstructionActionData constructionActionData = action_data.m_Player.GetConstructionActionData();
		CombinationLock combination_lock =  constructionActionData.GetCombinationLock();
		combination_lock.DialNextNumber();
	}
}
