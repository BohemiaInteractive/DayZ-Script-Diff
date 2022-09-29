class Well extends BuildingSuper
{
	override bool IsBuilding()
	{
		return false;
	}
	
	override bool IsWell()
	{
		return true;
	}
	
	override float GetLiquidThroughputCoef()
	{
		return LIQUID_THROUGHPUT_WELL;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionWashHandsWellOne);
		AddAction(ActionDrinkWellContinuous);
	}
}