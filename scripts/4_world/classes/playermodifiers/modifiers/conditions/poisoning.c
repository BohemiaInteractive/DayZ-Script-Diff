class PoisoningMdfr: ModifierBase
{
	static const int AGENT_THRESHOLD_ACTIVATE = 150;
	static const int AGENT_THRESHOLD_DEACTIVATE = 0;
	
	static const int VOMIT_OCCURRENCES_PER_HOUR_MIN = 60;
	static const int VOMIT_OCCURRENCES_PER_HOUR_MAX = 120;

	static const int WATER_DRAIN_FROM_VOMIT = 70;
	static const int ENERGY_DRAIN_FROM_VOMIT = 55;
	
	override void Init()
	{
		m_TrackActivatedTime	= true;
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
		if(player.GetSingleAgentCount(eAgents.FOOD_POISON) >= AGENT_THRESHOLD_ACTIVATE) 
		{
			return true;
		}
		else 
		{
			return false;
		}
	}
	
	override bool DeactivateCondition(PlayerBase player)
	{
		if(player.GetSingleAgentCount(eAgents.FOOD_POISON) <= AGENT_THRESHOLD_DEACTIVATE) 
		{
			return true;
		}
		else 
		{
			return false;
		}
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
		player.DecreaseDiseaseCount();
	}

	override void OnTick(PlayerBase player, float deltaT)
	{	
		int agent_max = PluginTransmissionAgents.GetAgentMaxCount(eAgents.FOOD_POISON);
		int agent_count = player.m_AgentPool.GetSingleAgentCount(eAgents.FOOD_POISON);
		float stomach_volume = player.m_PlayerStomach.GetStomachVolume();
		
		float norm_value = Math.InverseLerp(0, agent_max, agent_count );
		float eased_value = Easing.EaseInCirc(norm_value);
		float chance = Math.Lerp(VOMIT_OCCURRENCES_PER_HOUR_MIN, VOMIT_OCCURRENCES_PER_HOUR_MAX, eased_value );
		chance = (chance / 3600) * deltaT;
		
		if( Math.RandomFloat01() < chance )
		{
			SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
			
			if( symptom )
			{
				symptom.SetDuration(5);

				if (m_Player.GetStatWater().Get() > (WATER_DRAIN_FROM_VOMIT))
					m_Player.GetStatWater().Add(-1 * WATER_DRAIN_FROM_VOMIT);
				if (m_Player.GetStatEnergy().Get() > (ENERGY_DRAIN_FROM_VOMIT))
					m_Player.GetStatEnergy().Add(-1 * ENERGY_DRAIN_FROM_VOMIT);
			}
		}
	}
};