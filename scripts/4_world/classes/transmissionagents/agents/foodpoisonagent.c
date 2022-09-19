class FoodPoisonAgent extends AgentBase
{
	override void Init()
	{
		m_Type 					= eAgents.FOOD_POISON;
		m_Invasibility 			= 1;
		m_TransferabilityIn		= 1;
		m_TransferabilityOut	= 0;
		m_AntibioticsResistance = 0.5;
		m_MaxCount 				= 400;
		m_Potency 				= EStatLevels.CRITICAL;
		m_DieOffSpeed 			= 1;
		m_Digestibility 		= 1;
	}
}