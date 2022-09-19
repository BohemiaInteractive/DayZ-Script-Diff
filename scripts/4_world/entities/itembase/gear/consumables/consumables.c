class Bait: ItemBase {};
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
};
class Bandana_RedPattern: Bandana_ColorBase {};
class Bandana_BlackPattern: Bandana_ColorBase {};
class Bandana_PolkaPattern: Bandana_ColorBase {};
class Bandana_Greenpattern: Bandana_ColorBase {};
class Bandana_CamoPattern: Bandana_ColorBase {};
class Bandana_Blue: Bandana_ColorBase {};
class Bandana_Pink: Bandana_ColorBase {};
class Bandana_Yellow: Bandana_ColorBase {};

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
class BoneBait: ItemBase {};
class BoneHook:  ItemBase {};
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
class Hook:  ItemBase {};
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
