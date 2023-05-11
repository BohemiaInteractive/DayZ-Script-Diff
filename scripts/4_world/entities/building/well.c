class Well extends BuildingSuper
{
	override bool IsBuilding()
	{
		return false;
	}
	
	override bool IsWell()
	{
		return GetWaterSourceObjectType();
	}
	
	override EWaterSourceObjectType GetWaterSourceObjectType()
	{
		return EWaterSourceObjectType.WELL;
	}
	
	override float GetLiquidThroughputCoef()
	{
		return LIQUID_THROUGHPUT_WELL;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionWashHandsWell);
		AddAction(ActionDrinkWellContinuous);
	}
}