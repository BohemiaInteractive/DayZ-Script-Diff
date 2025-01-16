class ActionEatBigCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_BIG, UATimeSpent.DEFAULT);
	}
};

class ActionEatBig: ActionConsume
{
	void ActionEatBig()
	{
		m_CallbackClass = ActionEatBigCB;
		//m_Sound = "EatingSoft_0";
		m_Text = "#eat";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}
	
	override int IsEat()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}
}


//-------------- Action Eat
class ActionEatCB : ActionEatBigCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_NORMAL, UATimeSpent.DEFAULT);
	}
};

class ActionEat: ActionEatBig
{
	void ActionEat()
	{
		m_CallbackClass = ActionEatCB;
	}
	
	
};

//-------------- Action Eat Small
class ActionEatSmallCB : ActionEatBigCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_SMALL, UATimeSpent.DEFAULT);
	}
};

class ActionEatSmall: ActionEatBig
{
	void ActionEatSmall()
	{
		m_CallbackClass = ActionEatSmallCB;
	}
};