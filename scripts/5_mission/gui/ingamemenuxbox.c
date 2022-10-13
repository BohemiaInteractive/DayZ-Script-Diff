class InGameMenuXbox extends UIScriptedMenu
{
//#ifdef PLATFORM_CONSOLE
	
	// Widgets texts id
	protected  string 						m_MuteButtonTextID;
	protected  string 						m_UnmuteButtonTextID;
	protected  string 						m_BackButtonTextID;
	protected  string 						m_SelectButtonTextID;
	protected  string 						m_OpenGameCardButtonTextID;
	
	protected bool 							m_PlayerAlive;
	
	protected ref PlayerListScriptedWidget	m_ServerInfoPanel;
	
	protected Widget						m_OnlineMenu;
	
	protected ButtonWidget					m_ContinueButton;
	protected ButtonWidget					m_ExitButton;
	protected ButtonWidget					m_RestartDeadButton;
	protected ButtonWidget					m_RestartButton;
	protected ButtonWidget					m_OptionsButton;
	protected ButtonWidget					m_ControlsButton;
	protected ButtonWidget					m_OnlineButton;
	protected ButtonWidget					m_TutorialsButton;
	
	protected TextWidget					m_Version;
	
	const int 								BUTTON_XBOX_CONTROLS = 201;
	
	void InGameMenuXbox()
	{
		GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
	}

	void ~InGameMenuXbox()
	{
		ClientData.SyncEvent_OnPlayerListUpdate.Remove( SyncEvent_OnRecievedPlayerList );
		OnlineServices.m_PermissionsAsyncInvoker.Remove( OnPermissionsUpdate );
		
		Mission mission = GetGame().GetMission();
		if ( mission )
		{
			IngameHud hud = IngameHud.Cast( mission.GetHud() );
			if ( hud )
			{
				hud.ShowHudUI(true);
				hud.ShowQuickbarUI(true);
			}
		}
		PPERequesterBank.GetRequester(PPERequester_MenuEffects).Stop();
	}
	
	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		#endif
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		switch (pInputDeviceType)
		{
		case EInputDeviceType.CONTROLLER:
			UpdateControlsElements();
			layoutRoot.FindAnyWidget("toolbar_bg").Show(true);
		break;

		default:
			if (GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
			{
				layoutRoot.FindAnyWidget("toolbar_bg").Show(false);
			}
		break;
		}
	}
	
	override Widget Init()
	{
		layoutRoot		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/xbox/day_z_ingamemenu_xbox.layout");
		
		m_OnlineMenu	= GetGame().GetWorkspace().CreateWidgets("gui/layouts/xbox/ingamemenu_xbox/online_info_menu.layout", layoutRoot);
		m_OnlineMenu.Show( false );
		
		m_ContinueButton	= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "continuebtn" ) );
		m_RestartDeadButton	= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "restartbtn_dead" ) );
		m_ExitButton		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "exitbtn" ) );
		m_RestartButton		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "restartbtn" ) );
		m_OptionsButton		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "optionsbtn" ) );
		m_ControlsButton	= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "controlsbtn" ) );
		m_OnlineButton		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "onlinebtn" ) );
		m_TutorialsButton	= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "tutorialsbtn" ) );
		m_Version			= TextWidget.Cast( layoutRoot.FindAnyWidget( "version" ) );
		
		Man player = GetGame().GetPlayer();
		if (player)
		{
			int life_state = player.GetPlayerState();

			if (life_state == EPlayerStates.ALIVE)
			{
				m_PlayerAlive = true;
			}
		}
		
		UpdateMenuFocus();
		
		string version;
		GetGame().GetVersion( version );
		#ifdef PLATFORM_CONSOLE
			version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		#else
			version = "#main_menu_version" + " " + version;
		#endif
		m_Version.SetText( version );
		
		if ( GetGame().IsMultiplayer() )
		{
			m_OnlineButton.Show( true );
			
			TextWidget w_text = TextWidget.Cast(m_OnlineMenu.FindAnyWidget("OnlineTextWidget"));
			w_text.SetText(g_Game.GetHostName());
			
			m_ServerInfoPanel = new PlayerListScriptedWidget( m_OnlineMenu.FindAnyWidget( "ServerInfoPanel" ) );
			
			OnlineServices.m_PermissionsAsyncInvoker.Insert( OnPermissionsUpdate );
			ClientData.SyncEvent_OnPlayerListUpdate.Insert( SyncEvent_OnRecievedPlayerList );
			
			m_ServerInfoPanel.Reload( ClientData.m_PlayerList );
			m_ServerInfoPanel.ReloadLocal( OnlineServices.GetMuteList() );
			
			string uid = m_ServerInfoPanel.FindPlayerByWidget( GetFocus() );
			if ( uid != "" )
			{
				if ( IsLocalPlayer( uid ) || m_ServerInfoPanel.IsEmpty() )
				{
					layoutRoot.FindAnyWidget( "Mute" ).Show( false );
					layoutRoot.FindAnyWidget( "Gamercard" ).Show( false );
				}
				else
				{
					layoutRoot.FindAnyWidget( "Mute" ).Show( !GetGame().GetWorld().IsDisabledReceivingVoN() );
					#ifndef PLATFORM_PS4
					layoutRoot.FindAnyWidget( "Gamercard" ).Show( true );
					#endif
					SetMuteButtonText(OnlineServices.IsPlayerMuted( uid ));
				}
				
				if ( m_ServerInfoPanel.IsGloballyMuted( uid ) )
				{
					layoutRoot.FindAnyWidget( "Mute" ).Show( false );
				}
			}
		}
		else
		{
			layoutRoot.FindAnyWidget( "onlinebtn" ).Show( false );
			layoutRoot.FindAnyWidget( "invitebtn" ).Show( false );
		}
		
		layoutRoot.FindAnyWidget( "Gamercard" ).Show( false );
		layoutRoot.FindAnyWidget( "Mute" ).Show( false );
		
		//RESPAWN & RESTART
		ButtonWidget restart_btn = ButtonWidget.Cast( layoutRoot.FindAnyWidgetById( IDC_INT_RETRY ) );
		if (GetGame().IsMultiplayer())
		{
			restart_btn.SetText("#main_menu_respawn");
		}
		else
		{
			restart_btn.SetText("#main_menu_restart");
		}
		
		if ( GetGame().IsMultiplayer() && !( GetGame().CanRespawnPlayer() || ( player && player.IsUnconscious() ) ) )
		{
			restart_btn.Enable( false );
			restart_btn.Show( false );
		}
		//
		
		#ifdef BULDOZER
		delete restart_btn;
		#endif
		
		Mission mission = GetGame().GetMission();
		if ( mission )
		{
			IngameHud hud = IngameHud.Cast( mission.GetHud() );
			if ( hud )
			{
				hud.ShowHudUI(false);
				hud.ShowQuickbarUI(false);
			}
		}
		
		PPERequester_MenuEffects requester;
		Class.CastTo(requester,PPERequesterBank.GetRequester(PPERequester_MenuEffects));
		requester.SetVignetteIntensity(0.6);
			
		UpdateControlsElements();

		/*#ifdef PLATFORM_XBOX
		if ( CGame.IsDigitalCopy() )
		{
			layoutRoot.FindAnyWidget( "PhysicalCopy" ).Show( false );
			layoutRoot.FindAnyWidget( "DigitalCopy" ).Show( true );
		}
		else
		{
			layoutRoot.FindAnyWidget( "PhysicalCopy" ).Show( true );
			layoutRoot.FindAnyWidget( "DigitalCopy" ).Show( false );
		}
		#endif*/
		LoadTextStrings();
		LoadFooterButtonTexts();
		
		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());
		
		return layoutRoot;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		switch ( w.GetUserID() )
		{
			case IDC_MAIN_CONTINUE:
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetMission().Continue);
				return true;
			}
			case IDC_MAIN_OPTIONS:
			{
				EnterScriptedMenu(MENU_OPTIONS);
				return true;
			}
			case BUTTON_XBOX_CONTROLS:
			{
				EnterScriptedMenu(MENU_XBOX_CONTROLS);
				return true;
			}
			case IDC_MAIN_QUIT:
			{
				GetGame().GetUIManager().ShowDialog("#main_menu_exit", "#main_menu_exit_desc", IDC_INT_EXIT, DBT_YESNO, DBB_YES, DMT_QUESTION, NULL);
				return true;
			}
			case IDC_INT_RETRY:
			{
				if ( !GetGame().IsMultiplayer() )
				{
					GetGame().GetUIManager().ShowDialog("#main_menu_restart", "Are you sure you want to restart?", IDC_INT_RETRY, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
				}
				else
				{
					GetGame().GetUIManager().ShowDialog("#main_menu_respawn", "#main_menu_respawn_question", IDC_INT_RETRY, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
				}
				return true;
			}
			case IDC_MAIN_ONLINE:
			{
				m_OnlineMenu.Show( true );
				layoutRoot.FindAnyWidget( "play_panel_root" ).Show( false );
				layoutRoot.FindAnyWidget( "play_panel_root2" ).Show( GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer() );
				//layoutRoot.FindAnyWidget( "dayz_logo" ).Show( false );
				layoutRoot.FindAnyWidget( "Select" ).Show( false );
				m_ServerInfoPanel.FocusFirst();
				return true;
			}
			case 117:
			{
				EnterScriptedMenu(MENU_TUTORIAL);
				return true;
			}
			case IDC_MULTI_INVITE:
			{
				OnlineServices.ShowInviteScreen();
				return true;
			}
		}
		
		if( w == layoutRoot.FindAnyWidget( "backbtn" ) )
		{
			CloseOnline();
		}
		else if ( w == m_RestartDeadButton )
		{
			if ( GetGame().GetMission().GetRespawnModeClient() == GameConstants.RESPAWN_MODE_CUSTOM )
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetUIManager().EnterScriptedMenu,MENU_RESPAWN_DIALOGUE,this);
			}
			else
			{
				GameRetry(true);
			}
			return true;
		}

		return false;
	}
	
	override bool OnModalResult(Widget w, int x, int y, int code, int result)
	{
		super.OnModalResult(w, x, y, code, result);
		
		if ( code == IDC_INT_EXIT && result == DBB_YES )
		{
			if (GetGame().IsMultiplayer())
			{
				GetGame().LogoutRequestTime();
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetMission().CreateLogoutMenu, this);
			}
			else
			{
				// skip logout screen in singleplayer
				GetGame().GetMission().AbortMission();
			}
			g_Game.CancelLoginTimeCountdown();
			
			return true;	
		
		}
		else if ( code == IDC_INT_EXIT && result == DBB_NO )
		{
			g_Game.CancelLoginTimeCountdown();
		}
		else if ( code == IDC_INT_RETRY )
		{
			if ( result == DBB_YES )
			{
				if ( GetGame().GetMission().GetRespawnModeClient() == GameConstants.RESPAWN_MODE_CUSTOM )
				{
					GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetUIManager().EnterScriptedMenu,MENU_RESPAWN_DIALOGUE,this);
				}
				else
				{
					GameRetry(true);
					return true;
				}
			}
			else
			{
				UpdateMenuFocus();
			}
		}
		
		return false;
	}
	
	void GameRetry(bool random)
	{
		if ( GetGame().IsMultiplayer() )
		{
			GetGame().GetUIManager().CloseAll();
			GetGame().GetMenuDefaultCharacterData(false).SetRandomCharacterForced(random); //todo
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().RespawnPlayer);
			//turns off dead screen, hides HUD for countdown
			//---------------------------------------------------
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if(player)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(player.ShowDeadScreen, DayZPlayerImplement.DEAD_SCREEN_DELAY, false, false);
			}
			
			MissionGameplay missionGP = MissionGameplay.Cast(GetGame().GetMission());
			missionGP.DestroyAllMenus();
			missionGP.SetPlayerRespawning(true);
			//---------------------------------------------------
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(missionGP.Continue);
		}
		else
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().RestartMission);
		}
		//Close();
	}
	
	void MenuRequestRespawn(UIScriptedMenu menu, bool random)
	{
		if (RespawnDialogue.Cast(menu))
			GameRetry(random);
	}
	
	bool IsLocalPlayer( string uid )
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		string local_uid;
		if( GetGame().GetUserManager() )
			local_uid = GetGame().GetUserManager().GetSelectedUser().GetUid();
		return ( uid == local_uid );
	}
	
	SyncPlayerList CreateFakePlayerList( int player_count )
	{
		SyncPlayerList player_list = new SyncPlayerList;
		player_list.m_PlayerList = new array<ref SyncPlayer>;
		for ( int i = 0; i < player_count; i++ )
		{
			SyncPlayer sync_player = new SyncPlayer;
			sync_player.m_UID = "uid" + i;
			sync_player.m_PlayerName = "Player " + i;
			player_list.m_PlayerList.Insert( sync_player );
		}
		return player_list;
	}
	
	override void Update( float timeslice )
	{
		UpdateGUI();
		
		if( GetGame().IsMultiplayer() && layoutRoot.FindAnyWidget( "OnlineInfo" ).IsVisible() )
		{
			PlayerListEntryScriptedWidget selected;
			if( m_ServerInfoPanel )
				selected = m_ServerInfoPanel.GetSelectedPlayer();
			if( GetGame().GetInput().LocalPress( "UAUICtrlX", false ) )
			{
				if( selected )
					m_ServerInfoPanel.ToggleMute( selected.GetUID() );
				Refresh();
			}
			
			#ifndef PLATFORM_PS4
			if( GetGame().GetInput().LocalPress( "UAUICtrlY", false ) )
			{
				if( selected )
					OnlineServices.ShowUserProfile( selected.GetUID() );
			}
			#endif
		}
	}
	
	void UpdateGUI()
	{
		Man player = GetGame().GetPlayer();
		if (player)
		{
			int life_state = player.GetPlayerState();
			m_PlayerAlive = life_state == EPlayerStates.ALIVE;
		}
		
		if ( m_PlayerAlive )
		{
			m_RestartButton.Show( player.IsUnconscious() );
		}
		else
		{
			m_RestartButton.Show( false );
		}
		
		m_ContinueButton.Show( m_PlayerAlive );
		m_RestartDeadButton.Show( !m_PlayerAlive );		
	}
	
	bool IsOnlineOpen()
	{
		return m_OnlineMenu.IsVisible();
	}

	void CloseOnline()
	{
		m_OnlineMenu.Show( false );
		layoutRoot.FindAnyWidget( "play_panel_root2" ).Show( false );
		layoutRoot.FindAnyWidget( "play_panel_root" ).Show( true );
		layoutRoot.FindAnyWidget( "dayz_logo" ).Show( true );
		layoutRoot.FindAnyWidget( "Select" ).Show( true );
		layoutRoot.FindAnyWidget( "Mute" ).Show( false );
		layoutRoot.FindAnyWidget( "Gamercard" ).Show( false );
		
		SetFocus( m_OnlineButton );
	}
	
	void SelectServer()
	{
		if( m_ServerInfoPanel )
		{
			m_ServerInfoPanel.FocusFirst();
			
			Refresh();
		}
	}
	
	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		return false;
	}
	
	void SyncEvent_OnRecievedPlayerList( SyncPlayerList player_list )
	{
		m_ServerInfoPanel.Reload( player_list );
		
		Refresh();
	}
	
	void OnPermissionsUpdate( BiosPrivacyUidResultArray result_list )
	{
		m_ServerInfoPanel.Reload( result_list );
		
		Refresh();
	}
	
	override void OnShow()
	{
		super.OnShow();
		Man player = GetGame().GetPlayer();
		if (player)
		{
			int life_state = player.GetPlayerState();

			if (life_state == EPlayerStates.ALIVE)
			{
				m_PlayerAlive = true;
			}
		}
		
		if ( m_PlayerAlive )
		{
			m_RestartButton.Show( player.IsUnconscious() );
		}
		else
		{
			m_RestartButton.Show( false );
		}
		m_ContinueButton.Show( m_PlayerAlive );
		m_RestartDeadButton.Show( !m_PlayerAlive );
		
		UpdateMenuFocus();
		
		#ifdef PLATFORM_CONSOLE
			bool mk = GetGame().GetInput().IsEnabledMouseAndKeyboard();
			bool mk_server = GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer();
			TextWidget warning = TextWidget.Cast( layoutRoot.FindAnyWidget( "MouseAndKeyboardWarning" ) );
			if( mk )
			{
				if( mk_server )
				{
					warning.SetText( "#str_mouse_and_keyboard_server_warning" );
				}
				else
				{
					warning.SetText( "#str_controller_server_warning" );
				}
			}
		
			warning.Show( mk );
		#endif
	}
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorHighlight( w );
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorNormal( w );
			return true;
		}
		return false;
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorHighlight( w );
			return true;
		}
		return false;
	}
	
	override bool OnFocusLost( Widget w, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorNormal( w );
			return true;
		}
		return false;
	}
	
	bool IsFocusable( Widget w )
	{
		if( w )
		{
			if( w == m_ContinueButton || w == m_ExitButton || w == m_RestartButton || w == m_OptionsButton || w == m_ControlsButton || w == m_OnlineButton || w == m_TutorialsButton );
				return true;
		}
		return false;
	}
	
	override void Refresh()
	{
		string version;
		GetGame().GetVersion( version );
		#ifdef PLATFORM_CONSOLE
			version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		#else
			version = "#main_menu_version" + " " + version;
		#endif
		m_Version.SetText( version );
		
		if( GetGame().IsMultiplayer() && layoutRoot.FindAnyWidget( "OnlineInfo" ).IsVisible() && m_ServerInfoPanel )
		{
			PlayerListEntryScriptedWidget selected = m_ServerInfoPanel.GetSelectedPlayer();
			if( selected && !selected.IsLocalPlayer() )
			{
				layoutRoot.FindAnyWidget( "Mute" ).Show( !GetGame().GetWorld().IsDisabledReceivingVoN() && !selected.IsGloballyMuted() );
				#ifndef PLATFORM_PS4
				layoutRoot.FindAnyWidget( "Gamercard" ).Show( true );
				#endif							
				SetMuteButtonText( selected.IsMuted() );
			}
			else
			{
				layoutRoot.FindAnyWidget( "Mute" ).Show( false );
				layoutRoot.FindAnyWidget( "Gamercard" ).Show( false );
			}
		}
	}
	
	void ColorDisable( Widget w )
	{
		#ifdef PLATFORM_WINDOWS
		SetFocus( null );
		#endif
		
		ButtonWidget button = ButtonWidget.Cast( w );
		if( button && button != m_ContinueButton )
		{
			button.SetTextColor( ARGB( 255, 255, 255, 255 ) );
		}
		ButtonSetColor( w, ARGB(0, 0, 0, 0) );
		ButtonSetTextColor(w,  ARGB(60, 0, 0, 0) );
	}
	
	void ColorHighlight( Widget w )
	{
		if( !w )
			return;
				
		int color_pnl = ARGB(255, 0, 0, 0);
		int color_lbl = ARGB(255, 255, 0, 0);
		
		#ifdef PLATFORM_CONSOLE
			color_pnl = ARGB(255, 200, 0, 0);
			color_lbl = ARGB(255, 255, 255, 255);
		#endif
		
		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}
	
	void ColorNormal( Widget w )
	{
		if( !w )
			return;
		
		int color_pnl = ARGB(0, 0, 0, 0);
		int color_lbl = ARGB(255, 255, 255, 255);
		
		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}
	
	void ButtonSetText( Widget w, string text )
	{
		if( !w )
			return;
				
		TextWidget label = TextWidget.Cast(w.FindWidget( w.GetName() + "_label" ) );
		
		if( label )
		{
			label.SetText( text );
		}
		
	}
	
	void ButtonSetColor( Widget w, int color )
	{
		if( !w )
			return;
		
		Widget panel = w.FindWidget( w.GetName() + "_panel" );
		
		if( panel )
		{
			panel.SetColor( color );
		}
	}
	
	void ButtonSetTextColor( Widget w, int color )
	{
		if( !w )
			return;

		TextWidget label	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_label" ) );
		TextWidget text		= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text_1" ) );
				
		if( label )
		{
			label.SetColor( color );
		}
		
		if( text )
		{
			text.SetColor( color );
		}
		
		if( text2 )
		{
			text2.SetColor( color );
		}
	}
	
	/// Set mute text button text (mute / unmute)
	protected void SetMuteButtonText( bool isMuted )
	{
		TextWidget mute_text = TextWidget.Cast( layoutRoot.FindAnyWidget( "Mute" ).FindAnyWidget( "MuteText" ) );
		
		if( isMuted )
		{
			mute_text.SetText( m_UnmuteButtonTextID );
		}
		else
		{
			mute_text.SetText( m_MuteButtonTextID );
		}
		
		mute_text.Update();
	}
	
	/// Set correct bottom button texts based on platform (ps4 vs xbox texts)
	protected void LoadTextStrings()
	{
		#ifdef PLATFORM_PS4
		m_MuteButtonTextID			= "#ps4_ingame_menu_mute";
		m_UnmuteButtonTextID		= "#ps4_ingame_menu_unmute";
		m_BackButtonTextID			= "#ps4_ingame_menu_back";
		m_SelectButtonTextID		= "#ps4_ingame_menu_select";
		m_OpenGameCardButtonTextID	= "#ps4_ingame_menu_opencard";
		#else
		m_MuteButtonTextID			= "#xbox_ingame_menu_mute";
		m_UnmuteButtonTextID		= "#xbox_ingame_menu_unmute";
		m_BackButtonTextID			= "#STR_rootFrame_toolbar_bg_ConsoleToolbar_Back_BackText0";
		m_SelectButtonTextID		= "#layout_xbox_ingame_menu_select";
		m_OpenGameCardButtonTextID	= "#layout_xbox_ingame_menu_gamecard";
		#endif	
	}
	
	/// Initial texts load for the footer buttons
	protected void LoadFooterButtonTexts()
	{
		TextWidget uiGamecardText 	= TextWidget.Cast(layoutRoot.FindAnyWidget( "GamercardText" ));
		TextWidget uiBackText 		= TextWidget.Cast(layoutRoot.FindAnyWidget( "BackText" ));
		TextWidget uiSelectText 	= TextWidget.Cast(layoutRoot.FindAnyWidget( "SelectText" ));
		
		if (uiGamecardText)
		{
			uiGamecardText.SetText(m_OpenGameCardButtonTextID);
		}
		if (uiBackText)
		{
			uiBackText.SetText(m_BackButtonTextID);
		}
		if (uiSelectText)
		{
			uiSelectText.SetText(m_SelectButtonTextID);
		}
	}
	
	void UpdateMenuFocus()
	{
		Man player = GetGame().GetPlayer();
		if( m_PlayerAlive )
		{
			if( player.IsUnconscious() )
			{
				SetFocus( m_RestartButton );
			}
			else
			{
				SetFocus( m_ContinueButton );
			}
		}
		else
		{
			SetFocus( m_RestartDeadButton );
		}
	}
	
	protected void UpdateControlsElements()
	{
		RichTextWidget toolbar_a	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("SelectIcon"));
		RichTextWidget toolbar_b	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		RichTextWidget toolbar_b2	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon0"));
		RichTextWidget toolbar_x	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("MuteIcon"));
		RichTextWidget toolbar_y	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("GamercardIcon"));
		toolbar_a.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		toolbar_b2.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER));
		toolbar_x.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlX", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		toolbar_y.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlY", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
	}
//#endif
}
