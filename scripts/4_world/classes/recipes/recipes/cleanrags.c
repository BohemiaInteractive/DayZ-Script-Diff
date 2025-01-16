class CleanRags extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#clean";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 1;//animation length in relative time units
		m_Specialty = -0.01;// value > 0 for roughness, value < 0 for precision
		
		
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
		InsertIngredient(0,"Rag");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = false;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"Pot");//you can insert multiple ingredients this way
		InsertIngredient(1,"CanisterGasoline");//you can insert multiple ingredients this way
		InsertIngredient(1,"Canteen");//you can insert multiple ingredients this way
		InsertIngredient(1,"WaterBottle");//you can insert multiple ingredients this way
		InsertIngredient(1,"FilteringBottle");//you can insert multiple ingredients this way
		InsertIngredient(1,"GlassBottle");//you can insert multiple ingredients this way
		InsertIngredient(1,"WaterPouch_ColorBase");//you can insert multiple ingredients this way
		InsertIngredient(1,"Barrel_ColorBase");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = false;// false = do nothing
		m_IngredientUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this ingredient
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		ItemBase rags;
		Class.CastTo(rags, ingredients[0]);
		ItemBase container;
		Class.CastTo(container, ingredients[1]);
		
		if ((container.GetLiquidType() & LIQUID_WATER) && container.GetQuantity() > 0 && !container.GetIsFrozen() && rags.GetHealthLevel() > 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		ItemBase rags = ingredients[0];
		ItemBase container = ingredients[1];
		
		float 	rags_health 	= rags.GetHealth("","");
		int 	rags_quantity 	= rags.GetQuantity();
		
		float 	rags_combined_damage = (100 - rags_health) * rags_quantity;
		
		int 	liquid_quantity = container.GetQuantity();
		int 	liquid_required = rags_combined_damage * 2;
		
		float 	used_ratio = 0;
		float	heal = 0;
		
		if( liquid_quantity > liquid_required )
		{
			container.AddQuantity(-liquid_required);
			heal = 	rags_combined_damage / rags_quantity;
		}
		else
		{
			container.SetQuantity(0);
			used_ratio = liquid_quantity / liquid_required;
			heal = 	(rags_combined_damage / rags_quantity) * used_ratio;
			
		}
		heal = Math.Clamp(heal, 0, (70 - rags_health));
		rags.AddHealth("", "",heal);
	}
};
