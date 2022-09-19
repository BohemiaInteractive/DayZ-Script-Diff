class BloodTestKit: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTestBloodTarget);
		AddAction(ActionTestBloodSelf);
	}
};
