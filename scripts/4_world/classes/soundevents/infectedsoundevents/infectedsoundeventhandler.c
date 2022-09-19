enum EInfectedSoundEventID
{
	MINDSTATE_CALM_IDLE = 1,
	MINDSTATE_CALM_MOVE,
	MINDSTATE_DISTURBED_IDLE,
	//MINDSTATE_CHASE_IDLE,
	MINDSTATE_CHASE_MOVE,
	MINDSTATE_ALERTED_IDLE,
	MINDSTATE_ALERTED_MOVE,
	//--------------
	ENUM_COUNT,
}

class InfectedSoundEventHandler extends SoundEventHandler
{
	const int SOUND_EVENTS_MAX = EInfectedSoundEventID.ENUM_COUNT;
	static ref InfectedSoundEventBase m_AvailableStates[SOUND_EVENTS_MAX];
	ref InfectedSoundEventBase m_CurrentState;
	ZombieBase m_Infected;
	
	void InfectedSoundEventHandler(ZombieBase pInfected)
	{
		m_Infected = pInfected;
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_CALM_IDLE] = new CalmIdleSoundEvent();
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_CALM_MOVE] = new CalmMoveSoundEvent();
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_DISTURBED_IDLE] = new DisturbedIdleSoundEvent();
		//m_AvailableStates[EInfectedSoundEventID.MINDSTATE_CHASE_IDLE] = new ChaseIdleSoundEvent();
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_CHASE_MOVE] = new ChaseMoveSoundEvent();
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_ALERTED_IDLE] = new AlertedIdleSoundEvent();
		m_AvailableStates[EInfectedSoundEventID.MINDSTATE_ALERTED_MOVE] = new AlertedMoveSoundEvent();
	}
	
	override static int GetSoundEventType(int id)
	{
		return m_AvailableStates[id].GetSoundEventType();
	}
	
	override int GetCurrentStateEventID()
	{
		if (m_CurrentState)
		{
			return m_CurrentState.GetSoundEventID();
		}
		return -1;
	}
	
	override int GetCurrentStateEventType()
	{
		if (m_CurrentState)
		{
			return m_CurrentState.GetSoundEventType();
		}
		return -1;
	}
	
	void Stop()
	{
		if ( m_CurrentState )
		{
			m_CurrentState.Stop();
		}
	}
	
	void SoftStop()
	{
		if ( m_CurrentState )
		{
			m_CurrentState.SoftStop();
		}
	}
	
	bool IsPlaying()
	{
		if ( m_CurrentState && m_CurrentState.IsSoundCallbackExist() )
		{
			return true;
		}

		return false;
	}
	
	override bool PlayRequest(int id, bool sent_from_server = false)
	{
		InfectedSoundEventBase requested_state = m_AvailableStates[id];

		if ( !requested_state.CanPlay() )
		{
			return false;
		}
		
		if (m_CurrentState)
		{
		 	if (!m_CurrentState.IsSoundCallbackExist())
			{
				//Print("Missing callback - cleanup and continue");
				delete m_CurrentState;
			}
			
			//! same sound event - skip
			if (GetCurrentStateEventID() == id)
			{
				//Print("Same ID - skipping");
				return false;
			}
		}
		
		if (m_CurrentState)
		{
			m_CurrentState.SoftStop();
			m_CurrentState = InfectedSoundEventBase.Cast(requested_state.ClassName().ToType().Spawn());
			m_CurrentState.Init(m_Infected);
			m_CurrentState.Play();
			return true;
		}
		else
		{
			m_CurrentState = InfectedSoundEventBase.Cast(requested_state.ClassName().ToType().Spawn());
			m_CurrentState.Init(m_Infected);
			m_CurrentState.Play();
			return true;
		}

		return false;//should never get here
	}
}