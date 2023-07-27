class DrysackBag_ColorBase: Backpack_Base
{
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionDeCraftDrysackBag);
	}
	
	override bool IsContainer()
	{
		return true;
	}
	
	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
			return false;
		
		if (parent && parent.IsKindOf("AmmoBox"))
			return false;

		return true;
	}
	
};
class DrysackBag_Green: DrysackBag_ColorBase {};
class DrysackBag_Orange: DrysackBag_ColorBase {};
class DrysackBag_Yellow: DrysackBag_ColorBase {};