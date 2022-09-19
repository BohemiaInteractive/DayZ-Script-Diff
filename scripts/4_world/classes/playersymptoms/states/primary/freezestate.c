class FreezeSymptom extends SymptomBase
{
	//this is just for the Symptom parameters set-up and is called even if the Symptom doesn't execute, don't put any gameplay code in here
	override void OnInit()
	{
		m_SymptomType = SymptomTypes.PRIMARY;
		m_Priority = 1;
		m_ID = SymptomIDs.SYMPTOM_FREEZE;
		m_DestroyOnAnimFinish = true;
		m_SyncToClient = false;
		m_MaxCount = 2;
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
		if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
		HumanMovementState hms = new HumanMovementState();
		player.GetMovementState(hms);
		ItemBase item = m_Player.GetItemInHands();

		if(!(item && item.IsHeavyBehaviour()) && m_Manager.GetCurrentCommandID() == DayZPlayerConstants.COMMANDID_MOVE && hms.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_IDLE && !player.IsRestrained() )
		{
			PlayAnimationADD(2);
		}
		else
		{
			PlaySound(EPlayerSoundEventID.FREEZING);
		}
	}

	//!gets called once on a Symptom which is being activated
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
		return new HeatComfortmMetaADD();
	}
}
