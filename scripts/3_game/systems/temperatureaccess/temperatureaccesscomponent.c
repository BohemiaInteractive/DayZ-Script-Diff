class TemperatureAccessComponent
{
	protected EntityAI m_Owner;
	protected TemperatureAccessTypeBase m_AccessDataCurrent;
	protected float m_LastAccessTime;
	
	void TemperatureAccessComponent(EntityAI owner)
	{
		m_Owner = owner;
	}
	
	bool TryAccessSource(TemperatureData data)
	{
		if (PriorityCheck(data) || CheckAccessTimeout(data))
		{
			m_AccessDataCurrent = TemperatureAccessManager.GetTAData(data.m_AccessType);
			m_LastAccessTime = g_Game.GetTickTime();
			return true;
		}
		
		return false;
	}
	
	protected bool PriorityCheck(TemperatureData data)
	{
		TemperatureAccessTypeBase dataNew = TemperatureAccessManager.GetTAData(data.m_AccessType);
		return !m_AccessDataCurrent || dataNew.GetPriority() >= m_AccessDataCurrent.GetPriority();
	}
	
	protected bool CheckAccessTimeout(TemperatureData data)
	{
		float timeCurrent = g_Game.GetTickTime();
		return (timeCurrent - m_LastAccessTime) > data.m_UpdateTimeInfo;
	}
}
