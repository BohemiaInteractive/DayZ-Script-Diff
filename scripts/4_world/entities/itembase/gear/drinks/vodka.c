class Vodka: Bottle_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionWashHandsItem);
	}
};
