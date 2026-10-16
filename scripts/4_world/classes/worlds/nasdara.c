//#define WEATHER_DATA_LOGGING
class NasdaraData : WorldData
{	
	bool previousWeatherHadSandstorm;
	bool sandstormWillChangeOvercast = false;
	
	//-------test variables & methods ------
	#ifdef WEATHER_DATA_LOGGING
	int overcastChangeCount = 0;
	int badWeatherCount = 0;
	int cloudyWeatherCount = 0;
	int clearWeatherCount = 0;
	int sandstormCount = 0;
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
		m_Sunrise_Jan = 7.15;
		m_Sunset_Jan = 16.85;
		m_Sunrise_Jul = 4.85;
		m_Sunset_Jul = 19.15;
		
		//! world specific override
		m_ShortestDayOfYear = 13;
		m_LongestDayOfYear  = 194;

		int tempIdx;
		m_MinTemps = {3.4, 5.4, 7.4, 8.4, 9.4, 12.4, 14.4, 13.4, 11.4, 8.4, 7.4, 4.4}; //{3, 5, 7, 8, 9, 12, 14, 13, 11, 8, 7, 4} original values
		if (CfgGameplayHandler.GetEnvironmentMinTemps() && CfgGameplayHandler.GetEnvironmentMinTemps().Count() == 12)
		{
			for (tempIdx = 0; tempIdx < CfgGameplayHandler.GetEnvironmentMinTemps().Count(); tempIdx++)
			{
				m_MinTemps[tempIdx] = CfgGameplayHandler.GetEnvironmentMinTemps().Get(tempIdx);
			}
		}

		m_MaxTemps = {17.4, 19.4, 22.4, 25.4, 29.4, 34.4, 38.4, 37.4, 34.4, 28.4, 22.4, 18.4}; //{15, 17, 20, 23, 27, 32, 36, 35, 32, 26, 20, 16} original values
		if (CfgGameplayHandler.GetEnvironmentMaxTemps() && CfgGameplayHandler.GetEnvironmentMaxTemps().Count() == 12)
		{
			for (tempIdx = 0; tempIdx < CfgGameplayHandler.GetEnvironmentMaxTemps().Count(); tempIdx++)
			{
				m_MaxTemps[tempIdx] = CfgGameplayHandler.GetEnvironmentMaxTemps().Get(tempIdx);
			}
		}
		
		m_WorldWindCoef = 0.5;

		m_UniversalTemperatureSourceCapModifier = -2;
		
		m_TemperaturePerHeightReductionModifier = 0.005; //if this value changes, calculate new map temperatures. map is elavated for ~80 meters
		m_CloudsTemperatureEffectModifier 		= 5.0;
		m_TemperatureInsideBuildingsModifier 	= 3.0;
		m_WaterContactTemperatureModifier 		= 5.0;
		
		m_TemperatureUnderSunModifier[WorldDataDaytime.DAWN] = 2.0;
		m_TemperatureUnderSunModifier[WorldDataDaytime.MORNING] = 6.0;
		m_TemperatureUnderSunModifier[WorldDataDaytime.NOON] = 8.0;
		m_TemperatureUnderSunModifier[WorldDataDaytime.AFTERNOON] = 4.0;
		m_TemperatureUnderSunModifier[WorldDataDaytime.EVENING] = 2.0;
		m_TemperatureUnderSunModifier[WorldDataDaytime.DUSK] = 0.0;
		m_TemperatureUnderSunModifier[WorldDataDaytime.NIGHT] = 0.0;
		
