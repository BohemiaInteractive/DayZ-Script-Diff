class SurrenderDummyItem extends DummyItem
{
	protected PlayerBase m_player;
	
	void ~SurrenderDummyItem()
	{
	}
	
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner,new_owner);
		
		if (old_owner)
		{
			PlayerBase pb;
			if (Class.CastTo(pb, old_owner))
			{
				EmoteManager em = pb.GetEmoteManager();
				if (em)
				{
					em.ForceSurrenderState(false);
				}
			}
			
			DeleteSafe();
		}
	}
	
	override bool CanSwapEntities(EntityAI otherItem, InventoryLocation otherDestination, InventoryLocation destination)
	{
		return false;
	}
}