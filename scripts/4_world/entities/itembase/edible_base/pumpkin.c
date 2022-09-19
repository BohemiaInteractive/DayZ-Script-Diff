class Pumpkin : Edible_Base
{
	override bool CanBeCooked()
	{
		return false;
	}		
	
	override bool CanBeCookedOnStick()
	{
		return false;
	}
	
	override bool IsFruit()
	{
		return true;
	}

	override bool CanDecay()
	{
		return true;
	}
}