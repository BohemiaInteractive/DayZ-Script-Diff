enum EObjectTemperatureState
{
	HOT_LVL_FOUR,
	HOT_LVL_THREE,
	HOT_LVL_TWO,
	HOT_LVL_ONE,
	NEUTRAL,
	COLD_LVL_ONE,
	COLD_LVL_TWO,
	COLD_LVL_THREE,
	COLD_LVL_FOUR
}

class ObjectTemperatureState
{
	EObjectTemperatureState m_State;
	int m_Color;
	string m_LocalizedName;
	
	static ObjectTemperatureState GetStateData(int temperature)
	{
		ObjectTemperatureState data = new ObjectTemperatureState();
		
		if (temperature >= GameConstants.STATE_HOT_LVL_FOUR)
		{
			data.m_State = EObjectTemperatureState.HOT_LVL_FOUR;
			data.m_Color = Colors.TEMPERATURE_HOT_LVL_FOUR;
			data.m_LocalizedName = "#inv_inspect_hot_lvl_four";
		}
		else if (temperature >= GameConstants.STATE_HOT_LVL_THREE)
		{
			data.m_State = EObjectTemperatureState.HOT_LVL_THREE;
			data.m_Color = Colors.TEMPERATURE_HOT_LVL_THREE;
			data.m_LocalizedName = "#inv_inspect_hot_lvl_three";
		}
		else if (temperature >= GameConstants.STATE_HOT_LVL_TWO)
		{
			data.m_State = EObjectTemperatureState.HOT_LVL_TWO;
			data.m_Color = Colors.TEMPERATURE_HOT_LVL_TWO;
			data.m_LocalizedName = "#inv_inspect_hot_lvl_two";
		}
		else if (temperature >= GameConstants.STATE_HOT_LVL_ONE)
		{
			data.m_State = EObjectTemperatureState.HOT_LVL_ONE;
			data.m_Color = Colors.TEMPERATURE_HOT_LVL_ONE;
			data.m_LocalizedName = "#inv_inspect_hot_lvl_one";
		}
		else if (temperature >= GameConstants.STATE_NEUTRAL_TEMP)
		{
			data.m_State = EObjectTemperatureState.NEUTRAL;
			data.m_Color = Colors.TEMPERATURE_NEUTAL;
			data.m_LocalizedName = "";
		}
		else if (temperature >= GameConstants.STATE_COLD_LVL_ONE)
		{
			data.m_State = EObjectTemperatureState.COLD_LVL_ONE;
			data.m_Color = Colors.TEMPERATURE_COLD_LVL_ONE;
			data.m_LocalizedName = "#inv_inspect_cold_lvl_one";
		}
		else if (temperature >= GameConstants.STATE_COLD_LVL_TWO)
		{
			data.m_State = EObjectTemperatureState.COLD_LVL_TWO;
			data.m_Color = Colors.TEMPERATURE_COLD_LVL_TWO;
			data.m_LocalizedName = "#inv_inspect_cold_lvl_two";
		}
		else if (temperature >= GameConstants.STATE_COLD_LVL_THREE)
		{
			data.m_State = EObjectTemperatureState.COLD_LVL_THREE;
			data.m_Color = Colors.TEMPERATURE_COLD_LVL_THREE;
			data.m_LocalizedName = "#inv_inspect_cold_lvl_three";
		}
		else
		{
			data.m_State = EObjectTemperatureState.COLD_LVL_FOUR;
			data.m_Color = Colors.TEMPERATURE_COLD_LVL_FOUR;
			data.m_LocalizedName = "#inv_inspect_cold_lvl_four";
		}
		
		return data;
	}
}