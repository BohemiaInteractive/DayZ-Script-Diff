class CholeraAgent extends AgentBase
{
	override void Init()
	{
		m_Type 					= eAgents.CHOLERA;
		m_Invasibility 			= 0.15;
		m_TransferabilityIn		= 0.1;
		m_TransferabilityOut	= 0.1;
		m_AntibioticsResistance = 0;
		m_MaxCount 				= 1000;
		m_Potency 				= EStatLevels.HIGH;
		m_DieOffSpeed 			= 0.45;
	}
}