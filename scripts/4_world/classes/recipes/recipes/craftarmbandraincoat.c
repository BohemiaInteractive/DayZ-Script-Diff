class CraftArmbandRaincoat extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#STR_craftarmband0";
		m_IsInstaRecipe = false;					// should this recipe be performed instantly without animation
		m_AnimationLength = 1.8;					// animation length in relative time units

		//conditions
		m_MinDamageIngredient[0]		= -1;		// -1 = disable check
		m_MaxDamageIngredient[0]		= 3; 		// -1 = disable check
		m_MinQuantityIngredient[0]		= -1;		// -1 = disable check
		m_MaxQuantityIngredient[0]		= -1;		// -1 = disable check

		m_MinDamageIngredient[1]		= -1;
		m_MaxDamageIngredient[1]		= 3;
		m_MinQuantityIngredient[1]		= -1;
		m_MaxQuantityIngredient[1]		= -1;

		//----------------------------------------------------------------------------------------------------------------------
		//INGREDIENTS
		//ingredient 1
		InsertIngredientEx(0,"Raincoat_ColorBase","ImprovisedCloth_craft");	// you can insert multiple ingredients this way

		m_IngredientAddHealth[0]		= 0;		// 0 = do nothing
		m_IngredientSetHealth[0]		= -1;		// -1 = do nothing
		m_IngredientAddQuantity[0]		= 0;		// 0 = do nothing
		m_IngredientDestroy[0]			= 1;		// true = destroy, false = do nothing

		//ingredient 2
		InsertIngredient(1,"Sickle");
		InsertIngredient(1,"KukriKnife");
		InsertIngredient(1,"FangeKnife");
		InsertIngredient(1,"Hacksaw");
		InsertIngredient(1,"HandSaw");
		InsertIngredient(1,"KitchenKnife");
		InsertIngredient(1,"SteakKnife");
		InsertIngredient(1,"HayHook");
		InsertIngredient(1,"StoneKnife");
		InsertIngredient(1,"Cleaver");
		InsertIngredient(1,"CombatKnife");
		InsertIngredient(1,"HuntingKnife");
		InsertIngredient(1,"Machete");
		InsertIngredient(1,"CrudeMachete");
		InsertIngredient(1,"OrientalMachete");
		InsertIngredient(1,"Screwdriver");
		InsertIngredient(1,"Crowbar");
		InsertIngredient(1,"Pickaxe");
		InsertIngredient(1,"WoodAxe");
		InsertIngredient(1,"Hatchet");
		InsertIngredient(1,"FirefighterAxe");
		InsertIngredient(1,"Sword");
		InsertIngredient(1,"AK_Bayonet");
		InsertIngredient(1,"M9A1_Bayonet");
		InsertIngredient(1,"Mosin_Bayonet");
		InsertIngredient(1,"SKS_Bayonet");
		InsertIngredient(1,"BoneKnife");
		
		m_IngredientAddHealth[1]		= -2;
		m_IngredientSetHealth[1]		= -1;
		m_IngredientAddQuantity[1]		= 0;
		m_IngredientDestroy[1]			= 0;

		//----------------------------------------------------------------------------------------------------------------------
		//RESULTS
		//result1
		AddResult("Armband_");							// add results here

		m_ResultSetFullQuantity[0]		= false;		// true = set full quantity, false = do nothing
		m_ResultSetQuantity[0]			= -1;   		// -1 = do nothing
		m_ResultSetHealth[0]			= -1;   		// -1 = do nothing
		m_ResultInheritsHealth[0]		= 0;    		// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this resu
		m_ResultInheritsColor[0]		= 0;    		// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and confi
		m_ResultToInventory[0]			= -2;   		// (value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredien
		m_ResultReplacesIngredient[0]	= -1;   		// (value) == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
		
		//result2
		AddResult("Armband_");

		m_ResultSetFullQuantity[1]		= false;
		m_ResultSetQuantity[1]			= -1;
		m_ResultSetHealth[1]			= -1;
		m_ResultInheritsHealth[1]		= 0;
		m_ResultInheritsColor[1]		= 0;
		m_ResultToInventory[1]			= -2;
		m_ResultReplacesIngredient[1]	= -1;
		
		//result3
		AddResult("Armband_");

		m_ResultSetFullQuantity[2]		= false;
		m_ResultSetQuantity[2]			= -1;
		m_ResultSetHealth[2]			= -1;
		m_ResultInheritsHealth[2]		= 0;
		m_ResultInheritsColor[2]		= 0;
		m_ResultToInventory[2]			= -2;
		m_ResultReplacesIngredient[2]	= -1;
		
		//result4
		AddResult("Armband_");

		m_ResultSetFullQuantity[3]		= false;
		m_ResultSetQuantity[3]			= -1;
		m_ResultSetHealth[3]			= -1;
		m_ResultInheritsHealth[3]		= 0;
		m_ResultInheritsColor[3]		= 0;
		m_ResultToInventory[3]			= -2;
		m_ResultReplacesIngredient[3]	= -1;
		
		//result5
		AddResult("Armband_");

		m_ResultSetFullQuantity[4]		= false;
		m_ResultSetQuantity[4]			= -1;
		m_ResultSetHealth[4]			= -1;
		m_ResultInheritsHealth[4]		= 0;
		m_ResultInheritsColor[4]		= 0;
		m_ResultToInventory[4]			= -2;
		m_ResultReplacesIngredient[4]	= -1;
	}

	//final check for recipe's validity
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		return ingredients[0].IsEmpty();
	}
};
