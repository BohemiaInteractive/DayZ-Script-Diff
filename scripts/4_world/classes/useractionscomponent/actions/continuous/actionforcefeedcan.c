class ActionForceFeedCan: ActionForceFeed
{	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		return super.ActionCondition(player, target, item);
	}
}
