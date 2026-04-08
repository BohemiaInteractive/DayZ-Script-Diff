class PrepareAnimal extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#skin";
		m_IsInstaRecipe = false;					// should this recipe be performed instantly without animation
		m_AnimationLength = 1.5;					// animation length in relative time units
		m_RecipeUID = DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING;

		//----------------------------------------------------------------------------------------------------------------------
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
		InsertIngredient(1,"Sickle", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);					//you can insert multiple ingredients this way
		InsertIngredient(1,"KukriKnife", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"FangeKnife", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"Hacksaw", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"HandSaw", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"KitchenKnife", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"SteakKnife", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"StoneKnife", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"Cleaver", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"CombatKnife", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"HuntingKnife", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"Machete", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"CrudeMachete", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"OrientalMachete", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"WoodAxe", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"Hatchet", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"FirefighterAxe", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"Sword", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"AK_Bayonet", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"M9A1_Bayonet", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"SKS_Bayonet", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"BoneKnife", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"Screwdriver", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);
		InsertIngredient(1,"Iceaxe", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, true);

		m_IngredientAddHealth[1]		= -6;			// 0 = do nothing
		m_IngredientSetHealth[1]		= -1;			// -1 = do nothing
		m_IngredientAddQuantity[1]		= 0;			// 0 = do nothing
		m_IngredientDestroy[1]			= false;		// false = do nothing
	}

	//final check for recipe's validity
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		return !ingredients[0].GetIsFrozen();
	}
	
	//gets called upon recipe's completion
	override void Do(ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight)
	{
		ItemBase ingredient = ingredients[0];
		
		for (int i=0; i < results.Count(); ++i)
		{
			//Transfer current food state
			MiscGameplayFunctions.TransferItemProperties(ingredient, results[i]);
			results[i].SetQuantityNormalized(ingredient.GetQuantityNormalized());
		}

		SetBloodyHands(ingredients, player);
	}
	
	protected void SetBloodyHands(ItemBase ingredients[], PlayerBase player)
	{
		ItemBase ingredient = ingredients[0];

		PluginLifespan lifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan));
		lifespan.UpdateBloodyHandsVisibility(player, true);
		player.SetBloodyHandsPenaltyChancePerAgent(eAgents.SALMONELLA, ingredient.GetSkinningBloodInfectionChance(eAgents.SALMONELLA));	
	}
}
