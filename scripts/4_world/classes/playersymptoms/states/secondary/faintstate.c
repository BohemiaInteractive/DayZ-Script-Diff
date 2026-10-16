class FaintSymptom : SymptomBase
{
	private float m_Intensity;
	private float m_FadeoutDuration;
	private float m_Interval;
	
	private ref PPERequester_UnconEffects m_FaintEffect;
	
	//this is just for the Symptom parameters set-up and is called even if the Symptom doesn't execute, don't put any gameplay code in here
	override void OnInit()
	{
		m_Priority 				= 0;
		m_SymptomType 			= SymptomTypes.SECONDARY;
		m_ID 					= SymptomIDs.SYMPTOM_FAINT;
		m_DestroyOnAnimFinish 	= false;
		m_IsPersistent 			= false;
		m_ServerUpdateInterval	= 5.0;
		m_SyncToClient			= true;
		
		if (!g_Game.IsDedicatedServer())
			m_FaintEffect = PPERequester_UnconEffects.Cast(PPERequesterBank.GetRequester(PPERequester_UnconEffects));
	}
	
	override void SetParam(Param p)
	{
		Param2<int, ref SymptomSyncData> params = Param2<int, ref SymptomSyncData>.Cast(p);
		if (params && params.param1 == SymptomIDs.SYMPTOM_FAINT)
		{
			if (!g_Game.IsDedicatedServer())
			{
				SymptomSyncData faintData = params.param2;
				if (faintData.m_IntArray[0] == 1)
				{
					if (!m_FaintEffect.IsRequesterRunning())
					{
						Param1<float> vignette = new Param1<float>(0.0);
						m_FaintEffect.Start(vignette);
					}
					m_FaintEffect.FadeInEffect(faintData.m_FloatArray[0], faintData.m_FloatArray[1]);
				}
				else if (faintData.m_IntArray[0] == 0)
					m_FaintEffect.FadeOutEffect(faintData.m_FloatArray[0] + faintData.m_FloatArray[1]);
				else if (faintData.m_IntArray[0] == 2)
				{
					m_FaintEffect.FadeOutEffect(0.3);
					m_FaintEffect.SetRequesterUpdating(false);
				}
			}
		}
		
		#ifdef SERVER
		g_Game.RPCSingleParam(m_Player, ERPCs.RPC_SYMPTOM_PARAM_SYNC, params, true, m_Player.GetIdentity());
		#endif
	}

	override void OnGetActivatedClient(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable())
			Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
	}
	
	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedClient(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable())
			Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());
	}

	override void OnDestructed()
	{
		super.OnDestructed();

		if (!g_Game.IsDedicatedServer())
			m_FaintEffect.Stop();
	}
}
