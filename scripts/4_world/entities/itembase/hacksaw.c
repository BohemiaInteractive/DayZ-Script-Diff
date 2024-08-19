class Hacksaw extends ToolBase
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
}