class CCTSelf : CCTBase
{
	override bool Can( PlayerBase player, ActionTarget target )
	{
		return ( player && !player.IsDamageDestroyed());
	}
	
	override bool CanContinue( PlayerBase player, ActionTarget target )
	{
		return Can(player, target);
	}
};