class ActionDrinkThroughContinuous: ActionDrinkPondContinuous
{
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTCursor(UAMaxDistances.SMALL);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (item && item.IsHeavyBehaviour())
			return false;
		
		if (!player.CanEatAndDrink())
			return false;
		
		return target.GetObject() && target.GetObject().GetWaterSourceObjectType() == EWaterSourceObjectType.THROUGH;
	}
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
}
