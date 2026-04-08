class CorpseData
{
	const int 	GET_LIFETIME_TRIES_MAX = 3;
	
	bool 		m_bUpdate;
	int 		m_iLastUpdateTime;
	int 		m_iTimeOfDeath;
	int 		m_iMaxLifetime;
	int 		m_iCorpseState;
	int 		m_iTriesToGetLifetime;
	
	float 		m_LifetimeAdjusted = float.MIN;
	float 		m_LastLifetime = float.MIN;
	PlayerBase 	m_Player;
	
	void CorpseData(notnull PlayerBase player,int time_of_death)
	{
		m_bUpdate = true;
		m_iLastUpdateTime = time_of_death;
		m_iTimeOfDeath = time_of_death;
		m_iMaxLifetime = -1;
		m_iCorpseState = PlayerConstants.CORPSE_STATE_FRESH;
		m_Player = player;
		m_iTriesToGetLifetime = 0;
	}
	
	void UpdateCorpseState(bool force_check = false)
	{
		if (m_iMaxLifetime <= 0 )
		{
			if ( m_iCorpseState == PlayerConstants.CORPSE_STATE_DECAYED && !force_check )
			{
				m_bUpdate = false;
				return;
			}
			else
			{
				m_iMaxLifetime = m_Player.GetLifetime();
				if (m_iMaxLifetime <= 0) //cleanup time not initialized yet!
				{
					++m_iTriesToGetLifetime;
					m_iMaxLifetime = -1;
					if (m_iTriesToGetLifetime >= GET_LIFETIME_TRIES_MAX)
					{
						m_bUpdate = false;
					}
					
					return;
				}
				m_iMaxLifetime -= 30 * m_iTriesToGetLifetime; //adjusts for failed init attempts
				m_Player.SetLifetime(m_iMaxLifetime);
			}
		}
		
		if (m_LifetimeAdjusted == float.MIN)
			m_LifetimeAdjusted = m_iMaxLifetime;
		
		if (m_LastLifetime == float.MIN)
			m_LastLifetime = m_iMaxLifetime;
		
		float lifetime = m_Player.GetLifetime();
		
		if (!CanProgressDecay())
		{
			m_LastLifetime = lifetime;
			return;
		}
		
		int corpseStateOld = m_iCorpseState;
		float delta = lifetime - m_LastLifetime;
		
		#ifdef DIAG_DEVELOPER
		if (FeatureTimeAccel.GetFeatureTimeAccelEnabled(ETimeAccelCategories.FOOD_DECAY))
		{
			float timeAccel = 1;
			timeAccel = FeatureTimeAccel.GetFeatureTimeAccelValue();
			delta *= timeAccel;
		}
		#endif
		
		m_LifetimeAdjusted += delta;
		
		float corpseFreshness = 0.0;
		if (m_iMaxLifetime != 0)
			corpseFreshness = m_LifetimeAdjusted / m_iMaxLifetime;
		
		if (corpseFreshness > PlayerConstants.CORPSE_THRESHOLD_MEDIUM)
		{
			m_iCorpseState = PlayerConstants.CORPSE_STATE_FRESH;
		}
		else if (corpseFreshness <= PlayerConstants.CORPSE_THRESHOLD_MEDIUM && corpseFreshness > PlayerConstants.CORPSE_THRESHOLD_DECAYED)
		{
			m_iCorpseState = PlayerConstants.CORPSE_STATE_MEDIUM;
		}
		else
		{
			m_iCorpseState = PlayerConstants.CORPSE_STATE_DECAYED;
			m_bUpdate = false;
		}
		
		if (corpseStateOld != m_iCorpseState)
		{
			m_Player.m_CorpseState = m_iCorpseState;
			m_Player.SetSynchDirty();
		}
		
		m_LastLifetime = lifetime;
	}
	
	protected bool CanProgressDecay()
	{
		return !m_Player.GetIsFrozen();
	}
}
