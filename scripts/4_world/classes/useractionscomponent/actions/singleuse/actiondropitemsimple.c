class ActionDropItemSimple: ActionDropItem
{
	//bool m_HeavyItemOnly;
	
	void ActionDropItemSimple()
	{
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool MainItemAlwaysInHands()
	{
		return false;
	}
	
	override bool CanBeUsedOnBack()
	{
		return true;
	}
		
	override void Start( ActionData action_data )
	{
		action_data.m_MainItem.m_ThrowItemOnDrop = action_data.m_MainItem.ConfigGetString("physLayer") == "item_large"; //hack, should be redundant anyway
		if ( !GetGame().IsDedicatedServer() )
		{
			ClearInventoryReservationEx(action_data);
			PhysicalDropItem(action_data);
		}
	}
	
	override void PhysicalDropItem( ActionData action_data )
	{
		action_data.m_Player.PredictiveDropEntity(action_data.m_MainItem);
	}
};