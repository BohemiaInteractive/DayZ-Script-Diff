class CraftBaseBallBatBarbed extends RecipeBase
{
	override void Init()
	{
		m_Name = "#STR_CraftBaseBallBatBarbed0";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 1;//animation length in relative time units
		m_Specialty = 0.02;// value > 0 for roughness, value < 0 for precision

		//conditions
		m_MinDamageIngredient[0] = -1;
		m_MaxDamageIngredient[0] = 3;
		m_MinQuantityIngredient[0] = 12;
		m_MaxQuantityIngredient[0] = -1;
		
		m_MinDamageIngredient[1] = -1;
		m_MaxDamageIngredient[1] = 3;
		m_MinQuantityIngredient[1] = -1;
		m_MaxQuantityIngredient[1] = -1;
		
		
		//ingredient 1
		InsertIngredient(0,"BarbedWire");//you can insert multiple ingredients this way

		m_IngredientAddHealth[0] = 0;
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = -1;
		m_IngredientDestroy[0] = true;
		m_IngredientUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"BaseballBat");//you can insert multiple ingredients this way

		m_IngredientAddHealth[1] = 0;
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;
		m_IngredientDestroy[1] = true;
		m_IngredientUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		// crafting multiple results
		//AddResult("CombatKnife");//additional results, must be processed individually in Do method (as "results" parameter)
		
		
		AddResult("BarbedBaseballBat");//single result
		m_ResultSetFullQuantity[0] = 0;
		m_ResultSetQuantity[0] = -1;
		m_ResultSetHealth[0] = -1;
		m_ResultInheritsHealth[0] = 1;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = 1; //(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = 0;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
	}

	
	override void Do( ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight )//gets called upon recipe's completion
	{
		ItemBase gloves = ItemBase.Cast(player.FindAttachmentBySlotName("Gloves"));
		if ( gloves && !gloves.IsDamageDestroyed() )
			return;

		int rand = Math.RandomIntInclusive(0,9);
		if ( rand == 0 )
		{
			rand = Math.RandomIntInclusive(0,1);
			if ( rand == 0 && !player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftForeArmRoll") )
			{
				player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("RightForeArmRoll");
			}
			else if ( rand == 1 && !player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("RightForeArmRoll") )
			{
				player.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("LeftForeArmRoll");
			}
		}
	}
};
