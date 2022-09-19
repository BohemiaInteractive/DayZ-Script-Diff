class Sickle extends ToolBase
{
	void Sickle()
	{
	}

	override bool IsMeleeFinisher()
	{
		return true;
	}
	
	override array<int> GetValidFinishers()
	{
		return {EMeleeHitType.FINISHER_LIVERSTAB,EMeleeHitType.FINISHER_NECKSTAB};
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionMineBush);
		AddAction(ActionSkinning);
		AddAction(ActionMineTreeBark);
		AddAction(ActionDigWorms);
		AddAction(ActionShaveTarget);
		AddAction(ActionShave);
	}
}