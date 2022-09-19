class AttachActionData : ActionData
{
	int m_AttSlot;
}


class ActionAttach: ActionSingleUseBase
{
	void ActionAttach()
	{
		m_Text = "#attach";
	}

	override void CreateConditionComponents() 
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.DEFAULT );
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override ActionData CreateActionData()
	{
		AttachActionData action_data = new AttachActionData;
		return action_data;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		ref InventoryLocation il = new InventoryLocation;
		if (!GetGame().IsDedicatedServer())
		{
			EntityAI target_entity;
			
			if ( target.IsProxy() )
			{
				target_entity = EntityAI.Cast( target.GetParent() );
			}
			else
			{
				target_entity = EntityAI.Cast( target.GetObject() );
			}
			
			if (!target_entity.GetInventory().FindFreeLocationFor( item, FindInventoryLocationType.ATTACHMENT, il ))
				return false;
		}
			
		if ( super.SetupAction( player, target, item, action_data, extra_data))
		{
			if (!GetGame().IsDedicatedServer())
			{
				AttachActionData action_data_a = AttachActionData.Cast(action_data);
				action_data_a.m_AttSlot = il.GetSlot();
			}
			return true;
		}
		return false;
	}
	
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		EntityAI target_entity = EntityAI.Cast( target.GetObject() );
		
		if ( target_entity && item )
		{
			if ( target_entity.GetInventory() && target_entity.GetInventory().CanAddAttachment( item ) )
			{
				return true;
			}
		}	
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		if (GetGame().IsMultiplayer())
			return;
		
		ClearInventoryReservationEx(action_data);
		AttachActionData action_data_a = AttachActionData.Cast(action_data);
		EntityAI target_EAI;
			
		if ( action_data.m_Target.IsProxy() )
		{
			target_EAI = EntityAI.Cast( action_data_a.m_Target.GetParent() ); // cast to ItemBase
		}
		else
		{
			target_EAI = EntityAI.Cast( action_data_a.m_Target.GetObject() ); // cast to ItemBase
		}
		
		if (target_EAI && action_data_a.m_MainItem)
		{
			action_data_a.m_Player.PredictiveTakeEntityToTargetAttachmentEx(target_EAI, action_data_a.m_MainItem,action_data_a.m_AttSlot);
		}
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
		AttachActionData action_data_a = AttachActionData.Cast(action_data);
		
		EntityAI target_EAI;
			
		if ( action_data.m_Target.IsProxy() )
		{
			target_EAI = EntityAI.Cast( action_data_a.m_Target.GetParent() ); // cast to ItemBase
		}
		else
		{
			target_EAI = EntityAI.Cast( action_data_a.m_Target.GetObject() ); // cast to ItemBase
		}
		
		if (target_EAI && action_data_a.m_MainItem)
		{
			action_data_a.m_Player.PredictiveTakeEntityToTargetAttachmentEx(target_EAI, action_data_a.m_MainItem, action_data_a.m_AttSlot);
		}
	}
	
	override bool CanBeSetFromInventory()
	{
		return false;
	}
}