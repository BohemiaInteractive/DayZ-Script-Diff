class SoundEventBase
{
	AbstractWave 	m_SoundSetCallback;
	int 			m_Type;
	int 			m_ID;
	int 			m_SoundVoiceAnimEventClassID;
	bool 			m_RequestDestroy;
	string 			m_SoundSetNameRoot;
	bool 			m_SkipForControlledCharacter;
	int				m_Param;
	
	
	void ~SoundEventBase()
	{
		if(m_SoundSetCallback) m_SoundSetCallback.Stop();
	}
	
	//obsolete function, now possible to set a param
	bool IsSkipForControlled()
	{
		return m_SkipForControlledCharacter;
	}
	
	void Tick()
	{
		if(!m_SoundSetCallback)
		{
			m_RequestDestroy = true;
		}
	}
	
	bool IsSoundCallbackExist()
	{
		if( m_SoundSetCallback )
		{
			return true;
		}
		
		return false;
	}
	
	bool IsDestroyRequested()
	{
		return m_RequestDestroy;
	}
	
	bool CanPlay()
	{
		return true;
	}
	
	bool Play()
	{
		return true;
	}
	
	void OnPlay(PlayerBase player);
	
	void Stop()
	{
		if(m_SoundSetCallback) 
		{
			m_SoundSetCallback.Stop();
		}
	}	

	int GetSoundEventID()
	{
		return m_ID;
	}

	int GetSoundEventType()
	{
		return m_Type;
	}
	
	AbstractWave GetSoundSetCallback()
	{
		return m_SoundSetCallback;
	}
}

class SoundEventHandler
{
	static int GetSoundEventType(int id) { return -1; }
	//void OnTick(float delta_time) {}
	bool PlayRequest(EPlayerSoundEventID id, bool sent_from_server = false) { return true; }
	bool PlayRequestEx(EPlayerSoundEventID id, bool sent_from_server = false, int param = 0){ return true; };
	int GetCurrentStateEventID() { return -1; }
	int GetCurrentStateEventType() { return -1; }
}