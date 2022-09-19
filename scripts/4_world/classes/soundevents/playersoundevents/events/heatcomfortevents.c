class HeatComfortEventsBase extends PlayerSoundEventBase
{
	
	void HeatComfortEventsBase()
	{
		m_HasPriorityOverTypes = EPlayerSoundEventType.DUMMY | EPlayerSoundEventType.INJURY | EPlayerSoundEventType.STAMINA | EPlayerSoundEventType.GENERAL;//-1 for all
		m_Type = EPlayerSoundEventType.GENERAL;
	}
	
	override bool HasPriorityOverCurrent(PlayerBase player, EPlayerSoundEventID other_state_id,EPlayerSoundEventType type_other )
	{
		return true;
	}
}

class FreezingSoundEvent extends HeatComfortEventsBase
{
	void FreezingSoundEvent()
	{
		m_ID = EPlayerSoundEventID.FREEZING;
		m_SoundVoiceAnimEventClassID = 11;
	}
}

class HotSoundEvent extends HeatComfortEventsBase
{
	void HotSoundEvent()
	{
		m_ID = EPlayerSoundEventID.HOT;
		m_SoundVoiceAnimEventClassID = 1111111;
	}
}