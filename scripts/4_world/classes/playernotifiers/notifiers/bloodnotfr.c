class BloodNotfr: NotifierBase
{
	protected const float 	DEC_TRESHOLD_LOW 	= 0;
	protected const float 	DEC_TRESHOLD_MED 	= -12;
	protected const float 	DEC_TRESHOLD_HIGH	= -22;
	protected const float 	INC_TRESHOLD_LOW 	= 0;
	protected const float 	INC_TRESHOLD_MED 	= 3;
	protected const float 	INC_TRESHOLD_HIGH	= 6;

	void BloodNotfr(NotifiersManager manager)
	{
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_BLOOD;
	}

	override void DisplayTendency(float delta)
	{
		int tendency = CalculateTendency(delta, INC_TRESHOLD_LOW, INC_TRESHOLD_MED, INC_TRESHOLD_HIGH, DEC_TRESHOLD_LOW, DEC_TRESHOLD_MED, DEC_TRESHOLD_HIGH);
		
		//DSLevels level = DetermineLevel( GetObservedValue(), PlayerConstants.THRESHOLD_BLOOD_WARNING,  PlayerConstants.THRESHOLD_BLOOD_CRITICAL, PlayerConstants.THRESHOLD_BLOOD_BLINKING, PlayerConstants.THRESHOLD_BLOOD_EXTRA );
		DisplayElementTendency dis_elm = DisplayElementTendency.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_TDCY_BLOOD));
		
		EStatLevels blood_level = m_Player.GetStatLevelBlood();
		
		if( dis_elm )
		{
			dis_elm.SetTendency(tendency);
			dis_elm.SetSeriousnessLevel(blood_level);
			
		}
		
	}

	override float GetObservedValue()
	{
		float blood = m_Player.GetHealth("GlobalHealth","Blood");
		return blood;
	}
};