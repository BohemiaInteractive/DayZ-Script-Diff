class FieldShovel extends ItemBase
{
	override bool CanMakeGardenplot()
	{
		return true;
	}

	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionClapBearTrapWithThisItem);
		AddAction(ActionTogglePlaceObjectDigging);
		AddAction(ActionDigGardenPlot);
		AddAction(ActionDismantleGardenPlot);
		AddAction(ActionDismantlePart);
		AddAction(ActionBuildPart);
		AddAction(ActionBuryBody);
		AddAction(ActionBuryAshes);
		AddAction(ActionDigInStash);
		AddAction(ActionDigOutStash);
		AddAction(ActionFillObject);
		AddAction(ActionDigWorms);
		AddAction(ActionCreateGreenhouseGardenPlot);
	}
		
	override int GetOnDigWormsAmount()
	{
		return 3;
	}
}