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
		m_CallbackClass = ActionDialCombinationLockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#dial_combination_lock";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNone;
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		m_Text = "#dial_combination_lock " + construction_action_data.GetDialNumberText();
	}

	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if ( item.IsInherited( CombinationLock ) )
		{
			ConstructionActionData construction_action_data = player.GetConstructionActionData();
			construction_action_data.SetCombinationLock( CombinationLock.Cast( item ) );
	
			return true;
		}
		
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		//set dialed number
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock combination_lock =  construction_action_data.GetCombinationLock();
		combination_lock.DialNextNumber();
	}
}