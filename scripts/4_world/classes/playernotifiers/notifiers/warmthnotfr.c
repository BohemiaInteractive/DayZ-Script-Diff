class WarmthNotfr: NotifierBase
{
	private const float 	DEC_TRESHOLD_LOW 			= 0;
	private const float 	DEC_TRESHOLD_MED 			= -0.2;
	private const float 	DEC_TRESHOLD_HIGH			= -0.3;
	private const float 	INC_TRESHOLD_LOW 			= 0;
	private const float 	INC_TRESHOLD_MED 			= 0.2;
	private const float 	INC_TRESHOLD_HIGH			= 0.3;
		
	void WarmthNotfr(NotifiersManager manager)
	{
		m_TendencyBufferSize = 6;
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_WARMTH;
	}
	
	
	protected DSLevelsTemp DetermineLevel(float value, float m_warning_treshold, float m_critical_treshold, float m_empty_treshold, float p_warning_treshold, float p_critical_treshold, float p_empty_treshold )
	{
		DSLevelsTemp level = DSLevelsTemp.NORMAL;
		
		if(value < 0)
		{
			if(value < m_warning_treshold) level = DSLevelsTemp.WARNING_MINUS;
			if(value < m_critical_treshold) level = DSLevelsTemp.CRITICAL_MINUS;
			if(value <= m_empty_treshold) level = DSLevelsTemp.BLINKING_MINUS;
		}
		else
		{
			if(value > p_warning_treshold) level = DSLevelsTemp.WARNING_PLUS;
			if(value > p_critical_treshold) level = DSLevelsTemp.CRITICAL_PLUS;
			if(value >= p_empty_treshold) level = DSLevelsTemp.BLINKING_PLUS;
		}

		return level;
	}

	override void DisplayTendency(float delta)
	{
		int tendency = CalculateTendency(delta, INC_TRESHOLD_LOW, INC_TRESHOLD_MED, INC_TRESHOLD_HIGH, DEC_TRESHOLD_LOW, DEC_TRESHOLD_MED, DEC_TRESHOLD_HIGH);
		
		DSLevelsTemp level = DetermineLevel( GetObservedValue(), PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING,  PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL, PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_EMPTY, PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_WARNING,  PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_CRITICAL, PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_EMPTY );
		
		DisplayElementTendency dis_elm = DisplayElementTendency.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_TDCY_TEMPERATURE));
		
		if( dis_elm )
		{
			dis_elm.SetTendency(tendency);
			dis_elm.SetSeriousnessLevel(level);
		}
	}

	override protected float GetObservedValue()
	{
		return m_Player.GetStatHeatComfort().Get();
	}
};