class Nail extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttachToConstruction);
	}
}