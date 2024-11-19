class UniversalTemperatureSourceLambdaFireplace : UniversalTemperatureSourceLambdaBaseImpl
{
	int m_NormalFireplaceTemperatureMax;
	
	void UniversalTemperatureSourceLambdaFireplace()
	{
		m_FuelCount							= 0;
		m_SmallFireplaceTemperatureMax		= 0;
		m_NormalFireplaceTemperatureMax		= 0;
		m_Temperature						= 0;
	}
	
	void SetNormalFireplaceTemperatureMax(int value)
	{
		m_NormalFireplaceTemperatureMax = value;
	}
	
	//! just an update
	void UpdateFireplaceTemperature(UniversalTemperatureSourceSettings pSettings)
	{
		/*
		float target = m_NormalFireplaceTemperatureMax;
		float speedCoef = Math.Clamp((3 - wetnessSpeedCoef),0,float.MAX);
		speedCoef = GameConstants.TEMP_COEF_FIREPLACE_HEATING * speedCoef;
		
		if (m_FuelCount == 0)
		{
			target = g_Game.GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(pSettings.m_Parent);
		}
		pSettings.m_Parent.SetTemperatureEx(new TemperatureDataInterpolated(target,ETemperatureAccessTypes.ACCESS_FIREPLACE,updateTime,speedCoef));
		*/
		
		m_Temperature = pSettings.m_Parent.GetTemperature(); //legacy reasons
	}
	
	///////////////////////////
	//Deprecated code playpen//
	///////////////////////////
	float m_Temperature; //! DEPRECATED
	int m_SmallFireplaceTemperatureMax; //! DEPRECATED
	int m_FuelCount; //! DEPRECATED
	
	//! DEPRECATED
	void SetSmallFireplaceTemperatureMax(int value)
	{
		m_SmallFireplaceTemperatureMax = value;
	}
	
	//! DEPRECATED
	void SetFuelCount(int value)
	{
		m_FuelCount = value;
	}
	
	//!DEPRECATED
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
}
