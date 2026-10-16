class Bandana_ColorBase: Clothing
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionBandageTarget);
		AddAction(ActionBandageSelf);
		AddAction(ActionWringClothes);
	}
	
	void Bandana_ColorBase()
	{

	}
		
	override bool CanBeDisinfected()
	{
		return true;
	}
	
	override float GetBandagingEffectivity()
	{
		return 0.5;
	};
	
	override float GetInfectionChance(int system = 0, Param param = null)
	{
		if(m_Cleanness == 1)
		{
			return 0.00;
		}
		else
		{
			return 0.15;
		}
	}
	
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		switch (slotId)
		{
			case InventorySlots.HEADGEAR:
				return ret;//no discernable conflict here
			break;
			
			default:
				ret.Insert(EAttExclusions.EXCLUSION_MASK_3);
				
				ret.Insert(EAttExclusions.SHAVING_MASK_ATT_0);
			break;
		}
		return ret;
	}
};
class Bandana_RedPattern: Bandana_ColorBase {};
class Bandana_BlackPattern: Bandana_ColorBase {};
class Bandana_PolkaPattern: Bandana_ColorBase {};
class Bandana_Greenpattern: Bandana_ColorBase {};
class Bandana_CamoPattern: Bandana_ColorBase {};
class Bandana_Blue: Bandana_ColorBase {};
class Bandana_Pink: Bandana_ColorBase {};
class Bandana_Yellow: Bandana_ColorBase {};
class Shemag_ColorBase: Clothing
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionBandageTarget);
		AddAction(ActionBandageSelf);
		AddAction(ActionWringClothes);
	}
		
	override bool CanBeDisinfected()
	{
		return true;
	}
	
	override float GetBandagingEffectivity()
	{
		return 0.5;
	}
	
	override float GetInfectionChance(int system = 0, Param param = null)
	{
		if (m_Cleanness == 1)
			return 0.00;

		return 0.15;
	}
	
	override ECachedEquipmentItemCategory GetCachedEquipmentCategory()
	{
		return ECachedEquipmentItemCategory.MASK;
	}

	override float GetProtectionLevel(int type, bool consider_filter = false, int system = 0)
	{
		if (IsDamageDestroyed())
			return 0.0;

		InventoryLocation inventoryLocation = new InventoryLocation();	
		GetInventory().GetCurrentInventoryLocation(inventoryLocation);
		
		if (inventoryLocation.GetSlot() == InventorySlots.MASK)
		{	
			switch (type)
			{
				case DEF_DUST_PARTICLE_BREATH:
					return 0.65;
				case DEF_DUST_PARTICLE_EYES:
					return 0.0;
			}
		}
		
		return super.GetProtectionLevel(type, consider_filter, system);
	}
	
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		
		switch (slotId)
		{
			case InventorySlots.HEADGEAR:
				ret.Insert(EAttExclusions.EXCLUSION_MASK_2);
			break;
			
			default:
				ret.Insert(EAttExclusions.EXCLUSION_MASK_2);
				ret.Insert(EAttExclusions.EXCLUSION_MASK_3);
				
				ret.Insert(EAttExclusions.SHAVING_MASK_ATT_0);
			break;
		}
		return ret;
	}
}

class Shemag_Brown: Shemag_ColorBase {}
class Shemag_Green: Shemag_ColorBase {}
class Shemag_Red: Shemag_ColorBase {}
class Shemag_White: Shemag_ColorBase {}

