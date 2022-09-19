class Binoculars: ItemOptics
{
	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionViewOptics);
		AddAction(ActionViewBinoculars);
	}
};
