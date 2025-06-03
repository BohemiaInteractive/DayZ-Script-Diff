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
		
		//! use hit position from ActionTarget otherwise player's position
		vector hitPosition = target.GetCursorHitPos();
		
		string surfaceType;
		float waterLevel = player.GetCurrentWaterLevel();
		g_Game.SurfaceGetType3D(hitPosition[0], hitPosition[1] + waterLevel, hitPosition[2], surfaceType);

		if (waterLevel > 0.0)
			return Surface.AllowedWaterSurface(hitPosition[1] + waterLevel, surfaceType, m_AllowedSurfaceList);

		float surfaceHeight = g_Game.SurfaceY(hitPosition[0], hitPosition[2]);		
		//! special handling for sea
		if (!surfaceType)
		{
			surfaceHeight = hitPosition[1];
		}

		float heightDiff = Math.AbsFloat(hitPosition[1] - surfaceHeight);
		if (surfaceType != "" && heightDiff > HEIGHT_DIFF_LIMIT_METERS)
			return false;
		
		float distSq = vector.DistanceSq(player.GetPosition(), hitPosition);
		if (distSq > m_MaximalActionDistanceSq)
			return false;

		return Surface.AllowedWaterSurface(hitPosition[1], surfaceType, m_AllowedSurfaceList);
	}
	
	override bool CanContinue(PlayerBase player, ActionTarget target)
	{
		return true;
	}
}

class CCTWaterSurfaceEx : CCTBase
{
	protected float m_MaximalActionDistanceSq;
	protected int m_AllowedLiquidSource;
	protected int m_LiquidType = LIQUID_NONE;
	
	void CCTWaterSurfaceEx(float maximal_target_distance, int allowedLiquidSource)
	{
		m_MaximalActionDistanceSq = maximal_target_distance * maximal_target_distance;	
		m_AllowedLiquidSource = allowedLiquidSource;
	}
	
	override bool Can(PlayerBase player, ActionTarget target)
	{
		if (!target)
			return false;
		
		vector hitPosition = target.GetCursorHitPos();
		float distSq = vector.DistanceSq(player.GetPosition(), hitPosition);
		if (distSq > m_MaximalActionDistanceSq)
			return false;
				
		int liquidType = GetSurfaceLiquidType(target);
		string surfaceName = target.GetSurfaceName();
		return CheckLiquidSource(hitPosition, surfaceName, liquidType, m_AllowedLiquidSource);
	}
	
	bool CheckLiquidSource(vector hitPos, string surfaceName, int liquidType, int allowedWaterSourceMask)
	{
		bool success = false;
		if (surfaceName == "" && hitPos != vector.Zero)
		{
			//! Only return true if surface name is empty and
			bool isSea = g_Game.SurfaceIsSea(hitPos[0], hitPos[2]);
			if (allowedWaterSourceMask & LIQUID_SALTWATER && isSea)
			{
				success = hitPos[1] <= (g_Game.SurfaceGetSeaLevel() + 0.25);
			}
		}
		else
		{
			success = allowedWaterSourceMask & liquidType;
		}

		return success;
	}
	
	int GetSurfaceLiquidType(ActionTarget target)
	{
		m_LiquidType = LIQUID_NONE;
		
		//! Check target surface liquid source first
		if (target.GetSurfaceLiquidType() != LIQUID_NONE)
		{
			m_LiquidType = target.GetSurfaceLiquidType();
		}
		
		//! Check target object for liquid source next
		Object targetObject = target.GetObject();
		if (m_LiquidType == LIQUID_NONE && targetObject)
		{
			m_LiquidType = targetObject.GetLiquidSourceType();
		}
		
		return m_LiquidType;
	}
	
	override bool CanContinue(PlayerBase player, ActionTarget target)
	{
		return true;
	}
	
	int GetLiquidType()
	{
		return m_LiquidType;
	}
}
