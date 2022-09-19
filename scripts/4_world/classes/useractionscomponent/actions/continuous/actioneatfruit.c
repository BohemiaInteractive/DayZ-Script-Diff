class ActionEatFruitCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.EAT_NORMAL,UATimeSpent.DEFAULT);
	}
};

class ActionEatFruit: ActionEatBig
{
	void ActionEatFruit()
	{
		m_CallbackClass = ActionEatFruitCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EAT;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_EAT;
	}
		
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if (!super.ActionCondition(player, target, item))
			return false;
		Edible_Base food_item;

		if ( Class.CastTo(food_item, item) )
		{
			if ( food_item.IsFruit() )
				return true;
		}

		return false;
	}
}
