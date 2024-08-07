class Well extends BuildingSuper
{
	override bool IsBuilding()
	{
		return false;
	}
	
	override bool IsWell()
	{
		return GetWaterSourceObjectType() == EWaterSourceObjectType.WELL;
	}
	
	override EWaterSourceObjectType GetWaterSourceObjectType()
	{
		return EWaterSourceObjectType.WELL;
	}
	
	override float GetLiquidThroughputCoef()
	{
		return LIQUID_THROUGHPUT_WELL;
	}
	
	override int GetLiquidSourceType()
	{
		return LIQUID_CLEANWATER;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionWashHandsWell);
		AddAction(ActionDrinkWellContinuous);
	}
}