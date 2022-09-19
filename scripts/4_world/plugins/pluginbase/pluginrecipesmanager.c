enum ERecipeSanityCheck
{
	IS_IN_PLAYER_INVENTORY = 1,
	NOT_OWNED_BY_ANOTHER_LIVE_PLAYER = 2,
	CLOSE_ENOUGH = 4,
}

const float ACCEPTABLE_DISTANCE = 5;

const int SANITY_CHECK_ACCEPTABLE_RESULT = ERecipeSanityCheck.NOT_OWNED_BY_ANOTHER_LIVE_PLAYER | ERecipeSanityCheck.CLOSE_ENOUGH;

class PluginRecipesManager extends PluginRecipesManagerBase
{
	static ref map<string,ref CacheObject > m_RecipeCache = new map<string,ref CacheObject >;
	static ref map<typename, bool> m_RecipesInitializedItem = new ref map<typename, bool>;
	
	
	ref Timer m_TestTimer;
	const int MAX_NUMBER_OF_RECIPES = GetMaxNumberOfRecipes();
	const int MAX_CONCURENT_RECIPES = 128;
	const int MAX_INGREDIENTS = 5;
	
	int m_RegRecipeIndex;
	int m_ResolvedRecipes[MAX_CONCURENT_RECIPES];
	
	bool m_EnableDebugCrafting = false;
	ItemBase m_Ingredients[MAX_INGREDIENTS];
	int m_IngredientBitMask[MAX_INGREDIENTS];
	int m_IngredientBitMaskSize[MAX_INGREDIENTS];
	
	int m_BitsResults[MAX_INGREDIENTS];
	
	ItemBase m_ingredient1[MAX_CONCURENT_RECIPES];
	ItemBase m_ingredient2[MAX_CONCURENT_RECIPES];
	ItemBase m_ingredient3[MAX_CONCURENT_RECIPES];
	
	ItemBase m_sortedIngredients[MAX_NUMBER_OF_INGREDIENTS];
	
	ref array<int> m_RecipesMatched = new array<int>;
	ref array<string> m_CachedItems = new array<string>;

	ref array<ref RecipeBase> m_RecipeList = new array<ref RecipeBase>;//all recipes
	static ref map<string, int> m_RecipeNamesList = new map<string, int>;//all recipes
	
	ref Timer myTimer1;
	
	static int GetMaxNumberOfRecipes()
	{
		return 2048;
	}
	
	void PluginRecipesManager()
	{

		CreateAllRecipes();
		GenerateRecipeCache();
		
		myTimer1 = new Timer();
	}
	
	void ~PluginRecipesManager()
	{
	}
	

	bool IsEnableDebugCrafting()
	{
		return m_EnableDebugCrafting;
	}

	void SetEnableDebugCrafting(bool enable)
	{
		m_EnableDebugCrafting = enable;
	}


	string GetRecipeName(int recipe_id)
	{
		if ( m_RecipeList[recipe_id] ) return m_RecipeList[recipe_id].GetName();
		return "";
	}


	//will fill the map 'ids' with valid recipes for the 'item1' and 'item2' items, where the key is the recipe ID, and the value is the recipe name
	int GetValidRecipes(ItemBase item1, ItemBase item2, array<int> ids, PlayerBase player)
	{
		if ( item1 == NULL || item2 == NULL )
		{
			if (ids) ids.Clear();
			return 0;
		}
		
		if ( ( item1.GetInventory().IsAttachment() && item1.GetHierarchyParent().DisassembleOnLastDetach() ) || ( item2.GetInventory().IsAttachment() && item2.GetHierarchyParent().DisassembleOnLastDetach() ) )
		{
			if (ids) ids.Clear();
			return 0;
		}
		m_Ingredients[0] = item1;
		m_Ingredients[1] = item2;
		
		return GetValidRecipesProper(2,m_Ingredients, ids, player);
	}
	
