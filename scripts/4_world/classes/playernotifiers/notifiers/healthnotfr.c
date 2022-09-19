class HealthNotfr: NotifierBase
{
	/*
	private const float	 	HEALTHY_BLOOD_TRESHOLD	= 5000;
	private const float	 	HEALTHY_TRESHOLD		= 5000;
	private const float	 	HEALING_ENERGY_TRESHOLD	= 4000;
	private const float	 	HEALING_WATER_TRESHOLD	= 2500;
	private const float	 	HEALING_BLOOD_TRESHOLD	= 5000;
	*/
	private const float 	DEC_TRESHOLD_LOW 			= 0;
	private const float 	DEC_TRESHOLD_MED 			= -0.7;
	private const float 	DEC_TRESHOLD_HIGH			= -1.3;
	private const float 	INC_TRESHOLD_LOW 			= 0;
	private const float 	INC_TRESHOLD_MED 			= 0.7;
	private const float 	INC_TRESHOLD_HIGH			= 1.3;
	
		
	void HealthNotfr(NotifiersManager manager)
	{
		m_TickInterval = 3000;
		m_TendencyBufferSize = 6;
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_HEALTHY;
	}

	override void DisplayBadge()
	{
	}
	
	override void DisplayTendency(float delta)
	{
		int tendency = CalculateTendency(delta, INC_TRESHOLD_LOW, INC_TRESHOLD_MED, INC_TRESHOLD_HIGH, DEC_TRESHOLD_LOW, DEC_TRESHOLD_MED, DEC_TRESHOLD_HIGH);
		//PrintString("tendency:" + tendency);
		//GetVirtualHud().SetStatus(eDisplayElements.DELM_TDCY_HEALTH,tendency);
		
		//DSLevels level = DetermineLevel( GetObservedValue(), PlayerConstants.THRESHOLD_HEALTH_WARNING, PlayerConstants.THRESHOLD_HEALTH_CRITICAL, PlayerConstants.THRESHOLD_HEALTH_BLINKING, PlayerConstants.THRESHOLD_HEALTH_EXTRA);
		
		EStatLevels health_level = m_Player.GetStatLevelHealth();
		DisplayElementTendency dis_elm = DisplayElementTendency.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_TDCY_HEALTH));
		
		if( dis_elm )
		{
			dis_elm.SetSeriousnessLevel(health_level);
			dis_elm.SetTendency(tendency);
		}
	}

	override void HideBadge()
	{
		
		////GetVirtualHud().SetStatus(eDisplayElements.DELM_NTFR_HEALTHY,DELM_LVL_0);
	}

	override protected float GetObservedValue()
	{
		float health = m_Player.GetHealth("","Health");
		//PrintString("health:" + health);
		return health;
	}
	

};