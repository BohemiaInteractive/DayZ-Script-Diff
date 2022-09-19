class HumanSteakMeat extends Edible_Base
{
	void HumanSteakMeat()
	{
		InsertAgent(eAgents.BRAIN, 1);
	}
	
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

	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatMeat);
		
		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
	}
}
