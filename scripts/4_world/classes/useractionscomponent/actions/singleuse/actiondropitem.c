class ActionDropItemCB : ActionSingleUseBaseCB
{	
	override void CreateActionComponent()
	{
		super.CreateActionComponent();
		EnableCancelCondition(true);
	}
		
	override void EndActionComponent()
	{
		SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		m_ActionData.m_State = UA_FINISHED;
	}
	
	bool CancelCondition()
	{
		HumanMovementState ms = new HumanMovementState();
		m_ActionData.m_Player.GetMovementState(ms);
		if (ms.m_CommandTypeId == DayZPlayerConstants.COMMANDID_MOVE)
		{
			// Cancel action, if hands are raised
			if (ms.IsRaised())
				return true;
		}
		
		return false;
	}	
}

class ActionDropItem : ActionSingleUseBase
{
	string m_ItemName = "";

	void ActionDropItem()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_DROPITEM_HANDS;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_DROPITEM_HANDS;
		m_CallbackClass 	= ActionDropItemCB;
		m_Text 				= "#drop_item";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNone();
	}
	
	override typename GetInputType()
	{
		return DropActionInput;
	}
	
	override bool HasProneException()
	{
		return true;
	}
	
	override bool CanBeUsedThrowing()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool UseAcknowledgment()
	{
		return false;
	}

	override bool ActionConditionContinue(ActionData action_data)
	{
		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		HumanCommandMove hcm = player.GetCommand_Move();
		if (hcm && hcm.IsChangingStance())
			return false;

		return player && item;
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		if (action_data.m_Player.IsPlacingServer())
			action_data.m_Player.PlacingCancelServer();
		
		ClearInventoryReservationEx(action_data);
		
		if (g_Game.IsServer() && g_Game.IsMultiplayer())
		{
			action_data.m_Player.ServerDropEntity(action_data.m_Player.GetItemInHands()); // multiplayer server side
		}
		else
		{
			PhysicalDropItem(action_data); // single player or multiplayer client side
		}
	}

	void PhysicalDropItem(ActionData action_data)
	{
		action_data.m_Player.PhysicalPredictiveDropItem(action_data.m_Player.GetItemInHands());
	}
}
