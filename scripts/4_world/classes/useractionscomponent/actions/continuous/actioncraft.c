class ActionCraftCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousCraft(0); //default value can be set in recipes
	}
};

class ActionCraft: ActionContinuousBase
{
	bool m_HasStarted;
		
	ItemBase m_Item1;
	ItemBase m_Item2;
		
	int m_RecipeID;
	
	void ActionCraft()
	{
		m_Sound = "craft_universal_0";
		
		m_CallbackClass = ActionCraftCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		
		m_Text = "#craft";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}	
		
	//override void OnStart( ActionData action_data )
	//{
		/*
		if( action_data.m_Player.GetCraftingMeta() ) 	
		{
			m_Item1 = action_data.m_Player.GetCraftingMeta().GetIngredient1();
		 	m_Item2 = action_data.m_Player.GetCraftingMeta().GetIngredient2();
			m_RecipeID = action_data.m_Player.GetCraftingMeta().GetRecipeID();
			if(m_Item1 == NULL || m_Item2 == NULL || m_RecipeID < 0) return;
		}
		*/
		//m_HasStarted = true;
	//}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		/*if( !m_HasStarted )
		{
			if ( player.IsCraftingSetUp() )
			{
				return true;
			}
			return false;
		}
		else
		{
			return true;
		}*/
	}


	
	override void ApplyModifiers( ActionData action_data )
	{
	}



	void CancelCraft(PlayerBase player)
	{
		//action_data.m_Player.SetCraftingReady(false);
		//action_data.m_Player.DisableCrafting();
	}
};