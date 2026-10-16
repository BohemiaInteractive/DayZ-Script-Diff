class CommonBrick extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttachToConstruction);
		AddAction(ActionAttachOnSelection);
	}
}