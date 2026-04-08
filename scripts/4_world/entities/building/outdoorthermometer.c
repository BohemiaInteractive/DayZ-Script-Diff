class OutdoorThermometerManager
{
	private static float TIME_BETWEEN_UPDATE = 1;
	private static float LIMIT_FOR_UPDATE_ALL_THERMOMETERS = 100;
	private static ref array<OutdoorThermometer> m_Thermometers;
	private static int m_LastCheckIndex;
	private static float m_TimeToUpdate;
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
		if (count)
		{
			if (m_TimeToUpdate < 0)
			{
				for (int i = 0; i < m_NumUpdate; i++)
				{
					if (m_LastCheckIndex >= count)
						m_LastCheckIndex = 0;

					m_Thermometers[m_LastCheckIndex].UpdateTemperature();
					m_LastCheckIndex++;
				}

				m_TimeToUpdate = TIME_BETWEEN_UPDATE;
			}
			else
				m_TimeToUpdate -= timeslice;
		}
	}
}

class OutdoorThermometer : House
{
	protected float m_MaxValue = 50;
	protected float m_MinValue = -20;
	
	void OutdoorThermometer()
	{
		OutdoorThermometerManager.Add(this);

		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(LateInit, 250);
	}
	
	void ~OutdoorThermometer()
	{
		OutdoorThermometerManager.Remove(this);
	}
	
	void UpdateTemperature()
	{
		float temperature = g_Game.GetMission().GetWorldData().GetTemperature(this, EEnvironmentTemperatureComponent.ALTITUDE|EEnvironmentTemperatureComponent.OVERCAST|EEnvironmentTemperatureComponent.FOG);
		float temperature01 = (temperature - m_MinValue) / (m_MaxValue - m_MinValue);
		
		temperature01 = Math.Clamp(temperature01, 0.0, 1.0);
				
		SetAnimationPhase("ThermoGauge", temperature01);
	}
	
	private void LateInit()
	{
		g_Game.RegisterNetworkStaticObject(this);
		UpdateTemperature();
	}
}

class Land_OutsideThermometer : OutdoorThermometer
{
	void Land_OutsideThermometer()
	{
		m_MinValue = -40.0;
		m_MaxValue = 50.0;
	}
}

class Land_OutsideThermometer_1 : OutdoorThermometer
{
	void Land_OutsideThermometer_1()
	{
		m_MinValue = -30.0;
		m_MaxValue = 50;
	}
}

class Land_OutsideThermometer_2 : OutdoorThermometer
{
	void Land_OutsideThermometer_2()
	{
		m_MinValue = -35.0;
		m_MaxValue = 55.0;
	}
}

class Land_OutsideThermometer_2_wall : Land_OutsideThermometer_2
{
}