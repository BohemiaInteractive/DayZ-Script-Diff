class ActionDropItemSimple: ActionDropItem
{
	void ActionDropItemSimple() {}
	
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
		
	override void Start(ActionData action_data)
	{
		super.Start(action_data);

		#ifndef SERVER
		ClearInventoryReservationEx(action_data);
		PhysicalDropItem(action_data);
		#endif
	}
	
	override void PhysicalDropItem(ActionData action_data)
	{
		action_data.m_Player.PredictiveDropEntity(action_data.m_MainItem);
	}
}
