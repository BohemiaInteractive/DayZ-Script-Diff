class BurlapSackCover extends ClothingBase
{
	PlayerBase m_Player;

	void ~BurlapSackCover()
	{
		if( m_Player )
		{
			OnRemovedFromHead(m_Player);
		}
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc,newLoc);
		
		if (GetGame().IsDedicatedServer() && newLoc.GetType() == InventoryLocationType.GROUND)
		{
			if (m_Player)
			{
				MiscGameplayFunctions.TurnItemIntoItem(this, "BurlapSack", m_Player);
			}
		}
	}

	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		Class.CastTo(m_Player, parent.GetHierarchyRootPlayer());
		//bool selected = player.IsPlayerSelected();//this is 0 upon player's connection to server

		if ( (!GetGame().IsDedicatedServer()) && m_Player && m_Player.IsControlledPlayer() && slot_id == InventorySlots.HEADGEAR )
		{
			//GetGame().GetWorld().SetAperture(100000);
			PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects).Start();
			m_Player.SetInventorySoftLock(true);
			m_Player.SetMasterAttenuation("BurlapSackAttenuation");
			
			if ( GetGame().GetUIManager().IsMenuOpen(MENU_INVENTORY) )
			{
				GetGame().GetMission().HideInventory();
			}
		}
		SetInvisibleRecursive(true,m_Player,{InventorySlots.MASK,InventorySlots.EYEWEAR});
	}

	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		
		if ( GetGame().IsServer() )
		{
			PlayerBase player;
			Class.CastTo(player, parent.GetHierarchyRootPlayer());
			//MiscGameplayFunctions.TurnItemIntoItem(this, "BurlapSack", player);
		}
	}

	override bool CanPutInCargo( EntityAI parent )
	{
		if( !super.CanPutInCargo(parent) ) {return false;}
		if ( parent != this )
		{
			return true;
		}
		return false;
	}

	override bool CanDetachAttachment( EntityAI parent )
	{
		PlayerBase player;
		Class.CastTo(player, GetHierarchyRootPlayer());
		return false; // (player && !player.IsAlive());
	}

	void OnRemovedFromHead(PlayerBase player)
	{
		if ( player.IsControlledPlayer() )
		{
			PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects).Stop();
			player.SetInventorySoftLock(false);
			player.SetMasterAttenuation("");
		}
		SetInvisibleRecursive(false,player,{InventorySlots.MASK,InventorySlots.EYEWEAR});
	}
}