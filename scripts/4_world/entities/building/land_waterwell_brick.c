class Land_WaterWell_Brick : Well
{
	override int GetLiquidSourceType()
	{
		Rebuilding reb;
		if (Class.CastTo(reb,GetConstructionBasic()) && reb.IsPartConstructed("roof"))
			return LIQUID_CLEANWATER;
		return LIQUID_NONE;
	}
	
	override EWaterSourceObjectType GetWaterSourceObjectType()
	{
		Rebuilding reb;
		if (Class.CastTo(reb,GetConstructionBasic()) && reb.IsPartConstructed("roof"))
			return EWaterSourceObjectType.WELL;
		else
			return EWaterSourceObjectType.NONE;
	}
	
	override void OnDebugSpawn()
	{
		ConstructionBase con = ConstructionBase.Cast(GetConstructionBasic());
		
		con.BuildPartServerEx(null,"rubble_rubble1",-1);
		con.BuildPartServerEx(null,"well_brick",-1);
		con.BuildPartServerEx(null,"roof",-1);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionDrinkWellContinuous);
		
		AddAction(ActionDrinkWellContinuousBucket);
	}
}