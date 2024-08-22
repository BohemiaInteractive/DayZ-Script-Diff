class ChernarusPlusData extends WorldData
{
	protected int m_sameWeatherCnt = 0;
	protected int m_stepValue = 5;
	protected int m_chance = 50;

	protected int m_choosenWeather = 1;
	protected int m_lastWeather = 0;
	
	protected int m_DefaultHeigthBias = 50;
	
	//All Chernarus firing coordinates 
	protected static const ref array<vector> CHERNARUS_ARTY_STRIKE_POS = 
	{
		"-500.00 165.00 5231.69",
		"-500.00 300.00 9934.41",
		"10406.86 192.00 15860.00",
		"4811.75 370.00 15860.00",
		"-500.00 453.00 15860.00"
	};

	override void Init()
	{
		super.Init();

		// new temperature curve settings
		m_Sunrise_Jan = 8.54;
		m_Sunset_Jan = 15.52;
		m_Sunrise_Jul = 3.26;
		m_Sunset_Jul = 20.73;

		int tempIdx;
		m_MinTemps = {-3,-2,0,4,9,14,18,17,13,11,9,0}; //{-3,-2,0,4,9,14,18,17,12,7,4,0} original values
		if (CfgGameplayHandler.GetEnvironmentMinTemps() && CfgGameplayHandler.GetEnvironmentMinTemps().Count() == 12)
		{
			for (tempIdx = 0; tempIdx < CfgGameplayHandler.GetEnvironmentMinTemps().Count(); tempIdx++)
			{
				m_MinTemps[tempIdx] = CfgGameplayHandler.GetEnvironmentMinTemps().Get(tempIdx);
			}
		}

		m_MaxTemps = {3,5,7,14,19,24,26,25,18,14,10,5}; //{3,5,7,14,19,24,26,25,21,16,10,5} original values
		if (CfgGameplayHandler.GetEnvironmentMaxTemps() && CfgGameplayHandler.GetEnvironmentMaxTemps().Count() == 12)
		{
			for (tempIdx = 0; tempIdx < CfgGameplayHandler.GetEnvironmentMaxTemps().Count(); tempIdx++)
			{
				m_MaxTemps[tempIdx] = CfgGameplayHandler.GetEnvironmentMaxTemps().Get(tempIdx);
			}
		}

		m_FiringPos = CHERNARUS_ARTY_STRIKE_POS;
		
		m_WorldWindCoef = 0.4;
		m_CloudsTemperatureEffectModifier = 2.0;
		m_TemperaturePerHeightReductionModifier = 0.012;
		
		m_UniversalTemperatureSourceCapModifier = -1.0;		
		
		if (GetGame().IsServer() && !GetGame().IsMultiplayer())
		{			
			m_Weather.SetDynVolFogHeightBias(m_DefaultHeigthBias);	
		
			if (GetGame().IsMultiplayer())
			{
				float startingOvercast = Math.RandomFloat(0.2,0.75);
				m_Weather.GetOvercast().Set(startingOvercast,0,5); //forcing a random weather at a clean server start and an instant change for overcast
				CalculateVolFog(startingOvercast, m_Weather.GetWindSpeed(), 0);
			}
		}
	} 
	
	override void SetupWeatherSettings()
	{
		super.SetupWeatherSettings();
		
		m_WeatherDefaultSettings.m_StormThreshold		 	= 0.9;
		m_WeatherDefaultSettings.m_GlobalSuddenChance	 	= 0;
		m_WeatherDefaultSettings.m_BadWeatherSuddenChance 	= 0;
	}
	
	override bool WeatherOnBeforeChange( EWeatherPhenomenon type, float actual, float change, float time )
	{
		float phmnTime 	= 5;
		float phmnLength 	= 10;
		float phmnValue = 0;

		m_Weather.SetStorm( 1.0, m_WeatherDefaultSettings.m_StormThreshold, 45 );

		m_Weather.SetRainThresholds( m_WeatherDefaultSettings.m_RainThreshold, 1.0, 60 );
		m_Weather.SetWindMaximumSpeed( 20 );
		
		if (m_Weather.GetDynVolFogHeightBias() < m_DefaultHeigthBias)
		{
			m_Weather.SetDynVolFogHeightBias(m_DefaultHeigthBias);
		}

		switch (type)
		{
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.OVERCAST:			
			{
				float windDirection, windMag;

				//went something goes wrong choose some default random weather	
				phmnValue = Math.RandomFloatInclusive( 0.2, 0.7 );
				phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
				phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );				
			
				//----
				//calculate next weather
				m_chance = Math.RandomIntInclusive( 0, 100 );
				
				//--
				if ( m_lastWeather == WorldDataWeatherConstants.CLEAR_WEATHER )
				{
					m_ClearWeatherChance -= ( m_stepValue * m_sameWeatherCnt);					//decrease the chance of the same weather
				}

				if ( m_lastWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
				{
					m_ClearWeatherChance += ( m_stepValue * m_sameWeatherCnt);					//increase the chance of the better weather
				}
			
				if ( m_lastWeather == WorldDataWeatherConstants.BAD_WEATHER )
				{
					m_ClearWeatherChance += m_stepValue;										//increase the chance of the better weather slightly
					m_BadWeatherChance += (( m_stepValue * m_sameWeatherCnt ) + m_stepValue  );	//decrease the chance of the same weather rapidly
				}
			
				//----
				if ( m_chance < m_ClearWeatherChance )
				{
					m_choosenWeather = WorldDataWeatherConstants.CLEAR_WEATHER;
					if ( m_lastWeather == WorldDataWeatherConstants.CLEAR_WEATHER )
						m_sameWeatherCnt ++;
				}
				else if ( m_chance > m_BadWeatherChance )
				{
					m_choosenWeather = WorldDataWeatherConstants.BAD_WEATHER;
					if ( m_lastWeather == WorldDataWeatherConstants.BAD_WEATHER )
						m_sameWeatherCnt ++;
				}
				else
				{
					m_choosenWeather = WorldDataWeatherConstants.CLOUDY_WEATHER;
					if ( m_lastWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
						m_sameWeatherCnt ++;
				}

				if ( m_choosenWeather != m_lastWeather )
					m_sameWeatherCnt = 0;

				m_ClearWeatherChance = m_WeatherDefaultSettings.m_ClearWeatherChance;
				m_BadWeatherChance = m_WeatherDefaultSettings.m_BadWeatherChance;

				//----
				//set choosen weather			
				if ( m_choosenWeather == WorldDataWeatherConstants.CLEAR_WEATHER  )
				{
					m_lastWeather = WorldDataWeatherConstants.CLEAR_WEATHER;
					
					phmnValue = Math.RandomFloatInclusive( 0.0, 0.3 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
				}

				if ( m_choosenWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
				{
					m_lastWeather = WorldDataWeatherConstants.CLOUDY_WEATHER;

					phmnValue = Math.RandomFloatInclusive( 0.3, 0.6 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
				}
			
				if ( m_choosenWeather == WorldDataWeatherConstants.BAD_WEATHER )
				{
					m_lastWeather = WorldDataWeatherConstants.BAD_WEATHER;

					phmnValue = Math.RandomFloatInclusive( 0.6, 1.0 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
				}

				m_Weather.GetOvercast().Set( phmnValue, phmnTime, phmnLength );
				
				CalculateWind( m_choosenWeather, false, windMag, windDirection );
				m_Weather.GetWindMagnitude().Set( windMag, phmnTime * WIND_MAGNITUDE_TIME_MULTIPLIER , phmnTime * (1 - WIND_MAGNITUDE_TIME_MULTIPLIER) ); // magnitude change happens during the overcast change, after overcast change finishes wind will decrease a bit
				m_Weather.GetWindDirection().Set( windDirection, phmnTime * WIND_DIRECTION_TIME_MULTIPLIER , phmnTime - (phmnTime * WIND_DIRECTION_TIME_MULTIPLIER) + phmnLength + 1000 );
				
				CalculateVolFog(phmnValue, windMag, phmnTime);

				Debug.WeatherLog(string.Format("Chernarus::Weather::Overcast:: (%1) overcast: %2", g_Game.GetDayTime(), actual));
				Debug.WeatherLog(string.Format("Chernarus::Weather::Overcast::Rain:: (%1) %2", g_Game.GetDayTime(), m_Weather.GetRain().GetActual()));

				return true;
			}
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.RAIN:
			{
				float actualOvercast = m_Weather.GetOvercast().GetActual();
				
				m_chance = Math.RandomIntInclusive( 0, 100 );
				phmnValue = 0.2;
				phmnTime = 90;
				phmnLength = 30;
			
				if ( actualOvercast <= m_WeatherDefaultSettings.m_RainThreshold )
				{
					m_Weather.GetRain().Set( 0.0, m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
					Debug.WeatherLog(string.Format("Chernarus::Weather::Rain::ForceEnd:: (%1) %2 -> 0", g_Game.GetDayTime(), actual));
					return true;
				}
			
				if ( actualOvercast > m_WeatherDefaultSettings.m_StormThreshold )
				{
					phmnValue = Math.RandomFloatInclusive( 0.8, 1.0 );
					phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
					phmnLength = 0;

					m_Weather.GetRain().Set( phmnValue, phmnTime, phmnLength );
					Debug.WeatherLog(string.Format("Chernarus::Weather::Rain::ForceStorm:: (%1) %2 -> %3", g_Game.GetDayTime(), actual, phmnValue));
					return true;
				}
			
				//make a differnce in "normal rain"
				if ( actualOvercast < 0.75 )
				{
					if ( m_chance < 30 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.1, 0.3 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else if ( m_chance < 60 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.2, 0.5 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else if ( m_chance < 80 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.0, 0.2 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else //also have the chance to not have rain at all
					{
						phmnValue = 0;
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 120;
					}
				}
				else
				{
					if ( m_chance < 25 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.5, 0.7 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else if ( m_chance < 50 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.2, 0.4 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else if ( m_chance < 75 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.4, 0.6 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else //also have the chance to not have rain at all
					{
						phmnValue = 0;
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 120;
					}
				}
		
				m_Weather.GetRain().Set( phmnValue, phmnTime, phmnLength );

				Debug.WeatherLog(string.Format("Chernarus::Weather::Rain:: (%1) %2", g_Game.GetDayTime(), actual));

				return true;
			}
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.FOG:
			{
				float fogMin = 0.0;
				float fogMax = 0.15;
				float fogTime = 1800.0;

				float fogyMorning = Math.RandomFloatInclusive( 0.0, 1.0 );

				if ( fogyMorning > 0.85 )
				{
					if ( (g_Game.GetDayTime() > 4 && g_Game.GetDayTime() < 7 ) )
					{
						fogMin = 0.10;
						fogMax = 0.35;
						fogTime = 300;
					}
				}

				if ( m_Weather.GetOvercast().GetActual() < 0.3 )
				{
					fogMin = 0.0;
					fogMax = 0.08;
					fogTime = 900.0;
				}

				m_Weather.GetFog().Set( Math.RandomFloatInclusive( fogMin, fogMax ), fogTime, 0);
				
				Debug.WeatherLog(string.Format("Chernarus::Weather::Fog:: (%1) %2", g_Game.GetDayTime(), actual));

				return true;
			}
			//-----------------------------------------------------------------------------------------------------------------------------		
			case EWeatherPhenomenon.WIND_MAGNITUDE:
			{
				phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
				m_Weather.GetWindMagnitude().Set(m_Weather.GetWindMagnitude().GetActual() * 0.75, phmnTime , m_WeatherDefaultSettings.m_OvercastMaxTime); // next change will be happen with the overcast change
			
			return true;
			}
		}

		return false;
	}
	
	protected override void CalculateWind(int newWeather, bool suddenChange, out float magnitude, out float direction)
	{
		magnitude = 5;
		direction = 0;		
		
		float windChance = Math.RandomIntInclusive( 0, 100 );
		
		if ( newWeather == WorldDataWeatherConstants.CLEAR_WEATHER )
		{
			if ( windChance < 30 )						
			{
				magnitude = Math.RandomFloatInclusive( 6 , 10 );
				direction = Math.RandomFloatInclusive( -1.0 , -0.5);
			}
			else if ( windChance < 75 )	
			{
				magnitude = Math.RandomFloatInclusive( 8 , 12 );
				direction = Math.RandomFloatInclusive( -1.3 , -0.9);
			}
			else
			{
				magnitude = Math.RandomFloatInclusive( 4 , 6 );
				direction = Math.RandomFloatInclusive( -0.6 , 0.0);
			}
			
		}		
		else if ( newWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
		{			
			if ( windChance < 45 )						
			{
				magnitude = Math.RandomFloatInclusive( 6 , 10 );
				direction = Math.RandomFloatInclusive( -3.14 , -2.4);
			}
			else if ( windChance < 90 )	
			{
				magnitude = Math.RandomFloatInclusive( 8 , 12 );
				direction = Math.RandomFloatInclusive( -2.6, -2.0);
			}
			else
			{
				magnitude = Math.RandomFloatInclusive( 10 , 14 );
				direction = Math.RandomFloatInclusive( -2.2 , -1.4);
			}
		}		
		else
		{
			if ( suddenChange || m_Weather.GetOvercast().GetActual() > m_WeatherDefaultSettings.m_StormThreshold || m_Weather.GetOvercast().GetForecast() - m_Weather.GetOvercast().GetActual() >= 0.4 )
			{
				magnitude = Math.RandomFloatInclusive( 14 , 17 );
				direction = Math.RandomFloatInclusive( 0.9 , 1.45);
			}
			else if ( windChance < 45 )						
			{
				magnitude = Math.RandomFloatInclusive( 9 , 12 );
				direction = Math.RandomFloatInclusive( 1.45, 1.7);
			}
			else if ( windChance < 90 )	
			{
				magnitude = Math.RandomFloatInclusive( 7 , 10 );
				direction = Math.RandomFloatInclusive( 1.6 , 2);
			}
			else
			{
				magnitude = Math.RandomFloatInclusive( 4 , 8 );
				direction = Math.RandomFloatInclusive( 1.9, 2.2 );
			}			
		}
	}
	
	protected override void CalculateVolFog(float lerpValue, float windMagnitude, float changeTime)
	{				
		float distanceDensity, heigthDensity, heightBias;
		int year, month, day, hour, minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
				
		if ( hour < 9 && hour >= 5 )
		{			
			distanceDensity = Math.Lerp( 0.015, 0.05, lerpValue ) * Math.Clamp(1 - (windMagnitude / m_Weather.GetWindMaximumSpeed()), 0.1, 1);	
			heigthDensity = Math.Lerp( 0.8, 1, lerpValue);
			
			heightBias = m_Weather.GetDynVolFogHeightBias();
			
			if (heightBias == m_DefaultHeigthBias) //checks if the randomization has been done
			{
				int diceRoll = Math.RandomIntInclusive(1,100);
				
				if (diceRoll < 50)
				{
					heightBias = Math.RandomInt(m_DefaultHeigthBias + 1, 80);
				}
				else if (diceRoll < 85)
				{
					heightBias = Math.RandomInt(80, 120);
				}
				else
				{
					heightBias = Math.RandomInt(120, 200);
				}
			}
		}
		else if ( hour < 18 && hour >= 9 )
		{
			distanceDensity = Math.Lerp( 0.01, 0.05, lerpValue ) * Math.Clamp(1 - (windMagnitude / m_Weather.GetWindMaximumSpeed()), 0.1, 1);	
			heigthDensity = Math.Lerp( 0.9, 1, lerpValue);
			heightBias = m_DefaultHeigthBias;
		}
		else
		{
			distanceDensity = Math.Lerp( 0.01, 0.03, lerpValue ) * Math.Clamp(1 - (windMagnitude / m_Weather.GetWindMaximumSpeed()), 0.1, 1);	
			heigthDensity = Math.Lerp( 0.9, 1, lerpValue);
			heightBias = m_DefaultHeigthBias;
		}
					
		m_Weather.SetDynVolFogDistanceDensity(distanceDensity, changeTime);
		m_Weather.SetDynVolFogHeightDensity(heigthDensity, changeTime);
		m_Weather.SetDynVolFogHeightBias(heightBias, changeTime);	
	}

	//! DEPRECATED (see WorldDataWeatherConstants)
	const int CLEAR_WEATHER = 1;
	const int CLOUDY_WEATHER = 2;
	const int BAD_WEATHER = 3;

	//! DEPRECATED (see WorldDataWeatherSettings)
	const int OVERCAST_MIN_TIME = 600;
	const int OVERCAST_MAX_TIME = 900;
	
	const float RAIN_THRESHOLD = 0.6;
	const int RAIN_TIME_MIN = 60;
	const int RAIN_TIME_MAX = 120;
	const float STORM_THRESHOLD = 0.9;
	
	protected int m_clearWeatherChance = m_ClearWeatherChance;
	protected int m_badWeatherChance = m_BadWeatherChance;
}
