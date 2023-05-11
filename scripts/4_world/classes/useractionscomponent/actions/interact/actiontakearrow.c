/*class TakeArrowActionReciveData : ActionReciveData
{
	EntityAI m_Arrow;
}

class TakeArrowActionData : ActionData
{
	EntityAI m_Arrow;
}*/

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
	
	/*override ActionData CreateActionData()
	{
		ActionData action_data = new TakeArrowActionData;
		return action_data;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		TakeArrowActionData ad = TakeArrowActionData.Cast(action_data);
		ctx.Write(ad.m_Arrow);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if (!action_recive_data)
		{
			action_recive_data = new TakeArrowActionReciveData;
		}
		
		if (!super.ReadFromContext( ctx, action_recive_data ))
			return false;
		
		TakeArrowActionReciveData recive_data_ta = TakeArrowActionReciveData.Cast( action_recive_data );

		if (!ctx.Read(recive_data_ta.m_Arrow))
			return false;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		TakeArrowActionReciveData recive_data_ta = TakeArrowActionReciveData.Cast(action_recive_data);
		TakeArrowActionData action_data_ta = TakeArrowActionData.Cast(action_data);
		
		action_data_ta.m_Arrow = recive_data_ta.m_Arrow;
	}*/
	
	override void OnExecuteClient(ActionData action_data)
	{

		InventoryLocation il = action_data.m_ReservedInventoryLocations.Get(0);
		
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
	
	/*override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = null)
	{	
		if (super.SetupAction(player, target, item, action_data, extra_data))
		{
			#ifndef SERVER
			TakeArrowActionData action_data_ta = TakeArrowActionData.Cast(action_data);
			InventoryLocation il = action_data_ta.m_ReservedInventoryLocations.Get(0);
			action_data_ta.m_Arrow = il.GetItem();
			#endif

			return true;
		}

		return false;
	}*/
	
	/*override void OnStartServer(ActionData action_data)
	{
		TakeArrowActionData action_data_ta = TakeArrowActionData.Cast(action_data);
		action_data.m_Target.GetObject().RemoveChild(action_data_ta.m_Arrow);
	}*/
	
	override void OnExecuteServer(ActionData action_data)
	{
		//TakeArrowActionData action_data_ta = TakeArrowActionData.Cast(action_data);
		//action_data.m_Target.GetObject().RemoveChild(action_data_ta.m_Arrow);
		
		if (GetGame().IsMultiplayer())
			return;

		InventoryLocation il = action_data.m_ReservedInventoryLocations.Get(0);
		ItemBase arrow = ItemBase.Cast(il.GetItem());
		InventoryLocation arrowInventoryLocation = new InventoryLocation;
		arrow.GetInventory().GetCurrentInventoryLocation(arrowInventoryLocation);
		
		EntityAI targetEntity = EntityAI.Cast(action_data.m_Target.GetObject());
		if (targetEntity)
		{
			targetEntity.RemoveChild(arrow);
		}
		
		//float ammoMax = arrow.GetAmmoMax();
		
		//if ( stackable == 0 || ammoMax >= arrow.GetAmmoCount() )
		//{
			action_data.m_Player.PredictiveTakeToDst(arrowInventoryLocation, il);
		/*}
		else
		{
			arrow.SplitIntoStackMaxToInventoryLocationClient( il );
		}*/
	}
}