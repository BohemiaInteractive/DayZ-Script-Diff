class ActionTakeArrowToHands: ActionInteractBase
{
	void ActionTakeArrowToHands()
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
		EntityAI targetEntity = EntityAI.Cast(target.GetObject());
		if (!targetEntity || (targetEntity && targetEntity.IsManagingArrows() && !targetEntity.IsDamageDestroyed()))
			return false;
		
		ArrowManagerBase arrowManager = targetEntity.GetArrowManager();
		if (arrowManager)
		{
			ItemBase arrow = ItemBase.Cast(arrowManager.GetFirstArrow());
			if (arrow && !arrow.IsSetForDeletion())
			{
				if (player.GetInventory().CanAddEntityIntoHands(arrow))
					return true;
			}
		}
		
		return false;
	}
	
	override Object GetDisplayInteractObject(PlayerBase player, ActionTarget target)
	{
		EntityAI targetEntity = EntityAI.Cast(target.GetObject());
		if (!targetEntity || (targetEntity && targetEntity.IsManagingArrows() && !targetEntity.IsDamageDestroyed()))
			return null;
		
		ArrowManagerBase arrowManager = targetEntity.GetArrowManager();
		if (arrowManager)
		{
			return arrowManager.GetFirstArrow();
		}
		
		return null;
	}
	
	override bool CanContinue(ActionData action_data)
	{
		return true;
	}
	
	override bool InventoryReservation(ActionData action_data)
	{
		bool success = true;
		
		InventoryLocation il = new InventoryLocation();
		
		EntityAI targetEntity;
		if (EntityAI.CastTo(targetEntity, action_data.m_Target.GetObject()))
		{
			ArrowManagerBase arrowManager = targetEntity.GetArrowManager();
			if (arrowManager)
			{
				ItemBase targetArrow = ItemBase.Cast(arrowManager.GetFirstArrow());
				if (targetArrow)
				{
					il.SetHands(action_data.m_Player,targetArrow);
					if (action_data.m_Player.GetInventory().HasInventoryReservation(targetArrow, il))
					{
						success = false;
					}
					else
					{
						action_data.m_Player.GetInventory().AddInventoryReservationEx(targetArrow, il, GameInventory.c_InventoryReservationTimeoutMS);
					}
				}
			}
		}	
		
		if (success)
		{
			if (il)
				action_data.m_ReservedInventoryLocations.Insert(il);
		}
		
		return success;
	}
	
	
	override void OnExecute(ActionData action_data)
	{
		if (g_Game.IsDedicatedServer())
		{
			ClearActionJuncture(action_data);
			return;
		}
	
		InventoryLocation il = action_data.m_ReservedInventoryLocations.Get(0);
		
		ItemBase arrow = ItemBase.Cast(il.GetItem());
		ClearInventoryReservationEx(action_data);
		
		float stackable = arrow.GetTargetQuantityMax(-1);
		
		if (stackable == 0 || stackable >= arrow.GetQuantity())
		{
			action_data.m_Player.PredictiveTakeEntityToHands(arrow);
		}
		else
		{
			arrow.SplitIntoStackMaxToInventoryLocationClient(il);
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