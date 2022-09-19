class HayHook: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionSkinning);
	}
};
