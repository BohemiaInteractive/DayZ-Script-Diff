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