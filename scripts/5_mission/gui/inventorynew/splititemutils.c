class SplitItemUtils
{
	static void TakeOrSplitToInventory ( notnull PlayerBase player, notnull EntityAI target, notnull EntityAI item)
	{
		ItemBase item_base = ItemBase.Cast( item );
		
		if( !item.GetInventory().CanRemoveEntity() || !player.CanManipulateInventory() )
			return;
		
		InventoryLocation il = new InventoryLocation;
		if( target.GetInventory().FindFreeLocationFor( item, FindInventoryLocationType.ANY, il) )
		{
			if( item_base.GetTargetQuantityMax(il.GetSlot()) >= item_base.GetQuantity() )
			{
				if( il.GetType() == InventoryLocationType.ATTACHMENT )
				{
					player.PredictiveTakeEntityToTargetAttachmentEx(il.GetParent(), item, il.GetSlot());
				}
				else
				{
					InventoryLocation src = new InventoryLocation;
					if (item.GetInventory().GetCurrentInventoryLocation(src))
						player.PredictiveTakeToDst(src, il);
				
				}
			}
			else
			{
				item_base.SplitIntoStackMaxClient( il.GetParent(), il.GetSlot() );
			}
		}
	}
	
	static void TakeOrSplitToInventoryLocation ( notnull PlayerBase player, notnull InventoryLocation dst)
	{
		ItemBase item_base = ItemBase.Cast( dst.GetItem() );	
		int slot = dst.GetSlot();

		if( !dst.GetItem().GetInventory().CanRemoveEntity() || !player.CanManipulateInventory() )
			return;
		
		float stack_max = item_base.GetTargetQuantityMax(slot);
		
		if( stack_max >= item_base.GetQuantity() )
		{
			InventoryLocation src = new InventoryLocation;
			if (dst.GetItem().GetInventory().GetCurrentInventoryLocation(src))
			{
				player.PredictiveTakeToDst(src, dst);
			}
			else
				Error("TakeIntoCargoEx cannot get src for dst=" + dst.DumpToString());
		}
		else
		{
			item_base.SplitIntoStackMaxToInventoryLocationClient( dst );
		}
	}
}