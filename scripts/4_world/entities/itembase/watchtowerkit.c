class WatchtowerKit extends KitBase
{
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		ItemBase att = ItemBase.Cast(GetInventory().FindAttachment(slotId));
		if (att)
			return false;
		
		return super.CanReceiveAttachment(attachment, slotId);
	}

	//================================================================
	// ADVANCED PLACEMENT
	//================================================================			
		
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
		
		if ( GetGame().IsServer() )
		{
			//Create watchtower
			Watchtower watchtower = Watchtower.Cast( GetGame().CreateObjectEx( "Watchtower", GetPosition(), ECE_PLACE_ON_SURFACE ) );
			watchtower.SetPosition( position );
			watchtower.SetOrientation( orientation );
			
			//make the kit invisible, so it can be destroyed from deploy UA when action ends
			HideAllSelections();
			
			SetIsDeploySound( true );
		}
	}
	
	override bool DoPlacingHeightCheck()
	{
		return true;
	}
	
	override float HeightCheckOverride()
	{
		return 2.83;//9.56;
	}
	
	override void DisassembleKit(ItemBase item)
	{
		if (!IsHologram())
		{
			ItemBase stick = ItemBase.Cast(GetGame().CreateObjectEx("WoodenStick",GetPosition(),ECE_PLACE_ON_SURFACE));
			MiscGameplayFunctions.TransferItemProperties(this, stick);
			stick.SetQuantity(4);
			Rope rope = Rope.Cast(item);
			CreateRope(rope);
		}
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		SpawnEntityOnGroundPos("Shovel", GetPosition());
		SpawnEntityOnGroundPos("Hammer", GetPosition());
		SpawnEntityOnGroundPos("Hammer", GetPosition());
		SpawnEntityOnGroundPos("Hammer", GetPosition());
		SpawnEntityOnGroundPos("Hammer", GetPosition());
		SpawnEntityOnGroundPos("Pliers", GetPosition());

		for (int i0 = 0; i0 < 30; i0++)
		{
			SpawnEntityOnGroundPos("WoodenPlank", GetPosition());
		}
		
		for (int i1 = 0; i1 < 10; i1++)
		{
			SpawnEntityOnGroundPos("WoodenLog", GetPosition());
		}
		
		for (int i2 = 0; i2 < 15; i2++)
		{
			SpawnEntityOnGroundPos("Nail", GetPosition());
		}	
		
		for (int i3 = 0; i3 < 9; i3++)
		{
			SpawnEntityOnGroundPos("CamoNet", GetPosition());
		}	
		
		for (int i4 = 0; i4 < 6; i4++)
		{
			SpawnEntityOnGroundPos("BarbedWire", GetPosition());
		}
	}
}
