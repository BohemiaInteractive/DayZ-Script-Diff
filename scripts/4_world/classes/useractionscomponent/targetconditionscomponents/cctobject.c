class CCTObject : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	void CCTObject ( float maximal_target_distance = UAMaxDistances.DEFAULT )
	{		
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can( PlayerBase player, ActionTarget target )
	{	
		Object targetObject = target.GetObject();
		if ( !targetObject || !player )
			return false;
		
		vector playerHeadPos;
		MiscGameplayFunctions.GetHeadBonePos(player, playerHeadPos);
		
		float distanceRoot = vector.DistanceSq(targetObject.GetPosition(), player.GetPosition());
		float distanceHead = vector.DistanceSq(targetObject.GetPosition(), playerHeadPos);
		
		return ( distanceRoot <= m_MaximalActionDistanceSq || distanceHead <= m_MaximalActionDistanceSq );
	}
};

class CCTParent : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	void CCTParent ( float maximal_target_distance = UAMaxDistances.DEFAULT )
	{		
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can( PlayerBase player, ActionTarget target )
	{	
		Object targetParent = target.GetParent();
		if ( !targetParent || !player )
			return false;
		
		vector playerHeadPos;
		MiscGameplayFunctions.GetHeadBonePos(player, playerHeadPos);
		
		float distanceRoot = vector.DistanceSq(targetParent.GetPosition(), player.GetPosition());
		float distanceHead = vector.DistanceSq(targetParent.GetPosition(), playerHeadPos);
		
		return ( distanceRoot <= m_MaximalActionDistanceSq || distanceHead <= m_MaximalActionDistanceSq );
	}
};