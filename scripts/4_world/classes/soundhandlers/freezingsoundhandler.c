/*
class FreezingSoundHandlerBase extends SoundHandlerBase
{
	override void Init()
	{
		m_Id = eSoundHandlers.FREEZING;
	}
	
}

//---------------------------
// Client
//---------------------------
class FreezingSoundHandlerClient extends FreezingSoundHandlerBase
{
	const float SOUND_INTERVALS_LIGHT_MIN = 5;	const float SOUND_INTERVALS_LIGHT_MAX = 20;
	float m_SoundTime;
	EffectSound m_Sound;
	ref HumanMovementState	hms = new HumanMovementState();
	override void Update()
	{
		if( m_Player.GetShakeLevel() > 0 )
		{
			ProcessSound();
		}
	}
	
	void ProcessSound()
	{
		m_Player.GetMovementState(hms);
		if (hms.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_IDLE)
		{
			//return;
		}
		
		if( GetGame().GetTime() > m_SoundTime )
		{
			float offset_time = Math.RandomFloatInclusive(SOUND_INTERVALS_LIGHT_MIN, SOUND_INTERVALS_LIGHT_MAX) * 1000;
			m_SoundTime = GetGame().GetTime() + offset_time;
			PlaySound();
		}
	}
	
	void PlaySound()
	{
		//Print("------------- freezing  --------------");
		m_Player.PlaySoundEvent(EPlayerSoundEventID.FREEZING);
	}
}


//---------------------------
// Server
//---------------------------
class FreezingSoundHandlerServer extends HungerSoundHandlerBase
{
	
}*/