#ifdef DIAG_DEVELOPER
class GeyserDebugData
{
	bool m_DiagEnabled;
	float m_DiagTimeInterval;
	float m_DiagTimeDuration;
	
	void SetData(bool enabled, float timeInterval, float timeDuration)
	{
		m_DiagEnabled = enabled;
		m_DiagTimeInterval = timeInterval;
		m_DiagTimeDuration = timeDuration;
	}
}
#endif