class CCTCursorParent : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	//approximate head heights
	const float HEIGHT_ERECT = 1.6;
	const float HEIGHT_CROUCH = 1.05;
	const float HEIGHT_PRONE = 0.66;
	
	void CCTCursorParent(float maximal_target_distance = UAMaxDistances.DEFAULT)
	{
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can(PlayerBase player, ActionTarget target)
	{
		if (!target)
			return false;
		
		Object parentObject = target.GetParent();
		if (!player || !parentObject || parentObject.IsDamageDestroyed())
			return false;
		
		vector playerPosition = player.GetPosition();
		vector playerHeadPos = MiscGameplayFunctions.GetPlayerHeadPosition(player);
		
		vector cursorHitPosition = target.GetCursorHitPos();	
		
		float distanceRoot = vector.DistanceSq(cursorHitPosition, playerPosition);
		float distanceHead = vector.DistanceSq(cursorHitPosition, playerHeadPos);
		
		return (distanceRoot <= m_MaximalActionDistanceSq || distanceHead <= m_MaximalActionDistanceSq);
	}
}
