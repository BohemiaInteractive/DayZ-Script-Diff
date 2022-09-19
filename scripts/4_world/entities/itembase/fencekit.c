class FenceKit extends KitBase
{
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if ( !super.CanReceiveAttachment(attachment, slotId) )
			return false;
		
		ItemBase att = ItemBase.Cast(GetInventory().FindAttachment(slotId));
		if (att)
			return false;
		
		return true;
	}

	//================================================================
	// ADVANCED PLACEMENT
	//================================================================			
		
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
		
		if ( GetGame().IsServer() )
		{
			//Create fence
			
			Fence fence = Fence.Cast( GetGame().CreateObjectEx( "Fence", GetPosition(), ECE_PLACE_ON_SURFACE ) );
			fence.SetPosition( position );
			fence.SetOrientation( orientation );
			
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
		return 2.54;
	}
	
	override void DisassembleKit(ItemBase item)
	{
		if (!IsHologram())
		{
			ItemBase stick = ItemBase.Cast(GetGame().CreateObjectEx("WoodenStick",GetPosition(),ECE_PLACE_ON_SURFACE));
			MiscGameplayFunctions.TransferItemProperties(this, stick);
			stick.SetQuantity(2);
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
		SpawnEntityOnGroundPos("Pliers", GetPosition());

		SpawnEntityOnGroundPos("WoodenLog", GetPosition());
		SpawnEntityOnGroundPos("WoodenLog", GetPosition());
		SpawnEntityOnGroundPos("Nail", GetPosition());
		SpawnEntityOnGroundPos("CamoNet", GetPosition());
		SpawnEntityOnGroundPos("BarbedWire", GetPosition());
		SpawnEntityOnGroundPos("BarbedWire", GetPosition());
		SpawnEntityOnGroundPos("MetalWire", GetPosition());
		SpawnEntityOnGroundPos("CombinationLock", GetPosition());
		SpawnEntityOnGroundPos("WoodenPlank", GetPosition());
		SpawnEntityOnGroundPos("WoodenPlank", GetPosition());
		SpawnEntityOnGroundPos("WoodenPlank", GetPosition());
		SpawnEntityOnGroundPos("WoodenPlank", GetPosition());
	}
}
