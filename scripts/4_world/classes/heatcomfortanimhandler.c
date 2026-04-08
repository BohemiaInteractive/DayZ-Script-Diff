class HeatComfortAnimHandler
{
	const float TICK_INTERVAL  = 2;
	
	float m_TimeSinceLastTick;
	float m_ProcessTimeAccuFreeze;
	float m_ProcessTimeAccuFreezeRattle;
	float m_ProcessTimeAccuHot;
	
	PlayerBase m_Player;
	
	float m_EventTimeFreezeRattle = -1;			// -1 = symptom effect needs to be triggered for first time
	float m_EventTimeFreeze = -1;			
	float m_EventTimeHot = -1;	
	
	protected ref HumanMovementState m_MovementState = new HumanMovementState();
	
	const float TIME_INTERVAL_HC_MINUS_LOW_MIN 	= 12; 	// Freezing and rattle symptoms interval minimum, lerped according to heat confort
	const float TIME_INTERVAL_HC_MINUS_HIGH_MIN = 25; 
	
	const float TIME_INTERVAL_HC_MINUS_LOW_MAX 	= 20;	// Interval maximum, lerped according to heat confort
	const float TIME_INTERVAL_HC_MINUS_HIGH_MAX = 40;
	
	const float TIME_INTERVAL_HC_PLUS_LOW_MIN 	= 12; 	// Ditto for heat symptoms
	const float TIME_INTERVAL_HC_PLUS_HIGH_MIN 	= 25;
	
	const float TIME_INTERVAL_HC_PLUS_LOW_MAX 	= 20;
	const float TIME_INTERVAL_HC_PLUS_HIGH_MAX 	= 40;
	
	void HeatComfortAnimHandler(PlayerBase player)
	{
		m_Player = player;
	}

	void Update(float delta_time, HumanMovementState hms)
	{
		m_TimeSinceLastTick += delta_time;

		if (m_TimeSinceLastTick > TICK_INTERVAL)
		{
			if (g_Game.IsServer())
				Process(m_TimeSinceLastTick);
			
			m_TimeSinceLastTick = 0;
		}
	}

	void Process(float delta_time)
	{
		float hc = m_Player.GetStatHeatComfort().Get();

		if (hc >= PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_WARNING)			// Yellow zone
		{
			m_ProcessTimeAccuHot += delta_time;
			
			if (m_EventTimeHot < 0 || m_ProcessTimeAccuHot >= m_EventTimeHot)
				ProcessHot(hc);
		}
		else if (Math.IsInRange(hc, PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING, PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_WARNING))
		{
			// White zone, reset the timers
			
			if (m_EventTimeHot > -1 && hc <= 0)
				m_EventTimeHot = -1;
			
			if (m_EventTimeFreeze > -1 && hc >= 0)
				m_EventTimeFreeze = -1;
			
			if (m_EventTimeFreezeRattle > -1)
				m_EventTimeFreezeRattle = -1;
	
		}
		else if (hc <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING)	// Light blue zone
		{
			if (hc <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL)	// Deep blue blinking zone, rattling is layerd on top of freezing
			{
				m_ProcessTimeAccuFreezeRattle += delta_time;
				
				if (m_EventTimeFreezeRattle < 0 || m_ProcessTimeAccuFreezeRattle >= m_EventTimeFreezeRattle)
					ProcessFreezeRattle(hc);	
			}
			
			m_ProcessTimeAccuFreeze += delta_time;
			
			if (m_EventTimeFreeze < 0 || m_ProcessTimeAccuFreeze >= m_EventTimeFreeze)	
				ProcessFreeze(hc);	
		}
		
	//	Debug.Log("HeatComfort: " + hc);
	}

	private void ProcessHot(float hcValue)
	{
		m_Player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_HOT);		
		m_EventTimeHot = GetEventTime(hcValue, PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_EMPTY, PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_WARNING, TIME_INTERVAL_HC_PLUS_LOW_MIN, TIME_INTERVAL_HC_PLUS_HIGH_MIN, TIME_INTERVAL_HC_PLUS_LOW_MAX, TIME_INTERVAL_HC_PLUS_HIGH_MAX);
		m_ProcessTimeAccuHot = 0;
		
	//	Debug.Log("HeatComfort: " + hcValue + ", HOT in: " + m_EventTimeHot + "s");	
	}
	
	private void ProcessFreeze(float hcValue)
	{
		m_Player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_FREEZE);	
		m_EventTimeFreeze = GetEventTime(hcValue, PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_EMPTY, PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING, TIME_INTERVAL_HC_MINUS_LOW_MIN, TIME_INTERVAL_HC_MINUS_HIGH_MIN, TIME_INTERVAL_HC_MINUS_LOW_MAX, TIME_INTERVAL_HC_MINUS_HIGH_MAX);
		m_ProcessTimeAccuFreeze = 0;	
	
	//	Debug.Log("HeatComfort: " + hcValue + ", FREEZE in: " + m_EventTimeFreeze + "s");
	}
	
	private void ProcessFreezeRattle(float hcValue)
	{
		m_Player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_FREEZE_RATTLE);
		m_EventTimeFreezeRattle = GetEventTime(hcValue, -1, PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL, TIME_INTERVAL_HC_MINUS_LOW_MIN, TIME_INTERVAL_HC_MINUS_HIGH_MIN, TIME_INTERVAL_HC_MINUS_LOW_MAX, TIME_INTERVAL_HC_MINUS_HIGH_MAX);
		m_ProcessTimeAccuFreezeRattle = 0;	
		
	//	Debug.Log("HeatComfort: " + hcValue + ", RATTLE in: " + m_EventTimeFreezeRattle + "s");
	}

	private float GetEventTime(float hcValue, float threshold_low, float thresholdHigh, float lowMin, float highMin, float lowMax, float highMax)
	{
		float inv_value = Math.Clamp(Math.InverseLerp(threshold_low, thresholdHigh, hcValue), 0, 1);
		float valueMin = Math.Lerp(lowMin, highMin, inv_value);
		float value_max = Math.Lerp(lowMax, highMax, inv_value);
		
		return Math.RandomFloatInclusive(valueMin, value_max);
	}
}