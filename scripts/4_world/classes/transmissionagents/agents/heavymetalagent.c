class HeavyMetalAgent : AgentBase
{
	override void Init()
	{
		m_Type 					= eAgents.HEAVYMETAL;
		m_TransferabilityIn		= 0.5;
		m_TransferabilityOut	= 0;
		m_AntibioticsResistance = 1; //deprecated, use m_DrugResistances to initialize this agents resistance to a specific drug, as seen on lines 11
		m_MaxCount 				= 600;
		m_Potency 				= EStatLevels.MEDIUM;
		m_DrugResistances.Set(EMedicalDrugsType.CHELATION, 0.0);
	}
	
	override bool GrowDuringMedicalDrugsAttack(EMedicalDrugsType drugType, PlayerBase player)
	{
		if (drugType == EMedicalDrugsType.CHELATION)
			return false;

		return super.GrowDuringMedicalDrugsAttack(drugType, player);
	}
	
	override float GetInvasibilityEx(PlayerBase player)
	{
		if (player.GetModifiersManager().IsModifierActive(eModifiers.MDF_HEAVYMETAL3))
			return 0;
		
		return player.GetStomach().GetVolumeContainingAgent01(eAgents.HEAVYMETAL);
	}
	
	override EStatLevels GetPotencyEx(PlayerBase player)
	{
		if (player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CHELATION))
			return EStatLevels.LOW;
		
		if (player.GetModifiersManager().IsModifierActive(eModifiers.MDF_HEAVYMETAL3))
			return EStatLevels.GREAT;
		
		if (player.GetStomach().GetVolumeContainingAgent01(eAgents.HEAVYMETAL) > 0.0)
			return EStatLevels.GREAT;
		
		return m_Potency;
	}
	
	override float GetDieOffSpeedEx(PlayerBase player)
	{
		if (player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CHELATION))
			return GetDieOffSpeed() * 0.3;

		return super.GetDieOffSpeedEx(player);
	}
}
