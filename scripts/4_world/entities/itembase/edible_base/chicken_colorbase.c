class DeadChicken_ColorBase : Edible_Base
{
	void DeadChicken_ColorBase()
	{
		CacheSkinningBloodInfectionChance(eAgents.SALMONELLA);
	}

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
}

class DeadRooster : DeadChicken_ColorBase {}
class DeadChicken_White : DeadChicken_ColorBase {}
class DeadChicken_Spotted : DeadChicken_ColorBase {}
class DeadChicken_Brown : DeadChicken_ColorBase {}