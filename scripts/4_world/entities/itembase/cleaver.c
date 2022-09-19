class Cleaver extends ToolBase
{
	void Cleaver()
	{
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionBurnSewTarget);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionSkinning);
		AddAction(ActionMineBush);
		AddAction(ActionMineTreeBark);
		AddAction(ActionBurnSewSelf);
		AddAction(ActionDigWorms);
		AddAction(ActionShaveTarget);
		AddAction(ActionShave);
	}
}