	int GetValidRecipesProper(int num_of_items, ItemBase items[], array<int> ids, PlayerBase player)
	{
		if (ids) ids.Clear();
		GetRecipeIntersection(num_of_items,items);
		int numOfRecipes = SortIngredients(num_of_items,items,m_ResolvedRecipes);
		//will return number of cached recipes for the 2 items being considered, 
		//and save their indexes in m_ResolvedRecipes, please note the m_ResolvedRecipes is a static array, 
		//and does not clear up with each query, so the number of recipes returned is critical to make sure we don't accidentally
		//mix in some other indexes from previous queries(and obviously loop only as far as we have to)
		//this also saves the ingredients in the correct assignment as ingredient 1/2 into m_ingredient1/m_ingredient2 arrays, these 3 arrays
		//therefore provide you with information per each index with: recipeid,ingredience1,ingredience2
		if ( numOfRecipes == 0 ) return 0;
		int found = 0;
		RecipeBase p_recipe = NULL;
		for (int i = 0; i < numOfRecipes; i++)
		{
			p_recipe = m_RecipeList[m_ResolvedRecipes[i]];

			if ( p_recipe.CheckRecipe(m_ingredient1[i],m_ingredient2[i], player) == true )
			{
				if (ids) ids.Insert( p_recipe.GetID() );
				found++;
			}
		}
		return found;
	}


	float GetRecipeLengthInSecs(int recipe_id)
	{
		if ( m_RecipeList[recipe_id] ) return m_RecipeList[recipe_id].GetLengthInSecs();
		return 0;
	}

	float GetRecipeSpecialty(int recipe_id)
	{
		if ( m_RecipeList[recipe_id] ) return m_RecipeList[recipe_id].GetSpecialty();
		return 0;
	}

	bool GetIsInstaRecipe(int recipe_id)
	{
		if ( m_RecipeList[recipe_id] ) return m_RecipeList[recipe_id].IsInstaRecipe();
		else return false;
	}

	override void OnInit()
	{
		super.OnInit();
		//ReadCacheFromFile(PATH_CACHE_FILE);//read the cache from a file
		//GenerateHumanReadableRecipeList();
	}


	void CallbackGenerateCache()
	{
		Debug.Log("CallbackGenerateCache","recipes");
		GenerateRecipeCache();
		//SaveCacheToFile(PATH_CACHE_FILE);//generate the cache and save it to a file
		//ReadCacheFromFile(PATH_CACHE_FILE);
	}

	protected void GenerateRecipeCache()
	{
		GetGame().ProfilerStart("m_RecipeCache");
		
		//m_CacheBasesMap.Clear();
		m_CachedItems.Clear();
		PluginRecipesManager.m_RecipeCache.Clear();
		
		TStringArray all_config_paths = new TStringArray;
		
		all_config_paths.Insert(CFG_VEHICLESPATH);
		all_config_paths.Insert(CFG_WEAPONSPATH);
		all_config_paths.Insert(CFG_MAGAZINESPATH);
		//Debug.Log("Got here 0","caching");
		string config_path;
		string child_name;
		int scope;
		TStringArray full_path = new TStringArray;
		WalkRecipes();
		for (int i = 0; i < all_config_paths.Count(); i++)
		{
			config_path = all_config_paths.Get(i);
			int children_count = GetGame().ConfigGetChildrenCount(config_path);
			
			for (int x = 0; x < children_count; x++)
			{
				GetGame().ConfigGetChildName(config_path, x, child_name);
				scope = GetGame().ConfigGetInt( config_path + " " + child_name + " scope" );

				if ( scope == 2 )
				{
					GetGame().ConfigGetFullPath(config_path +" "+ child_name,/*out*/ full_path);
					MatchItems(full_path);
				}
			}
		}
		GetGame().ProfilerStop("m_RecipeCache");
	}

	void WalkRecipes()
	{
		//Print("------------- WalkRecipes --------------");
		for (int c = 0; c < m_RecipeList.Count(); c++)
		{
			RecipeBase recipe = m_RecipeList.Get(c);
			if (recipe)
			{
				//Print(recipe.ClassName());
				int recipe_id = recipe.GetID();
				for (int i = 0; i < MAX_NUMBER_OF_INGREDIENTS; i++)
				{
					array<string> list = recipe.m_Ingredients[i];
					
					for (int x = 0; x < list.Count(); x++)
					{
						string ingredient = list.Get(x);
						int mask = Math.Pow(2,i);
						CacheObject co = m_RecipeCache.Get(ingredient);
	
						if (!co)
						{
							co = new CacheObject;
							m_RecipeCache.Insert(ingredient,co);
						}
						co.AddRecipe(recipe_id, mask);
					}
				}
			}
		}
	}

	
	// moved outside method to speed things up
	ref array<int> m_RcpsArray;
	string m_BaseName;
	int m_RecipeID;
	int item_mask;
	int m_BaseMask;
	string m_ItemName;
	ref CacheObject m_CoItem;
	ref CacheObject m_CoBase;
	
