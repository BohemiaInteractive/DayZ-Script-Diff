class AttachActionData : ActionData
{
	int m_AttSlot;
}


class ActionAttach : ActionSingleUseBase
{
	void ActionAttach()
	{
		m_Text = "#attach";
	}

	override void CreateConditionComponents() 
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override ActionData CreateActionData()
	{
		AttachActionData action_data = new AttachActionData();
		return action_data;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = null)
	{
		InventoryLocation il = new InventoryLocation();
		if (!GetGame().IsDedicatedServer())
		{
			EntityAI targetEntity;
			if (target.IsProxy())
			{
				targetEntity = EntityAI.Cast(target.GetParent());
			}
			else
			{
				targetEntity = EntityAI.Cast(target.GetObject());
			}
			
			if (!targetEntity.GetInventory().FindFreeLocationFor(item, FindInventoryLocationType.ATTACHMENT, il))
				return false;
		}
			
		if (super.SetupAction(player, target, item, action_data, extra_data))
		{
			#ifndef SERVER
			AttachActionData action_data_a = AttachActionData.Cast(action_data);
			action_data_a.m_AttSlot = il.GetSlot();
			#endif

			return true;
		}

		return false;
	}
	
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		EntityAI targetEntity = EntityAI.Cast(target.GetObject());
		if (targetEntity && item)
		{
			return targetEntity.GetInventory() && targetEntity.GetInventory().CanAddAttachment(item) && !targetEntity.CanUseConstruction();
		}

		return false;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		if (GetGame().IsMultiplayer())
			return;
		
		ClearInventoryReservationEx(action_data);
		AttachActionData action_data_a = AttachActionData.Cast(action_data);
		AttachItem(action_data_a);
	}
	
	override void OnExecuteClient(ActionData action_data)
	{
		ClearInventoryReservationEx(action_data);

		AttachActionData action_data_a = AttachActionData.Cast(action_data);
		AttachItem(action_data_a);
	}
	
	protected void AttachItem(AttachActionData action_data)
	{
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
			action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx(entity, action_data.m_MainItem, action_data.m_AttSlot);
		}
	}
	
	override bool CanBeSetFromInventory()
	{
		return false;
	}
}