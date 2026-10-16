class BurlapSack: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionCoverHeadTarget);
		AddAction(ActionCoverHeadSelf);
	}
};
class GorkaHelmetVisor: Inventory_Base
{
	static int SLOT_ID = InventorySlots.GetSlotIdFromString("Glass");

	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_2);
		
		AddSingleExclusionValueGlobal(EAttExclusions.SHAVING_HEADGEAR_ATT_0);
	}
};

class ChickenFeather: Inventory_Base 
{
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionCraftBoltsFeather);
	}
};

class LongWoodenStick: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionClapBearTrapWithThisItem);
		AddAction(ActionBreakLongWoodenStick);
		AddAction(ActionAttachToConstruction);
	}
};

class SharpWoodenStick: LongWoodenStick
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionClapBearTrapWithThisItem);
		AddAction(ActionCookOnStick);
	}
}

class Rope: Inventory_Base
{
	ref InventoryLocation m_TargetLocation = new InventoryLocation;
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionRestrainTarget);
		AddAction(ActionCraftRopeBelt);
		AddAction(ActionRestrainSelf);
		AddAction(ActionAttachToConstruction);
	}
	
	InventoryLocation GetTargetLocation()
	{
		return m_TargetLocation;
	}
	
	void SetTargetLocation(InventoryLocation targetLocation)
	{
		m_TargetLocation.CopyLocationFrom(targetLocation, true);
	}
	
	override bool CanAssignToQuickbar()
	{
		return (!GetInventory().IsAttachment());
	}
};

class Spear : Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionClapBearTrapWithThisItem);
		
	}
};

class SpearStone : Spear
{

};

class SpearBone : Spear
{

};

class Fabric : Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionRepairTentPart);
		AddAction(ActionAttachToConstruction);
		AddAction(ActionRepairCarChassis);
	}
};