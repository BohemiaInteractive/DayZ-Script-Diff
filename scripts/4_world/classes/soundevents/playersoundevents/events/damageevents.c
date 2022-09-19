class DamageSoundEvents extends PlayerSoundEventBase
{
	
	void DamageSoundEvents()
	{
		m_HasPriorityOverTypes = -1;//-1 for all
		m_Type = EPlayerSoundEventType.DAMAGE;
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

class DamageLightSoundEvent extends DamageSoundEvents
{
	void DamageLightSoundEvent()
	{
		
		m_ID = EPlayerSoundEventID.TAKING_DMG_LIGHT;
		m_SoundVoiceAnimEventClassID = 12;
	}
}

class DamageHeavySoundEvent extends DamageSoundEvents
{
	void DamageHeavySoundEvent()
	{
		m_ID = EPlayerSoundEventID.TAKING_DMG_HEAVY;
		m_SoundVoiceAnimEventClassID = 13;
	}
}

