class ActionUnlockShippingContainer: ActionUnlockDoors
{
	//custom condition, wrong key unlock attempt is allowed
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ContainerLockedBase shipCont;
		if (Class.CastTo(shipCont, target.GetObject()))
		{
			int doorIndex = TranslateLockSelectionIntoDoorIdx(target);
			if (doorIndex != -1)
				return shipCont.IsDoorLocked(doorIndex);
		}		
		return false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		ShippingContainerKeys_ColorBase key = ShippingContainerKeys_ColorBase.Cast(action_data.m_MainItem);
		ContainerLockedBase shipCont = ContainerLockedBase.Cast(action_data.m_Target.GetObject());
		if (shipCont && key && ((shipCont.GetLockCompatibilityType(shipCont.GetDoorIndex(action_data.m_Target.GetComponentIndex())) & key.GetKeyCompatibilityType()) == 0))
		{
			key.DestroyKeyServer();
		}
		else
		{
			UnlockDoor(action_data.m_Target);
			MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, APPLIED_DMG);
		}
	}
	
	override protected void UnlockDoor(ActionTarget target)
	{
		Building building;
		if (Class.CastTo(building, target.GetObject()))
		{
			int doorIndex = TranslateLockSelectionIntoDoorIdx(target);
			if (doorIndex != -1)
			{
				building.UnlockDoor(doorIndex,false);
			}
		}
	}
	
	//! Returns door idx
	protected int TranslateLockSelectionIntoDoorIdx(ActionTarget target)
	{
		//side1_lock
		ContainerLockedBase shipCont;
		if (Class.CastTo(shipCont, target.GetObject()))
		{
			string selectionName = shipCont.GetActionComponentName( target.GetComponentIndex() );
			
			if (selectionName.Contains("_lock"))
				return (selectionName.Substring(4,1).ToInt() - 1);
		}
		
		return -1;
	}
};