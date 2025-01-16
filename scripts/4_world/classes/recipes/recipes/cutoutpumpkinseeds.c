class CutOutPumpkinSeeds extends CutOutSeeds
{	
	override void Init()
	{
		m_Name = "#STR_prepare0";
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
		InsertIngredient(0,"Pumpkin");					// you can insert multiple ingredients this way

		m_IngredientAddHealth[0]			= 0;		// 0 = do nothing
		m_IngredientSetHealth[0]			= -1;		// -1 = do nothing
		m_IngredientAddQuantity[0]			= 0;		// 0 = do nothing
		m_IngredientDestroy[0]				= true;		// true = destroy, false = do nothing

		//ingredient 2
		InsertIngredient(1,"Sickle");
		InsertIngredient(1,"KukriKnife");
		InsertIngredient(1,"FangeKnife");
		InsertIngredient(1,"Hacksaw");
		InsertIngredient(1,"KitchenKnife");
		InsertIngredient(1,"SteakKnife");
		InsertIngredient(1,"StoneKnife");
		InsertIngredient(1,"Cleaver");
		InsertIngredient(1,"CombatKnife");
		InsertIngredient(1,"HuntingKnife");
		InsertIngredient(1,"Machete");
		InsertIngredient(1,"CrudeMachete");
		InsertIngredient(1,"OrientalMachete");
		InsertIngredient(1,"AK_Bayonet");
		InsertIngredient(1,"M9A1_Bayonet");
		InsertIngredient(1,"Mosin_Bayonet");
		InsertIngredient(1,"SKS_Bayonet");
		InsertIngredient(1,"FarmingHoe");
		InsertIngredient(1,"Shovel");
		InsertIngredient(1,"Crowbar");
		InsertIngredient(1,"Hammer");
		InsertIngredient(1,"Mace");
		InsertIngredient(1,"Wrench");
		InsertIngredient(1,"LugWrench");
		InsertIngredient(1,"Screwdriver");
		InsertIngredient(1,"SledgeHammer");
		InsertIngredient(1,"BaseballBat");
		InsertIngredient(1,"Pickaxe");
		InsertIngredient(1,"Sword");
		InsertIngredient(1,"WoodAxe");
		InsertIngredient(1,"FirefighterAxe");
		InsertIngredient(1,"Hatchet");
		InsertIngredient(1,"HandSaw");
		InsertIngredient(1,"BoneKnife");

		m_IngredientAddHealth[1]		= -2;
		m_IngredientSetHealth[1]		= -1;
		m_IngredientAddQuantity[1]		= 0;
		m_IngredientDestroy[1]			= false;


		//----------------------------------------------------------------------------------------------------------------------
		//RESULTS
		//result1
		AddResult("PumpkinSeeds");				//add results here

		m_ResultSetFullQuantity[0]		= false;	// true = set full quantity, false = do nothing
		m_ResultSetQuantity[0]			= 12;		// -1 = do nothing
		m_ResultSetHealth[0]			= -1;		// -1 = do nothing
		m_ResultInheritsHealth[0]		= 0;		// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0]		= -1;		// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0]			= -2;		// (value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultReplacesIngredient[0]	= -1;		// (value) == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
	
		//result2
		AddResult("SlicedPumpkin");//add results here

		m_ResultSetFullQuantity[1]		= true;
		m_ResultSetQuantity[1]			= -1;
		m_ResultSetHealth[1]			= -1;
		m_ResultInheritsHealth[1]		= 0;
		m_ResultInheritsColor[1]		= -1;
		m_ResultToInventory[1]			= 1;
		m_ResultReplacesIngredient[1]	= -1;

		//result3
		AddResult("SlicedPumpkin");//add results here

		m_ResultSetFullQuantity[2]		= true;
		m_ResultSetQuantity[2]			= -1;
		m_ResultSetHealth[2]			= -1;
		m_ResultInheritsHealth[2]		= 0;
		m_ResultInheritsColor[2]		= -1;
		m_ResultToInventory[2]			= 1;
		m_ResultReplacesIngredient[2]	= -1;
	}

	//gets called upon recipe's completion
	override void Do(ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight)
	{		
		ItemBase pumpkin = ingredients[0];

		ItemBase slice1 = results[1];	
		ItemBase slice2 = results[2];
		
		float quantityMultiplier = pumpkin.GetHealth01("",""); 
		float quantityNorm = pumpkin.GetQuantity()/pumpkin.GetQuantityMax();
		
		quantityMultiplier *= quantityNorm;
		
		if ( pumpkin )
		{
			if ( slice1 )
			{
				slice1.SetQuantityNormalized(quantityMultiplier);
				MiscGameplayFunctions.TransferItemProperties(pumpkin, slice1);
			}
			
			if ( slice2 )
			{
				slice2.SetQuantityNormalized(quantityMultiplier);
				MiscGameplayFunctions.TransferItemProperties(pumpkin, slice2);
			}	
		}
	}
}
