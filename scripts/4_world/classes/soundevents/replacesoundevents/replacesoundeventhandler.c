enum ESoundEventType 
{
	SOUND_COMMON = 0	// "Sound"
	SOUND_WEAPON,		// "SoundWeapon"
	SOUND_ATTACHMENT,	// "SoundAttachment"
	SOUND_VOICE			// "SoundVoice"
}

enum EReplaceSoundEventID
{
	DIG_PRIMARY = 1,
	DIG_SECONDARY
}

// Main purpose is to intercept animation system based sound event calls and replace them with different ones based on context 
class ReplaceSoundEventHandler
{	
	protected static ref array< ref map<int, ref ReplaceSoundEventBase> > m_SoundEventReplaceMaps = {};
	
	protected PlayerBase m_Player;
	protected ref Timer m_UpdateTimer;
		
	void ReplaceSoundEventHandler(PlayerBase player)
	{
		m_Player = player;
		
		RegisterEvent(new DigPrimarySoundEvent());
		RegisterEvent(new DigSecondarySoundEvent());
	}
	
	// Inserts sound replace event to array of event maps, creates new event map if first sound of an event type is being registered
	protected void RegisterEvent(ReplaceSoundEventBase soundEvent)
	{
		int sType = soundEvent.GetSoundEventType();
		
		if (!m_SoundEventReplaceMaps.IsValidIndex(sType))
		{
			ref map<int, ref ReplaceSoundEventBase> replaceMap = new map<int, ref ReplaceSoundEventBase>();
			m_SoundEventReplaceMaps.InsertAt(replaceMap, sType);
		}
		
		m_SoundEventReplaceMaps[sType].Insert(soundEvent.GetSoundAnimEventClassID(), soundEvent);
	}
	
	int GetSoundEventID(int anim_id, ESoundEventType soundType)
	{
		if (!m_SoundEventReplaceMaps.IsValidIndex(soundType))
			return 0;
		
		ReplaceSoundEventBase soundEvent = m_SoundEventReplaceMaps[soundType].Get(anim_id);
		if (!soundEvent)
			return 0;
		
		return soundEvent.GetSoundEventID();
	}
	
	ReplaceSoundEventBase GetSoundEventByID(int id, ESoundEventType soundType)
	{
		if (!m_SoundEventReplaceMaps.IsValidIndex(soundType))
			return null;
		
		foreach (int animID, ReplaceSoundEventBase soundEvent : m_SoundEventReplaceMaps[soundType])
		{
			if (soundEvent.GetSoundEventID() == id)
				return soundEvent;
		}
		
		return null;
	}
	
	bool PlayReplaceSound(int soundEventID, ESoundEventType soundType, int flags)
	{
		ReplaceSoundEventBase soundEvent = GetSoundEventByID(soundEventID, soundType);
		if (!soundEvent)
			return false;

		ReplaceSoundEventBase soundEventObj = ReplaceSoundEventBase.Cast(soundEvent.ClassName().ToType().Spawn());
		soundEventObj.Init(m_Player);
		if (soundEventObj.Play())
			return true;
		
		return false;
	}
}