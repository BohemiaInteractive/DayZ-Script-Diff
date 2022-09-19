class ActionForceFeedCan: ActionForceFeed
{	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		return true;
		/* 
		//if staging for cans is introduced
		if ( item && item.IsOpen() )
		{
			return true;
		}
		return false;
		*/
	}
};