		m_ClearWeatherChance 	= m_WeatherDefaultSettings.m_ClearWeatherChance;
		m_BadWeatherChance 		= m_WeatherDefaultSettings.m_BadWeatherChance;
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer()) 
		{	
			m_Weather.SetDynVolFogHeightBias( 0, 0 );
			m_Weather.GetFog().Set(0,0,1000);
			m_Weather.GetFog().SetLimits(0,0);
			m_Weather.GetOvercast().SetLimits(0.1, 1);
			m_Weather.GetRain().SetLimits(0, 0.3);
			m_Weather.SetRainThresholds( m_WeatherDefaultSettings.m_RainThreshold, 1.0, 60 );
			m_Weather.GetSnowfall().SetLimits(0, 0);
			m_Weather.SetStorm( 0.1, m_WeatherDefaultSettings.m_StormThreshold, 120 );		
			m_Weather.SetWindMaximumSpeed( 20 );
			m_Weather.GetWindMagnitude().SetForecastChangeLimits( 0, 20 );		
			
			if (GetGame().IsMultiplayer()) 
			{
				float startingOvercast = Math.RandomFloat(0.2,0.3);
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
		m_YieldBank.RegisterYieldItem(new YieldItemCatfish(42));
		m_YieldBank.RegisterYieldItem(new YieldItemRoundGoby(15));
		
		//fishy junk
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"SandalsShoes_Grey"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"SandalsShoes_Brown"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"SandalsShoes_BlueNavy"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunk(1,"SandalsShoes_Black"));
		m_YieldBank.RegisterYieldItem(new YieldItemJunkEmpty(1,"Pot"));
		
		//non-fishies
		m_YieldBank.RegisterYieldItem(new YieldItemDeadRabbit(2));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadRooster(1));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadChicken_White(1));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadChicken_Spotted(1));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadChicken_Brown(1));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadFox(2));
		m_YieldBank.RegisterYieldItem(new YieldItemDeadLizard(4));
	}	
	
	override void SetupWeatherSettings()
	{
		super.SetupWeatherSettings();

		m_WeatherDefaultSettings.m_ClearWeatherChance 		= 40;
		m_WeatherDefaultSettings.m_BadWeatherChance 		= 75;
		m_WeatherDefaultSettings.m_RainThreshold			= 0.40;
		m_WeatherDefaultSettings.m_StormThreshold			= 1.1; // we do not want lightning
	}
	
	override void SetupLiquidTemperatures()
	{		
		m_LiquidSettings = new WorldDataLiquidSettings();
		
		m_LiquidSettings.m_Temperatures[LIQUID_SALTWATER] 	= -0.5;
		m_LiquidSettings.m_Temperatures[LIQUID_WATER] 		= 20.0;
		m_LiquidSettings.m_Temperatures[LIQUID_STILLWATER] 	= 20.0;
		m_LiquidSettings.m_Temperatures[LIQUID_RIVERWATER] 	= 20.0;
		m_LiquidSettings.m_Temperatures[LIQUID_FRESHWATER] 	= 20.0;
		m_LiquidSettings.m_Temperatures[LIQUID_CLEANWATER] 	= 8.0;
		m_LiquidSettings.m_Temperatures[LIQUID_SNOW] 		= -5.0;
		m_LiquidSettings.m_Temperatures[LIQUID_HOTWATER] 	= 90.0;
	}
	
	override void SetupDustEffects()
	{
		super.SetupDustEffects();
		
		m_DustEffectsSettings.m_GroundEffectEnable = true;
		m_DustEffectsSettings.m_GroundEffectMinWindStrength = 8;
		m_DustEffectsSettings.m_GroundEffectDistance = 30;
		m_DustEffectsSettings.m_GroundEffectFrequency = 24;
		m_DustEffectsSettings.m_GroundEffectWindWeight = 0;
		m_DustEffectsSettings.m_GroundEffectEndPoindTest = true;
		m_DustEffectsSettings.m_CloudEffectEnable = true;
		m_DustEffectsSettings.m_CloudEffectMinWindStrength = 5;
		m_DustEffectsSettings.m_CloudEffectCooldownMin = 6;
		m_DustEffectsSettings.m_CloudEffectCooldownMax = 10;
		m_DustEffectsSettings.m_CloudEffectDurationMin = 2;
		m_DustEffectsSettings.m_CloudEffectDurationMax = 7;
		m_DustEffectsSettings.m_CloudEffectDistance = 16;
		m_DustEffectsSettings.m_CloudEffectFromSurface = true;
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
			
		float phmnTime 	= 120;
		float phmnLength = 10;
		float phmnValue = 0;
				
		m_Weather.GetRain().SetLimits(0, 0.3);
		m_Weather.SetRainThresholds( m_WeatherDefaultSettings.m_RainThreshold, 1.0, 60 );
		
		m_Weather.GetOvercast().SetLimits(0.07, 1);
		m_Weather.GetSnowfall().SetLimits(0, 0);
		
		m_Weather.SetStorm( 0.1, m_WeatherDefaultSettings.m_StormThreshold, 120 );
		
		m_Weather.SetWindMaximumSpeed( 20 );
		m_Weather.GetWindMagnitude().SetForecastChangeLimits( 0, 20 );
		
		m_Weather.GetFog().SetLimits(0,0);
		
		Sandstorm sandstorm = m_Weather.GetSandstorm();

		switch (type)
		{
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.OVERCAST:
			{
				float windDirection, windMag;
				float windMagLength = m_WeatherDefaultSettings.m_OvercastMaxLength; 
				
				#ifdef WEATHER_DATA_LOGGING	
				overcastChangeCount++;
				#endif
				
				if (sandstormWillChangeOvercast)
				{	
					phmnValue = Math.RandomFloatInclusive( 0.75, 1 );
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinLength, m_WeatherDefaultSettings.m_OvercastMaxLength ) * 1.4;	
					m_Weather.GetOvercast().Set( phmnValue, 120, phmnLength );
					sandstormWillChangeOvercast = false;
					return true;
				}
				
				//went something goes wrong choose some default random weather	
				phmnValue = Math.RandomFloatInclusive( 0.2, 0.4 );
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
				if ( m_Chance < m_ClearWeatherChance )
				{				
					m_ChoosenWeather = WorldDataWeatherConstants.CLEAR_WEATHER;
						
					if ( m_LastWeather == WorldDataWeatherConstants.CLEAR_WEATHER )
						m_SameWeatherCnt ++;
				}
				else if ( m_Chance > m_BadWeatherChance )
				{
					m_ChoosenWeather = WorldDataWeatherConstants.BAD_WEATHER;	
					
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
					phmnValue = Math.RandomFloatInclusive( 0.07, 0.2 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinLength, m_WeatherDefaultSettings.m_OvercastMaxLength );
				}

				if ( m_ChoosenWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
				{
					m_LastWeather = WorldDataWeatherConstants.CLOUDY_WEATHER;
					#ifdef WEATHER_DATA_LOGGING
					cloudyWeatherCount++;
					#endif
					
					phmnValue = Math.RandomFloatInclusive( 0.2, 0.45 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinLength, m_WeatherDefaultSettings.m_OvercastMaxLength );
				}
			
				if ( m_ChoosenWeather == WorldDataWeatherConstants.BAD_WEATHER )
				{
					m_LastWeather = WorldDataWeatherConstants.BAD_WEATHER;	

					phmnValue = Math.RandomFloatInclusive( 0.45, 0.75 );
					phmnTime = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinTime, m_WeatherDefaultSettings.m_OvercastMaxTime );
					phmnLength = Math.RandomIntInclusive( m_WeatherDefaultSettings.m_OvercastMinLength, m_WeatherDefaultSettings.m_OvercastMaxLength );
				
					#ifdef WEATHER_DATA_LOGGING
					badWeatherCount++;
					#endif									
				}
				
				//we want to control wind with the overcast calculation and change with it.
				CalculateWind( m_ChoosenWeather, false, windMag, windDirection );
								
				if (m_IsSandstorm && !previousWeatherHadSandstorm)
				{
					phmnLength = 0;
					sandstormWillChangeOvercast = true; //this will force the overcast to be in a higher value when the sandstorm ends.
					StartSandstorm(phmnTime * WIND_MAGNITUDE_TIME_MULTIPLIER);
					previousWeatherHadSandstorm = true;
					
					if (phmnValue < 0.75) //TODO: remove once second layer is added
					{
						phmnValue = 0.75;
					}
					
					windMagLength = 99999; //we want sandstorm to stay in the map until the next real overcast change.
				}
				else if (m_IsSandstorm && previousWeatherHadSandstorm)
				{
					Debug.LogError("Two consecutive sandstorm tried to occur");
					windMag = Math.RandomFloatInclusive( 3, 7 );
					
					if ( sandstorm.IsActive() )
					{
						sandstorm.Stop(phmnTime * WIND_MAGNITUDE_TIME_MULTIPLIER, true);
						windDirection = -1.5;
					}
					else
					{
						windDirection = Math.RandomFloatInclusive( 0.20 , 0.90);
					}
					
					m_IsSandstorm = false;
					previousWeatherHadSandstorm = false;
					
					windMagLength = phmnTime * (1 - WIND_MAGNITUDE_TIME_MULTIPLIER); // magnitude change happens during the overcast change, after overcast change finishes wind will decrease a bit
				}
				else
				{
					previousWeatherHadSandstorm = false;
					if ( sandstorm.IsActive() )
					{
						sandstorm.Stop(phmnTime * WIND_MAGNITUDE_TIME_MULTIPLIER, true);
						windDirection = m_Weather.GetWindDirection().GetActual();
					}	
					
					windMagLength = phmnTime * (1 - WIND_MAGNITUDE_TIME_MULTIPLIER); // magnitude change happens during the overcast change, after overcast change finishes wind will decrease a bit
				}				
				
				m_Weather.GetOvercast().Set( phmnValue, phmnTime, phmnLength );
				
				m_Weather.GetWindMagnitude().Set( windMag, phmnTime * WIND_MAGNITUDE_TIME_MULTIPLIER, windMagLength );
				m_Weather.GetWindDirection().Set( windDirection, phmnTime * WIND_DIRECTION_TIME_MULTIPLIER, 99999 ); // we do not want direction to change outside of CalculateWind method
				
				CalculateVolFog(phmnValue, windMag, phmnTime);
								
				Debug.WeatherLog(string.Format("Nasdara::Weather::Overcast:: (%1) overcast: %2", g_Game.GetDayTime(), actual));
				Debug.WeatherLog(string.Format("Nasdara::Weather::Overcast::Rain:: (%1) %2", g_Game.GetDayTime(), m_Weather.GetRain().GetActual()));
			
				#ifdef WEATHER_DATA_LOGGING
				int testYear = 0;
				int testMonth = 0;
				int testDay = 0;
				int testHour = 0;
				int testMinute = 0;
				GetGame().GetWorld().GetDate(testYear, testMonth, testDay, testHour, testMinute);
					
				if ( testDay - startDay > currentDay && testHour - startHour >= 0 && testMinute - startMinute >= 0 )
				{
					FileHandle file = OpenFile("$profile:OvercastCountsNasdara" + (currentDay + 1) + ".log", FileMode.WRITE);
					FPrintln(file, "================================================================");
					FPrintln(file," ================== Day " + (currentDay + 1) + " ================== ");
					FPrintln(file, "================================================================");
					FPrintln(file, "Overcast Change Count: " + overcastChangeCount);
					FPrintln(file, "Bad Weather Change Count: " + badWeatherCount);			
					FPrintln(file, "Cloudy Weather Count: " + cloudyWeatherCount);
					FPrintln(file, "Clear Weather Count: " + clearWeatherCount);				
					FPrintln(file, "Sandstorm Count: " + sandstormCount);	
				
					currentDay++;
					CloseFile(file);
					if ( currentDay == daysToRun )
					{
						g_Game.RequestExit(IDC_MAIN_QUIT);
					}
				
					overcastChangeCount = 0;
					badWeatherCount = 0;
					cloudyWeatherCount = 0;
					clearWeatherCount = 0;
					sandstormCount = 0;				
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
			
				if ( actualOvercast < m_WeatherDefaultSettings.m_RainThreshold )
				{
					m_Weather.GetRain().Set( 0.0, m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
					Debug.WeatherLog(string.Format("Nasdara::Weather::Rain::ForceEnd:: (%1) %2 -> 0", g_Game.GetDayTime(), actual));
					return true;
				}
				else
				{
					if ( m_Chance < 8 )
					{
						phmnValue = Math.RandomFloatInclusive( 0.05, 0.3 );
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax ) * 4;
					}
					else
					{
						phmnValue = 0.0;
						phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
						phmnLength = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax ) * 10;
					}
				}
				
				m_Weather.GetRain().Set( phmnValue, phmnTime, phmnLength );

				Debug.WeatherLog(string.Format("Nasdara::Weather::Rain:: (%1) %2", g_Game.GetDayTime(), actual));

				return true;
			}
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.FOG:
			{	
				m_Weather.GetFog().Set(0,0,100000);
				
				return true;
			}
			//-----------------------------------------------------------------------------------------------------------------------------
			case EWeatherPhenomenon.WIND_MAGNITUDE:
			{
				phmnTime = Math.RandomInt( m_WeatherDefaultSettings.m_RainTimeMin, m_WeatherDefaultSettings.m_RainTimeMax );
				float sereneWindMag = m_Weather.GetWindMagnitude().GetActual() * 0.6;
				
				if ( sereneWindMag > m_WeatherDefaultSettings.m_SandstormMinWindMagnitude )
					sereneWindMag = m_WeatherDefaultSettings.m_SandstormMinWindMagnitude - 1;
				
				m_Weather.GetWindMagnitude().Set(sereneWindMag, phmnTime , m_WeatherDefaultSettings.m_OvercastMaxLength); // next change will be happen with the overcast change
				
				CalculateVolFog(m_Weather.GetOvercast().GetActual(), sereneWindMag, phmnTime);
				
				if (m_IsSandstorm)
				{
					sandstorm.Stop(phmnTime, true);
					m_IsSandstorm = false;
				}
				
				return true;
			}
			case EWeatherPhenomenon.WIND_DIRECTION:
			{
				m_Weather.GetWindDirection().Set( 0, 60 , 100000);
				Debug.LogError("Wind direction has been changed out of overcast change");
				
				if( sandstorm.IsActive() )
				{
					sandstorm.Stop(30, true);
					Debug.LogError("Wind direction changed while sandstorm was active, stopping sandstorm");
				}
									
				return true;
			}
			
		}

		return false;
	}
	
	protected override void OnSandstormStart(bool isStartedByWeather)
	{
		if (!isStartedByWeather)
		{
			float windMagnitude = Math.RandomFloatInclusive( m_WeatherDefaultSettings.m_SandstormMinWindMagnitude , m_Weather.GetWindMaximumSpeed() ); 
			float windDirection = Math.RandomFloatInclusive( m_WeatherDefaultSettings.m_SandstormMinWindDirection, m_WeatherDefaultSettings.m_SandstormMaxWindDirection);
			float randomizedOvercast = Math.RandomFloatInclusive( 0.75, 1 );
			float fadeInTime = 40;
			
			m_Weather.GetWindMagnitude().Set( windMagnitude, fadeInTime, 99999 );
			m_Weather.GetWindDirection().Set( windDirection, fadeInTime * 0.5 , 99999 );
			m_Weather.GetOvercast().Set( randomizedOvercast , fadeInTime * 2, 99999);
			CalculateVolFog(randomizedOvercast, windMagnitude, fadeInTime * 2);
		}
	}
	
	protected override void OnSandstormStop(bool calledFromWeather)
	{
		if (!calledFromWeather)
		{
			// Called once the sandstorm is fully gone :)
			m_Weather.GetOvercast().Set(m_Weather.GetOvercast().GetActual(), 0, 0);
		}
	}
	
	protected override void CalculateWind(int newWeather, bool suddenChange, out float magnitude, out float direction)
	{
		magnitude = 5;
		direction = 0;		
		
		float windChance = Math.RandomIntInclusive( 0, 99 );
		m_IsSandstorm = false;
				
		if ( newWeather == WorldDataWeatherConstants.CLEAR_WEATHER )
		{
			if ( windChance < 40 )						
			{
				magnitude = Math.RandomFloatInclusive( 8 , 15 );
				direction = Math.RandomFloatInclusive( 2.10 , 2.70);
			}
			else if ( windChance < 80 )	
			{
				magnitude = Math.RandomFloatInclusive( 3 , 6 );
				direction = Math.RandomFloatInclusive( 0.60 , 1.90);
			}
			else
			{
				magnitude = Math.RandomFloatInclusive( 4 , 12 );
				direction = Math.RandomFloatInclusive( 0.30 , 0.80);
			}
		}		
		else if ( newWeather == WorldDataWeatherConstants.CLOUDY_WEATHER )
		{			
			if ( windChance < 45 )						
			{
				magnitude = Math.RandomFloatInclusive( 5 , 10 );
				direction = Math.RandomFloatInclusive( 0 , 0.4);
			}
			else	
			{
				magnitude = Math.RandomFloatInclusive( 2 , 8 );
				direction = Math.RandomFloatInclusive( -1.40, 0);
			}
		}		
		else //bad weather
		{	
			float sandstormChance = 8;
			float windRatio = 0.4;
			
			sandstormChance = Math.Clamp( sandstormChance * m_WeatherDefaultSettings.m_sandstormFrequency, 0, 100 );
			
			float highWindChance = ( 100 - sandstormChance ) * windRatio;
			
			if ( !previousWeatherHadSandstorm && windChance < sandstormChance )						
			{
				magnitude = Math.RandomFloatInclusive( m_WeatherDefaultSettings.m_SandstormMinWindMagnitude, m_Weather.GetWindMaximumSpeed() ); 
				direction = Math.RandomFloatInclusive( m_WeatherDefaultSettings.m_SandstormMinWindDirection, m_WeatherDefaultSettings.m_SandstormMaxWindDirection);
				m_IsSandstorm = true;
				
				#ifdef WEATHER_DATA_LOGGING
				sandstormCount++;
				#endif
			}
			else if ( windChance < sandstormChance + highWindChance )	
			{
				magnitude = Math.RandomFloatInclusive( 7 , 15 );
				direction = Math.RandomFloatInclusive( -1.18 , -0.7);				
			}
			else
			{
				magnitude = Math.RandomFloatInclusive( 3, 7 );
				direction = Math.RandomFloatInclusive( -2.5 , -2.0);
			}	
		}		
	}
	
	protected override void CalculateVolFog(float lerpValue, float windMagnitude, float changeTime)
	{	
		float maxVolFogDistanceDensity = 0.006;		
		float minVolFogDistanceDensity = 0.001;

		maxVolFogDistanceDensity = Math.Lerp(maxVolFogDistanceDensity * 0.5, maxVolFogDistanceDensity, lerpValue);
		
		if (minVolFogDistanceDensity > maxVolFogDistanceDensity)
			maxVolFogDistanceDensity = minVolFogDistanceDensity;
		
		float volFogDistanceDensity = Math.Lerp( minVolFogDistanceDensity, maxVolFogDistanceDensity, windMagnitude/m_Weather.GetWindMaximumSpeed() );
		
		m_Weather.SetDynVolFogDistanceDensity(volFogDistanceDensity, changeTime);
		
		float volFogHeightDensity = Math.Lerp(0.95, 0.98, lerpValue);
		m_Weather.SetDynVolFogHeightDensity(volFogHeightDensity, changeTime );
	}

	bool LogWeatherData() //called from mission file to check if the logging should start
	{
		#ifdef WEATHER_DATA_LOGGING
		return true;
		#endif
		return false;
	}
	
	override int GetDaytime()
	{
		int year, month, day, hour, minute;
		g_Game.GetWorld().GetDate(year, month, day, hour, minute);

		WorldData worldData = g_Game.GetMission().GetWorldData();
		int dayNumber = DayOfYear(day, month);
		float sunriseTimeStart = GetSunriseTime(dayNumber);
		float sunsetTimeStart = GetSunsetTime(dayNumber);
		float currentTime = hour + (minute / 60);
		float dayLength = sunsetTimeStart - sunriseTimeStart;
		float dawnT = sunriseTimeStart + (dayLength * 0.1);
		float morningT = sunriseTimeStart + (dayLength * 0.35);
		float noonT = sunriseTimeStart + (dayLength * 0.65);
		float afternoonT = sunriseTimeStart + (dayLength * 0.9);

		if (currentTime >= sunriseTimeStart && currentTime < dawnT)
			return WorldDataDaytime.DAWN;
		if (currentTime >= dawnT && currentTime < morningT)
			return WorldDataDaytime.MORNING;
		if (currentTime >= morningT && currentTime < noonT)
			return WorldDataDaytime.NOON;
		if (currentTime >= noonT && currentTime < afternoonT)
			return WorldDataDaytime.AFTERNOON;
		if (currentTime >= afternoonT && currentTime < sunsetTimeStart)
			return WorldDataDaytime.EVENING;
		if (currentTime >= sunsetTimeStart && currentTime < (sunsetTimeStart + 2))
			return WorldDataDaytime.DUSK;
			
		return WorldDataDaytime.NIGHT;
	}
}