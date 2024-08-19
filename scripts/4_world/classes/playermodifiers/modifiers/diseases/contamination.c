class ContaminationStage1Mdfr: ModifierBase
{
	static const int AGENT_THRESHOLD_ACTIVATE = 1;
	static const int AGENT_THRESHOLD_DEACTIVATE = 100;
	
	static const int EVENT_1_INTERVAL_MIN = 20;
	static const int EVENT_1_INTERVAL_MAX = 40;
	
	protected float m_NextEvent;
	protected float m_Time;
	
	override void Init()
	{
		m_ID 					= eModifiers.MDF_CONTAMINATION1;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
		m_SyncID				= eModifierSyncIDs.MODIFIER_SYNC_CONTAMINATION;
	}
	
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return ( player.GetSingleAgentCount(eAgents.CHEMICAL_POISON) >= AGENT_THRESHOLD_ACTIVATE && player.GetSingleAgentCount(eAgents.CHEMICAL_POISON) < AGENT_THRESHOLD_DEACTIVATE );
	}

	override protected void OnActivate(PlayerBase player)
	{
		//player.IncreaseDiseaseCount();
		m_NextEvent = Math.RandomFloatInclusive( EVENT_1_INTERVAL_MIN, EVENT_1_INTERVAL_MAX );
	}

	override protected void OnDeactivate(PlayerBase player)
	{
		//player.DecreaseDiseaseCount();
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return !ActivateCondition(player);
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		/*
		m_Time += deltaT;
		
		if ( m_Time >= m_NextEvent )
		{
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
			m_Time = 0;
			m_NextEvent = Math.RandomFloatInclusive( EVENT_1_INTERVAL_MIN, EVENT_1_INTERVAL_MAX );
		}
		*/
	}
};