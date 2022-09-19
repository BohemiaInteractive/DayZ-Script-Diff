class CCTNonRuined : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	void CCTNonRuined ( float maximal_target_distance = UAMaxDistances.DEFAULT )
	{		
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can( PlayerBase player, ActionTarget target )
	{	
		if( !target )
			return false;
		
		Object targetObject = target.GetObject();
		if ( !player || !targetObject || targetObject.IsMan() || targetObject.IsDamageDestroyed() )
			return false;
				
		return ( vector.DistanceSq(targetObject.GetPosition(), player.GetPosition()) <= m_MaximalActionDistanceSq );
	}
};