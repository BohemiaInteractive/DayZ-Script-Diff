/*!
	All classes related to game weather
*/


//-----------------------------------------------------------------------------
/*!
	Weather phenomenon type
*/
enum EWeatherPhenomenon
{
	OVERCAST,
	FOG,
	RAIN,
	SNOWFALL,
	WIND_DIRECTION,
	WIND_MAGNITUDE,
	VOLFOG_HEIGHT_DENSITY,
	VOLFOG_DISTANCE_DENSITY,
	VOLFOG_HEIGHT_BIAS	
};


//-----------------------------------------------------------------------------
/*!
	Weather phenomenon
*/
class WeatherPhenomenon
{
	private void WeatherPhenomenon() {}
	private void ~WeatherPhenomenon() {}
	
	//! Returns type of this phenomenon.
	proto native EWeatherPhenomenon GetType();

	//! Returns actual value of phenomenon in range <0, 1>.
	//! (does not apply for Wind, which always returns the magnitude values)
	proto native float GetActual();

	//! Returns a forecast value the phenomenon is heading towards.
	proto native float GetForecast();

	/*!
		\brief Sets the forecast.
		\param forecast     Desired forecast value that should be met in given time.
		\param time         A time of the next change (how long it takes in seconds to interpolate to given value).
		\param minDuration  A minimal time in seconds the change will last.
	*/
	proto native void Set( float forecast, float time = 0, float minDuration = 0 );

	//! Reads the time in seconds when the next forecast will be computed.
	proto native float GetNextChange();
	//! Sets the time in seconds when the next forecast will be computed.
	proto native void SetNextChange( float time );

	/*!
		\brief Reads limits of this phenomenon.
		\param fnMin Function minimum (in range <0, 1>).
		\param fnMax Function maximum (in range <0, 1>).
	*/
	proto void GetLimits( out float fnMin, out float fnMax );
	/*!
		\brief Sets limits of this phenomenon.

		Actual value of this phenomenon will be always held in range <fnMin, fnMax>.

		Default values are:
			fnMin  = 0
			fnMax  = 1

		\param fnMin Function minimum (in range <0, 1>).
		\param fnMax Function maximum (in range <0, 1>).
	*/
	proto native void SetLimits( float fnMin, float fnMax );

	/*!
		\brief Reads limits of change when forecast is computed.
		\param fcMin Forecast change minimum (in range <0, 1>).
		\param fcMax Forecast change maximum (in range <0, 1>).
	*/
	proto void GetForecastChangeLimits( out float fcMin, out float fcMax );
	/*!
		\brief Sets limits of change when forecast is computed.

		These limits govern how much the forecast value can change
		when it is computed by weather controller.

		Default values are:
			fcMin  = 0
			fcMax  = 1

		\param fcMin Forecast change minimum (in range <0, 1>).
		\param fcMax Forecast change maximum (in range <0, 1>).
	*/
	proto native void SetForecastChangeLimits( float fcMin, float fcMax );

	/*!
		\brief Reads time range in which next forecast can be computed.
		\param ftMin Minimal number of seconds.
		\param ftMax Maximal number of seconds.
	*/
	proto void GetForecastTimeLimits( out float ftMin, out float ftMax );
	/*!
		\brief Sets time range in which next forecast can be computed.

		Default values are:
			ftMin  = 300  (5 minutes)
			ftMax  = 3600 (1 hour)

		\param ftMin Minimal number of seconds.
		\param ftMax Maximal number of seconds.
	*/
	proto native void SetForecastTimeLimits( float ftMin, float ftMax );

	/*!
		\brief Is called every time the Phenomenon computes new forecast.

		\note Called on server only.

		\param change Computed change of forecast value.
		\param time   Seconds when the next forecast will be computed.
		\return True when script modifies state of this phenomenon false otherwise.
	*/

	bool OnBeforeChange( float change, float time )
	{
		// check if mission forces use of custom weather
		Weather weather = g_Game.GetWeather();
		
		if ( weather.GetMissionWeather() )
			return false;
		
		// check for active worlddata with custom onbeforechange behaviour
		Mission currentMission = g_Game.GetMission();

		if ( currentMission )
		{
			WorldData worldData = currentMission.GetWorldData();
			if ( worldData )
			{
				return worldData.WeatherOnBeforeChange( GetType(), GetActual(), change, time );
			}
		}

		return false;
	}
};


