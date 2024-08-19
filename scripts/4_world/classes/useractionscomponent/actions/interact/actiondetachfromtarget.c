class ActionDetachFromTarget: ActionInteractBase
{
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor;
	}
	
	void ActionDetachFromTarget()
	{
		m_Text = "#take_to_hands";
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override ActionData CreateActionData()
	{
		DetachActionData action_data = new DetachActionData;
		return action_data;
	}
	
	int FindSlotIdToDetach(PlayerBase player, ActionTarget target, ItemBase item)
	{
		EntityAI target_entity = EntityAI.Cast(target.GetObject());
		
		if ( player && target_entity )
		{
			array<string> selections = new array<string>();
			target_entity.GetActionComponentNameList( target.GetComponentIndex(),selections );
			
			if ( target_entity && target_entity.GetInventory() && target_entity.GetInventory().AttachmentCount() > 0 )
			{
				for(int i = 0; i < selections.Count(); i++ )
				{
					int target_slot_id = InventorySlots.GetSlotIdFromString( selections[i] );
					EntityAI att = target_entity.GetInventory().FindAttachment(target_slot_id);
					
					if ( att && player.GetInventory().CanAddEntityIntoHands(att) )
					{
						if ( att.CanDetachAttachment( target_entity ) && target_entity.CanReleaseAttachment( att ) )
							return target_slot_id;
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
			attSlotId = FindSlotIdToDetach(player, target, item);
		}
		
		if ( super.SetupAction( player, target, item, action_data, extra_data))
		{
			if (!GetGame().IsDedicatedServer())
			{
				if(attSlotId != InventorySlots.INVALID)
				{
					DetachActionData action_data_a = DetachActionData.Cast(action_data);
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
		if ( GetGame().IsMultiplayer() && GetGame().IsServer() ) return true;
		
		return FindSlotIdToDetach(player, target, item) != InventorySlots.INVALID);
	}
	
	override Object GetDisplayInteractObject(PlayerBase player, ActionTarget target)
	{
		int target_slot_id = FindSlotIdToDetach(player, target, null);
		EntityAI target_entity = EntityAI.Cast( target.GetObject() );
		
		if(target_slot_id != InventorySlots.INVALID)
		{
			return target_entity.GetInventory().FindAttachment(target_slot_id);
		}
		return null;
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		Process(action_data);
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		if(!GetGame().IsMultiplayer())
			Process(action_data);
	}

	void Process( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
		
		DetachActionData action_data_a = DetachActionData.Cast(action_data);
		EntityAI target_entity = EntityAI.Cast( action_data_a.m_Target.GetObject() );
				
		ItemBase attachment = ItemBase.Cast(target_entity.GetInventory().FindAttachment(action_data_a.m_AttSlot));
			
		if(attachment)
		{
			float stackable = attachment.GetTargetQuantityMax();
			if( stackable == 0 || stackable >= attachment.GetQuantity() )
			{
				//take to hands
				action_data.m_Player.PredictiveTakeEntityToHands( attachment );
			}
			else if( stackable != 0 && stackable < attachment.GetQuantity() )
			{
				//split and take to hands
				attachment.SplitIntoStackMaxHandsClient( action_data.m_Player );
			}
		}
	}
}

class ActionDetachFromTarget_SpecificSlot: ActionDetachFromTarget
{
	string m_slotToDetach;
	
	override int FindSlotIdToDetach(PlayerBase player, ActionTarget target, ItemBase item)
	{
		EntityAI target_entity = EntityAI.Cast(target.GetObject());
		
		if ( player && target_entity )
		{
			array<string> selections = new array<string>();
			target_entity.GetActionComponentNameList( target.GetComponentIndex(),selections );
			
			if ( target_entity && target_entity.GetInventory() && target_entity.GetInventory().AttachmentCount() > 0 )
			{
				for(int i = 0; i < selections.Count(); i++ )
				{
					if( selections[i] == m_slotToDetach )
					{
						int target_slot_id = InventorySlots.GetSlotIdFromString( selections[i] );
						EntityAI att = target_entity.GetInventory().FindAttachment(target_slot_id);
					
						if ( att && player.GetInventory().CanAddEntityIntoHands(att) )
						{
							if ( att.CanDetachAttachment( target_entity ) && target_entity.CanReleaseAttachment( att ) )
								return target_slot_id;
						}
					}
				}		
			}
		}
		return InventorySlots.INVALID;
	}
}

class ActionDetachFromTarget_SpecificSlotsCategory: ActionDetachFromTarget
{
	string m_slotsToDetach;
	
	override int FindSlotIdToDetach(PlayerBase player, ActionTarget target, ItemBase item)
	{
		EntityAI target_entity = EntityAI.Cast(target.GetObject());
		
		if ( player && target_entity )
		{
			array<string> selections = new array<string>();
			target_entity.GetActionComponentNameList( target.GetComponentIndex(),selections );
			
			if ( target_entity && target_entity.GetInventory() && target_entity.GetInventory().AttachmentCount() > 0 )
			{
				for(int i = 0; i < selections.Count(); i++ )
				{
					if (selections[i].Contains(m_slotsToDetach))
					{
						int target_slot_id = InventorySlots.GetSlotIdFromString( selections[i] );
						EntityAI att = target_entity.GetInventory().FindAttachment(target_slot_id);
					
						if ( att && player.GetInventory().CanAddEntityIntoHands(att) )
						{
							if ( att.CanDetachAttachment( target_entity ) && target_entity.CanReleaseAttachment( att ) )
								return target_slot_id;
						}
					}
				}		
			}
		}
		return InventorySlots.INVALID;
	}
}


class ActionDetachFromTarget_SpecificSlot_WoodenLogs: ActionDetachFromTarget_SpecificSlot
{
	void ActionDetachFromTarget_SpecificSlot_WoodenLogs ()
	{
		m_slotToDetach = "truck_01_woodenlogs";
	}
}

class ActionDetachFromTarget_SpecificSlot_WoodenPlanks: ActionDetachFromTarget_SpecificSlot
{
	void ActionDetachFromTarget_SpecificSlot_WoodenPlanks ()
	{
		m_slotToDetach = "truck_01_woodenplanks";
	}
}

class ActionDetachFromTarget_SpecificSlot_MetalSheets: ActionDetachFromTarget_SpecificSlot
{
	void ActionDetachFromTarget_SpecificSlot_MetalSheets ()
	{
		m_slotToDetach = "truck_01_metalsheets";
	}
}

class ActionDetachFromTarget_SpecificSlotsCategory_Barrel: ActionDetachFromTarget_SpecificSlotsCategory
{
	void ActionDetachFromTarget_SpecificSlotsCategory_Barrel ()
	{
		m_slotsToDetach = "truck_01_barrel";
	}
}

class ActionDetachFromTarget_SpecificSlotsCategory_WoodenCrate: ActionDetachFromTarget_SpecificSlotsCategory
{
	void ActionDetachFromTarget_SpecificSlotsCategory_WoodenCrate ()
	{
		m_slotsToDetach = "truck_01_woodencrate";
	}
}