class DonerKnife : ToolBase
{
	void DonerKnife()
	{
	}

	override bool IsMeleeFinisher()
	{
		return false;
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionSkinning);
		AddAction(ActionMineBush);
		AddAction(ActionMineTreeBark);
		AddAction(ActionDigWorms);
		AddAction(ActionCraftBolts);
	}
}