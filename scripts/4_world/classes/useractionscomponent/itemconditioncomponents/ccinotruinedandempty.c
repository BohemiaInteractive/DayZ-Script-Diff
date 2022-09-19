class CCINotRuinedAndEmpty : CCIBase
{
	override bool Can( PlayerBase player, ItemBase item )
	{
		return ( item && !item.IsDamageDestroyed() && item.GetQuantity() > 0);
	}
};