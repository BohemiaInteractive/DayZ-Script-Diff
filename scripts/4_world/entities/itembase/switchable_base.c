class Switchable_Base extends ItemBase
{
	override void OnWasAttached( EntityAI parent, int slot_id )
	{
		super.OnWasAttached( parent, slot_id );
		
		ItemBase parent_item;
		if ( Class.CastTo(parent_item,parent) )
		{
			parent_item.AddLightSourceItem(this);
		}
	}
	
	override void OnWasDetached( EntityAI parent, int slot_id )
	{
		super.OnWasDetached( parent, slot_id );
		
		ItemBase parent_item;
		if ( Class.CastTo(parent_item,parent) )
		{
			parent_item.RemoveLightSourceItem();
		}
	}
	
	override void EEItemLocationChanged (notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		
		if (GetLight())
		{
			GetLight().UpdateMode();
		}
	}
}