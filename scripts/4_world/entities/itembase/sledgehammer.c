class SledgeHammer extends Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionClapBearTrapWithThisItem);
		AddAction(ActionMineRock);
		AddAction(ActionDismantlePart);
		AddAction(ActionBuildPart);
		//AddAction(ActionDestroyPart);
	}
}