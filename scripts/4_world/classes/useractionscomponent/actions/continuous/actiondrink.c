class ActionDrinkCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.DRINK,UATimeSpent.DEFAULT);
	}
};

class ActionDrink: ActionConsume
{
	void ActionDrink()
	{
		m_CallbackClass = ActionDrinkCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_DRINK;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_DRINK;
		m_Text = "#drink";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINotRuinedAndEmpty;
		m_ConditionTarget = new CCTSelf;
	}

	override bool IsDrink()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if (!super.ActionCondition(player, target, item))
			return false;
		
		return player.CanEatAndDrink();
	}
};