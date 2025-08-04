//#define WEATHER_DATA_LOGGING
class EnochData : WorldData
{
	//-------test variables & methods ------
	#ifdef WEATHER_DATA_LOGGING
	int overcastChangeCount = 0;
	int suddenChangeCount = 0;
	int stormCount = 0;
	int suddenStormCount = 0;
	int badWeatherCount = 0;
	int cloudyWeatherCount = 0;
	int clearWeatherCount = 0;
	int startYear = 0;
	int startMonth = 0;
	int startDay = 0;
	int startHour = 0;
	int startMinute = 0;
	int currentDay = 0;
	int daysToRun = 10;
	bool dayInit = false;
	#endif
	//------------------------	
	
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
			m_Weather.GetSnowfall().SetLimits(0, 0);
			m_Weather.SetDynVolFogHeightBias(m_WeatherDefaultSettings.m_DefaultHeigthBias);
		
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
		m_YieldBank.RegisterYieldItem(new YieldItemCarp(42));
		m_YieldBank.RegisterYieldItem(new YieldItemMackerel(42));
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
		#ifdef WEATHER_DATA_LOGGING	
		if ( !dayInit )
		{
			GetGame().GetWorld().GetDate(startYear, startMonth, startDay, startHour, startMinute);
			dayInit = true;
		}
		#endif
		
		float phmnTime = 5;
		float phmnLength = 10;
		float phmnValue = 0;
		
