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
	}
	
	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		
		Pelt_Base ingredient1 = Pelt_Base.Cast(ingredients[0]);
		ItemBase ingredient2 = ingredients[1]; //The garden lime
		
		//Evaluate the amount of Lime required to craft leather from Pelt (percentage based)
		float yieldQuantity =  ingredient1.ConfigGetFloat("leatherYield");
		float qtyModifier = (4 - ingredient1.GetHealthLevel(""))/4; // Normalize the health level so Pristine is 1 and Ruined is 0. Necessary like this on CLIENT
		yieldQuantity = Math.Clamp(yieldQuantity * qtyModifier,1,float.MAX);

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
		ItemBase ingredient1 = ingredients[0];
		
		//Set tanned leather output quantity
		int yieldQuantity = ingredient1.ConfigGetFloat("leatherYield");
		float qtyModifier = ingredient1.GetHealth01("","Health");
		yieldQuantity = Math.Clamp(yieldQuantity * qtyModifier,1,float.MAX);
		
		//Use X% of GardenLime for each tanned leather item crafted
		ItemBase gardenLime = ingredients[1];
		float usedLime = (gardenLime.GetQuantityMax() * m_PercentageUsed) * yieldQuantity;
		gardenLime.SetQuantity(gardenLime.GetQuantity() - usedLime);
		
		//Create output piles
		vector posHead;
		MiscGameplayFunctions.GetHeadBonePos(player,posHead);
		vector posTarget = player.GetPosition() + (player.GetDirection() * DEFAULT_SPAWN_DISTANCE);
		MiscGameplayFunctions.CreateItemBasePilesDispersed("TannedLeather",posHead,posTarget,UAItemsSpreadRadius.NARROW,yieldQuantity,float.MAX,player);
	}
}