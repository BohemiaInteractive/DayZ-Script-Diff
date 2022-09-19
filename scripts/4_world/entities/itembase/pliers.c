class Pliers extends ToolBase
{
	void Pliers()
	{
		m_MineDisarmRate = 100;
	}

	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionDismantlePart);
		AddAction(ActionBuildPart);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionMountBarbedWire);
		AddAction(ActionUnmountBarbedWire);
		AddAction(ActionLockAttachment);
		AddAction(ActionDisarmExplosive);
	}
}