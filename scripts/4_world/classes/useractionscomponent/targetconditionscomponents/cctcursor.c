class CCTCursor : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	//approximate head heights
	const float HEIGHT_ERECT = 1.6;
	const float HEIGHT_CROUCH = 1.05;
	const float HEIGHT_PRONE = 0.66;
	
	void CCTCursor( float maximal_target_distance = UAMaxDistances.DEFAULT )
	{
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can( PlayerBase player, ActionTarget target )
	{
		if( !target )
			return false;
		
		Object targetObject = target.GetObject();
		if ( !targetObject )
			targetObject = target.GetParent();
		
		if ( !player || !targetObject || targetObject.IsDamageDestroyed() )
			return false;
		
		vector playerHeadPos;
		MiscGameplayFunctions.GetHeadBonePos(player, playerHeadPos);
		
		float distanceRoot = vector.DistanceSq(target.GetCursorHitPos(), player.GetPosition());
		float distanceHead = vector.DistanceSq(target.GetCursorHitPos(), playerHeadPos);
		
		return ( distanceRoot <= m_MaximalActionDistanceSq || distanceHead <= m_MaximalActionDistanceSq );
	}
};