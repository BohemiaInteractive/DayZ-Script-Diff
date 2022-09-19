class SurvivorBase extends PlayerBaseClient
{
	override int GetHideIconMask()
	{
		return EInventoryIconVisibility.HIDE_VICINITY;
	}
}