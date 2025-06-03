class SalmonellaMdfr: ModifierBase
{
	static const int AGENT_THRESHOLD_ACTIVATE = 60;
	static const int AGENT_THRESHOLD_DEACTIVATE = 20;

	static const int CHANCE_OF_VOMIT = 10;			// base vomit chance
	static const int CHANCE_OF_VOMIT_AGENT = 20;	// adjusted by the agent count
	static const int WATER_DRAIN_FROM_VOMIT = 450;
	static const int ENERGY_DRAIN_FROM_VOMIT = 310;
	static const float STOMACH_MIN_VOLUME = 200;	// min volume of stomach for vomit symptom
	
	static const float EVENT_INTERVAL_MIN = 12;
	static const float EVENT_INTERVAL_MAX = 18;

	private float m_Time;
	private float m_NextEvent;
	private float m_ExhaustionTimer;
	
	private bool m_Exhaustion;
	
	override void Init()
	{
		m_TrackActivatedTime	= false;
		m_AnalyticsStatsEnabled = true;
		m_ID 					= eModifiers.MDF_SALMONELLA;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return player.GetSingleAgentCount(eAgents.SALMONELLA) >= AGENT_THRESHOLD_ACTIVATE;
	}
	
	override protected bool DeactivateCondition(PlayerBase player)
	{
		return player.GetSingleAgentCount(eAgents.SALMONELLA) <= AGENT_THRESHOLD_DEACTIVATE;
	}

	override protected void OnActivate(PlayerBase player)
	{
		player.IncreaseDiseaseCount();
		
		SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom( SymptomIDs.SYMPTOM_VOMIT );
		if (symptom)
		{
			CachedObjectsParams.PARAM1_FLOAT.param1 = 50.0;
			symptom.SetParam(CachedObjectsParams.PARAM1_FLOAT);
			symptom.SetDuration(5.0);
		}
	}

	override protected void OnDeactivate(PlayerBase player)
	{
		player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
		player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
		m_Exhaustion = false;
		m_ExhaustionTimer = 0;
		player.DecreaseDiseaseCount();
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		float stomach_volume = player.m_PlayerStomach.GetStomachVolume();
		if (stomach_volume >= STOMACH_MIN_VOLUME)
		{
			int roll = Math.RandomInt(0, 100);
			int chanceOfVomit = CHANCE_OF_VOMIT + (CHANCE_OF_VOMIT_AGENT * player.GetSingleAgentCountNormalized(eAgents.SALMONELLA));
			if (roll < chanceOfVomit)
			{
				SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
				
				if (symptom)
				{
					CachedObjectsParams.PARAM1_FLOAT.param1 = 50.0;
					symptom.SetParam(CachedObjectsParams.PARAM1_FLOAT);
					symptom.SetDuration(5);
					player.m_AgentPool.ReduceAgent(eAgents.SALMONELLA, 30.0);

					if (m_Player.GetStatWater().Get() > WATER_DRAIN_FROM_VOMIT)
						m_Player.GetStatWater().Add(-1 * WATER_DRAIN_FROM_VOMIT);
					if (m_Player.GetStatEnergy().Get() > ENERGY_DRAIN_FROM_VOMIT)
						m_Player.GetStatEnergy().Add(-1 * ENERGY_DRAIN_FROM_VOMIT);
					
					player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
					player.GetStaminaHandler().ActivateDepletionModifier(EStaminaMultiplierTypes.VOMIT_EXHAUSTION);
					
					m_Exhaustion = true;
					m_ExhaustionTimer = 0;
				}
			}
		}
		
		m_Time += deltaT;
	
		if (m_Time >= m_NextEvent)
		{
			m_Time = 0;
			m_NextEvent = Math.RandomFloatInclusive(EVENT_INTERVAL_MIN, EVENT_INTERVAL_MAX);
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_PAIN_LIGHT);
		}
		
		if (m_Exhaustion)
		{
			m_ExhaustionTimer += deltaT;
			if (m_ExhaustionTimer >= 45)
			{
				player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
				player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
				
				m_Exhaustion = false;
			}
		}
		
		
	}
}
