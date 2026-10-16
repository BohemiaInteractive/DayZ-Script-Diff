class IrritatedEyesMdfr : ModifierBase
{
	private int m_EyesIrritationAgentsMax;
	private int m_AgentsCountPrevious;

	private ref SymptomBase m_IrritatedEyesSymptom;

	override void Init()
	{
		m_TrackActivatedTime	= false;
		m_ID 					= eModifiers.MDF_PARTICLES_EYES;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE_LONG;
		m_TickIntervalActive 	= 5;
		m_SyncID				= eModifierSyncIDs.MODIFIER_SYNC_PARTICLE_EYES;
		
		m_EyesIrritationAgentsMax 	= PluginTransmissionAgents.GetAgentMaxCount(eAgents.EYES_IRRITATION);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.EYES_IRRITATION) > 0);
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.EYES_IRRITATION) == 0);
	}

	override void OnActivate(PlayerBase player)
	{
		Param2<int, ref SymptomSyncData> params = new Param2<int, ref SymptomSyncData>(0, null);
		
		m_IrritatedEyesSymptom = player.GetSymptomManager().QueueUpSecondarySymptomEx(SymptomIDs.SYMPTOM_IRRITATED_EYES);
		if (m_IrritatedEyesSymptom)
		{
			SymptomSyncData irritatedEyesData = new SymptomSyncData();
			params.param1 = SymptomIDs.SYMPTOM_IRRITATED_EYES;
			irritatedEyesData.m_FloatArray.Insert(0.0);

			params.param2 = irritatedEyesData;

			m_IrritatedEyesSymptom.SetParam(params);		
		}
	}

	override void OnDeactivate(PlayerBase player)
	{
		if (m_IrritatedEyesSymptom)
		{
			Param2<int, ref SymptomSyncData> params = new Param2<int, ref SymptomSyncData>(0, null);
			SymptomSyncData irritatedEyesData = new SymptomSyncData();
			params.param1 = SymptomIDs.SYMPTOM_IRRITATED_EYES;
			irritatedEyesData.m_FloatArray.Insert(0.0);

			params.param2 = irritatedEyesData;

			m_IrritatedEyesSymptom.SetParam(params);		
		}

		player.GetSymptomManager().RemoveSecondarySymptom(SymptomIDs.SYMPTOM_IRRITATED_EYES);
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		#ifdef DEVELOPER
		if(!player.GetCanBeDestroyed())
			return;
		#endif
		
		float agentsCount = player.GetSingleAgentCount(eAgents.EYES_IRRITATION);

		Param2<int, ref SymptomSyncData> params = new Param2<int, ref SymptomSyncData>(0, null);
		if (m_IrritatedEyesSymptom)
		{
			SymptomSyncData irritatedEyesData = new SymptomSyncData();
			params.param1 = SymptomIDs.SYMPTOM_IRRITATED_EYES;
		
			// TODO@RR move count limit to class wide const
			float AGENTS_LIMIT = 15.0;
			if (agentsCount >= AGENTS_LIMIT && agentsCount != m_AgentsCountPrevious)
			{
				float threshold = Math.Remap(AGENTS_LIMIT, m_EyesIrritationAgentsMax, 0.0, 1.0, m_AgentsCountPrevious);
				
				m_AgentsCountPrevious = agentsCount;
				irritatedEyesData.m_FloatArray.Insert(threshold);
				params.param2 = irritatedEyesData;
				m_IrritatedEyesSymptom.SetParam(params);
			}
			else if (agentsCount < AGENTS_LIMIT)
			{
				irritatedEyesData.m_FloatArray.Insert(0.0);
				params.param2 = irritatedEyesData;
				m_IrritatedEyesSymptom.SetParam(params);
			}
		}
	}
}
