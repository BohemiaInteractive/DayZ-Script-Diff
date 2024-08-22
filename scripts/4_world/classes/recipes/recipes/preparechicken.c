class PrepareChicken extends PrepareAnimal
{	
	override void Init()
	{
		super.Init();

		//----------------------------------------------------------------------------------------------------------------------
		//ingredient 1
		InsertIngredient(0,"DeadChicken_ColorBase");//you can insert multiple ingredients this way
		m_IngredientAddHealth[0] = 0;			// 0 = do nothing
		m_IngredientSetHealth[0] = -1;			// -1 = do nothing
		m_IngredientAddQuantity[0] = 0;			// 0 = do nothing
		m_IngredientDestroy[0] = true;			// true = destroy, false = do nothing

		//----------------------------------------------------------------------------------------------------------------------
		AddResult("Bone");//add results here
		m_ResultSetFullQuantity[0] = false;		// true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = 2;				// -1 = do nothing
		m_ResultSetHealth[0] = -1;				// -1 = do nothing
		m_ResultInheritsHealth[0] = 0;			// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;			// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -2;			// (value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultReplacesIngredient[0] = 0;		// (value) == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value

	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		if (ingredients[0].GetHealthLevel() == GameConstants.STATE_RUINED)
		{
			return false;
		}
		return true;
	}
	
	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		ItemBase deadChicken = ingredients[0];
		ItemBase resultBones = results[0];

		int steakCount = Math.RandomIntInclusive(2,4);
		for (int i=0; i < steakCount; i++)
		{
			ItemBase result = ItemBase.Cast(player.SpawnEntityOnGroundOnCursorDir("ChickenBreastMeat", DEFAULT_SPAWN_DISTANCE));
			MiscGameplayFunctions.TransferItemProperties(deadChicken, result);
			result.SetQuantityNormalized(Math.RandomFloatInclusive(0.8,1));
		}

		ItemBase resultFeathers = ItemBase.Cast(player.SpawnEntityOnGroundOnCursorDir("ChickenFeather", DEFAULT_SPAWN_DISTANCE));
		MiscGameplayFunctions.TransferItemProperties(deadChicken, resultFeathers);
		resultFeathers.SetQuantity(Math.RandomIntInclusive(5,15));

		MiscGameplayFunctions.TransferItemProperties(deadChicken, resultBones);
		resultBones.SetQuantity(Math.RandomIntInclusive(2,5));		

		PluginLifespan lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
		lifespan.UpdateBloodyHandsVisibility( player, true );
	}
};
