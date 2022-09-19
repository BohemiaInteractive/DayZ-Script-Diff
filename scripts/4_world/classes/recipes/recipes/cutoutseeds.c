class CutOutSeeds extends RecipeBase
{	
	override void Init()
	{
		
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		// Adjusts quantity of results to the quantity of the 1st ingredient
		Edible_Base item_ingredient = Edible_Base.Cast(ingredients[0]);
		
		float MaxQuantity = item_ingredient.GetQuantityMax();
		float CurrentQuantity = item_ingredient.GetQuantity();
		float qtPercentage = CurrentQuantity / MaxQuantity;
		
		FoodStageType stage = item_ingredient.GetFoodStageType();
		
		float seedMultiplier = Math.RandomFloat(0.5, 1);		
		
		//Set seeds amount proportional to ingredient quantity. No seeds if the is less that 30% of ingredient
		if(qtPercentage < 0.3)
			seedMultiplier = 0;
		else
			seedMultiplier *= qtPercentage;
		
		//Set seeds amount based on the stage of the ingredient
		switch(stage)
		{
			case FoodStageType.BURNED:
			case FoodStageType.BOILED:
			case FoodStageType.BAKED:
				seedMultiplier *= 0;
				break;
			case FoodStageType.DRIED:
			case FoodStageType.ROTTEN:
				seedMultiplier *= 0.5;
				break;	
			case FoodStageType.RAW:
			default:
				seedMultiplier *= 1;
				break;
		}
		
		ItemBase result = results[0];
		float maxQuantity = result.GetQuantityMax();
		float resultQty = m_ResultSetQuantity[0]/ maxQuantity;
		resultQty *= seedMultiplier * maxQuantity;
		
		resultQty = Math.Ceil(resultQty);
		
		result.SetQuantity(resultQty);
		
		Debug.Log("Recipe Do method called","recipes");
	}
};