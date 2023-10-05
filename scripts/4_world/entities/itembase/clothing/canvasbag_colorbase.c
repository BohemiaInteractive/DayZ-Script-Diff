class CanvasBag_ColorBase : Backpack_Base 
{
	override void OnDebugSpawn()
	{
		GetInventory().CreateInInventory("Chemlight_Red");
		EntityAI item = GetInventory().CreateInInventory("GPSReceiver");
		item.GetInventory().CreateInInventory("Battery9V");
		item = GetInventory().CreateEntityInCargo("PersonalRadio");
		item.GetInventory().CreateInInventory("Battery9V");
	}
};
class CanvasBag_Olive : CanvasBag_ColorBase {};
class CanvasBag_Medical : CanvasBag_ColorBase {};