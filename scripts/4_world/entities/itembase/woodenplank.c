class WoodenPlank extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionClapBearTrapWithThisItem);
		AddAction(ActionAttachToConstruction);
		AddAction(ActionRepairCarChassis);
		AddAction(ActionAttachOnSelection);
	}
}