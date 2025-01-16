class TemperatureData
{
	ETemperatureAccessTypes m_AccessType;
	bool m_UseGlobalCooling;
	float m_Value; 					//target or increment, depends on context!
	float m_AdjustedTarget; 		//actual target of the operation (can be adjusted via over-time interpolation, not necessarily the original target value!)
	float m_UpdateTimeInfo; 		//if the temperature change was accumulated over some time, pass this info to temperature subsystems
	float m_UpdateTimeCoef; 		//multiplies the time
	float m_HeatPermeabilityCoef; 	//permeability multiplier (modifies change speed).
	
	float m_InterpolatedStepSize; 	//only useful for interpolated temperature values
	float m_InterpolatedFraction;	//only useful for interpolated temperature values
	
	void TemperatureData(float val, ETemperatureAccessTypes accessType = ETemperatureAccessTypes.ACCESS_UNKNOWN, float time = -1, float timeCoef = 1, float heatPermCoef = 1)
	{
		m_Value = val;
		m_AdjustedTarget = val;
		m_AccessType = accessType;
		m_UpdateTimeInfo = time;
		m_UpdateTimeCoef = timeCoef;
		m_HeatPermeabilityCoef = heatPermCoef;
		
		Init();
	}
	
	protected void Init()
	{
		m_UseGlobalCooling = true;
		m_InterpolatedFraction = 0.0;
	}
}

class TemperatureDataInterpolated : TemperatureData
{
	void InterpolateTemperatureDelta(float start)
	{
		m_InterpolatedStepSize = -1;
		
		float target = m_Value;
		m_AdjustedTarget = start;
		
		if (start != target)
		{
			float diffAbs = Math.Max(start,target) - Math.Min(start,target);
			m_InterpolatedFraction = Math.Clamp(Math.InverseLerp(GameConstants.TEMPERATURE_INTERPOLATION_THRESHOLD_MIN_ABS,GameConstants.TEMPERATURE_INTERPOLATION_THRESHOLD_MAX_ABS,diffAbs),0,1);
			m_InterpolatedStepSize = Math.Lerp(GameConstants.TEMPERATURE_RATE_AVERAGE_ABS,GameConstants.TEMPERATURE_RATE_MAX_ABS,m_InterpolatedFraction);
			
			float coef = m_UpdateTimeCoef;
			float absBaseTempChange = m_UpdateTimeInfo * m_InterpolatedStepSize;
			
			if (start > target)
			{
				absBaseTempChange *= -1;
				if (m_UseGlobalCooling)
					coef = GameConstants.TEMP_COEF_COOLING_GLOBAL;
			}

			coef *= m_HeatPermeabilityCoef;
			absBaseTempChange *= coef;
			
			m_AdjustedTarget = start + Math.Clamp(absBaseTempChange,-diffAbs,diffAbs);
		}
	}
}