class CCTObject : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	void CCTObject(float maximal_target_distance = UAMaxDistances.DEFAULT)
	{		
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can(PlayerBase player, ActionTarget target)
	{	
		Object targetObject = target.GetObject();
		if (!targetObject || !player)
			return false;
		
		vector playerPosition = player.GetPosition();
		vector playerHeadPos = MiscGameplayFunctions.GetPlayerHeadPosition(player);
		
		vector objectPosition = targetObject.GetPosition();
		
		float distanceRoot = vector.DistanceSq(objectPosition, playerPosition);
		float distanceHead = vector.DistanceSq(objectPosition, playerHeadPos);
		
		return (distanceRoot <= m_MaximalActionDistanceSq || distanceHead <= m_MaximalActionDistanceSq);
	}
}

class CCTParent : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	
	void CCTParent(float maximal_target_distance = UAMaxDistances.DEFAULT)
	{		
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;
	}
	
	override bool Can(PlayerBase player, ActionTarget target)
	{	
		Object targetParent = target.GetParent();
		if (!targetParent || !player)
			return false;
		
		vector playerPosition = player.GetPosition();
		vector playerHeadPos = MiscGameplayFunctions.GetPlayerHeadPosition(player);
		
		vector parentPosition = targetParent.GetPosition();
		
		float distanceRoot = vector.DistanceSq(parentPosition, playerPosition);
		float distanceHead = vector.DistanceSq(parentPosition, playerHeadPos);
		
		return (distanceRoot <= m_MaximalActionDistanceSq || distanceHead <= m_MaximalActionDistanceSq);
	}
}
