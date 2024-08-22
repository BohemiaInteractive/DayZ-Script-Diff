class CableReel extends ItemBase
{
	bool m_ForceIntoHands;

	static const string SEL_CORD_FOLDED		= "cord_folded";
	static const string SEL_CORD_PLUGGED 	= "cord_plugged";
	
	void CableReel()
	{
		m_ForceIntoHands = false;
		RegisterNetSyncVariableBool("m_IsSoundSynchRemote");
		RegisterNetSyncVariableBool("m_IsPlaceSound");
	}
	
	override bool IsElectricAppliance()
	{
		return true;
	}
	
	void ForceIntoHandsNow(PlayerBase player)
	{
		m_ForceIntoHands = true;
		player.LocalTakeEntityToHands(this); // Local, because ForceIntoHandsNow is executed on both, Client and Server
		m_ForceIntoHands = false;
	}
	
	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
			return false;

		EntityAI owner = GetHierarchyParent();
		if (owner && owner.IsKindOf("Fence"))
			return true;
		
		bool allowIntoInventory = !GetCompEM().IsPlugged();
		return allowIntoInventory;
	}

	override bool CanPutIntoHands(EntityAI player) 
	{
		if (!super.CanPutIntoHands(parent))
			return false;

		if (m_ForceIntoHands)
		{
			return true;
		}
		else
		{
			EntityAI owner = GetHierarchyParent();
			if (owner && owner.IsKindOf("Fence"))
				return true;
		}
		
		return true;
	}

	// Event called on item when it is placed in the player(Man) inventory, passes the owner as a parameter
	override void OnInventoryEnter(Man player)
	{
		super.OnInventoryEnter(player);
		
		PlayerBase playerPB = PlayerBase.Cast(player);
		if (playerPB.GetItemInHands() == this && GetCompEM().IsPlugged())
			return;
		
		GetCompEM().UnplugAllDevices();		
		
		if (!playerPB.IsPlacingLocal())
			GetCompEM().UnplugThis();
	}
	
	override bool CanRemoveFromHands(EntityAI player)
	{
		return true;
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
						
		if (IsPlaceSound())
			PlayPlaceSound();
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override void OnPlacementStarted(Man player)
	{
		super.OnPlacementStarted(player);

		array<string> selections = {
			SEL_CORD_PLUGGED,
			SEL_CORD_FOLDED
		};
		
		PlayerBase playerPB = PlayerBase.Cast(player);
		if (GetGame().IsMultiplayer() && GetGame().IsServer())
			playerPB.GetHologramServer().SetSelectionToRefresh(selections);		
		else
			playerPB.GetHologramLocal().SetSelectionToRefresh(selections);
	}
	
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		
		SetIsPlaceSound(true);
	}
	
	override string GetPlaceSoundset()
	{
		return "placeCableReel_SoundSet";
	}
	
	override void SetActions()
	{
		super.SetActions();

		RemoveAction(ActionTakeItemToHands);
		
		AddAction(ActionPlugIn);
		AddAction(ActionPlugTargetIntoThis);
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionPullOutPlug);
		AddAction(ActionUnplugThisByCord);
		AddAction(ActionRepositionPluggedItem);
		AddAction(ActionPlaceObject);
		AddAction(ActionTakeItemToHands);
	}
}