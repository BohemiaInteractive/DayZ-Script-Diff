class FirefighterAxe extends ToolBase
{
	void FirefighterAxe()
	{
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionMineTree);
		AddAction(ActionMineTreeBark);
		AddAction(ActionMineBush);
		//AddAction(ActionRepairPart);
		AddAction(ActionDismantlePart);
		//AddAction(ActionBuildPart);
		//AddAction(ActionDestroyPart);
		//AddAction(ActionSawPlanks);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionSkinning);
	}
}