class BloodSyringe extends BloodContainerBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionGiveBloodTarget);
		AddAction(ActionGiveBloodSelf);
	}
}