class CCINotRuinedAndDry extends CCIBase
{
	override bool Can( PlayerBase player, ItemBase item )
	{
		return ( item && !item.IsDamageDestroyed() && item.GetWet() <= 0 ) ;
	}
};