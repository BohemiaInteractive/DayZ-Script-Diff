// Toggle object class for items which are able to dig
class ActionTogglePlaceObjectDigging : ActionTogglePlaceObject
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		if (!GetGame().IsDedicatedServer())
		{	
			if (!CfgGameplayHandler.GetDisableColdAreaPlacementCheck() && player.GetInColdArea())
				return false;	
		}
			
		return true;
	}
}