class Land_Misc_Through_Static : BuildingSuper
{
	override EWaterSourceObjectType GetWaterSourceObjectType()
	{
		return EWaterSourceObjectType.THROUGH;
	}
	
	override int GetLiquidSourceType()
	{
		return LIQUID_FRESHWATER;
	}

	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionDrink);

		AddAction(ActionWashHandsWell);
		AddAction(ActionDrinkThroughContinuous);
		AddAction(ActionFillBottleBase);
	}
}