class BloodBagEmpty extends BloodContainerBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionCollectBloodTarget);
		AddAction(ActionCollectBloodSelf);
	}
}