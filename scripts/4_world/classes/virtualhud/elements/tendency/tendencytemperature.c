class TendencyTemperature extends DisplayElementTendency 
{
	void TendencyTemperature(PlayerBase player)
	{
		TENDENCY_MASK = 7;// 3 bits
		SERIOUSNESS_BIT_MASK = 7;// 3 bits
		SERIOUSNESS_BIT_OFFSET = 3;//3 bit offset
		NUM_OF_BITS = 6;		

		m_Type	= eDisplayElements.DELM_TDCY_TEMPERATURE;
		m_Key = NTFKEY_FEVERISH;
	}
	
	override int TranslateLevelToStatus(int level)
	{
		if( level == DSLevelsTemp.WARNING_PLUS ) return 2;
		if( level == DSLevelsTemp.CRITICAL_PLUS ) return 3;
		if( level == DSLevelsTemp.BLINKING_PLUS ) return 4;
		if( level == DSLevelsTemp.WARNING_MINUS ) return 5;
		if( level == DSLevelsTemp.CRITICAL_MINUS ) return 6;
		if( level == DSLevelsTemp.BLINKING_MINUS ) return 7;
		return 1;
	}
}