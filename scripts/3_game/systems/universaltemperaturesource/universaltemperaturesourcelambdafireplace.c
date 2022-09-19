class UniversalTemperatureSourceLambdaFireplace : UniversalTemperatureSourceLambdaBase
{
	int m_FuelCount;
	int m_SmallFireplaceTemperatureMax;
	int m_NormalFireplaceTemperatureMax;
	float m_Temperature;

	void UniversalTemperatureSourceLambdaFireplace()
	{
		m_FuelCount							= 0;
		m_SmallFireplaceTemperatureMax		= 0;
		m_NormalFireplaceTemperatureMax		= 0;
		m_Temperature						= 0;
	}
	
	void SetSmallFireplaceTemperatureMax(int value)
	{
		m_SmallFireplaceTemperatureMax = value;
	}
	
	void SetNormalFireplaceTemperatureMax(int value)
	{
		m_NormalFireplaceTemperatureMax = value;
	}
	
	void SetFuelCount(int value)
	{
		m_FuelCount = value;
	}
	
	void SetCurrentTemperature(float temperature)
	{
		//no fuel present, temperature should be low but there can be high temperature from previous fuel burning
		if (m_FuelCount == 0 || temperature <= m_SmallFireplaceTemperatureMax)
		{
			temperature = Math.Clamp(temperature, 0, m_SmallFireplaceTemperatureMax); //small fire
		}
		else
		{
			temperature = Math.Clamp(temperature, 0, m_NormalFireplaceTemperatureMax); //normal fire
		}
		
		m_Temperature = temperature;
	}

	override void Execute(UniversalTemperatureSourceSettings pSettings, UniversalTemperatureSourceResult resultValues)
	{
		resultValues.m_Temperature = m_Temperature;
		//Debug.Log(string.Format("Execute: temperature: %1", resultValues.m_Temperature), "UTS Fireplace");
		
		if (pSettings.m_AffectStat)
		{
			//! set temperature to the item stat
			pSettings.m_Parent.SetTemperature(m_Temperature);
		}
		
		DryItemsInVicinity(pSettings);
	}
}
