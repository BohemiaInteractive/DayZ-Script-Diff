class WarmthNotfr: NotifierBase
{
	private const float DEC_TRESHOLD_LOW 	= -0.01;
	private const float DEC_TRESHOLD_MED 	= -0.12;
	private const float DEC_TRESHOLD_HIGH	= -0.25;
	private const float INC_TRESHOLD_LOW 	= 0.01;
	private const float INC_TRESHOLD_MED 	= 0.12;
	private const float INC_TRESHOLD_HIGH	= 0.25;
	
	protected ref SimpleMovingAverage<float> m_AverageHeatComfortDeltaBuffer; //! replaces the original delta of values from NotifierBase
		
	void WarmthNotfr(NotifiersManager manager)
	{
		m_AverageHeatComfortDeltaBuffer = new SimpleMovingAverage<float>(6, 0.0);
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_WARMTH;
	}	
	
	protected DSLevelsTemp DetermineLevel(float value, float m_warning_treshold, float m_critical_treshold, float m_empty_treshold, float p_warning_treshold, float p_critical_treshold, float p_empty_treshold )
	{
		DSLevelsTemp level = DSLevelsTemp.NORMAL;
		
		if (value < 0)
		{
			if (value < m_warning_treshold)
				level = DSLevelsTemp.WARNING_MINUS;
			if (value < m_critical_treshold)
				level = DSLevelsTemp.CRITICAL_MINUS;
			if (value <= m_empty_treshold)
				level = DSLevelsTemp.BLINKING_MINUS;
		}
		else
		{
			if (value > p_warning_treshold)
				level = DSLevelsTemp.WARNING_PLUS;
			if (value > p_critical_treshold)
				level = DSLevelsTemp.CRITICAL_PLUS;
			if (value >= p_empty_treshold)
				level = DSLevelsTemp.BLINKING_PLUS;
		}

		return level;
	}

	override void DisplayTendency(float delta)
	{
		int tendency = CalculateTendency(
			GetObservedValue(),
			INC_TRESHOLD_LOW,
			INC_TRESHOLD_MED,
			INC_TRESHOLD_HIGH,
			DEC_TRESHOLD_LOW,
			DEC_TRESHOLD_MED,
			DEC_TRESHOLD_HIGH,
		);
		
		DSLevelsTemp level = DetermineLevel(
			m_Player.GetStatHeatComfort().Get(),
			PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING, 
			PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL,
			PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_EMPTY,
			PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_WARNING,
			PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_CRITICAL,
			PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_EMPTY,
		);
		
		DisplayElementTendency displayElement = DisplayElementTendency.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_TDCY_TEMPERATURE));
		
		if (displayElement)
		{
			displayElement.SetTendency(tendency);
			displayElement.SetSeriousnessLevel(level);
		}
	}

	override protected float GetObservedValue()
	{
		return m_AverageHeatComfortDeltaBuffer.Add(m_Player.m_Environment.GetTargetHeatComfort() - m_Player.GetStatHeatComfort().Get());
	}
}
