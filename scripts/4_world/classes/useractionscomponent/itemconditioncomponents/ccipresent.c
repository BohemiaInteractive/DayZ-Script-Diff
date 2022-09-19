class CCIPresent : CCIBase
{
	override bool Can( PlayerBase player, ItemBase item )
	{	
		return item != null;
	}
	
	override bool CanContinue( PlayerBase player, ItemBase item )
	{
		return ( Can(player, item) );
	}
};