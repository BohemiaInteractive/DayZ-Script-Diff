class CableReel extends ItemBase
{
	bool m_ForceIntoHands;

	static const string SEL_CORD_FOLDED 			= "cord_folded";
	static const string SEL_CORD_PLUGGED 			= "cord_plugged";
	
	void CableReel () 
	{
		m_ForceIntoHands = false;
		RegisterNetSyncVariableBool("m_IsSoundSynchRemote");
		RegisterNetSyncVariableBool("m_IsPlaceSound");
	}
	
	override bool IsElectricAppliance()
	{
		return true;
	}
	
	void ForceIntoHandsNow( PlayerBase player ) 
	{
		m_ForceIntoHands = true;
		player.LocalTakeEntityToHands(this); // Local, because ForceIntoHandsNow is executed on both, Client and Server
		m_ForceIntoHands = false;
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if ( !super.CanPutInCargo(parent) ) {return false;}
		EntityAI owner_EAI = GetHierarchyParent();
		if ( owner_EAI  &&  owner_EAI.IsKindOf("Fence"))
		{
			return true;
		}
		
		bool allow_into_inv = !GetCompEM().IsPlugged();
		return allow_into_inv;
	}

	override bool CanPutIntoHands ( EntityAI player ) 
	{
		if ( !super.CanPutIntoHands( parent ) )
		{
			return false;
		}
		if ( m_ForceIntoHands )
		{
			return true;
		}
		else
		{
			EntityAI owner_EAI = GetHierarchyParent();
			if ( owner_EAI  &&  owner_EAI.IsKindOf("Fence"))
			{
				return true;
			}
		}
		
		return true;
		
		/*bool allow_into_inv = !GetCompEM().IsPlugged();
		return allow_into_inv;*/
	}

	// Event called on item when it is placed in the player(Man) inventory, passes the owner as a parameter
	override void OnInventoryEnter(Man player)
	{
		super.OnInventoryEnter(player);
		
		PlayerBase player_PB;
		Class.CastTo(player_PB, player);
		if (player_PB.GetItemInHands() == this && GetCompEM().IsPlugged())
		{
			//player_PB.TogglePlacingLocal();
			return;
		}
		
		GetCompEM().UnplugAllDevices();		
		
		if ( !player_PB.IsPlacingLocal() )
		{
			GetCompEM().UnplugThis();
		}
	}
	
	override bool CanRemoveFromHands( EntityAI player ) 
	{
		return true;
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
						
		if ( IsPlaceSound() )
		{
			PlayPlaceSound();
		}
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override void OnPlacementStarted( Man player )
	{
		super.OnPlacementStarted( player );
		
		ref array<string> array_of_selections = {SEL_CORD_PLUGGED, SEL_CORD_FOLDED};
		PlayerBase player_PB = PlayerBase.Cast( player );
				
		if ( GetGame().IsMultiplayer() && GetGame().IsServer() )
		{
			if ( player_PB.GetHologramServer() )
				player_PB.GetHologramServer().SetSelectionToRefresh( array_of_selections );
		}
		else
		{
			player_PB.GetHologramLocal().SetSelectionToRefresh( array_of_selections );
		}
	}
	
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
		
		SetIsPlaceSound( true );
	}
	
	override string GetPlaceSoundset()
	{
		return "placeCableReel_SoundSet";
	}
	
	override void SetActions()
	{
		super.SetActions();
		RemoveAction(ActionTakeItemToHands);
		
		//AddAction(ActionClapBearTrapWithThisItem);
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