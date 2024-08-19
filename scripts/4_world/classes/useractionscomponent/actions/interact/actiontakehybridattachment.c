
class ActionTakeHybridAttachment: ActionInteractBase
{
	string m_ItemName = "";

	void ActionTakeHybridAttachment()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_INVENTORY;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_PICKUP_INVENTORY;
		m_Text = "#take";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor;
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
		EntityAI tgt_entity = EntityAI.Cast( target.GetObject() );
		
		if ( tgt_entity && BaseBuildingBase.Cast(tgt_entity) &&!tgt_entity.IsBeingPlaced() )
		{
			string component_name = tgt_entity.GetActionComponentName( target.GetComponentIndex() );
			
			/*if (!tgt_entity.GetInventory().HasInventorySlot(InventorySlots.GetSlotIdFromString(component_name)))
				return false;*/
			
			ItemBase attachment = ItemBase.Cast(tgt_entity.FindAttachmentBySlotName(component_name));
			
			if ( attachment && player.GetInventory().CanAddEntityIntoInventory(attachment) && attachment.IsTakeable() )
			{
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
		
		EntityAI tgt_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		string component_name = tgt_entity.GetActionComponentName( action_data.m_Target.GetComponentIndex() );
		ItemBase attachment;
		
		if ( tgt_entity && ItemBase.CastTo(attachment, tgt_entity.FindAttachmentBySlotName(component_name)) )
		{
			action_data.m_Player.GetInventory().FindFreeLocationFor( attachment , FindInventoryLocationType.ANY, il );
			if ( action_data.m_Player.GetInventory().HasInventoryReservation( attachment, il) )
			{
				success = false;
			}
			else
			{
				action_data.m_Player.GetInventory().AddInventoryReservationEx( attachment, il, GameInventory.c_InventoryReservationTimeoutMS);
			}
		}	
		
		if ( success )
		{
			if( il )
				action_data.m_ReservedInventoryLocations.Insert(il);
		}
		
		return success;
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		if (GetGame().IsMultiplayer())
			return;
		
		//Debug.Log("[Action DEBUG] Start time stamp: " + action_data.m_Player.GetSimulationTimeStamp());
		EntityAI tgt_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		string component_name = tgt_entity.GetActionComponentName( action_data.m_Target.GetComponentIndex() );
		ItemBase attachment;
		
		if ( tgt_entity && ItemBase.CastTo(attachment, tgt_entity.FindAttachmentBySlotName(component_name)) )
		{
			InventoryLocation il = action_data.m_ReservedInventoryLocations.Get(0);	
			InventoryLocation targetInventoryLocation = new InventoryLocation;
			attachment.GetInventory().GetCurrentInventoryLocation(targetInventoryLocation);
			ClearInventoryReservationEx(action_data);
			//SplitItemUtils.TakeOrSplitToInventoryLocation( action_data.m_Player, il );
			
			float stackable = attachment.GetTargetQuantityMax(il.GetSlot());
			
			if( stackable == 0 || stackable >= attachment.GetQuantity() )
			{
				action_data.m_Player.PredictiveTakeToDst(targetInventoryLocation, il);
			}
			else
			{
				attachment.SplitIntoStackMaxToInventoryLocationClient( il );
			}
		}
		//action_data.m_Player.PredictiveTakeToDst(targetInventoryLocation, il);
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		//Debug.Log("[Action DEBUG] Start time stamp: " + action_data.m_Player.GetSimulationTimeStamp());
		EntityAI tgt_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		string component_name = tgt_entity.GetActionComponentName( action_data.m_Target.GetComponentIndex() );
		ItemBase attachment;
		
		if ( tgt_entity && ItemBase.CastTo(attachment, tgt_entity.FindAttachmentBySlotName(component_name)) )
		{
			InventoryLocation il = action_data.m_ReservedInventoryLocations.Get(0);
			InventoryLocation targetInventoryLocation = new InventoryLocation;
			attachment.GetInventory().GetCurrentInventoryLocation(targetInventoryLocation);
			ClearInventoryReservationEx(action_data);
			//SplitItemUtils.TakeOrSplitToInventoryLocation( action_data.m_Player, il );
			//action_data.m_Player.PredictiveTakeToDst(targetInventoryLocation, il);
			float stackable = attachment.GetTargetQuantityMax(il.GetSlot());
			
			if( stackable == 0 || stackable >= attachment.GetQuantity() )
			{
				action_data.m_Player.PredictiveTakeToDst(targetInventoryLocation, il);
			}
			else
			{
				attachment.SplitIntoStackMaxToInventoryLocationClient( il );
			}
		}
	}
};