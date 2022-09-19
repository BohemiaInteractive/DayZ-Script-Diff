class ActionEatCanCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_BIG, UATimeSpent.DEFAULT);
	}
};


class ActionEatCan: ActionEatBig
{
	void ActionEatCan()
	{
		m_CallbackClass = ActionEatCanCB;
	}
		
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		return super.ActionCondition(player, target, item);
		/* 
		//if staging for cans is introduced
		if ( item && item.IsOpen() )
		{
			return true;
		}
		return false;
		*/
	}
};

//-----------------SMALL BITES VARIANT-------------------

class ActionEatSmallCanCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_SMALL, UATimeSpent.DEFAULT);
	}
};


class ActionEatSmallCan: ActionEatBig
{
	void ActionEatSmallCan()
	{
		m_CallbackClass = ActionEatSmallCanCB;
	}
		
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if (!super.ActionCondition(player, target, item))
			return false;
		return true;
		/* 
		//if staging for cans is introduced
		if ( item && item.IsOpen() )
		{
			return true;
		}
		return false;
		*/
	}
};