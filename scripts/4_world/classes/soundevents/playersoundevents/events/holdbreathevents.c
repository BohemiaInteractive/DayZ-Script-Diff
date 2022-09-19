class HoldBreathSoundEventBase extends PlayerSoundEventBase
{
	void HoldBreathSoundEventBase()
	{
		m_HasPriorityOverTypes = -1;
	}
	
	override bool HasHoldBreathException()
	{
		return true;
	}

}


class HoldBreathSoundEvent extends HoldBreathSoundEventBase
{
	void HoldBreathSoundEvent()
	{
		m_Type = EPlayerSoundEventType.GENERAL;
		m_ID = EPlayerSoundEventID.HOLD_BREATH;
		m_SoundVoiceAnimEventClassID = 20;
	}
	
	override bool HasPriorityOverCurrent(PlayerBase player, EPlayerSoundEventID other_state_id, EPlayerSoundEventType type_other)
	{
		if (other_state_id == EPlayerSoundEventID.RELEASE_BREATH)
		{
			return false;
		}
		return true;
	}
}

class ExhaustedBreathSoundEvent extends HoldBreathSoundEventBase
{
	void ExhaustedBreathSoundEvent()
	{
		m_Type = EPlayerSoundEventType.GENERAL;
		m_ID = EPlayerSoundEventID.EXHAUSTED_BREATH;
		m_SoundVoiceAnimEventClassID = 22;
	}
	
	override bool HasPriorityOverCurrent(PlayerBase player, EPlayerSoundEventID other_state_id, EPlayerSoundEventType type_other)
	{
		return false;
	}
}

class ReleaseBreathSoundEvent extends HoldBreathSoundEventBase
{
	void ReleaseBreathSoundEvent()
	{
		m_Type = EPlayerSoundEventType.GENERAL;
		m_ID = EPlayerSoundEventID.RELEASE_BREATH;
		m_SoundVoiceAnimEventClassID = 21;
	}
	
	override bool HasPriorityOverCurrent(PlayerBase player, EPlayerSoundEventID other_state_id, EPlayerSoundEventType type_other)
	{
		if( other_state_id == EPlayerSoundEventID.HOLD_BREATH)
		{
			return false;
		}
		return true;
	}
}