class VomitSymptom : SymptomBase
{
	static const float STAMINA_DEPLETION_MULTIPLIER	= 1.3;
	static const float STAMINA_RECOVERY_MULTIPLIER 	= 0.5;

	//just for the Symptom parameters set-up and gets called even if the Symptom doesn't execute, don't put any gameplay code in here
	const int BLOOD_LOSS = 250;
	const int STOMACH_CONTENT_PERCENTAGE_DEFAULT = 5; //per second
	
	private float m_VomitContentPercentage = -1; //considered unspecified at -1
	
	override void OnInit()
	{
		m_SymptomType = SymptomTypes.PRIMARY;
		m_Priority = 100;
		m_ID = SymptomIDs.SYMPTOM_VOMIT;
		m_DestroyOnAnimFinish = true;
		m_SyncToClient = false;
		m_Duration = 5;
		m_MaxCount = 1;
		m_VomitContentPercentage = -1;
	}
	
	bool IsContaminationActive()
	{
		return m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION2) || m_Player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CONTAMINATION3);
	}
	
	override void SetParam(Param p)
	{
		Param1<float> p1 = Param1<float>.Cast(p);
		if ( p1 )
		{
			m_VomitContentPercentage = p1.param1;
		}
	}
	
	override void OnAnimationStart()
	{
		if (m_Player)
		{
			m_Player.GetStatToxicity().Set(0);
			if (m_Player.m_PlayerStomach)
			{
				if (m_VomitContentPercentage < 0) //unspecified, calculate from duration
				{
					float contentLoss = STOMACH_CONTENT_PERCENTAGE_DEFAULT * m_Duration;
					m_Player.m_PlayerStomach.ReduceContents(contentLoss);
				}
				else
					m_Player.m_PlayerStomach.ReduceContents(m_VomitContentPercentage);
			}
			
			if (IsContaminationActive())
				m_Player.AddHealth("","Blood", -BLOOD_LOSS);
		}
	}
	
	override void OnAnimationFinish()
	{
		//! deplete stamina
		m_Player.GetStaminaHandler().DepleteStaminaEx(EStaminaModifiers.OVERALL_DRAIN);
		
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
	}

	override SmptAnimMetaBase SpawnAnimMetaObject()
	{
		return new SmptAnimMetaFB();
	}
	
	override bool IsSyncToRemotes()
	{
		return true;
	}
}