		int year, month, day, hour, minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);

		m_Weather.SetStorm( 1.0, m_WeatherDefaultSettings.m_StormThreshold, 20 );
		m_Weather.GetSnowfall().SetLimits(0, 0);

		m_Weather.SetRainThresholds( m_WeatherDefaultSettings.m_RainThreshold, 1.0, 60 );
		m_Weather.SetWindMaximumSpeed( 20 );
		
		switch (type)
		{
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.OVERCAST:
			{
				#ifdef WEATHER_DATA_LOGGING	
				overcastChangeCount++;
				#endif
				
				float windDirection, windMag;
			
				//went something goes wrong choose some default random weather	
				phmnValue = Math.RandomFloatInclusive( 0.1, 0.4 );
				phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
				phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinLength, m_WeatherDefaultSettings.m_OvercastMaxLength );	
			
				//----
				//calculate next weather
				m_Chance = Math.RandomIntInclusive( 0, 100 );
				
				//--
				if ( m_LastWeather == WorldDataWeatherConstants.CLEAR_WEATHER )
				{
					m_ClearWeatherChance -= ( m_StepValue * m_SameWeatherCnt);					//decrease the chance of the same weather
				}

				if ( m_LastWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
				{
					m_ClearWeatherChance += ( m_StepValue * m_SameWeatherCnt);					//increase the chance of the better weather
					m_BadWeatherChance -= ( m_StepValue * m_SameWeatherCnt);
				}
			
				if ( m_LastWeather == WorldDataWeatherConstants.BAD_WEATHER )
				{
					m_ClearWeatherChance += m_StepValue;										//increase the chance of the better weather slightly
					m_BadWeatherChance += ( m_StepValue * m_SameWeatherCnt );					//decrease the chance of the same weather
				}
			
				//----
				if ( m_Chance > m_WeatherDefaultSettings.m_GlobalSuddenChance && !m_IsSuddenChange) //checks if previous weather was a sudden change
				{						
					m_IsSuddenChange = true;
					m_ChoosenWeather = WorldDataWeatherConstants.BAD_WEATHER;
					if ( m_LastWeather == WorldDataWeatherConstants.BAD_WEATHER )
						m_SameWeatherCnt ++;
				}
				else if ( m_Chance < m_ClearWeatherChance )
				{
					m_ChoosenWeather = WorldDataWeatherConstants.CLEAR_WEATHER;
					m_IsSuddenChange = false;
					if ( m_LastWeather == WorldDataWeatherConstants.CLEAR_WEATHER )
						m_SameWeatherCnt ++;
				}
				else if ( m_Chance > m_BadWeatherChance )
				{
					m_ChoosenWeather = WorldDataWeatherConstants.BAD_WEATHER;
					m_IsSuddenChange = false;
					if ( m_LastWeather == WorldDataWeatherConstants.BAD_WEATHER )
						m_SameWeatherCnt ++;
				}
				else
				{
					m_ChoosenWeather = WorldDataWeatherConstants.CLOUDY_WEATHER;
					m_IsSuddenChange = false;
					if ( m_LastWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
						m_SameWeatherCnt ++;
				}

				if ( m_ChoosenWeather != m_LastWeather )
					m_SameWeatherCnt = 0;

				m_ClearWeatherChance = m_WeatherDefaultSettings.m_ClearWeatherChance;
				m_BadWeatherChance = m_WeatherDefaultSettings.m_BadWeatherChance;

				//----
				//set choosen weather			
				if ( m_ChoosenWeather == WorldDataWeatherConstants.CLEAR_WEATHER  )
				{
					m_LastWeather = WorldDataWeatherConstants.CLEAR_WEATHER;
					#ifdef WEATHER_DATA_LOGGING
					clearWeatherCount++;
					#endif
					
					phmnValue = Math.RandomFloatInclusive( 0.0, 0.3 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinLength, m_WeatherDefaultSettings.m_OvercastMaxLength );
				}

				if ( m_ChoosenWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
				{
					m_LastWeather = WorldDataWeatherConstants.CLOUDY_WEATHER;
					#ifdef WEATHER_DATA_LOGGING
					cloudyWeatherCount++;
					#endif

					phmnValue = Math.RandomFloatInclusive( 0.3, 0.7 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( 0, m_WeatherDefaultSettings.m_OvercastMinLength );
				}
			
				if ( m_ChoosenWeather == WorldDataWeatherConstants.BAD_WEATHER )
				{
					m_LastWeather = WorldDataWeatherConstants.BAD_WEATHER;

					phmnValue = Math.RandomFloatInclusive( 0.7, 1.0 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( 0, m_WeatherDefaultSettings.m_OvercastMinLength );
					
					#ifdef WEATHER_DATA_LOGGING
					badWeatherCount++;
					#endif
					
					if ( m_IsSuddenChange )
					{
						#ifdef WEATHER_DATA_LOGGING
						suddenChangeCount++;
						badWeatherCount--;
						#endif
						
						phmnTime *= SUDDENCHANGE_TIME_MULTIPLIER;
						phmnLength *= SUDDENCHANGE_LENGTH_MULTIPLIER;
					}
					
					#ifdef WEATHER_DATA_LOGGING
					if ( phmnValue > m_WeatherDefaultSettings.m_StormThreshold )
					{						
						if ( m_IsSuddenChange )
						{
							suddenChangeCount--;
							suddenStormCount++;																					
						}
						else
						{	stormCount++;
							badWeatherCount--;
						}
					}
					#endif
				}

				m_Weather.GetOvercast().Set( phmnValue, phmnTime, phmnLength );
			
				//we want to control wind with the overcast calculation and change with it.
				CalculateWind( m_ChoosenWeather, m_IsSuddenChange, windMag, windDirection );
				m_Weather.GetWindMagnitude().Set( windMag, phmnTime * WIND_MAGNITUDE_TIME_MULTIPLIER , phmnTime * (1 - WIND_MAGNITUDE_TIME_MULTIPLIER) ); // magnitude change happens during the overcast change, after overcast change finishes wind will decrease a bit
				m_Weather.GetWindDirection().Set( windDirection, phmnTime * WIND_DIRECTION_TIME_MULTIPLIER , phmnTime - (phmnTime * WIND_DIRECTION_TIME_MULTIPLIER) + phmnLength + 1000 );
			
				CalculateVolFog(phmnValue, windMag, phmnTime);
			
				if( m_IsSuddenChange )
				{
					m_Weather.GetRain().Set(Math.RandomFloatInclusive( 0.4, 0.8 ), phmnTime, 0); //forces to rain in the suddenchange.
				}

				Debug.WeatherLog(string.Format("Enoch::Weather::Overcast:: (%1) overcast: %2", g_Game.GetDayTime(), actual));
				Debug.WeatherLog(string.Format("Enoch::Weather::Overcast::Rain:: (%1) %2", g_Game.GetDayTime(), m_Weather.GetRain().GetActual()));
				
				#ifdef WEATHER_DATA_LOGGING
				int testYear = 0;
				int testMonth = 0;
				int testDay = 0;
				int testHour = 0;
				int testMinute = 0;
				GetGame().GetWorld().GetDate(testYear, testMonth, testDay, testHour, testMinute);
					
				if ( testDay - startDay > currentDay && testHour - startHour >= 0 && testMinute - startMinute >= 0 )
				{
					FileHandle file = OpenFile("$profile:OvercastCountsLivonia" + (currentDay + 1) + ".log", FileMode.WRITE);
					FPrintln(file, "================================================================");
					FPrintln(file," ================== Day " + (currentDay + 1) + " ================== ");
					FPrintln(file, "================================================================");
					FPrintln(file, "Overcast Change Count: " + overcastChangeCount);
					FPrintln(file, "Bad Weather Change Count: " + badWeatherCount);
					FPrintln(file, "Sudden Change Count: " + suddenChangeCount);					
					FPrintln(file, "Storm Count: " + stormCount);
					FPrintln(file, "Sudden Storm Count: " + suddenStormCount);					
					FPrintln(file, "Cloudy Weather Count: " + cloudyWeatherCount);
					FPrintln(file, "Clear Weather Count: " + clearWeatherCount);
				
					currentDay++;					
					CloseFile(file);
					if ( currentDay == daysToRun )
					{
						g_Game.RequestExit(IDC_MAIN_QUIT);
					}
				
					overcastChangeCount = 0;
					suddenChangeCount = 0;
					stormCount = 0;
					suddenStormCount = 0;
					badWeatherCount = 0;
					cloudyWeatherCount = 0;
					clearWeatherCount = 0;				
				}			
				#endif
				
				return true;
			}
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.RAIN:
			{
				float actualOvercast = m_Weather.GetOvercast().GetActual();

				m_Chance = Math.RandomIntInclusive( 0, 100 );	
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
					if( m_Weather.GetOvercast().GetActual() > m_Weather.GetOvercast().GetForecast() ) //ensure to have a pause to rain after the storm
					{
						phmnValue = 0;
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax ) * 2;
						phmnLength =  Math.RandomInt( m_WeatherDefaultSettings.m_CalmAfterStormTimeMin, m_WeatherDefaultSettings.m_CalmAfterStormTimeMax );
					}
					else
					{
						phmnValue = Math.RandomFloatInclusive( 0.8, 1.0 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					
					m_Weather.GetRain().Set( phmnValue, phmnTime, phmnLength );	
					Debug.WeatherLog(string.Format("Enoch::Weather::Rain::ForceStorm:: (%1) %2 -> %3", g_Game.GetDayTime(), actual, phmnValue));
					return true;
				}

				//make a differnce in "normal rain"
				if ( actualOvercast < 0.75 )
				{
					if ( m_Chance < 30 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.1, 0.3 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = Math.RandomInt( m_WeatherDefaultSettings.m_RainLengthMin, m_WeatherDefaultSettings.m_RainLengthMax );
					}
					else if ( m_Chance < 60 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.2, 0.4 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = Math.RandomInt( m_WeatherDefaultSettings.m_RainLengthMin, m_WeatherDefaultSettings.m_RainLengthMax );
					}
					else if ( m_Chance < 85 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.05, 0.2 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = Math.RandomInt( m_WeatherDefaultSettings.m_RainLengthMin, m_WeatherDefaultSettings.m_RainLengthMax );
					}
					else //also have the chance to not have rain at all
					{
						phmnValue = 0;
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = Math.RandomInt( m_WeatherDefaultSettings.m_RainLengthMin, m_WeatherDefaultSettings.m_RainLengthMax ) * 4;
					}
				}
				else //overcast between storm threshold and 0.75
				{
					if ( m_Chance < 15 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.4, 0.6 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else if ( m_Chance < 50 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.2, 0.4 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else if ( m_Chance < 90 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.6, 0.8 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = 0;
					}
					else //also have the chance to not have rain at all
					{
						phmnValue = 0;
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = Math.RandomInt( m_WeatherDefaultSettings.m_RainLengthMin, m_WeatherDefaultSettings.m_RainLengthMax ) * 4;
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
				m_Weather.GetWindMagnitude().Set(m_Weather.GetWindMagnitude().GetActual() * 0.75, phmnTime , m_WeatherDefaultSettings.m_OvercastMaxLength); // next change will be happen with the overcast change
			
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
			
			if ( heightBias == m_WeatherDefaultSettings.m_DefaultHeigthBias ) //verify if the foggy morning check has been done
			{
				bool foggyMorning = Math.RandomIntInclusive(0,1);
				
				if (foggyMorning)
				{
					heightBias = Math.RandomInt(m_WeatherDefaultSettings.m_FoggyMorningHeigthBiasLowLimit, m_WeatherDefaultSettings.m_DefaultHeigthBias);					
				}
				else
				{
					heightBias = m_WeatherDefaultSettings.m_FoggyMorningHeigthBiasLowLimit - 1;
				}				
			}
			
			if (heightBias == m_WeatherDefaultSettings.m_FoggyMorningHeigthBiasLowLimit - 1) //not foggy morning
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
			heightBias = m_WeatherDefaultSettings.m_DefaultHeigthBias;
		}
		else
		{
			distanceDensity = Math.Lerp( 0.01, 0.03, lerpValue ) * Math.Clamp(1 - (windMagnitude / m_Weather.GetWindMaximumSpeed()), 0.1, 1);	
			heigthDensity = Math.Lerp( 0.9, 1, lerpValue);
			heightBias = m_WeatherDefaultSettings.m_DefaultHeigthBias;
		}
				
		m_Weather.SetDynVolFogDistanceDensity(distanceDensity, changeTime);
		m_Weather.SetDynVolFogHeightDensity(heigthDensity, changeTime);
		m_Weather.SetDynVolFogHeightBias(heightBias, changeTime);
	}
		
	bool LogWeatherData() //called from mission file to check if the logging should start
	{
		#ifdef WEATHER_DATA_LOGGING
		return true;
		#endif
		return false;
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
	
	protected int m_sameWeatherCnt = m_SameWeatherCnt;
	protected int m_stepValue = m_StepValue;
	protected int m_chance = m_Chance;

	protected int m_choosenWeather = m_ChoosenWeather;
	protected int m_lastWeather = m_LastWeather;
}
