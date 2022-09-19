class VomitSymptom extends SymptomBase
{
	//just for the Symptom parameters set-up and gets called even if the Symptom doesn't execute, don't put any gameplay code in here
	const int BLOOD_LOSS = 250;
	override void OnInit()
	{
		m_SymptomType = SymptomTypes.PRIMARY;
		m_Priority = 100;
		m_ID = SymptomIDs.SYMPTOM_VOMIT;
		m_DestroyOnAnimFinish = true;
		m_SyncToClient = false;
		m_Duration = 5;
		m_MaxCount = 1;
	}
	
	//!gets called every frame
	override void OnUpdateServer(PlayerBase player, float deltatime)
	{

	}

	override void OnUpdateClient(PlayerBase player, float deltatime)
	{

	}
	
	bool IsContaminationActive()
	{
		return m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION2) || m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION3);
	}
	
	override void OnAnimationStart()
	{
		if(m_Player)
		{
			m_Player.GetStatToxicity().Set(0);
			if(m_Player.m_PlayerStomach)
				m_Player.m_PlayerStomach.ClearContents();
			
			if (IsContaminationActive())
			{
				m_Player.AddHealth("","Blood", -BLOOD_LOSS);
			}
			
		}
		//Print("------------ vomit start -------------");
	}
	
	override void OnAnimationFinish()
	{
		//! deplete stamina
		m_Player.GetStaminaHandler().DepleteStamina(EStaminaModifiers.OVERALL_DRAIN);
		
		if (IsContaminationActive())
		{
			PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
			module_lifespan.UpdateBloodyHandsVisibilityEx( m_Player, eBloodyHandsTypes.JUST_BLOOD );
		}
	}
	
	override bool CanActivate()
	{
		return ( m_Manager.GetCurrentCommandID() == DayZPlayerConstants.COMMANDID_MOVE || m_Manager.GetCurrentCommandID() == DayZPlayerConstants.COMMANDID_ACTION );
	}	
	
	//!gets called once on an Symptom which is being activated
	override void OnGetActivatedServer(PlayerBase player)
	{
		PlayAnimationFB(DayZPlayerConstants.CMD_ACTIONFB_VOMIT,DayZPlayerConstants.STANCEMASK_CROUCH, GetDuration() );
		//timer.Run(10, this, "Destroy");
		//if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetActivated", m_Player.ToString());
	}

	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedServer(PlayerBase player)
	{
		//if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());
	}

	override void OnGetDeactivatedClient(PlayerBase player)
	{
		//if (LogManager.IsSymptomLogEnable()) Debug.SymptomLog("n/a", this.ToString(), "n/a", "OnGetDeactivated", m_Player.ToString());	
	}
	
	override SmptAnimMetaBase SpawnAnimMetaObject()
	{
		return new SmptAnimMetaFB();
	}
}