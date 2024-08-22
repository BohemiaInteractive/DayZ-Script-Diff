class Shovel extends ItemBase
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
		AddAction(ActionDigOutStash);
		AddAction(ActionDigInStash);
		AddAction(ActionFillObject);
		AddAction(ActionDigWorms);
		AddAction(ActionCreateGreenhouseGardenPlot);
	}
	
	override void SetActionAnimOverrides()
	{
		OverrideActionAnimation(ActionDigWorms, DayZPlayerConstants.CMD_ACTIONFB_DIGGIN_WORMS, DayZPlayerConstants.STANCEMASK_ERECT);
	}
	
	override int GetOnDigWormsAmount()
	{
		return 3;
	}
}