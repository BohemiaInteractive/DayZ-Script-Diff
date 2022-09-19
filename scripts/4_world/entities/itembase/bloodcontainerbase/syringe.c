class Syringe extends BloodContainerBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionCollectSampleTarget);
		AddAction(ActionCollectSampleSelf);
	}
}