/*class ActionTakeItemReciveData : ActionReciveData
{
	ref InventoryLocation		m_InventoryLocation;
}*/

class ActionTakeItem: ActionInteractBase
{
	string m_ItemName = "";

	void ActionTakeItem()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_INVENTORY;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_PICKUP_INVENTORY;
		m_Text = "#take";
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

	override bool ActionConditionContinue( ActionData action_data )
	{
		return true;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase tgtItem = ItemBase.Cast(target.GetObject());
		if (tgtItem && !tgtItem.IsTakeable())
			return false;
		if (tgtItem && tgtItem.IsBeingPlaced())
			return false;
		
		EntityAI tgtParent = EntityAI.Cast(target.GetParent());
		EntityAI tgtEntity = EntityAI.Cast(target.GetObject());
		
		if (tgtEntity && (!tgtParent || tgtParent.CanUseConstruction()))
		{
			if (tgtEntity && tgtEntity.IsItemBase() && player.GetInventory().CanAddEntityIntoInventory(tgtEntity) && tgtEntity.GetHierarchyRootPlayer() != player)
			{
				if (tgtParent && (!tgtItem.CanDetachAttachment(tgtParent) || !tgtParent.CanReleaseAttachment(tgtItem)))
					return false;
				
				//! Special handling for digital code lock
				DigitalCodeLock codeLock = DigitalCodeLock.Cast(tgtItem);
				if (codeLock && !codeLock.CanDetachCodeLock(player, target.GetComponentIndex()))
					return false;
				
				return true;
			}
		}
		return false;
	}
	
	override bool CanBeUsedOnBack()
	{
		return true;
	}
	
	override bool InventoryReservation(ActionData action_data)
	{
		bool success = true;
		
		InventoryLocation il = new InventoryLocation;
		
		ItemBase targetItem;
		if ( ItemBase.CastTo(targetItem, action_data.m_Target.GetObject()) )
		{
			action_data.m_Player.GetInventory().FindFreeLocationFor( targetItem , FindInventoryLocationType.ANY, il );
			if ( action_data.m_Player.GetInventory().HasInventoryReservation( targetItem, il) )
			{
				success = false;
			}
			else
			{
				action_data.m_Player.GetInventory().AddInventoryReservationEx( targetItem, il, GameInventory.c_InventoryReservationTimeoutMS);
			}
		}	
		
		if ( success )
		{
			if( il )
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
		
		//Debug.Log("[Action DEBUG] Start time stamp: " + action_data.m_Player.GetSimulationTimeStamp());
		ItemBase ntarget = ItemBase.Cast(action_data.m_Target.GetObject());	
		InventoryLocation il = action_data.m_ReservedInventoryLocations.Get(0);	
		InventoryLocation targetInventoryLocation = new InventoryLocation;
		ntarget.GetInventory().GetCurrentInventoryLocation(targetInventoryLocation);
		
		ClearInventoryReservationEx(action_data);
		
		//SplitItemUtils.TakeOrSplitToInventoryLocation( action_data.m_Player, il );
		
		float stackable = ntarget.GetTargetQuantityMax(il.GetSlot());
		
		if( stackable == 0 || stackable >= ntarget.GetQuantity() )
		{
			action_data.m_Player.PredictiveTakeToDst(targetInventoryLocation, il);
		}
		else
		{
			ClearInventoryReservationEx(action_data);
			ntarget.SplitIntoStackMaxToInventoryLocationClient( il );
		}
		
		//action_data.m_Player.PredictiveTakeToDst(targetInventoryLocation, il);
	}
};