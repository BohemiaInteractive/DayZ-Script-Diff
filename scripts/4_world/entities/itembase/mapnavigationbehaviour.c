enum EMapNavigationType
{
	BASIC 	= 0,
	COMPASS	= 1,
	GPS 	= 2,
	ALL		= 4
}

class MapNavigationBehaviour
{
	const int RANDOM_DEVIATION_MIN 						= 4;
	const int RANDOM_DEVIATION_MAX 						= 15;
	
	static const int DISPLAY_GRID_POS_MAX_CHARS_COUNT 	= 3;
	static const int DISPLAY_ALT_MAX_CHARS_COUNT 		= 4;
	
	protected static const string GRID_SIZE_CFG_PATH	= "CfgWorlds %1 Grid Zoom1 stepX";

	protected int					m_RandomPositionDeviationX;
	protected int					m_RandomPositionDeviationZ;
	protected EMapNavigationType	m_NavigationType;
	protected PlayerBase			m_Player;
	
	protected ref array<EntityAI>	m_GPSInPossessionArr;
	protected ref array<EntityAI>	m_CompassInPossessionArr;
	
	void MapNavigationBehaviour(PlayerBase pPlayer, EMapNavigationType pNavigationType = EMapNavigationType.BASIC)
	{
		m_Player 					= pPlayer;
		m_NavigationType			= pNavigationType;
		
		m_GPSInPossessionArr 		= new array<EntityAI>();
		m_CompassInPossessionArr	= new array<EntityAI>();
	}
	
	protected void SetNavigationType(EMapNavigationType pType)
	{
		m_NavigationType = m_NavigationType | pType;
	}
	
	protected void UnsetNavigationType(EMapNavigationType pType)
	{
		m_NavigationType = m_NavigationType & ~pType;
	}

	EMapNavigationType GetNavigationType()
	{
		return m_NavigationType;
	}
	
	void OnItemInPlayerPossession(EntityAI item)
	{
		if (item.IsInherited(ItemGPS))
		{
			if (m_GPSInPossessionArr.Find(item) == INDEX_NOT_FOUND)
			{
				m_GPSInPossessionArr.Insert(item);
				SetNavigationType(EMapNavigationType.GPS);
			}
		}
		
		if (item.IsInherited(ItemCompass))
		{
			if (m_CompassInPossessionArr.Find(item) == INDEX_NOT_FOUND)
			{
				m_CompassInPossessionArr.Insert(item);
				SetNavigationType(EMapNavigationType.COMPASS);
			}
		}
	}
	
	void OnItemNotInPlayerPossession(EntityAI item)
	{
		if (item.IsInherited(ItemGPS))
		{
			m_GPSInPossessionArr.RemoveItem(item);
			if (m_GPSInPossessionArr.Count() == 0)
			{
				UnsetNavigationType(EMapNavigationType.GPS);
			}
		}
		
		if (item.IsInherited(ItemCompass))
		{
			m_CompassInPossessionArr.RemoveItem(item);
			if (m_CompassInPossessionArr.Count() == 0)
			{
				UnsetNavigationType(EMapNavigationType.COMPASS);
			}
		}
	}
	
	void RandomizePosition()
	{
		m_RandomPositionDeviationX 	= RandomizedDeviation();
		m_RandomPositionDeviationZ 	= RandomizedDeviation();
	}
	
	protected float RandomizedDeviation()
	{
		Math.Randomize(GetWorldTime() + Math.RandomIntInclusive(2, 4096));
		if ((Math.RandomIntInclusive(0, 10) % 2) == 0)
		{
			return Math.RandomIntInclusive(-RANDOM_DEVIATION_MAX, -RANDOM_DEVIATION_MIN);
		}
		else
		{
			return Math.RandomIntInclusive(RANDOM_DEVIATION_MIN, RANDOM_DEVIATION_MAX);
		}
	}
	
	vector GetPositionRandomized()
	{
		vector realPosition = m_Player.GetPosition();
		vector randomizedPosition = Vector(realPosition[0] + m_RandomPositionDeviationX, realPosition[1], realPosition[2] + m_RandomPositionDeviationZ);

		return randomizedPosition;
	}
	
	vector GetPositionReal()
	{
		return m_Player.GetPosition();
	}
	
	static array<int> OrderedPositionNumbersFromGridCoords(EntityAI pEntity)
	{
		float gridSize = GetGame().ConfigGetFloat(string.Format(GRID_SIZE_CFG_PATH, GetGame().GetWorldName()));
		int gridX, gridZ;
		GetGame().GetWorld().GetGridCoords(pEntity.GetPosition(), gridSize, gridX, gridZ);
		
		gridX = Math.AbsInt(gridX);
		gridZ = Math.AbsInt(gridZ);
		
		array<int> positions 	= new array<int>();
		string gridXStr 		= gridX.ToStringLen(DISPLAY_GRID_POS_MAX_CHARS_COUNT);
		string gridZStr 		= gridZ.ToStringLen(DISPLAY_GRID_POS_MAX_CHARS_COUNT);
		
		int i = 0;
		int gridCoordNumber;
		for (i = 0; i < gridXStr.Length(); ++i)
		{
			gridCoordNumber = gridXStr.Get(i).ToInt();
			if (IsOutOfMap(pEntity))
			{
				gridCoordNumber = -1;
			}

			positions.Insert(gridCoordNumber);
		}

		for (i = 0; i < gridZStr.Length(); ++i)
		{
			gridCoordNumber = gridZStr.Get(i).ToInt();
			if (IsOutOfMap(pEntity))
			{
				gridCoordNumber = -1;
			}

			positions.Insert(gridCoordNumber);
		}
		
		return positions;
	}
	
	static array<int> OrderedAltitudeNumbersPosition(EntityAI pEntity)
	{
		array<int> altArray = new array<int>();
		float altF 			= pEntity.GetPosition()[1];
		int altI 			= Math.Round(altF); 
		string altString 	= altI.ToStringLen(DISPLAY_ALT_MAX_CHARS_COUNT);
		
		for (int i = 0; i < altString.Length(); ++i)
		{
			altArray.Insert(altString.Get(i).ToInt());
		}
		
		return altArray;
	}
	
	static bool IsOutOfMap(EntityAI pEntity)
	{
		vector worldPos = pEntity.GetPosition();

		if (worldPos[0] < 0 || worldPos[0] > GetGame().GetWorld().GetWorldSize() || worldPos[2] < 0 || worldPos[2] > GetGame().GetWorld().GetWorldSize())
		{
			return true;
		}
		
		return false;
	}
}