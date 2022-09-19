class ActionTakeMaterialToHands: ActionInteractBase
{
	void ActionTakeMaterialToHands()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HANDS;
		m_Text = "#take";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINotPresent;
		m_ConditionTarget = new CCTCursor;
	}	
	
	override bool HasProneException()
	{
		return true;
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		EntityAI attachment = construction_action_data.GetActualAttachmentToDetach();
			
		if ( attachment )
		{
			m_Text = "#take " + attachment.GetDisplayName();
		}
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool HasProgress()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		EntityAI target_entity = EntityAI.Cast( target.GetObject() );
		if ( target_entity && target_entity.CanUseConstruction() )
		{
			BaseBuildingBase base_building = BaseBuildingBase.Cast( target_entity );
			if(!base_building.IsPlayerInside(player,""))
				return false;
			
			ConstructionActionData construction_action_data = player.GetConstructionActionData();
			string main_part_name = target_entity.GetActionComponentName( target.GetComponentIndex() );
			//Print(main_part_name);
			
			if ( GetGame().IsMultiplayer() || GetGame().IsServer() )
			{
				construction_action_data.RefreshAttachmentsToDetach( target_entity, main_part_name );
			}
			EntityAI attachment = construction_action_data.GetActualAttachmentToDetach();	
			
			if ( attachment && player.GetInventory().CanAddEntityIntoHands( attachment ) && attachment.GetHierarchyRootPlayer() != player )
			{
				return true;
			}
		}
		
		return false;
	}
	
	override bool CanContinue( ActionData action_data )
	{
		return true;
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		OnExecuteImpl(action_data);
	}
	
	protected void OnExecuteImpl( ActionData action_data )
	{
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		ItemBase item_target = ItemBase.Cast( construction_action_data.GetActualAttachmentToDetach() );
		if( item_target )
		{
			float stackable = item_target.GetTargetQuantityMax();
			
			if( stackable == 0 || stackable >= item_target.GetQuantity() )
			{
				//take to hands
				action_data.m_Player.PredictiveTakeEntityToHands( item_target );
			}
			else if( stackable != 0 && stackable < item_target.GetQuantity() )
			{
				//split and take to hands
				item_target.SplitIntoStackMaxHandsClient( action_data.m_Player );
			}
		}
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		if (GetGame().IsMultiplayer())
			return; // multiplayer handled on client side via OnExecuteClient
		else
			OnExecuteImpl(action_data); // single player
	}
	
	override void CreateAndSetupActionCallback( ActionData action_data )
	{
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		EntityAI target = construction_action_data.GetActualAttachmentToDetach();
		
		bool heavy_item = false;
		ActionBaseCB callback;
		if ( target && target.ConfigIsExisting("heavyItem") && target.ConfigGetBool("heavyItem") )
		{
			heavy_item = true;
		}
			
		if( heavy_item )
		{
			Class.CastTo(callback, action_data.m_Player.StartCommand_Action(DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HEAVY,GetCallbackClassTypename(),DayZPlayerConstants.STANCEMASK_ERECT));
		}
		else
		{
			if( action_data.m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT) )
			{
				Class.CastTo(callback, action_data.m_Player.AddCommandModifier_Action(DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS,GetCallbackClassTypename()));
			}
			else
			{
				Class.CastTo(callback, action_data.m_Player.StartCommand_Action(DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HANDS,GetCallbackClassTypename(),DayZPlayerConstants.STANCEMASK_PRONE));
			}
		}
		callback.SetActionData(action_data); 
		callback.InitActionComponent();
		action_data.m_Callback = callback;
	}
}