class DrowningSoundEventBase  extends PlayerSoundEventBase
{
	void DrowningEvents()
	{
		m_HasPriorityOverTypes = -1;//-1 for all
		m_Type = EPlayerSoundEventType.DROWNING;
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

class DrowningEvent1 extends DrowningSoundEventBase
{
	void DrowningEvent1()
	{
		m_ID = EPlayerSoundEventID.DROWNING_BREATH;
		m_SoundVoiceAnimEventClassID = 901;
	}
}

class DrowningEvent2 extends DrowningSoundEventBase
{
	void DrowningEvent2()
	{
		m_ID = EPlayerSoundEventID.DROWNING_PAIN;
		m_SoundVoiceAnimEventClassID = 900;
	}
}
