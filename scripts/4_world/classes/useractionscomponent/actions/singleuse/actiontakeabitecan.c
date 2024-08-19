class ActionTakeABiteCan: ActionTakeABite
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		return false;
		/* 
		//if staging for cans is introduced
		if ( action_data.m_MainItem && action_data.m_MainItem.IsOpen() )
		{
			return true;
		}
		return false;
		*/
	}
};