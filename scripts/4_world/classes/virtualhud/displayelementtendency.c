class DisplayElementTendency extends DisplayElementBase
{
	protected int TENDENCY_MASK = 7;// first x bits
	protected int SERIOUSNESS_BIT_MASK = 7;// second x bits
	protected int SERIOUSNESS_BIT_OFFSET = 3;//bit offset - points to where seriousness bit starts(TODO: get as log from mask)
	
	void DisplayElementTendency(PlayerBase player)
	{
		NUM_OF_BITS = 6;//the overall num of bits this element occupies(can be calculated from masks, better leave it explicit)
	}
	
	void SetSeriousnessLevel( DSLevels level )
	{
		m_Value = (~(SERIOUSNESS_BIT_MASK << SERIOUSNESS_BIT_OFFSET)) & m_Value;//clear the last value
		m_Value = (level << SERIOUSNESS_BIT_OFFSET)  | m_Value;
	}
	
	override void UpdateHUD()
	{
		super.UpdateHUD();
		int seriousness = m_Value >> SERIOUSNESS_BIT_OFFSET;
		int tendency = TENDENCY_MASK & m_Value;
		//PrintString(tendency.ToString());
		
		if(tendency > 3) 
		{
			tendency = -(tendency - 3);
		}
		/*
		PrintString(this.ToString());
		PrintString("ser:"+ seriousness.ToString());
		PrintString("tnd:"+ tendency.ToString());
		*/
		m_ModulePlayerStatus.DisplayTendency(m_Key, tendency, TranslateLevelToStatus(seriousness));
	}
	
	void SetTendency(int tendency)
	{
		m_Value = (~TENDENCY_MASK) & m_Value;//clear the last value
		m_Value =  tendency | m_Value;//insert the new one
		//PrintString(m_Value.ToString());
	}
}