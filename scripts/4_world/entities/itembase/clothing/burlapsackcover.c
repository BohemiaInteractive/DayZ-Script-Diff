class BurlapSackCover extends HeadGear_Base
{
	protected PlayerBase m_Player;
		
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
		
		if (g_Game.IsDedicatedServer() && newLoc.GetType() == InventoryLocationType.GROUND)
		{
			EntityAI newItem = EntityAI.Cast(g_Game.CreateObjectEx("BurlapSack",newLoc.GetPos(),ECE_PLACE_ON_SURFACE,RF_DEFAULT));
			MiscGameplayFunctions.TransferItemProperties(this,newItem);
			DeleteSafe();
		}
	}
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		if (slot_id == InventorySlots.HEADGEAR)
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HandleAttachedToHead, 0);
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
			return false;
		
		if (parent && parent != this)
			return true;
		
		return false;
	}

	override bool CanDetachAttachment(EntityAI parent)
	{
		return false;
	}

	protected void OnRemovedFromHead(PlayerBase player)
	{
		if ((!g_Game.IsServer() || !g_Game.IsMultiplayer()) && PlayerBase.Cast(g_Game.GetPlayer()) == player) // Client side
		{
			PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects).Stop();
			player.SetInventorySoftLock(false);
			player.SetMasterAttenuation("");
		}
		
		if (g_Game.IsServer() || g_Game.IsMultiplayer()) // Server side or single player
			SetInvisibleRecursive(false, player, {InventorySlots.MASK, InventorySlots.EYEWEAR});
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
	
	protected void HandleAttachedToHead()
	{
		Class.CastTo(m_Player, GetHierarchyRootPlayer());
		if (!m_Player)
			return;
		
		if (!g_Game.IsServer() || !g_Game.IsMultiplayer()) // Client side
		{
			if (m_Player && PlayerBase.Cast(g_Game.GetPlayer()) == m_Player)
			{
				PPERequesterBase ppeRB = PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects);
				if (ppeRB)
					ppeRB.Start();
				
				m_Player.SetInventorySoftLock(true);
				m_Player.SetMasterAttenuation("BurlapSackAttenuation");
				
				if (g_Game.GetUIManager().IsMenuOpen(MENU_INVENTORY))
					g_Game.GetMission().HideInventory();
			}
		}
		
		if (g_Game.IsServer() || g_Game.IsMultiplayer()) // Server side or single player
			SetInvisibleRecursive(true, m_Player, {InventorySlots.MASK, InventorySlots.EYEWEAR});
	}
}
