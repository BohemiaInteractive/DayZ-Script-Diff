class CCTCursorInherited : CCTBase
{
	protected float					m_MaximalActionDistanceSq;
	protected ref array<typename>	m_AllowedTypes;

	void CCTCursorInherited(notnull array<typename> allowedTypes, float maximal_target_distance = UAMaxDistances.DEFAULT)
	{
		m_MaximalActionDistanceSq	= maximal_target_distance * maximal_target_distance;
		m_AllowedTypes				= allowedTypes;
	}

	override bool Can(PlayerBase player, ActionTarget target)
	{
		if (!target || !player)
			return false;

		Object targetObject = target.GetObject();
		if (!targetObject)
			targetObject = target.GetParent();

		if (!targetObject)
			return false;

		foreach (typename allowedType : m_AllowedTypes)
		{
			if (targetObject.IsInherited(allowedType))
			{
				vector cursorHitPos = target.GetCursorHitPos();
				bool fromPlayerRoot = vector.DistanceSq(cursorHitPos, player.GetPosition()) <= m_MaximalActionDistanceSq;
				bool fromPlayerHead = vector.DistanceSq(cursorHitPos, MiscGameplayFunctions.GetPlayerHeadPosition(player)) <= m_MaximalActionDistanceSq;
				
				return fromPlayerRoot || fromPlayerHead;
			}
		}

		return false;
	}
}
