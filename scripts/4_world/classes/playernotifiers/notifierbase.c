class NotifierBase
{
	float				m_DeltaT; // time in seconds since the last tick
	ref Timer 			m_Timer1; // timer which can be used for whatever
	PlayerBase			m_Player; //the player this Notifier belongs to
	int					m_Type;
	NotifiersManager	m_Manager;
	int 				m_TendencyBufferSize = 3;//for best results, this should be somewhat aligned with modifier frequency
	const int 			TENDENCY_BUFFER_SIZE = 30;//this needs to be bigger or same size as buffer size of any invidual buffer size
	bool				m_ShowTendency;
	bool				m_Active;
	int					m_TickInterval;
	int					m_TickIntervalLastTick;
	float				m_TendencyBuffer[TENDENCY_BUFFER_SIZE];
	int					m_TendencyBufferWriteIterator;
	float 				m_LastTendency;
	float				m_LastMA;
	bool				m_FirstPass = true;
	
	PluginPlayerStatus 	m_ModulePlayerStatus;
		
	void NotifierBase(NotifiersManager manager)
	{
		m_ModulePlayerStatus = PluginPlayerStatus.Cast(GetPlugin(PluginPlayerStatus));
		m_Active = true; 
		m_Manager = manager;
		m_Player = manager.GetPlayer();
		m_TickInterval = 1000;
		manager.RegisterItself(GetNotifierType(), this);
	}

	bool IsTimeToTick(int current_time)
	{
		int tickTime = (m_TickIntervalLastTick + m_TickInterval);
		bool tick = (current_time >= tickTime);
		#ifdef DIAG_NOTIFIER_LOGS
		ErrorEx(string.Format("Tick time for notifier %1 is %2 | Current time= %3.", this, tickTime, current_time), ErrorExSeverity.INFO);
		if (tick)
		{
			float timePassed = (current_time - m_TickIntervalLastTick) / 1000.0;
			float expectedInterval = m_TickInterval / 1000.0;
			float drift = (current_time - tickTime) / 1000.0;
			ErrorEx(string.Format("Notifier %1 updated after %2s (expected: %3s, drift: +%4s)", this, timePassed, expectedInterval, drift), ErrorExSeverity.INFO);
		}
		#endif
		return tick;
	}

	VirtualHud GetVirtualHud()
	{
		return m_Player.GetVirtualHud();
	}

	int GetNotifierType()
	{
		return m_Type;
	}

	string GetName()
	{
		return this.ClassName() + " Notifier";
	}

	bool IsActive()
	{
		return m_Active;
	}

	void SetActive(bool state)
	{
		m_Active = state;
		if (!state)
			HideBadge();		
	}

	void DisplayTendency(float delta);

	void AddToCyclicBuffer(float value)//for tendency
	{
		m_TendencyBuffer[m_TendencyBufferWriteIterator] = value;
		++m_TendencyBufferWriteIterator;
		if (m_TendencyBufferWriteIterator == m_TendencyBufferSize)
		{
			m_TendencyBufferWriteIterator = 0;
			m_ShowTendency = true;
		}
	}
	
	float ReadFromCyclicBuffer(int index)
	{
		int indx = m_TendencyBufferWriteIterator + index;
		if ( indx >= m_TendencyBufferSize)
		{
			indx = indx - m_TendencyBufferSize;
		}

		return m_TendencyBuffer[indx];
	}
	
	float GetDeltaAvaraged() //for tendency
	{
		array<float> values = new array<float>();
		for (int i = 0; i < m_TendencyBufferSize; ++i)
		{
			values.Insert(ReadFromCyclicBuffer(i));
		}
		
		float valuesSum = 0;

		int nValues = values.Count();
		for (i = 0; i < nValues; ++i)
		{
			valuesSum += values.Get(i);
		}
		
		float sma = valuesSum / m_TendencyBufferSize;
		if (m_FirstPass)
		{
			m_LastMA = sma;
			m_FirstPass = false;
		}
		
		float tnd = sma - m_LastMA;
		m_LastMA = sma;

		return tnd;
	}
	
	void SmoothOutFloatValues(array<float> values)
	{
		float value1;
		float value2;
		int nValues = values.Count();
		for (int i = 0; i < nValues - 1; i++)
		{
			value1 = values.Get(i);
			value2 = values.Get(i + 1);
			float average = (value1 + value2) / 2;
			values.Set(i, average);
			values.Set(i + 1, average);
		}

		int index = nValues - 1;
		values.Set(index, value2);
	}

	void OnTick(float current_Time)
	{
		OnTick((int)current_Time);
	}
	
	void OnTick(int currentTime)
	{
		#ifdef DIAG_NOTIFIER_LOGS
		ErrorEx(string.Format("Set last tick interval time on notifier %1 | Time= %2.", m_TickIntervalLastTick, currentTime), ErrorExSeverity.INFO);
		#endif	
		m_TickIntervalLastTick = currentTime;
		
		DisplayBadge();
		AddToCyclicBuffer(GetObservedValue());

		if (m_ShowTendency)
			DisplayTendency(GetDeltaAvaraged());
	}
	
	protected int CalculateTendency(float delta, float inctresholdlow, float inctresholdmed, float inctresholdhigh, float dectresholdlow, float dectresholdmed, float dectresholdhigh)
	{                                        	
		int tendency = TENDENCY_STABLE;
		if (delta > inctresholdhigh)
			tendency = TENDENCY_INC_HIGH;	
		else if (delta > inctresholdmed)
			tendency = TENDENCY_INC_MED;
		else if (delta > inctresholdlow)
			tendency = TENDENCY_INC_LOW;
		else if (delta < dectresholdhigh)
			tendency = TENDENCY_DEC_HIGH;		
		else if (delta < dectresholdmed)
			tendency = TENDENCY_DEC_MED;
		else if (delta < dectresholdlow)
			tendency = TENDENCY_DEC_LOW;

		return tendency;
	}
	
	
	protected eBadgeLevel DetermineBadgeLevel(float value, float lvl_1, float lvl_2, float lvl_3)
	{
		eBadgeLevel level = eBadgeLevel.NONE;
		if (value >= lvl_3)
			level = eBadgeLevel.THIRD;
		else if (value >= lvl_2)
			level = eBadgeLevel.SECOND;
		else if (value >= lvl_1)
			level = eBadgeLevel.FIRST;

		return 	level;
	}
	

	protected void DisplayBadge();
	protected void HideBadge();

	protected float GetObservedValue()
	{
		return 0.0;
	}
}
