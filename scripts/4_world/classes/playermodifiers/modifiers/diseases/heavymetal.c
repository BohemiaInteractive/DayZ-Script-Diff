class HeavyMetalMdfr : ModifierBase
{	
	static const float WATER_LOSS_HEAVYMETAL_MIN = 0.3;
	static const float WATER_LOSS_HEAVYMETAL_MAX = 0.8;	
	
	static const float HEALTH_LOSS_HEAVYMETAL_MIN = 0.03;
	static const float HEALTH_LOSS_HEAVYMETAL_MAX = 0.05;
	
	static const int SYMPTOMFALLOFF_THRESHOLD1 = 200;
	static const int SYMPTOMFALLOFF_THRESHOLD2 = 300;
	static const int SYMPTOMFALLOFF_THRESHOLD3 = 400;
	
	protected float m_NextEvent;
	protected float m_Time;
	
	protected float m_DeafnessTime;
	protected bool m_IsDeaf;
	
	override void Init()
	{
		m_ID = eModifiers.MDF_HEAVYMETAL1;

		m_TrackActivatedTime	= false;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	override protected void OnActivate(PlayerBase player)
	{
		player.IncreaseDiseaseCount();
	}

	override protected void OnDeactivate(PlayerBase player)
	{
		player.DecreaseDiseaseCount();
	}
}

class HeavyMetalPhase1Mdfr : HeavyMetalMdfr
{
	static const int AGENT_THRESHOLD_ACTIVATE	= 1;
	static const int AGENT_THRESHOLD_DEACTIVATE = 200;
	
	static const int SHIVER_EVENT_INTERVAL_MIN = 20; 
	static const int SHIVER_EVENT_INTERVAL_MAX = 40;

	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.HEAVYMETAL) >= AGENT_THRESHOLD_ACTIVATE && player.GetSingleAgentCount(eAgents.HEAVYMETAL) < AGENT_THRESHOLD_DEACTIVATE);
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.HEAVYMETAL) < AGENT_THRESHOLD_ACTIVATE || player.GetSingleAgentCount(eAgents.HEAVYMETAL) > AGENT_THRESHOLD_DEACTIVATE);
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		if (player.GetSingleAgentCount(eAgents.HEAVYMETAL) <= SYMPTOMFALLOFF_THRESHOLD3) 
		{
			float waterLoss = (deltaT * (WATER_LOSS_HEAVYMETAL_MAX * Math.Max(WATER_LOSS_HEAVYMETAL_MIN, player.GetSingleAgentCountNormalized(eAgents.HEAVYMETAL))));
			player.GetStatWater().Add(-waterLoss);
		}
		
		m_Time += deltaT;
		
		if (player.GetSingleAgentCount(eAgents.HEAVYMETAL) <= SYMPTOMFALLOFF_THRESHOLD2) 
		{
			player.m_AgentPool.AddAgent(eAgents.HEAVYMETAL, -0.5);
		}
		
		if (player.GetSingleAgentCount(eAgents.HEAVYMETAL) <= SYMPTOMFALLOFF_THRESHOLD3) 
		{
			if ( m_Time >= m_NextEvent )
			{
				player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_HAND_SHIVER);
				m_Time = 0;
		
				m_NextEvent = Math.RandomFloatInclusive(SHIVER_EVENT_INTERVAL_MIN, SHIVER_EVENT_INTERVAL_MAX);
			}
		}
	}
}

class HeavyMetalPhase2Mdfr : HeavyMetalMdfr
{
	static const int AGENT_THRESHOLD_ACTIVATE	= 200;
	static const int AGENT_THRESHOLD_DEACTIVATE = 400;
	
	static const int PAIN_EVENT_INTERVAL_MIN = 20; 
	static const int PAIN_EVENT_INTERVAL_MAX = 40;
	
	static const int DEAFNESS_INTERVAL	= 3;
	
