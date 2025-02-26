class WorldCraftActionReciveData : ActionReciveData
{
	int 		m_RecipeID;
}
class WorldCraftActionData : ActionData
{
	int m_RecipeID;
}

class ActionWorldCraftCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousCraft(UATimeSpent.DEFAULT_CRAFT); //default value can be set in recipes
	}

	/*override void OnFinish(bool pCanceled)	
	{
		super.OnFinish(pCanceled);
		if( !GetGame().IsDedicatedServer() )
		{
			PlayerBase player;
			if( Class.CastTo(player, GetGame().GetPlayer()) )
			{
				if( player.GetCraftingManager().IsInventoryCraft() )
					player.GetCraftingManager().CancelInventoryCraft();
			}
		}
	}*/
};

class ActionWorldCraft: ActionContinuousBase
{
	private string m_ActionPrompt;

	void ActionWorldCraft()
	{
		m_CallbackClass = ActionWorldCraftCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override ActionData CreateActionData()
	{
		ActionData action_data = new WorldCraftActionData;
		return action_data;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		PluginRecipesManager module_recipes_manager;
		Class.CastTo(module_recipes_manager,  GetPlugin(PluginRecipesManager) );
		m_Text = module_recipes_manager.GetRecipeName( player.GetCraftingManager().GetRecipeID(m_VariantID) ); 
	}
	
	override string GetText()
	{
		PlayerBase player;
		if( Class.CastTo(player, GetGame().GetPlayer()) )
		{
			PluginRecipesManager moduleRecipesManager;
			Class.CastTo(moduleRecipesManager,  GetPlugin(PluginRecipesManager) );
			return moduleRecipesManager.GetRecipeName( player.GetCraftingManager().GetRecipeID(m_VariantID) );
		}

		return "Default worldcraft text";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (GetGame().IsServer())
		{					
			if (!target.GetObject() || !item)
				return false;
		}
		
		return true;
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		if (GetGame().IsServer())
		{					
			if (!action_data.m_Target.GetObject() || !action_data.m_MainItem)
				return false;
			
			PluginRecipesManager moduleRecipesManager;
			Class.CastTo(moduleRecipesManager,  GetPlugin(PluginRecipesManager) );
			
			WorldCraftActionData action_data_wc = WorldCraftActionData.Cast(action_data);
			
			ItemBase item2;
			Class.CastTo(item2, action_data_wc.m_Target.GetObject());
			if(!moduleRecipesManager.IsRecipePossibleToPerform(action_data_wc.m_RecipeID, action_data.m_MainItem, item2, action_data.m_Player))
			{
				return false;
			}
		}
		
		return true;
	}
	
	override string GetSoundCategory(ActionData action_data)
	{
		WorldCraftActionData actionDataWorldCraft = WorldCraftActionData.Cast(action_data);
		
		ItemBase target = ItemBase.Cast(actionDataWorldCraft.m_Target.GetObject());
		ItemBase item = actionDataWorldCraft.m_MainItem;
		
		PluginRecipesManager recipesManager = PluginRecipesManager.Cast(GetPlugin(PluginRecipesManager));
		
		string soundCat = recipesManager.GetSoundCategory(actionDataWorldCraft.m_RecipeID,target, item);
		
		return soundCat;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL )
	{
		if (super.SetupAction(player, target, item, action_data, extra_data ))
		{
			WorldCraftActionData action_data_wc = WorldCraftActionData.Cast(action_data);
			
			if (!GetGame().IsDedicatedServer())	// server synchs the recipe id through HandleReceiveData before this
				action_data_wc.m_RecipeID = player.GetCraftingManager().GetRecipeID(m_VariantID);

			PluginRecipesManager moduleRecipesManager;
			Class.CastTo(moduleRecipesManager, GetPlugin(PluginRecipesManager));
			moduleRecipesManager.IsRecipePossibleToPerform(action_data_wc.m_RecipeID, action_data_wc.m_MainItem, ItemBase.Cast(action_data_wc.m_Target.GetObject()), player); // sorts items server side, needed for anim selection
			
			m_CommandUID = moduleRecipesManager.GetAnimationCommandUID(action_data_wc.m_RecipeID);
			
			return true;
		}
		return false;
	}
		
	override void Start( ActionData action_data ) //Setup on start of action
	{
		super.Start(action_data);
		if ( action_data.m_Player ) action_data.m_Player.TryHideItemInHands(true);
	}
	
	override void OnEndServer( ActionData action_data )
	{
		if ( action_data.m_Player ) action_data.m_Player.TryHideItemInHands(false);
		
		super.OnEndServer(action_data);
	}
	
	override void OnEndClient( ActionData action_data )
	{
		if ( action_data.m_Player ) action_data.m_Player.TryHideItemInHands(false);
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		WorldCraftActionData action_data_wc;
		PluginRecipesManager module_recipes_manager;
		ItemBase item2;
		
		Class.CastTo(action_data_wc, action_data);
		Class.CastTo(module_recipes_manager, GetPlugin(PluginRecipesManager));
		Class.CastTo(item2, action_data.m_Target.GetObject());
		
		if (action_data.m_MainItem && item2)
		{
			if (GetGame().IsMultiplayer())
				ClearActionJuncture(action_data);
			else
				ClearInventoryReservationEx(action_data);
			
			module_recipes_manager.PerformRecipeServer(action_data_wc.m_RecipeID, action_data.m_MainItem, item2, action_data.m_Player);
			
			if (GetGame().IsMultiplayer())
				AddActionJuncture(action_data);
			else
				InventoryReservation(action_data);
		}
	}
		
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		WorldCraftActionData action_data_wc = WorldCraftActionData.Cast(action_data);
		
		ctx.Write(action_data_wc.m_RecipeID);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if (!action_recive_data)
		{
			action_recive_data = new WorldCraftActionReciveData;
		}
		
		super.ReadFromContext(ctx, action_recive_data);

		int recipeID;		
		if (!ctx.Read(recipeID))
			return false;
		
		WorldCraftActionReciveData recive_data_wc = WorldCraftActionReciveData.Cast(action_recive_data);
		recive_data_wc.m_RecipeID = recipeID;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		
		WorldCraftActionReciveData recive_data_wc = WorldCraftActionReciveData.Cast(action_recive_data);
		WorldCraftActionData action_data_wc = WorldCraftActionData.Cast(action_data);
		
		action_data_wc.m_MainItem = recive_data_wc.m_MainItem;
		if (!action_recive_data.m_Target)
		{
			action_data.m_Target = new ActionTarget(NULL, NULL, -1, vector.Zero, 0); 
		}
		else
		{
			action_data_wc.m_Target = recive_data_wc.m_Target;
		}
		action_data_wc.m_RecipeID = recive_data_wc.m_RecipeID;
	}
};

