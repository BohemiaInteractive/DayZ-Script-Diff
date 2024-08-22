class InfluenzaAgent : AgentBase
{
	const float INFLUENZA_AGENT_AUTOINFECT_THRESHOLD_HC = PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING;
	
	protected const float INVASIBILITY_INC_LOW_HC 		= 0.20;
	protected const float INVASIBILITY_INC_MEDIUM_HC 	= 0.30;
	protected const float INVASIBILITY_INC_CRITICAL_HC	= 0.75;
	
	protected const float INVASIBILITY_DEC_COMMON_COLD  = -0.40;
	protected const float INVASIBILITY_DEC_INFLUENZA	= -0.50;
	protected const float INVASIBILITY_DEC_PNEUMONIA	= 0.00;

	protected const int MAX_TIME_TO_CONTRACT = 6500;			//! at this time [s], character will contract the disease
	protected const int CONTRACT_TIME_THRESHOLD_MIN = 10;		//! min value of time [s] for subtraction from MAX_TIME_TO_CONTRACT (used for randomization)
	protected const int CONTRACT_TIME_THRESHOLD_MAX = 50;		//! max value of time [s] for subtraction from MAX_TIME_TO_CONTRACT (used for randmization)
	
	protected int m_TimeToContract = MAX_TIME_TO_CONTRACT;

	protected float m_HeatBufferValueLast;
	
	protected const float CONTRACT_HEATCOMFORT_MODIFIER[3] = {
		1.0, 	// low
		1.75, 	// medium
		2.0,	// critical
	};

	protected int m_ElapsedTime;

	override void Init()
	{
		m_Type 					= eAgents.INFLUENZA;
		m_Invasibility 			= 0.33;
		m_TransferabilityIn		= 1;
		m_TransferabilityOut	= 1;
		m_MaxCount 				= 1200;
		m_Digestibility			= 0.1;
		m_AntibioticsResistance = 0;
		m_AutoinfectCount		= CommonColdMdfr.AGENT_THRESHOLD_ACTIVATE;
		m_TransferabilityAirOut = 1;
		m_Potency 				= EStatLevels.MEDIUM;
		m_DieOffSpeed 			= 0.66;
		
		m_ElapsedTime			= 0.0;
	}

	override bool AutoinfectCheck(float deltaT, PlayerBase player)
	{
		float heatcomfort = player.GetStatHeatComfort().Get();
		if (heatcomfort < PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING)
		{
			float subtractTimeModifier;
			float subtractTimeRandomized = Math.RandomFloatInclusive(CONTRACT_TIME_THRESHOLD_MIN, CONTRACT_TIME_THRESHOLD_MAX);
			
			if (heatcomfort <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING && heatcomfort > PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL)
				subtractTimeModifier = CONTRACT_HEATCOMFORT_MODIFIER[0];
			else if (heatcomfort <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL && heatcomfort > PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_EMPTY)
				subtractTimeModifier = CONTRACT_HEATCOMFORT_MODIFIER[1];
			else if (heatcomfort <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_EMPTY)
				subtractTimeModifier = CONTRACT_HEATCOMFORT_MODIFIER[2];
			
			float subtractTimeModified = subtractTimeRandomized * subtractTimeModifier;

			m_TimeToContract -= subtractTimeModified;
			if (m_ElapsedTime >= m_TimeToContract)
			{
				m_ElapsedTime = 0;
				m_TimeToContract = MAX_TIME_TO_CONTRACT;

				return true;
			}

			m_ElapsedTime += deltaT;
		}
		else
		{
			m_ElapsedTime = Math.Clamp(m_ElapsedTime - deltaT, 0.0, float.MAX);
			m_TimeToContract = Math.Clamp(m_TimeToContract + deltaT, 0.0, MAX_TIME_TO_CONTRACT);
		}
		
		return false;
	}

	override bool CanAutoinfectPlayer(PlayerBase player)
	{
		if (player.GetStatHeatComfort().Get() < INFLUENZA_AGENT_AUTOINFECT_THRESHOLD_HC)
			return true;

		return false;
	}
	
	override float GetInvasibilityEx(PlayerBase player)
	{
		if (player.GetModifiersManager().IsModifierActive(eModifiers.MDF_PNEUMONIA))
			return INVASIBILITY_DEC_PNEUMONIA;
		
		if (player.GetModifiersManager().IsModifierActive(eModifiers.MDF_INFLUENZA))
			return INVASIBILITY_DEC_INFLUENZA;
		
		float heatBufferNormalized = player.GetStatHeatBuffer().Get() / player.GetStatHeatBuffer().GetMax();;
		if (heatBufferNormalized >= m_HeatBufferValueLast && heatBufferNormalized >= HeatBufferMdfr.STAGE_THRESHOLDS[1])
		{
			m_HeatBufferValueLast = heatBufferNormalized;
			return INVASIBILITY_DEC_COMMON_COLD;
		}
		
		m_HeatBufferValueLast = heatBufferNormalized;

		float heatComfort = player.GetStatHeatComfort().Get();

		if (heatComfort <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING && heatComfort > PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL)
			return INVASIBILITY_INC_LOW_HC;
		else if (heatComfort <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL && heatComfort > PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_EMPTY)
			return INVASIBILITY_INC_MEDIUM_HC;
		else if (heatComfort <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_EMPTY)
			return INVASIBILITY_INC_CRITICAL_HC;
		
		return INVASIBILITY_DEC_COMMON_COLD;
	}
	
	override EStatLevels GetPotencyEx(PlayerBase player)
	{
		if (player.GetModifiersManager().IsModifierActive(eModifiers.MDF_PNEUMONIA))
			return EStatLevels.GREAT;
		
		return super.GetPotencyEx(player);
	}
}