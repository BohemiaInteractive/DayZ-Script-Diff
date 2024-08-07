class Shrimp extends Edible_Base
{
	override bool CanBeCookedOnStick()
	{
		return true;
	}	

	override bool CanBeCooked()
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
	
	override bool UseConfigInitTemperature()
	{
		return true;
	}	
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEatMeat);
	}
}
