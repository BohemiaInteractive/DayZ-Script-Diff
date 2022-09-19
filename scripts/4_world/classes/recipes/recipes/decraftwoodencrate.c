class DeCraftWoodenCrate extends RecipeBase
{
	override void Init()
	{
		m_Name = "#STR_breakdown0";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 1;//animation length in relative time units
		m_Specialty = 0.02;// value > 0 for roughness, value < 0 for precision
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"WoodenCrate");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"Crowbar");
		InsertIngredient(1,"WoodAxe");
		InsertIngredient(1,"Hatchet");
		InsertIngredient(1,"FirefighterAxe");
		
		m_IngredientAddHealth[1] = -20;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = false;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("WoodenPlank");//add results here
		
		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = 2;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = 1;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -2;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
		
		//result2
		AddResult("Nail");//add results here
		
		m_ResultSetFullQuantity[1] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[1] = 6;// -1 = do nothing
		m_ResultSetHealth[1] = -1;// -1 = do nothing
		m_ResultInheritsHealth[1] = 1;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[1] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[1] = -2;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[1] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
		//----------------------------------------------------------------------------------------------------------------------
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		ItemBase crate;
		Class.CastTo(crate, ingredients[0]);

		if ( crate.GetInventory().CountInventory() > 1 )
			return false;

		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		ItemBase crate;
		Class.CastTo(crate, ingredients[0]);
		int hp = crate.GetHealthLevel("");

		switch ( hp )
		{
			case GameConstants.STATE_PRISTINE:
				results[0].SetQuantity(Math.RandomIntInclusive( 6, 8 ));
				results[0].SetHealth( results[0].GetMaxHealth() * Math.RandomFloat( 0.6, 0.8 ));
				results[1].SetQuantity(Math.RandomIntInclusive( 12, 16 ));
				results[1].SetHealth( results[1].GetMaxHealth() * Math.RandomFloat( 0.6, 0.8 ));
			break;

			case GameConstants.STATE_WORN:
				results[0].SetQuantity(Math.RandomIntInclusive( 5, 7 ));
				results[0].SetHealth( results[0].GetMaxHealth() * Math.RandomFloat( 0.5, 0.8 ));
				results[1].SetQuantity(Math.RandomIntInclusive( 9, 13 ));
				results[1].SetHealth( results[1].GetMaxHealth() * Math.RandomFloat( 0.5, 0.8 ));
			break;

			case GameConstants.STATE_DAMAGED:
				results[0].SetQuantity(Math.RandomIntInclusive( 4, 6 ));
				results[0].SetHealth( results[0].GetMaxHealth() * Math.RandomFloat( 0.3, 0.6 ));
				results[1].SetQuantity(Math.RandomIntInclusive( 6, 10 ));
				results[1].SetHealth( results[1].GetMaxHealth() * Math.RandomFloat( 0.3, 0.6 ));
			break;
			
			case GameConstants.STATE_BADLY_DAMAGED:
				results[0].SetQuantity(Math.RandomIntInclusive( 3, 5 ));
				results[0].SetHealth( results[0].GetMaxHealth() * Math.RandomFloat( 0.1, 0.3 ));
				results[1].SetQuantity(Math.RandomIntInclusive( 3, 7 ));
				results[1].SetHealth( results[1].GetMaxHealth() * Math.RandomFloat( 0.1, 0.3 ));
			break;

			//unnescessary? We do not allow recipe crafting with ruined items?
			case GameConstants.STATE_RUINED:
				results[0].SetQuantity(Math.RandomIntInclusive( 1, 2 ));
				results[0].SetHealth( results[0].GetMaxHealth() * Math.RandomFloat( 0.05, 0.2 ));
				results[1].SetQuantity(Math.RandomIntInclusive( 1, 4 ));
				results[1].SetHealth( results[1].GetMaxHealth() * Math.RandomFloat( 0.05, 0.2 ));
			break;

			default:
				results[0].SetQuantity(Math.RandomIntInclusive( 1, 2 ));
				results[0].SetHealth( results[0].GetMaxHealth() * Math.RandomFloat( 0.05, 0.2 ));
				results[1].SetQuantity(Math.RandomIntInclusive( 1, 4 ));
				results[1].SetHealth( results[1].GetMaxHealth() * Math.RandomFloat( 0.05, 0.2 ));
			break;
		}
		
		/*
		float originalPlanks = 8;
		float originalNails = 16;

		float hp = crate.GetHealth01("","Health");
		originalPlanks *= hp;
		originalNails *= hp;

		int newPlanks = Math.Round( originalPlanks );
		int newNails = Math.Round( originalNails );

		m_ResultSetQuantity[0] = newPlanks;
		m_ResultSetQuantity[1] = newNails;
		*/
		Debug.Log("Recipe Do method called","recipes");
	}
};
