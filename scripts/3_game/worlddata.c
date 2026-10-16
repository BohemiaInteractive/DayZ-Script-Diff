//! Keeps information about currently loaded world, like temperature
class WorldData
{
	const float SPAWN_CHANCE_CHOLERA_DEF = 50;
	const float COLD_AREA_TOOL_DMG_MODIF_DEF = 1;
	
	//! directly accesible (defined/overriden in Init())
	float m_TemperaturePerHeightReductionModifier; 	//! amount of °C reduced for each 100 meteres of height above water level
	float m_CloudsTemperatureEffectModifier; 		//! how many % of environment temperature can be lowered by clouds
	float m_TemperatureInsideBuildingsModifier;
	ref map<int, float> m_TemperatureUnderSunModifier;
	float m_WaterContactTemperatureModifier;

	protected float SUDDENCHANGE_TIME_MULTIPLIER 	= 0.2;
	protected float SUDDENCHANGE_LENGTH_MULTIPLIER 	= 0.4;
	protected float WIND_MAGNITUDE_TIME_MULTIPLIER 	= 0.1;
	protected float WIND_DIRECTION_TIME_MULTIPLIER 	= 0.05;
	
	protected Weather m_Weather;
	protected float m_EnvironmentTemperature;
	protected bool m_EnTempUpdated;
	protected float m_Timer;
	protected float	m_MaxTemps[12];
	protected float m_MinTemps[12];
	protected float m_Sunrise_Jan;
	protected float m_Sunset_Jan;
	protected float m_Sunrise_Jul;
	protected float m_Sunset_Jul;
		
	protected int m_ShortestDayOfYear = 1;
	protected int m_LongestDayOfYear  = 1;
	
	protected ref array<vector> m_FiringPos; // Where we should fire from. On Init set the relevant data
	protected bool m_Pollution;
	protected ref CatchYieldBank m_YieldBank;
	protected ref WorldDataWeatherSettings m_WeatherDefaultSettings;
	protected ref WorldDataLiquidSettings m_LiquidSettings;
	protected ref DustEffectsSettings m_DustEffectsSettings;
	protected ref TStringArray m_DefaultPlayerRestrictedAreas;

	//! weather related
	protected int m_BadWeatherChance;
	protected int m_ClearWeatherChance;
	protected bool m_IsSuddenChange;
	protected float m_WorldWindCoef;
	protected bool m_IsSandstorm;
	protected bool m_IsSandstormStartedByWeather;

	protected float m_UniversalTemperatureSourceCapModifier;	
	
	//used at next weather calculation
	protected int m_SameWeatherCnt = 0;
	protected int m_StepValue = 5;
	protected int m_Chance = 50;
	protected int m_ChoosenWeather = 1;
	protected int m_LastWeather = 0;

	void WorldData()
	{
		Init();
		CreateYieldBank();
		InitYieldBank();
		UpdateBaseEnvTemperature(0);
	}
	
