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
		if(m_SoundSetCallback) m_SoundSetCallback.Stop();
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
		#ifdef DEVELOPER
		Error("[ERROR] No sound callback for : " + m_Infected.ClassName() + " Please define relevant SoundSet in Zombie_SoundSets.hpp");
		#endif
		PrintToRPT("[WARNING] No sound callback for : " + m_Infected.ClassName() + " Please define relevant SoundSet in sounds/hpp/config.cpp");
		return false;
	}
}