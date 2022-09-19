class SneezeSymptom extends SymptomBase
{
	//this is just for the Symptom parameters set-up and is called even if the Symptom doesn't execute, don't put any gameplay code in here
	override void OnInit()
	{
		m_SymptomType = SymptomTypes.PRIMARY;
		m_Priority = 100;
		m_ID = SymptomIDs.SYMPTOM_SNEEZE;
		m_DestroyOnAnimFinish = true;
		m_SyncToClient = false;
	}
	
	//!gets called every frame
	override void OnUpdateServer(PlayerBase player, float deltatime)
	{

	}

	override void OnUpdateClient(PlayerBase player, float deltatime)
	{
	}
	
	//!gets called once on an Symptom which is being activated
	override void OnGetActivatedServer(PlayerBase player)
	{
		if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
		if( m_Manager.GetCurrentCommandID() == DayZPlayerConstants.COMMANDID_MOVE && !player.IsRaised() && player.GetCommand_Move() && !player.GetCommand_Move().IsOnBack())
		{
			PlayAnimationADD(0);
		}
		else
		{
			PlaySound(EPlayerSoundEventID.SYMPTOM_SNEEZE);
		}
		player.SpreadAgentsEx(3);
	}

	//!gets called once on an Symptom which is being activated
	override void OnGetActivatedClient(PlayerBase player)
	{
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
		
	override SmptAnimMetaBase SpawnAnimMetaObject()
	{
		return new SmptAnimMetaADD();
	}
}