	void Init()
	{
		SetupWeatherSettings();
		SetupLiquidTemperatures();
		SetupDustEffects();

		m_EnTempUpdated = false;
		m_Weather = g_Game.GetWeather();
		m_EnvironmentTemperature = 12.0;
		m_Timer = 0.0;
		m_Sunrise_Jan = 8.54;
		m_Sunset_Jan = 15.52;
		m_Sunrise_Jul = 3.26;
		m_Sunset_Jul = 20.73;
		m_MaxTemps = {3,5,7,14,19,24,26,25,21,16,10,5};
		m_MinTemps = {-3,-2,0,4,9,14,18,17,12,7,4,0};
		m_Pollution = EPollution.NONE;
		m_WorldWindCoef = 0.5;

		m_UniversalTemperatureSourceCapModifier = 0.0;
		
		m_TemperaturePerHeightReductionModifier = 0.02;
		m_CloudsTemperatureEffectModifier 		= 3.0;
		m_TemperatureInsideBuildingsModifier 	= 1.0;
		m_TemperatureUnderSunModifier			= new map<int, float>();
		m_WaterContactTemperatureModifier 		= 20.0;
		
		m_ClearWeatherChance = m_WeatherDefaultSettings.m_ClearWeatherChance;
		m_BadWeatherChance = m_WeatherDefaultSettings.m_BadWeatherChance;
		
		m_DefaultPlayerRestrictedAreas = {};
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

	int GetDaytime()
	{
		int year, month, day, hour, minute;
		g_Game.GetWorld().GetDate(year, month, day, hour, minute);

		WorldData worldData = g_Game.GetMission().GetWorldData();
		float sunriseTimeStart = worldData.GetApproxSunriseTime(month);
		float sunsetTimeStart = worldData.GetApproxSunsetTime(month);

		if (hour >= sunriseTimeStart && hour < (sunriseTimeStart + 2))
			return WorldDataDaytime.DAWN;
		else if (hour >= (sunriseTimeStart + 2) && hour < sunsetTimeStart)
			return WorldDataDaytime.DAY;
		else if (hour >= sunsetTimeStart && hour < (sunsetTimeStart + 2))
			return WorldDataDaytime.DUSK;
			
		return WorldDataDaytime.NIGHT;
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
		if (m_Timer > 30 || !m_EnTempUpdated)
		{
			int year, month, day, hour, minute;
			g_Game.GetWorld().GetDate( year, month, day, hour, minute );
			m_EnvironmentTemperature = CalcBaseEnvironmentTemperature( month + ( day / 32.0 ), hour + ( minute / 60.0 ) );
			m_Timer = 0;
			
			if (!m_EnTempUpdated)
				m_EnTempUpdated = true;
		}
	}
	
	/*!
		\brief Updates *local* weather effects.
		\param weather Weather instance
		\param timeslice Time delta since last update
	*/
	void UpdateWeatherEffects( Weather weather, float timeslice )
	{
		float snowflakeScale = ComputeSnowflakeScale( weather );
		weather.SetSnowflakeScale( snowflakeScale );
	}
	
	/*!
		\brief Returns the desired snowflake scale based on weather simulation state.
		\param weather Weather instance
	*/
	float ComputeSnowflakeScale( Weather weather )
	{
		float overcast01 = Math.Clamp( Math.InverseLerp( 0.4, 1.0, weather.GetOvercast().GetActual() ), 0.0, 1.0 );  // remap range to <0.4, 1.0> snowfall overcast threshold
		float wind01 = weather.GetWindSpeed() / weather.GetWindMaximumSpeed();
		
		float overcastScale = Math.Lerp( 0.50, 1.25, overcast01 );
		float windScale     = Math.Lerp( 1.25, 1.00, wind01 );
		
		return Math.Clamp( overcastScale * windScale, 0.50, 1.25 );
	}

	// getter for the new base enviro temperature
	float GetBaseEnvTemperature()
	{
		return m_EnvironmentTemperature;
	}
	
	float GetBaseEnvTemperatureAtObject(notnull Object object)
	{
		return GetBaseEnvTemperatureAtPosition(object.GetPosition());
	}
	
	float GetBaseEnvTemperatureAtPosition(vector pos)
	{
		float terrainHeight = pos[1];
		float heightCorrection = Math.Max(0, (terrainHeight * m_TemperaturePerHeightReductionModifier));
		return m_EnvironmentTemperature - heightCorrection;
	}

	float GetBaseEnvTemperatureExact(int month, int day, int hour, int minute)
	{
		return CalcBaseEnvironmentTemperature( month + ( day / 32.0 ), hour + ( minute / 60.0 ) );
	}

	float GetLiquidTypeEnviroTemperature(int liquidType)
	{
		if (m_LiquidSettings.m_Temperatures.Count() > 0 && m_LiquidSettings.m_Temperatures.Contains(liquidType) != INDEX_NOT_FOUND)
			return m_LiquidSettings.m_Temperatures.Get(liquidType);
		
		#ifdef DEVELOPER
		ErrorEx("Undefined enviro temperature for liquid type: " + liquidType);
		#endif
		
		return m_EnvironmentTemperature;
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
	
	// Returns chance percentage for selected agent to spawn, used with spawned loot
	float GetAgentSpawnChance(eAgents agent)
	{
		if (agent == eAgents.CHOLERA)
			return SPAWN_CHANCE_CHOLERA_DEF;
		
		return 0;
	}
	
	// Returns modifier which is added to the tool damage logic when player is in cold area
	float GetColdAreaToolDamageModifier()
	{
		return COLD_AREA_TOOL_DMG_MODIF_DEF;
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
	
	int GetPollution()
	{
		return m_Pollution;
	}
	
	float GetWindCoef()
	{
		return m_WorldWindCoef;
	}
	
	float GetUniversalTemperatureSourceCapModifier()
	{
		return m_UniversalTemperatureSourceCapModifier;
	}
	
	/*!
		\brief Return actual temperature of environment based on provided parameters
		\param object Reference to object that is used mainly for sea height related calculation
		\param properties Flag made of EEnvironmentTemperatureComponent which will influence the resulting value of temperature based on combination of the parts
	*/	
	float GetTemperature(Object object, EEnvironmentTemperatureComponent properties = EEnvironmentTemperatureComponent.BASE)
	{
		// EEnvironmentTemperatureComponent.BASE only
		float temperature = GetBaseEnvTemperature();

		if (object && properties & EEnvironmentTemperatureComponent.ALTITUDE)
			temperature = GetBaseEnvTemperatureAtObject(object);
		
		if (properties & EEnvironmentTemperatureComponent.OVERCAST)
			temperature += m_Weather.GetOvercast().GetActual() * m_CloudsTemperatureEffectModifier;

		if (properties & EEnvironmentTemperatureComponent.WIND)
			temperature += WindEffectTemperatureValue(temperature);
		
		if (properties & EEnvironmentTemperatureComponent.FOG)
			temperature += m_Weather.GetFog().GetActual() * GameConstants.ENVIRO_FOG_TEMP_EFFECT;

		return temperature;
	}

	/*!
		\brief Return value of queried EEnvironmentTemperatureComponent which can be used in future calculation(s)
		\param temperature Base temperature which will be used in component calculation (currently WIND only)
		\param properties Flag made of EEnvironmentTemperatureComponent which will influence the resulting value of temperature based on combination of the parts
	*/	
	float GetTemperatureComponentValue(float temperatureIn, EEnvironmentTemperatureComponent properties = 0)
	{
		float temperatureOut = 0.0;

		if ((properties & EEnvironmentTemperatureComponent.OVERCAST) == EEnvironmentTemperatureComponent.OVERCAST)
			temperatureOut = m_Weather.GetOvercast().GetActual() * m_CloudsTemperatureEffectModifier;
		else if ((properties & EEnvironmentTemperatureComponent.WIND) == EEnvironmentTemperatureComponent.WIND)
			temperatureOut = WindEffectTemperatureValue(temperatureIn);
		else if ((properties & EEnvironmentTemperatureComponent.FOG) == EEnvironmentTemperatureComponent.FOG)
			temperatureOut = m_Weather.GetFog().GetActual() * GameConstants.ENVIRO_FOG_TEMP_EFFECT;
		else
		{
			Debug.Log(string.Format("Only OVERCAST, WIND and FOG parameters are supported"));
		}

		return temperatureOut;
	}
	
	float GetSunEffectOnTemperature(int m_DayTime)
	{
		float overcastEffectOnSunModifier = 1 - Math.InverseLerp(m_WeatherDefaultSettings.m_OvercastEffectMinLimitOnSunExposure, m_WeatherDefaultSettings.m_OvercastEffectMaxLimitOnSunExposure, m_Weather.GetOvercast().GetActual());
		overcastEffectOnSunModifier = Math.Clamp(overcastEffectOnSunModifier, 0, 1);
		return m_TemperatureUnderSunModifier[m_DayTime] * overcastEffectOnSunModifier;
	}
	
	/*!
	* \brief Returns number of day in current year (eg. 1 - 365)
	*/
	protected int DayOfYear(int day, int month)
    {
        int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        int dayNumber = 0;

        for (int i = 0; i < month - 1; i++)
        {
            dayNumber += daysInMonth[i];
        }
		
        dayNumber += day;
        return dayNumber;
    }	

	/*!
	* \brief Returns sunrise time for passed day
	*/
	protected float GetSunriseTime(float dayNumber)
	{
		float avg = (m_Sunrise_Jan + m_Sunrise_Jul) / 2.0;
		float amp = (m_Sunrise_Jul - m_Sunrise_Jan) / 2.0;
		float angle = Math.PI2 * (dayNumber - m_LongestDayOfYear) / 365;
		return avg + amp * Math.Cos(angle);
	}

	/*!
	* \brief Returns sunset time for passed day
	*/
	protected float GetSunsetTime( float dayNumber )
	{
		float avg = (m_Sunset_Jan + m_Sunset_Jul) / 2.0;
		float amp = (m_Sunset_Jul - m_Sunset_Jan) / 2.0;
		float angle =  Math.PI2 * (dayNumber - m_LongestDayOfYear) / 365;
		
		return avg - amp * Math.Cos(angle);
	}
	
	protected float WindEffectTemperatureValue(float temperatureInput)
	{
		float temperatureOutput = 0.0;

		temperatureOutput = (temperatureInput - GameConstants.ENVIRO_WIND_CHILL_LIMIT) / (GameConstants.ENVIRO_WIND_EFFECT_SLOPE - GameConstants.ENVIRO_WIND_CHILL_LIMIT);
		temperatureOutput = temperatureOutput * m_Weather.GetWindMagnitude().GetActual() * GetWindCoef();
		
		return -temperatureOutput;
	}
	
	protected void CalculateWind(int newWeather, bool suddenChange, out float magnitude, out float direction);
	
	protected void CalculateVolFog(float lerpValue, float windMagnitude, float changeTime);
	
	protected void CreateYieldBank()
	{
		m_YieldBank = new CatchYieldBank();
	}
	
	//! override this to properly register world-specific yields
	protected void InitYieldBank()
	{
		GetDayZGame().GetYieldDataInitInvoker().Invoke(m_YieldBank); //injects defaults from 4_World and above
	}
	
	protected void SetupWeatherSettings()
	{
		m_WeatherDefaultSettings = new WorldDataWeatherSettings();		
		
		// Obtain the 'sandstormFrequency' from cfgGameplay.json; this defines the rate at which sandstorms should occur,
		// for example:
		//   0.0 : No sandstorms whatsoever
		//   0.5 : Sandstorms are twice as less likely to occur
		//   1.0 : Sandstorms are ocurring at their normal rate
		//   2.0 : Sandstorms are twice as likely to occur
		m_WeatherDefaultSettings.m_sandstormFrequency = CfgGameplayHandler.GetSandstormFrequency();
		
		if ( m_WeatherDefaultSettings.m_sandstormFrequency < 0 )
		{
			m_WeatherDefaultSettings.m_sandstormFrequency = 0;
		}
	}
	
	protected void SetupLiquidTemperatures()
	{
		m_LiquidSettings = new WorldDataLiquidSettings();
		
		m_LiquidSettings.m_Temperatures[LIQUID_SALTWATER] 	= 23.0;
		m_LiquidSettings.m_Temperatures[LIQUID_WATER] 		= 15.0;
		m_LiquidSettings.m_Temperatures[LIQUID_STILLWATER] 	= 15.0;
		m_LiquidSettings.m_Temperatures[LIQUID_RIVERWATER] 	= 15.0;
		m_LiquidSettings.m_Temperatures[LIQUID_FRESHWATER] 	= 15.0;
		m_LiquidSettings.m_Temperatures[LIQUID_CLEANWATER] 	= 10.0;
		m_LiquidSettings.m_Temperatures[LIQUID_SNOW] 		= -5.0;
	}
	
	protected void SetupDustEffects()
	{
		m_DustEffectsSettings = new DustEffectsSettings();
	}
	
	CatchYieldBank GetCatchYieldBank()
	{
		return m_YieldBank;
	}
	
	TStringArray GetDefaultPRAPaths()
	{
		return m_DefaultPlayerRestrictedAreas;
	}
	
	/*!
	* \brief Helper function that starts the sandstorm while setting an internal flag
	*        that allows WorldData identify the source of the call.
	*/
	protected void StartSandstorm(float timeToGather)
	{
		// Set instigator flag to identify that this request
		// is coming from WorldData
		bool lastStartedByWeather     = m_IsSandstormStartedByWeather;
		m_IsSandstormStartedByWeather = true;
		
		// In case of a failure make sure to reset the flag
		// to its original state as this request did not pass
		if (!m_Weather.GetSandstorm().Start(timeToGather, true))
		{
			m_IsSandstormStartedByWeather = lastStartedByWeather;
		}
	}
	/*!
	* \brief Event propagated from SandstormController::OnStart raised when the
	*        sandstorm starts and begins fading in to its target magnitude.
	*/
	sealed void WeatherSandstormStart(bool continuedSession)
	{
		// Read the state of the flag and restore it to default state
		bool isStartedByWeather = false;
		if (!continuedSession)
		{
			isStartedByWeather = m_IsSandstormStartedByWeather;
		}
		else
		{
			isStartedByWeather = true;
		}
		
		m_IsSandstormStartedByWeather = false;
		// Pass this information to the callback
		OnSandstormStart(isStartedByWeather);
	}
	/*!
	* \brief Event propagated from SandstormController::OnStop raised when the
	*        sandstorm fades out and terminates.
	*/
	sealed void WeatherSandstormStop(bool calledFromWeather)
	{
		OnSandstormStop(calledFromWeather);
	}
	/*!
	* \see See `WeatherSandstormStart` instead.
	* \param isStartedByWeather Whether the start is instigated by WorldData (true) or external factors (false).
	*/
	protected void OnSandstormStart(bool isStartedByWeather);
	/*!
	* \see See `WeatherSandstormStop` instead.
	*/
	protected void OnSandstormStop(bool calledFromWeather);


	//!
	//! DEPRECATED
	//!
	
	protected float m_DayTemperature 	= 10;	// legacy, no longer used
	protected float m_NightTemperature	= 6;	// legacy, no longer used

	float GetDayTemperature()
	{
		return m_DayTemperature;
	}

	float GetNightTemperature()
	{
		return m_NightTemperature;
	}
	
	DustEffectsSettings GetDustEffectsSettings()
	{
		return m_DustEffectsSettings;
	}
}

class WorldDataWeatherConstants
{
	const int CLEAR_WEATHER 	= 1;
	const int CLOUDY_WEATHER 	= 2;
	const int BAD_WEATHER 		= 3;
}

class WorldDataWeatherSettings
{
	int m_OvercastMinTime	= 600;
	int m_OvercastMaxTime 	= 900;
	int m_OvercastMinLength = 600;
	int m_OvercastMaxLength = 900;
	
	float m_RainThreshold 	= 0.6;
	int m_RainTimeMin 		= 60;
	int m_RainTimeMax 		= 120;
	int m_RainLengthMin 	= 60;
	int m_RainLengthMax 	= 120;
	
	float m_StormThreshold 			= 0.85;
	float m_ThundersnowThreshold 	= 0.98;
	
	float m_SnowfallThreshold 	= 0.3;
	int m_SnowfallTimeMin 		= 60;
	int m_SnowfallTimeMax 		= 120;
	int m_SnowfallLengthMin 	= 150;
	int m_SnowfallLengthMax 	= 300;

	int m_GlobalSuddenChance		= 95;
	int m_ClearWeatherChance 		= 30;
	int m_BadWeatherChance 			= 80;
	int m_BadWeatherSuddenChance 	= 95;
		
	int m_FoggyMorningHeigthBiasLowLimit = 155;
	int m_DefaultHeigthBias = 170;
	
	int m_CalmAfterStormTimeMin = 480;
	int m_CalmAfterStormTimeMax = 600;
	
	float m_SandstormMinWindDirection = -1.77;
	float m_SandstormMaxWindDirection = -1.20;
	float m_SandstormMinWindMagnitude = 18;
	float m_sandstormFrequency = 1;
	
	float m_OvercastEffectMinLimitOnSunExposure = 0.6;
	float m_OvercastEffectMaxLimitOnSunExposure = 1;
}

class WorldDataLiquidSettings
{
	ref map<int, float> m_Temperatures = new map<int, float>();
}

class WorldDataDaytime
{
	static int ANY			= -1;
	static int NIGHT 		= 0;
	static int DAY 			= 1;
	static int DUSK			= 2;
	static int DAWN			= 3;
	static int MORNING		= 4;
	static int NOON			= 5;
	static int AFTERNOON	= 6;
	static int EVENING		= 7;
	
	static string ToString(int value)
	{
		switch (value)
		{
			case NIGHT:
				return "NIGHT";
			case DAY:
				return "DAY";
			case DUSK:
				return "DUSK";
			case DAWN:
				return "DAWN";
			case MORNING:
				return "MORNING";
			case NOON:
				return "NOON";
			case AFTERNOON:
				return "AFTERNOON";
			case EVENING:
				return "EVENING";
		}
		
		return "ANYTIME";
	}
	
	static bool IsInRange(int current, TIntArray range)
	{
		foreach (int value : range)
		{
			if (current == value)
				return true;
		}
		
		return false;
	}
}

class DustEffectsSettings
{
	bool 	m_GroundEffectEnable = false;
    float 	m_GroundEffectMinWindStrength = 0;
    float	m_GroundEffectDistance = 10;
    float	m_GroundEffectFrequency = 1;
    float	m_GroundEffectWindWeight = 0;
    bool	m_GroundEffectEndPoindTest = true;
    float	m_GroundEffectLifetimeMin = 1;
    float	m_GroundEffectLifetimeMax = 10;
    float	m_GroundEffectSizeMin = 1;
    float	m_GroundEffectSizeMax = 10;
    bool	m_CloudEffectEnable = false;
    float	m_CloudEffectMinWindStrength = 0;
    float	m_CloudEffectCooldownMin = 10;
	float	m_CloudEffectCooldownMax = 35;
    float	m_CloudEffectDurationMin = 3;
	float	m_CloudEffectDurationMax = 7;
    float 	m_CloudEffectDistance = 16;
	bool	m_CloudEffectFromSurface = true;
}