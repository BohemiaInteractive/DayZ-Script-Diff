class CholeraAgent extends AgentBase
{
	override void Init()
	{
		m_Type 					= eAgents.CHOLERA;
		m_Invasibility 			= 0.15;
		m_TransferabilityIn		= 0.1;
		m_TransferabilityOut	= 0.1;
		m_AntibioticsResistance = 0; //deprecated, use m_DrugResistances to initialize this agents resistance to a given drug, as seen on lines 13 and 14
		m_MaxCount 				= 1000;
		m_Potency 				= EStatLevels.HIGH;
		m_DieOffSpeed 			= 0.45;
		m_DrugResistances.Set(EMedicalDrugsType.ANTIBIOTICS, 0.0);
	}
}