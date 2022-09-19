class BakedBeansCan_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedCan);
		AddAction(ActionEatCan);
	}
};
