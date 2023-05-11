class DeadChicken_ColorBase extends Edible_Base
{
	override bool CanBeCookedOnStick()
	{
		return false;
	}

	override bool CanBeCooked()
	{
		return false;
	}	
	
	override bool IsCorpse()
	{
		return true;
	}

	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanBeSkinned()
	{
		if ( GetHealthLevel() == GameConstants.STATE_RUINED )
			return false;

		return true;
	}
}

class DeadRooster extends DeadChicken_ColorBase {}
class DeadChicken_White extends DeadChicken_ColorBase {}
class DeadChicken_Spotted extends DeadChicken_ColorBase {}
class DeadChicken_Brown extends DeadChicken_ColorBase {}