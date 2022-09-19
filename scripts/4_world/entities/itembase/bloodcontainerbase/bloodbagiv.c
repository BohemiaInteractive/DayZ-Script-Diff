class BloodBagIV extends BloodContainerBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionGiveBloodTarget);
		AddAction(ActionGiveBloodSelf);
	}
}