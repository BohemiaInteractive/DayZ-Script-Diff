class Lockpick: ToolBase
{
	void Lockpick()
	{
		m_MineDisarmRate = 90;
	}
	
	override int GetKeyCompatibilityType()
	{
		return 1 << EBuildingLockType.LOCKPICK;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionLockDoors);
		AddAction(ActionUnlockDoors);
		AddAction(ActionDisarmMine);
		AddAction(ActionDisarmExplosive);
	}
};
