class AreaExposureMdfr: ModifierBase
{
	const int EVENT_1_INTERVAL_MIN = 3;
	const int EVENT_1_INTERVAL_MAX = 5;
	
	const float AGENTS_PER_SEC = 5;
	protected float m_NextEvent1;
	protected float m_Time1;
	
	const int EVENT_2_INTERVAL_MIN = 13;
	const int EVENT_2_INTERVAL_MAX = 18;
	
	
	const float AGENT_DOSE_PER_BS_SEC = 0.33;//how many agents will be injected in one sec per a single bleeding source
	
	protected float m_NextEvent2;
	protected float m_Time2;
	
	
	
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_AREAEXPOSURE;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE_LONG;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE_SHORT;
		m_SyncID				= eModifierSyncIDs.MODIFIER_SYNC_ZONE_EXPOSURE;
		DisableActivateCheck();
		DisableDeactivateCheck();
		
	}

	override bool ActivateCondition(PlayerBase player)
	{
		return false;
	}
	
	override void OnActivate(PlayerBase player)
	{
	
		JsonDataContaminatedAreas data = EffectAreaLoader.GetData();
		if (data)
		{
			MiscGameplayFunctions.TeleportCheck(player, data.SafePositions);
		}
		
		//make the player cough immediately
		float transmitted = TransmitAgents(player, 1);
		if(transmitted) 
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
		
		m_NextEvent1 = Math.RandomFloatInclusive( EVENT_1_INTERVAL_MIN, EVENT_1_INTERVAL_MAX );
	}
	
	override void OnDeactivate(PlayerBase player)
	{
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		#ifdef DEVELOPER
		if(!player.GetCanBeDestroyed())
			return;
		#endif
		
		float transmitted = TransmitAgents(player, AGENTS_PER_SEC * deltaT);
		
		m_Time2 += deltaT;
		
		if (transmitted)
		{
			m_Time1 += deltaT;
			if (m_Time1 >= m_NextEvent1 )
			{
				player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
				
				if(Math.RandomFloat01() < 0.25)//creates a cough cooldown once in a while
				{
					m_NextEvent1 = Math.RandomFloatInclusive( EVENT_1_INTERVAL_MIN * 2, EVENT_1_INTERVAL_MAX * 2 );
				}
				else
				{
					m_NextEvent1 = Math.RandomFloatInclusive( EVENT_1_INTERVAL_MIN, EVENT_1_INTERVAL_MAX );
				}
			
				m_Time1 = 0;
			}
		}
		
		if ( m_Time2 >= m_NextEvent2 )
		{
			BleedingSourceCreateCheck(player);
			m_Time2 = 0;
			m_NextEvent2 = Math.RandomFloatInclusive( EVENT_2_INTERVAL_MIN, EVENT_2_INTERVAL_MAX );
		}
		
		ApplyAgentsToBleedingSources(player, deltaT);
		
	}

	void ApplyAgentsToBleedingSources(PlayerBase player, float deltaT)
	{
		
		int count = player.GetBleedingSourceCount();
		float agent_dose = count * AGENT_DOSE_PER_BS_SEC * deltaT;
		player.InsertAgent(eAgents.CHEMICAL_POISON, agent_dose);
		
	}
	
	void BleedingSourceCreateCheck(PlayerBase player)
	{
		int free_bs_locations = 0;//bitmask where each bit set to 1 represents available bleeding source location
		set<int> list = player.GetBleedingManagerServer().GetBleedingSourcesLocations();
		
		foreach(int location: list)
		{
			float prot_level = PluginTransmissionAgents.GetProtectionLevelEx(DEF_CHEMICAL, location, player, true);
			float dice_throw = Math.RandomFloat01();
			if(dice_throw > prot_level)
			{
				free_bs_locations = player.GetBleedingManagerServer().GetFreeBleedingSourceBitsByInvLocation(location) | free_bs_locations;
			}
		}
		
		int num_of_free_bs = Math.GetNumberOfSetBits(free_bs_locations);//gets us the number of bits set to 1, where each represents a free bleeding source location
		
		if ( num_of_free_bs > 0 )
		{
			int random_bs_index = Math.RandomIntInclusive(0, num_of_free_bs - 1 );// - 1 on the max to convert count to index
			int random_bs_bit = Math.Pow(2, Math.GetNthBitSet(free_bs_locations,random_bs_index));
			player.GetBleedingManagerServer().AttemptAddBleedingSourceDirectly(random_bs_bit, eBleedingSourceType.CONTAMINATED);
		}
	}

	float TransmitAgents(PlayerBase player, float count)
	{
		PluginTransmissionAgents plugin = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		return plugin.TransmitAgentsEx(null, player, AGT_AIRBOURNE_CHEMICAL, count, eAgents.CHEMICAL_POISON);
	}
	
	
};