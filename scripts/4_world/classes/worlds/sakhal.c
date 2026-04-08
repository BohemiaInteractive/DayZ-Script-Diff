//#define WEATHER_DATA_LOGGING
class SakhalData : WorldData
{
	const float SPAWN_CHANCE_CHOLERA = 20;
	const float COLD_AREA_TOOL_DMG_MODIF = 1.2;
	
	//-------test variables & methods ------
	#ifdef WEATHER_DATA_LOGGING
	int overcastChangeCount = 0;
	int directSuddenChangeCount = 0;
	int badWeatherSuddenChangeCount = 0;
	int thundersnowCount = 0;
	int suddenThundersnowCount = 0;
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
	
	override void Init()
	{		
		super.Init();
		
		// new temperature curve settings
		m_Sunrise_Jan = 8.41;
		m_Sunset_Jan = 15.58;
		m_Sunrise_Jul = 3.75;
		m_Sunset_Jul = 20.25;

		int tempIdx;
		m_MinTemps = {-6.5, -9.5, -6.5, -9.5, 2, 6, 9, 10, 6, 1, -5, -10}; //{-13, -11, -7, -3, 2, 6, 9, 10, 6, 1, -5, -10} original values
		if (CfgGameplayHandler.GetEnvironmentMinTemps() && CfgGameplayHandler.GetEnvironmentMinTemps().Count() == 12)
		{
			for (tempIdx = 0; tempIdx < CfgGameplayHandler.GetEnvironmentMinTemps().Count(); tempIdx++)
			{
				m_MinTemps[tempIdx] = CfgGameplayHandler.GetEnvironmentMinTemps().Get(tempIdx);
			}
		}

		m_MaxTemps = {-3, -5, -3, -5, 9, 14, 16, 17, 14, 8, 1, -3}; //{-6, -4, -1, 3, 9, 14, 16, 17, 14, 8, 1, -3} original values
		if (CfgGameplayHandler.GetEnvironmentMaxTemps() && CfgGameplayHandler.GetEnvironmentMaxTemps().Count() == 12)
		{
			for (tempIdx = 0; tempIdx < CfgGameplayHandler.GetEnvironmentMaxTemps().Count(); tempIdx++)
			{
				m_MaxTemps[tempIdx] = CfgGameplayHandler.GetEnvironmentMaxTemps().Get(tempIdx);
			}
		}
		m_Pollution = m_Pollution | EPollution.HEAVYMETAL;
		
		m_TemperatureInsideBuildingsModifier 	= -1.0;
		
		m_UniversalTemperatureSourceCapModifier = -3.0;
		
		m_ClearWeatherChance 	= m_WeatherDefaultSettings.m_ClearWeatherChance;
		m_BadWeatherChance 		= m_WeatherDefaultSettings.m_BadWeatherChance;
		
		if (g_Game.IsServer() || !g_Game.IsMultiplayer()) 
		{
			m_Weather.SetDynVolFogHeightDensity( 1, 0 );
			m_Weather.SetDynVolFogHeightBias( 0, 0 );
			m_Weather.GetFog().Set(0,0,1000);
			m_Weather.GetFog().SetLimits(0,0);
			m_Weather.GetOvercast().SetLimits(0.07, 1);
			
			if (g_Game.IsMultiplayer()) 
			{
				m_Weather.GetOvercast().Set(Math.RandomFloat(0,0.75),0,5); //forcing a random weather at a clean server start and an instant change for overcast
				CalculateVolFog(m_Weather.GetSnowfall().GetActual(),m_Weather.GetWindSpeed(),0);
			}
			
			m_DefaultPlayerRestrictedAreas = {"pra/warheadstorage.json"};
		}
		
		
	}
	
	override void InitYieldBank()
	{
		super.InitYieldBank();
		
		m_YieldBank.ClearAllRegisteredItems();
		
		//fishies
		m_YieldBank.RegisterYieldItem(new YieldItemWalleyePollock(61));
		m_YieldBank.RegisterYieldItem(new YieldItemSteelheadTrout(36));
		
		m_YieldBank.RegisterYieldItem(new YieldItemShrimp(1));
		
		//fishy junk
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Brown"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Grey"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Green"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Black"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunkEmpty(1,"Pot"));
		
