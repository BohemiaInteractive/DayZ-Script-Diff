class BoneKnife extends ToolBase
{
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
		AddAction(ActionSkinning);
		AddAction(ActionMineBush);
		AddAction(ActionMineTreeBark);
		AddAction(ActionDigWorms);
	}
}