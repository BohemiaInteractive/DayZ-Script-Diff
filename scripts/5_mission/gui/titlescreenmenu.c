/*! Xbox menu */
class TitleScreenMenu extends UIScriptedMenu
{
	RichTextWidget m_TextPress;
	
	void TitleScreenMenu()
	{
		g_Game.SetGameState( DayZGameState.MAIN_MENU );
		g_Game.SetLoadState( DayZLoadState.MAIN_MENU_START );
	}
	
	void ~TitleScreenMenu()
	{
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/xbox/day_z_title_screen.layout");
		
		MissionMainMenu mission = MissionMainMenu.Cast( g_Game.GetMission() );
		
		m_TextPress = RichTextWidget.Cast( layoutRoot.FindAnyWidget("InputPromptText") );
		if (m_TextPress)
		{
			string gamertag;
			string text = Widget.TranslateString( "#console_start_game" );
			GetGame().GetPlayerName(gamertag);
			#ifdef PLATFORM_XBOX
				BiosUserManager user_manager = GetGame().GetUserManager();
				if( user_manager && user_manager.GetSelectedUser() )
					m_TextPress.SetText( string.Format( text, "<image set=\"xbox_buttons\" name=\"A\" />" ) );
				else
					m_TextPress.SetText( string.Format( text, "<image set=\"xbox_buttons\" name=\"A\" />" ) );
			#endif
					
			#ifdef PLATFORM_PS4
				string confirm = "cross";
				if( GetGame().GetInput().GetEnterButton() == GamepadButton.A )
				{
					confirm = "cross";
				}
				else
				{
					confirm = "circle";
				}
				m_TextPress.SetText( string.Format( text, "<image set=\"playstation_buttons\" name=\"" + confirm + "\" />" ) );
			#endif
		}
		return layoutRoot;
	}
	
	override void OnShow()
	{
		if( g_Game.GetGameState() != DayZGameState.CONNECTING )
		{
			#ifdef PLATFORM_CONSOLE
				g_Game.GamepadCheck();
			#endif
		}
		layoutRoot.FindAnyWidget("notification_root").Show(false);
		
		#ifdef PLATFORM_CONSOLE
		#ifdef PLATFORM_XBOX
		#ifdef BUILD_EXPERIMENTAL
			layoutRoot.FindAnyWidget("notification_root").Show(true);
		#endif
		#endif
		#endif
		SetWidgetAnimAlpha( m_TextPress );
	}
	
	override void OnHide()
	{
		layoutRoot.FindAnyWidget("notification_root").Show(false);
	}
	
	override void Update(float timeslice)
	{
		super.Update( timeslice );
		
		if( GetGame().GetInput().LocalPress("UAUISelect",false) )
		{
			#ifdef PLATFORM_WINDOWS
				EnterScriptedMenu(MENU_MAIN);
			#endif
			#ifdef PLATFORM_XBOX
				g_Game.GamepadCheck();
			#endif
		}
	}
}