	//this will take the item class name and resolve it against all processed recipes
	protected void MatchItems(TStringArray full_path)
	{
		m_ItemName = full_path.Get(0);
		m_CoItem = m_RecipeCache.Get(m_ItemName);
		//Print(m_ItemName);
		if ( !m_CoItem )
		{
			m_CoItem = new CacheObject;
			m_RecipeCache.Insert(m_ItemName,m_CoItem);
		}
		for (int i = 1; i < full_path.Count(); i++)
		{
			m_BaseName = full_path.Get(i);
			m_CoBase = m_RecipeCache.Get(m_BaseName);
			if ( m_CoBase )//resolve new base classes
			{
				m_RcpsArray = m_RecipeCache.Get(m_BaseName).GetRecipes();
				
				for ( int x = 0; x < m_RcpsArray.Count(); x++ )//base recipes
				{
					m_RecipeID = m_RcpsArray.Get(x);
					
					//item_mask = m_CoItem.GetMaskByRecipeID(m_RecipeID);
					m_BaseMask = m_CoBase.GetMaskByRecipeID(m_RecipeID);
						
					m_CoItem.AddRecipe(m_RecipeID,m_BaseMask);
				}
			}
		}

		
	}

	void PerformRecipeServer(int id, ItemBase item_a,ItemBase item_b ,PlayerBase player)
	{
		m_Ingredients[0] = item_a;
		m_Ingredients[1] = item_b;
		
		if ( !item_a || !item_b ) 
		{
			Error("PerformRecipeServer - one of the items null !!");
			return;
		}
		
		SortIngredientsInRecipe(id, 2,m_Ingredients, m_sortedIngredients);

		bool is_recipe_valid = CheckRecipe(id,m_sortedIngredients[0],m_sortedIngredients[1],player);
		bool passed_sanity_check = RecipeSanityCheck(2,m_sortedIngredients,player);
		
		if ( !is_recipe_valid )
		{
			Error("PerformRecipeServer - recipe not valid !!");
			return;
		}
		if ( !passed_sanity_check )
		{
			Error("PerformRecipeServer - recipe failed to pass sanity check !!");
			return;
		}
		RecipeBase ptrRecipe = m_RecipeList[id];
		ptrRecipe.PerformRecipe(m_sortedIngredients[0],m_sortedIngredients[1],player);
		//player.RequestCraftingDisable();
		
	}
	
	void GenerateHumanReadableRecipeList()
	{
		FileHandle file = OpenFile("$profile:RecipeDump.txt", FileMode.WRITE);
		if ( file == 0 )
		{
			//error message
			PrintString("failed to open file RecipeDump");
			return;
		}
		array<int> recipes = new array<int>;
		for (int i = 0; i < PluginRecipesManager.m_RecipeCache.Count(); i++)
		{
			string key = PluginRecipesManager.m_RecipeCache.GetKey(i);
			CacheObject value = PluginRecipesManager.m_RecipeCache.GetElement(i);
			
			string line = key;
			recipes.Clear();
			recipes.InsertAll( value.GetRecipes() );
			
			//PrintString("Item: " + key);
			
			for (int x = 0; x < recipes.Count(); x++)
			{
				int recipe_id = recipes.Get(x);
				string recipe_name = GetRecipeName(recipe_id);
				line += "," +recipe_name;
			}
			FPrintln(file, line);
		}
		CloseFile(file);
	}
	
