class SeedPackBase extends Inventory_Base
{
	private static const float PACK_DAMAGE_TOLERANCE = 0.5;
	
	void SeedPackBase()
	{	
	}

	void EmptySeedPack(PlayerBase player)
	{
		string pack_type = GetType();
		string seeds_type = "";
		
		g_Game.ConfigGetText("cfgVehicles " + pack_type + " Horticulture ContainsSeedsType", seeds_type);
		
		int seeds_quantity_max = g_Game.ConfigGetInt("cfgVehicles " + pack_type + " Horticulture ContainsSeedsQuantity");
		int seeds_quantity = seeds_quantity_max;
		
		seeds_quantity = Math.Round(seeds_quantity_max * GetHealth01("",""));
	
		if (seeds_quantity < 1)
		{ 
			seeds_quantity = 1;
		}
		
		if (player)
		{
			EmptySeedsPackLambda lambda = new EmptySeedsPackLambda(this, seeds_type, player, seeds_quantity);
			player.ServerReplaceItemInHandsWithNew(lambda);
		}
		else
		{
			vector pos = GetPosition();
			g_Game.CreateObjectEx(seeds_type, pos, ECE_PLACE_ON_SURFACE);
			g_Game.ObjectDelete(this);
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionEmptySeedsPack);
	}
}

class EmptySeedsPackLambda : ReplaceItemWithNewLambdaBase
{
	int m_ItemCount;
	PlayerBase m_Player;
	
	void EmptySeedsPackLambda(EntityAI old_item, string new_item_type, PlayerBase player, int count) 
	{
		m_ItemCount = count;
		m_Player = player;
	}

	override void CopyOldPropertiesToNew(notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);

		ItemBase unboxed;
		Class.CastTo(unboxed, new_item);
		unboxed.SetQuantity(m_ItemCount);
	}
	
	override void OnSuccess(EntityAI new_item)
	{
		super.OnSuccess(new_item);
		
		//spawns wrapping Paper
		if (m_Player)
		{
			m_Player.SpawnEntityOnGroundRaycastDispersed("Paper",DEFAULT_SPAWN_DISTANCE,UAItemsSpreadRadius.NARROW);
		}
		else
		{
			g_Game.CreateObjectEx("Paper", new_item.GetHierarchyRoot().GetPosition(), ECE_PLACE_ON_SURFACE);
		}
	}
};