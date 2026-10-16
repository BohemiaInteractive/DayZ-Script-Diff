class ActionDetachBattery: ActionInteractBase
{
	protected const string SELECTION_BACK = "back";
	
	override void CreateConditionComponents() 
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTCursor(UAMaxDistances.DEFAULT);
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;
		
		EntityAI targetEntity = EntityAI.Cast(target.GetObject());
		if (!targetEntity || targetEntity && !targetEntity.GetInventory())
			return false;
		
		if (!targetEntity.GetInventory().HasAttachmentSlot(Battery9V.SLOT_ID))
			return false;
		
		EntityAI attachment = targetEntity.GetInventory().FindAttachment(Battery9V.SLOT_ID);
		if (!attachment)
			return false;
		
		DigitalCodeLock codeLock = DigitalCodeLock.Cast(targetEntity);
		if (codeLock)
		{
			if (!codeLock.GetHierarchyRoot())
				return false;
			
			if (codeLock.IsLocked() || codeLock.IsLockedInSlot())
				return false;
			
			array<string> selections = new array<string>;
			if (codeLock)
				codeLock.GetActionComponentNameList(target.GetComponentIndex(), selections);
			
			bool isFactingBackSide;
			foreach (string selectionName : selections)
			{
				if (selectionName == SELECTION_BACK)
				{
					isFactingBackSide = true;
					break;
				}
			}
			
			if (!isFactingBackSide)
				return false;
		}
		
		if (player.GetInventory().CanAddEntityIntoHands(attachment))
		{
			if (attachment.CanDetachAttachment(targetEntity) && targetEntity.CanReleaseAttachment(attachment))
			{
				m_Text = "#detach_battery " + attachment.GetDisplayName();
				return true;
			}
		}
						
		return false;
	}
	
	override void OnExecute(ActionData action_data)
	{
		if (GetGame().IsDedicatedServer())
		{
			ClearActionJuncture(action_data);
			return;
		}
				
		EntityAI targetEntity = EntityAI.Cast(action_data.m_Target.GetObject());
		if (targetEntity)
		{
			EntityAI attachment = targetEntity.GetInventory().FindAttachment(Battery9V.SLOT_ID);
			if (attachment)
			{
				ProcessDetach(action_data, attachment);
			}
		}
	}
	
	void ProcessDetach(ActionData actionData, EntityAI attachment)
	{
		ClearInventoryReservationEx(actionData);
		
		//take to hands
		actionData.m_Player.PredictiveTakeEntityToHands(attachment);
	}
	
	override bool CanBeSetFromInventory()
	{
		return false;
	}
}
