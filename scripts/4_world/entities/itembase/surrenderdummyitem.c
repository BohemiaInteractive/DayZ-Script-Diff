[Obsolete("1.30: no replacement - no longer in use")]
class SurrenderDummyItem extends DummyItem
{
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
					pb.SetSurrenderState(false);
					em.SetEmoteLockState(false);
				}
			}
			
			DeleteSafe();
		}
		else if (!old_owner && !new_owner)
		{
			DeleteSafe();
		}
	}
	
	override bool CanSwapEntities(EntityAI otherItem, InventoryLocation otherDestination, InventoryLocation destination)
	{
		return false;
	}
}