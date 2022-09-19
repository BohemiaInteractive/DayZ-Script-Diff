class Pitchfork extends ItemBase
{
	void Pitchfork()
	{
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionClapBearTrapWithThisItem);
	}
}