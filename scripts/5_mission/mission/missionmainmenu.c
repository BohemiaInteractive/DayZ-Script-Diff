class MissionMainMenu extends MissionBase
{
	private UIScriptedMenu m_mainmenu;
	private CreditsMenu m_CreditsMenu;
	private ref DayZIntroScenePC m_IntroScenePC;
	private ref DayZIntroSceneXbox m_IntroSceneXbox;

	bool m_NoCutscene;

	override void OnInit()
	{
		if (!m_NoCutscene)
		{
			CreateIntroScene();
		}
		
		if (!m_mainmenu)
		{
			#ifdef PLATFORM_CONSOLE
			if ( g_Game.GetGameState() != DayZGameState.PARTY )
			{
				m_mainmenu = UIScriptedMenu.Cast( g_Game.GetUIManager().EnterScriptedMenu( MENU_TITLE_SCREEN, null ) );
			}
			#else
				m_mainmenu = UIScriptedMenu.Cast( g_Game.GetUIManager().EnterScriptedMenu( MENU_MAIN, null ) );
			#endif
		}
		
		GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
	}
	
	override void Reset()
	{
		#ifdef PLATFORM_CONSOLE
			delete m_IntroSceneXbox;
		#else
			delete m_IntroScenePC;
		#endif
		
		CreateIntroScene();
	}
	
	DayZIntroScenePC GetIntroScenePC()
	{
		#ifdef PLATFORM_CONSOLE
			Error("missionMainMenu->GetIntroScenePC on PLATFORM_CONSOLE is not implemented!");
			return null;
		#else
			return m_IntroScenePC;
		#endif
	}
	
	DayZIntroSceneXbox GetIntroSceneXbox()
	{
		#ifdef PLATFORM_CONSOLE
			return m_IntroSceneXbox;
		#else
			Error("missionMainMenu->GetIntroScenePC on PLATFORM_PC is not implemented!");
			return null;
		#endif
	}

	void CreateIntroScene()
	{		
#ifdef PLATFORM_CONSOLE
		m_IntroSceneXbox = new DayZIntroSceneXbox;
#else
		m_IntroScenePC = new DayZIntroScenePC;
#endif
	}
	
	override void UpdateInputDevicesAvailability()
	{
		super.UpdateInputDevicesAvailability();
		
		g_Game.GetInput().UpdateConnectedInputDeviceList();
		g_Game.UpdateInputDeviceDisconnectWarning();
	}

	override void OnMissionStart()
	{
		g_Game.GetUIManager().ShowUICursor(true);
		g_Game.SetMissionState(DayZGame.MISSION_STATE_MAINMENU);
		m_DynamicMusicPlayer.SetCategory(EDynamicMusicPlayerCategory.MENU, true);
		
		g_Game.LoadingHide(true);
		ProgressAsync.DestroyAllPendingProgresses();
	}
	
	override void OnMissionFinish()
	{
		if ( m_mainmenu )
			m_mainmenu.Cleanup();
		GetGame().GetUIManager().CloseAll();
		m_mainmenu = NULL;
		
		m_IntroScenePC = null;
		m_IntroSceneXbox = null;
		m_CreditsMenu = null;
#ifndef FEATURE_CURSOR
		g_Game.GetUIManager().ShowUICursor(false);
#endif
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		
#ifdef DIAG_DEVELOPER
		UpdateInputDeviceDiag();
#endif
		
		if ( g_Game.IsLoading() )
		{
			return;
		}
		
		if (m_IntroScenePC)
		{
			m_IntroScenePC.Update();
		}
	}
	
	void OnMenuEnter(int menu_id)
	{
		switch (menu_id)
		{
			case MENU_CREDITS:
			{
				m_CreditsMenu = CreditsMenu.Cast(GetGame().GetUIManager().GetMenu());
			}
		}
	}
	
	void OnInputDeviceChanged(int device)
	{
		if (m_CreditsMenu)
		{
			m_CreditsMenu.UpdateInfoPanelText(device);
		}
	}
	
	int SortedInsert( array<int> list, int number )
	{
		int find_number = number;
		int index_min = 0;
		int index_max = list.Count() - 1;
		int target_index = Math.Floor( index_max / 2 );
		
		if ( index_max == -1 )
		{
			list.Insert( number );
			return 0;
		}
		
		while ( true )
		{
			int target_value = list[target_index];
			
			if ( find_number == target_value || ((index_max - index_min) <= 1) )
			{
				for ( int i = index_min; i <= index_max; i++ )
				{
					if ( find_number <= list[i] )
					{
						list.InsertAt( find_number, i );
						return i;
					}
				}
				
				index_max++;
				list.InsertAt( find_number, index_max );
				return target_index;
			}
			else if ( find_number < target_value )
			{
				index_max = target_index;
				target_index = Math.Floor( target_index / 2 );
			}
			else if ( find_number > target_value )
			{
				index_min = target_index;
				target_index += Math.Floor( (index_max - index_min) / 2 );
			}			
		}
		
		return target_index;
	}

	//!	
	//! DEPRECATED
	//!
	private AbstractWave m_MenuMusic;

	void PlayMusic()
	{
		if ( !m_MenuMusic )
		{
			SoundParams soundParams			= new SoundParams( "Music_Menu_SoundSet" );
			SoundObjectBuilder soundBuilder	= new SoundObjectBuilder( soundParams );
			SoundObject soundObject			= soundBuilder.BuildSoundObject();
			soundObject.SetKind( WaveKind.WAVEMUSIC );
			m_MenuMusic = GetGame().GetSoundScene().Play2D(soundObject, soundBuilder);
			m_MenuMusic.Loop( true );
			m_MenuMusic.Play();
		}
	}
	
	void StopMusic()
	{
		if ( m_MenuMusic )
			m_MenuMusic.Stop();
	}
	
	AbstractWave GetMenuMusic()
	{
		return m_MenuMusic;
	}
}
