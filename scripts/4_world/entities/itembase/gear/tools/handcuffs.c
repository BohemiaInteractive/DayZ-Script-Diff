class Handcuffs: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionRestrainTarget);
		AddAction(ActionRestrainSelf);
	}
};
