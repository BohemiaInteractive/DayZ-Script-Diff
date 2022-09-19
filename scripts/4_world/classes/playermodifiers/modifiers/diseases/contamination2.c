class ContaminationStage2Mdfr: ModifierBase
{
	static const int AGENT_THRESHOLD_ACTIVATE = 100;
	static const int AGENT_THRESHOLD_DEACTIVATE = 400;
	
	static const int EVENT_1_INTERVAL_MIN = 20;
	static const int EVENT_1_INTERVAL_MAX = 40;
	
	protected float m_NextEvent1;
	protected float m_Time1;
	
	
	static const int EVENT_2_INTERVAL_MIN = 200;
	static const int EVENT_2_INTERVAL_MAX = 400;
	
	protected float m_NextEvent2;
	protected float m_Time2;
	
	override void Init()
	{
		m_ID 					= eModifiers.MDF_CONTAMINATION2;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
		m_SyncID				= eModifierSyncIDs.MODIFIER_SYNC_CONTAMINATION2;
	}
	
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return ( player.GetSingleAgentCount(eAgents.CHEMICAL_POISON) >= AGENT_THRESHOLD_ACTIVATE && player.GetSingleAgentCount(eAgents.CHEMICAL_POISON) < AGENT_THRESHOLD_DEACTIVATE);
	}

	override protected void OnActivate(PlayerBase player)
	{
		player.IncreaseDiseaseCount();
		m_NextEvent1 = Math.RandomFloatInclusive( EVENT_1_INTERVAL_MIN, EVENT_1_INTERVAL_MAX );
		m_NextEvent2 = Math.RandomFloatInclusive( EVENT_2_INTERVAL_MIN, EVENT_2_INTERVAL_MAX );
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
		
		m_Time1 += deltaT;
		m_Time2 += deltaT;
		
	
		if ( m_Time1 >= m_NextEvent1 )
		{
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
			m_Time1 = 0;
			m_NextEvent1 = Math.RandomFloatInclusive( EVENT_1_INTERVAL_MIN, EVENT_1_INTERVAL_MAX );
		}
		if ( m_Time2 >= m_NextEvent2 )
		{
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
			m_Time2 = 0;
			m_NextEvent2 = Math.RandomFloatInclusive( EVENT_2_INTERVAL_MIN, EVENT_2_INTERVAL_MAX );
		}

	}
};