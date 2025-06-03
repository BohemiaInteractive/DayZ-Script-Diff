// remove by 1.28

class CivilianSedan_TEST : CivilianSedan
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GetInventory().CreateInInventory("CivSedanWheel_TEST");
		GetInventory().CreateInInventory("CivSedanWheel_TEST");
		GetInventory().CreateInInventory("CivSedanWheel_TEST");
		GetInventory().CreateInInventory("CivSedanWheel_TEST");

		GetInventory().CreateInInventory("CivSedanDoors_Driver");
		GetInventory().CreateInInventory("CivSedanDoors_CoDriver");
		GetInventory().CreateInInventory("CivSedanDoors_BackLeft");
		GetInventory().CreateInInventory("CivSedanDoors_BackRight");
		GetInventory().CreateInInventory("CivSedanHood");
		GetInventory().CreateInInventory("CivSedanTrunk");

		//-----IN CAR CARGO
		GetInventory().CreateInInventory("CivSedanWheel_TEST");
		GetInventory().CreateInInventory("CivSedanWheel_TEST");
	}
};

class Hatchback_02_TEST : Hatchback_02
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();
		
		GetInventory().CreateInInventory("Hatchback_02_Wheel_TEST");
		GetInventory().CreateInInventory("Hatchback_02_Wheel_TEST");
		GetInventory().CreateInInventory("Hatchback_02_Wheel_TEST");
		GetInventory().CreateInInventory("Hatchback_02_Wheel_TEST");

		GetInventory().CreateInInventory("Hatchback_02_Door_1_1");
		GetInventory().CreateInInventory("Hatchback_02_Door_1_2");
		GetInventory().CreateInInventory("Hatchback_02_Door_2_1");
		GetInventory().CreateInInventory("Hatchback_02_Door_2_2");
		GetInventory().CreateInInventory("Hatchback_02_Hood");
		GetInventory().CreateInInventory("Hatchback_02_Trunk");

		//-----IN CAR CARGO
		GetInventory().CreateInInventory("Hatchback_02_Wheel_TEST");
		GetInventory().CreateInInventory("Hatchback_02_Wheel_TEST");
	}
};

class Offroad_02_TEST : Offroad_02
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GetInventory().CreateInInventory("Offroad_02_Wheel_TEST");
		GetInventory().CreateInInventory("Offroad_02_Wheel_TEST");
		GetInventory().CreateInInventory("Offroad_02_Wheel_TEST");
		GetInventory().CreateInInventory("Offroad_02_Wheel_TEST");

		GetInventory().CreateInInventory("Offroad_02_Door_1_1");
		GetInventory().CreateInInventory("Offroad_02_Door_1_2");
		GetInventory().CreateInInventory("Offroad_02_Door_2_1");
		GetInventory().CreateInInventory("Offroad_02_Door_2_2");
		GetInventory().CreateInInventory("Offroad_02_Hood");
		GetInventory().CreateInInventory("Offroad_02_Trunk");

		//-----IN CAR CARGO
		GetInventory().CreateInInventory("Offroad_02_Wheel_TEST");
		GetInventory().CreateInInventory("Offroad_02_Wheel_TEST");
	}
};

class OffroadHatchback_TEST : OffroadHatchback
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GetInventory().CreateInInventory("HatchbackWheel_TEST");
		GetInventory().CreateInInventory("HatchbackWheel_TEST");
		GetInventory().CreateInInventory("HatchbackWheel_TEST" );
		GetInventory().CreateInInventory("HatchbackWheel_TEST");

		GetInventory().CreateInInventory("HatchbackDoors_Driver");
		GetInventory().CreateInInventory("HatchbackDoors_CoDriver");
		GetInventory().CreateInInventory("HatchbackHood");
		GetInventory().CreateInInventory("HatchbackTrunk");

		//-----IN CAR CARGO
		GetInventory().CreateInInventory("HatchbackWheel_TEST");
		GetInventory().CreateInInventory("HatchbackWheel_TEST");
	}
};

class Sedan_02_TEST : Sedan_02
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GetInventory().CreateInInventory("Sedan_02_Wheel_TEST");
		GetInventory().CreateInInventory("Sedan_02_Wheel_TEST");
		GetInventory().CreateInInventory("Sedan_02_Wheel_TEST");
		GetInventory().CreateInInventory("Sedan_02_Wheel_TEST");
		GetInventory().CreateInInventory("Sedan_02_Wheel_TEST");

		GetInventory().CreateInInventory("Sedan_02_Door_1_1");
		GetInventory().CreateInInventory("Sedan_02_Door_1_2");
		GetInventory().CreateInInventory("Sedan_02_Door_2_1");
		GetInventory().CreateInInventory("Sedan_02_Door_2_2");
		GetInventory().CreateInInventory("Sedan_02_Hood");
		GetInventory().CreateInInventory("Sedan_02_Trunk");

		//-----IN CAR CARGO
		GetInventory().CreateInInventory("Sedan_02_Wheel_TEST");
		GetInventory().CreateInInventory("Sedan_02_Wheel_TEST");
	}
};

class Truck_01_Covered_TEST : Truck_01_Covered
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GetInventory().CreateInInventory("Truck_01_Wheel_TEST");
		GetInventory().CreateInInventory("Truck_01_Wheel_TEST");

		GetInventory().CreateInInventory("Truck_01_WheelDouble_TEST");
		GetInventory().CreateInInventory("Truck_01_WheelDouble_TEST");
		GetInventory().CreateInInventory("Truck_01_WheelDouble_TEST");
		GetInventory().CreateInInventory("Truck_01_WheelDouble_TEST");

		GetInventory().CreateInInventory("Truck_01_Door_1_1_Orange");
		GetInventory().CreateInInventory("Truck_01_Door_2_1_Orange");
		GetInventory().CreateInInventory("Truck_01_Hood_Orange");
			
		//-----IN CAR CARGO
		GetInventory().CreateInInventory("Truck_01_Wheel_TEST");
		GetInventory().CreateInInventory("Truck_01_Wheel_TEST");
		GetInventory().CreateInInventory("Truck_01_WheelDouble_TEST");
		GetInventory().CreateInInventory("Truck_01_WheelDouble_TEST");
	}
};
