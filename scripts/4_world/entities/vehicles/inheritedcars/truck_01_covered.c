class Truck_01_Covered extends Truck_01_Base {};

class Truck_01_Covered_Orange extends Truck_01_Covered
{
	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;

		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			
			entity.GetInventory().CreateInInventory( "Truck_01_WheelDouble" );
			entity.GetInventory().CreateInInventory( "Truck_01_WheelDouble" );
			entity.GetInventory().CreateInInventory( "Truck_01_WheelDouble" );
			entity.GetInventory().CreateInInventory( "Truck_01_WheelDouble" );

			entity.GetInventory().CreateInInventory( "TruckBattery" );

			entity.GetInventory().CreateInInventory( "Truck_01_Door_1_1_Orange" );
			entity.GetInventory().CreateInInventory( "Truck_01_Door_2_1_Orange" );
			entity.GetInventory().CreateInInventory( "Truck_01_Hood_Orange" );

			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			entity.GetInventory().CreateInInventory( "TruckBattery" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			//--
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			//--
			entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			ent = entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			if ( Class.CastTo(container, ent) )
			{
				container.SetLiquidType(LIQUID_WATER, true);
			}
		};

		Fill( CarFluid.FUEL, 120 );
		Fill( CarFluid.OIL, 4.0 );
	};
};

class Truck_01_Covered_Blue extends Truck_01_Covered
{
	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;

		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			
			entity.GetInventory().CreateInInventory( "Truck_01_WheelDouble" );
			entity.GetInventory().CreateInInventory( "Truck_01_WheelDouble" );
			entity.GetInventory().CreateInInventory( "Truck_01_WheelDouble" );
			entity.GetInventory().CreateInInventory( "Truck_01_WheelDouble" );

			entity.GetInventory().CreateInInventory( "TruckBattery" );

			entity.GetInventory().CreateInInventory( "Truck_01_Door_1_1_Blue" );
			entity.GetInventory().CreateInInventory( "Truck_01_Door_2_1_Blue" );
			entity.GetInventory().CreateInInventory( "Truck_01_Hood_Blue" );

			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			entity.GetInventory().CreateInInventory( "Truck_01_Wheel" );
			entity.GetInventory().CreateInInventory( "TruckBattery" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			//--
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			//--
			entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			ent = entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			if ( Class.CastTo(container, ent) )
			{
				container.SetLiquidType(LIQUID_WATER, true);
			}
		};

		Fill( CarFluid.FUEL, 120 );
		Fill( CarFluid.OIL, 4.0 );
	};
};