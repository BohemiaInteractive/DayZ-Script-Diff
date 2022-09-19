//! Client only - manage set up crafting on client 
class CraftingManager
{
	const int CM_MODE_NONE = 0;
	const int CM_MODE_WORLD = 1;
	const int CM_MODE_INVENTORY = 2;

	PlayerBase m_player;
	PluginRecipesManager m_recipesManager; 
	ActionVariantManager m_actionVariantManager;
	int m_recipeID;
	int m_contextualRecipeID;
	int m_recipeCount;
	int m_craftingMode;
	ItemBase m_item1;
	ItemBase m_item2;
	
	ref array<int> m_recipes;

	void CraftingManager(PlayerBase player, PluginRecipesManager recipesManager)
	{
		m_recipesManager = recipesManager;
		m_player = player;
		m_craftingMode = CM_MODE_NONE;
		m_actionVariantManager = ActionManagerClient.GetVariantManager( ActionWorldCraft );
		m_actionVariantManager.GetOnUpdateInvoker().Clear();
		m_actionVariantManager.GetOnUpdateInvoker().Insert(OnUpdate);
		m_recipes = new array<int>;
	}

	void SetRecipeID(int recipeID)
	{
		m_recipeID = recipeID;
	}
	
	int GetRecipeID()
	{
		return m_recipeID;
	}
	
	bool IsInventoryCraft()
	{
		return m_craftingMode == CM_MODE_INVENTORY;
	}
	
	bool IsWorldCraft()
	{
		return m_craftingMode == CM_MODE_WORLD;
	}
	
	int GetRecipesCount()
	{
		return 	m_recipeCount;
	}
	 // deprecated
	void SetNextRecipe()
	{

	}
	
	void OnUpdate( Object item, Object target, int component_index )
	{
		ItemBase item1 = ItemBase.Cast( item );
		ItemBase item2 = ItemBase.Cast( target );
		
		if ( m_player.GetActionManager().GetRunningAction() )
			return;
		
		ItemBase item_in_hands = m_player.GetItemInHands();
		
		if (!item1 || !item2)
		{
			m_recipeCount = 0;
			m_craftingMode = CM_MODE_NONE;
			m_actionVariantManager.Clear();
			return;
		}
		else
		{
			
			if ( item1 != item_in_hands && item2 != item_in_hands )
			{
				InventoryLocation il1 = new InventoryLocation;
				InventoryLocation il2 = new InventoryLocation;
				
				item1.GetInventory().GetCurrentInventoryLocation(il1);
				item2.GetInventory().GetCurrentInventoryLocation(il2);
				
				Error("Crafting manager - both of items are out of hands - item1: " + il1.DumpToString() + " item2: " + il2.DumpToString() + " / item in hands: - " + item_in_hands);
			}
		}
		
		int recipeCount = 0;
		
		if (m_craftingMode == CM_MODE_INVENTORY)
		{
			recipeCount = m_recipesManager.GetValidRecipes(m_item1, m_item2, m_recipes, m_player);
			m_recipeCount = recipeCount;
			m_recipeID = m_recipes.Get(m_contextualRecipeID);
			return;
		}
		
		recipeCount = m_recipesManager.GetValidRecipes(item1, item2, m_recipes, m_player);

		if (recipeCount == 0)
		{
			m_recipeCount = 0;
			m_craftingMode = CM_MODE_NONE;
			m_actionVariantManager.Clear();
		}
		else
		{
			if ( m_craftingMode == CM_MODE_NONE || m_recipeCount != recipeCount  || m_item1 != item1  || m_item2 != item2 )
			{
				m_craftingMode = CM_MODE_WORLD;
				m_recipeCount = recipeCount;
				m_contextualRecipeID = 0;
				m_item1 = item1;
				m_item2 = item2;
				
				m_actionVariantManager.SetActionVariantCount(m_recipeCount);
			}
			m_recipeID = m_recipes.Get(m_contextualRecipeID);
		}
		
	}
	
	bool SetInventoryCraft(int recipeID, ItemBase item1, ItemBase item2)
	{
		int recipeCount = m_recipesManager.GetValidRecipes(item1,item2,m_recipes, m_player);
		
		for ( int i = 0; i < recipeCount; i++ )
		{
			if (recipeID == -1 || m_recipes.Get(i) == recipeID)
			{
				if ( m_recipesManager.GetIsInstaRecipe(m_recipes.Get(i)) || m_recipesManager.IsEnableDebugCrafting() )
				{
					Param craftParam = new Param3<int, ItemBase, ItemBase>(m_recipes.Get(i), item1, item2);
					m_player.RPCSingleParam(ERPCs.RPC_CRAFTING_INVENTORY_INSTANT, craftParam, true, m_player.GetIdentity());
					return true;
				}
				else
				{
					m_craftingMode = CM_MODE_INVENTORY;
					m_recipeCount = recipeCount;
					m_contextualRecipeID = i;
					m_item1 = item1;
					m_item2 = item2;
					m_recipeID = m_recipes.Get(i);

					ActionManagerClient am = ActionManagerClient.Cast(m_player.GetActionManager());
					
					if ( m_player.GetItemInHands() == item1) am.SetInventoryAction( am.GetAction(ActionWorldCraft), item2, item1);
					else am.SetInventoryAction( am.GetAction(ActionWorldCraft), item1, item2);					

					return true;
				}		
			}			
			
		}	
		return false;
	}
	
	void ResetInventoryCraft()
	{
		m_recipeCount = 0;
		m_craftingMode = CM_MODE_NONE;
	}
	
	bool IsEnableDebugCrafting()
	{
		return true;
	}
	
	int GetRecipeID( int action_index )
	{
		return m_recipes[action_index];
	}
}
