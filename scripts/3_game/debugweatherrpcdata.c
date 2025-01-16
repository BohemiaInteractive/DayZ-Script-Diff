class DebugWeatherRPCData
{
	float m_FogValue 					= -1;
	float m_OvercastValue 				= -1;
	float m_RainValue 					= -1;
	float m_SnowfallValue				= -1;
	
	float m_FogInterpolation 			= 0;
	float m_OvercastInterpolation 		= -1;
	float m_RainInterpolation 			= -1;
	float m_SnowfallInterpolation		= -1;
	
	float m_FogDuration 				= -1;
	float m_OvercastDuration			= -1;
	float m_RainDuration 				= -1;
	float m_SnowfallDuration			= -1;
	
	float m_VolFogDistanceDensity 		= -1;
	float m_VolFogDistanceDensityTime 	= -1;

	float m_VolFogHeightDensity 		= -1;
	float m_VolFogHeightDensityTime 	= -1;

	float m_VolFogHeightBias 			= -1;
	float m_VolFogHeightBiasTime		= -1;
	
	float m_StormDensity				= -1;
	float m_StormThreshold				= -1;
	float m_StormTimeOut				= -1;
	
	float m_WindMagnitudeValue			= -1;
	float m_WindMInterpolation			= -1;
	float m_WindMDuration				= -1;
	
	float m_WindDirectionValue			= -1;
	float m_WindDInterpolation			= -1;
	float m_WindDDuration				= -1;
}