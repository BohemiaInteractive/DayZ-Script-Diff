class CCTCursorNoObject : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	void CCTCursorNoObject (float maximal_target_distance)
	{
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can(PlayerBase player, ActionTarget target)
	{	
		if (!target)
			return false;
			
		if ( g_Game.IsServer() && g_Game.IsMultiplayer() )
			return true;
		
		vector playerPosition = player.GetPosition();
		vector playerHeadPos = MiscGameplayFunctions.GetPlayerHeadPosition(player);
		vector cursorHitPosition = target.GetCursorHitPos();	
		
		float distanceRoot = vector.DistanceSq(cursorHitPosition, playerPosition);
		float distanceHead = vector.DistanceSq(cursorHitPosition, playerHeadPos);
		
		return (distanceRoot <= m_MaximalActionDistanceSq || distanceHead <= m_MaximalActionDistanceSq);
	}
}
