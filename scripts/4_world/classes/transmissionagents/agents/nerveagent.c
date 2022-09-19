class NerveAgent extends AgentBase
{
	override void Init()
	{
		m_Type 					= eAgents.NERVE_AGENT;
		m_Invasibility 			= 0;
		m_TransferabilityIn		= 0.1;
		m_TransferabilityOut	= 1;
		m_TransferabilityAirOut	= 0;
		m_AntibioticsResistance = 0.5;//override in a func. GetAntiboticsResistance()
		m_MaxCount 				= 500;
		m_Potency 				= EStatLevels.CRITICAL;
		m_DieOffSpeed 			= 0.1;
	}
	

	
}