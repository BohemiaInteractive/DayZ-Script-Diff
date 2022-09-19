class TripodBase : ItemBase
{
	override bool HasProxyParts()
	{
		return true;
	}
	
	override bool CanDetachAttachment( EntityAI parent )
	{
		FireplaceBase fireplace = FireplaceBase.Cast(parent);
		if(fireplace)
		{
			if ( fireplace.GetCookingEquipment() != null )
			{
				return false;
			}
		}
		return true;
	}

	override bool CanSwapEntities(EntityAI otherItem, InventoryLocation otherDestination, InventoryLocation destination)
	{
		if (GetHierarchyParent() && GetHierarchyParent().IsFireplace() && otherItem)
		{
			if (otherItem.IsInherited(Pot) || otherItem.IsInherited(Cauldron))
			{
				return false;
			}
		}
		return true;
	}
	
	
	override void OnDebugSpawn()
	{
		HideAllSelections();
		ShowSelection( "Deployed" );
	}
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionAttach);
		AddAction(ActionDetach);
	}
	
	override void OnWasAttached( EntityAI parent, int slot_id )
	{
		super.OnWasAttached(parent, slot_id);
		if (parent.IsFireplace())
		{
			HideAllSelections();
			ShowSelection( "Deployed" );
		}
	}
	
	override void OnWasDetached( EntityAI parent, int slot_id )
	{
		super.OnWasDetached(parent, slot_id);
		if (parent.IsFireplace())
		{
			HideAllSelections();
			ShowSelection( "Collapsed" );
		}
	}
}


class Tripod : TripodBase
{
	
}