	protected bool RecipeSanityCheck(int num_of_ingredients, InventoryItemBase items[], PlayerBase player)
	{
		int check_results[MAX_INGREDIENTS];

		for (int i = 0; i < num_of_ingredients;i++)
		{
			ItemBase item = items[i];
			Man item_owner_player = item.GetHierarchyRootPlayer();
			vector item_pos = item.GetPosition();
			vector player_pos = player.GetPosition();
			
			if (item_owner_player == player)
			{
				check_results[i] =  check_results[i] | ERecipeSanityCheck.IS_IN_PLAYER_INVENTORY;
			}
			
			if ( item_owner_player == NULL || item_owner_player == player || !item_owner_player.IsAlive() )
			{
				check_results[i] = check_results[i] | ERecipeSanityCheck.NOT_OWNED_BY_ANOTHER_LIVE_PLAYER;			
			}
			
			if ( vector.Distance(item_pos, player_pos ) < ACCEPTABLE_DISTANCE )
			{
				check_results[i] = check_results[i] | ERecipeSanityCheck.CLOSE_ENOUGH;			
			}
		}
		for (i = 0; i < num_of_ingredients;i++)
		{
			if ( !((check_results[i] & SANITY_CHECK_ACCEPTABLE_RESULT) == SANITY_CHECK_ACCEPTABLE_RESULT))
			{
				return false;
			}
		}
		return true;
	}

	override protected void RegisterRecipe(RecipeBase recipe)
	{
		
		if ( m_RegRecipeIndex >= MAX_NUMBER_OF_RECIPES )
		{
			Error("Exceeded max. number of recipes, max set to: "+MAX_NUMBER_OF_RECIPES.ToString());
		}
		
		m_RegRecipeIndex = m_RecipeList.Insert(recipe);
		recipe.SetID(m_RegRecipeIndex);
		m_RecipeNamesList.Insert(recipe.ClassName(), m_RegRecipeIndex);
		//Print("RegisterRecipe: " +recipe.ClassName() + ", "+ m_RegRecipeIndex.ToString());
	}

	override protected void UnregisterRecipe(string clasname)
	{
		int recipe_id = RecipeIDFromClassname(clasname);
		//Print("UnregisterRecipe: " + recipe_id.ToString());
		if (recipe_id != -1)
		{
			m_RecipeNamesList.Remove(clasname);
			//Print(m_RecipeList[recipe_id]);
			m_RecipeList[recipe_id] = null;
			//Print(m_RecipeList[recipe_id]);
		}
	}
	
	static int RecipeIDFromClassname(string classname)
	{
		if (m_RecipeNamesList.Contains(classname))
			return m_RecipeNamesList.Get(classname);
		return -1;
	}
	
	protected bool CheckRecipe(int id, ItemBase item1, ItemBase item2, PlayerBase player)//requires ordered items
	{
		RecipeBase p_recipe = m_RecipeList[id];
		return p_recipe.CheckRecipe(item1,item2, player);
	}
	
	protected void PrintCache()
	{
		for (int i = 0; i < PluginRecipesManager.m_RecipeCache.Count(); i++)
		{
			string key = PluginRecipesManager.m_RecipeCache.GetKey(i);
			CacheObject co = PluginRecipesManager.m_RecipeCache.GetElement(i);

			PrintString("Item: " + key);
			co.DebugPrint();
			PrintString("----------------");
		}
	}
	//!sorts ingredients correctly as either first or second ingredient based on their masks
	protected bool SortIngredientsInRecipe(int id, int num_of_ingredients, ItemBase ingredients_unsorted[], ItemBase ingredients_sorted[] )
	{
		ClearResults();
		
		for (int i = 0; i < num_of_ingredients; i++)
		{
			CacheObject co_item = PluginRecipesManager.m_RecipeCache.Get( ingredients_unsorted[i].GetType() );
			m_IngredientBitMask[i] = co_item.GetMaskByRecipeID(id);
			m_IngredientBitMaskSize[i] = co_item.GetBitCountByRecipeID(id);
		}
		
		bool result = ResolveIngredients(num_of_ingredients);
		
		if (result)
		{
			for (i = 0; i < num_of_ingredients; i++)
			{
				int index = Math.Log2( m_BitsResults[i]);
				ingredients_sorted[index] = ingredients_unsorted[ i ];
			}
		}
		//PrintResultMasks(num_of_ingredients);
		return result;
	}
	
	protected void ClearResults()
	{
		for (int i = 0; i < MAX_INGREDIENTS; i++)
		{
			m_BitsResults[i] = 0;			
		}	
			
	}
	
