class CCTMan : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	protected bool m_MustBeAlive;
	
	void CCTMan ( float maximal_target_distance = UAMaxDistances.DEFAULT, bool must_be_alive = true )
	{
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;	
		m_MustBeAlive = must_be_alive;
	}
	
	override bool Can( PlayerBase player, ActionTarget target )
	{	
		if( !target )
			return false;
		
		Object targetObject = target.GetObject();
		if ( !player || !targetObject || targetObject == player || !targetObject.IsMan() || !player.IsFacingTarget(targetObject) || ( m_MustBeAlive && targetObject.IsDamageDestroyed() ) )
			return false;
		
		return ( vector.DistanceSq(targetObject.GetPosition(), player.GetPosition()) <= m_MaximalActionDistanceSq );
	}
};