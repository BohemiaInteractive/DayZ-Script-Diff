class SpaghettiCan_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !(GetAgents() & eAgents.FOOD_POISON);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedCan);
		AddAction(ActionEatCan);
	}
}
