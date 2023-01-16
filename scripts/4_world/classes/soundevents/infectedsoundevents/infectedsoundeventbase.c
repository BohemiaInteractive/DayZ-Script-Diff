enum EInfectedSoundEventType
{
	GENERAL,
}

class InfectedSoundEventBase extends SoundEventBase
{
	ZombieBase m_Infected;

	void InfectedSoundEventBase()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
	}
	
	void ~InfectedSoundEventBase()
	{
		if (m_SoundSetCallback) m_SoundSetCallback.Stop();
	}

	void Init(ZombieBase pInfected)
	{
		m_Infected = pInfected;
	}
	
	void SoftStop()
	{
		if (m_SoundSetCallback)
		{
			m_SoundSetCallback.Loop(false);
			m_SoundSetCallback = null;
		}
	}

	override void Stop()
	{
		if (m_SoundSetCallback)
		{
			m_SoundSetCallback.Stop();
			m_SoundSetCallback = null;
		}

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).RemoveByName(this, "PosUpdate");
	}
	
	void PosUpdate()
	{
		if (m_SoundSetCallback)
		{
			m_SoundSetCallback.SetPosition(m_Infected.GetPosition());
		}
	}
	
	override bool Play()
	{
		string soundset_name;
			
		soundset_name = string.Format("%1_%2_SoundSet", m_Infected.ClassName(), m_SoundSetNameRoot);
		m_SoundSetCallback = m_Infected.ProcessVoiceFX(soundset_name);
		if ( m_SoundSetCallback )
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLaterByName(this, "PosUpdate", 0, true);
			return true;
		}
		string error = "[%1] No sound callback for : \'%2\' in state: \'%3\' Please define relevant SoundSet in %4";
		#ifdef DEVELOPER
		Error(string.Format(error, "ERROR", m_Infected.ClassName(), m_SoundSetNameRoot, "Zombie_SoundSets.hpp"));
		#else
		PrintToRPT(string.Format(error, "WARNING", m_Infected.ClassName(), m_SoundSetNameRoot, "sounds/hpp/config.cpp"));
		#endif
		return false;
	}
}