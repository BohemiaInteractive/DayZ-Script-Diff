class UndergroundStash extends ItemBase
{
	void UndergroundStash() {}
	
	void PlaceOnGround()
	{
		vector pos = GetPosition();
		pos[1] = GetGame().SurfaceRoadY(pos[0], pos[2]);
		pos[1] = pos[1] + 0.22;
		SetPosition(pos);
	}
	
	ItemBase GetStashedItem()
	{
		ItemBase item;		
		if (GetInventory().GetCargo().GetItemCount() > 0)
		{
			item = ItemBase.Cast(GetInventory().GetCargo().GetItem(0));
		}

		return item;
	}
	
	override bool CanDisplayCargo()
	{
		return false;
	}
	
	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}
	
	override bool CanReleaseCargo(EntityAI cargo)
	{
		return false;
	}
	
	override bool CanReceiveItemIntoHands(EntityAI item_to_hands)
	{
		return false;
	}
	
	override bool CanSaveItemInHands(EntityAI item_in_hands)
	{
		return false;
	}
	
	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}
	
	override bool IsInventoryVisible()
	{
		return false;
	}
}