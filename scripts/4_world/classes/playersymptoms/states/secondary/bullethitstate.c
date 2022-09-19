/*
class BulletHitSymptom extends SymptomBase
{
	float m_HitDuration;
	float m_BreakPoint;
	float m_TimeActive;
	//this is just for the Symptom parameters set-up and is called even if the Symptom doesn't execute, don't put any gameplay code in here
	override void OnInit()
	{
		m_SymptomType = SymptomTypes.SECONDARY;
		m_Priority = 0;
		m_ID = SymptomIDs.SYMPTOM_BULLET_HIT;
		m_DestroyOnAnimFinish = true;
		m_SyncToClient = false;
		m_HitDuration = 0.4;
		m_BreakPoint = 0.1;
	}
	
	override void OnUpdateClient(PlayerBase player, float deltatime)
	{
		m_TimeActive += deltatime;
		if(m_TimeActive >= m_HitDuration)
		{
			RequestDestroy();
		}
		
		float value;
		
		if( m_TimeActive <= m_BreakPoint )
		{
			value = Math.InverseLerp(0, m_BreakPoint, m_TimeActive);
		}
		else
		{
			float tmp_value = Math.InverseLerp(m_BreakPoint, m_HitDuration, m_TimeActive);
			value = 1 - tmp_value;
		}
		
		PPEffects.HitEffect(value);
		
	}
	

	override void OnGetActivatedClient(PlayerBase player)
	{
		//PPEffects.EnableBurlapSackBlindness();
		//PPEffects.HitEffect(1);
		
	}

	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedClient(PlayerBase player)
	{
		PPEffects.HitEffect(0);
		Debug.Log("OnGetDeactivated CoughSymptom called", "PlayerSymptom");
	}
	
}
*/