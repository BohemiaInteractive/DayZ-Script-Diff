class BurlapSack: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionCoverHeadTarget);
		AddAction(ActionCoverHeadSelf);
	}
};
class GorkaHelmetVisor: Inventory_Base {};
class ChickenFeather: Inventory_Base {};
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