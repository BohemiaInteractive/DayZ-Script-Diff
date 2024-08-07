// Toggle object class for items which are able to dig
class ActionTogglePlaceObjectDigging : ActionTogglePlaceObject
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{			
		return !player.GetInColdArea();
	}
}