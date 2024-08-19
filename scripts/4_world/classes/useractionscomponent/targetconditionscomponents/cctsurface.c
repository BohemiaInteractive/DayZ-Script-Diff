class CCTSurface : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	void CCTSurface ( float maximal_target_distance = UAMaxDistances.DEFAULT )
	{
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can( PlayerBase player, ActionTarget target )
	{	
		if( !target || (target && target.GetObject()) )
			return false;
		
		if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
			return true;
		
		vector hit_pos = target.GetCursorHitPos();
		if (hit_pos == vector.Zero)
			return false;
		
		return ( vector.DistanceSq(hit_pos, player.GetPosition()) <= m_MaximalActionDistanceSq );
	}
};
