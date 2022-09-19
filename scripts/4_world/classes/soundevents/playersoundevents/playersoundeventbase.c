enum EPlayerSoundEventType
{
	GENERAL 	= 0x00000001,
	MELEE 		= 0x00000002,
	STAMINA 	= 0x00000004,
	DAMAGE 		= 0x00000008,
	DUMMY 		= 0x00000010,
	INJURY 		= 0x00000020,
	DROWNING 	= 0x00000040,
	//HEAT_COMFORT	= 0x00000080,
}

enum EPlayerSoundEventParam
{
	SKIP_CONTROLLED_PLAYER 	= 0x00000001,
	HIGHEST_PRIORITY		= 0x00000002,
	/*
	STAMINA 	= 0x00000004,
	DAMAGE 		= 0x00000008,
	DUMMY 		= 0x00000010,
	INJURY 		= 0x00000020,
	HEAT_COMFORT	= 0x00000040,
	*/
	
	// ONLY COUNT BELLOW
	ENUM_COUNT,

}

class PlayerSoundEventBase extends SoundEventBase
{
	PlayerBase 	m_Player;
	float		m_DummySoundLength;
	float 		m_DummyStartTime;
	bool		m_IsDummyType;
	bool		m_ProcessPlaybackEvent;
	float 		m_PlayTime;

	ref HumanMovementState m_Hms = new HumanMovementState();
	EPlayerSoundEventType m_HasPriorityOverTypes;
	
	bool IsDummy()
	{
		return m_IsDummyType;
	}
		
	EPlayerSoundEventType GetPriorityOverTypes()
	{
		return m_HasPriorityOverTypes;
	}
	
	// !can this event play during hold breath
	bool HasHoldBreathException()
	{
		return false;
	}
	
	void PlayerSoundEventBase()
	{
		m_Type = EPlayerSoundEventType.GENERAL;
	}
	
	void ~PlayerSoundEventBase()
	{
		if(!m_SoundSetCallback) 
			OnEnd();
	}
	
	int GetSoundVoiceAnimEventClassID()
	{
		return m_SoundVoiceAnimEventClassID;
	}
	
	bool HasPriorityOverCurrent(PlayerBase player, EPlayerSoundEventID other_state_id, EPlayerSoundEventType type_other)
	{
		return true;
	}
	
	bool IsFinished()
	{
		if(IsDummy())
		{
			return IsDummyFinished();
		}
		else
		{
			return !IsSoundCallbackExist();
		}
	}
	
	bool IsDummyFinished()
	{
		return GetGame().GetTime() > (m_DummyStartTime + m_DummySoundLength);
	}
	
	
	void OnTick(float delta_time)
	{
		if ( m_SoundSetCallback )
		{
 			m_SoundSetCallback.SetPosition(m_Player.GetPosition());
			//---------- Playback event -------------
			if( delta_time > 0 && m_ProcessPlaybackEvent )//delta_time 0 is for remotes
			{
				
				m_PlayTime += delta_time;
				//this is not 100% precise as the playback position is not obtained from the sound system
				float playback01 = Math.Clamp(m_PlayTime / m_SoundSetCallback.GetLength(),0,1);
				SendEvent(playback01);
			//---------- Playback event -------------
			}
		}
	}

	bool CanPlay(PlayerBase player)
	{
		player.GetMovementState(m_Hms);
		
		if (player.IsHoldingBreath() && !HasHoldBreathException())
		{
			return false;
		}
		if (player.m_IsDrowning || (player.IsSwimming() && m_Hms.m_iMovement != 0))
		{
			return false;
		} 
		return true;
	}
	
	void Init(PlayerBase player)
	{
		InitEx(player,0);
	}
	
	void InitEx(PlayerBase player, int param)
	{
		m_Player = player;
		m_Param = param;
		if (param & EPlayerSoundEventParam.HIGHEST_PRIORITY)
		{
			m_HasPriorityOverTypes = -1;
		}
	}
	
	void OnEnd()
	{
		if(m_ProcessPlaybackEvent)
			SendEvent(1);
	}
	
	void SendEvent(float time)
	{
		if(m_PlayTime < 0 || !m_SoundSetCallback)//negative m_PlayTime value indicates the event has already been sent for playback01 = 1
			return;
		
		m_Player.OnVoiceEventPlayback(this, m_SoundSetCallback, time);
		if(time >= 1)
			m_PlayTime = -float.MAX;
	}
	
	void OnInterupt()
	{

	}
	
	override void OnPlay(PlayerBase player)
	{
		super.OnPlay(player);
		//Print("start playing -------------------->" + m_Type);
		player.OnVoiceEvent(this);
		//m_Player.OnVoiceEventDuration(m_SoundSetCallback, 0);
	}

	override bool Play()
	{
		if(!super.Play())
			return false;
	
		if( !IsDummy() )
		{
			m_SoundSetCallback = m_Player.ProcessVoiceEvent("","", m_SoundVoiceAnimEventClassID);
			
			if(m_SoundSetCallback)
			{
				AbstractWaveEvents events = AbstractWaveEvents.Cast(m_SoundSetCallback.GetUserData());
				events.Event_OnSoundWaveEnded.Insert( OnEnd );
				events.Event_OnSoundWaveStopped.Insert( OnInterupt );
				return true;
			}
			else 
				return false;
		}
		else
		{
			m_DummyStartTime = GetGame().GetTime();
			return true;
		}
		return false;
		
	}
}