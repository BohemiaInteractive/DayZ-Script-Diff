class ActionTakeArrow: ActionTakeItem
{
	void ActionTakeArrow()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_Text 				= "#take";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTObject(UAMaxDistances.DEFAULT);
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
			if (arrow)
			{
				if (player.GetInventory().CanAddEntityIntoInventory(arrow))
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
	
	override bool CanBeUsedOnBack()
	{
		return false;
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
					action_data.m_Player.GetInventory().FindFreeLocationFor(targetArrow , FindInventoryLocationType.ANY, il);
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
		
		ClearInventoryReservationEx(action_data);
		
		ItemBase arrow = ItemBase.Cast(il.GetItem());
		InventoryLocation arrowInventoryLocation = new InventoryLocation;
		arrow.GetInventory().GetCurrentInventoryLocation(arrowInventoryLocation);
		
		float stackable = arrow.GetTargetQuantityMax(il.GetSlot());
		
		if ( stackable == 0 || stackable >= arrow.GetQuantity() )
		{
			action_data.m_Player.PredictiveTakeToDst(arrowInventoryLocation, il);
		}
		else
		{
			arrow.SplitIntoStackMaxToInventoryLocationClient( il );
		}
	}
}