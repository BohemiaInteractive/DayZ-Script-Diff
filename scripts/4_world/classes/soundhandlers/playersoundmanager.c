
const float SOUNDS_HEARING_DISTANCE = 50;

enum eSoundHandlers
{
	STAMINA,
	HUNGER,
	INJURY,
	//FREEZING,
	
	//------
	// add stuff above !
	//------
	COUNT
}



class PlayerSoundManagerBase
{
	PlayerBase m_Player;
	
	const int MAX_HANDLERS_COUNT = eSoundHandlers.COUNT;
	ref SoundHandlerBase m_Handlers[MAX_HANDLERS_COUNT];
	
	void PlayerSoundManagerBase(PlayerBase player)
	{
		m_Player = player;
		Init();
	}
	
	void RegisterHandler(SoundHandlerBase handler)
	{
		int index = handler.GetID();
		m_Handlers[index] = handler;
	}
	
	SoundHandlerBase GetHandler(eSoundHandlers id)
	{
		return m_Handlers[id];
	}
	
	void Init()
	{
	
	}
	
	void Update()
	{
	
	}
	

}

class PlayerSoundManagerServer extends PlayerSoundManagerBase
{
	//------------------------
	// REGISTER HANDLER HERE 
	//------------------------
	override void Init()
	{
		RegisterHandler(new StaminaSoundHandlerServer(m_Player));
		RegisterHandler(new HungerSoundHandlerServer(m_Player));
		RegisterHandler(new InjurySoundHandlerServer(m_Player));
	}
	
	override void Update()
	{
		if( m_Player.IsUnconscious() )
		{
			return;
		}
		
		for(int i = 0; i < (MAX_HANDLERS_COUNT - 1); i++)
		{
			m_Handlers[i].Update();
		}
	}
}

class PlayerSoundManagerClient extends PlayerSoundManagerBase
{
	ref Timer m_ClientCharacterTick = new Timer;
	ref Timer m_ClientCharacterDistanceCheck = new Timer;
	bool m_UpdateTimerRunning;

	//------------------------
	// REGISTER HANDLER HERE 
	//------------------------
	override void Init()
	{
		RegisterHandler(new StaminaSoundHandlerClient(m_Player));
		RegisterHandler(new HungerSoundHandlerClient(m_Player));
		RegisterHandler(new InjurySoundHandlerClient(m_Player));
		//RegisterHandler(new FreezingSoundHandlerClient(m_Player));
	}
	
	void PlayerSoundManagerClient(PlayerBase player)
	{
		
		if( !GetGame().IsDedicatedServer() ) 
		{
			m_ClientCharacterDistanceCheck.Run(2, this, "CheckAllowUpdate", null, true);
		}
	}
	
	void SetAllowUpdate(bool enable)
	{
		if( enable )
		{
			m_UpdateTimerRunning = true;
			m_ClientCharacterTick.Run(0.03, this, "Update", null, true);
		}
		else
		{
			m_UpdateTimerRunning = false;
			m_ClientCharacterTick.Stop();
		}
	}
	
	override void Update()
	{
		if( !m_Player.IsAlive() )
		{
			SetAllowUpdate(false);
			return;
		}

#ifndef NO_GUI
		for(int i = 0; i < MAX_HANDLERS_COUNT; i++)
		{
			m_Handlers[i].Update();
		}
#endif
	}
	
	void CheckAllowUpdate()
	{
		if( GetGame().GetPlayer() )
		{
			bool is_at_hearing_distance = vector.Distance(GetGame().GetPlayer().GetPosition(), m_Player.GetPosition()) < SOUNDS_HEARING_DISTANCE;
			
			if( m_UpdateTimerRunning && !is_at_hearing_distance )
			{
				SetAllowUpdate(false);
			}
			else if( !m_UpdateTimerRunning && is_at_hearing_distance )
			{
				SetAllowUpdate(true);
			}
			//PrintString("distance:" + vector.Distance(GetGame().GetPlayer().GetPosition(), m_Player.GetPosition()).ToString());
		}
	}
}