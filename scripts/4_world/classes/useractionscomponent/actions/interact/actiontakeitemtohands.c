class ActionTakeItemToHands: ActionInteractBase
{
	void ActionTakeItemToHands()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HANDS;
		m_Text 				= "#take_to_hands";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTObject(UAMaxDistances.DEFAULT);
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

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ItemBase targetItem = ItemBase.Cast(target.GetObject());
		if (!targetItem)
			return false;
		
		EntityAI targetParent = EntityAI.Cast(targetItem.GetHierarchyParent());
		if (!targetItem.IsTakeable() || targetItem.IsBeingPlaced() || targetItem.IsSetForDeletion() || ( targetItem.GetHierarchyParent() && !BaseBuildingBase.Cast(targetParent) ) || ((targetParent && !targetItem.CanDetachAttachment(targetParent)) || (targetParent && !targetParent.CanReleaseAttachment(targetItem))))
			return false;
		
		return player.GetInventory().CanAddEntityIntoHands(targetItem);
	}
	
	override bool CanContinue(ActionData action_data)
	{
		return true;
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		EntityAI takenEntity = EntityAI.Cast(action_data.m_Target.GetObject());
		if (takenEntity && takenEntity.GetCompEM() && takenEntity.IsElectricAppliance())
			takenEntity.GetCompEM().UnplugThis();
	}
	
	override void OnExecute(ActionData action_data)
	{
		if (g_Game.IsDedicatedServer())
		{
			ClearActionJuncture(action_data);
			return;
		}
			
		ItemBase ntarget = ItemBase.Cast(action_data.m_Target.GetObject());
		ClearInventoryReservationEx(action_data);
		
		float stackable = ntarget.GetTargetQuantityMax(-1);
		
		if (stackable == 0 || stackable >= ntarget.GetQuantity())
		{
			action_data.m_Player.PredictiveTakeEntityToHands(ntarget);
		}
		else
		{
			InventoryLocation il = new InventoryLocation();
			il.SetHands(action_data.m_Player, ntarget);
			ntarget.SplitIntoStackMaxToInventoryLocationClient(il);
		}
	}
	
	override void CreateAndSetupActionCallback(ActionData action_data)
	{
		EntityAI target = EntityAI.Cast(action_data.m_Target.GetObject());
		if (!target)
			return;
		
		ActionBaseCB callback;
		
		if (target.IsHeavyBehaviour())
		{
			Class.CastTo(callback, action_data.m_Player.StartCommand_Action(DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HEAVY,GetCallbackClassTypename(), DayZPlayerConstants.STANCEMASK_ERECT));
		}
		else
		{
			if (action_data.m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT))
			{
				Class.CastTo(callback, action_data.m_Player.AddCommandModifier_Action(m_CommandUID,GetCallbackClassTypename()));
			}
			else
			{
				Class.CastTo(callback, action_data.m_Player.StartCommand_Action(m_CommandUIDProne,GetCallbackClassTypename(),DayZPlayerConstants.STANCEMASK_PRONE));
			}
		}

		callback.SetActionData(action_data); 
		callback.InitActionComponent();
		action_data.m_Callback = callback;
	}
	
		
	override bool CanBeUsedOnBack()
	{
		return true;
	}
}

class ActionSwapItemToHands: ActionTakeItemToHands
{
	bool m_Executable;
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ItemBase targetItem = ItemBase.Cast(target.GetObject());
		if (!targetItem || !targetItem.IsTakeable() || targetItem.IsBeingPlaced())
			return false;
		
		return player.GetInventory().CanSwapEntitiesEx(targetItem,item);
	}
	
	override bool UseMainItem()
	{
		return true;
	}
	
	override bool MainItemAlwaysInHands()
	{
		return true;
	}
	
	override void CreateAndSetupActionCallback(ActionData action_data)
	{
		EntityAI target = EntityAI.Cast(action_data.m_Target.GetObject());
		if (!target)
			return;
		
		ActionBaseCB callback;
		
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
	
	override void Start(ActionData action_data)
	{
		super.Start(action_data);
		
		bool b1 = action_data.m_MainItem.ConfigGetString("physLayer") == "item_large";
		action_data.m_MainItem.m_ThrowItemOnDrop = b1;
		
		EntityAI object = EntityAI.Cast(action_data.m_Target.GetObject());
		if (!object || !object.IsHeavyBehaviour())
		{
			action_data.m_Player.GetActionManager().InterruptNoSync();
		}
		else
			m_Executable = true;
	}
	
	override void OnExecute(ActionData action_data)
	{
		if (!m_Executable)
			return;
		
		if (g_Game.IsDedicatedServer())
		{
			ClearActionJuncture(action_data);
			return;
		}
		
		PerformSwap(action_data);
	}
	
	override void OnEnd(ActionData action_data)
	{
		super.OnEnd(action_data);
	
		if (m_Executable)
		{
			m_Executable = false;
			return;
		}
		
		if (g_Game.IsDedicatedServer())
		{
			ClearActionJuncture(action_data);
			return;
		}
		
		PerformSwap(action_data);
	}
		
	void PerformSwap( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
		
		EntityAI ntarget = EntityAI.Cast(action_data.m_Target.GetObject());
		if (action_data.m_Player)
		{
			InventoryMode invMode = InventoryMode.PREDICTIVE;
			if (!g_Game.IsMultiplayer())
				invMode = InventoryMode.LOCAL;
			else if (action_data.m_Player.NeedInventoryJunctureFromServer(ntarget, ntarget.GetHierarchyParent(), action_data.m_Player))
				invMode = InventoryMode.JUNCTURE;
			
			action_data.m_Player.TakeEntityToHandsImpl(invMode, ntarget);
		}
	}
}