	protected bool ResolveIngredients(int num_of_ingredients, int passes = 0)
	{
		int rightmost_bit;
		int smallest = 99999;
		int smallest_index = 0;

		for (int i = 0; i < num_of_ingredients; i++)
		{
			int count = m_IngredientBitMaskSize[i];
			if ( count != 0 && count < smallest)
			{
				smallest = m_IngredientBitMaskSize[i];
				smallest_index = i;
			}
		}
		
		rightmost_bit = m_IngredientBitMask[smallest_index] & (-m_IngredientBitMask[smallest_index]);
		m_BitsResults[smallest_index] = m_BitsResults[smallest_index] | rightmost_bit;
		
		for (int x = 0; x < num_of_ingredients; x++)
		{
			m_IngredientBitMask[x] = ~rightmost_bit & m_IngredientBitMask[x];
			m_IngredientBitMask[smallest_index] = 0;
			m_IngredientBitMaskSize[smallest_index] = 0;
		}
		
		// check validity
		int check_sum_vertical = 0;
		
		for (int z = 0; z < num_of_ingredients; z++)
		{
			check_sum_vertical = check_sum_vertical | m_IngredientBitMask[z];//vertical sum
			check_sum_vertical = check_sum_vertical | m_BitsResults[z];//vertical sum
			if ((m_IngredientBitMask[z] | m_BitsResults[z]) == 0) 
			{
				return false;//horizontal check
			}
		}
		
		if ( check_sum_vertical != (Math.Pow(2, num_of_ingredients) - 1)) return false;//vertical check
		
		passes++;
		
		if (passes < num_of_ingredients) 
		{
			if ( !ResolveIngredients(num_of_ingredients, passes) ) return false;
		}
		return true;
	}
	
	
	protected void PrintResultMasks(int num)
	{
		for (int i = 0; i < num; i++)
		{
			Debug.Log("results mask("+i.ToString()+") = " +m_BitsResults[i].ToString() );
		}
	}
	
	//!fills an array with recipe IDs which 'item_a' and 'item_b' share
	protected int GetRecipeIntersection(int num_of_ingredients, ItemBase items[])
	{
		int count = 0;
		int smallest = 9999;
		int smallest_index = 0;
		m_RecipesMatched.Clear();
		
		/*
		m_Ingredients[0] = item_a;
		m_Ingredients[1] = item_b;
		*/
		//find the item with smallest number of recipes
		CacheObject co_least_recipes;
		
		for (int i = 0; i < num_of_ingredients; i++)
		{
			CacheObject cobject = PluginRecipesManager.m_RecipeCache.Get( items[i].GetType() );
			if (!cobject) 
			{
				return 0;
			}
			if (cobject.GetNumberOfRecipes() < smallest)
			{
				smallest = cobject.GetNumberOfRecipes();
				smallest_index = i;
				co_least_recipes = cobject;
			}
		}
		
		//look for matches
		array<int> recipes = co_least_recipes.GetRecipes();
		for (int x = 0; x < recipes.Count(); x++)
		{
			int id = recipes.Get(x);
			for (int z = 0; z < num_of_ingredients; z++)
			{
				if ( z!= smallest_index)
				{
					CacheObject cobject2 = PluginRecipesManager.m_RecipeCache.Get( items[z].GetType() );
					if ( cobject2.IsContainRecipe(id) )
					{
						m_RecipesMatched.Insert(id);
						count++;
					}
				}
			}
		}
		return count;
	}

	protected int SortIngredients(int num_of_ingredients, ItemBase items_unsorted[], int resolved_recipes[])
	{
		int count = 0;
		for (int i = 0; i < m_RecipesMatched.Count() && i < MAX_CONCURENT_RECIPES; i++)
		{
			int recipe_id = m_RecipesMatched.Get(i);
			
				if (SortIngredientsInRecipe(recipe_id, num_of_ingredients, items_unsorted, m_sortedIngredients))//...and now we need to determine which item will be which ingredient number
				{
					resolved_recipes[count] = recipe_id;
					m_ingredient1[count] = m_sortedIngredients[0];
					m_ingredient2[count] = m_sortedIngredients[1];
					//m_ingredient3[count] = m_sortedIngredients[2];
					count++;
				}
		}
		return count;
	}
	
	
	protected void CreateAllRecipes()
	{
		RegisterRecipies();
	}
}