class CraftTannedLeather extends RecipeBase
{
	float m_PercentageUsed = 0.05; 				//Variable used to define relative quantity of Lime used when crafting tanned leather
	
	//Init Leather crafting recipe
	override void Init()
	{
		m_Name = "#STR_CraftTannedLeather0";
		m_IsInstaRecipe = false;				//should this recipe be performed instantly without animation
		m_AnimationLength = 1;					//animation length in relative time units
		m_Specialty = 0.02;						// value > 0 for roughness, value < 0 for precision
		
		m_AnywhereInInventory = true;
		//conditions
		m_MinDamageIngredient[0] = -1;			//-1 = disable check
		m_MaxDamageIngredient[0] = 3;			//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;		//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;		//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;			//-1 = disable check
		m_MaxDamageIngredient[1] = 3;			//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;		//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;		//-1 = disable check
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0, "Pelt_Base");		//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[0] = 0;			// 0 = do nothing
		m_IngredientSetHealth[0] = -1; 			// -1 = do nothing
		m_IngredientAddQuantity[0] = -1;		// 0 = do nothing
		m_IngredientDestroy[0] = true;			//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;	// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1, "GardenLime");		//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;			// 0 = do nothing
		m_IngredientSetHealth[1] = -1; 			// -1 = do nothing
		m_IngredientAddQuantity[1] = 0;			// 0 = do nothing
		m_IngredientDestroy[1] = false;			//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[1] = false;	// set 'true' to allow modification of the values by softskills on this ingredient
		
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("TannedLeather");				//add results here

		m_ResultSetFullQuantity[0] = false;		//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1; 			//-1 = do nothing
		m_ResultSetHealth[0] = -1;				//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;			// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;			// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -2;			//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;		// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;		// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
	}
	
	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		
		//return true;
		Pelt_Base ingredient1 = Pelt_Base.Cast(ingredients[0]);
		ItemBase ingredient2 = ingredients[1]; //The garden lime
		
		//Evaluate the amount of Lime required to craft leather from Pelt (percentage based)
		float yieldQuantity =  ingredient1.ConfigGetFloat("leatherYield");
		float qtyModifier = (4 - ingredient1.GetHealthLevel(""))/4; // Normalize the health level so Pristine is 1 and Ruined is 0
		yieldQuantity = yieldQuantity * qtyModifier;

		float m_NeededQuantity = (ingredient2.GetQuantityMax() * m_PercentageUsed) * yieldQuantity; 
		if( ingredient1.ConfigGetFloat("leatherYield") >= 0 && ingredient2.GetQuantity() >= m_NeededQuantity)
		{
			return true;
		}
		else
		{
			return false;
		}
	
	}

	
	override void Do( ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight )//gets called upon recipe's completion
	{
		Debug.Log("Craft Tanned Leather","recipes");
		
		ItemBase result;
		Class.CastTo(result, results.Get(0));
		ItemBase ingredient1 = ingredients[0];
		
		//Set tanned leather output quantity
		int quantity = ingredient1.ConfigGetFloat("leatherYield");
		float qtyModifier = (4 - ingredient1.GetHealthLevel(""))/4; // Normalize the health level so Pristine is 1 and Ruined is 0
		quantity = quantity * qtyModifier;

		result.SetQuantity(quantity);
		
		//Use X% of GardenLime for each tanned leather item crafted
		ItemBase gardenLime = ingredients[1];
		float usedLime = (gardenLime.GetQuantityMax() * m_PercentageUsed) * quantity;
		gardenLime.SetQuantity(gardenLime.GetQuantity() - usedLime);

		result.SetHealthMax("","");
		
		//Split stack if returned quantity over Max stack
		if (quantity > result.GetQuantityMax())
		{
			//Allow spawn of a second stack of tanned leather
			MiscGameplayFunctions.CreateItemBasePiles("TannedLeather", player.GetPosition(), (quantity - 8), result.GetMaxHealth() ,false);
		}
	}
}