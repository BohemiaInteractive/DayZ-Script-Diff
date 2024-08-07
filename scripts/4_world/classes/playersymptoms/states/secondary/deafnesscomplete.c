class DeafnessCompleteSymptom : SymptomBase
{
	override void OnInit()
	{
		m_SymptomType 	= SymptomTypes.SECONDARY;
		m_Priority 		= 0;
		m_ID 			= SymptomIDs.SYMPTOM_DEAFNESS_COMPLETE;
		m_IsPersistent 	= false;
		m_MaxCount		= 1;

		m_DestroyOnAnimFinish = true;
	}
	
	override void OnGetActivatedClient(PlayerBase player)
	{
		//m_Player.SetMasterAttenuation("DeafnessCompleteAttenuation");
		m_Player.SetMasterAttenuation("FlashbangAttenuation");
	}
	
	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedClient(PlayerBase player)
	{
		m_Player.SetMasterAttenuation("");
	}
}
