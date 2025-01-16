class CleanWeapon extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#repair";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 1.5;//animation length in relative time units
		m_Specialty = -0.02;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = 1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"WeaponCleaningKit",DayZPlayerConstants.CMD_ACTIONFB_CLEANING_WEAPON);//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = false;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"DefaultWeapon");
		InsertIngredient(1,"DefaultMagazine");
		InsertIngredient(1,"ItemSuppressor");
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = false;// false = do nothing
		m_IngredientUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this ingredient
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		PluginRepairing module_repairing;
		Class.CastTo(module_repairing, GetPlugin(PluginRepairing));
		ItemBase ingredient1;
		Class.CastTo(ingredient1, ingredients[0]);
		ItemBase ingredient2;
		Class.CastTo(ingredient2, ingredients[1]);
		return module_repairing.CanRepair(ingredient1,ingredient2);
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		PluginRepairing module_repairing;
		Class.CastTo(module_repairing, GetPlugin(PluginRepairing));
		PlayerBase playerPB;
		Class.CastTo(playerPB, player);
		ItemBase ingredient1;
		Class.CastTo(ingredient1, ingredients[0]);
		ItemBase ingredient2;
		Class.CastTo(ingredient2, ingredients[1]);
		module_repairing.Repair(playerPB, ingredient1,ingredient2,m_Specialty);
	}
	
	override bool IsRepeatable()
	{
		return true;
	}
};
