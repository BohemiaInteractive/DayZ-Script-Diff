class SurvivorBaseType : PlayerBaseType
{

};

class SurvivorBase : PlayerBaseClient
{
	void SurvivorBase()
	{
		CacheSkinningBloodInfectionChance(eAgents.SALMONELLA);
	}

	override int GetHideIconMask()
	{
		return EInventoryIconVisibility.HIDE_VICINITY;
	}
}