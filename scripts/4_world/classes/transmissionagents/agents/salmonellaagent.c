class SalmonellaAgent extends AgentBase
{
	override void Init()
	{
		m_Type 					= eAgents.SALMONELLA;
		m_Invasibility 			= 0.75;
		m_TransferabilityIn		= 0.1;
		m_TransferabilityOut	= 0.1;
		m_AntibioticsResistance = 1;
		m_MaxCount 				= 300;
		m_Potency 				= EStatLevels.HIGH;
		m_DieOffSpeed 			= 1;
	}
}