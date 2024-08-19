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
		ItemBase ingredient = ingredients[0];
		
		for (int i=0; i < results.Count(); i++)
		{
			ItemBase item_result;
			Class.CastTo(item_result, results.Get(i));
			
			//Trasnfer current food state
			MiscGameplayFunctions.TransferItemProperties(ingredient, item_result);
			item_result.SetQuantityNormalized(Math.RandomFloat(0.8,1));
		}
		
		PluginLifespan lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
		lifespan.UpdateBloodyHandsVisibility( player, true );
	}
};
