class SilicosisAgent : AgentBase
{
	override void Init()
	{
		m_Type 					= eAgents.SILICOSIS;
		m_Invasibility 			= 0.0;
		m_TransferabilityIn		= 3.5;
		m_TransferabilityOut	= 0.0;
		m_MaxCount 				= 1000;
		m_Digestibility			= 0.0;
		m_AntibioticsResistance = 0;
		m_AutoinfectCount		= 0.0;
		m_TransferabilityAirOut = 0.0;
		m_Potency 				= EStatLevels.LOW;
		m_DieOffSpeed 			= 0.9;
	}
	
	override float GetDieOffSpeedEx(PlayerBase player)
	{
		if (player.IsUnconscious())
			return 0.0;
		
		return super.GetDieOffSpeedEx(player);
	}
}