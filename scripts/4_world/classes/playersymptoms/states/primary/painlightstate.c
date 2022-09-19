class PainLightSymptom extends SymptomBase
{
	//this is just for the Symptom parameters set-up and is called even if the Symptom doesn't execute, don't put any gameplay code in here
	override void OnInit()
	{
		m_SymptomType = SymptomTypes.PRIMARY;
		m_Priority = 1;
		m_ID = SymptomIDs.SYMPTOM_PAIN_LIGHT;
		m_SyncToClient = true;
		m_MaxCount = 2;
		m_Duration = 1;
	}
	
	//!gets called every frame
	override void OnUpdateServer(PlayerBase player, float deltatime)
	{

	}

	override void OnUpdateClient(PlayerBase player, float deltatime)
	{
	}
	
	override void OnAnimationPlayFailed()
	{
		
	}
	
	override bool CanActivate()
	{
		return true;
	}
	
	//!gets called once on an Symptom which is being activated
	override void OnGetActivatedServer(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable()) 
			Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
		
		PlaySound(EPlayerSoundEventID.TAKING_DMG_LIGHT);
		//player.AddHealth("","Shock",-10);
	}

	//!gets called once on a Symptom which is being activated
	override void OnGetActivatedClient(PlayerBase player)
	{
		//player.SpawnShockEffect(0.5);
		CachedObjectsParams.PARAM2_FLOAT_FLOAT.param1 = 26;
		CachedObjectsParams.PARAM2_FLOAT_FLOAT.param2 = 0.25;
		player.SpawnDamageDealtEffect2(CachedObjectsParams.PARAM2_FLOAT_FLOAT);
		if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
	}

	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedServer(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());
	}

	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedClient(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());
	}
}
