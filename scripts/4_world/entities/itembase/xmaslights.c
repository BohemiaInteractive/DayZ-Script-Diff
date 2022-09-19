class XmasLights extends Inventory_Base
{
	private ItemBase m_AttachedTo = NULL;
	
	void XmasLights()
	{
		TurnOffItem( this );
	}
	
	override bool IsElectricAppliance()
	{
		return true;
	}

	override void OnWorkStart()
	{
		TurnOnItem( m_AttachedTo );
		TurnOnItem( this );
	}

	override void OnWorkStop()
	{
		TurnOffItem( m_AttachedTo );
		TurnOffItem( this );
	}

	void AttachToObject(ItemBase parent)
	{
		//SetPilotLight( false );	
		
		m_AttachedTo = parent;
		TurnOnItem( parent );

		if ( GetCompEM().IsPlugged() )
		{
			parent.SetAnimationPhase( "Cord_plugged", 0);
			parent.SetAnimationPhase( "Cord_folded", 1);
		}
		else
		{
			parent.SetAnimationPhase( "Cord_plugged", 1);
			parent.SetAnimationPhase( "Cord_folded", 0);
		}
	}

	void DetachFromObject(ItemBase parent)
	{
		m_AttachedTo = NULL;
		HideOnItem( parent );
		TurnOnItem( this );
		
		GetCompEM().UnplugThis();
	}

	void TurnOnItem(ItemBase item)
	{
		if ( item != NULL )
		{
			if ( GetCompEM().IsWorking() )
			{
				item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_r"), "dz\\gear\\camping\\data\\christmas_lights_red_on.rvmat" );
				item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_g"), "dz\\gear\\camping\\data\\christmas_lights_green_on.rvmat" );
				item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_b"), "dz\\gear\\camping\\data\\christmas_lights_blue_on.rvmat" );
				item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_y"), "dz\\gear\\camping\\data\\christmas_lights_yellow_on.rvmat" );
				item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_r"), "dz\\gear\\camping\\data\\christmas_lights_string_ca.paa" );
				item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_g"), "dz\\gear\\camping\\data\\christmas_lights_string_ca.paa" );
				item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_b"), "dz\\gear\\camping\\data\\christmas_lights_string_ca.paa" );
				item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_y"), "dz\\gear\\camping\\data\\christmas_lights_string_ca.paa" );
				//item.SetPilotLight(true);	
			}
			else
			{
				TurnOffItem( item );
			}
		}
	}

	void TurnOffItem(ItemBase item)
	{
		if ( item != NULL )
		{
			item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_r"), "dz\\gear\\camping\\data\\christmass_ligths_string.rvmat" );
			item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_g"), "dz\\gear\\camping\\data\\christmass_ligths_string.rvmat" );
			item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_b"), "dz\\gear\\camping\\data\\christmass_ligths_string.rvmat" );
			item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_y"), "dz\\gear\\camping\\data\\christmass_ligths_string.rvmat" );
			item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_r"), "dz\\gear\\camping\\data\\christmas_lights_string_ca.paa" );
			item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_g"), "dz\\gear\\camping\\data\\christmas_lights_string_ca.paa" );
			item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_b"), "dz\\gear\\camping\\data\\christmas_lights_string_ca.paa" );
			item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_y"), "dz\\gear\\camping\\data\\christmas_lights_string_ca.paa" );
			//item.SetPilotLight(false);
		}
	}

	void HideOnItem(ItemBase item)
	{
		item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_r"), "" );
		item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_g"), "" );
		item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_b"), "" );
		item.SetObjectMaterial( item.GetHiddenSelectionIndex("xlights_glass_y"), "" );
		item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_r"), "" );
		item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_g"), "" );
		item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_b"), "" );
		item.SetObjectTexture( item.GetHiddenSelectionIndex("xlights_glass_y"), "" );
//		item.SetPilotLight(false);
		item.SetAnimationPhase( "Cord_plugged", 1);
		item.SetAnimationPhase( "Cord_folded", 1);
	}
	
	// --------------------------------------------------
	// Advanced placement
	//---------------------------------------------------
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttachOnProxy);
		AddAction(ActionPlugIn);
		AddAction(ActionUnplugThisByCord);
	}
	
}
