class FliesMdfr: ModifierBase
{
	const float DISTANCE_SENSITIVITY_SQR = Math.SqrFloat(0.05/*actual distance in meters*/);
	const int TICK_FREQUENCY = 15;
	const int IDLE_COUNT_THRESHOLD = 60;
	vector m_PrevPosition;
	int m_IdleCount;
	
	
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_FLIES;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= TICK_FREQUENCY;
		DisableActivateCheck();
		DisableDeactivateCheck();
	}	

	override bool ActivateCondition(PlayerBase player)
	{
		return false;
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{	
		float dist_sqr = vector.DistanceSq(player.GetPosition(), m_PrevPosition);
		if( dist_sqr < DISTANCE_SENSITIVITY_SQR)//has the player stayed still since last check
		{
			m_IdleCount++;
		}
		else
		{
			if(m_IdleCount >= IDLE_COUNT_THRESHOLD)//disable the effect
			{
				player.m_CorpseState = -PlayerConstants.CORPSE_STATE_DECAYED;
				player.SetSynchDirty();
			}
			
			m_IdleCount = 0;//player moved, reset the count
		}
		m_PrevPosition = player.GetPosition();
		
		if( m_IdleCount == IDLE_COUNT_THRESHOLD)//should we play the effect ?
		{
			player.m_CorpseState = PlayerConstants.CORPSE_STATE_DECAYED;
			player.SetSynchDirty();
		}
		
		
	}
	
	override void OnReconnect(PlayerBase player)
	{
		OnActivate(player);
	}
	
	override void OnActivate(PlayerBase player)
	{
		m_IdleCount = 0;
		player.m_CorpseState = -PlayerConstants.CORPSE_STATE_DECAYED;
		player.SetSynchDirty();
	}	
	
	override void OnDeactivate(PlayerBase player)
	{

	}
	
	
	
};