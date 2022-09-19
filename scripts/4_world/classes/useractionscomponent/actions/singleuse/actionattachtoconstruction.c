class ActionAttachToConstruction: ActionSingleUseBase
{
	void ActionAttachToConstruction()
	{
		m_Text = "#attach";
	}

	override void CreateConditionComponents() 
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursor;
	}
	
	override ActionData CreateActionData()
	{
		AttachActionData action_data = new AttachActionData;
		return action_data;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		EntityAI target_entity = EntityAI.Cast( target.GetObject() );
		
		if ( target_entity && target_entity.CanUseConstruction() )
		{
			string selection = target_entity.GetActionComponentName( target.GetComponentIndex() );
			ConstructionActionData construction_action_data = player.GetConstructionActionData();
			int slot_id  = construction_action_data.GetAttachmentSlotFromSelection( player, target_entity, item, selection );
			BaseBuildingBase base_building;
			
			if ( slot_id != -1 )
			{
				base_building = BaseBuildingBase.Cast( target_entity );
				if ( base_building.CheckSlotVerticalDistance( slot_id, player ) && base_building.IsPlayerInside(player,"") )
				{
					construction_action_data.SetSlotId( slot_id );
					
					return true;
				}
			}
			else if ( item.IsKindOf("CombinationLock") )
			{
				base_building = BaseBuildingBase.Cast( target_entity );
				
				//simpler hack
				/*construction_action_data.SetSlotId( InventorySlots.GetSlotIdFromString("Att_CombinationLock") );
				return true;*/
				
				InventoryLocation loc = new InventoryLocation;
				bool found = base_building.GetInventory().FindFreeLocationFor(item,FindInventoryLocationType.ATTACHMENT,loc);
				
				if (found)
				{
					//Print("slot name: " + InventorySlots.GetSlotName(loc.GetSlot()) );
					construction_action_data.SetSlotId( loc.GetSlot() );
					
					return true;
				}
			}
		}
		
		return false;
	}
	
	override void OnStartClient( ActionData action_data )
	{
		//set action initiator
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		construction_action_data.SetActionInitiator( action_data.m_Player );
	}
	
	protected void OnExecuteImpl( ActionData action_data )
	{
		EntityAI target_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		ItemBase item = ItemBase.Cast( action_data.m_MainItem );
		
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		int slot_id = construction_action_data.GetSlotId();
		
		if ( slot_id != -1 )
		{
			ItemBase attachment = ItemBase.Cast( target_entity.GetInventory().FindAttachment( slot_id ) );
			
			if ( attachment )
			{
				//combine
				attachment.CombineItemsClient( item );
			}
			else// if( target_entity.GetInventory().CanAddAttachmentEx( item, slot_id ) )
			{
				ItemBase item_base	= ItemBase.Cast( item );
				float stackable	= item_base.GetTargetQuantityMax( slot_id );
				
				if( stackable == 0 || stackable >= item_base.GetQuantity() )
				{
					action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx(target_entity, item, slot_id);
				}
				else if( stackable != 0 && stackable < item_base.GetQuantity() )
				{
					item_base.SplitIntoStackMaxClient( target_entity, slot_id );
				}
			}
		}
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
		OnExecuteImpl(action_data);
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		if ( !GetGame().IsMultiplayer() )
		{
			ClearInventoryReservationEx(action_data);
			
			OnExecuteImpl(action_data); // single player
		}
	}
}