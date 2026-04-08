class ActionAttachOnSelection: ActionSingleUseBase
{
	void ActionAttachOnSelection()
	{
		m_Text = "#attach";
	}
	
	override void CreateConditionComponents() 
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTCursor();
	}
	
	override ActionData CreateActionData()
	{
		AttachActionData action_data = new AttachActionData();
		return action_data;
	}
	
	int FindSlotIdToAttachOrCombine(PlayerBase player, ActionTarget target, ItemBase item)
	{
		EntityAI targetEntity = EntityAI.Cast(target.GetObject());
		if (targetEntity && item)
		{				
			GameInventory inv = targetEntity.GetInventory();
			if (!inv)
				return InventorySlots.INVALID;

			int slotsCount = item.GetInventory().GetSlotIdCount();			
			array<string> selections = new array<string>();
			targetEntity.GetActionComponentNameList(target.GetComponentIndex(), selections);

			foreach (string selection : selections)
			{
				int slotId = -1;
				if (!targetEntity.TranslateSlotFromSelection(selection, slotId))
					slotId = InventorySlots.GetSlotIdFromString(selection);

				if (slotId == -1)
					continue;

				for (int i=0; i < slotsCount; ++i)
				{
					int itemSlotId = item.GetInventory().GetSlotId(i);
					if (slotId == itemSlotId)
					{
						ItemBase currentAttachment = ItemBase.Cast(inv.FindAttachment(slotId));
						if (currentAttachment)
						{
							if (currentAttachment.CanBeCombined(item))
								return itemSlotId;
						}
						else
						{
							if (inv.CanAddAttachment(item))
								return itemSlotId;
						}	
					}
				}
			}
		}

		return InventorySlots.INVALID;
	}
	
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = null)
	{
		int attSlotId = InventorySlots.INVALID;
		if (!g_Game.IsDedicatedServer() )
		{
			attSlotId = FindSlotIdToAttachOrCombine(player, target, item);
		}
		
		if (super.SetupAction( player, target, item, action_data, extra_data))
		{
			if (!g_Game.IsDedicatedServer())
			{
				if (attSlotId != InventorySlots.INVALID)
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

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (g_Game.IsMultiplayer() && g_Game.IsServer())
			return true;
		
		if (target.GetObject() && target.GetObject().CanUseConstruction())
			return false;
		
		return FindSlotIdToAttachOrCombine(player, target, item) != InventorySlots.INVALID;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		if (g_Game.IsMultiplayer())
			return;
		
		ClearInventoryReservationEx(action_data);
		
		AttachActionData action_data_a = AttachActionData.Cast(action_data);
		EntityAI entity;

		if (action_data.m_Target.IsProxy())
		{
			entity = EntityAI.Cast(action_data.m_Target.GetParent());
		}
		else
		{
			entity = EntityAI.Cast(action_data.m_Target.GetObject());
		}
		
		if (entity && action_data.m_MainItem)
		{
			action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx(entity, action_data_a.m_MainItem, action_data_a.m_AttSlot);
		}
	}
	
	override void OnExecuteClient(ActionData action_data)
	{
		ClearInventoryReservationEx(action_data);
		EntityAI targetEntity 	= EntityAI.Cast(action_data.m_Target.GetObject());
		EntityAI itemEntity 	= action_data.m_MainItem;
		
		AttachActionData action_data_a = AttachActionData.Cast(action_data);
		
		ItemBase attachment = ItemBase.Cast(targetEntity.GetInventory().FindAttachment(action_data_a.m_AttSlot));
		if (attachment)
		{
			attachment.CombineItemsClient(itemEntity);
		}
		else
		{
			ItemBase item_base	= ItemBase.Cast( itemEntity );
			float stackable	= item_base.GetTargetQuantityMax( action_data_a.m_AttSlot );
				
			if (stackable == 0 || stackable >= item_base.GetQuantity())
			{
				action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx(targetEntity, itemEntity, action_data_a.m_AttSlot);
			}
			else if (stackable != 0 && stackable < item_base.GetQuantity())
			{
				item_base.SplitIntoStackMaxClient(targetEntity, action_data_a.m_AttSlot);
			}
		}
	}
}