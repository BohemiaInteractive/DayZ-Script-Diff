class WoundInfectionMdfr: ModifierBase
{
	static const int AGENT_THRESHOLD_ACTIVATE = 100;
	static const int AGENT_THRESHOLD_DEACTIVATE = 20;
	 
	void WoundInfectionMdfr()
	{
		Error("[ERROR] :: WoundInfectionMdfr is deprecated.");
	}
};



class WoundInfectStage1Mdfr: ModifierBase
{
	static const int AGENT_THRESHOLD_ACTIVATE = 1;
	static const int AGENT_THRESHOLD_DEACTIVATE = 250;
	static const int AGENT_THRESHOLD_FEVER = 250;
	
	static const int PAIN_EVENT_INTERVAL_MIN = 18;
	static const int PAIN_EVENT_INTERVAL_MAX = 26;
	
	protected float m_NextEvent;
	protected float m_Time;
	
	override void Init()
	{
		m_TrackActivatedTime	= false;
		m_ID 					= eModifiers.MDF_WOUND_INFECTION1;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
		m_SyncID				= eModifierSyncIDs.MODIFIER_SYNC_WOUND_INFECT_1;
	}
	
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		if( player.GetSingleAgentCount(eAgents.WOUND_AGENT) >= AGENT_THRESHOLD_ACTIVATE && !player.GetModifiersManager().IsModifierActive(eModifiers.MDF_WOUND_INFECTION2)) 
		{
			return true;
		}
		else 
		{
			return false;
		}
	}

	override protected void OnActivate(PlayerBase player)
	{
		player.IncreaseDiseaseCount();
		m_NextEvent = Math.RandomFloatInclusive( PAIN_EVENT_INTERVAL_MIN, PAIN_EVENT_INTERVAL_MAX );
	}

	override protected void OnDeactivate(PlayerBase player)
	{
		player.DecreaseDiseaseCount();
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return !ActivateCondition(player);
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		
		m_Time += deltaT;
		
		if ( m_Time >= m_NextEvent )
		{
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_PAIN_LIGHT);
			m_Time = 0;
			m_NextEvent = Math.RandomFloatInclusive( PAIN_EVENT_INTERVAL_MIN, PAIN_EVENT_INTERVAL_MAX );
		}
		

	}
};