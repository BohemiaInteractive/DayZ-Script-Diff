class BrickTrowel extends ToolBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionBuildPart);
		AddAction(ActionRepairPart);
		AddAction(ActionDigWorms);
		AddAction(ActionClapBearTrapWithThisItem);
		AddAction(ActionMineBush);
		AddAction(ActionMineTreeBark);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionCraftBolts);
	}
}