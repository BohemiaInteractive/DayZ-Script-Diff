class ChemicalAgent extends AgentBase
{
	override void Init()
	{
		m_Type 					= eAgents.CHEMICAL_POISON;
		m_Invasibility 			= 0;
		m_TransferabilityIn		= 0.1;
		m_TransferabilityOut	= 1;
		m_TransferabilityAirOut	= 1;
		m_AntibioticsResistance = 1;
		m_MaxCount 				= 500;
		m_Potency 				= EStatLevels.CRITICAL;
		m_DieOffSpeed 			= 0.1;
	}
	
	override float GetDieOffSpeedEx(PlayerBase player)
	{
		if( player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION2) || player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION3))
		{
			return 0;
		}
		else
		{
			return super.GetDieOffSpeedEx(player);//round-about way of getting the m_DieOffSpeed
		}
	}
}