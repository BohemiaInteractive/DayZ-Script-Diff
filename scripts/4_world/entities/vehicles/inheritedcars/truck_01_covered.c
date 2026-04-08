class Truck_01_Covered extends Truck_01_Base {};

class Truck_01_Covered_Orange extends Truck_01_Covered
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GameInventory inventory = GetInventory();
		inventory.CreateInInventory("Truck_01_Wheel");
		inventory.CreateInInventory("Truck_01_Wheel");

		inventory.CreateInInventory("Truck_01_WheelDouble");
		inventory.CreateInInventory("Truck_01_WheelDouble");
		inventory.CreateInInventory("Truck_01_WheelDouble");
		inventory.CreateInInventory("Truck_01_WheelDouble");

		inventory.CreateInInventory("Truck_01_Door_1_1_Orange");
		inventory.CreateInInventory("Truck_01_Door_2_1_Orange");
		inventory.CreateInInventory("Truck_01_Hood_Orange");
			
		//-----IN CAR CARGO
		inventory.CreateInInventory("Truck_01_Wheel");
		inventory.CreateInInventory("Truck_01_Wheel");
		inventory.CreateInInventory("Truck_01_WheelDouble");
		inventory.CreateInInventory("Truck_01_WheelDouble");
	}
}

class Truck_01_Covered_Blue extends Truck_01_Covered
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GameInventory inventory = GetInventory();
		inventory.CreateInInventory("Truck_01_Wheel");
		inventory.CreateInInventory("Truck_01_Wheel");

		inventory.CreateInInventory("Truck_01_WheelDouble");
		inventory.CreateInInventory("Truck_01_WheelDouble");
		inventory.CreateInInventory("Truck_01_WheelDouble");
		inventory.CreateInInventory("Truck_01_WheelDouble");

		inventory.CreateInInventory("Truck_01_Door_1_1_Blue");
		inventory.CreateInInventory("Truck_01_Door_2_1_Blue");
		inventory.CreateInInventory("Truck_01_Hood_Blue");
			
		//-----IN CAR CARGO
		inventory.CreateInInventory("Truck_01_Wheel");
		inventory.CreateInInventory("Truck_01_Wheel");
		inventory.CreateInInventory("Truck_01_WheelDouble");
		inventory.CreateInInventory("Truck_01_WheelDouble");
	}
}