typedef WeatherPhenomenon Overcast;
typedef WeatherPhenomenon Fog;
typedef WeatherPhenomenon Rain;
typedef WeatherPhenomenon Snowfall;
typedef WeatherPhenomenon WindDirection;
typedef WeatherPhenomenon WindMagnitude;




//-----------------------------------------------------------------------------
/*!
	Weather controller
*/
class Weather
{
	protected bool m_missionWeather;
	
	private void Weather()
	{
		m_missionWeather = false;
	}
	
	private void ~Weather() {}
	
	//! Returns actual time from start of a server (how many seconds elapsed from server start).
	proto native float GetTime();

	//! Returns an overcast phenomenon object.
	proto native Overcast GetOvercast();

	//! Returns a fog phenomenon object.
	proto native Fog GetFog();

	//! Returns a rain phenomenon object.
	proto native Rain GetRain();
	
	//! Returns a snowfall phenomenon object.
	proto native Snowfall GetSnowfall();
	
	/*!
		\brief Returns a wind direction phenomenon object.
		\note Wind direction is angle in radians, typically in the <-PI, +PI> interval.
		\see See Weather.WindDirectionToAngle and Weather.AngleToWindDirection for conversions.
	*/
	proto native WindDirection GetWindDirection();
	
	/*!
		\brief Returns a wind magnitude phenomenon object.
		\note Wind magnitude is the absolute speed in m/s.
	*/
	proto native WindMagnitude GetWindMagnitude();

	/*!
		\brief Sets the thunderstorm properties.
		\param  density   A value in <0, 1> range where 0 means no thunderstorms at all
		                  and 1 means thunderstorm every time it gets cloudy.
		\param  threshold The overcast value that must be exceeded so that lightning can appear.
		\param  timeOut   A minimal time in seconds between lightning during thunderstorm.
	*/
	proto native void SetStorm( float density, float threshold, float timeOut );
	
	//! enables/disables thunderbolt simulation on client (together with sounds)
	proto native void SuppressLightningSimulation(bool state);

	//! Returns wind vector (direction and speed as length of the vector).
	proto native vector GetWind();
	
	/*!
		\brief Sets the wind vector (direction and speed as length of the vector).	
		\note Equivalent to setting WindMagnitude and WindDirection phenomenon(s) with zero time and duration.
		\see See Weather.WindDirectionToAngle and Weather.AngleToWindDirection for conversions.
		\code
			float len = wind.Normalize();
			if ( len > 0 )
			{
				float angle = WindDirectionToAngle( wind );
				GetWindMagnitude().Set( len, 0, 0 );
				GetWindDirection().Set( angle, 0, 0 );
			}
		\endcode
	*/
	proto native void SetWind( vector wind );
	
	/*!
		\brief Returns actual wind speed in metre per second.
		\note Wind is changing continuously in time, so the returned value may not stand for too long.
              Equivalent to WindMagnitude phenomenon actual value.
	*/
	proto native float GetWindSpeed();
	
	/*!
		\brief Sets the actual wind speed in metre per second.
		\note Wind is changing continuously in time, so the returned value may not stand for too long.
		      Minimum speed for wind is 0.1 m/s.
		      Equivalent to setting WindMagnitude phenomenon(s) with zero time and duration.
	*/
	proto native void SetWindSpeed( float speed );
	
	/*!
		\brief Returns maximal wind speed in metre per second.
		\note By default this value is 10 m/s but it can be overridden in script on mission initialization.
	          Equivalent to getting WindMagnitude phenomenon upper limit.
	*/
	proto native float GetWindMaximumSpeed();
	
	/*!
		\brief Sets the maximal wind speed in metre per second.
               Equivalent to setting WindMagnitude phenomenon(s) value upper limit.
	*/
	proto native void SetWindMaximumSpeed( float maxSpeed );
	
	/*!
		\brief Reads function parameters that controls the wind behaviour (change in time).
		\param fnMin   Function relative minimum (in range <0, 1>).
		\param fnMax   Function relative maximum (in range <0, 1>).
		\param fnSpeed Controls speed of change of function value.
	*/
	proto void GetWindFunctionParams( out float fnMin, out float fnMax, out float fnSpeed );
	
	/*!
		\brief Sets function parameters that controls the wind behaviour (change in time).
		\param fnMin   Function relative minimum (in range <0, 1>).
		\param fnMax   Function relative maximum (in range <0, 1>).
		\param fnSpeed Controls speed of change of function value.
	*/
	proto native void SetWindFunctionParams( float fnMin, float fnMax, float fnSpeed );

