class CCINotPresent : CCIBase
{
	override bool Can( PlayerBase player, ItemBase item )
	{	
		return !item;
	}
	
	override bool CanContinue( PlayerBase player, ItemBase item )
	{
		return ( Can(player, item) );
	}
};