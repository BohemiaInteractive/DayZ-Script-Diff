class FillGasMask_Filter extends RecipeBase	
{	
	override void Init()
	{
		m_Name = "#STR_FillFilter";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 0.5;//animation length in relative time units
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
		InsertIngredient(0,"CharcoalTablets");//you can insert multiple ingredients this way

		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = false;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"GasMask_Filter");//you can insert multiple ingredients this way
		InsertIngredient(1,"GasMask_Filter_Improvised");
		
		m_IngredientAddHealth[1] = 0;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = false;// false = do nothing
		m_IngredientUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		//----------------------------------------------------------------------------------------------------------------------
		/*
		//result1
		//AddResult("GasMask_Filter_Improvised");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = 0;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = 1;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -2;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
		*/
	}

	override bool CanDo( ItemBase ingredients[], PlayerBase player )//final check for recipe's validity
	{
		ItemBase filter = ingredients[1];
		
		//Do not fill alread full filters
		if(filter.GetQuantity() >= filter.GetQuantityMax())
		{
			return false;
		}		
		
		// Check if not attached to a gas mask
		InventoryLocation il = new InventoryLocation;
		filter.GetInventory().GetCurrentInventoryLocation(il);
		EntityAI inv = il.GetParent();
		
		if(inv != null)
		{
			if (inv.GetType() == "GP5GasMask" || inv.GetType() == "AirborneMask")
			{	
				 return false;
			}
		}		
		
		return true;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		// Get charcoal quantity
		ItemBase charcoal = ingredients[0];	
		float charcoalCurrent = charcoal.GetQuantity();
		
		// Get filter quantity
		ItemBase filter = ingredients[1];
		
		float filterMax = filter.GetQuantityMax();
		float filterCurrent = filter.GetQuantity();
		
		//1 tablet = 10 quantity
		float fillAmount = filterCurrent + (charcoalCurrent * 10);
		
		// All if possible
		if(fillAmount >= filterMax)
		{
			ingredients[1].SetQuantity(filterMax);
			
			//Remove only the used part from charcoal
			fillAmount = fillAmount - filterMax;
			ingredients[0].SetQuantity(Math.Round(fillAmount / 10));
		}
		else
		{		
			ingredients[1].SetQuantity( fillAmount);
			ingredients[0].AddQuantity( - charcoalCurrent);
		}
	}
};
