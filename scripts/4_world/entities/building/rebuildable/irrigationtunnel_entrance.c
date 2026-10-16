class Land_IrrigationTunnel_Entrance_01 : BuildingSuper
{		
	override void OnDebugSpawn()
	{
		GameInventory inventory = GetInventory();
		ItemBase item;
		if (Class.CastTo(item, inventory.CreateInInventory("WoodenStick")))
			item.SetQuantityMax();
		if (Class.CastTo(item, inventory.CreateInInventory("Rope")))
			item.SetQuantityMax();
	}
};
