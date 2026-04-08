
//Digging
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
		
		m_ReplacedSoundAnimID = g_Game.ConfigGetInt(cfg_path + " " + m_Player.GetSurfaceType());
		if (m_ReplacedSoundAnimID == 0)
			m_ReplacedSoundAnimID = g_Game.ConfigGetInt(cfg_path + " default");	
	}
};

//Cleaning hands 
class CleanHandsPrimarySoundEvent extends CleanHandsSoundEventBase
{
	void CleanHandsPrimarySoundEvent()
	{
		m_EventType = ESoundEventType.SOUND_COMMON;
		m_ID = EReplaceSoundEventID.CLEANHANDS_PRIMARY;
		m_SoundAnimEventClassID = 10007;
		m_UserString = "StopOnAnimEnd";
	}
}

class CleanHandsSecondarySoundEvent extends CleanHandsSoundEventBase
{
	void CleanHandsSecondarySoundEvent()
	{
		m_EventType = ESoundEventType.SOUND_COMMON;
		m_ID = EReplaceSoundEventID.CLEANHANDS_SECONDARY;
		m_SoundAnimEventClassID = 10006;
		m_UserString = "StopOnAnimEnd";
	}
}

class CleanHandsSoundEventBase extends ReplaceSoundEventBase
{	
	override protected void SelectSoundID()
	{
		string cfg_path = "cfgVehicles " + m_Player.GetType() + " AnimEvents " + "Sound " + "SurfaceCleanHands";
		if (m_ID == EReplaceSoundEventID.CLEANHANDS_PRIMARY)
			cfg_path = cfg_path + " CleanHandsPrimary";
		else if (m_ID == EReplaceSoundEventID.CLEANHANDS_SECONDARY)
			cfg_path = cfg_path + " CleanHandsSecondary";
		
		m_ReplacedSoundAnimID = g_Game.ConfigGetInt(cfg_path + " " + m_Player.GetSurfaceType());
		if (m_ReplacedSoundAnimID == 0)
			m_ReplacedSoundAnimID = g_Game.ConfigGetInt(cfg_path + " default");	
	}
};