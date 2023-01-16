class Truck_01_Covered extends Truck_01_Base {};

class Truck_01_Covered_Orange extends Truck_01_Covered
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GetInventory().CreateInInventory("Truck_01_Wheel");
		GetInventory().CreateInInventory("Truck_01_Wheel");

		GetInventory().CreateInInventory("Truck_01_WheelDouble");
		GetInventory().CreateInInventory("Truck_01_WheelDouble");
		GetInventory().CreateInInventory("Truck_01_WheelDouble");
		GetInventory().CreateInInventory("Truck_01_WheelDouble");

		GetInventory().CreateInInventory("Truck_01_Door_1_1_Orange");
		GetInventory().CreateInInventory("Truck_01_Door_2_1_Orange");
		GetInventory().CreateInInventory("Truck_01_Hood_Orange");
			
		//-----IN CAR CARGO
		GetInventory().CreateInInventory("Truck_01_Wheel");
		GetInventory().CreateInInventory("Truck_01_Wheel");
		GetInventory().CreateInInventory("Truck_01_WheelDouble");
		GetInventory().CreateInInventory("Truck_01_WheelDouble");
	}
}

class Truck_01_Covered_Blue extends Truck_01_Covered
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GetInventory().CreateInInventory("Truck_01_Wheel");
		GetInventory().CreateInInventory("Truck_01_Wheel");

		GetInventory().CreateInInventory("Truck_01_WheelDouble");
		GetInventory().CreateInInventory("Truck_01_WheelDouble");
		GetInventory().CreateInInventory("Truck_01_WheelDouble");
		GetInventory().CreateInInventory("Truck_01_WheelDouble");

		GetInventory().CreateInInventory("Truck_01_Door_1_1_Blue");
		GetInventory().CreateInInventory("Truck_01_Door_2_1_Blue");
		GetInventory().CreateInInventory("Truck_01_Hood_Blue");
			
		//-----IN CAR CARGO
		GetInventory().CreateInInventory("Truck_01_Wheel");
		GetInventory().CreateInInventory("Truck_01_Wheel");
		GetInventory().CreateInInventory("Truck_01_WheelDouble");
		GetInventory().CreateInInventory("Truck_01_WheelDouble");
	}
}
