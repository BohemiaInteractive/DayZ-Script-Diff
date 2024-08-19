class HandSaw: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionMineBush);
		AddAction(ActionSawPlanks);
		AddAction(ActionDismantlePart);
		//AddAction(ActionDestroyCombinationLock);
		//AddAction(ActionDestroyPart);
		AddAction(ActionSkinning);
		AddAction(ActionMineTreeBark);
	}
};
