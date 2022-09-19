class ActionAttachOnSelection: ActionSingleUseBase
{
	void ActionAttachOnSelection()
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
	
	int FindSlotIdToAttachOrCombine(PlayerBase player, ActionTarget target, ItemBase item)
	{
		EntityAI target_entity = EntityAI.Cast( target.GetObject() );
		
		if ( target_entity && item )
		{
			if ( !target_entity.GetInventory() ) return InventorySlots.INVALID;
			
			array<string> selections = new array<string>;
			target_entity.GetActionComponentNameList(target.GetComponentIndex(), selections);

			for (int s = 0; s < selections.Count(); s++)
			{
				//Print(selections[s]);
				int carId = -1;
				if ( !target_entity.TranslateSlotFromSelection(selections[s],carId) )
					carId = InventorySlots.GetSlotIdFromString( selections[s] );
				
				if ( carId == -1 )
					continue;
				
				int slotsCnt = item.GetInventory().GetSlotIdCount();

				for (int i=0; i < slotsCnt; i++ )
				{
					int itmSlotId = item.GetInventory().GetSlotId(i);
					
					if ( carId == itmSlotId )
					{
						ItemBase currentAttachment = ItemBase.Cast(target_entity.GetInventory().FindAttachment( carId ));
						if (currentAttachment)
						{
							if(currentAttachment.CanBeCombined( item ) )
							{
								return itmSlotId;
							}
						}
						else
						{
							if ( target_entity.GetInventory() && target_entity.GetInventory().CanAddAttachment( item ) )
							{
								return itmSlotId;
							}
						}	
					}
				}
			}
		}	
		return InventorySlots.INVALID;
	}
	
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		int attSlotId = InventorySlots.INVALID;
		if (!GetGame().IsDedicatedServer() )
		{
			attSlotId = FindSlotIdToAttachOrCombine(player, target, item);
		}
		
		if ( super.SetupAction( player, target, item, action_data, extra_data))
		{
			if (!GetGame().IsDedicatedServer())
			{
				if(attSlotId != InventorySlots.INVALID)
				{
					AttachActionData action_data_a = AttachActionData.Cast(action_data);
					action_data_a.m_AttSlot = attSlotId;
					return true;
				}
				return false;
			}
			return true;
		}
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (GetGame().IsMultiplayer() && GetGame().IsServer() ) return true;
		
		//if (BaseBuildingBase.Cast(target.GetObject())) return false;
		if (target.GetObject() && target.GetObject().CanUseConstruction()) return false;
		
		return FindSlotIdToAttachOrCombine(player, target, item) != InventorySlots.INVALID;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		if (GetGame().IsMultiplayer())
			return;
		
		EntityAI target_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		EntityAI item_entity = action_data.m_MainItem;
		
		AttachActionData action_data_a = AttachActionData.Cast(action_data);
		
		action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx(target_entity, item_entity, action_data_a.m_AttSlot );
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
		EntityAI target_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		EntityAI item_entity = action_data.m_MainItem;
		
		AttachActionData action_data_a = AttachActionData.Cast(action_data);
		
		ItemBase attachment = ItemBase.Cast( target_entity.GetInventory().FindAttachment( action_data_a.m_AttSlot ) );
			
		
		if ( attachment )
		{
			attachment.CombineItemsClient( item_entity );
		}
		else
		{
			ItemBase item_base	= ItemBase.Cast( item_entity );
			float stackable	= item_base.GetTargetQuantityMax( action_data_a.m_AttSlot );
				
			if( stackable == 0 || stackable >= item_base.GetQuantity() )
			{
				action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx(target_entity, item_entity, action_data_a.m_AttSlot);
			}
			else if( stackable != 0 && stackable < item_base.GetQuantity() )
			{
				item_base.SplitIntoStackMaxClient( target_entity, action_data_a.m_AttSlot );
			}
		}
	}
}