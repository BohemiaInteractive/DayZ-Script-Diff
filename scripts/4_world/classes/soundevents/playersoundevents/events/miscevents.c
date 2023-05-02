class PickupHeavySoundEvent extends PlayerSoundEventBase
{
	void PickupHeavySoundEvent()
	{
		m_HasPriorityOverTypes = -1;//-1 for all
		m_Type = EPlayerSoundEventType.STAMINA;
		m_ID = EPlayerSoundEventID.PICKUP_HEAVY;
		m_SoundVoiceAnimEventClassID = 19;
	}
}

class ThirstSoundEvent extends PlayerSoundEventBase
{
	void ThirstSoundEvent()
	{
		m_HasPriorityOverTypes = EPlayerSoundEventType.DUMMY | EPlayerSoundEventType.STAMINA;//-1 for all
		m_Type = EPlayerSoundEventType.GENERAL;
		m_ID = EPlayerSoundEventID.THIRST;
		m_SoundVoiceAnimEventClassID = 902;
	}
}