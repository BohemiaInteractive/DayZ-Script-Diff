class SoundParameters
{
	float m_FadeIn 	= 0;
	float m_FadeOut = 0;
	bool m_Loop 	= false;
}

/*
Provides functionality to start item related sound from server and synchronize it to clients 
Useful in situations where unique animation can't be used to bind the sound

Steps to add new item sound:
	1) Add new sound ID to SoundConstants class
	2) Override InitItemSounds() method on subject item to link the new sound ID to a soundset 
		2.5) Generally soundset is retreived by adding override to the subject item class, but retreiving from config or using other means could also be used here
	3) Start sound using ItemBase::StartItemSoundServer(int id) called from server side
		3.5) For looped sounds which do not end on their own, use ItemBase::StopItemSoundServer(int id)

Limitations:
	Avoid starting two sounds at the same time using this handler as just single variable is used for synchronization (can be solved by secondary synch var if need arises)

*/
class ItemSoundHandler : Managed
{
	protected ItemBase m_Parent;
	
	protected ref map<int, ref EffectSound> m_PlayingSounds;
	protected ref map<int, string> m_AvailableSoundsets;
	protected ref map<int, ref SoundParameters> m_SoundParamsMap;
	
	void ItemSoundHandler(ItemBase parent)
	{
		m_Parent = parent;
		Init();
	}
	
	void ~ItemSoundHandler()
	{
		if (!m_PlayingSounds)
			return;
		
		foreach (EffectSound sound : m_PlayingSounds)
		{
			if (sound)
				sound.SoundStop();
		}
	}
	
	void Init()
	{
		m_PlayingSounds = new map<int, ref EffectSound>();
		m_AvailableSoundsets = new map<int, string>();
		m_SoundParamsMap = new map<int, ref SoundParameters>();
	}
	
	void PlayItemSoundClient(int id)
	{
		string soundSet = m_AvailableSoundsets.Get(id);
		if (soundSet == string.Empty)
		{
			Debug.Log("Attempting to play soundID " + id + " without defined soundset. Item: " + m_Parent.GetType());
			return;
		}
		
		if (m_PlayingSounds.Get(id))	// already playing
			return;
		
		EffectSound sound;
		SoundParameters params = m_SoundParamsMap.Get(id);
		if (params)
			sound = SEffectManager.PlaySoundCachedParams(soundSet, m_Parent.GetPosition(), params.m_FadeIn, params.m_FadeOut, params.m_Loop);
		else
			sound = SEffectManager.PlaySound(soundSet, m_Parent.GetPosition());
		
		if (sound)
			sound.SetAutodestroy(true);
		else
			Debug.Log("Null when creating sound from set: " + soundSet + " Item: " + m_Parent.GetType() );
		
		m_PlayingSounds.Insert(id, sound);
	}
	
	void StopItemSoundClient(int id)
	{
		if (GetGame().IsDedicatedServer())
			return;
				
		EffectSound sound = m_PlayingSounds.Get(id);
		if (sound)
		{
			sound.SetSoundFadeOut(0.1);
			sound.SoundStop();
		}
		
		m_PlayingSounds.Remove(id);
	}
	
	void AddSound(int sound, string soundset, SoundParameters params = null)
	{
		m_AvailableSoundsets.Insert(sound, soundset);
		
		if (params)
			m_SoundParamsMap.Insert(sound, params);
	}
}