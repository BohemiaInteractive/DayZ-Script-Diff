class WolfSteakMeat extends Edible_Base
{
	override bool CanBeCooked()
	{
		return true;
	}		
	
	override bool CanBeCookedOnStick()
	{
		return true;
	}
	
	override bool IsMeat()
	{
		return true;
	}
	
	override bool CanDecay()
	{
		return true;
	}
	
	override void HandleFoodStageChangeAgents(FoodStageType stageOld, FoodStageType stageNew)
	{
		int keepAgentsRnd = 0;
		if (Math.RandomFloat01() <= GameConstants.SALMONELLA_RETENTION_PREDATOR)
		{
			keepAgentsRnd |= eAgents.SALMONELLA;
		}
		
		switch (stageNew)
		{
			case FoodStageType.BAKED:
			case FoodStageType.BOILED:
			case FoodStageType.DRIED:
				RemoveAllAgentsExcept(keepAgentsRnd|eAgents.BRAIN|eAgents.HEAVYMETAL);
			break;
			
			case FoodStageType.BURNED:
				RemoveAllAgentsExcept(eAgents.SALMONELLA|eAgents.HEAVYMETAL);
			break;
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatMeat);
		
		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
	}
}
