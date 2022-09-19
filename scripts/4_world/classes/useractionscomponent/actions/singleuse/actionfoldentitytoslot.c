class ActionFoldEntityToSlot : ActionSingleUseBase
{
	int m_SlotID = InventorySlots.INVALID;

	override void CreateConditionComponents ()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget () { return false; }

	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{
		EntityAI att = player.GetInventory().FindAttachment(m_SlotID);
		if ( item && att == null )
			return true;
		return false;
	}

	override bool ActionConditionContinue ( ActionData action_data ) { return true; }

	override void OnExecuteClient ( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
	}

	override void OnExecuteServer ( ActionData action_data )
	{
		if ( !GetGame().IsMultiplayer() )
			ClearInventoryReservationEx(action_data);

		ItemBase old_item = action_data.m_MainItem;
		if (old_item.ConfigIsExisting("ChangeIntoOnAttach"))
		{
			string slot_name = InventorySlots.GetSlotName(m_SlotID);
			string str = old_item.ChangeIntoOnAttach(slot_name);
			if (str != "")
			{
				FoldBandanaToSlotLambda lambda = new FoldBandanaToSlotLambda(action_data.m_MainItem, str, action_data.m_Player, m_SlotID);
				action_data.m_Player.ServerReplaceItemInHandsWithNewElsewhere(lambda);
			}
		}
	}
};

class FoldBandanaToSlotLambda : TurnItemIntoItemLambda
{
	void FoldBandanaToSlotLambda (EntityAI old_item, string new_item_type, PlayerBase player, int slot)
	{
		InventoryLocation targetAtt = new InventoryLocation;
		targetAtt.SetAttachment(player, NULL, slot);
		OverrideNewLocation(targetAtt);
	}
};
