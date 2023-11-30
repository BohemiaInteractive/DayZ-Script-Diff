class BurlapSackCover extends HeadGear_Base
{
	PlayerBase m_Player;

	void ~BurlapSackCover()
	{
		if (m_Player)
		{
			OnRemovedFromHead(m_Player);
		}
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc,newLoc);
		
		if (GetGame().IsDedicatedServer() && newLoc.GetType() == InventoryLocationType.GROUND)
		{
			EntityAI newItem = EntityAI.Cast(GetGame().CreateObjectEx("BurlapSack",newLoc.GetPos(),ECE_PLACE_ON_SURFACE,RF_DEFAULT));
			MiscGameplayFunctions.TransferItemProperties(this,newItem);
			DeleteSafe();
		}
	}

	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		Class.CastTo(m_Player, parent.GetHierarchyRootPlayer());

		if ((!GetGame().IsDedicatedServer()) && m_Player && m_Player.IsControlledPlayer() && slot_id == InventorySlots.HEADGEAR)
		{
			PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects).Start();
			m_Player.SetInventorySoftLock(true);
			m_Player.SetMasterAttenuation("BurlapSackAttenuation");
			
			if (GetGame().GetUIManager().IsMenuOpen(MENU_INVENTORY))
			{
				GetGame().GetMission().HideInventory();
			}
		}
		SetInvisibleRecursive(true,m_Player,{InventorySlots.MASK,InventorySlots.EYEWEAR});
	}

	override bool CanPutInCargo( EntityAI parent )
	{
		if (!super.CanPutInCargo(parent))
			return false;
		
		if (parent && parent != this)
			return true;
		
		return false;
	}

	override bool CanDetachAttachment( EntityAI parent )
	{
		return false;
	}

	void OnRemovedFromHead(PlayerBase player)
	{
		if (player.IsControlledPlayer())
		{
			PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects).Stop();
			player.SetInventorySoftLock(false);
			player.SetMasterAttenuation("");
		}
		SetInvisibleRecursive(false,player,{InventorySlots.MASK,InventorySlots.EYEWEAR});
	}
	
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		if (slotId == InventorySlots.HEADGEAR)
		{
			ret.Insert(EAttExclusions.SHAVING_HEADGEAR_ATT_0);
		}
		return ret;
	}
}
