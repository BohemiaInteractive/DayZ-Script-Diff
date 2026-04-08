class InGameMenu extends UIScriptedMenu
{
	string 						m_ServerInfoText;
	
	protected Widget			m_ContinueButton;
	protected Widget			m_SeparatorPanel;
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
	protected Widget			m_FeedbackButton;
	
	protected ref TextWidget	m_ModdedWarning;
	protected ref TextWidget 	m_ServerIP;
	protected ref TextWidget 	m_ServerPort;
	protected ref TextWidget 	m_ServerName;
	
	protected ref UiHintPanel m_HintPanel;
	
	void ~InGameMenu()
	{
		HudShow(true);
		
		Mission mission = g_Game.GetMission();
		if (mission)
			mission.Continue();
	}

	override Widget Init()
	{
		layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/day_z_ingamemenu.layout");
				
		m_ContinueButton			= layoutRoot.FindAnyWidget("continuebtn");
		m_SeparatorPanel			= layoutRoot.FindAnyWidget("separator_red");
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
		m_FavoriteImage 			= layoutRoot.FindAnyWidget("favorite_image");
		m_UnfavoriteImage 			= layoutRoot.FindAnyWidget("unfavorite_image");
		m_CopyInfoButton 			= layoutRoot.FindAnyWidget("copy_button");
		m_FeedbackButton			= layoutRoot.FindAnyWidget("feedbackbtn");
		
		if (g_Game.IsMultiplayer())
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
		
		Mission mission = g_Game.GetMission();
		if (mission)
			mission.Pause();
		
		return layoutRoot;
	}
	
	protected void SetGameVersion()
	{
		TextWidget version_widget = TextWidget.Cast(layoutRoot.FindAnyWidget("version"));
		string version;
		g_Game.GetVersion(version);
		version_widget.SetText("#main_menu_version" + " " + version);

		#ifdef PREVIEW_BUILD
		version_widget.SetText("THIS IS PREVIEW");
		#endif
	}
	
	protected bool SetServerInfo()
	{
		if (g_Game.IsMultiplayer())
		{
			MenuData menu_data = g_Game.GetMenuData();
			GetServersResultRow info = OnlineServices.GetCurrentServerInfo();
			
			if (info)
			{
				m_ServerPort.SetText(info.m_HostPort.ToString());
				m_ServerIP.SetText(info.m_HostIp);
				m_ServerName.SetText(info.m_Name);
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
				m_ServerPort.SetText(port.ToString());
				m_ServerIP.SetText(address);
				m_ServerName.SetText(name);
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
		Mission mission = g_Game.GetMission();
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
			#ifdef DEVELOPER
			if (g_Game.IsMultiplayer() || (g_Game.GetPlayer() && g_Game.GetPlayer().IsUnconscious()))
				OnClick_Restart();
			else
			{
				PluginDeveloper plugin = PluginDeveloper.GetInstance();
				if (plugin)
					plugin.ToggleMissionLoader();
			}
			#else
			OnClick_Restart();
			#endif
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
		else if (w == m_CopyInfoButton)
		{
			g_Game.CopyToClipboard(m_ServerInfoText);
		}
		else if (w == m_FeedbackButton)
		{
			OpenFeedback();
		}

		return false;
	}
	
	protected void OnClick_Continue()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GUI).Call(g_Game.GetMission().Continue);
	}
	
	protected void OnClick_Restart()
	{
		if (!g_Game.IsMultiplayer())
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GUI).Call(g_Game.RestartMission);
		}
		else
		{
			OnClick_Respawn();
		}
	}
	
	protected void OnClick_Respawn()
	{
		Man player = g_Game.GetPlayer();
		
		if (player && player.IsUnconscious() && !player.IsDamageDestroyed())
		{
			g_Game.GetUIManager().ShowDialog("#main_menu_respawn", "#main_menu_respawn_question", IDC_INT_RETRY, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
		}
		else
		{
			if (g_Game.GetMission().GetRespawnModeClient() == GameConstants.RESPAWN_MODE_CUSTOM)
			{
				g_Game.GetCallQueue(CALL_CATEGORY_GUI).Call(g_Game.GetUIManager().EnterScriptedMenu,MENU_RESPAWN_DIALOGUE,this);
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
		g_Game.LogoutRequestTime();
		g_Game.GetCallQueue(CALL_CATEGORY_GUI).Call(g_Game.GetMission().CreateLogoutMenu, this);
	}
	
	override bool OnModalResult(Widget w, int x, int y, int code, int result)
	{
		super.OnModalResult(w, x, y, code, result);
		if (code == IDC_INT_EXIT && result == DBB_YES)
		{
			if (g_Game.IsMultiplayer())
			{
				g_Game.LogoutRequestTime();
				g_Game.GetCallQueue(CALL_CATEGORY_GUI).Call(g_Game.GetMission().CreateLogoutMenu, this);
			}
			else
			{
				// skip logout screen in singleplayer
				g_Game.GetMission().AbortMission();
			}	
			g_Game.CancelLoginTimeCountdown();
			return true;
		}
		else if (code == IDC_INT_EXIT && result == DBB_NO)
		{
			g_Game.CancelLoginTimeCountdown();
		}
		else if (code == IDC_INT_RETRY && result == DBB_YES && g_Game.IsMultiplayer())
		{
			PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
			//! We dont want players to see the respawn dialog when they are still alive and not unconscious anymore (edge case).
			if (player && player.IsAlive() && !player.IsUnconscious())
			{
				return true;
			}
			
			if (g_Game.GetMission().GetRespawnModeClient() == GameConstants.RESPAWN_MODE_CUSTOM)
			{
				g_Game.GetCallQueue(CALL_CATEGORY_GUI).Call(g_Game.GetUIManager().EnterScriptedMenu,MENU_RESPAWN_DIALOGUE,this);
			} 
			else
			{
				GameRespawn(true);
			}
			return true;
		}
		
		return false;
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		UpdateGUI();
	}
	
	protected void UpdateGUI()
	{
		#ifdef BULDOZER
		m_RestartButton.Show(false);
		m_RespawnButton.Show(false);
		#else
		Man player = g_Game.GetPlayer();
		bool playerAlive = player && player.GetPlayerState() == EPlayerStates.ALIVE;

		if (g_Game.IsMultiplayer())
		{
			m_RestartButton.Show(playerAlive && player.IsUnconscious() && !CfgGameplayHandler.GetDisableRespawnInUnconsciousness());
			m_RespawnButton.Show(!playerAlive);
		}
		else
		{
			m_RestartButton.Show(true);
			m_RespawnButton.Show(false);
			m_SeparatorPanel.Show(playerAlive);
		}

		m_ContinueButton.Show(playerAlive);
		#endif
	}
	
	void MenuRequestRespawn(UIScriptedMenu menu, bool random)
	{
		if (RespawnDialogue.Cast(menu))
			GameRespawn(random);
	}
	
	protected void GameRespawn(bool random)
	{
		g_Game.GetMenuDefaultCharacterData(false).SetRandomCharacterForced(random);
		g_Game.RespawnPlayer();
		
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player)
		{
			player.SimulateDeath(true);
			g_Game.GetCallQueue(CALL_CATEGORY_GUI).Call(player.ShowDeadScreen, true, 0);
		}
		
		MissionGameplay missionGP = MissionGameplay.Cast(g_Game.GetMission());
		missionGP.DestroyAllMenus();
		missionGP.SetPlayerRespawning(true);
		missionGP.Continue();

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
			label.SetText(text);
		
	}

	protected void ButtonSetColor(Widget w, int color)
	{
		Widget panel = w.FindWidget(w.GetName() + "_panel");
		if (panel)
			panel.SetColor(color);
	}
	
	protected void ButtonSetTextColor(Widget w, int color)
	{
		TextWidget label = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		if (label)
			label.SetColor(color);
	}
	
	void SetServerInfoVisibility(bool show)
	{
		m_ServerInfoPanel.Show(show);
	}
	
	protected void OpenFeedback()
	{
		g_Game.OpenURL("https://feedback.bistudio.com/project/view/2/");
	}
	
	//! DEPRECATED
	void ToggleFavoriteServer();
}
