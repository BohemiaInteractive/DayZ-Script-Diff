class CCIBase
{
	bool Can( PlayerBase player, ItemBase item )
	{
		return true;
	}
	
	bool CanContinue( PlayerBase player, ItemBase item )
	{
		return ( Can(player,item) && item && item == player.GetItemInHands() );
	}
};