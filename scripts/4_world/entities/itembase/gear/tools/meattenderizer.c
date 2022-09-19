class MeatTenderizer: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionBuildPart);
		AddAction(ActionRepairPart);
		AddAction(ActionMineRock1H);


	}
};
