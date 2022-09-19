class CCTCursorNoRuinCheck : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	//approximate head heights
	const float HEIGHT_ERECT = 1.6;
	const float HEIGHT_CROUCH = 1.05;
	const float HEIGHT_PRONE = 0.66;
	
	void CCTCursorNoRuinCheck( float maximal_target_distance = UAMaxDistances.DEFAULT )
	{
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can( PlayerBase player, ActionTarget target )
	{
		if( !target )
			return false;
		
		Object targetObject = target.GetObject();
		if ( !player || !targetObject )
			return false;
		
		vector playerHeadPos = player.GetPosition();
		
		switch (player.m_MovementState.m_iStanceIdx)
		{
			case DayZPlayerConstants.STANCEIDX_ERECT:
				playerHeadPos[1] = playerHeadPos[1] + HEIGHT_ERECT;
			break;
			
			case DayZPlayerConstants.STANCEIDX_CROUCH:
				playerHeadPos[1] = playerHeadPos[1] + HEIGHT_CROUCH;
			break;
			
			case DayZPlayerConstants.STANCEIDX_PRONE:
				playerHeadPos[1] = playerHeadPos[1] + HEIGHT_PRONE;
			break;
		}
		
		float distanceRoot = vector.DistanceSq(target.GetCursorHitPos(), player.GetPosition());
		float distanceHead = vector.DistanceSq(target.GetCursorHitPos(), playerHeadPos);
		
		bool b1 = distanceRoot <= m_MaximalActionDistanceSq;
		bool b2 = distanceHead <= m_MaximalActionDistanceSq;
		
		if ( b1 || b2 )
			return true;
		
		return false;
	}
};