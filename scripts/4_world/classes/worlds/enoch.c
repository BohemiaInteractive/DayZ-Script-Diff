class EnochData : WorldData
{
	protected int m_stepValue = 5;

	protected int m_sameWeatherCnt = 0;
	protected int m_chance = 50;

	protected int m_choosenWeather = 1;
	protected int m_lastWeather = 0;
	
	protected int m_FoggyMorningHeigthBiasLowLimit = 155;
	protected int m_DefaultHeigthBias = 170;
	
	protected static const ref array<vector> LIVONIA_ARTY_STRIKE_POS = 
	{ 
		"7440.00 417.00 -500.00",
		"-500.00 276.00 5473.00",
		"-500.00 265.00 9852.00",
		"4953.00 240.00 13300.00",
		"9620.00 188.00 13300.00",
		"13300.00 204.00 10322.00",
		"13300.00 288.00 6204.00",
		"13300.00 296.00 -500.00"
	};

	override void Init()
	{
		super.Init();

		// new temperature curve settings
		m_Sunrise_Jan = 8.4;
		m_Sunset_Jan = 15.63;
		m_Sunrise_Jul = 3.65;
		m_Sunset_Jul = 20.35;

		int tempIdx;
		m_MinTemps = {-7,-7.4,-4.1,1.5,7,11.3,20.4,19.1,18,5.3,0.8,-3.6}; //{-7,-7.4,-4.1,1.5,7,11.3,13.4,13.1,9.3,5.3,0.8,-3.6} original values, due to altitude of the map, changed the values drastically
		if (CfgGameplayHandler.GetEnvironmentMinTemps() && CfgGameplayHandler.GetEnvironmentMinTemps().Count() == 12)
		{
			for (tempIdx = 0; tempIdx < CfgGameplayHandler.GetEnvironmentMinTemps().Count(); tempIdx++)
			{
				m_MinTemps[tempIdx] = CfgGameplayHandler.GetEnvironmentMinTemps().Get(tempIdx);
			}
		}

		m_MaxTemps = {-2.5,-2.1,2.3,9,15.5,19.4,25,22,21,10.5,4.2,0.1}; //{-2.5,-2.1,2.3,9,15.5,19.4,20.9,20.4,16,10.5,4.2,0.1} original values
		if (CfgGameplayHandler.GetEnvironmentMaxTemps() && CfgGameplayHandler.GetEnvironmentMaxTemps().Count() == 12)
		{
			for (tempIdx = 0; tempIdx < CfgGameplayHandler.GetEnvironmentMaxTemps().Count(); tempIdx++)
			{
				m_MaxTemps[tempIdx] = CfgGameplayHandler.GetEnvironmentMaxTemps().Get(tempIdx);
			}
		}

		m_FiringPos = LIVONIA_ARTY_STRIKE_POS;
		
		m_CloudsTemperatureEffectModifier = -5.0;
		m_WorldWindCoef = 0.3;
		m_TemperaturePerHeightReductionModifier = 0.01;
		
		m_UniversalTemperatureSourceCapModifier = 1.0;			
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
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
	
	override void InitYieldBank()
	{
		super.InitYieldBank();
		
		m_YieldBank.ClearAllRegisteredItems();
		
		//fishies
		m_YieldBank.RegisterYieldItem(new YieldItemCarp(15));
		m_YieldBank.RegisterYieldItem(new YieldItemMackerel(15));
		m_YieldBank.RegisterYieldItem(new YieldItemSardines(15));
		m_YieldBank.RegisterYieldItem(new YieldItemBitterlings(15));
		
		//fishy junk
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Brown"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Grey"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Green"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Black"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunkEmpty(1,"Pot"));
		
		//non-fishies
		m_YieldBank.RegisterYieldItem(new YieldItemDeadRabbit(4));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadRooster(1));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadChicken_White(1));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadChicken_Spotted(1));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadChicken_Brown(1));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadFox(2));
	}
	
	override void SetupWeatherSettings()
	{
		super.SetupWeatherSettings();
		
		m_WeatherDefaultSettings.m_ClearWeatherChance 		= 50;
		m_WeatherDefaultSettings.m_BadWeatherChance 		= 75;
		m_WeatherDefaultSettings.m_BadWeatherSuddenChance 	= 0;
	}
		
	override void SetupLiquidTemperatures()
	{
		m_LiquidSettings = new WorldDataLiquidSettings();
		
		m_LiquidSettings.m_Temperatures[LIQUID_SALTWATER] 	= 25;
		m_LiquidSettings.m_Temperatures[LIQUID_WATER] 		= 25.0;
		m_LiquidSettings.m_Temperatures[LIQUID_STILLWATER] 	= 25.0;
		m_LiquidSettings.m_Temperatures[LIQUID_RIVERWATER] 	= 20.0;
		m_LiquidSettings.m_Temperatures[LIQUID_FRESHWATER] 	= 20.0;
		m_LiquidSettings.m_Temperatures[LIQUID_CLEANWATER] 	= 15.0;
	}
	
	override bool WeatherOnBeforeChange( EWeatherPhenomenon type, float actual, float change, float time )
	{
		float phmnTime = 5;
		float phmnLength = 10;
		float phmnValue = 0;
		
		int year, month, day, hour, minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);

		m_Weather.SetStorm( 1.0, m_WeatherDefaultSettings.m_StormThreshold, 20 );

		m_Weather.SetRainThresholds( m_WeatherDefaultSettings.m_RainThreshold, 1.0, 60 );
		m_Weather.SetWindMaximumSpeed( 20 );
		
		switch (type)
		{
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.OVERCAST:
			{
				float windDirection, windMag;
			
				//went something goes wrong choose some default random weather	
				phmnValue = Math.RandomFloatInclusive( 0.1, 0.4 );
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
					m_badWeatherChance -= ( m_stepValue * m_sameWeatherCnt);
				}
			
				if ( m_lastWeather == WorldDataWeatherConstants.BAD_WEATHER )
				{
					m_ClearWeatherChance += m_stepValue;										//increase the chance of the better weather slightly
					m_badWeatherChance += ( m_stepValue * m_sameWeatherCnt );	//decrease the chance of the same weather
				}
			
				//----
				if ( m_chance > m_WeatherDefaultSettings.m_GlobalSuddenChance && !m_IsSuddenChange) //checks if previous weather was a sudden change
				{	
					m_IsSuddenChange = true;
					m_choosenWeather = WorldDataWeatherConstants.BAD_WEATHER;
					if ( m_lastWeather == WorldDataWeatherConstants.BAD_WEATHER )
						m_sameWeatherCnt ++;
				}
				else if ( m_chance < m_ClearWeatherChance )
				{
					m_choosenWeather = WorldDataWeatherConstants.CLEAR_WEATHER;
					if ( m_lastWeather == WorldDataWeatherConstants.CLEAR_WEATHER )
						m_sameWeatherCnt ++;
				}
				else if ( m_chance > m_badWeatherChance )
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
				m_badWeatherChance = m_WeatherDefaultSettings.m_BadWeatherChance;

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

					phmnValue = Math.RandomFloatInclusive( 0.3, 0.7 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( 0, m_WeatherDefaultSettings.m_OvercastMinTime );
				}
			
				if ( m_choosenWeather == WorldDataWeatherConstants.BAD_WEATHER )
				{
					m_lastWeather = WorldDataWeatherConstants.BAD_WEATHER;

					phmnValue = Math.RandomFloatInclusive( 0.7, 1.0 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( 0, m_WeatherDefaultSettings.m_OvercastMinTime );
				
					if ( m_IsSuddenChange )
					{
						phmnTime *= SUDDENCHANGE_TIME_MULTIPLIER;
						phmnLength *= SUDDENCHANGE_LENGTH_MULTIPLIER;
					}
				}

				m_Weather.GetOvercast().Set( phmnValue, phmnTime, phmnLength );
			
				//we want to control wind with the overcast calculation and change with it.
				CalculateWind( m_choosenWeather, m_IsSuddenChange, windMag, windDirection );
				m_Weather.GetWindMagnitude().Set( windMag, phmnTime * WIND_MAGNITUDE_TIME_MULTIPLIER , phmnTime * (1 - WIND_MAGNITUDE_TIME_MULTIPLIER) ); // magnitude change happens during the overcast change, after overcast change finishes wind will decrease a bit
				m_Weather.GetWindDirection().Set( windDirection, phmnTime * WIND_DIRECTION_TIME_MULTIPLIER , phmnTime - (phmnTime * WIND_DIRECTION_TIME_MULTIPLIER) + phmnLength + 1000 );
			
				CalculateVolFog(phmnValue, windMag, phmnTime);
			
				if( m_IsSuddenChange )
				{
					m_Weather.GetRain().Set(Math.RandomFloatInclusive( 0.4, 0.8 ), phmnTime, 0); //forces to rain in the suddenchange.
				}

				Debug.WeatherLog(string.Format("Enoch::Weather::Overcast:: (%1) overcast: %2", g_Game.GetDayTime(), actual));
				Debug.WeatherLog(string.Format("Enoch::Weather::Overcast::Rain:: (%1) %2", g_Game.GetDayTime(), m_Weather.GetRain().GetActual()));

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
			
				if ( actualOvercast <= m_WeatherDefaultSettings.m_RainThreshold)
				{
					m_Weather.GetRain().Set( 0.0, m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
					Debug.WeatherLog (string.Format("Enoch::Weather::Rain::ForceEnd:: (%1) %2 -> 0", g_Game.GetDayTime(), actual));
					return true;
				}
			
				if ( actualOvercast > m_WeatherDefaultSettings.m_StormThreshold )
				{
					phmnValue = Math.RandomFloatInclusive( 0.8, 1.0 );
					phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
					phmnLength = 0;

					m_Weather.GetRain().Set( phmnValue, phmnTime, phmnLength );	
					Debug.WeatherLog(string.Format("Enoch::Weather::Rain::ForceStorm:: (%1) %2 -> %3", g_Game.GetDayTime(), actual, phmnValue));
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
						phmnValue = Math.RandomFloatInclusive( 0.2, 0.4 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else if ( m_chance < 85 )
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
				else if ( actualOvercast < m_WeatherDefaultSettings.m_StormThreshold)
				{
					if ( m_chance < 15 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.4, 0.6 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else if ( m_chance < 50 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.2, 0.4 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else if ( m_chance < 90 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.6, 0.8 );
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
					if ( m_chance < 50 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.9, 1.0 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else
					{
						phmnValue = Math.RandomFloatInclusive( 0.8, 0.9 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
				}
		
				m_Weather.GetRain().Set( phmnValue, phmnTime, phmnLength );

				Debug.WeatherLog(string.Format("Enoch::Weather::Rain:: (%1) %2", g_Game.GetDayTime(), actual));
				return true;
			}
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.FOG:
			{
				if (( hour >= 2 ) && ( hour <= 5 ))
				{
					m_Weather.GetFog().Set( 0.13, 900, 0 );
				} 
				else 
				{
					m_Weather.GetFog().Set( 0.01, 900, 0 );
				}
			
				Debug.WeatherLog(string.Format("Enoch::Weather::Fog:: (%1) %2", g_Game.GetDayTime(), actual));

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
			if ( windChance < 10 )						
			{
				magnitude = Math.RandomFloatInclusive( 2 , 4 );
				direction = Math.RandomFloatInclusive( -2.3 , -1.8);
			}
			else if ( windChance < 50 )	
			{
				magnitude = Math.RandomFloatInclusive( 4 , 8 );
				direction = Math.RandomFloatInclusive( -3.14 , -2.3);
			}
			else
			{
				magnitude = Math.RandomFloatInclusive( 6 , 12 );
				direction = Math.RandomFloatInclusive( 2.3 , 3.14);
			}
			
		}		
		else if ( newWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
		{			
			if ( windChance < 45 )						
			{
				magnitude = Math.RandomFloatInclusive( 5 , 10 );
				direction = Math.RandomFloatInclusive( -1.8 , -1.0);
			}
			else if ( windChance < 90 )	
			{
				magnitude = Math.RandomFloatInclusive( 8 , 12 );
				direction = Math.RandomFloatInclusive( -1.0, -0.7);
			}
			else
			{
				magnitude = Math.RandomFloatInclusive( 10 , 14 );
				direction = Math.RandomFloatInclusive( -2.4 , -1.8);
			}
		}		
		else
		{
			if ( suddenChange || m_Weather.GetOvercast().GetActual() > m_WeatherDefaultSettings.m_StormThreshold || m_Weather.GetOvercast().GetForecast() - m_Weather.GetOvercast().GetActual() >= 0.4 )
			{
				magnitude = Math.RandomFloatInclusive( 17 , 20 );
				direction = Math.RandomFloatInclusive( -0.2 , 1.0);
			}
			else if ( windChance < 45 )						
			{
				magnitude = Math.RandomFloatInclusive( 6 , 12 );
				direction = Math.RandomFloatInclusive( -0.6, 0);
			}
			else if ( windChance < 90 )	
			{
				magnitude = Math.RandomFloatInclusive( 7 , 10 );
				direction = Math.RandomFloatInclusive( 1.3 , 1.9);
			}
			else
			{
				magnitude = Math.RandomFloatInclusive( 4 , 8 );
				direction = Math.RandomFloatInclusive( -1.8, -1.3 );
			}			
		}
	}
	
	protected override void CalculateVolFog(float lerpValue, float windMagnitude, float changeTime)
	{				
		float distanceDensity, heigthDensity, heightBias;
		int year, month, day, hour, minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
				
		if ( hour < 6 && hour >= 3 )
		{
			heightBias = m_Weather.GetDynVolFogHeightBias();
			
			if ( heightBias == m_DefaultHeigthBias ) //verify if the foggy morning check has been done
			{
				bool foggyMorning = Math.RandomIntInclusive(0,1);
				
				if (foggyMorning)
				{
					heightBias = Math.RandomInt(m_FoggyMorningHeigthBiasLowLimit, m_DefaultHeigthBias);					
				}
				else
				{
					heightBias = m_FoggyMorningHeigthBiasLowLimit - 1;
				}				
			}
			
			if (heightBias == m_FoggyMorningHeigthBiasLowLimit - 1) //not foggy morning
			{
				distanceDensity = Math.Lerp( 0.01, 0.03, lerpValue ) * Math.Clamp(1 - (windMagnitude / m_Weather.GetWindMaximumSpeed()), 0.1, 1);	
				heigthDensity = Math.Lerp( 0.9, 1, lerpValue);
			}
			else //foggy morning
			{
				distanceDensity = Math.Lerp( 0.5, 0.1, lerpValue ) * Math.Clamp(1 - (windMagnitude / m_Weather.GetWindMaximumSpeed()), 0.1, 1);	
				heigthDensity = Math.Lerp( 0.2, 1, lerpValue);
			}
		}
		else if ( hour < 18 && hour >= 6 )
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
	const float STORM_THRESHOLD = 0.85;
	
	protected int m_clearWeatherChance = m_ClearWeatherChance;
	protected int m_badWeatherChance = m_BadWeatherChance;
}
