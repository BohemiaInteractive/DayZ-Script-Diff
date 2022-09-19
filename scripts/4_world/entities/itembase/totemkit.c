class TerritoryFlagKit extends KitBase
{
	override bool PlacementCanBeRotated()
	{
		return false;
	}
	
	override bool DoPlacingHeightCheck()
	{
		return true;
	}
	
	override float HeightCheckOverride()
	{
		return 11.3;
	}

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
			//Create TerritoryFlag
			TerritoryFlag totem = TerritoryFlag.Cast( GetGame().CreateObjectEx( "TerritoryFlag", GetPosition(), ECE_PLACE_ON_SURFACE ) );
			totem.SetPosition( position );
			totem.SetOrientation( orientation );
			
			//make the kit invisible, so it can be destroyed from deploy UA when action ends
			HideAllSelections();
			
			SetIsDeploySound( true );
		}
	}

	override void DisassembleKit(ItemBase item)
	{
		if (!IsHologram())
		{
			ItemBase stick = ItemBase.Cast(GetGame().CreateObjectEx("WoodenStick",GetPosition(),ECE_PLACE_ON_SURFACE));
			MiscGameplayFunctions.TransferItemProperties(this, stick);
			stick.SetQuantity(3);
			Rope rope = Rope.Cast(item);
			CreateRope(rope);
		}
	}
	
	//================================================================
	// DEBUG
	//================================================================	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		SpawnEntityOnGroundPos("SledgeHammer", GetPosition());
		SpawnEntityOnGroundPos("Hammer", GetPosition());
		SpawnEntityOnGroundPos("Shovel", GetPosition());
		
		Nail nails = Nail.Cast(SpawnEntityOnGroundPos("Nail", GetPosition()));
		nails.SetQuantity(60);
		SpawnEntityOnGroundPos("Rope", GetPosition());
		SpawnEntityOnGroundPos("MetalWire", GetPosition());
		SpawnEntityOnGroundPos("Flag_DayZ", GetPosition());

		for (int i0 = 0; i0 < 32; i0++)
		{
			SpawnEntityOnGroundPos("Stone", GetPosition());
		}
		
		for (int i1 = 0; i1 < 10; i1++)
		{
			SpawnEntityOnGroundPos("WoodenLog", GetPosition());
		}
	}
}
