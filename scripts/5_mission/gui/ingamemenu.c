class InGameMenu extends UIScriptedMenu
{
	string 						m_ServerInfoText;
	
	protected Widget			m_ContinueButton;
	protected Widget			m_ExitButton;
	protected Widget			m_RestartButton;
	protected Widget 			m_RespawnButton;
	protected Widget			m_RestartDeadRandomButton;
	protected Widget			m_RestartDeadCustomButton;
	protected Widget			m_OptionsButton;
	protected Widget 			m_ServerInfoPanel;
	protected Widget 			m_FavoriteButton;
	protected Widget 			m_FavoriteImage;
	protected Widget 			m_UnfavoriteImage;
	protected Widget 			m_CopyInfoButton;
	
	protected ref TextWidget	m_ModdedWarning;
	protected ref TextWidget 	m_ServerIP;
	protected ref TextWidget 	m_ServerPort;
	protected ref TextWidget 	m_ServerName;
	
	protected ref UiHintPanel m_HintPanel;
	
	void ~InGameMenu()
	{
		HudShow(true);
	}

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_ingamemenu.layout");
				
		m_ContinueButton			= layoutRoot.FindAnyWidget("continuebtn");
		m_ExitButton				= layoutRoot.FindAnyWidget("exitbtn");
		m_RestartButton				= layoutRoot.FindAnyWidget("restartbtn");
		m_RespawnButton 			= layoutRoot.FindAnyWidget("respawn_button");
		m_RestartDeadRandomButton	= layoutRoot.FindAnyWidget("respawn_button_random");
		m_RestartDeadCustomButton	= layoutRoot.FindAnyWidget("respawn_button_custom");
		m_OptionsButton				= layoutRoot.FindAnyWidget("optionsbtn");
		m_ModdedWarning				= TextWidget.Cast(layoutRoot.FindAnyWidget("ModdedWarning"));
		m_HintPanel					= new UiHintPanel(layoutRoot.FindAnyWidget("hint_frame"));
		m_ServerInfoPanel 			= layoutRoot.FindAnyWidget("server_info");
		m_ServerIP 					= TextWidget.Cast(layoutRoot.FindAnyWidget("server_ip"));
		m_ServerPort 				= TextWidget.Cast(layoutRoot.FindAnyWidget("server_port"));
		m_ServerName 				= TextWidget.Cast(layoutRoot.FindAnyWidget("server_name"));
		//m_FavoriteButton 			= layoutRoot.FindAnyWidget("favorite_button");
		m_FavoriteImage 			= layoutRoot.FindAnyWidget("favorite_image");
		m_UnfavoriteImage 			= layoutRoot.FindAnyWidget("unfavorite_image");
		m_CopyInfoButton 			= layoutRoot.FindAnyWidget("copy_button");
		
		if (GetGame().IsMultiplayer())
		{
			ButtonSetText(m_RestartButton, "#main_menu_respawn");
		}
		else
		{
			ButtonSetText(m_RestartButton, "#main_menu_restart");
		}		
		
		HudShow(false);
		SetGameVersion();
		SetServerInfoVisibility(SetServerInfo() && g_Game.GetProfileOption(EDayZProfilesOptions.SERVERINFO_DISPLAY));
		m_ModdedWarning.Show(g_Game.ReportModded());
		
		return layoutRoot;
	}
	
	protected void SetGameVersion()
	{
		TextWidget version_widget = TextWidget.Cast(layoutRoot.FindAnyWidget("version"));
		string version;
		GetGame().GetVersion(version);
		version_widget.SetText("#main_menu_version" + " " + version);

		#ifdef PREVIEW_BUILD
			version_widget.SetText("THIS IS PREVIEW");
		#endif
	}
	
	protected bool SetServerInfo()
	{
		if (GetGame().IsMultiplayer())
		{
			//PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			MenuData menu_data = g_Game.GetMenuData();
			GetServersResultRow info = OnlineServices.GetCurrentServerInfo();
			
			if (info)
			{
				//text
				m_ServerPort.SetText(info.m_HostPort.ToString());
				m_ServerIP.SetText(info.m_HostIp);
				m_ServerName.SetText(info.m_Name);
				//favorite
				m_UnfavoriteImage.Show(info.m_Favorite);
				m_FavoriteImage.Show(!info.m_Favorite);
				m_ServerInfoText = "" +  info.GetIpPort();
				
				return true;
			}
			//temporary, incomplete solution, OnlineServices.GetCurrentServerInfo() should be working!
			else if (menu_data && menu_data.GetLastPlayedCharacter() != GameConstants.DEFAULT_CHARACTER_MENU_ID)
			{
				int char_id = menu_data.GetLastPlayedCharacter();
				int port;
				string address,name;
				
				menu_data.GetLastServerAddress(char_id,address);
				port = menu_data.GetLastServerPort(char_id);
				menu_data.GetLastServerName(char_id,name);
				//text
				m_ServerPort.SetText(port.ToString());
				m_ServerIP.SetText(address);
				m_ServerName.SetText(name);
				//favorite
				//m_FavoriteButton.Show(false); // buton should be non-functional in this case!
				m_ServerInfoText = "" + address + ":" + port;
				
				return true;
			}
			else
			{
				g_Game.RefreshCurrentServerInfo();
			}
		}
		return false;
	}
	
	protected void HudShow(bool show)
	{
		Mission mission = GetGame().GetMission();
		if (mission)
		{
			IngameHud hud = IngameHud.Cast(mission.GetHud());
			if (hud)
			{
				hud.ShowHudUI(g_Game.GetProfileOption(EDayZProfilesOptions.HUD) && show);
				hud.ShowQuickbarUI(g_Game.GetProfileOption(EDayZProfilesOptions.QUICKBAR) && show);
			}
		}
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		ColorHighlight(w);
		return true;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		ColorNormal(w);
		return true;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w == m_ContinueButton)
		{
			OnClick_Continue();
			return true;
		}
		else if (w == m_RestartButton)
		{
			OnClick_Restart();
			return true;
		}
		else if (w == m_RespawnButton)
		{
			OnClick_Respawn();
			return true;
		}
		else if (w == m_OptionsButton)
		{
			OnClick_Options();
			return true;
		}
		else if (w == m_ExitButton)
		{
			OnClick_Exit();
			return true;
		}
		/*else if (w == m_FavoriteButton)
		{
			ToggleFavoriteServer();
		}*/
		else if (w == m_CopyInfoButton)
		{
			GetGame().CopyToClipboard(m_ServerInfoText);
		}

		return false;
	}
	
	protected void OnClick_Continue()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetMission().Continue);
	}
	
	protected void OnClick_Restart()
	{
		if (!GetGame().IsMultiplayer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().RestartMission);
		}
		else
		{
			OnClick_Respawn();
		}
	}
	
	protected void OnClick_Respawn()
	{
		Man player = GetGame().GetPlayer();
		
		if (player && player.IsUnconscious())
		{
			GetGame().GetUIManager().ShowDialog("#main_menu_respawn", "#main_menu_respawn_question", IDC_INT_RETRY, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
		}
		else
		{
			if (GetGame().GetMission().GetRespawnModeClient() == GameConstants.RESPAWN_MODE_CUSTOM)
			{
				//GetGame().GetUIManager().ShowDialog("Respawning", "Respawn as custom?", IDC_INT_RESPAWN, DBT_YESNOCANCEL, DBB_YES, DMT_QUESTION, this);
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetUIManager().EnterScriptedMenu,MENU_RESPAWN_DIALOGUE,this);
			}
			else
			{
				GameRespawn(true);
			}
		}
	}
	
	protected void OnClick_Options()
	{
		EnterScriptedMenu(MENU_OPTIONS);
	}
	
	protected void OnClick_Exit()
	{
		GetGame().LogoutRequestTime();
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetMission().CreateLogoutMenu, this);
		
		//GetGame().GetUIManager().ShowDialog("EXIT", "Are you sure you want to exit?", IDC_INT_EXIT, DBT_YESNO, DBB_YES, DMT_QUESTION, NULL);
	}
	
	override bool OnModalResult(Widget w, int x, int y, int code, int result)
	{
		super.OnModalResult(w, x, y, code, result);
		if (code == IDC_INT_EXIT && result == DBB_YES)
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
		else if (code == IDC_INT_EXIT && result == DBB_NO)
		{
			g_Game.CancelLoginTimeCountdown();
		}
		else if (code == IDC_INT_RETRY && result == DBB_YES && GetGame().IsMultiplayer())
		{
			if (GetGame().GetMission().GetRespawnModeClient() == GameConstants.RESPAWN_MODE_CUSTOM)
			{
				//GetGame().GetUIManager().ShowDialog("#main_menu_respawn", "#main_menu_respawn_question", IDC_INT_RESPAWN, DBT_YESNOCANCEL, DBB_YES, DMT_QUESTION, this);	
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetUIManager().EnterScriptedMenu,MENU_RESPAWN_DIALOGUE,this);
			} 
			else
			{
				GameRespawn(true);
			}
			return true;
		}
		/*else if (code == IDC_INT_RESPAWN && GetGame().IsMultiplayer())
		{
			if (result == DBB_YES)
			{
				GameRespawn(false);
				return true;
			}
			else if (result == DBB_NO)
			{
				GameRespawn(true);
				return true;
			}
		}*/
		
		return false;
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		UpdateGUI();
	}
	
	protected void UpdateGUI()
	{
		Man player = GetGame().GetPlayer();
		bool player_is_alive = false;

		if (player)
		{
			int life_state = player.GetPlayerState();

			if (life_state == EPlayerStates.ALIVE)
			{
				player_is_alive = true;
			}
		}
		
		#ifdef BULDOZER
		m_RestartButton.Show(false);
		m_RespawnButton.Show(false);
		#else
		if (GetGame().IsMultiplayer())
		{
			m_RestartButton.Show(player_is_alive && player.IsUnconscious());
			m_RespawnButton.Show(!player_is_alive);
			//m_RestartDeadCustomButton.Show(!player_is_alive && GetGame().GetMission().GetRespawnModeClient() == GameConstants.RESPAWN_MODE_CUSTOM);
			//m_RestartDeadRandomButton.Show(!player_is_alive);
		}
		else
		{
			m_RestartButton.Show(true);
			m_RespawnButton.Show(false);
			//m_RestartDeadCustomButton.Show(false);
			//m_RestartDeadRandomButton.Show(false);
		}
		m_ContinueButton.Show(player_is_alive);
		#endif
	}
	
	void MenuRequestRespawn(UIScriptedMenu menu, bool random)
	{
		if (RespawnDialogue.Cast(menu))
			GameRespawn(random);
	}
	
	protected void GameRespawn(bool random)
	{
		GetGame().GetMenuDefaultCharacterData(false).SetRandomCharacterForced(random);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().RespawnPlayer);
		//turns off dead screen, hides HUD for countdown
		//---------------------------------------------------
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(player.ShowDeadScreen, DayZPlayerImplement.DEAD_SCREEN_DELAY, false, false, 0);
		}
		
		MissionGameplay missionGP = MissionGameplay.Cast(GetGame().GetMission());
		missionGP.DestroyAllMenus();
		missionGP.SetPlayerRespawning(true);
		//---------------------------------------------------
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetMission().Continue);
		Close();
	}
	
	protected void ColorHighlight(Widget w)
	{
		if (!w)
			return;
		
		ButtonSetColor(w, ARGB(255, 0, 0, 0));
		ButtonSetTextColor(w, ARGB(255, 255, 0, 0));
	}
	
	protected void ColorNormal(Widget w)
	{
		if (!w)
			return;
		
		ButtonSetColor(w, ARGB(0, 0, 0, 0));
		ButtonSetTextColor(w, ARGB(255, 255, 255, 255));
	}
	
	protected void ColorDisable(Widget w)
	{
		if (!w)
			return;
		
		ButtonSetColor(w, ARGB(0, 0, 0, 0));
		ButtonSetTextColor(w, ColorManager.COLOR_DISABLED_TEXT);
	}
	
	protected void ButtonSetText(Widget w, string text)
	{
		if (!w)
			return;
				
		TextWidget label = TextWidget.Cast(w.FindWidget(w.GetName() + "_label"));
		
		if (label)
		{
			label.SetText(text);
		}
		
	}
	
	protected void ButtonSetColor(Widget w, int color)
	{
		if (!w)
			return;
		
		Widget panel = w.FindWidget(w.GetName() + "_panel");
		
		if (panel)
		{
			panel.SetColor(color);
		}
	}
	
	protected void ButtonSetTextColor(Widget w, int color)
	{
		if (!w)
			return;

		TextWidget label	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
				
		if (label)
		{
			label.SetColor(color);
		}
	}
	
	void SetServerInfoVisibility(bool show)
	{
		m_ServerInfoPanel.Show(show);
	}
	
	void ToggleFavoriteServer()
	{
		//TODO insert favorite mechanism here
		GetServersResultRow info = OnlineServices.GetCurrentServerInfo();
		bool favorite = !info.m_Favorite;
		
		OnlineServices.SetServerFavorited(info.GetIP(), info.m_HostPort, info.m_SteamQueryPort, favorite);		
		m_UnfavoriteImage.Show(!favorite);
		m_FavoriteImage.Show(favorite);
	}
}
