class ThirstSoundHandlerBase extends SoundHandlerBase
{
	override void Init()
	{
		m_Id = eSoundHandlers.THIRST;
	}
	
}

//---------------------------
// Client
//---------------------------
class ThirstSoundHandlerClient extends ThirstSoundHandlerBase
{
	const float SOUND_INTERVALS_LIGHT_MIN = 10;
	const float SOUND_INTERVALS_LIGHT_MAX = 30;
	float m_SoundTime;
	EffectSound m_Sound;
	
	override void Update()
	{
		if ( m_Player.GetMixedSoundStates() & eMixedSoundStates.THIRSTY )
		{
			ProcessSound();
		}
	}
	
	void ProcessSound()
	{
		if ( GetGame().GetTime() > m_SoundTime)
		{
			float offset_time = Math.RandomFloatInclusive(SOUND_INTERVALS_LIGHT_MIN, SOUND_INTERVALS_LIGHT_MAX) * 1000;
			m_SoundTime = GetGame().GetTime() + offset_time;
			PlaySound();
		}
	}
	
	void PlaySound()
	{
		m_Player.PlaySoundEventEx(EPlayerSoundEventID.THIRST);
	}
}


//---------------------------
// Server
//---------------------------
class ThirstSoundHandlerServer extends ThirstSoundHandlerBase
{
	
}