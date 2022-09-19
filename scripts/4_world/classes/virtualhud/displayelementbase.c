class DisplayElementBase extends MessageReceiverBase
{
	int		m_Value;
	int		m_ValueLast = -99999;
	int 	m_Key = 0;
	int		m_Type = 0;
	int 	m_Range = 0;
	int		m_Colors[7];
	string 	m_Labels[7];
	int		m_Pos = 1;
	int		NUM_OF_BITS;
	bool 	m_IsClientOnly = false;
	PlayerBase m_Player;
	
	PluginPlayerStatus 	m_ModulePlayerStatus;

	void DisplayElementBase(PlayerBase player)
	{
		m_System = "VirtualHud";
		m_ModulePlayerStatus = PluginPlayerStatus.Cast(GetPlugin(PluginPlayerStatus));
		m_Player = player;
		//SetMaxValue();
	}
	
	//for elements which get their value client side and can be updated much more quickly
	bool IsClientOnly()
	{
		return m_IsClientOnly;
	}
	
	bool IsValueChanged()
	{
		return !(m_Value == m_ValueLast);
	}
	
	void UpdateHUD()
	{
		m_ValueLast = m_Value;
	}
	
	int GetNumberOfBits()
	{
		return NUM_OF_BITS;
	}
	
	void SetValue(int value, int range = 0)
	{
		m_Value = value;
		if (range != 0 )
		{
			m_Range = range;
		}
	}

	int GetValue()
	{
		return m_Value;
	}

	int BitToDec(int mask, int index, int length)
	{
		int value = mask & (GetCompareMask() << index);
		value = value >> index;
		return value;
	}

	int GetCompareMask()
	{
		int mask = Math.Pow(2, GetNumberOfBits() ) - 1;
		return mask;
	}
	
	int GetType()
	{
		return m_Type;
	}
	
	int TranslateLevelToStatus(int level)
	{
		if( level == DSLevels.WARNING )
			return 2;
		if( level == DSLevels.CRITICAL )
			return 3;
		if( level == DSLevels.BLINKING )
			return 4;
		if( level == DSLevels.EXTRA )
			return 5;
		return 1;
	}
}