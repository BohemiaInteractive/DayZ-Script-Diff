enum RandomGeneratorSyncUsage
{
	RGSRecoil,
	RGSJam,
	RGSGeneric,
	RGSAimingModel,
	Count,
}

class RandomGeneratorSyncManager
{
	const int USAGE_COUNT = RandomGeneratorSyncUsage.Count;
	
	float randomValuesArray[USAGE_COUNT];
	DayZPlayer m_player;
	
	void RandomGeneratorSyncManager(DayZPlayer player)
	{
		m_player = player;
	}

	void Update()
	{
		for(int i = 0; i < USAGE_COUNT; i++ )
		{
			randomValuesArray[i] = m_player.Random01();
		}
	}	
	
	
	float GetRandom01(RandomGeneratorSyncUsage usage_index)
	{
		if(usage_index < USAGE_COUNT)
		{
			return randomValuesArray[usage_index];
		}
		return -1;
	}
	
	float GetRandomInRange(RandomGeneratorSyncUsage usage_index, float min, float max)
	{
		return GetRandom01(usage_index)*(max - min) + min;
	}
}