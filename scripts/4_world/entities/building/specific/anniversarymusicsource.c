enum EAnniversaryMusicStates
{
	INVALID,
	LIGHT,
	INTENSE
}

class AnniversaryMusicSource : Building
{
	private const string SOUND_INTENSE 	= "AnniversaryMusic_Intense_SoundSet";
	private const string SOUND_LIGHT 	= "AnniversaryMusic_Light_SoundSet";

	#ifndef SERVER
	#ifndef NO_GUI
	protected EffectSound m_LightAmbientSound;
	protected EffectSound m_IntenseAmbientSound;
	#endif
	#endif
		
	protected int m_MusicState;
	protected int m_MusicStateLocal;
	
	void AnniversaryMusicSource()
	{
		m_MusicState 		= EAnniversaryMusicStates.INVALID;
		m_MusicStateLocal 	= EAnniversaryMusicStates.INVALID;
		
		RegisterNetSyncVariableInt("m_MusicState", 0, EnumTools.GetEnumSize(EAnniversaryMusicStates));
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		
		#ifndef SERVER
		#ifndef NO_GUI
		if (m_LightAmbientSound)
			SEffectManager.DestroyEffect(m_LightAmbientSound);
		
		if (m_IntenseAmbientSound)
			SEffectManager.DestroyEffect(m_IntenseAmbientSound);
		#endif
		#endif
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (m_MusicState != m_MusicStateLocal)
		{
			m_MusicStateLocal = m_MusicState;
			#ifndef SERVER
			#ifndef NO_GUI
			UpdateMusic();
			#endif
			#endif
		}
	}
	
	void UpdateMusic()
	{
		#ifndef SERVER
		#ifndef NO_GUI
		if (!m_IntenseAmbientSound)
			m_IntenseAmbientSound = SEffectManager.CreateSound(SOUND_INTENSE, GetPosition(), 0.5, 0.5, false);
		
		if (!m_LightAmbientSound)
			m_LightAmbientSound = SEffectManager.CreateSound(SOUND_LIGHT, GetPosition(), 0.5, 0.5, true);
		#endif
		#endif

		switch (m_MusicState)
		{
			case EAnniversaryMusicStates.LIGHT:
				PlayLight();
				break;
			
			case EAnniversaryMusicStates.INTENSE:
				PlayIntense();
				break;
			
			default:
				StopAllMusic();
				break;
		}
	}
	
	//just sync
	void SetIntenseServer()
	{
		m_MusicState = EAnniversaryMusicStates.INTENSE;
		SetSynchDirty();
	}
	
	//just sync
	void SetLightServer()
	{
		m_MusicState = EAnniversaryMusicStates.LIGHT;
		SetSynchDirty();
	}
	
	//just sync
	void SetInvalidServer()
	{
		m_MusicState = EAnniversaryMusicStates.INVALID;
		SetSynchDirty();
	}
	
	void PlayIntense()
	{
		#ifndef SERVER
		#ifndef NO_GUI
		m_LightAmbientSound.SoundStop();
		m_IntenseAmbientSound.SoundPlay();
		#endif
		#endif
	}
	
	void PlayLight()
	{
		#ifndef SERVER
		#ifndef NO_GUI
		m_IntenseAmbientSound.SoundStop();
		m_LightAmbientSound.SoundPlay();
		#endif
		#endif
	}
	
	void StopAllMusic()
	{
		#ifndef SERVER
		#ifndef NO_GUI
		m_IntenseAmbientSound.SoundStop();
		m_LightAmbientSound.SoundStop();
		#endif
		#endif
	}
}
