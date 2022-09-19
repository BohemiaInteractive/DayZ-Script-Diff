enum eInjurySoundZones
{
	NONE = 0,
	LIGHT,
	MEDIUM,
	HEAVY,
	
	// PUT NEW STATES ABOVE
	COUNT
}

class InjurySoundHandlerBase extends SoundHandlerBase
{
	override void Init()
	{
		m_Id = eSoundHandlers.INJURY;
	}
	
}

//---------------------------
// Client
//---------------------------
class InjurySoundHandlerClient extends InjurySoundHandlerBase
{
	const float SOUND_INTERVALS_LIGHT_MIN 	= 15; const float SOUND_INTERVALS_LIGHT_MAX = 30;
	const float SOUND_INTERVALS_MEDIUM_MIN 	= 10; const float SOUND_INTERVALS_MEDIUM_MAX = 25;
	const float SOUND_INTERVALS_HEAVY_MIN	= 3; const float SOUND_INTERVALS_HEAVY_MAX = 12;
	
	ref HumanMovementState m_MovementState = new HumanMovementState;
	eInjurySoundZones m_InjurySoundZone;
	eInjuryHandlerLevels m_InjuryLevel;
	float m_SoundTime;

	
	eInjurySoundZones DetermineInjuryZone(eInjuryHandlerLevels level)
	{
		if( level == eInjuryHandlerLevels.PRISTINE ) 
			return eInjurySoundZones.NONE;
		
		m_Player.GetMovementState(m_MovementState);
		int speed = m_MovementState.m_iMovement;
		int stance = m_MovementState.m_iStanceIdx;
		
		//int stance_lvl_down = DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_CROUCH;
		
		if( speed == 0 ) 
			return eInjurySoundZones.NONE;
		
		level--;// shift the level so that we play from higher damage
		
		if( stance ==  DayZPlayerConstants.STANCEIDX_PRONE || stance ==  DayZPlayerConstants.STANCEIDX_CROUCH )
		{
			level--;
		}
		
		if( speed == DayZPlayerConstants.MOVEMENTIDX_WALK )
		{
			level--;
		}
		else if(speed == DayZPlayerConstants.MOVEMENTIDX_SPRINT)
		{
			level++;
		}
		level = Math.Clamp(level, eInjurySoundZones.NONE, eInjurySoundZones.HEAVY);
		return level;
	}
	
		
	override void Update()
	{
		m_InjuryLevel = m_Player.m_HealthLevel;
		
		if( m_InjuryLevel != eInjuryHandlerLevels.PRISTINE )
		{
			eInjurySoundZones zone = DetermineInjuryZone(m_Player.m_HealthLevel);
			ProcessSound(zone);
		}
	}
	
	void ProcessSound(eInjurySoundZones zone)
	{
		//Print("injury sound zone:"+ zone);
		// process sound here
		if( GetGame().GetTime() > m_SoundTime)
		{
			float offset_time;
			if(zone == eInjurySoundZones.NONE)
			{
				offset_time = 3000;
				m_SoundTime = GetGame().GetTime() + offset_time;
				return;
			}
			if(zone == eInjurySoundZones.LIGHT)
			{
				offset_time = Math.RandomFloatInclusive(SOUND_INTERVALS_LIGHT_MIN, SOUND_INTERVALS_LIGHT_MAX) * 1000;
			}
			else if(zone == eInjurySoundZones.MEDIUM)
			{
				offset_time = Math.RandomFloatInclusive(SOUND_INTERVALS_MEDIUM_MIN, SOUND_INTERVALS_MEDIUM_MAX) * 1000;
			}
			else if(zone == eInjurySoundZones.HEAVY)
			{
				offset_time = Math.RandomFloatInclusive(SOUND_INTERVALS_HEAVY_MIN, SOUND_INTERVALS_HEAVY_MAX) * 1000;
			}
			m_SoundTime = GetGame().GetTime() + offset_time;
			PlaySound(zone);
		}
	}
	
	void PlaySound(eInjurySoundZones zone)
	{
		m_Player.PlaySoundEvent(EPlayerSoundEventID.INJURED_LIGHT);
	}
	
}


//---------------------------
// Server
//---------------------------
class InjurySoundHandlerServer extends InjurySoundHandlerBase
{


	

}