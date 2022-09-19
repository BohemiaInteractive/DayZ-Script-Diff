class MetalPlate extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttachToConstruction);
		AddAction(ActionAttachOnSelection);
	}
}