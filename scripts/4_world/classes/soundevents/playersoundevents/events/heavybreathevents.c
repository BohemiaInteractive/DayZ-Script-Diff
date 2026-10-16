class HeavyBreathSoundEventBase : PlayerSoundEventBase
{
	void HeavyBreathSoundEventBase()
	{
		m_HasPriorityOverTypes = -1;//-1 for all

		m_Type = EPlayerSoundEventType.GENERAL;
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

class HeavyBreathEvent1 : HeavyBreathSoundEventBase
{
	void HeavyBreathEvent1()
	{
		m_ID = EPlayerSoundEventID.HEAVY_BREATHING;
		m_SoundVoiceAnimEventClassID = 907;
	}
}
