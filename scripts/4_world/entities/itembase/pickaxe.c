class Pickaxe extends ItemBase
{
	override bool CanMakeGardenplot()
	{
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionTogglePlaceObjectDigging);
		AddAction(ActionDigGardenPlot);
		AddAction(ActionDismantleGardenPlot);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionMineRock);
		AddAction(ActionDismantlePart);
		AddAction(ActionBuildPart);
		AddAction(ActionBuryBody);
		AddAction(ActionBuryAshes);
		AddAction(ActionDigWorms);
		AddAction(ActionSkinning);
		AddAction(ActionDigOutStash);
		AddAction(ActionDigInStash);
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