class Keffiyeh_ColorBase: Clothing
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionBandageTarget);
		AddAction(ActionBandageSelf);
		AddAction(ActionWringClothes);
	}
	
	void Keffiyeh_ColorBase()
	{

	}
		
	override bool CanBeDisinfected()
	{
		return true;
	}
	
	override float GetBandagingEffectivity()
	{
		return 0.5;
	};
	
	override float GetInfectionChance(int system = 0, Param param = null)
	{
		if(m_Cleanness == 1)
		{
			return 0.00;
		}
		else
		{
			return 0.15;
		}
	}
	
	override ECachedEquipmentItemCategory GetCachedEquipmentCategory()
	{
		return ECachedEquipmentItemCategory.MASK;
	}

	override float GetProtectionLevel(int type, bool consider_filter = false, int system = 0)
	{
		if (IsDamageDestroyed())
			return 0.0;

		InventoryLocation inventoryLocation = new InventoryLocation();	
		GetInventory().GetCurrentInventoryLocation(inventoryLocation);
		
		if (inventoryLocation.GetSlot() == InventorySlots.MASK)
		{	
			switch (type)
			{
				case DEF_DUST_PARTICLE_BREATH:
					return 0.65;
				case DEF_DUST_PARTICLE_EYES:
					return 0.0;
			}
		}
		
		return super.GetProtectionLevel(type, consider_filter, system);
	}
	
	override protected set<int> GetAttachmentExclusionInitSlotValue(int slotId)
	{
		set<int> ret = super.GetAttachmentExclusionInitSlotValue(slotId);
		switch (slotId)
		{
			case InventorySlots.HEADGEAR:
				return ret;//no discernable conflict here
			break;
			
			default:
				ret.Insert(EAttExclusions.EXCLUSION_MASK_3);
				
				ret.Insert(EAttExclusions.SHAVING_MASK_ATT_0);
			break;
		}
		return ret;
	}
};
class Keffiyeh_LightBlue: Keffiyeh_ColorBase {};
class Keffiyeh_LightRed: Keffiyeh_ColorBase {};
class Keffiyeh_Brown: Keffiyeh_ColorBase {};
class Keffiyeh_Gray: Keffiyeh_ColorBase {};
class Keffiyeh_LightBrown: Keffiyeh_ColorBase {};
class Keffiyeh_Red: Keffiyeh_ColorBase {};
class Keffiyeh_Blue: Keffiyeh_ColorBase {};
class Keffiyeh_Black: Keffiyeh_ColorBase {};

class BatteryD: ItemBase {};
class Bone: ItemBase 
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionCraftBoneKnife);
		AddAction(ActionCraftBoneKnifeEnv);
	}
};
class BurlapStrip: ItemBase {};
class ButaneCanister: ItemBase {};
class DuctTape: ItemBase 
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionRepairTent);
		AddAction(ActionRepairShelter);
		//AddAction(ActionRepairPart);
		AddAction(ActionRestrainTarget);
		AddAction(ActionRestrainSelf);
	}
};
class Flashbang: ItemBase {};
class LargeGasCanister: ItemBase
{
	override bool CanSwitchDuringAttach(EntityAI parent)
	{
		return true;
	}
	
	override string GetDestructionBehaviour()
    {
        return "DestructionEffectGasCanister";
    }
 
    override bool IsDestructionBehaviour()
    {	
		
		if (GetQuantity() > 0)
		{
			return true;
		}
		        
		else
		{
			return false;
		}
    }
};

class MediumGasCanister: ItemBase
{
	override bool CanSwitchDuringAttach(EntityAI parent)
	{
		return true;
	}
	
	override string GetDestructionBehaviour()
    {
        return "DestructionEffectGasCanister";
    }
 
    override bool IsDestructionBehaviour()
    {	
		
		if (GetQuantity() > 0)
		{
			return true;
		}
		        
		else
		{
			return false;
		}
    }
};

class NailBox: Box_Base {};
class Netting: ItemBase {};
class SmallGasCanister: ItemBase
{
	override bool CanSwitchDuringAttach(EntityAI parent)
	{
		return true;
	}
	
	override string GetDestructionBehaviour()
    {
        return "DestructionEffectGasCanister";
    }
 
    override bool IsDestructionBehaviour()
    {	
		
		if (GetQuantity() > 0)
		{
			return true;
		}
		        
		else
		{
			return false;
		}
    }
};
class SmallStone: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionCraftStoneKnifeEnv);
	}
};
class Spraycan_ColorBase: ItemBase {};
class Spraycan_Black: Spraycan_ColorBase {};
class Spraycan_Green: Spraycan_ColorBase {};
class TannedLeather: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttachToConstruction);
	}
};
