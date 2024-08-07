class HMP3Symptom extends SymptomBase
{
	protected float m_EffectDuration;
	protected float m_EffectTime;
	protected float m_EffectStartTime;
	protected float m_Time;
	
	protected PPERequester_HMPGhosts 	m_Requester;
	
	const int EFFECT_DURATION_TIME_MIN = 6;
	const int EFFECT_DURATION_TIME_MAX = 10;
	
	const int MIN_TIME_BETWEEN_EFFECTS = 24.0;
	const int MAX_TIME_BETWEEN_EFFECTS = 30.0;
	
	//this is just for the Symptom parameters set-up and is called even if the Symptom doesn't execute, don't put any gameplay code in here
	override void OnInit()
	{
		m_SymptomType = SymptomTypes.SECONDARY;
		m_Priority = 0;
		m_ID = SymptomIDs.SYMPTOM_HMP_SEVERE;
		m_IsPersistent = false;
		m_SyncToClient = true;
		m_MaxCount = 1;
		
		if (!GetGame().IsDedicatedServer())
		{
			Class.CastTo(m_Requester,PPERequesterBank.GetRequester(PPERequesterBank.REQ_HMP_GHOST));
		}
	}
	
	override void OnUpdateClient(PlayerBase player, float deltatime)
	{
		m_Time += deltatime;
		if (m_EffectStartTime <= 0)
		{
			m_EffectStartTime = Math.RandomFloatInclusive(MIN_TIME_BETWEEN_EFFECTS, MAX_TIME_BETWEEN_EFFECTS);
			m_EffectDuration = Math.RandomFloatInclusive(EFFECT_DURATION_TIME_MIN, EFFECT_DURATION_TIME_MAX);
		}
		
		if (m_EffectStartTime > 0 && m_Time > m_EffectStartTime)
		{
			m_EffectTime += deltatime / m_EffectDuration;
			m_Requester.SetPulseProgress(Math.Clamp(m_EffectTime,0,1));
			
			if (m_EffectTime >= 1)
			{
				m_EffectTime = 0;
				m_EffectStartTime = -1;
				m_Time = 0;
			}
		}
	}
	
	//!gets called once on an Symptom which is being activated
	override void OnGetActivatedServer(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
	}

	override void OnGetActivatedClient(PlayerBase player)
	{
		m_Requester.Start();
		if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
	}

	override void OnGetDeactivatedServer(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());
	}
	
	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedClient(PlayerBase player)
	{
		m_Requester.FadeOutEffect(3);
		if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());
	}
}
