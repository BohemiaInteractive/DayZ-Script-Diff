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
		RemoveAction(ActionDrinkPondContinuous);
		
		AddAction(ActionWashHandsWell);
		AddAction(ActionDrinkThroughContinuous);
		AddAction(ActionObtainLiquidBase);
	}
	
	override void SetActionAnimOverrides()
	{
		OverrideActionAnimation(ActionObtainLiquidBase, DayZPlayerConstants.CMD_ACTIONFB_FILLBOTTLEPOND, DayZPlayerConstants.STANCEMASK_CROUCH);
		OverrideActionAnimation(ActionWashHandsWell, DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSPOND, DayZPlayerConstants.STANCEMASK_CROUCH);
	}
}