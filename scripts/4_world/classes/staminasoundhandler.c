const float TICK_INTERVAL = 1;

const float STAMINA_SOUND_TR1 = 0.55;
const float STAMINA_SOUND_TR2 = 0.25;

enum eStaminaZones
{
	ZONE0,
	ZONE1,
	ZONE2,
}

enum eStaminaTendency
{
	UP,
	DOWN,
}

enum eStaminaState
{
	ZONE0_UP = 1,
	ZONE0_DOWN,
	ZONE1_UP,
	ZONE1_DOWN,
	ZONE2_UP,
	ZONE2_DOWN,
	//count bellow, add above
	COUNT
}


class SoundHandlerBase
{
	eSoundHandlers m_Id;
	PlayerBase m_Player;
	
	void Update();
	
	eSoundHandlers GetID()
	{
		return m_Id;
	}
	
	void SoundHandlerBase(PlayerBase player)
	{
		m_Player = player;
		Init();
	}
	
	void Init();
}

class StaminaSoundHandlerBase extends SoundHandlerBase
{

	override void Init()
	{
		m_Id = eSoundHandlers.STAMINA;
	}
	
	eStaminaState GetStaminaState(eStaminaTendency tendency, eStaminaZones zone)
	{
		eStaminaState stamina_state;
		
		zone = Math.Clamp(zone, eStaminaZones.ZONE0, eStaminaZones.ZONE2);
		
		if(zone == eStaminaZones.ZONE1 && tendency == eStaminaTendency.DOWN)
		{
			stamina_state = eStaminaState.ZONE1_DOWN;
		}
		
		if(zone == eStaminaZones.ZONE1 && tendency == eStaminaTendency.UP)
		{
			stamina_state = eStaminaState.ZONE1_UP;
		}
		
		if(zone == eStaminaZones.ZONE2 && tendency == eStaminaTendency.DOWN)
		{
			stamina_state = eStaminaState.ZONE2_DOWN;
		}
		
		if(zone == eStaminaZones.ZONE2 && tendency == eStaminaTendency.UP)
		{
			stamina_state = eStaminaState.ZONE2_UP;
		}
		
		if(zone == eStaminaZones.ZONE0 && tendency == eStaminaTendency.UP)
		{
			stamina_state = eStaminaState.ZONE0_UP;
		}
		
		if(zone == eStaminaZones.ZONE0 && tendency == eStaminaTendency.DOWN)
		{
			stamina_state = eStaminaState.ZONE0_DOWN;
		}
	
		return stamina_state;
	}
}


//---------------------------
// Server
//---------------------------
class StaminaSoundHandlerServer extends StaminaSoundHandlerBase
{
	float m_Stamina;
	StaminaHandler m_StaminaHandler;
	float m_StaminaLastValue;
	int m_StaminaZoneOffset;
	
	void SetStaminaZoneOffset(int zone_offset)
	{
		m_StaminaZoneOffset = zone_offset;
	}
	
	// ! Called from command handler
	override void Update()
	{
		m_Stamina = m_Player.GetStaminaHandler().GetStamina();
		float stamina_delta = m_StaminaLastValue - m_Stamina;
		eStaminaTendency stamina_tendency;
		eStaminaZones stamina_zone;
		
		//if( stamina_delta == 0 ) return;
		
		if( stamina_delta > 0 )
		{
			stamina_tendency = eStaminaTendency.DOWN;
		}
		if( stamina_delta <= 0 )
		{
			stamina_tendency = eStaminaTendency.UP;
		}
		
		m_StaminaLastValue = m_Stamina;
		
		stamina_zone = GetZone(m_Stamina);
		m_Player.SetStaminaState( GetStaminaState(stamina_tendency, stamina_zone + m_StaminaZoneOffset) );
		
		//EPlayerSoundEventID sound_event_id = GetPlayerSoundEventID();
		/*
		//PrintString("stamina delta " +stamina_delta.ToString());
		//PrintString("stamina zone " +m_StaminaZone.ToString());
		//PrintString("stamina tendency " +m_StaminaTendency.ToString());
		*/
		//PrintString(" sound event id " +sound_event_id.ToString());
		
		
		//if( sound_event_id !=0 ) m_Player.SendSoundEvent(sound_event_id);
	}

	eStaminaZones GetZone(float stamina)
	{
		float stamina_normalized = Math.InverseLerp(0, m_Player.GetStaminaHandler().GetStaminaCap(), stamina);
		stamina_normalized = Math.Clamp(stamina_normalized,0,1);
		//PrintString(" stamina_normalized " +stamina_normalized.ToString());
		eStaminaZones stamina_zone = eStaminaZones.ZONE0;
		
		if( stamina_normalized < STAMINA_SOUND_TR1)
		{
			stamina_zone = eStaminaZones.ZONE1;
		}
		
		if( stamina_normalized < STAMINA_SOUND_TR2)
		{
			stamina_zone = eStaminaZones.ZONE2;
		}
		
		return stamina_zone;
	}
}


//---------------------------
// Client
//---------------------------
class StaminaSoundHandlerClient extends StaminaSoundHandlerBase
{
	float m_PostponeTime = -1;

	float m_LastTick;
	bool m_UpdateTimerRunning;
	bool m_StaminaUpEndPlayed = true;
	ref Timer m_ClientCharacterTick = new Timer;
	ref Timer m_ClientCharacterDistanceCheck = new Timer;
	

	void PostponeStamina(float time)
	{
		m_PostponeTime = GetGame().GetTime() + time;
	}

	
	override void Update()
	{
		if( GetGame().GetTime() < m_PostponeTime || m_Player.GetPlayerSoundEventHandler().m_CurrentState )
		{
			return;
		}
		//Print(Math.RandomFloat01());	
		eStaminaState stamina_state = m_Player.GetStaminaState();
		switch(stamina_state)
		{
			case eStaminaState.ZONE1_DOWN:
				if(m_Player.PlaySoundEvent(EPlayerSoundEventID.STAMINA_DOWN_LIGHT))
				{
					m_StaminaUpEndPlayed = false;
				}
			break;
			
			case eStaminaState.ZONE1_UP:
				if(m_Player.PlaySoundEvent(EPlayerSoundEventID.STAMINA_UP_LIGHT))
				{
					m_StaminaUpEndPlayed = false;
				}
			break;
			
			case eStaminaState.ZONE2_DOWN:
				if(m_Player.PlaySoundEvent(EPlayerSoundEventID.STAMINA_DOWN_HEAVY))
				{
					m_StaminaUpEndPlayed = false;
				}
			break;
			
			case eStaminaState.ZONE2_UP:
				if(m_Player.PlaySoundEvent(EPlayerSoundEventID.STAMINA_UP_HEAVY))
				{
					m_StaminaUpEndPlayed = false;
				}
			break;
			
			case eStaminaState.ZONE0_UP:
				if(!m_StaminaUpEndPlayed && m_Player.PlaySoundEvent(EPlayerSoundEventID.STAMINA_UP_END))
				{
					m_StaminaUpEndPlayed = true;
				}
				else
				{
					m_Player.PlaySoundEvent(EPlayerSoundEventID.STAMINA_NORMAL_DUMMY);
				}
			break;
			
			case eStaminaState.ZONE0_DOWN:
				m_Player.PlaySoundEvent(EPlayerSoundEventID.STAMINA_NORMAL_DUMMY);
			break;
		}
	}
}