	override void Init()
	{
		super.Init();

		m_ID = eModifiers.MDF_HEAVYMETAL2;
	}
		
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.HEAVYMETAL) >= AGENT_THRESHOLD_ACTIVATE && player.GetSingleAgentCount(eAgents.HEAVYMETAL) < AGENT_THRESHOLD_DEACTIVATE);
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.HEAVYMETAL) < AGENT_THRESHOLD_ACTIVATE || player.GetSingleAgentCount(eAgents.HEAVYMETAL) > AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected void OnTick(PlayerBase player, float deltaT)
	{
		
		if (m_IsDeaf)
		{
			m_DeafnessTime += deltaT;
		}
		
		if (m_Time >= m_NextEvent)
		{
			if (player.GetSingleAgentCount(eAgents.HEAVYMETAL) >= SYMPTOMFALLOFF_THRESHOLD3)
			{
				player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_PAIN_LIGHT);
				m_Time = 0;
		
				m_NextEvent = Math.RandomFloatInclusive(PAIN_EVENT_INTERVAL_MIN, PAIN_EVENT_INTERVAL_MAX);
			}
		}
		
		float chanceOfDeafness = player.GetSingleAgentCountNormalized(eAgents.HEAVYMETAL);
		if (Math.RandomFloat01() < chanceOfDeafness / Math.RandomInt(5, 10))
		{
			player.GetSymptomManager().QueueUpSecondarySymptom(SymptomIDs.SYMPTOM_DEAFNESS_COMPLETE);
			m_Time = 0;
			m_DeafnessTime = 0;
			m_IsDeaf = true;
			
			m_NextEvent = Math.RandomFloatInclusive(PAIN_EVENT_INTERVAL_MIN, PAIN_EVENT_INTERVAL_MAX);
		}	
		
		if (m_IsDeaf && m_DeafnessTime > DEAFNESS_INTERVAL + player.GetSingleAgentCountNormalized(eAgents.HEAVYMETAL)*0.6)
		{
			player.GetSymptomManager().RemoveSecondarySymptom(SymptomIDs.SYMPTOM_DEAFNESS_COMPLETE);
			m_IsDeaf = false;
		}
		
		if (player.GetSingleAgentCount(eAgents.HEAVYMETAL) <= SYMPTOMFALLOFF_THRESHOLD3 && eAgents.HEAVYMETAL >= SYMPTOMFALLOFF_THRESHOLD1) 
		{
			float waterLoss = (deltaT * (WATER_LOSS_HEAVYMETAL_MAX * Math.Max(WATER_LOSS_HEAVYMETAL_MIN, player.GetSingleAgentCountNormalized(eAgents.HEAVYMETAL))));
			player.GetStatWater().Add(-waterLoss);
		}
		
		if (player.GetSingleAgentCount(eAgents.HEAVYMETAL) <= SYMPTOMFALLOFF_THRESHOLD3) 
		{
			player.m_AgentPool.AddAgent(eAgents.HEAVYMETAL, -0.5);
		}
	}
}

class HeavyMetalPhase3Mdfr : HeavyMetalMdfr
{
	static const int AGENT_THRESHOLD_ACTIVATE	= 400;
	static const int AGENT_THRESHOLD_DEACTIVATE = 600;
	
	static const int VOMIT_EVENT_INTERVAL_MIN = 180; 
	static const int VOMIT_EVENT_INTERVAL_MAX = 360;

	override void Init()
	{
		super.Init();

		m_ID = eModifiers.MDF_HEAVYMETAL3;
	}
	
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.HEAVYMETAL) >= AGENT_THRESHOLD_ACTIVATE && player.GetSingleAgentCount(eAgents.HEAVYMETAL) < AGENT_THRESHOLD_DEACTIVATE);
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return player.GetSingleAgentCount(eAgents.HEAVYMETAL) < AGENT_THRESHOLD_ACTIVATE;
	}
	
	override protected void OnActivate(PlayerBase player)
	{
		super.OnActivate(player);
		
		player.GetSymptomManager().QueueUpSecondarySymptom(SymptomIDs.SYMPTOM_DEAFNESS_COMPLETE);
		player.GetSymptomManager().QueueUpSecondarySymptom(SymptomIDs.SYMPTOM_HMP_SEVERE);
	}
	
	override protected void OnDeactivate(PlayerBase player)
	{
		super.OnDeactivate(player);
		
		player.GetSymptomManager().RemoveSecondarySymptom(SymptomIDs.SYMPTOM_DEAFNESS_COMPLETE);
		player.GetSymptomManager().RemoveSecondarySymptom(SymptomIDs.SYMPTOM_HMP_SEVERE);
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{	
		float healthLoss = (deltaT * (HEALTH_LOSS_HEAVYMETAL_MAX * Math.Max(HEALTH_LOSS_HEAVYMETAL_MIN, player.GetSingleAgentCountNormalized(eAgents.HEAVYMETAL))));
		player.AddHealth(-healthLoss);
			
		m_Time += deltaT;
		
		if (m_Time >= m_NextEvent)
		{
			SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
			if (symptom)
			{
				CachedObjectsParams.PARAM1_FLOAT.param1 = 30.0; //percentage of stomach
				symptom.SetParam(CachedObjectsParams.PARAM1_FLOAT);			
			}
			
			m_Time = 0;
			m_NextEvent = Math.RandomFloatInclusive( VOMIT_EVENT_INTERVAL_MIN, VOMIT_EVENT_INTERVAL_MAX );
		}
	}
}
