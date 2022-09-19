class Iceaxe: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionBuryAshes);
		AddAction(ActionDigGardenPlot);
		AddAction(ActionDismantleGardenPlot);
		//AddAction(ActionMineRock1H);
		AddAction(ActionDigWorms);
		AddAction(ActionDismantlePart);
		AddAction(ActionBuildPart);
		AddAction(ActionSkinning);
		AddAction(ActionCreateGreenhouseGardenPlot);
	}
};
