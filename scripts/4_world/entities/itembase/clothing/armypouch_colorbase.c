class ArmyPouch_ColorBase : Backpack_Base
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
class ArmyPouch_Green : ArmyPouch_ColorBase {};
class ArmyPouch_Black : ArmyPouch_ColorBase {};
class ArmyPouch_Camo : ArmyPouch_ColorBase {};