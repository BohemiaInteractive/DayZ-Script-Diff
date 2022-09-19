class MetalWire extends ItemBase
{
	static string SEL_WIRE_ROLLED	= "rolled";
	static string SEL_WIRE_PREFIX	= "Att_";
	static string SEL_WIRE_SUFIX	= "_plugged";
	static string SEL_PLUG_SUFIX	= "_plug";
	
	void MetalWire()
	{
		
	}
	
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent))
			return false;
		
		if ( parent.IsInherited(VehicleBattery) )
		{
			EntityAI battery_owner = EntityAI.Cast( parent.GetHierarchyParent() );
			
			// Check for Not player as if parent is not player, battery is already attached and should not receive new attachment
			if ( battery_owner && !battery_owner.IsInherited(PlayerBase) )
			{
				return false;
			}
		}
		
		return true;
	}
	
	void UpdateAllSelections()
	{
		HideAllSelections();
		EntityAI energy_source = GetCompEM().GetEnergySource();
		
		if (energy_source)
		{
			string ES_model = energy_source.GetModelName();
			EntityAI powered_device = GetCompEM().GetPluggedDevice();
			
			if (powered_device)
			{
				// Show metal wire selection
				string selection_wire = SEL_WIRE_PREFIX + ES_model + SEL_WIRE_SUFIX;
				ShowSelection(selection_wire);
				
				// Show plug selection
				string selection_plug = SEL_WIRE_PREFIX + ES_model + SEL_PLUG_SUFIX;
				selection_plug.ToLower();
				ShowSelection(selection_plug);
				
				// Set plug's texture
				int selection_index = GetHiddenSelectionIndex(selection_plug);
				string texture_path = powered_device.GetCompEM().GetCordTextureFile();
				SetObjectTexture( selection_index, texture_path );
			}
			else
			{
				// Show metal wire selection
				string selection_wire2 = SEL_WIRE_PREFIX + ES_model;
				ShowSelection(selection_wire2);
			}
		}
		else
		{
			ShowSelection(SEL_WIRE_ROLLED);
		}
	}
	
	// Event called on item when it is placed in the player(Man) inventory, passes the owner as a parameter
	override void OnInventoryEnter( Man player )
	{
		super.OnInventoryEnter( player );
		
		PlayerBase player_PB;
		Class.CastTo( player_PB, player );
		if ( player_PB.GetItemInHands() == this && GetCompEM().IsPlugged() )
		{
			return;
		}
		
		//Only unplug if we are "powering" something
		ItemBase powered_device = ItemBase.Cast( GetCompEM().GetPluggedDevice() );
		if ( powered_device )
		{
			GetCompEM().UnplugAllDevices();		
		
			if ( !player_PB.IsPlacingLocal() )
			{
				GetCompEM().UnplugThis();
			}
		}
	}
	
	// Called when THIS is attached to battery
	override void OnIsPlugged(EntityAI source_device)
	{
		UpdateAllSelections();
	}
	
	// Called when THIS is detached from battery
	override void OnIsUnplugged(EntityAI last_energy_source)
	{
		UpdateAllSelections();
		GetCompEM().UnplugAllDevices();
	}
	
	// Called when some device is plugged into THIS
	override void OnOwnSocketTaken( EntityAI device )
	{
		UpdateAllSelections();
	}
	
	// Called when some device is unplugged from THIS
	override void OnOwnSocketReleased( EntityAI device )
	{
		UpdateAllSelections();
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionRestrainTarget);
		AddAction(ActionRestrainSelf);
		AddAction(ActionAttachToConstruction);
		AddAction(ActionAttach);
	}
}