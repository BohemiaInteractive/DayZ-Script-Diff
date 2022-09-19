class CCTDummy : CCTBase
{
	override bool Can( PlayerBase player, ActionTarget target )
	{
		return ( target && target.GetObject() );
	}
};