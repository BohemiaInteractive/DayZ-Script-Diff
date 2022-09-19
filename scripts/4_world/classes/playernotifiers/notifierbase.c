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
	//private float		m_SecsToSound; //internal counter
	//protected float		m_MinPauseBetweenSounds; //minimal amount of seconds that needs to pass till sound is played
	//protected float		m_MaxPauseBetweenSounds; //maximal amount of seconds that can pass till sound is played
	//private float		m_SecsSinceLastAnimation; //internal counter
	//private float		m_SecsToAnimation; //internal counter
	//protected float		m_MinPauseBetweenAnimations; //minimal amount of seconds that needs to pass till animation played
	//protected float		m_MaxPauseBetweenAnimations; //maximal amount of seconds that can pass till animation is splayed
	int					m_TickInterval;
	int					m_TickIntervalLastTick;
	float				m_TendencyBuffer[TENDENCY_BUFFER_SIZE];
	int					m_TendencyBufferWriteIterator;
	float 				m_LastTendency;
	float				m_LastMA;
	bool				m_FirstPass = true;
	//int					m_TendencyID;
	
	PluginPlayerStatus 	m_ModulePlayerStatus;
		
	void NotifierBase(NotifiersManager manager)
	{

		//m_Timer1 = new Timer();
		m_ModulePlayerStatus = PluginPlayerStatus.Cast(GetPlugin(PluginPlayerStatus));
		m_Active = true; 
		m_Manager = manager;
		m_Player = manager.GetPlayer();
		m_TickInterval = 1000;
		manager.RegisterItself(GetNotifierType(), this);
	}

	bool IsTimeToTick(int current_time)
	{
		return ( current_time > m_TickIntervalLastTick + m_TickInterval );
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

	void SetActive( bool state )
	{
		m_Active = state;
		if ( !state ) HideBadge();
	}

	void DisplayTendency(float delta)
	{
	}

	void AddToCyclicBuffer(float value)//for tendency
	{
		m_TendencyBuffer[m_TendencyBufferWriteIterator] = value;
		m_TendencyBufferWriteIterator++;
		if( m_TendencyBufferWriteIterator == m_TendencyBufferSize )
		{
			m_TendencyBufferWriteIterator = 0;
			m_ShowTendency = true;
		}
	}
	
	float ReadFromCyclicBuffer(int index)
	{
		int indx = m_TendencyBufferWriteIterator + index;
		if( indx >= m_TendencyBufferSize)
		{
			indx = indx - m_TendencyBufferSize;
		}
		return m_TendencyBuffer[indx];
	}
	
	float GetDeltaAvaraged()//for tendency
	{
		array<float> values = new array<float>;
		for(int i = 0; i < m_TendencyBufferSize; i++)
		{
			values.Insert(ReadFromCyclicBuffer(i));
		}
		//SmoothOutFloatValues(values);
		
		//--------------------------------------------------------------------------
		
		float values_sum = 0;
		//PrintString("----------------------------");
		//PrintString("tendency:" + this.ClassName() );
		for(i = 0; i < values.Count(); i++)
		{
			float value = values.Get(i);
			values_sum += value;
			//PrintString( value.ToString() );
		}
		
		float sma = values_sum / m_TendencyBufferSize;
		if( m_FirstPass ) 
		{
			m_LastMA = sma;
			m_FirstPass = false;
		}
		
		float tnd = sma - m_LastMA;
		m_LastMA = sma;
		/*
		PrintString(GetName()+" tendency:" + tnd.ToString() );
		PrintString("----------------------------");
		*/
		return tnd;
		
		//--------------------------------------------------------------------------
		
	}
	
	void SmoothOutFloatValues(array<float> values)
	{
		float value1;
		float value2;
		for(int i = 0; i < values.Count() - 1; i++)
		{
			value1 = values.Get(i);
			value2 = values.Get(i + 1);
			float average = (value1 + value2) / 2;
			values.Set(i, average);
			values.Set(i + 1, average);
		}
		int index = values.Count() - 1;
		values.Set(index, value2);
	}

	void OnTick(float current_time)
	{
		//------------
		/*
		float time = GetGame().GetTime();
		Print("ticking notifier "+ this.ClassName() +" for player " + m_Player.ToString() + " at time:" + time.ToString());
		*/
		//------------
		m_TickIntervalLastTick = current_time;
		DisplayBadge();
		AddToCyclicBuffer( GetObservedValue() );
		if (m_ShowTendency) DisplayTendency( GetDeltaAvaraged() );

	}
	
	protected int CalculateTendency(float delta, float inctresholdlow, float inctresholdmed, float inctresholdhigh, float dectresholdlow, float dectresholdmed, float dectresholdhigh)
	{                                        	
		int tendency = TENDENCY_STABLE;
		if ( delta > inctresholdlow ) 	tendency = TENDENCY_INC_LOW;
		if ( delta > inctresholdmed )  	tendency = TENDENCY_INC_MED;
		if ( delta > inctresholdhigh ) 	tendency = TENDENCY_INC_HIGH;	
		if ( delta < dectresholdlow )  	tendency = TENDENCY_DEC_LOW;
		if ( delta < dectresholdmed )  	tendency = TENDENCY_DEC_MED;
		if ( delta < dectresholdhigh ) 	tendency = TENDENCY_DEC_HIGH;
		//Print(" " + delta + " tendency:  " + tendency +" " + this);
		return tendency;
	}
	
	
	protected eBadgeLevel DetermineBadgeLevel(float value, float lvl_1, float lvl_2, float lvl_3)
	{
		eBadgeLevel level = eBadgeLevel.NONE;
		if ( value > lvl_1 ) 	level = eBadgeLevel.FIRST;
		if ( value > lvl_2 )  	level = eBadgeLevel.SECOND;
		if ( value > lvl_3 ) 	level = eBadgeLevel.THIRD;
		return 	level;
	}
	

	protected void DisplayBadge()
	{
	}

	protected void HideBadge()
	{
	}

	protected float GetObservedValue()
	{
		return 0;
	}
}