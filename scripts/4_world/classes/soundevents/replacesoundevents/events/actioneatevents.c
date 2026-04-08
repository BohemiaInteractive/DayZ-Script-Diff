class EatingVoiceOverrideDefault : EatingVoiceOverrideBase
{
	void EatingVoiceOverrideDefault()
	{
		m_ID = EReplaceSoundEventID.EAT_DEFAULT;
		m_SoundAnimEventClassID = 888;
	}
}

class EatingVoiceOverrideFruit : EatingVoiceOverrideBase
{
	void EatingVoiceOverrideFruit()
	{
		m_ID = EReplaceSoundEventID.EAT_FRUIT;
		m_SoundAnimEventClassID = 889;
	}
}

class EatingVoiceOverrideBase : ReplaceSoundEventBase
{
	void EatingVoiceOverrideBase()
	{
		m_EventType = ESoundEventType.SOUND_VOICE;
		m_UserString = "StopOnAnimEnd";
	}
	
	override protected void SelectSoundID()
	{
		string cfgPath = "cfgVehicles " + m_Player.GetItemInHands().GetType() + " AnimEvents Sound VoiceEatOverride eating_loop_default";
		if (GetGame().ConfigIsExisting(cfgPath))
			m_ReplacedSoundAnimID = GetGame().ConfigGetInt(cfgPath);
	}
}