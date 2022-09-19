class ActionTakeABiteCB : ActionSingleUseBaseCB
{
	private const float QUANTITY_USED_PER_SEC2 = 50;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUseQuantityEdible(QUANTITY_USED_PER_SEC2);
	}
};

class ActionTakeABite: ActionConsumeSingle
{
	void ActionTakeABite()
	{
		m_CallbackClass = ActionTakeABiteCB;
		m_Sound = "EatingSoft_0";
		//m_Animation = "eat";
		m_Text = "#take_a_bite";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		return false;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}
};