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
	
	
	
	float m_Time;
	float m_NextEvent;
	
	override void Init()
	{
		m_TrackActivatedTime	= false;
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
		if(player.GetSingleAgentCount(eAgents.SALMONELLA) >= AGENT_THRESHOLD_ACTIVATE) 
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
		//if( player.m_NotifiersManager ) player.m_NotifiersManager.ActivateByType(eNotifiers.NTF_SICK);
		player.IncreaseDiseaseCount();
	}

	override protected void OnDeactivate(PlayerBase player)
	{
		player.DecreaseDiseaseCount();
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		if( player.GetSingleAgentCount( eAgents.SALMONELLA ) <= AGENT_THRESHOLD_DEACTIVATE ) 
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
		float stomach_volume = player.m_PlayerStomach.GetStomachVolume();
		if( stomach_volume >= STOMACH_MIN_VOLUME )
		{
			int roll = Math.RandomInt( 0, 100 );
			int chance_of_vomit = CHANCE_OF_VOMIT + ( CHANCE_OF_VOMIT_AGENT * player.GetSingleAgentCountNormalized( eAgents.SALMONELLA ) );
			if( roll < chance_of_vomit )
			{
				SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom( SymptomIDs.SYMPTOM_VOMIT );
				
				if( symptom )
				{
					symptom.SetDuration( 5 );

					// figure something more clever
					if ( m_Player.GetStatWater().Get() > WATER_DRAIN_FROM_VOMIT )
						m_Player.GetStatWater().Add( -1 * WATER_DRAIN_FROM_VOMIT );
					if ( m_Player.GetStatEnergy().Get() > ENERGY_DRAIN_FROM_VOMIT )
						m_Player.GetStatEnergy().Add( -1 * ENERGY_DRAIN_FROM_VOMIT );
				}
			}
		}
		
		m_Time += deltaT;
	
		if ( m_Time >= m_NextEvent )
		{
			m_Time = 0;
			m_NextEvent = Math.RandomFloatInclusive( EVENT_INTERVAL_MIN, EVENT_INTERVAL_MAX );
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_PAIN_LIGHT);
		}
		
		
	}
};