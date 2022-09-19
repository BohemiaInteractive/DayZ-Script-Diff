class CCINonRuined : CCIBase
{
	override bool Can( PlayerBase player, ItemBase item )
	{
		return ( item && !item.IsDamageDestroyed() );
	}
};