class DuffelBagSmall_ColorBase : Backpack_Base 
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
class DuffelBagSmall_Green : DuffelBagSmall_ColorBase {};
class DuffelBagSmall_Black : DuffelBagSmall_ColorBase {};
class DuffelBagSmall_Camo : DuffelBagSmall_ColorBase {};