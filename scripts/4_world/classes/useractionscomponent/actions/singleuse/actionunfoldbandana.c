class ActionUnfoldBandana : ActionUnfoldEntity
{
	void ActionUnfoldBandana ()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_Text = "#bandana_unfold";
	}
};

class UnfoldBandanaLambda : TurnItemIntoItemLambda
{
	void UnfoldBandanaLambda (EntityAI old_item, string new_item_type, PlayerBase player)
	{
	}
};

