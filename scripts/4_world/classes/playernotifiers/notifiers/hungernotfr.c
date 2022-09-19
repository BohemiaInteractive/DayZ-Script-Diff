class HungerNotfr: NotifierBase
{
	protected const float 	DEC_TRESHOLD_LOW 			= 0;
	protected const float 	DEC_TRESHOLD_MED 			= -0.35;
	protected const float 	DEC_TRESHOLD_HIGH			= -0.57;
	protected const float 	INC_TRESHOLD_LOW 			= 0;
	protected const float 	INC_TRESHOLD_MED 			= 0.35;
	protected const float 	INC_TRESHOLD_HIGH			= 0.57;
	
	void HungerNotfr(NotifiersManager manager)
	{
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_HUNGRY;
	}

	override void DisplayTendency(float delta)
	{
		//PrintString("delta:"+delta.ToString());
		int tendency = CalculateTendency(delta, INC_TRESHOLD_LOW, INC_TRESHOLD_MED, INC_TRESHOLD_HIGH, DEC_TRESHOLD_LOW, DEC_TRESHOLD_MED, DEC_TRESHOLD_HIGH);
		//GetVirtualHud().SetStatus(eDisplayElements.DELM_TDCY_ENERGY,tendency);
		
		//DSLevels level = DetermineLevel( GetObservedValue(), PlayerConstants.THRESHOLD_ENERGY_WARNING, PlayerConstants.THRESHOLD_ENERGY_CRITICAL, PlayerConstants.THRESHOLD_ENERGY_BLINKING, PlayerConstants.THRESHOLD_ENERGY_EXTRA );
	
		EStatLevels energy_level = m_Player.GetStatLevelEnergy();
		DisplayElementTendency dis_elm = DisplayElementTendency.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_TDCY_ENERGY));
		
		if( dis_elm )
		{
			dis_elm.SetTendency(tendency);
			dis_elm.SetSeriousnessLevel(energy_level);
			
		}
		
		/*
		Print("-----------------------------------------------------------");
		Print("water:"+ typename.EnumToString(EStatLevels,m_Player.GetStatLevelWater()));
		Print("energy:"+ typename.EnumToString(EStatLevels,m_Player.GetStatLevelEnergy()));
		Print("health:"+ typename.EnumToString(EStatLevels,m_Player.GetStatLevelHealth()));
		Print("blood:"+ typename.EnumToString(EStatLevels,m_Player.GetStatLevelBlood()));
		*/
	}

	override void DisplayBadge()
	{
		
	}

	override void HideBadge()
	{
	}

	
	override float GetObservedValue()
	{
		return m_Player.GetStatEnergy().Get();
	}
};