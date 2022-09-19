class Wrench: Inventory_Base 
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionLockAttachment);
		AddAction(ActionMineRock1H);
	}
};
