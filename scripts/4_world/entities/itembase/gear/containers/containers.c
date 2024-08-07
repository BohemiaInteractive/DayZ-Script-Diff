class AmmoBox : Container_Base 
{
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}
	
	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
			return false;
		
		if (parent && parent.IsKindOf("WaterproofBag_ColorBase"))
			return false;

		return true;
	}
}

class FirstAidKit : Container_Base 
{
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}
}

class PlateCarrierPouches : Container_Base
{
	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
			return false;
		
		if (parent && parent.IsKindOf("AmmoBox"))
			return false;
		
		if (parent && parent.IsKindOf("PlateCarrierPouches"))
			return false;

		return true;
	}

	override bool CanReceiveItemIntoCargo( EntityAI item )
	{
		if (!super.CanReceiveItemIntoCargo(item))
			return false;
		
		if (GetInventory().IsAttachment())
		{
			return !GetHierarchyParent().GetInventory().IsInCargo() && (!item.GetInventory().GetCargo() || (item.GetInventory().GetCargo().GetItemCount() == 0 || item.IsContainer()));
		}
		
		return !item.GetInventory().GetCargo() || (item.GetInventory().GetCargo().GetItemCount() == 0 || item.IsContainer());
	}
	
	override bool CanLoadItemIntoCargo( EntityAI item )
	{
		if (!super.CanLoadItemIntoCargo(item))
			return false;
		
		return !item.GetInventory().GetCargo() || (item.GetInventory().GetCargo().GetItemCount() == 0 || item.IsContainer());
	}
}

class PlateCarrierPouches_Black extends PlateCarrierPouches {};
class PlateCarrierPouches_Camo extends PlateCarrierPouches {};
class PlateCarrierPouches_Green extends PlateCarrierPouches {};
class PlateCarrierPouches_Winter extends PlateCarrierPouches {};

class Refrigerator : WorldContainer_Base {};
class RefrigeratorMinsk : WorldContainer_Base {};
class SmallProtectorCase : Container_Base 
{
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}
}

class TrashCan : WorldContainer_Base {};
