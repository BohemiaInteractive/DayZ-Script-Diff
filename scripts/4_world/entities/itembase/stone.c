class Stone extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionAttach);
		AddAction(ActionDetach);
	}
}