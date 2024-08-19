class ActionTakeHybridAttachmentToHands: ActionInteractBase
{
	void ActionTakeHybridAttachmentToHands()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HANDS;
		m_Text = "#take_to_hands";
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
		EntityAI tgt_entity = EntityAI.Cast( target.GetObject() );
		
		if ( tgt_entity && BaseBuildingBase.Cast(tgt_entity) &&!tgt_entity.IsBeingPlaced() /*&& BaseBuildingBase.Cast(tgt_entity).IsPlayerInside(player,"")*/ )
		{
			string component_name = tgt_entity.GetActionComponentName( target.GetComponentIndex() );
			
			ItemBase attachment = ItemBase.Cast(tgt_entity.FindAttachmentBySlotName(component_name));
			
			if ( attachment && player.GetInventory().CanAddEntityIntoHands(attachment) && attachment.IsTakeable() )
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
		EntityAI tgt_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		string component_name = tgt_entity.GetActionComponentName( action_data.m_Target.GetComponentIndex() );
		ItemBase attachment;
		
		if ( tgt_entity && ItemBase.CastTo(attachment, tgt_entity.FindAttachmentBySlotName(component_name)) )
		{
			ClearInventoryReservationEx(action_data);
			float stackable = attachment.GetTargetQuantityMax(-1);
			
			if( stackable == 0 || stackable >= attachment.GetQuantity() )
			{
				//take to hands
				action_data.m_Player.PredictiveTakeEntityToHands( attachment );
			}
			else if( stackable != 0 && stackable < attachment.GetQuantity() )
			{
				//split and take to hands
				attachment.SplitIntoStackMaxHandsClient( action_data.m_Player );
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
		EntityAI tgt_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		string component_name = tgt_entity.GetActionComponentName( action_data.m_Target.GetComponentIndex() );
		ItemBase attachment;
		bool heavy_item = false;
		
		if ( tgt_entity && ItemBase.CastTo(attachment, tgt_entity.FindAttachmentBySlotName(component_name)) )
		{
			ActionBaseCB callback;
			if ( attachment && attachment.ConfigIsExisting("heavyItem") && attachment.ConfigGetBool("heavyItem") )
			{
				heavy_item = true;
			}
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