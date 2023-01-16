class VehicleBattery : ItemBase
{
	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (!super.CanPutAsAttachment(parent)) 
			return false;
		
		if (parent.IsInherited(BatteryCharger))
		{
			BatteryCharger charger = BatteryCharger.Cast(parent);
			return charger.CanReceiveAttachment(this, InventorySlots.INVALID);
		}
		
		if (GetCompEM().GetPluggedDevice())
			return false;
		
		return true;
	}
	
	override bool CanDetachAttachment(EntityAI parent)
	{
		return true;
	}
	
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if (GetCompEM().IsPlugged())
			return false;
		
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
	override bool CanPutIntoHands(EntityAI player)
	{
		if (!super.CanPutIntoHands(parent))
		{
			return false;
		}
		
		if (HasEnergyManager())
		{
			ItemBase poweredDevice = ItemBase.Cast(GetCompEM().GetPluggedDevice());
			if (poweredDevice && poweredDevice.IsInherited(MetalWire))
			{
				return true;
			}
			else if (poweredDevice)
			{
				return false;
			}
		}

		return true;
	}
	
	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
		{
			return false;
		}
		
		ItemBase poweredDevice = ItemBase.Cast(GetCompEM().GetPluggedDevice());

		return !(poweredDevice && poweredDevice.IsInherited(MetalWire));
	}
	
	override void OnInventoryEnter(Man player)
	{
		super.OnInventoryEnter(player);
		
		if (GetHierarchyParent() == player || (GetHierarchyParent() && GetHierarchyParent().GetInventory().GetCargo()))
		{
			if (HasEnergyManager())
			{
				ItemBase poweredDevice = ItemBase.Cast(GetCompEM().GetPluggedDevice());
			
				if (poweredDevice)
				{
					if (poweredDevice.IsInherited(MetalWire))
					{
						//Unplug the device the wire is powering, but keep wire plugged to battery
						if (poweredDevice.GetCompEM().IsPlugged())
							poweredDevice.GetCompEM().UnplugDevice(poweredDevice.GetCompEM().GetPluggedDevice());
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
		
		if (HasEnergyManager())
		{
			ItemBase poweredDevice = ItemBase.Cast(GetCompEM().GetPluggedDevice());
			
			if (poweredDevice)
			{
				//Should not be possible, but better safe than sorry
				if (poweredDevice.IsInherited(MetalWire))
				{
					poweredDevice.GetCompEM().UnplugAllDevices();
				}
				else
				{
					this.GetCompEM().UnplugAllDevices();
				}
			}
		}
	}
	
	override bool CanDisplayAttachmentSlot(int slot_id)
	{
		if (GetCompEM().IsPlugged())
			return false;

		return super.CanDisplayAttachmentSlot(slot_id);
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
		
		AddAction(ActionAttachOnSelection);
		AddAction(ActionDetach);
		AddAction(ActionPlugTargetIntoThis);
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

		if (GetCompEM())
			GetCompEM().SetEnergy( GetCompEM().GetEnergyMax() * (GetQuantity() / GetQuantityMax()));
	}
	
	override void OnEnergyConsumed()
	{
		super.OnEnergyConsumed();
		
		#ifdef SERVER
		float energyCoef = GetCompEM().GetEnergy0To1();
		
		if (energyCoef < m_EfficiencyDecayStart && m_EfficiencyDecayStart > 0)
		{
			m_Efficiency0To10 = Math.Round((energyCoef / m_EfficiencyDecayStart) * 10);
		}

		SetSynchDirty();
		#endif
	}
		
	// BatteryCharging
	override void OnEnergyAdded()
	{
		super.OnEnergyAdded();
		
		#ifdef SERVER
		float energyCoef = GetCompEM().GetEnergy0To1();
		
		if (energyCoef < m_EfficiencyDecayStart && m_EfficiencyDecayStart > 0)
		{
			m_Efficiency0To10 = Math.Round((energyCoef / m_EfficiencyDecayStart) * 10);
		}
		else
		{
			m_Efficiency0To10 = 10;
		}
		
		SetSynchDirty();
		#endif
	}
}
