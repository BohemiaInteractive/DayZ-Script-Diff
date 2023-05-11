class WoodenStick : ItemBase
{
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
		AddAction(ActionAttach);
		AddAction(ActionDetach);
		AddAction(ActionAttachToConstruction);
		AddAction(ActionCraftBolts);
	}
}
