//! Keeps information about currently loaded world, like temperature
class WorldData
{
	protected float m_DayTemperature;	// legacy, no longer used
	protected float m_NightTemperature;	// legacy, no longer used
	protected Weather m_Weather;
	protected float m_EnvironmentTemperature;
	protected float m_Timer;
	protected float	m_MaxTemps[12];
	protected float m_MinTemps[12];
	protected float m_Sunrise_Jan;
	protected float m_Sunset_Jan;
	protected float m_Sunrise_Jul;
	protected float m_Sunset_Jul;
	protected ref array<vector> m_FiringPos; // Where we should fire from. On Init set the relevant data

	void WorldData()
	{
		Init();
	}
	
	void Init()
	{
		m_DayTemperature = 10; 	// legacy, no longer used
		m_NightTemperature = 6;	// legacy, no longer used
		m_Weather = g_Game.GetWeather();
		m_EnvironmentTemperature = 12.0;
		m_Timer = 0.0;
		m_Sunrise_Jan = 8.54;
		m_Sunset_Jan = 15.52;
		m_Sunrise_Jul = 3.26;
		m_Sunset_Jul = 20.73;
		m_MaxTemps = {3,5,7,14,19,24,26,25,21,16,10,5};
		m_MinTemps = {-3,-2,0,4,9,14,18,17,12,7,4,0};
	}

	float GetApproxSunriseTime( float monthday )
	{
		if ( monthday <= 8.0 )
			return ( ( m_Sunrise_Jan - m_Sunrise_Jul ) / ( 8 - 1 ) ) * ( 1 - monthday ) + m_Sunrise_Jan;
		else
			return ( ( ( monthday - 8 ) * ( m_Sunrise_Jan - m_Sunrise_Jul ) ) / ( 13 - 8 ) ) + m_Sunrise_Jul;
	}
	float GetApproxSunsetTime( float monthday )
	{
		if ( monthday <= 8.0 )
			return ( ( m_Sunset_Jan - m_Sunset_Jul ) / (8 - 1) ) * ( 1 - monthday ) + m_Sunset_Jan;
		else
			return ( ( ( monthday - 8 ) * ( m_Sunset_Jan - m_Sunset_Jul ) ) / ( 13 - 8 ) ) + m_Sunset_Jul;
	}
	protected float CalcBaseEnvironmentTemperature( float monthday, float daytime )
	{
		float approxSunrise = GetApproxSunriseTime( monthday );
		float approxSunset = GetApproxSunsetTime( monthday );
		float dayLight = approxSunset - approxSunrise;
		float nightTime = 24.0 - dayLight;
		int tempArrayIndex = Math.Floor(monthday) - 1;
		int tempArrayIndexToLerp = tempArrayIndex + 1;
		if ( tempArrayIndexToLerp >= 12 )
			tempArrayIndexToLerp = 0;
		float tempArrayLerp = monthday - Math.Floor(monthday);
		float minTempA = m_MinTemps[tempArrayIndex];
		float minTempB = m_MinTemps[tempArrayIndexToLerp];
		float maxTempA = m_MaxTemps[tempArrayIndex];
		float maxTempB = m_MaxTemps[tempArrayIndexToLerp];
		float eveningMinA = minTempA + ( 0.5 * Math.AbsFloat( minTempA - maxTempA ) );
		float eveningMinB = minTempB + ( 0.5 * Math.AbsFloat( minTempB - maxTempB ) );

		if ( ( daytime >= approxSunrise ) && ( daytime <= approxSunset ) ) {
			if ( daytime <= ( approxSunrise + ( dayLight * 0.75 ) ) )
				return Math.Lerp(
					Math.Lerp( minTempA, minTempB, tempArrayLerp ),
					Math.Lerp( maxTempA, maxTempB, tempArrayLerp ),
					( ( daytime - approxSunrise ) / ( dayLight * 0.75 ) ) );
			else
				return Math.Lerp(
					Math.Lerp( maxTempA, maxTempB, tempArrayLerp ),
					Math.Lerp( eveningMinA, eveningMinB, tempArrayLerp ),
					( ( ( daytime - approxSunrise ) - ( dayLight * 0.75 ) ) / ( dayLight - ( dayLight * 0.75 ) ) ) );
		} else {
			if ( ( daytime > approxSunset ) && ( daytime < 24 ) )
				return Math.Lerp(
					Math.Lerp( eveningMinA, eveningMinB, tempArrayLerp ),
					Math.Lerp( minTempA, minTempB, tempArrayLerp ),
					( ( daytime - approxSunset ) / ( 24 - approxSunset ) ) / 2.0 );
			else
				return Math.Lerp(
					Math.Lerp( eveningMinA, eveningMinB, tempArrayLerp ),
					Math.Lerp( minTempA, minTempB, tempArrayLerp ),
					( ( ( daytime + ( 24 - approxSunset ) ) / nightTime ) / 2.0 ) + 0.5 );
		}
	}
	void UpdateBaseEnvTemperature(float timeslice)
	{
		m_Timer += timeslice;
		if ( m_Timer > 30 )
		{
			int year, month, day, hour, minute;
			GetGame().GetWorld().GetDate( year, month, day, hour, minute );
			m_EnvironmentTemperature = CalcBaseEnvironmentTemperature( month + ( day / 32.0 ), hour + ( minute / 60.0 ) );
			m_Timer = 0;
		}
	}

	// getter for the new base enviro temperature
	float GetBaseEnvTemperature()
	{
		return m_EnvironmentTemperature;
	}
	float GetBaseEnvTemperatureExact(int month, int day, int hour, int minute)
	{
		return CalcBaseEnvironmentTemperature( month + ( day / 32.0 ), hour + ( minute / 60.0 ) );
	}

	// legacy, no longer used
	float GetDayTemperature()
	{
		return m_DayTemperature;
	}
	// legacy, no longer used
	float GetNightTemperature()
	{
		return m_NightTemperature;
	}

	bool WeatherOnBeforeChange( EWeatherPhenomenon type, float actual, float change, float time )
	{
		// default behaviour is same like setting MissionWeather (in Weather) to true
		return false;
	}
	
	// Used to return the artillery firing positions
	array<vector> GetArtyFiringPos()
	{
		return m_FiringPos;
	}

	// debug
	void BaseTempDebug(int month, int day)
	{
		Print("--------------------");
		for ( int i = 0; i < 24; i++ )
		{
			for ( int j = 0; j < 6; j++ )
			{
				int minute = ( j * 10 );
				Print(string.Format( "%1:%2      %3", i, minute, GetBaseEnvTemperatureExact( month, day, i, minute ) ) );
			}
		}
	}
};