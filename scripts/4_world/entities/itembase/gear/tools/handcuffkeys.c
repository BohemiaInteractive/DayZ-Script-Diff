class HandcuffKeys: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionUnrestrainTarget);
	}
};
