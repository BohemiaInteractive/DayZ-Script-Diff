class PoisoningMdfr : ModifierBase
{
	static const int AGENT_THRESHOLD_ACTIVATE = 1;
	static const int AGENT_THRESHOLD_DEACTIVATE = 40;
	
	static const int VOMIT_OCCURRENCES_MIN = 2;
	static const int VOMIT_OCCURRENCES_MAX = 5;

	static const int WATER_DRAIN_FROM_VOMIT = 70;
	static const int ENERGY_DRAIN_FROM_VOMIT = 55;
	
	private float m_ExhaustionTimer;
	private bool m_Exhaustion;
	
	override void Init()
	{
		m_TrackActivatedTime	= true;
		m_AnalyticsStatsEnabled = true;
		m_IsPersistent			= true;
		m_ID 					= eModifiers.MDF_POISONING;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.FOOD_POISON) >= AGENT_THRESHOLD_ACTIVATE);
	}
	
	override bool DeactivateCondition(PlayerBase player)
	{
		return !ActivateCondition(player);
	}

	override void OnActivate(PlayerBase player)
	{
		player.IncreaseDiseaseCount();
	}
	
	override void OnReconnect(PlayerBase player)
	{
		this.OnActivate(player);
	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
		player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
		m_Exhaustion = false;
		m_ExhaustionTimer = 0;
		player.DecreaseDiseaseCount();
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		int agentCount = player.m_AgentPool.GetSingleAgentCount(eAgents.FOOD_POISON);
		if (agentCount <= AGENT_THRESHOLD_DEACTIVATE)
			return;

		int agentMax = PluginTransmissionAgents.GetAgentMaxCount(eAgents.FOOD_POISON);

		float invertedAgentCountValue = Math.InverseLerp(0, agentMax, agentCount);
		float chance = Math.Lerp(VOMIT_OCCURRENCES_MIN, VOMIT_OCCURRENCES_MAX, invertedAgentCountValue);
		chance = (chance / 300) * deltaT;
		
		if (Math.RandomFloat01() < chance)
		{
			SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
			if (symptom)
			{
				CachedObjectsParams.PARAM1_FLOAT.param1 = 30.0 * invertedAgentCountValue;
				symptom.SetParam(CachedObjectsParams.PARAM1_FLOAT);
				symptom.SetDuration(Math.Max(3.0, 10 * invertedAgentCountValue));
				//player.m_AgentPool.ReduceAgent(eAgents.FOOD_POISON, 100.0 * invertedAgentCountValue);
				player.m_AgentPool.ReduceAgent(eAgents.FOOD_POISON, 60);
				if (m_Player.GetStatWater().Get() > (WATER_DRAIN_FROM_VOMIT)) 
					m_Player.GetStatWater().Add(-1 * WATER_DRAIN_FROM_VOMIT);
				if (m_Player.GetStatEnergy().Get() > (ENERGY_DRAIN_FROM_VOMIT))
					m_Player.GetStatEnergy().Add(-1 * ENERGY_DRAIN_FROM_VOMIT);
				
				player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
				player.GetStaminaHandler().ActivateDepletionModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
					
				m_Exhaustion = true;
				m_ExhaustionTimer = 0;
			}
		}
		
		if (m_Exhaustion)
		{
			m_ExhaustionTimer += deltaT;
			if (m_ExhaustionTimer >= 30 * invertedAgentCountValue)
			{
				player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
				player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
				
				m_Exhaustion = false;
			}
		}
	}
	
	//! DEPRECATED
	static const int VOMIT_OCCURRENCES_PER_HOUR_MIN = 60;
	static const int VOMIT_OCCURRENCES_PER_HOUR_MAX = 120;
}
