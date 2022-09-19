class BoletusMushroom : MushroomBase
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
	}
}
