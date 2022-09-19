class WoodAxe extends ToolBase
{
	override void SetActions()
	{
		super.SetActions();
		
		//AddAction(ActionBuildPartSwitch);
		AddAction(ActionMineTree);
		AddAction(ActionMineTreeBark);
		AddAction(ActionMineBush);
		//AddAction(ActionRepairPart);
		AddAction(ActionDismantlePart);
		//AddAction(ActionBuildPart);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionSkinning);
	}
}