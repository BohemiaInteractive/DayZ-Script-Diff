class DayZCreatureAIType
{
	string m_CfgPath;
	string m_AnimEventsCfgPath;
	
	private ref map<int, ref AnimSoundEvent> 		m_AnimSoundEvents;
	private ref map<int, ref AnimStepEvent> 		m_AnimStepEvents;
	private ref map<int, ref AnimSoundVoiceEvent> 	m_AnimSoundVoiceEvents;
	private ref map<int, ref AnimDamageEvent> 	m_AnimDamageEvents;

	private void DayZCreatureAIType()
	{
		m_CfgPath = "CfgVehicles " + GetName() + " ";
		m_AnimEventsCfgPath = m_CfgPath	+ "AnimEvents ";
		LoadParams();
	}
	
	void LoadAnimSoundEvents()
	{
		m_AnimSoundEvents = new map<int, ref AnimSoundEvent>();
		
		string soundsCfgPath = m_AnimEventsCfgPath + "Sounds ";

		int soundCount = GetGame().ConfigGetChildrenCount(soundsCfgPath);
		for (int i = 0; i < soundCount; i++)
		{
			string soundName;
			GetGame().ConfigGetChildName(soundsCfgPath, i, soundName);			
			string soundPath = soundsCfgPath + soundName + " ";
			AnimSoundEvent soundEvent = new AnimSoundEvent(soundPath);
			if (soundEvent.IsValid())
				m_AnimSoundEvents.Set(soundEvent.m_iID, soundEvent);
		}
	}

	void LoadAnimSoundVoiceEvents()
	{
		m_AnimSoundVoiceEvents = new map<int, ref AnimSoundVoiceEvent>();
		
		string soundsCfgPath = m_AnimEventsCfgPath + "SoundVoice ";

		int soundCount = GetGame().ConfigGetChildrenCount(soundsCfgPath);
		for (int i = 0; i < soundCount; i++)
		{
			string soundName;
			GetGame().ConfigGetChildName(soundsCfgPath, i, soundName);			
			string soundPath = soundsCfgPath + soundName + " ";
			AnimSoundVoiceEvent soundEvent = new AnimSoundVoiceEvent(soundPath);
			if (soundEvent.IsValid())
				m_AnimSoundVoiceEvents.Set(soundEvent.m_iID, soundEvent);
		}
	}
	
	void LoadAnimStepEvents()
	{
		m_AnimStepEvents = new map<int, ref AnimStepEvent>();
		string stepsCfgPath = m_AnimEventsCfgPath + "Steps ";
		int stepsCount = GetGame().ConfigGetChildrenCount(stepsCfgPath);

		for (int i = 0; i < stepsCount; i++)
		{
			string stepName;
			GetGame().ConfigGetChildName(stepsCfgPath, i, stepName);			
			string stepPath = stepsCfgPath + stepName + " ";
			AnimStepEvent stepEvent = new AnimStepEvent(stepPath);
			m_AnimStepEvents.Set(stepEvent.m_iID, stepEvent);
		}
	}
	
	void LoadAnimDamageEvents()
	{
		m_AnimDamageEvents = new map<int, ref AnimDamageEvent>();

		string damagesCfgPath = m_AnimEventsCfgPath + "Damages ";
		int damagesCount = GetGame().ConfigGetChildrenCount(damagesCfgPath);

		for (int i = 0; i < damagesCount; i++)
		{
			string damageName;
			GetGame().ConfigGetChildName(damagesCfgPath, i, damageName);			
			string damagePath = damagesCfgPath + damageName + " ";
			AnimDamageEvent damageEvent = new AnimDamageEvent(damagePath);
			m_AnimDamageEvents.Set(damageEvent.m_iID, damageEvent);
		}
	}

	private void LoadParams()
	{
		LoadAnimSoundEvents();
		LoadAnimSoundVoiceEvents();
		LoadAnimStepEvents();
		LoadAnimDamageEvents();
	}

	AnimStepEvent GetStepEvent(int event_id)
	{
		return m_AnimStepEvents.Get(event_id);
	}

	AnimSoundEvent GetSoundEvent(int event_id)
	{
		return m_AnimSoundEvents.Get(event_id);
	}
	
	AnimSoundVoiceEvent GetSoundVoiceEvent(int event_id)
	{
		return m_AnimSoundVoiceEvents.Get(event_id);
	}

	AnimDamageEvent GetDamageEvent(int event_id)
	{
		return m_AnimDamageEvents.Get(event_id);
	}

	proto native owned string GetName();

	//! DEPRECATED
	ref array<ref AnimSoundEvent> 		m_animSoundEvents;
	ref array<ref AnimStepEvent> 		m_animStepEvents;
	ref array<ref AnimSoundVoiceEvent> 	m_animSoundVoiceEvents;
	ref array<ref AnimDamageEvent> 		m_animDamageEvents;
}