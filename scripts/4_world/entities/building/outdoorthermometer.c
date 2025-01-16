class OutdoorThermometerManager
{
	static float TIME_BETWEEN_UPDATE = 1;
	static float LIMIT_FOR_UPDATE_ALL_THERMOMETERS = 100;
	static const string FIREPOINT_ACTION_SELECTION	= "Thermometer_Pos";
	static ref array<OutdoorThermometer> m_Thermometers;
	static int m_LastCheckIndex;
	static float m_TimeToUpdate;
	//How many thermomethers will be set per update. 
	private static int m_NumUpdate = 1;
	
	static void Init()
	{
		m_Thermometers = new array<OutdoorThermometer>;
		m_LastCheckIndex = 0;
		m_TimeToUpdate = 0;
	}
	
	static void Cleanup()
	{
		m_Thermometers = null;
		m_LastCheckIndex = 0;
	}
	
	static void Add(OutdoorThermometer thermometer)
	{
		if (m_Thermometers)
		{
			m_Thermometers.Insert(thermometer);
			
			m_NumUpdate = Math.Ceil((m_Thermometers.Count() * TIME_BETWEEN_UPDATE) / LIMIT_FOR_UPDATE_ALL_THERMOMETERS);
		}
	}
	
	static void Remove(OutdoorThermometer thermometer)
	{
		if (m_Thermometers)
		{
			m_Thermometers.RemoveItem(thermometer);
			
			m_NumUpdate = Math.Ceil((m_Thermometers.Count() * TIME_BETWEEN_UPDATE) / LIMIT_FOR_UPDATE_ALL_THERMOMETERS);
		}
	}
	
	static void Update(float timeslice)
	{
		int count = m_Thermometers.Count();
		if(count)
		{
			if(m_TimeToUpdate < 0)
			{
				for(int i = 0; i < m_NumUpdate; i++)
				{
					if(m_LastCheckIndex >= count)
					{
						m_LastCheckIndex = 0;
					}
					m_Thermometers[m_LastCheckIndex].UpdateTemperature();
					m_LastCheckIndex++;
				}
				m_TimeToUpdate = TIME_BETWEEN_UPDATE;
			}
			else
			{
				m_TimeToUpdate -= timeslice;
			}
		}
	}
}

class OutdoorThermometer extends House
{
	float m_MaxValue = 50;
	float m_MinValue = -20;
	
	void OutdoorThermometer()
	{
		OutdoorThermometerManager.Add(this);
	}

	float GetTemperatureValue(PlayerBase player)
	{
		float temperature = GetGame().GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this);	
		float temperature01 = (temperature - m_MinValue) / (m_MaxValue - m_MinValue);
				
		SetAnimationPhase("ThermoGauge", temperature01);
		return temperature;
	}
	
	void UpdateTemperature()
	{
		float temperature = GetGame().GetMission().GetWorldData().GetBaseEnvTemperatureAtObject(this);
		float temperature01 = (temperature - m_MinValue) / (m_MaxValue - m_MinValue);
		
		temperature01 = Math.Clamp(temperature01, 0, 1);
				
		SetAnimationPhase("ThermoGauge", temperature01);
	}
	
	void ~OutdoorThermometer()
	{
		OutdoorThermometerManager.Remove(this);
	}
}

class Land_OutsideThermometer extends OutdoorThermometer
{
	void Land_OutsideThermometer()
	{
		m_MinValue = -40.0;
		m_MaxValue = 50.0;
	}
}

class Land_OutsideThermometer_1 extends OutdoorThermometer
{
	void Land_OutsideThermometer_1()
	{
		m_MinValue = -30.0;
		m_MaxValue = 50;
	}
}

class Land_OutsideThermometer_2 extends OutdoorThermometer
{
	void Land_OutsideThermometer_2()
	{
		m_MinValue = -35.0;
		m_MaxValue = 55.0;
	}
}

class Land_OutsideThermometer_2_wall extends Land_OutsideThermometer_2
{
}