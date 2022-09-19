class KuruShake
{
	const float RANDOM_RANGE_POINT = 1;
	const float RANDOM_RANGE_DEVIATION = 0.50;
	const float SHAKE_DURATION_PHASE1 = 0.01;
	const float SHAKE_STRENGTH_MIN = 1;
	const float SHAKE_STRENGTH_MAX = 5;
	
	PlayerBase m_Player;
	float m_Time;
	float m_Time2;
	float m_RelativeTime;
	float m_RelativeTime2;
	float m_ShakeStrength;
	
	
	ref array<vector> m_Curve = new array<vector>;
	
	void KuruShake(PlayerBase player, float amount)
	{
		
		vector pos1;
		int seed = Math.Randomize(player.GetRandomGeneratorSyncManager().GetRandomInRange(RandomGeneratorSyncUsage.RGSGeneric,0,10000));
		Math.RandomFloat(-RANDOM_RANGE_POINT,RANDOM_RANGE_POINT);//throw away
		m_ShakeStrength = Math.Lerp(SHAKE_STRENGTH_MIN, SHAKE_STRENGTH_MAX, amount);
		pos1[0] = Math.RandomFloat(-RANDOM_RANGE_POINT,RANDOM_RANGE_POINT);
		pos1[1] = Math.RandomFloat(-RANDOM_RANGE_POINT,RANDOM_RANGE_POINT);
		pos1.Normalize();
		pos1 = pos1 * m_ShakeStrength;
		
		vector pos2;
		pos2[0] = pos1[0] + Math.RandomFloat(-RANDOM_RANGE_DEVIATION,RANDOM_RANGE_DEVIATION) + pos1[0];
		pos2[1] = pos1[1] + Math.RandomFloat(-RANDOM_RANGE_DEVIATION,RANDOM_RANGE_DEVIATION) + pos1[1];
		m_Curve.Insert("0 0 0");
		m_Curve.Insert(pos1);
		m_Curve.Insert(pos2);
		
		/*
		Print("==========================");
		for(int i = 0; i < m_Curve.Count(); i++)
		{
			Print(m_Curve[i]);
		}
		Print("==========================");
		*/
	}
	
	void Update(float delta_time, out float pos_x, out float pos_y )
	{
		if(m_RelativeTime < 0.99)
		{
			m_Time += delta_time;
			m_RelativeTime = Math.InverseLerp(0, SHAKE_DURATION_PHASE1, m_Time);
			//m_RelativeTime = m_RelativeTime - 0.01;
			m_RelativeTime = Easing.EaseInQuint(m_RelativeTime);
			vector pos = Math3D.Curve(ECurveType.CatmullRom, m_RelativeTime, m_Curve);
			pos_x = pos[0];
			pos_y = pos[1];
		}
		else
		{
			m_Time2 += delta_time * 1;
			m_RelativeTime2 = 1 -  m_Time2;
			m_RelativeTime2 = Easing.EaseInQuint(m_RelativeTime2);
			vector pos2 = Math3D.Curve(ECurveType.CatmullRom, m_RelativeTime2, m_Curve);
			pos_x = pos2[0];
			pos_y = pos2[1];
		}
		
		if(m_Time2 >= 0.99)
		{
			delete this;
		}
	}

	
			
}