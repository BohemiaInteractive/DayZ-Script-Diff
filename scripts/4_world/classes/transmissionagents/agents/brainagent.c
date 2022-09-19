class BrainAgent extends AgentBase
{
	override void Init()
	{
		m_Type 					= eAgents.BRAIN;
		m_Invasibility 			= 1;
		m_TransferabilityIn		= 1;
		m_TransferabilityOut	= 0;
		m_AntibioticsResistance = 1;
		m_MaxCount 				= 5000;
		m_Potency 				= EStatLevels.GREAT;
		m_DieOffSpeed 			= 1;
	}
}