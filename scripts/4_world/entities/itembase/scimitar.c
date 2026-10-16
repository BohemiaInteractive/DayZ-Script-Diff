class Scimitar : ToolBase
{
	void Scimitar()
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
		AddAction(ActionMineBush);
		AddAction(ActionMineTreeBark);
		AddAction(ActionDigWorms);
		AddAction(ActionCraftBolts);
	}
}