	/*!
		\brief Sets overcast threshold values for rain phenomena.

		Rain can start only if actual overcast value is in given range of <tMin, tMax>.
		If it's already raining and actual overcast value gets out of given range
		then rain will stop in given tTime seconds.

		Default values are:
			tMin  = 0.6
			tMax  = 1
			tTime = 30

		\param tMin  Minimal overcast value (in range <0, 1>).
		\param tMax  Maximal overcast value (in range <0, 1>).
		\param tTime Time in seconds when it stops raining.
	*/
	proto native void SetRainThresholds( float tMin, float tMax, float tTime );
	
	/*!
		\brief Sets overcast threshold values for snowfall phenomena.

		Snowfall can start only if actual overcast value is in given range of <tMin, tMax>.
		If it's already snowing and actual overcast value gets out of given range
		then snowfall will stop in given tTime seconds.

		Default values are:
			tMin  = 0.6
			tMax  = 1
			tTime = 30

		\param tMin  Minimal overcast value (in range <0, 1>).
		\param tMax  Maximal overcast value (in range <0, 1>).
		\param tTime Time in seconds when it stops snowing.
	*/
	proto native void SetSnowfallThresholds( float tMin, float tMax, float tTime );	
	
	/*!
		\brief Sets the overall scale of snowflakes during snowfall phenomenon.
				This value is not synchronized and should be set by deterministic means.
		\param scale Scale, 1.0 = default
	*/
	proto native void SetSnowflakeScale(float scale);
	
	/*!
		\brief Returns the overall scale of snowflakes during snowfall phenomenon.
	*/
	proto native float GetSnowflakeScale();
	
	/*!
	   \brief Returns the xz angle of the provided wind vector.
	   \param dir Non-zero wind vector
	   \return Wind angle in the -PI, +PI interval.
	 */
	static proto float WindDirectionToAngle( vector dir );
	
	/*!
	   \brief Returns wind direction from the provided wind angle.
	   \param angle Wind angle in the -PI, +PI interval.
	   \return Wind direction vector.
	 */
	static proto vector AngleToWindDirection( float angle );
	
	
	/*!
	 * \brief Dynamic volumetric fog only takes effect if enabled in the world config.
	 * \return Whether the dynamic volumetric fog is enabled.
	 */
	proto native bool IsDynVolFogEnabled();
	/*!
	 * \brief Sets the dynamic volumetric fog distance density. Only takes effect if dynamic volumetric fog is enabled.
	 * \param value Density percentage in <0,1> range.
	 * \param time Transition time in seconds. (0 for immediate effect)
	*/
	proto native void SetDynVolFogDistanceDensity(float value, float time = 0);
	/*!
		\brief Returns the current 'dynamic' volumetric fog distance density.
	*/
	proto native float GetDynVolFogDistanceDensity();
	/*!
	 * \brief Sets the dynamic volumetric fog height density. Only takes effect if dynamic volumetric fog is enabled.
	 * \param value Density percentage in <0,1> range.
	 * \param time Transition time in seconds. (0 for immediate effect)
	*/
	proto native void SetDynVolFogHeightDensity(float value, float time = 0);
	/*!
		\brief Returns the current 'dynamic' volumetric fog height density.
	*/
	proto native float GetDynVolFogHeightDensity();
	/*!
		\brief Sets the 'dynamic' volumetric height bias.
			   Takes effect only if enabled via world config.
		\param value Height offset in meters relative to 0.
		\param time Transition time in seconds.
	*/
	proto native void SetDynVolFogHeightBias(float value, float time = 0);
	/*!
		\brief Returns the current 'dynamic' volumetric fog height bias in meters.
	*/
	proto native float GetDynVolFogHeightBias();
	
	
	void MissionWeather( bool use )
	{
		m_missionWeather = use;
	}
	
	bool GetMissionWeather()
	{
		return m_missionWeather;
	}
	
	// Noise reduction due to environmental conditions, used for AI noise evaluation
	float GetNoiseReductionByWeather()
	{
		float rainReduction = GetRain().GetActual() * GameConstants.RAIN_NOISE_REDUCTION_WEIGHT;
		float snowfallReduction = GetSnowfall().GetActual() * GameConstants.SNOWFALL_NOISE_REDUCTION_WEIGHT;
		
		if (rainReduction == 0 && snowfallReduction == 0)
			return 1;
		
		if (rainReduction > snowfallReduction)	// combined phenomenons dont need to have multiplicative effects
			return 1 - rainReduction;
		else
			return 1 - snowfallReduction;
	}
};
