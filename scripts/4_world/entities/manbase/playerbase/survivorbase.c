class SurvivorBase extends PlayerBaseClient
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