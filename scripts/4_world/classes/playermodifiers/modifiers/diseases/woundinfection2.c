class WoundInfectStage2Mdfr: ModifierBase
{
	static const int AGENT_THRESHOLD_ACTIVATE = 250;
	static const int AGENT_THRESHOLD_DEACTIVATE = 0;

	static const int PAIN_EVENT_INTERVAL_MIN = 6;
	static const int PAIN_EVENT_INTERVAL_MAX = 12;
	
	
	static const float DAMAGE_PER_SEC = 0.04;
	
	protected float m_NextEvent;
	protected float m_Time;
	
	override void Init()
	{
		m_TrackActivatedTime	= false;
		m_ID 					= eModifiers.MDF_WOUND_INFECTION2;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
		m_SyncID				= eModifierSyncIDs.MODIFIER_SYNC_WOUND_INFECT_2;
	}
	
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		if(player.GetSingleAgentCount(eAgents.WOUND_AGENT) >= AGENT_THRESHOLD_ACTIVATE) 
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
		
		SymptomBase shivers = player.GetSymptomManager().QueueUpSecondarySymptomEx(SymptomIDs.SYMPTOM_HAND_SHIVER);
			if ( shivers )
			{
				CachedObjectsParams.PARAM1_INT.param1 = 3;
				shivers.SetParam(CachedObjectsParams.PARAM1_INT);
			}
		
	}

	override protected void OnDeactivate(PlayerBase player)
	{
		player.DecreaseDiseaseCount();
		player.GetSymptomManager().RemoveSecondarySymptom(SymptomIDs.SYMPTOM_HAND_SHIVER);
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		if(player.GetSingleAgentCount(eAgents.WOUND_AGENT) <= AGENT_THRESHOLD_DEACTIVATE) 
		{
			return true;
		}
		else 
		{
			return false;
		}
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		m_Time += deltaT;
	
		if ( m_Time >= m_NextEvent )
		{
			if( player.IsAntibioticsActive() )
			{
				player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_PAIN_LIGHT);
			}
			else
			{
				player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_PAIN_HEAVY);
				float damage = m_Time * (DAMAGE_PER_SEC + player.GetHealthRegenSpeed());
				player.AddHealth("","", -damage);
			}
			
			m_Time = 0;
			m_NextEvent = Math.RandomFloatInclusive( PAIN_EVENT_INTERVAL_MIN, PAIN_EVENT_INTERVAL_MAX );
		}

		
	}
};