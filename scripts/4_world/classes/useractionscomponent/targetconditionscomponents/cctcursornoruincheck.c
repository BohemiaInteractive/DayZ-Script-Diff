class CCTCursorNoRuinCheck : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	protected float m_MaximalActionDistance;

	//approximate head heights
	const float HEIGHT_ERECT = 1.6;
	const float HEIGHT_CROUCH = 1.05;
	const float HEIGHT_PRONE = 0.66;
	
	void CCTCursorNoRuinCheck(float maximal_target_distance = UAMaxDistances.DEFAULT)
	{
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
		m_MaximalActionDistance = maximal_target_distance;
	}
	
	override bool Can(PlayerBase player, ActionTarget target)
	{
		if (!target)
			return false;
		
		Object targetObject = target.GetObject();
		if (!targetObject)
			targetObject = target.GetParent();
		
		if (!player || !targetObject)
			return false;
		
		if (player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			float maxDistanceWithSize = m_MaximalActionDistance + targetObject.GetCollisionRadius();
			return (vector.DistanceSq(targetObject.GetPosition(), player.GetPosition()) <= maxDistanceWithSize * maxDistanceWithSize);
		}

		vector playerPosition = player.GetPosition();
		vector playerHeadPos = MiscGameplayFunctions.GetPlayerHeadPosition(player);
		
		vector cursorHitPosition = target.GetCursorHitPos();	
		
		float distanceRoot = vector.DistanceSq(cursorHitPosition, playerPosition);
		float distanceHead = vector.DistanceSq(cursorHitPosition, playerHeadPos);
		
		return (distanceRoot <= m_MaximalActionDistanceSq || distanceHead <= m_MaximalActionDistanceSq);
	}
}
