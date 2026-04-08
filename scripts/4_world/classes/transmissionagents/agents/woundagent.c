class WoundAgent : AgentBase
{
	static const float RESISTANCE_STAGE_1 = 1;
	static const float RESISTANCE_STAGE_2 = 0.5;
	
	override void Init()
	{
		m_Type 					= eAgents.WOUND_AGENT;
		m_Invasibility 			= 0.208;//to reach 250 in 20 mins
		m_TransferabilityIn		= 1;
		m_TransferabilityOut	= 0;
		m_AntibioticsResistance = 0.5; //deprecated, use m_DrugResistances to initialize this agents resistance to a specific drug, as seen on lines 16
		m_MaxCount 				= 500;
		m_Potency 				= EStatLevels.GREAT;
		m_DieOffSpeed 			= 1;
		m_DrugResistances.Set(EMedicalDrugsType.ANTIBIOTICS, 0.5);	
	}
	
	override float GetAntibioticsResistanceEx(PlayerBase player)
	{
		if (player.GetModifiersManager().IsModifierActive(eModifiers.MDF_WOUND_INFECTION1))
			return RESISTANCE_STAGE_1;
		else
			return RESISTANCE_STAGE_2;
	}
	
	override bool GrowDuringMedicalDrugsAttack(EMedicalDrugsType drugType, PlayerBase player)
	{
		if (drugType == EMedicalDrugsType.ANTIBIOTICS)
		{
			if (player.GetModifiersManager().IsModifierActive(eModifiers.MDF_WOUND_INFECTION1))
				return true;
			else
				return false;
		}
		
		return super.GrowDuringMedicalDrugsAttack(drugType, player);
	}
}