class FuelStation extends BuildingSuper
{
	override bool IsBuilding()
	{
		return false;
	}
	
	override bool IsFuelStation()
	{
		return true;
	}
}