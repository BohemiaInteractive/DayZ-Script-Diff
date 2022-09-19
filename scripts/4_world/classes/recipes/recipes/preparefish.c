class PrepareFish extends RecipeBase
{	
	override void Init()
	{
		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		// Adjusts quantity of results to the quantity of the 1st ingredient
		ItemBase item_ingredient = ingredients[0];
		//Class.CastTo(item_ingredient, ingredients[0]);
		
		float MaxQuantity = item_ingredient.GetQuantityMax();
		float CurrentQuantity = item_ingredient.GetQuantity();
		float adjust = CurrentQuantity / MaxQuantity;
		
		for (int i=0; i < results.Count(); i++)
		{
			ItemBase item_result;
			Class.CastTo(item_result, results.Get(i));
			
			//Trasnfer current food state
			MiscGameplayFunctions.TransferItemProperties(item_ingredient, item_result);
			
			if (m_ResultSetFullQuantity[i] == false)
			{
				item_result.SetQuantity(item_result.GetQuantityMax() * adjust);
			}
		}
		
		PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
		module_lifespan.UpdateBloodyHandsVisibility( player, true );
		
		Debug.Log("Recipe Do method called","recipes");
	}
};
