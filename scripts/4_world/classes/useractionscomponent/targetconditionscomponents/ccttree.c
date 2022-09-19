class CCTTree : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	void CCTTree ( float maximal_target_distance )
	{
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can( PlayerBase player, ActionTarget target )
	{	
		if( !target )
			return false;

		Object targetObject = target.GetObject();
		if ( !player || !targetObject || targetObject.IsDamageDestroyed() )
			return false;
		
		if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
			return true;

		return ( vector.DistanceSq(target.GetCursorHitPos(), player.GetPosition()) <= m_MaximalActionDistanceSq );
	}
};