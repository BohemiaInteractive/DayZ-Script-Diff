class CoughSymptom extends SymptomBase
{
	const int COUGH_BLOOD_LOSS = 50;
	//this is just for the Symptom parameters set-up and is called even if the Symptom doesn't execute, don't put any gameplay code in here
	override void OnInit()
	{
		m_SymptomType = SymptomTypes.PRIMARY;
		m_Priority = 100;
		m_ID = SymptomIDs.SYMPTOM_COUGH;
		m_DestroyOnAnimFinish = true;
		m_SyncToClient = false;
	}
	
	bool IsContaminationActive()
	{
		return m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION2) || m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION3);
	}
	
	
	//!gets called every frame
	override void OnUpdateServer(PlayerBase player, float deltatime)
	{

	}

	override void OnUpdateClient(PlayerBase player, float deltatime)
	{
	}
	
	override void OnAnimationStart()
	{
		if ( IsContaminationActive() )
		{
			PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
			module_lifespan.UpdateBloodyHandsVisibilityEx( m_Player, eBloodyHandsTypes.JUST_BLOOD );
		}
	}
	
	
	override SmptAnimMetaBase SpawnAnimMetaObject()
	{
		return new SmptAnimMetaADD();
	}
	
	//!gets called once on an Symptom which is being activated
	override void OnGetActivatedServer(PlayerBase player)
	{
		if( m_Manager.GetCurrentCommandID() == DayZPlayerConstants.COMMANDID_MOVE && !player.IsRaised() && player.GetCommand_Move() && player.GetCommand_Move() && !player.GetCommand_Move().IsOnBack())
		{
			PlayAnimationADD(1);
		}
		else
		{
			PlaySound(EPlayerSoundEventID.SYMPTOM_COUGH);
		}
		player.SpreadAgentsEx(3);
		
		if ( IsContaminationActive() )
			player.AddHealth("","Blood", -COUGH_BLOOD_LOSS);
	}

	//!gets called once on a Symptom which is being activated
	override void OnGetActivatedClient(PlayerBase player)
	{
		//if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
	}

	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedServer(PlayerBase player)
	{
		//if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());
	}

	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedClient(PlayerBase player)
	{
		//if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());
	}
}