		//non-fishies
		m_YieldBank.RegisterYieldItem(new YieldItemDeadRabbit(1));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadFox(1));
		//m_YieldBank.RegisterYieldItem(new YieldItemGenericSmallAnimal(1,"Animal_UrsusArctos",AnimalCatchingConstants.MASK_ENVIRO_FOREST,AnimalCatchingConstants.MASK_METHOD_LANDTRAP_MEATBAIT));
	}
	
	override void SetupWeatherSettings()
	{
		super.SetupWeatherSettings();
		
		m_WeatherDefaultSettings.m_GlobalSuddenChance		= 95;	// works if the randomization value is bigger than this. we do not want to decrease the clear sky chance
		m_WeatherDefaultSettings.m_ClearWeatherChance 		= 20;
		m_WeatherDefaultSettings.m_BadWeatherChance 		= 65;
		m_WeatherDefaultSettings.m_BadWeatherSuddenChance 	= 15;
	}
	
	override void SetupLiquidTemperatures()
	{
		
		m_LiquidSettings = new WorldDataLiquidSettings();
		
		m_LiquidSettings.m_Temperatures[LIQUID_SALTWATER] 	= -0.5;
		m_LiquidSettings.m_Temperatures[LIQUID_WATER] 		= 2.0;
		m_LiquidSettings.m_Temperatures[LIQUID_STILLWATER] 	= 2.0;
		m_LiquidSettings.m_Temperatures[LIQUID_RIVERWATER] 	= 2.0;
		m_LiquidSettings.m_Temperatures[LIQUID_FRESHWATER] 	= 2.0;
		m_LiquidSettings.m_Temperatures[LIQUID_CLEANWATER] 	= 5.0;
		m_LiquidSettings.m_Temperatures[LIQUID_SNOW] 		= -5.0;
		m_LiquidSettings.m_Temperatures[LIQUID_HOTWATER] 	= 90.0;
	}
	
	override bool WeatherOnBeforeChange( EWeatherPhenomenon type, float actual, float change, float time )
	{	
		#ifdef WEATHER_DATA_LOGGING	
		if ( !dayInit )
		{
			g_Game.GetWorld().GetDate(startYear, startMonth, startDay, startHour, startMinute);
			dayInit = true;
		}
		#endif
			
		float phmnTime 	= 120;
		float phmnLength = 10;
		float phmnValue = 0;
				
		m_Weather.GetRain().SetLimits(0, 0);//since we decided that it will not rain in sakhal...
		
		m_Weather.GetOvercast().SetLimits(0.07, 1);
		m_Weather.GetSnowfall().SetLimits(0, 1);
		m_Weather.SetSnowfallThresholds( m_WeatherDefaultSettings.m_SnowfallThreshold, 1.0, 60 );
		m_Weather.GetSnowfall().SetForecastChangeLimits(0, 1);
		
		m_Weather.SetStorm( 1.0, m_WeatherDefaultSettings.m_ThundersnowThreshold, 45 );
		
		m_Weather.SetWindMaximumSpeed( 20 );
		m_Weather.GetWindMagnitude().SetForecastChangeLimits( 0, 20 );
		
		m_Weather.GetFog().SetLimits(0,0);

		switch (type)
		{
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.OVERCAST:
			{
				float windDirection, windMag;
				
				#ifdef WEATHER_DATA_LOGGING	
				overcastChangeCount++;
				#endif
				//went something goes wrong choose some default random weather	
				phmnValue = Math.RandomFloatInclusive( 0.2, 0.7 );
				phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
				phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinLength, m_WeatherDefaultSettings.m_OvercastMaxLength );	
			
				//----				
				//--
				if ( m_LastWeather == WorldDataWeatherConstants.CLEAR_WEATHER )
				{
					m_ClearWeatherChance -= ( m_StepValue * m_SameWeatherCnt);					//decrease the chance of the same weather
				}

				if ( m_LastWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
				{
					m_ClearWeatherChance += ( m_StepValue * m_SameWeatherCnt);					//increase the chance of the better weather
				}
			
				if ( m_LastWeather == WorldDataWeatherConstants.BAD_WEATHER )
				{
					m_ClearWeatherChance += m_StepValue;										//increase the chance of the better weather slightly
					m_BadWeatherChance += (( m_StepValue * m_SameWeatherCnt ) + m_StepValue );	//decrease the chance of the same weather rapidly
				}
			
				//----
				//calculate next weather
				
				m_Chance = Math.RandomIntInclusive( 0, 100 );
				if ( m_Chance > m_WeatherDefaultSettings.m_GlobalSuddenChance && !m_IsSuddenChange) //checks if previous weather was a sudden change
				{	
					#ifdef WEATHER_DATA_LOGGING	
					directSuddenChangeCount++;
					#endif
					
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
					
					if ( m_LastWeather == WorldDataWeatherConstants.BAD_WEATHER )
						m_SameWeatherCnt ++;
				
					if ( m_IsSuddenChange )
					{
						m_IsSuddenChange = false;
						
						#ifdef WEATHER_DATA_LOGGING
						badWeatherCount++;
						#endif
					}
					else
					{
						if ( Math.RandomIntInclusive( 0, 100 ) < m_WeatherDefaultSettings.m_BadWeatherSuddenChance )
						{
							m_IsSuddenChange = true;
							
							#ifdef WEATHER_DATA_LOGGING
							badWeatherSuddenChangeCount++;
							#endif
						}
						else
						{
							#ifdef WEATHER_DATA_LOGGING
							badWeatherCount++;
							#endif
						}
					}
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
					phmnValue = Math.RandomFloatInclusive( 0.07, 0.3 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinLength, m_WeatherDefaultSettings.m_OvercastMaxLength );
				}

				if ( m_ChoosenWeather == CLOUDY_WEATHER )
				{
					#ifdef WEATHER_DATA_LOGGING
					cloudyWeatherCount++;
					#endif
					m_LastWeather = CLOUDY_WEATHER;

					phmnValue = Math.RandomFloatInclusive( 0.3, 0.6 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinLength, m_WeatherDefaultSettings.m_OvercastMaxLength );
				}
			
				if ( m_ChoosenWeather == WorldDataWeatherConstants.BAD_WEATHER )
				{
					m_LastWeather = WorldDataWeatherConstants.BAD_WEATHER;

					phmnValue = Math.RandomFloatInclusive( 0.6, 1.0 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinLength, m_WeatherDefaultSettings.m_OvercastMaxLength );
					
					if ( m_IsSuddenChange )
					{
						phmnTime *= SUDDENCHANGE_TIME_MULTIPLIER;
						phmnLength *= SUDDENCHANGE_LENGTH_MULTIPLIER;
					}
					
					#ifdef WEATHER_DATA_LOGGING
					if ( phmnValue > m_WeatherDefaultSettings.m_ThundersnowThreshold )
					{
						if ( m_IsSuddenChange )
						{
							suddenThundersnowCount++;
							if ( m_Chance < m_WeatherDefaultSettings.m_GlobalSuddenChance )
							{
								directSuddenChangeCount--;
							}
							else
							{
								badWeatherSuddenChangeCount--;
							}								
						}
						else
						{
							thundersnowCount++;
							badWeatherCount--;
						}
					}
					#endif
									
				}

				m_Weather.GetOvercast().Set( phmnValue, phmnTime, phmnLength );
				
				//we want to control wind with the overcast calculation and change with it.
				CalculateWind( m_ChoosenWeather, m_IsSuddenChange, windMag, windDirection );
				m_Weather.GetWindMagnitude().Set( windMag, phmnTime * WIND_MAGNITUDE_TIME_MULTIPLIER , phmnTime - (phmnTime * WIND_MAGNITUDE_TIME_MULTIPLIER) + phmnLength + 1000 );
				m_Weather.GetWindDirection().Set( windDirection, phmnTime * WIND_DIRECTION_TIME_MULTIPLIER , phmnTime - (phmnTime * WIND_DIRECTION_TIME_MULTIPLIER) + phmnLength + 1000 );
				
				if( m_IsSuddenChange )
				{
					m_Weather.GetSnowfall().Set(Math.RandomFloatInclusive( 0.4, 0.8 ), phmnTime, 0); //forces to snow in the suddenchange.
				}
				
				Debug.WeatherLog(string.Format("Sakhal::Weather::Overcast:: (%1) overcast: %2", g_Game.GetDayTime(), actual));
				Debug.WeatherLog(string.Format("Sakhal::Weather::Overcast::Snow:: (%1) %2", g_Game.GetDayTime(), m_Weather.GetRain().GetActual()));
			
				#ifdef WEATHER_DATA_LOGGING
				int testYear = 0;
				int testMonth = 0;
				int testDay = 0;
				int testHour = 0;
				int testMinute = 0;
				g_Game.GetWorld().GetDate(testYear, testMonth, testDay, testHour, testMinute);
					
				if ( testDay - startDay > currentDay && testHour - startHour >= 0 && testMinute - startMinute >= 0 )
				{
					FileHandle file = OpenFile("$profile:OvercastCountsSakhal" + (currentDay + 1) + ".log", FileMode.WRITE);
					FPrintln(file, "================================================================");
					FPrintln(file," ================== Day " + (currentDay + 1) + " ================== ");
					FPrintln(file, "================================================================");
					FPrintln(file, "Overcast Change Count: " + overcastChangeCount);
					FPrintln(file, "Bad Weather Change Count: " + badWeatherCount);
					FPrintln(file, "Bad Weather Sudden Change Count: " + badWeatherSuddenChangeCount);
					FPrintln(file, "Global Sudden Change Count: " + directSuddenChangeCount);					
					FPrintln(file, "Thunder Snow Count: " + thundersnowCount);
					FPrintln(file, "Sudden Thundersnow Count: " + suddenThundersnowCount);					
					FPrintln(file, "Cloudy Weather Count: " + cloudyWeatherCount);
					FPrintln(file, "Clear Weather Count: " + clearWeatherCount);
				
					currentDay++;
					CloseFile(file);
					if ( currentDay == daysToRun )
					{
						g_Game.RequestExit(IDC_MAIN_QUIT);
					}
				
					overcastChangeCount = 0;
					directSuddenChangeCount = 0;
					badWeatherSuddenChangeCount = 0;
					thundersnowCount = 0;
					suddenThundersnowCount = 0;
					badWeatherCount = 0;
					cloudyWeatherCount = 0;
					clearWeatherCount = 0;				
				}			
				#endif
			
			
				return true;
			}
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.SNOWFALL:
			{
				float actualOvercast = m_Weather.GetOvercast().GetActual();
				
				m_Chance = Math.RandomIntInclusive( 0, 100 );
				phmnValue = 0.2;
				phmnTime = 90;
				phmnLength = 20;
			
				if ( actualOvercast <= m_WeatherDefaultSettings.m_SnowfallThreshold )
				{
					m_Weather.GetSnowfall().Set( 0.0, m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMin );
					CalculateVolFog(0.0, m_Weather.GetWindMagnitude().GetForecast(), phmnTime);
					
					Debug.WeatherLog(string.Format("Sakhal::Weather::Snow::ForceEnd:: (%1) %2 -> 0", g_Game.GetDayTime(), actual));
					return true;
				}
			
				if ( actualOvercast > m_WeatherDefaultSettings.m_ThundersnowThreshold )
				{
					phmnValue = Math.RandomFloatInclusive( 0.8, 1.0 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax ) * 0.5;
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax ) * 1.2;

					m_Weather.GetSnowfall().Set( phmnValue, phmnTime, phmnLength );
					
					CalculateVolFog(phmnValue, m_Weather.GetWindMagnitude().GetForecast(), phmnTime);
					Debug.WeatherLog(string.Format("Sakhal::Weather::Snow::ForceStorm:: (%1) %2 -> %3", g_Game.GetDayTime(), actual, phmnValue));
					return true;
				}

				if ( actualOvercast == m_Weather.GetOvercast().GetForecast() && m_Weather.GetSnowfall().GetActual() >= 0.8 && m_Weather.GetWindSpeed() >= 10 ) //if overcast is not changing and there was high snowfall
				{
					phmnValue = Math.RandomFloatInclusive( 0.2, 0.4 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax ) * 0.5;
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax ) * 1.2;
					
					m_Weather.GetWindMagnitude().Set(m_Weather.GetWindMagnitude().GetActual() * 0.2, phmnTime , phmnLength); // next change will be happen in phenomenon change and reset the wind speed
					m_Weather.GetSnowfall().Set( phmnValue, phmnTime, phmnLength );
					
					CalculateVolFog(phmnValue, m_Weather.GetWindMagnitude().GetForecast(), phmnTime);
					Debug.WeatherLog(string.Format("Sakhal::Weather::Snow::ForceStorm:: (%1) %2 -> %3", g_Game.GetDayTime(), actual, phmnValue));
					return true;
				}

				if ( actualOvercast < 0.6 ) //snowfall treshold to 0.6 overcast, snowfall in this bracket does not increase wetness
				{
					if ( m_Chance < 40 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.02, 0.4 );
						phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
						phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax ) * 1.2;
					}
					else //also have the chance to not have snow at all
					{
						phmnValue = 0;
						phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
						phmnLength = Math.RandomIntInclusive(m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax) * 2;
					}			
				}
				else if ( actualOvercast < 0.75 ) //0.6 to 0.75 overcast
				{
					if ( m_Chance < 30 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.5, 0.8 );
						phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
						phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax );
					}
					else if ( m_Chance < 60 )
					{
						phmnValue = Math.RandomFloat( 0.3, 0.5 ); //0.5 snowfall starts to increase wetness
						phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
						phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax );
					}
					else if ( m_Chance < 75 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.02, 0.3 );
						phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
						phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax );
					}
					else //also have the chance to not have snow at all
					{
						if ( m_IsSuddenChange )
						{
							phmnValue = Math.RandomFloat( 0.3, 0.5 ); //0.5 snowfall starts to increase wetness
							phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
							phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax );							
						}
						else
						{
							phmnValue = 0;
							phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
							phmnLength = Math.RandomIntInclusive(m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax) * 1.2;
						}
					}
				}
				else //0.75 to thunderstorm threshold overcast
				{
					if ( m_Chance < 35 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.7, 1.0 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
						phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax );
					}
					else if ( m_Chance < 75 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.5, 0.7 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
						phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax ) ;
					}
					else if ( m_Chance < 85 )
					{
						phmnValue = Math.RandomFloat( 0.2, 0.5 ); //0.5 snowfall starts to increase wetness
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
						phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax );
					}
					else
					{
						if ( m_IsSuddenChange )
						{
							phmnValue = Math.RandomFloat( 0.5, 0.7 ); //0.5 snowfall starts to increase wetness
							phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
							phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax );							
						}
						else
						{
							phmnValue = Math.RandomFloat( 0.02, 0.1 );
							phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
							phmnLength = Math.RandomIntInclusive(m_WeatherDefaultSettings.m_SnowfallLengthMin, m_WeatherDefaultSettings.m_SnowfallLengthMax) * 1.2;
						}
					}
				}
		
				m_Weather.GetSnowfall().Set( phmnValue, phmnTime, phmnLength );
				CalculateVolFog(phmnValue, m_Weather.GetWindMagnitude().GetForecast(), phmnTime);

				Debug.WeatherLog(string.Format("Sakhal::Weather::Snow:: (%1) %2", g_Game.GetDayTime(), actual));

				return true;
			}
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.FOG:
			{	
				m_Weather.GetFog().Set(0,0,100000);
				
				return true;
				
				/*float fogMin = 0.0;
				float fogMax = 0.15;
				float fogTime = 1800.0;
				
				if ( m_Weather.GetWindSpeed() >= 10 )
				{
					m_Weather.GetFog().Set( 0, 300, 0);
					Debug.WeatherLog(string.Format("Sakhal::Weather::Fog:: (%1) %2", g_Game.GetDayTime(), actual));
				
					return true;
				}						
				
				if ( m_Weather.GetOvercast().GetActual() < 0.3 )
				{
					fogMin = 0.0;
					fogMax = 0.1;
					fogTime = 600.0;
				}
				else if ( m_Weather.GetOvercast().GetActual() > 0.6 )
				{
					fogMin = 0.1;
					fogMax = 0.3;
					fogTime = 600.0;
				}
				else
				{
					fogMin = 0.0;
					fogMax = 0.15;
					fogTime = 600.0;
				}
				
				m_Weather.GetFog().Set( Math.RandomFloatInclusive( fogMin, fogMax ), fogTime, 0);
				Debug.WeatherLog(string.Format("Sakhal::Weather::Fog:: (%1) %2", g_Game.GetDayTime(), actual));

				return true;*/
			}
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.WIND_MAGNITUDE:
			{
				phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_SnowfallTimeMin, m_WeatherDefaultSettings.m_SnowfallTimeMax );
				m_Weather.GetWindMagnitude().Set(m_Weather.GetWindMagnitude().GetActual() / 0.2, phmnTime , m_WeatherDefaultSettings.m_OvercastMaxTime + 1000); // next change will be happen with the overcast or snowfall change
				CalculateVolFog(m_Weather.GetSnowfall().GetForecast(), m_Weather.GetWindMagnitude().GetForecast(), phmnTime);
				
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
			if ( windChance < 15 )						
			{
				magnitude = Math.RandomFloatInclusive( 2 , 4 );
				direction = Math.RandomFloatInclusive( -0.35 , 0.35);
			}
			else if ( windChance < 90 )	
			{
				magnitude = Math.RandomFloatInclusive( 3 , 6 );
				direction = Math.RandomFloatInclusive( 0 , 2.2);
			}
			else
			{
				magnitude = Math.RandomFloatInclusive( 8 , 15 );
				direction = Math.RandomFloatInclusive( -3.0 , -2.25);
			}
		}		
		else if ( newWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
		{			
			if ( windChance < 45 )						
			{
				magnitude = Math.RandomFloatInclusive( 5 , 10 );
				direction = Math.RandomFloatInclusive( 1.0 , 1.25);
			}
			else if ( windChance < 90 )	
			{
				magnitude = Math.RandomFloatInclusive( 8 , 14 );
				direction = Math.RandomFloatInclusive( -1.8, -1.4);
			}
			else
			{
				magnitude = Math.RandomFloatInclusive( 2 , 4 );
				direction = Math.RandomFloatInclusive( -1.0 , -0.4);
			}
		}		
		else
		{
			if ( suddenChange || m_Weather.GetOvercast().GetForecast() > 0.85 )
			{
				magnitude = Math.RandomFloatInclusive( 18 , 20 );
				direction = Math.RandomFloatInclusive( 2.25 , 3.14);
			}
			else if ( windChance < 45 )						
			{
				magnitude = Math.RandomFloatInclusive( 6 , 12 );
				direction = Math.RandomFloatInclusive( -1.8 , -1.4);
			}
			else if ( windChance < 90 )	
			{
				magnitude = Math.RandomFloatInclusive( 8 , 20 );
				direction = Math.RandomFloatInclusive( 1.6 , 2.0);
			}
			else
			{
				magnitude = Math.RandomFloatInclusive( 4 , 10 );
				direction = Math.RandomFloatInclusive( 1.5, 1.75 );
			}			
		}		
		
		CalculateVolFog(m_Weather.GetSnowfall().GetForecast(), magnitude, 60);
	}
	
	protected override void CalculateVolFog(float lerpValue, float windMagnitude, float changeTime)// in sakhal volfog supports snowfall effect
	{				
		float maxVolFogDistanceDensity = Math.Lerp( 0, 0.35, Easing.EaseInQuart(lerpValue) );
		float minVolFogDistanceDensity = Math.Lerp(0.001, 0.01, m_Weather.GetOvercast().GetActual());
		
		if (minVolFogDistanceDensity > maxVolFogDistanceDensity)
			maxVolFogDistanceDensity = minVolFogDistanceDensity;
		
		float volFogDistanceDensity = Math.Lerp( minVolFogDistanceDensity, maxVolFogDistanceDensity, windMagnitude/m_Weather.GetWindMaximumSpeed() );
		m_Weather.SetDynVolFogDistanceDensity(volFogDistanceDensity, changeTime);
	}
	
	override float GetAgentSpawnChance(eAgents agent)
	{
		if (agent == eAgents.CHOLERA)
			return SPAWN_CHANCE_CHOLERA;
		
		return 0;
	}
	
	override float GetColdAreaToolDamageModifier()
	{
		return COLD_AREA_TOOL_DMG_MODIF;
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
}