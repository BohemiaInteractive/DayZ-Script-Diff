class CCIDummy : CCIBase
{
	override bool Can( PlayerBase player, ItemBase item )
	{
		return item != null;
	}
};