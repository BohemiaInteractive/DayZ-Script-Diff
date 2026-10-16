class StorageBox_Colorbase : Container_Base
{	
	override bool IsTakeable()
	{
		return false;
	}
	
	override bool CanSwapEntities(EntityAI otherItem, InventoryLocation otherDestination, InventoryLocation destination)
	{
		return false;
	}
	
	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}
	
	override bool DisableVicinityIcon()
	{
		return true;
	}
	
	override bool CanDisplayCargo()
	{
		return true;
	}
}

class StorageBox_Wooden : StorageBox_Colorbase {};
