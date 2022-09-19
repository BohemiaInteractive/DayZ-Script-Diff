class SalineBagIV: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionGiveSalineTarget);
		AddAction(ActionGiveSalineSelf);
	}
};
