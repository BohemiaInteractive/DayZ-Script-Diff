class ActionAttachOnProxy: ActionAttach
{
	void ActionAttachOnProxy()
	{
		m_Text = "#attach";
	}
	
	override void CreateConditionComponents() 
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursor;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!GetGame().IsDedicatedServer() && item)
			m_Text = "#attach " + item.GetDisplayName();
		
		EntityAI targetEntity = EntityAI.Cast(target.GetObject());				
		if (targetEntity)
		{
			if (CanAttachToProxy(player, targetEntity, item, target.GetComponentIndex()))
				return true;
		}
		
		EntityAI targetParentEntity = EntityAI.Cast(target.GetParent());
		if (targetParentEntity)
		{
			if (CanAttachToProxy(player, targetParentEntity, item, target.GetComponentIndex()))
				return true;
		}
				
		return false;
	}
	
	protected bool CanAttachToProxy(PlayerBase player, EntityAI targetEntity, ItemBase item, int componentIndex = -1)
	{
		if (targetEntity && item)
		{
			GameInventory inventory = targetEntity.GetInventory();
			InventoryLocation dst = new InventoryLocation();
			if (inventory && inventory.CanAddAttachment(item) && inventory.FindFreeLocationFor(item, FindInventoryLocationType.ANY_CARGO | FindInventoryLocationType.ATTACHMENT, dst))
			{
				return true;
			}
		}
		
		return false;
	}
	
	override void OnExecuteClient(ActionData action_data)
	{
		super.OnExecuteClient(action_data);
		
		if (action_data.m_Player.IsPlacingLocal())
		{
			action_data.m_Player.TogglePlacingLocal();
		}
	}
	
	override protected void AttachItem(AttachActionData action_data)
	{
		EntityAI targetEntity = EntityAI.Cast(action_data.m_Target.GetObject());
		if (targetEntity && CanAttachToProxy(action_data.m_Player, targetEntity, action_data.m_MainItem, action_data.m_Target.GetComponentIndex()))
		{
			action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx(targetEntity, action_data.m_MainItem, action_data.m_AttSlot);
			return;
		}
		
		EntityAI targetParentEntity = EntityAI.Cast(action_data.m_Target.GetParent());	
		if (targetParentEntity && CanAttachToProxy(action_data.m_Player, targetParentEntity, action_data.m_MainItem, action_data.m_Target.GetComponentIndex()))
		{
			action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx(targetParentEntity, action_data.m_MainItem, action_data.m_AttSlot);
		}
	}
}