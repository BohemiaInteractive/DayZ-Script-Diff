class OpenScientificBriefcase extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#STR_unlock_briefcase";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 1;//animation length in relative time units
		m_Specialty = 0.03;// value > 0 for roughness, value < 0 for precision
		
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"ScientificBriefcase");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = false;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		m_MaxDamageIngredient[0] = 3;
		
		//ingredient 2
		InsertIngredient(1,"ScientificBriefcaseKeys");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = -2.5;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = false;// false = do nothing
		m_IngredientUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		m_MaxDamageIngredient[1] = 3;
		
		//----------------------------------------------------------------------------------------------------------------------
	}
	
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		ScientificBriefcase brfcs = ScientificBriefcase.Cast(ingredients[0]);
		return !brfcs.IsOpen();
	}
	
	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)
	{
		ScientificBriefcase brfcs = ScientificBriefcase.Cast(ingredients[0]);
		brfcs.Open();
	}
};
