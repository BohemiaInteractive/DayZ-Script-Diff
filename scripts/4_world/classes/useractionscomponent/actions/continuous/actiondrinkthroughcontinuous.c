class ActionDrinkThroughContinuous: ActionDrinkPondContinuous
{
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTLiquid(LIQUID_GROUP_DRINKWATER - LIQUID_SNOW - LIQUID_HOTWATER, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		//! Other conditions are handled in CCTLiquid
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
