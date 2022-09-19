class LugWrench: Inventory_Base 
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionLockAttachment);
	}
};
