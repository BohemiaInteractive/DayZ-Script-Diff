class MindStateSoundEventBase extends InfectedSoundEventBase
{
	override bool CanPlay()
	{
		if( !super.CanPlay() )
		{
			return false;
		}
		return true;
	}
}

class CalmIdleSoundEvent extends MindStateSoundEventBase
{
	void CalmIdleSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_CALM_IDLE;
		m_SoundSetNameRoot = "CalmIdle";
	}
}

class CalmMoveSoundEvent extends MindStateSoundEventBase
{
	void CalmMoveSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_CALM_MOVE;
		m_SoundSetNameRoot = "CalmMove";
	}
}

class DisturbedIdleSoundEvent extends MindStateSoundEventBase
{
	void DisturbedIdleSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_DISTURBED_IDLE;
		m_SoundSetNameRoot = "DisturbedIdle";
	}
}

class ChaseMoveSoundEvent extends MindStateSoundEventBase
{
	void ChaseMoveSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_CHASE_MOVE;
		m_SoundSetNameRoot = "ChaseMove";
	}
}

class AlertedIdleSoundEvent extends MindStateSoundEventBase
{
	void AlertedIdleSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_ALERTED_IDLE;
		m_SoundSetNameRoot = "AlertedIdle";
	}
}

class AlertedMoveSoundEvent extends MindStateSoundEventBase
{
	void AlertedMoveSoundEvent()
	{
		m_Type = EInfectedSoundEventType.GENERAL;
		m_ID = EInfectedSoundEventID.MINDSTATE_ALERTED_MOVE;
		m_SoundSetNameRoot = "AlertedMove";
	}
}