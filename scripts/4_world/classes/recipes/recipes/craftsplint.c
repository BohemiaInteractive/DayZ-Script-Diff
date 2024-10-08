class CraftSplint extends RecipeBase
{
	override void Init()
	{
		m_Name = "#STR_CraftSplint0";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 1;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		//In the context of this recipe Check CanDo method as Rags can be stacked but bandages can't
		m_MinQuantityIngredient[0] = 1;//-1 = disable check 
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = 2;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"BandageDressing");//you can insert multiple ingredients this way
		InsertIngredient(0,"Rag");//you can insert multiple ingredients this way
		InsertIngredient(0,"DuctTape");//you can insert multiple ingredients this way
		InsertIngredient(0,"Shemag_ColorBase");//you can insert multiple ingredients this way

		
		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = false;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"WoodenStick");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = -2;// 0 = do nothing
		m_IngredientDestroy[1] = false;// false = do nothing
		m_IngredientUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("Splint");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = -2;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -2;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		ItemBase ingredient1 = ingredients[0];
		if (ingredient1.Type() == BandageDressing)
		{
			if (ingredient1.GetQuantity() == ingredient1.GetQuantityMax())
			{
				return true;
			}
			return false;
		}
		
		if (ingredient1.Type() == Rag)
		{
			if (ingredient1.GetQuantity() >= 4)
				return true;
			
			return false;
		}
		
		if (ingredient1.Type() == DuctTape)
		{
			if (ingredient1.GetQuantity() >= (ingredient1.GetQuantityMax()/2))
				return true;
			
			return false;
		}
		
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		ItemBase ingredient1 = ingredients[0];
		
		if (ingredients[0].Type() == Rag)
			ingredient1.AddQuantity(-4);
		
		if (ingredients[0].Type() == BandageDressing)
			ingredient1.AddQuantity(-ingredient1.GetQuantityMax());
		
		if (ingredients[0].Type() == DuctTape)
			ingredient1.AddQuantity(-ingredient1.GetQuantityMax()/2);

		if (ingredients[0].IsInherited(Shemag_ColorBase))
			ingredient1.DeleteSafe();
	}
}
