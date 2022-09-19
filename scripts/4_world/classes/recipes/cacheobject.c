
class RecipeCacheData
{
	int m_Mask;
	int m_BitCount;
	
	void RecipeCacheData(int mask)
	{
		SetMask(mask);
	}
	
	int GetMask()
	{
		return m_Mask;
	}
	
	int GetBitCount()
	{
		return m_BitCount;
	}
	
	void SetMask(int mask)
	{
		m_Mask = mask;
		m_BitCount = Math.GetNumberOfSetBits(mask);
	}
	
}

class CacheObject
{
	ref map<int, ref RecipeCacheData> m_Recipes;
	ref array<int> m_RecipeIDs;
	//RecipeCacheData data;
	void CacheObject()
	{
		m_Recipes = new map<int, ref RecipeCacheData>;
		m_RecipeIDs = new array<int>;
	}

	bool AddRecipe(int recipe_id, int mask)
	{
		RecipeCacheData data = m_Recipes.Get(recipe_id);
		if(data)
		{
			UpdateMask(recipe_id, mask);
		}
		else
		{
			m_Recipes.Insert(recipe_id, new RecipeCacheData(mask));
			m_RecipeIDs.Insert(recipe_id);

		}
		return true;
	}


	void UpdateMask(int recipe_id, int mask)
	{
		RecipeCacheData data = m_Recipes.Get(recipe_id);
		data.SetMask(data.GetMask() | mask);
		
	}

	
	int GetNumberOfRecipes()
	{
		return m_Recipes.Count();
	}
	
	array<int> GetRecipes()
	{
		return m_RecipeIDs;
	}
	
	bool IsContainRecipe(int recipe_id)
	{
		return m_Recipes.Contains(recipe_id);
	}
	
	int GetMaskByRecipeID(int recipe_id)
	{
		RecipeCacheData cache_data = m_Recipes.Get(recipe_id);
		if(cache_data)
			return cache_data.GetMask();
		return 0;
	}
	
	int GetBitCountByRecipeID(int recipe_id)
	{
		RecipeCacheData cache_data = m_Recipes.Get(recipe_id);
		if(cache_data)
			return cache_data.GetBitCount();
		return 0;
	}
	
	void DebugPrint()
	{
		for(int i = 0; i < m_Recipes.Count();i++)
		{
			Print("recipeID:"+m_Recipes.GetKey(i).ToString());
			Print("mask:"+m_Recipes.GetElement(i).m_Mask.ToString());
			Print("m_BitCount:"+m_Recipes.GetElement(i).m_BitCount.ToString());
			
		}
	}
}