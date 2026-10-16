class IrritatedEyes : SymptomBase
{
	protected float m_IrritationAmount;

	//this is just for the Symptom parameters set-up and is called even if the Symptom doesn't execute, don't put any gameplay code in here
	override void OnInit()
	{
		m_Priority 				= 0;
		m_SymptomType 			= SymptomTypes.SECONDARY;
		m_ID 					= SymptomIDs.SYMPTOM_IRRITATED_EYES;
		m_DestroyOnAnimFinish 	= true;
		m_IsPersistent 			= false;
		m_ServerUpdateInterval	= 5.0;
		m_SyncToClient			= true;
	}
	
	override void SetParam(Param p)
	{
		Param2<int, ref SymptomSyncData> params = Param2<int, ref SymptomSyncData>.Cast(p);
		if (params && params.param1 == SymptomIDs.SYMPTOM_IRRITATED_EYES)
		{
			SymptomSyncData irritatedEyesData = params.param2;
			CachedObjectsParams.PARAM1_FLOAT.param1 = irritatedEyesData.m_FloatArray[0];
		
			#ifdef SERVER
			g_Game.RPCSingleParam(m_Player, ERPCs.RPC_SYMPTOM_PARAM_SYNC, params, true, m_Player.GetIdentity());
			#endif
		}
	}

	override void OnUpdateClient(PlayerBase player, float deltatime)
	{
		Param1<float> params = new Param1<float>(0.0);
		params.param1 = CachedObjectsParams.PARAM1_FLOAT.param1;
		g_Game.GetMission().GetEffectWidgets().UpdateWidgets(EffectWidgetsTypes.EYE_VEINS, deltatime, params, EffectWidgetHandles.EYE_VEINS);
	}

	//!gets called once on an Symptom which is being activated
	override void OnGetActivatedClient(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable())
			Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
		
		g_Game.GetMission().GetEffectWidgets().AddActiveEffects({EffectWidgetsTypes.EYE_VEINS});
	}
	
	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedClient(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable())
			Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());

		Param1<float> params = new Param1<float>(0.0);
		g_Game.GetMission().GetEffectWidgets().UpdateWidgets(EffectWidgetsTypes.EYE_VEINS, 0.0, params, EffectWidgetHandles.EYE_VEINS);
		g_Game.GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.EYE_VEINS});
	}	
}
