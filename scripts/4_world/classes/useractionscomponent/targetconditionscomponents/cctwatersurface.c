class CCTWaterSurface : CCTBase
{
	protected const int HEIGHT_DIFF_LIMIT_METERS 	= 1.0;

	protected float m_MaximalActionDistanceSq;
	protected string m_SurfaceType; //!DEPRECATED
	protected ref array<string> m_AllowedSurfaceList;
	
	void CCTWaterSurface(float maximal_target_distance = UAMaxDistances.DEFAULT, string surfaceType = "")
	{
		m_MaximalActionDistanceSq 	= maximal_target_distance * maximal_target_distance;
		m_SurfaceType 				= surfaceType;
		
		m_AllowedSurfaceList		= new array<string>();
		surfaceType.Split("|", m_AllowedSurfaceList);
	}
	
	override bool Can(PlayerBase player, ActionTarget target)
	{
		if (!target || (target && target.GetObject()))
			return false;
		
		string surfaceType;

		//! use hit position from ActionTarget otherwise player's position
		vector hitPosition = target.GetCursorHitPos();
		if (hitPosition == vector.Zero)
		{
			float waterLevel = player.GetCurrentWaterLevel();
			hitPosition = player.GetPosition();
			if (waterLevel > 0.0)
			{
				g_Game.SurfaceGetType3D(hitPosition[0], hitPosition[1] + waterLevel, hitPosition[2], surfaceType);
				
				return Surface.AllowedWaterSurface(hitPosition[1] + waterLevel, surfaceType, m_AllowedSurfaceList);
			}
				
			return false;
		}
		
		g_Game.SurfaceGetType3D(hitPosition[0], hitPosition[1], hitPosition[2], surfaceType);
		
		float surfaceHeight = g_Game.SurfaceY(hitPosition[0], hitPosition[2]);
		float heightDiff = Math.AbsFloat(hitPosition[1] - surfaceHeight);
		if (heightDiff > HEIGHT_DIFF_LIMIT_METERS)
			return false;

		float distSq = vector.DistanceSq(player.GetPosition(), hitPosition);		
		//! special handling for sea
		if (!surfaceType)
		{
			float waterDepth = g_Game.GetWaterDepth(hitPosition);
			hitPosition[1] = heightDiff;
			distSq = distSq - waterDepth * waterDepth;
		}

		if (distSq > m_MaximalActionDistanceSq)
			return false;

		return Surface.AllowedWaterSurface(hitPosition[1], surfaceType, m_AllowedSurfaceList);
	}
	
	override bool CanContinue(PlayerBase player, ActionTarget target)
	{
		return true;
	}
}
