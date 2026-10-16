class AttachToConstructionActionData : ActionData
{
	int m_SlotID;
};

class ActionAttachToConstruction: ActionSingleUseBase
{
	// Used specifically because I didn't want to create an array on every run of the condition.
	// Is NOT meant to transfer data between methods, doing that in member variables of actions would be incredibly dangerous!
	private ref array<string> m_ComponentNames;
	
	void ActionAttachToConstruction()
	{
		m_Text = "#attach";
		m_ComponentNames = new array<string>();
	}

	override void CreateConditionComponents() 
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursor();
	}
	
	override ActionData CreateActionData()
	{
		AttachToConstructionActionData actionData = new AttachToConstructionActionData();
		return actionData;
	}
	
	override bool Post_SetupAction( ActionData action_data )
	{
		if (!super.Post_SetupAction(action_data))
			return false;
		
		if (!g_Game.IsDedicatedServer())
		{
			int slotID = FindAndTranslateAttachmentSlot(action_data.m_Player, action_data.m_Target, action_data.m_MainItem);
			AttachToConstructionActionData.Cast(action_data).m_SlotID = slotID;
			return slotID != InventorySlots.INVALID;
		}
		
		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		EntityAI targetEntity = EntityAI.Cast(target.GetObject());
		if (targetEntity && targetEntity.CanUseConstruction())
		{
			int slotID = FindAndTranslateAttachmentSlot(player, target, item);
			return slotID != InventorySlots.INVALID;
		}
		
		return false;
	}
	
	override void OnStartClient(ActionData action_data)
	{
		//set action initiator
		ConstructionActionData constructionActionData = action_data.m_Player.GetConstructionActionData();
		constructionActionData.SetActionInitiator(action_data.m_Player);
	}
	
	protected void OnExecuteImpl( ActionData action_data )
	{
		EntityAI targetEntity = EntityAI.Cast(action_data.m_Target.GetObject());
		
		int slotId = AttachToConstructionActionData.Cast(action_data).m_SlotID;
		
		if (slotId != InventorySlots.INVALID)
		{
			ItemBase attachment = ItemBase.Cast(targetEntity.GetInventory().FindAttachment(slotId));
			
			if (attachment)
			{
				//combine
				attachment.CombineItemsClient(action_data.m_MainItem);
			}
			else
			{
				float stackable	= action_data.m_MainItem.GetTargetQuantityMax(slotId);
				
				if (stackable == 0 || stackable >= action_data.m_MainItem.GetQuantity())
				{
					action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx(targetEntity, action_data.m_MainItem, slotId);
				}
				else if (stackable != 0 && stackable < action_data.m_MainItem.GetQuantity())
				{
					action_data.m_MainItem.SplitIntoStackMax(targetEntity, slotId, action_data.m_Player);
				}
			}
		}
	}
	
	override void OnExecuteClient(ActionData action_data)
	{
		ClearInventoryReservationEx(action_data);
		OnExecuteImpl(action_data);
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		if (!g_Game.IsMultiplayer())
		{
			ClearInventoryReservationEx(action_data);
			OnExecuteImpl(action_data); // single player
		}
		else
		{
			ClearActionJuncture(action_data);
		}
	}
	
	//! where appropriate, returns valid attachment slot ID even without proper selection
	protected int FindAndTranslateAttachmentSlot(PlayerBase player, ActionTarget target, ItemBase item)
	{
		int slotId = InventorySlots.INVALID;
		int count;
		Building building;
		BaseBuildingBase baseBuilding;
		ConstructionActionData constructionActionData = player.GetConstructionActionData();
		InventoryLocation loc;
		bool found;
		EntityAI targetEntity = EntityAI.Cast(target.GetObject());
		if (Class.CastTo(baseBuilding, targetEntity)) //BaseBuildingBase entities
		{
			if (item.IsExternalLockType())	// External lock handling of base building objects
			{
				loc = new InventoryLocation();
				found = baseBuilding.GetInventory().FindFreeLocationFor(item, FindInventoryLocationType.ATTACHMENT, loc);
				slotId = loc.GetSlot();
				if (found && slotId != InventorySlots.INVALID)
				{
					return slotId;
				}
			}
			else
			{
				string selection = baseBuilding.GetActionComponentName(target.GetComponentIndex());
				slotId = constructionActionData.GetAttachmentSlotFromSelection(player, baseBuilding, item, selection);
				
				if (slotId != InventorySlots.INVALID)
				{
					if (baseBuilding.CheckSlotVerticalDistance(slotId, player) && baseBuilding.IsPlayerInside(player,""))
					{
						return slotId;
					}
				}
			}
		}
		else if (Class.CastTo(building, targetEntity) && building.CanUseConstruction())	// Building entities
		{
			if (item.IsExternalLockType()) // External lock handling of building objects with doors and lock attachments and internal locks
			{
				int doorIndex = building.GetDoorIndex(target.GetComponentIndex());
				if (doorIndex > -1 && !building.IsDoorLocked(doorIndex) && !building.IsDoorOpen(doorIndex))
				{
					AdditionalDoorInfo doorInfo = building.GetDoorInfo(doorIndex);
					if (!doorInfo || !doorInfo.m_RelatedInventorySlotNames)
						return InventorySlots.INVALID;
					
					foreach (string slotName: doorInfo.m_RelatedInventorySlotNames)
					{
						EntityAI slotEntity = building.FindAttachmentBySlotName(slotName);
						if (slotEntity)
							continue;
						
						slotId = InventorySlots.GetSlotIdFromString(slotName);
						GameInventory inventory = building.GetInventory();
						if (inventory && inventory.CanAddAttachmentEx(item, slotId))
							break;
						
						slotId = InventorySlots.INVALID; //invalidate after unsuccessful check
					}
					
					if (slotId != InventorySlots.INVALID)
					{
						return slotId;
					}
				}
			}
			else //presumed material, that is only allowed on 'materials slot' selection. Using GUIInventoryAttachmentsProps structure
			{
				m_ComponentNames.Clear();
				building.GetActionComponentNameList(target.GetComponentIndex(), m_ComponentNames, LOD.NAME_VIEW);
				count = m_ComponentNames.Count();
				if (count > 0)
				{
					foreach (string sel : m_ComponentNames)
					{
						slotId = constructionActionData.GetAttachmentSlotFromSelection(player, building, item, sel);
						if (slotId != InventorySlots.INVALID)
							return slotId;
					}
				}
			}
		}
		else // Everything else
		{
			m_ComponentNames.Clear();
			targetEntity.GetActionComponentNameList(target.GetComponentIndex(), m_ComponentNames, LOD.NAME_VIEW);
			count = m_ComponentNames.Count();
			if (count > 0)
			{
				foreach (string sel2 : m_ComponentNames)
				{
					slotId = constructionActionData.GetAttachmentSlotFromSelection(player, targetEntity, item, sel2);
					if (slotId != InventorySlots.INVALID)
					{
						return slotId;
					}
				}
			}
		}
		
		return InventorySlots.INVALID;
	}
}