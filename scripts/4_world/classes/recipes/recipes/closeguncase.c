class CloseGunCase extends RecipeBase
{	
	
	protected ref map<string, string> m_AllowedCombines;
	
	void CloseGunCase()
	{
		m_AllowedCombines = new map<string, string>();
		m_AllowedCombines.Insert("GunCase_Black", "GunCaseKeys_Blue");
		m_AllowedCombines.Insert("GunCase_Orange", "GunCaseKeys_Orange");
		m_AllowedCombines.Insert("GunCase_White", "GunCaseKeys_Red");
	}
	
	override void Init()
	{
		m_Name = "#lock";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 1;//animation length in relative time units
		m_Specialty = 0.03;// value > 0 for roughness, value < 0 for precision
		
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"GunCase_ColorBase");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = false;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		m_MaxDamageIngredient[0] = 3;
		
		//ingredient 2
		InsertIngredient(1,"GunCaseKeys_Colorbase");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1] = -0.5;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = false;// false = do nothing
		m_IngredientUseSoftSkills[1] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		m_MaxDamageIngredient[1] = 3;
		
		//----------------------------------------------------------------------------------------------------------------------
	}
	
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		GunCase_ColorBase gncs = GunCase_ColorBase.Cast(ingredients[0]);
		GunCaseKeys_Colorbase key = GunCaseKeys_Colorbase.Cast(ingredients[1]);
		foreach (string caseName, string keyName: m_AllowedCombines)
		{
			if (caseName == gncs.GetType() && keyName == key.GetType())
				return gncs.IsOpen();
		}
		
		return false;
	}
	
	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)
	{
		GunCase_ColorBase gncs = GunCase_ColorBase.Cast(ingredients[0]);
		gncs.Close();
	}
};
