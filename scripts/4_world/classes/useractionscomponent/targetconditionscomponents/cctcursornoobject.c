class CCTCursorNoObject : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	void CCTCursorNoObject ( float maximal_target_distance )
	{
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can( PlayerBase player, ActionTarget target )
	{	
		if ( !target )
			return false;
			
		if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
			return true;
		
		vector playerHeadPos;
		MiscGameplayFunctions.GetHeadBonePos(player, playerHeadPos);
		
		float distanceRoot = vector.DistanceSq(target.GetCursorHitPos(), player.GetPosition());
		float distanceHead = vector.DistanceSq(target.GetCursorHitPos(), playerHeadPos);
		
		return ( distanceRoot <= m_MaximalActionDistanceSq || distanceHead <= m_MaximalActionDistanceSq );
	}
};