class AmmoBox : Container_Base 
{
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}
};
class FirstAidKit : Container_Base 
{
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}
};
class PlateCarrierPouches : Container_Base
{
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
};
class Refrigerator: WorldContainer_Base {};
class RefrigeratorMinsk : WorldContainer_Base {};
class SmallProtectorCase : Container_Base 
{
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}
};
class TrashCan : WorldContainer_Base {};
