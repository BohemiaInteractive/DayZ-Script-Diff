
class ActionUnfoldEntity : ActionSingleUseBase
{
	void ActionUnfoldEntity ()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
	}

	override void CreateConditionComponents ()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget () { return false; }

	override bool ActionCondition ( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (item)
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
		if (old_item.ConfigIsExisting("ChangeIntoOnDetach"))
		{
			string str = old_item.ChangeIntoOnDetach();
			if (str  != "")
			{
				UnfoldEntityLambda lambda = new UnfoldEntityLambda(action_data.m_MainItem, str, action_data.m_Player);
				lambda.SetTransferParams(true, true, true, false, 1);
				action_data.m_Player.ServerReplaceItemInHandsWithNew(lambda);
			}
		}
	}
};

class UnfoldEntityLambda : TurnItemIntoItemLambda
{
	void UnfoldEntityLambda (EntityAI old_item, string new_item_type, PlayerBase player)
	{
	}
};

