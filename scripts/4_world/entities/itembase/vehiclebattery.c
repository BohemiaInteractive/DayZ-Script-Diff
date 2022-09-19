class VehicleBattery : ItemBase
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent)) 
			return false;
		
		string obj_type = parent.GetType();
		if ( parent.IsInherited(BatteryCharger) )
		{
			BatteryCharger charger = BatteryCharger.Cast( parent );
			return charger.CanReceiveAttachment(this, InventorySlots.INVALID);
		}
		
		if (GetCompEM().GetPluggedDevice())
			return false;
		
		return true;
	}
	
	override bool CanDetachAttachment( EntityAI parent )
	{
		return true;
	}
	
	override bool CanReceiveAttachment( EntityAI attachment, int slotId)
	{
		if (GetCompEM().IsPlugged())
			return false;
		
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
	override bool CanPutIntoHands( EntityAI player ) 
	{
		if ( !super.CanPutIntoHands( parent ) )
		{
			return false;
		}
		
		if ( HasEnergyManager() )
		{
			ItemBase powered_device = ItemBase.Cast( GetCompEM().GetPluggedDevice() ); // Should return metal wire or barbed wire attachment
			
			if ( powered_device  &&  powered_device.IsInherited( MetalWire ) )
			{
				return true;
			}
			else if (powered_device)
			{
				return false;
			}
		}

		return true;
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		super.CanPutInCargo( parent );
		
		ItemBase powered_device = ItemBase.Cast( GetCompEM().GetPluggedDevice() ); // Should return metal wire or barbed wire attachment
			
		if ( powered_device )
		{
			//We block placing in cargo if metal wire is plugged
			//metal wires plugged to batteries in cargo cause too many issues for little gameplay gain
			if ( powered_device.IsInherited( MetalWire ) )
			{
				return false;
			}
		}
		return true;
	}
	
	override void OnInventoryEnter(Man player)
	{
		super.OnInventoryEnter(player);
		
		if (GetHierarchyParent() == player || (GetHierarchyParent() && GetHierarchyParent().GetInventory().GetCargo()))
		{
			if ( HasEnergyManager() )
			{
				ItemBase powered_device = ItemBase.Cast( GetCompEM().GetPluggedDevice() ); // Should return metal wire or barbed wire attachment
			
				if ( powered_device )
				{
					if ( powered_device.IsInherited( MetalWire ) )
					{
						//Unplug the device the wire is powering, but keep wire plugged to battery
						if ( powered_device.GetCompEM().IsPlugged() )
							powered_device.GetCompEM().UnplugDevice( powered_device.GetCompEM().GetPluggedDevice() );
					}
					else
					{
						this.GetCompEM().UnplugAllDevices();
					}
				}
			}
		}
	}
	
	override void OnMovedInsideCargo(EntityAI container)
	{
		super.OnMovedInsideCargo(container);
		
		if ( HasEnergyManager() )
		{
			ItemBase powered_device = ItemBase.Cast( GetCompEM().GetPluggedDevice() ); // Should return metal wire or barbed wire attachment
			
			if ( powered_device )
			{
				//Should not be possible, but better safe than sorry
				if ( powered_device.IsInherited( MetalWire ) )
				{
					powered_device.GetCompEM().UnplugAllDevices();
				}
				else
				{
					this.GetCompEM().UnplugAllDevices();
				}
			}
		}
	}
	
	override bool CanDisplayAttachmentSlot( int slot_id )
	{
		if ( GetCompEM().IsPlugged() )
			return false;
		return super.CanDisplayAttachmentSlot( slot_id );
	}
	
	override bool DisplayNameRuinAttach()
	{
		return true;
	}
	
	override bool ShowZonesHealth()
	{
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		//AddAction(ActionAttach);
		AddAction(ActionAttachOnSelection);
		AddAction(ActionDetach);
		//AddAction(ActionAttachPowerSourceToPanel); SHOULD NOT BE USED ANYMORE
		AddAction(ActionPlugTargetIntoThis);
	}
	
	override void OnQuantityChanged(float delta)
	{
		super.OnQuantityChanged(delta);
	}
	
	//------------------------------------
	//!ENERGY CONSUMPTION
	//------------------------------------
	
	private int				m_Efficiency0To10; // Synchronized variable
	static private float 	m_EfficiencyDecayStart = 0.1; // At this % of maximum energy the output of the battery starts to weaken.
	
	void VehicleBattery()
	{
		m_Efficiency0To10 = 10;
		RegisterNetSyncVariableInt("m_Efficiency0To10");
		
	}	
	
	//! Returns efficiency of this battery. The value is synchronized from server to all clients and is accurate down to 0.1 units.
	float GetEfficiency0To1()
	{
		return m_Efficiency0To10 * 0.1;
	}
	
	//! Returns efficiency of this battery. The value is synchronized from server to all clients and is accurate down to 0.1 unit.
	float GetEfficiencyDecayStart()
	{
		return m_EfficiencyDecayStart;
	}
	
	//Update Battery energy level before it overrides quantity
	override void SetCEBasedQuantity()
	{
		super.SetCEBasedQuantity();
		if ( GetCompEM() )
			GetCompEM().SetEnergy( GetCompEM().GetEnergyMax() * ( GetQuantity() / GetQuantityMax() ) );
	}
	
	override void OnEnergyConsumed()
	{
		super.OnEnergyConsumed();
		
		if ( GetGame().IsServer() )
		{
			float energy_coef = GetCompEM().GetEnergy0To1();
			
			if ( energy_coef < m_EfficiencyDecayStart  &&  m_EfficiencyDecayStart > 0 )			
			{
				m_Efficiency0To10 = Math.Round(  (energy_coef / m_EfficiencyDecayStart) * 10  );
			}
			SetSynchDirty();
		}
	}
		
	// BatteryCharging
	override void OnEnergyAdded()
	{
		super.OnEnergyAdded();
		
		if ( GetGame().IsServer() )
		{
			float energy_coef = GetCompEM().GetEnergy0To1();
			
			if ( energy_coef < m_EfficiencyDecayStart  &&  m_EfficiencyDecayStart > 0)
			{
				m_Efficiency0To10 = Math.Round(  (energy_coef / m_EfficiencyDecayStart) * 10  );
				SetSynchDirty();
			}
			else
			{
				m_Efficiency0To10 = 10;
				SetSynchDirty();
			}
		}
	}
}
