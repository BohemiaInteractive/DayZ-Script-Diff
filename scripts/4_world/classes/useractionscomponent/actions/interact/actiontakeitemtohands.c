class ActionTakeItemToHands: ActionInteractBase
{
	void ActionTakeItemToHands()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HANDS;
		m_Text = "#take_to_hands";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
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
		ItemBase tgt_item = ItemBase.Cast( target.GetObject() );
		if ( !tgt_item )
			return false;
		EntityAI tgt_parent = EntityAI.Cast(tgt_item.GetHierarchyParent());
		if ( !tgt_item.IsTakeable() || tgt_item.IsBeingPlaced() || ( tgt_item.GetHierarchyParent() && !BaseBuildingBase.Cast(tgt_parent) ) || ((tgt_parent && !tgt_item.CanDetachAttachment(tgt_parent)) || (tgt_parent && !tgt_parent.CanReleaseAttachment(tgt_item))) )
			return false;
		
		return player.GetInventory().CanAddEntityIntoHands(tgt_item);
	}
	
	override bool CanContinue( ActionData action_data )
	{
		return true;
	}
	
	override void OnExecute( ActionData action_data )
	{
		if ( GetGame().IsMultiplayer() && GetGame().IsServer() )
			return;
	
		ItemBase ntarget = ItemBase.Cast(action_data.m_Target.GetObject());
		ClearInventoryReservationEx(action_data);
		
		float stackable = ntarget.GetTargetQuantityMax(-1);
		
		if ( stackable == 0 || stackable >= ntarget.GetQuantity() )
		{
			action_data.m_Player.PredictiveTakeEntityToHands( ntarget );
		}
		else
		{
			InventoryLocation il = new InventoryLocation;
			il.SetHands(action_data.m_Player, ntarget);
			ntarget.SplitIntoStackMaxToInventoryLocationClient( il );
		}
	}
	
	override void CreateAndSetupActionCallback( ActionData action_data )
	{
		//Print("starting - CreateAndSetupActionCallback");
		EntityAI target = EntityAI.Cast(action_data.m_Target.GetObject());
		ActionBaseCB callback;
		if (!target)
			return;
		
		if (target.IsHeavyBehaviour())
		{
			//Print("heavybehaviour");
			Class.CastTo(callback, action_data.m_Player.StartCommand_Action(DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HEAVY,GetCallbackClassTypename(),DayZPlayerConstants.STANCEMASK_ERECT));
		}
		else
		{
			//Print("else - SHOULD NOT BE HERE");
			if( action_data.m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT) )
			{
				Class.CastTo(callback, action_data.m_Player.AddCommandModifier_Action(DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS,GetCallbackClassTypename()));
			}
			else
			{
				Class.CastTo(callback, action_data.m_Player.StartCommand_Action(DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HANDS,GetCallbackClassTypename(),DayZPlayerConstants.STANCEMASK_PRONE));
			}
		}
		//Print(callback);
		callback.SetActionData(action_data); 
		callback.InitActionComponent();
		action_data.m_Callback = callback;
	}
	
		
	override bool CanBeUsedOnBack()
	{
		return true;
	}
};

class ActionSwapItemToHands: ActionTakeItemToHands
{
	bool m_Executable;
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase tgt_item = ItemBase.Cast( target.GetObject() );
		if ( !tgt_item || !tgt_item.IsTakeable() || tgt_item.IsBeingPlaced() )
			return false;
		
		return player.GetInventory().CanSwapEntitiesEx(tgt_item,item);
	}
	
	override bool UseMainItem()
	{
		return true;
	}
	
	override bool MainItemAlwaysInHands()
	{
		return true;
	}
	
	override void CreateAndSetupActionCallback( ActionData action_data )
	{
		EntityAI target = EntityAI.Cast(action_data.m_Target.GetObject());
		ActionBaseCB callback;
		if (!target)
			return;
		
		if (target.IsHeavyBehaviour())
		{
			Class.CastTo(callback, action_data.m_Player.StartCommand_Action(DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HEAVY,GetCallbackClassTypename(),DayZPlayerConstants.STANCEMASK_ERECT));
		}
		else
		{
			return;
		}
		callback.SetActionData(action_data); 
		callback.InitActionComponent();
		action_data.m_Callback = callback;
	}
	
	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		//Print("action_data.m_Target.GetObject() + " + action_data.m_Target.GetObject());
		//Print("action_data.m_MainItem + " + action_data.m_MainItem);
		
		bool b1 = action_data.m_MainItem.ConfigGetString("physLayer") == "item_large";
		action_data.m_MainItem.m_ThrowItemOnDrop = b1; //hack, should be redundant anyway
		
		EntityAI object = EntityAI.Cast(action_data.m_Target.GetObject());
		if (!object || !object.IsHeavyBehaviour())
		{
			action_data.m_Player.GetActionManager().Interrupt();
		}
		else
			m_Executable = true;
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		if (!m_Executable)
			return;
		
		if (GetGame().IsMultiplayer())
			return;
		
		PerformSwap(action_data);
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		if (!m_Executable)
			return;
		
		PerformSwap(action_data);
	}
	
	override void OnEndServer( ActionData action_data )
	{
		if (m_Executable)
		{
			m_Executable = false;
			return;
		}
		
		if (GetGame().IsMultiplayer())
			return;
		
		PerformSwap(action_data);
	}
	
	override void OnEndClient( ActionData action_data )
	{
		if (m_Executable)
		{
			m_Executable = false;
			return;
		}
		
		PerformSwap(action_data);
	}
	
	void PerformSwap( ActionData action_data )
	{
		EntityAI ntarget = EntityAI.Cast(action_data.m_Target.GetObject());
		if (action_data.m_Player)
		{
			InventoryMode invMode = InventoryMode.PREDICTIVE;
			if (action_data.m_Player.NeedInventoryJunctureFromServer(ntarget, ntarget.GetHierarchyParent(), action_data.m_Player))
				invMode = InventoryMode.JUNCTURE;
			
			action_data.m_Player.TakeEntityToHandsImpl(invMode, ntarget);
		}
	}
}