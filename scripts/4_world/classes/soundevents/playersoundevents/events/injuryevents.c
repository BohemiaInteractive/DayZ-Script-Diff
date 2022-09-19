class InjurySoundEvents extends PlayerSoundEventBase
{
	
	void InjurySoundEvents()
	{
		m_HasPriorityOverTypes = -1;//-1 for all
		m_Type = EPlayerSoundEventType.INJURY;
	}
	
	override bool CanPlay(PlayerBase player)
	{
		return true;
	}
	
	override bool HasPriorityOverCurrent(PlayerBase player, EPlayerSoundEventID other_state_id,EPlayerSoundEventType type_other )
	{
		return true;
	}
}

class InjuryLightSoundEvent extends InjurySoundEvents
{
	void InjuryLightSoundEvent()
	{
		
		m_ID = EPlayerSoundEventID.INJURED_LIGHT;
		m_SoundVoiceAnimEventClassID = 28;
	}
}

class InjuryMediumSoundEvent extends InjurySoundEvents
{
	void InjuryMediumSoundEvent()
	{
		
		m_ID = EPlayerSoundEventID.INJURED_MEDIUM;
		m_SoundVoiceAnimEventClassID = 28;
	}
}

class InjuryHeavySoundEvent extends InjurySoundEvents
{
	void InjuryHeavySoundEvent()
	{
		m_ID = EPlayerSoundEventID.INJURED_HIGH;
		m_SoundVoiceAnimEventClassID = 28;
	}
}

