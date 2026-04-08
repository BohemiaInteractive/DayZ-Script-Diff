class ThirstNotfr: NotifierBase
{
	private const float 	HYDRATED_TRESHOLD 			= 3500;
	private const float 	THIRSTY_TRESHOLD 			= 2500;
	private const float 	VERY_THIRSTY_TRESHOLD 		= 1500;
	private const float 	FATALLY_THIRSTY_TRESHOLD 	= 0;
	private const float 	DEC_TRESHOLD_LOW 			= 0;
	private const float 	DEC_TRESHOLD_MED 			= -0.2;
	private const float 	DEC_TRESHOLD_HIGH			= -0.85;
	private const float 	INC_TRESHOLD_LOW 			= 0;
	private const float 	INC_TRESHOLD_MED 			= 0.2;
	private const float 	INC_TRESHOLD_HIGH			= 0.85;

	override int GetNotifierType()
	{
		return eNotifiers.NTF_THIRSTY;
	}
	
	override void DisplayTendency(float delta)
	{
		//PrintString(delta.ToString());
		int tendency = CalculateTendency(delta, INC_TRESHOLD_LOW, INC_TRESHOLD_MED, INC_TRESHOLD_HIGH, DEC_TRESHOLD_LOW, DEC_TRESHOLD_MED, DEC_TRESHOLD_HIGH);
		//GetVirtualHud().SetStatus(eDisplayElements.DELM_TDCY_WATER,tendency);
		
		//DSLevels level = DetermineLevel( GetObservedValue(), PlayerConstants.THRESHOLD_WATER_WARNING, PlayerConstants.THRESHOLD_WATER_CRITICAL, PlayerConstants.THRESHOLD_WATER_BLINKING, PlayerConstants.THRESHOLD_WATER_EXTRA);
		EStatLevels water_level = m_Player.GetStatLevelWater();
		DisplayElementTendency dis_elm = DisplayElementTendency.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_TDCY_WATER));
		
		if( dis_elm )
		{
			dis_elm.SetTendency(tendency);
			dis_elm.SetSeriousnessLevel(water_level);		
		}
	}
	
	override protected float GetObservedValue()
	{
		return m_Player.GetStatWater().Get();
	}
};