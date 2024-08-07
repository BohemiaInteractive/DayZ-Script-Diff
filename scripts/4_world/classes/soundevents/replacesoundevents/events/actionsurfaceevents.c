class DigPrimarySoundEvent extends DigSoundEventBase
{
	void DigPrimarySoundEvent()
	{
		m_EventType = ESoundEventType.SOUND_COMMON;
		m_ID = EReplaceSoundEventID.DIG_PRIMARY;
		m_SoundAnimEventClassID = 887;
	}
}

class DigSecondarySoundEvent extends DigSoundEventBase
{
	void DigSecondarySoundEvent()
	{
		m_EventType = ESoundEventType.SOUND_COMMON;
		m_ID = EReplaceSoundEventID.DIG_SECONDARY;
		m_SoundAnimEventClassID = 888;
	}
}

class DigSoundEventBase extends ReplaceSoundEventBase
{	
	override protected void SelectSoundID()
	{
		string cfg_path = "cfgVehicles " + m_Player.GetItemInHands().GetType() + " AnimEvents " + "Sound " + "SurfaceDig";
		if (m_ID == EReplaceSoundEventID.DIG_PRIMARY)
			cfg_path = cfg_path + " DigPrimary";
		else if (m_ID == EReplaceSoundEventID.DIG_SECONDARY)
			cfg_path = cfg_path + " DigSecondary";
		
		m_ReplacedSoundAnimID = GetGame().ConfigGetInt(cfg_path + " " + m_Player.GetSurfaceType());
		if (m_ReplacedSoundAnimID == 0)
			m_ReplacedSoundAnimID = GetGame().ConfigGetInt(cfg_path + " default");	
	}
}