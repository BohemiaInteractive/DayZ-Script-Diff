#ifdef DIAG_DEVELOPER
typedef Param3<bool, float, int> TimeAccelParam;//enabled, timeAccel,category mask

enum ETimeAccelCategories
{
	//DO NOT FORGET TO INCLUDE IN THE MAPPING 'Init()' FUNCTION
	UNDERGROUND_ENTRANCE 	= 0x00000001,
	UNDERGROUND_RESERVOIR 	= 0x00000002,
	ENERGY_CONSUMPTION 		= 0x00000004,
	ENERGY_RECHARGE 		= 0x00000008,
	FOOD_DECAY 				= 0x00000010,
	//SYSTEM5 = 0x00000020,
	//SYSTEM6 = 0x00000040,
	//SYSTEM6 = 0x00000080,
}

class FeatureTimeAccel
{
	static ref TimeAccelParam 		m_CurrentTimeAccel;// 	= new TimeAccelParam(false, 0, 0);
	static ref map<int,int> 		m_Mapping 			= new map<int,int>();
	static ref array<int> 			m_IDs				= new array<int>();
	static bool 					m_Initializeded 	= Init();
	
	static bool Init()
	{
		Bind(DiagMenuIDs.FEATURE_TIME_ACCEL_UG_ENTRANCES, 		ETimeAccelCategories.UNDERGROUND_ENTRANCE);
		Bind(DiagMenuIDs.FEATURE_TIME_ACCEL_UG_RESERVOIR, 		ETimeAccelCategories.UNDERGROUND_RESERVOIR);
		Bind(DiagMenuIDs.FEATURE_TIME_ACCEL_ENERGY_CONSUME, 	ETimeAccelCategories.ENERGY_CONSUMPTION);
		Bind(DiagMenuIDs.FEATURE_TIME_ACCEL_ENERGY_RECHARGE, 	ETimeAccelCategories.ENERGY_RECHARGE);
		Bind(DiagMenuIDs.FEATURE_TIME_ACCEL_FOOD_DECAY, 		ETimeAccelCategories.FOOD_DECAY);
		return true;
	}
	
	//-------------------------------
	
	static void Bind(DiagMenuIDs id, ETimeAccelCategories catBit)
	{
		m_Mapping.Insert(id, catBit);
		m_IDs.Insert(id);
	}
	
	//-------------------------------
	
	static int GetCategoryByDiagID(DiagMenuIDs id)
	{
		return m_Mapping.Get(id);
	}
	
	//-------------------------------
	
	static int GetDiagIDByCategory(ETimeAccelCategories category)
	{
		for (int i = 0; i < m_Mapping.Count();i++)
		{
			if (m_Mapping.GetElement(i) == category)
			{
				return m_Mapping.GetKey(i);
			}
		}
		return -1;
	}
	
	//-------------------------------
	
	static bool GetFeatureTimeAccelEnabled(ETimeAccelCategories categoryBit)
	{
		return (m_CurrentTimeAccel && m_CurrentTimeAccel.param1 && (categoryBit & m_CurrentTimeAccel.param3) != 0);
	}
	
	//-------------------------------
	
	static float GetFeatureTimeAccelValue()
	{
		if (m_CurrentTimeAccel)
			return m_CurrentTimeAccel.param2;
		else return 1;
	}
	
	//-------------------------------
	
	static void CopyTimeAccelClipboard(bool enable, int timeAccelBig, float timeAccelSmall, int bitMask)
	{
		string output = "-timeAccel=";
		int val = enable;
		output += val.ToString()+","+timeAccelBig.ToString()+","+timeAccelSmall.ToString()+","+bitMask.ToString();
		GetGame().CopyToClipboard(output);
	}
	
	//-------------------------------
	
	static int GetTimeAccelBitmask()
	{
		int bitmask = 0;
		
		foreach (int id : m_IDs)
		{
			WriteCategoryBit(bitmask, id);	
		}

		return bitmask;
	}
	
	//-------------------------------
	
	static void WriteCategoryBit(out int bitmask, int diagMenuID)
	{
		int bit = GetCategoryByDiagID(diagMenuID);
		if (DiagMenu.GetValue(diagMenuID))
		{
			bitmask = bitmask | bit;
		}
	}
	
	//-------------------------------
	
	static bool AreTimeAccelParamsSame(TimeAccelParam p1, TimeAccelParam p2)
	{
		if (!p1 || !p2)
			return false;
		if (p1.param1 != p2.param1)
			return false;
		if (p1.param2 != p2.param2)
			return false;
		if (p1.param3 != p2.param3)
			return false;
		return true;
	}
	
	//-------------------------------
	
	static void SendTimeAccel(Man player, TimeAccelParam param)
	{
		GetGame().RPCSingleParam(player, ERPCs.DIAG_TIMEACCEL, param, true, player.GetIdentity());
	}
}
#endif