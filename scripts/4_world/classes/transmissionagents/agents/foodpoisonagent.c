class FoodPoisonAgent : AgentBase
{
	override void Init()
	{
		m_Type 					= eAgents.FOOD_POISON;
		m_Invasibility 			= 1.0;
		m_TransferabilityIn		= 1.0;
		m_TransferabilityOut	= 0.0;
		m_AntibioticsResistance = 1.0; //deprecated, use m_DrugResistances to initialize this agents resistance to a given drug
		m_MaxCount 				= 800;
		m_Potency 				= EStatLevels.GREAT;
		m_DieOffSpeed 			= 0.0;
		m_Digestibility 		= 0.5;

	}

	override EStatLevels GetPotencyEx(PlayerBase player)
	{
		if (player.GetStomach().GetVolumeContainingAgent01(eAgents.FOOD_POISON) == 0.0)
			return EStatLevels.LOW;
		
		return super.GetPotencyEx(player);
	}
	
	override float GetInvasibilityEx(PlayerBase player)
	{
		return player.GetStomach().GetVolumeContainingAgent01(eAgents.FOOD_POISON);
	}
	
	override float GetDieOffSpeedEx(PlayerBase player)
	{
		if (player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CHARCOAL))
			return 0.5;
		
		if (player.m_AgentPool.GetSingleAgentCount(eAgents.FOOD_POISON) <= PoisoningMdfr.AGENT_THRESHOLD_DEACTIVATE)
			return 0.3;

		return 0.0;
	}
	
	override float GetDigestibilityEx(PlayerBase player)
	{
		if (!player.GetModifiersManager().IsModifierActive(eModifiers.MDF_POISONING))
			return 1.0;
		
		return super.GetDigestibilityEx(player);
	}
}