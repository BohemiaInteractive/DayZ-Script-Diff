typedef Param3<bool, float, int> TimeAccelParam;//enabled, timeAccel,category mask

enum ETimeAccelCategories
{
	UNDERGROUND_ENTRANCE 	= 0x00000001,
	UNDERGROUND_RESERVOIR 	= 0x00000002,
	ENERGY_CONSUMPTION 		= 0x00000004,
	ENERGY_RECHARGE 		= 0x00000008,
	//SYSTEM4 = 0x00000010,
	//SYSTEM4 = 0x00000020,
}

class FeatureTimeAccel
{
	static ref TimeAccelParam 	m_CurrentTimeAccel = new TimeAccelParam(false, 0, 0);
	
	static bool GetFeatureTimeAccelEnabled(ETimeAccelCategories categoryBit)
	{
		return (m_CurrentTimeAccel.param1 && (categoryBit & m_CurrentTimeAccel.param3) != 0);
	}
	
	static float GetFeatureTimeAccelValue()
	{
		return m_CurrentTimeAccel.param2;
	}
}