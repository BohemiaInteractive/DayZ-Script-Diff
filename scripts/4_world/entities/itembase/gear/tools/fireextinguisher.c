class FireExtinguisher: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionClapBearTrapWithThisItem);
		AddAction(ActionExtinguishFireplaceByExtinguisher);
	}
};
