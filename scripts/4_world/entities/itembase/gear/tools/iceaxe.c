class Iceaxe: Inventory_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionMineRock1H);
		AddAction(ActionDigWorms);
		AddAction(ActionSkinning);
		AddAction(ActionMineBush);
		AddAction(ActionMineTreeBark);
		AddAction(ActionUnrestrainTarget);
	}
};