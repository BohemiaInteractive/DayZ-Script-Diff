class ActionRefuelTorch: ActionSingleUseBase
{	
	void ActionRefuelTorch()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#STR_RefuelTorch0";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionTarget	= new CCTNonRuined();
		m_ConditionItem		= new CCINonRuined();
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Torch torch;
		Rag rag;
		Rag torchRag;
		
		if ((Class.CastTo(torch,target.GetObject()) && Class.CastTo(rag,item)) || (Class.CastTo(rag,target.GetObject()) && Class.CastTo(torch,item)))
		{
			if (Class.CastTo(torchRag,torch.GetRag()))
			{
				int slotId = InventorySlots.INVALID;
				InventoryLocation lcn = new InventoryLocation();
				torchRag.GetInventory().GetCurrentInventoryLocation(lcn);
				slotId = lcn.GetSlot();
				
				return torch.GetInventory().GetSlotLock(slotId) && torch.GetRagQuantity() < torchRag.GetQuantityMax() && torch.IsRagDryEnough(rag);
			}
		}
		
		return false;
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		Torch torch;
		Rag rag;
		Rag torchRag;
		
		if ((Class.CastTo(torch,action_data.m_Target.GetObject()) && Class.CastTo(rag,action_data.m_MainItem)) || (Class.CastTo(rag,action_data.m_Target.GetObject()) && Class.CastTo(torch,action_data.m_MainItem)))
		{
			Class.CastTo(torchRag,torch.GetRag());
			
			int qtyTransfer = torchRag.GetQuantityMax() - torch.GetRagQuantity(); //torch rag capacity
			qtyTransfer = (int)Math.Clamp(rag.GetQuantity(),0,qtyTransfer);
			torchRag.AddQuantity(qtyTransfer);
			rag.AddQuantity(-qtyTransfer);
		}
	}
};
