class CraftMortarMix extends RecipeBase	
{	
	private const float REQUIRED_QUANTITY = 1000.0;
	
	override void Init()
	{
		
		m_Name = "#STR_CraftMortarMix0";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 2.0;
		m_Specialty = 0.02;// value > 0 for roughness, value < 0 for precision
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = 1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"MortarMix",DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR, true);//you can insert multiple ingredients this way

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"Pot", DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR);//you can insert multiple ingredients this way
		InsertIngredient(1,"CanisterGasoline", DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR);//you can insert multiple ingredients this way
		InsertIngredient(1,"Canteen", DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR);//you can insert multiple ingredients this way
		InsertIngredient(1,"WaterBottle", DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR);//you can insert multiple ingredients this way
		InsertIngredient(1,"FilteringBottle", DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR);//you can insert multiple ingredients this way
		InsertIngredient(1,"GlassBottle", DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR);//you can insert multiple ingredients this way
		InsertIngredient(1,"WineBottle", DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR);//you can insert multiple ingredients this way
		InsertIngredient(1,"WaterPouch_ColorBase", DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR);//you can insert multiple ingredients this way
		InsertIngredient(1,"Barrel_ColorBase", DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR);//you can insert multiple ingredients this way
		InsertIngredient(1,"AyranBottle", DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR);//you can insert multiple ingredients this way

		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = false;// false = do nothing
		m_IngredientUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("MortarMix_Opened");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = -1;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = 0;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -2;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
	    ItemBase container;
	
	    if (!ItemBase.CastTo(container, ingredients[1]))
	        return false;
	
	    // Must not be frozen
	    if (container.GetIsFrozen())
	        return false;
	
	    // Must contain water (pure water type)
	    int liquidType = container.GetLiquidType();
	    if ((liquidType & LIQUID_WATER) == 0)
	        return false;
	
	    // Require at least 1 liter (1000 units)
	    float qty = container.GetQuantity();
	    if (qty < REQUIRED_QUANTITY)
	        return false;
	
	    return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight)
	{
	    ItemBase container = ingredients[1];
	
	    // Consume exactly 1 liter (1000 units)
	    float required = REQUIRED_QUANTITY;
	    float current = container.GetQuantity();
	
	    if (current >= required)
	    {
	        container.AddQuantity(-required);
	    }
	    else
	    {
	        // Fallback: if somehow less than required got through, consume what remains
	        container.SetQuantity(0);
	    }
	}
};
