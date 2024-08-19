class InfluenzaAgent extends AgentBase
{
	const float INFLUENZA_AGENT_AUTOINFECT_THRESHOLD_HC = PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING;

	override void Init()
	{
		m_Type 					= eAgents.INFLUENZA;
		m_Invasibility 			= 0.33;
		m_TransferabilityIn		= 1;
		m_TransferabilityOut	= 1;
		m_MaxCount 				= 1000;
		m_Digestibility			= 0.1;
		m_AntibioticsResistance = 0;
		m_AutoinfectProbability = CalculateAutoinfectProbability( 0.40 );
		m_TransferabilityAirOut = 1;
		m_Potency 				= EStatLevels.MEDIUM;
		m_DieOffSpeed 			= 0.66;
	}

	override bool CanAutoinfectPlayer(PlayerBase player)
	{
		if ( player.GetStatHeatComfort().Get() < INFLUENZA_AGENT_AUTOINFECT_THRESHOLD_HC )
		{
			return true;			
		}
		else
		{
			return false;	
		}
	}
}