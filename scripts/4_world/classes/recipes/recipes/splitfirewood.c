class SplitFirewood extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#STR_split0";
		m_IsInstaRecipe = false;					// should this recipe be performed instantly without animation
		m_AnimationLength = 1;						// animation length in relative time units

		//conditions
		m_MinDamageIngredient[0]		= -1;		// -1 = disable check
		m_MaxDamageIngredient[0]		= 3;		// -1 = disable check
		m_MinQuantityIngredient[0]		= -1;		// -1 = disable check
		m_MaxQuantityIngredient[0]		= -1;		// -1 = disable check

		m_MinDamageIngredient[1]		= -1;
		m_MaxDamageIngredient[1]		= 3;
		m_MinQuantityIngredient[1]		= -1;
		m_MaxQuantityIngredient[1]		= -1;

		//----------------------------------------------------------------------------------------------------------------------
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"Firewood");	// you can insert multiple ingredients this way

		m_IngredientAddHealth[0]		= 0;				// 0 = do nothing
		m_IngredientSetHealth[0]		= -1;				// -1 = do nothing
		m_IngredientAddQuantity[0]		= -1;				// 0 = do nothing
		m_IngredientDestroy[0]			= false;			// true = destroy, false = do nothing

		//ingredient 2
		InsertIngredientEx(1,"Hacksaw"  , 		 "FirewoodSplit_Saw", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredientEx(1,"HandSaw"  , 		 "FirewoodSplit_Saw", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredientEx(1,"Pickaxe"  , 		 "FirewoodSplit_Axe", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredientEx(1,"WoodAxe"  , 		 "FirewoodSplit_Axe", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredientEx(1,"Hatchet"  ,  		 "FirewoodSplit_Axe", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredientEx(1,"FirefighterAxe" ,  "FirewoodSplit_Axe", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);

		m_IngredientAddHealth[1]		= -6;
		m_IngredientSetHealth[1]		= -1;
		m_IngredientAddQuantity[1]		= 0;
		m_IngredientDestroy[1]			= false;
		
		//----------------------------------------------------------------------------------------------------------------------
		//RESULTS
		//result1
		AddResult("WoodenStick");							// add results here

		m_ResultSetFullQuantity[0]		= false;			// true = set full quantity, false = do nothing
		m_ResultSetQuantity[0]			= 3;				// -1 = do nothing
		m_ResultSetHealth[0]			= -1;				// -1 = do nothing
		m_ResultInheritsHealth[0]		= 0;				// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0]		= -1;				// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0]			= -2;				// (value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultReplacesIngredient[0]	= -1;				// (value) == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
	}

	//final check for recipe's validity
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		return true;
	}
};
