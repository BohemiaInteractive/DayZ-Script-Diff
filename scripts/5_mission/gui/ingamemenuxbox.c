class InGameMenuXbox extends UIScriptedMenu
{
	// Widgets texts id
	protected  string 						m_MuteButtonTextID;
	protected  string 						m_UnmuteButtonTextID;
	protected  string 						m_BackButtonTextID;
	protected  string 						m_SelectButtonTextID;
	protected  string 						m_OpenGameCardButtonTextID;
	protected  string 						m_CurrentMuteButtonText;
	
	protected bool 							m_SelectAvailable;
	protected bool 							m_MuteAvailable;
	protected bool 							m_BackAvailable;
	protected bool 							m_GamercardAvailable;
	
	protected bool 							m_PlayerAlive;
	
	protected ref PlayerListScriptedWidget	m_ServerInfoPanel;
	
	protected Widget						m_OnlineMenu;
	
	protected Widget						m_IngameMenuPanel;
	protected Widget						m_DialogPanel;
	protected ButtonWidget					m_ContinueButton;
	protected ButtonWidget					m_ExitButton;
	protected ButtonWidget					m_RestartDeadButton;
	protected ButtonWidget					m_RestartButton;
	protected ButtonWidget					m_OptionsButton;
	protected ButtonWidget					m_InviteButton;
	protected ButtonWidget					m_ControlsButton;
	protected ButtonWidget					m_OnlineButton;
	protected ButtonWidget					m_TutorialsButton;	
	protected TextWidget					m_Version;

	protected ButtonWidget					m_ShowFeedback;
	protected ImageWidget					m_FeedbackQRCode;
	protected ImageWidget					m_FeedbackPlatformIcon;
	protected ButtonWidget					m_FeedbackClose;
	protected RichTextWidget				m_FeedbackCloseLabel;
	
	const int 								BUTTON_XBOX_CONTROLS = 201;
	
	void InGameMenuXbox()
	{
		GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
	}

	void ~InGameMenuXbox()
	{
		ClientData.SyncEvent_OnPlayerListUpdate.Remove(SyncEvent_OnRecievedPlayerList);
		OnlineServices.m_PermissionsAsyncInvoker.Remove(OnPermissionsUpdate);
		
		Mission mission = GetGame().GetMission();
		if (mission)
		{
			IngameHud hud = IngameHud.Cast(mission.GetHud());
			if (hud)
			{
				hud.ShowHudUI(true);
				hud.ShowQuickbarUI(true);
			}
			
			mission.Continue();
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
		bool mk = GetGame().GetInput().IsEnabledMouseAndKeyboard();
		bool mkServer = GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer();
		
		switch (pInputDeviceType)
		{
			case EInputDeviceType.CONTROLLER:
			{
				if (mk && mkServer)
				{
					GetGame().GetUIManager().ShowUICursor(false);
					if (!GetFocus() || GetFocus() == m_FeedbackClose)
					{
						UpdateMenuFocus();
					}
	
					m_FeedbackClose.Show(false);
					m_ShowFeedback.Show(false);
				}
				break;
			}
			default:
			{
				if (mk && mkServer)
				{
					GetGame().GetUIManager().ShowUICursor(true);
					m_ShowFeedback.Show(true);
					m_FeedbackClose.Show(true);
					m_FeedbackCloseLabel.SetText(string.Format("%1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#close", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_NORMAL)));
				}
				break;
			}
		}
		
		UpdateControlsElements();
	}
	
	override Widget Init()
	{
		layoutRoot		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/xbox/day_z_ingamemenu_xbox.layout");
		
		m_OnlineMenu	= GetGame().GetWorkspace().CreateWidgets("gui/layouts/xbox/ingamemenu_xbox/online_info_menu.layout", layoutRoot);
		m_OnlineMenu.Show(false);
		
		m_IngameMenuPanel 		= layoutRoot.FindAnyWidget("ingame_menu_panel");
		m_ContinueButton		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("continuebtn"));
		m_RestartDeadButton		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("restartbtn_dead"));
		m_ExitButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("exitbtn"));
		m_RestartButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("restartbtn"));
		m_OptionsButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("optionsbtn"));
		m_InviteButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("invitebtn"));
		m_ControlsButton		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("controlsbtn"));
		m_OnlineButton			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("onlinebtn"));
		m_TutorialsButton		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("tutorialsbtn"));
		m_Version				= TextWidget.Cast(layoutRoot.FindAnyWidget("version"));
		m_ShowFeedback 			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("feedbackbtn"));
		m_FeedbackQRCode 		= ImageWidget.Cast(layoutRoot.FindAnyWidget("qr_image"));
		m_FeedbackClose 		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("close_button"));
		m_FeedbackCloseLabel 	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("close_button_label"));
		m_DialogPanel 			= layoutRoot.FindAnyWidget("ingame_menu_dialog");
		
		m_SelectAvailable = true;
		m_MuteAvailable = false;
		m_GamercardAvailable = false;
		m_BackAvailable = true;
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			m_PlayerAlive = player.GetPlayerState() == EPlayerStates.ALIVE;
			if (!m_PlayerAlive)
				m_BackAvailable = false;
			
			player.GetOnUnconsciousStart().Insert(UpdateGUI);
			player.GetOnUnconsciousStart().Insert(UpdateMenuFocus);
			player.GetOnUnconsciousStop().Insert(UpdateGUI);
			player.GetOnUnconsciousStop().Insert(UpdateMenuFocus);
			player.GetOnDeathStart().Insert(UpdateGUI);
			player.GetOnDeathStart().Insert(UpdateMenuFocus);
		}
		
		string version;
		GetGame().GetVersion(version);
		#ifdef PLATFORM_CONSOLE
		version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		#else
		version = "#main_menu_version" + " " + version;
		#endif
		m_Version.SetText(version);
		
		if (GetGame().IsMultiplayer())
		{
			m_OnlineButton.Show(true);
			
			TextWidget w_text = TextWidget.Cast(m_OnlineMenu.FindAnyWidget("OnlineTextWidget"));
			w_text.SetText(g_Game.GetHostName());
			
			m_ServerInfoPanel = new PlayerListScriptedWidget(m_OnlineMenu.FindAnyWidget("ServerInfoPanel"));
			
			OnlineServices.m_PermissionsAsyncInvoker.Insert(OnPermissionsUpdate);
			ClientData.SyncEvent_OnPlayerListUpdate.Insert(SyncEvent_OnRecievedPlayerList);
			
			m_ServerInfoPanel.Reload(ClientData.m_PlayerList);
			m_ServerInfoPanel.ReloadLocal(OnlineServices.GetMuteList());
			
			string uid = m_ServerInfoPanel.FindPlayerByWidget(GetFocus());
			if (uid != "")
			{
				if (IsLocalPlayer(uid) || m_ServerInfoPanel.IsEmpty())
				{
					m_MuteAvailable = false;
					m_GamercardAvailable = false;
				}
				else
				{
					m_MuteAvailable = !GetGame().GetWorld().IsDisabledReceivingVoN();
					#ifndef PLATFORM_PS4
					m_GamercardAvailable = true;
					#endif
					SetMuteButtonText(OnlineServices.IsPlayerMuted(uid));
				}
				
				if (m_ServerInfoPanel.IsGloballyMuted(uid))
				{
					m_MuteAvailable = false;
				}
			}
		}
		else
		{
			layoutRoot.FindAnyWidget("onlinebtn").Show(false);
			layoutRoot.FindAnyWidget("invitebtn").Show(false);
		}
		
		//RESPAWN & RESTART
		if (GetGame().IsMultiplayer())
		{
			m_RestartButton.SetText("#main_menu_respawn");
		}
		else
		{
			m_RestartButton.SetText("#main_menu_restart");
		}
		
		if (!ShouldRestartBeVisible(player))
		{
			m_RestartButton.Show(false);
		}
		
		Mission mission = GetGame().GetMission();
		if (mission)
		{
			IngameHud hud = IngameHud.Cast(mission.GetHud());
			if (hud)
			{
				hud.ShowHudUI(false);
				hud.ShowQuickbarUI(false);
			}
			
			mission.Pause();
		}
		
		PPERequester_MenuEffects requester;
		Class.CastTo(requester,PPERequesterBank.GetRequester(PPERequester_MenuEffects));
		requester.SetVignetteIntensity(0.6);

		LoadTextStrings();
		LoadFooterButtonTexts();
		
		#ifdef PLATFORM_PS4
		m_FeedbackQRCode.LoadImageFile(0, "gui/textures/feedback_qr_ps.edds");
		#else
		m_FeedbackQRCode.LoadImageFile(0, "gui/textures/feedback_qr_xbox.edds");
		#endif
		
		return layoutRoot;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (!FeedbackDialogVisible())
		{
			switch (w.GetUserID())
			{
				case IDC_MAIN_CONTINUE:
				{
					if (!IsOnlineOpen())
					{
						GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetMission().Continue);
					}
					else
					{
						CloseOnline();
					}
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
					if (!GetGame().IsMultiplayer())
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
					m_OnlineMenu.Show(true);
					m_SelectAvailable = false;
					
					UpdateControlsElements();
					m_ServerInfoPanel.FocusFirst();
					return true;
				}
				case IDC_MAIN_TUTORIAL:
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
			
			if (w == m_RestartDeadButton)
			{
				if (GetGame().GetMission().GetRespawnModeClient() == GameConstants.RESPAWN_MODE_CUSTOM)
				{
					GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetUIManager().EnterScriptedMenu,MENU_RESPAWN_DIALOGUE,this);
				}
				else
				{
					GameRetry(true);
				}
				return true;
			}
		}
		else
		{
			if (w == m_FeedbackClose)
			{
				ToggleFeedbackDialog();
				return true;
			}
		}
		
		if (w.GetUserID() == IDC_MAIN_FEEDBACK && !IsOnlineOpen())
		{
			ToggleFeedbackDialog();
			return true;
		}

		return false;
	}
	
	protected void ToggleFeedbackDialog()
	{
		bool dialogVisible = FeedbackDialogVisible();
		m_DialogPanel.Show(!dialogVisible);
		m_IngameMenuPanel.Show(dialogVisible);
		
		if (!dialogVisible)
		{
			PPERequesterBank.GetRequester(PPERequester_FeedbackBlur).Start();
		}
		else
		{
			PPERequesterBank.GetRequester(PPERequester_FeedbackBlur).Stop();
		}
		
		UpdateMenuFocus();
		UpdateControlsElements();
	}
	
	bool FeedbackDialogVisible()
	{
		return m_DialogPanel.IsVisible();
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
		else if (code == IDC_INT_RETRY)
		{
			if (result == DBB_YES)
			{
				if (GetGame().GetMission().GetRespawnModeClient() == GameConstants.RESPAWN_MODE_CUSTOM)
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
		if (GetGame().IsMultiplayer())
		{
			GetGame().GetMenuDefaultCharacterData(false).SetRandomCharacterForced(random);
			GetGame().RespawnPlayer();

			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if (player)
			{
				player.SimulateDeath(true);
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(player.ShowDeadScreen, true, 0);
			}
			
			MissionGameplay missionGP = MissionGameplay.Cast(GetGame().GetMission());
			missionGP.DestroyAllMenus();
			missionGP.SetPlayerRespawning(true);
			missionGP.Continue();

			Close();
		}
		else
		{
			GetGame().RestartMission();
		}
	}
	
	void MenuRequestRespawn(UIScriptedMenu menu, bool random)
	{
		if (RespawnDialogue.Cast(menu))
		{
			GameRetry(random);
		}
	}
	
	bool IsLocalPlayer(string uid)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		string local_uid;
		if (GetGame().GetUserManager())
		{
			local_uid = GetGame().GetUserManager().GetSelectedUser().GetUid();
		}

		return (uid == local_uid);
	}
	
	SyncPlayerList CreateFakePlayerList(int player_count)
	{
		SyncPlayerList player_list = new SyncPlayerList;
		player_list.m_PlayerList = new array<ref SyncPlayer>;
		
		for (int i = 0; i < player_count; i++)
		{
			SyncPlayer sync_player = new SyncPlayer;
			sync_player.m_UID = "uid" + i;
			sync_player.m_PlayerName = "Player " + i;
			player_list.m_PlayerList.Insert(sync_player);
		}

		return player_list;
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		if (GetUApi().GetInputByID(UAUIThumbRight).LocalPress() && !IsOnlineOpen() || FeedbackDialogVisible() && GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			ToggleFeedbackDialog();
		}
		
		if (GetGame().IsMultiplayer() && layoutRoot.FindAnyWidget("OnlineInfo").IsVisible())
		{
			PlayerListEntryScriptedWidget selected;
			if (m_ServerInfoPanel)
			{
				selected = m_ServerInfoPanel.GetSelectedPlayer();
			}

			if (GetUApi().GetInputByID(UAUICtrlX).LocalPress())
			{
				if (selected)
				{
					m_ServerInfoPanel.ToggleMute(selected.GetUID());
				}

				Refresh();
			}
			
			#ifndef PLATFORM_PS4
			if (GetUApi().GetInputByID(UAUICtrlY).LocalPress())
			{
				if (selected)
				{
					OnlineServices.ShowUserProfile(selected.GetUID());
				}
			}
			#endif
		}
	}
	
	bool ShouldRestartBeVisible(Man player)
	{
		PlayerBase p = PlayerBase.Cast(player);
		return p && p.IsUnconsciousStateOnly() && !CfgGameplayHandler.GetDisableRespawnInUnconsciousness();
	}

	void UpdateGUI()
	{
		bool shouldUpdateMenuFocus = false;
		Man player = GetGame().GetPlayer();
		if (player)
		{
			m_PlayerAlive = player.GetPlayerState() == EPlayerStates.ALIVE;
		}
		
		if (m_PlayerAlive)
		{
			bool showRestartBtn = ShouldRestartBeVisible(player);
			m_RestartButton.Show(showRestartBtn);
		}
		else
		{
			m_RestartButton.Show(false);
			m_BackAvailable = false;
		}
		
		m_ContinueButton.Show(m_PlayerAlive);
		m_RestartDeadButton.Show(!m_PlayerAlive);
		
		UpdateMenuFocus();
	}
	
	bool IsOnlineOpen()
	{
		return m_OnlineMenu.IsVisible();
	}

	void CloseOnline()
	{
		m_OnlineMenu.Show(false);
		m_SelectAvailable = true;
		m_MuteAvailable = false;
		m_GamercardAvailable = false;

		UpdateControlsElements();
		SetFocus(m_OnlineButton);
	}
	
	void SelectServer()
	{
		if (m_ServerInfoPanel)
		{
			m_ServerInfoPanel.FocusFirst();
			
			Refresh();
		}
	}
	
	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		return false;
	}
	
	void SyncEvent_OnRecievedPlayerList(SyncPlayerList player_list)
	{
		m_ServerInfoPanel.Reload(player_list);
		
		Refresh();
	}
	
	void OnPermissionsUpdate(BiosPrivacyUidResultArray result_list)
	{
		m_ServerInfoPanel.Reload(result_list);
		
		Refresh();
	}
	
	override void OnShow()
	{
		super.OnShow();

		#ifdef PLATFORM_CONSOLE
		bool mk = GetGame().GetInput().IsEnabledMouseAndKeyboard();
		bool mk_server = GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer();
		TextWidget warning = TextWidget.Cast(layoutRoot.FindAnyWidget("MouseAndKeyboardWarning"));
		if (mk)
		{
			if (mk_server)
			{
				warning.SetText("#str_mouse_and_keyboard_server_warning");
			}
			else
			{
				warning.SetText("#str_controller_server_warning");
			}
		}
	
		warning.Show(mk);
		#endif
		
		UpdateGUI();
		
		UpdateMenuFocus();
		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (IsFocusable(w))
		{
			ColorHighlight(w);
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (IsFocusable(w))
		{
			ColorNormal(w);
			return true;
		}
		return false;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		if (IsFocusable(w))
		{
			ColorHighlight(w);
			return true;
		}
		return false;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (IsFocusable(w))
		{
			ColorNormal(w);
			return true;
		}
		return false;
	}
	
	bool IsFocusable(Widget w)
	{
		if (w)
		{
			if (w == m_ContinueButton || w == m_RestartDeadButton || w == m_ExitButton || w == m_RestartButton || w == m_OptionsButton || w == m_InviteButton || w == m_ControlsButton || w == m_OnlineButton || w == m_TutorialsButton || w == m_ShowFeedback)
			{
				return true;
			}
		}
		return false;
	}
	
	override void Refresh()
	{
		string version;
		GetGame().GetVersion(version);
		#ifdef PLATFORM_CONSOLE
		version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		#else
		version = "#main_menu_version" + " " + version;
		#endif
		m_Version.SetText(version);
		
		m_MuteAvailable = false;
		m_GamercardAvailable = false;
		
		if (GetGame().IsMultiplayer() && layoutRoot.FindAnyWidget("OnlineInfo").IsVisible() && m_ServerInfoPanel)
		{
			PlayerListEntryScriptedWidget selected = m_ServerInfoPanel.GetSelectedPlayer();
			if (selected && !selected.IsLocalPlayer())
			{
				m_MuteAvailable = !GetGame().GetWorld().IsDisabledReceivingVoN() && !selected.IsGloballyMuted();
				#ifndef PLATFORM_PS4
				m_GamercardAvailable = true;
				#endif							
				SetMuteButtonText(selected.IsMuted());
			}
		}
		
		UpdateControlsElements();
	}
	
	void ColorDisable(Widget w)
	{
		#ifdef PLATFORM_WINDOWS
		SetFocus(null);
		#endif
		
		ButtonWidget button = ButtonWidget.Cast(w);
		if (button && button != m_ContinueButton)
		{
			button.SetTextColor(ARGB(255, 255, 255, 255));
		}
		ButtonSetColor(w, ARGB(0, 0, 0, 0));
		ButtonSetTextColor(w,  ARGB(60, 0, 0, 0));
	}
	
	void ColorHighlight(Widget w)
	{
		if (!w)
		{
			return;
		}
				
		int color_pnl = ARGB(255, 0, 0, 0);
		int color_lbl = ARGB(255, 255, 0, 0);
		
		#ifdef PLATFORM_CONSOLE
		color_pnl = ARGB(255, 200, 0, 0);
		color_lbl = ARGB(255, 255, 255, 255);
		#endif
		
		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}
	
	void ColorNormal(Widget w)
	{
		if (!w) return;

		int color_pnl = ARGB(0, 0, 0, 0);
		int color_lbl = ARGB(255, 255, 255, 255);
		
		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}
	
	void ButtonSetText(Widget w, string text)
	{
		if (!w)
		{
			return;
		}
				
		TextWidget label = TextWidget.Cast(w.FindWidget(w.GetName() + "_label"));
		if (label)
		{
			label.SetText(text);
		}
		
	}
	
	void ButtonSetColor(Widget w, int color)
	{
		if (!w)
		{
			return;
		}
		
		Widget panel = w.FindWidget(w.GetName() + "_panel");
		if (panel)
		{
			panel.SetColor(color);
		}
	}
	
	void ButtonSetTextColor(Widget w, int color)
	{
		if (!w)
		{
			return;
		}

		TextWidget label	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		TextWidget text		= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text"));
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text_1"));
				
		if (label)
		{
			label.SetColor(color);
		}
		
		if (text)
		{
			text.SetColor(color);
		}
		
		if (text2)
		{
			text2.SetColor(color);
		}
	}
	
	/// Set mute text button text (mute / unmute)
	protected void SetMuteButtonText(bool isMuted)
	{
		if (isMuted)
		{
			m_CurrentMuteButtonText = m_UnmuteButtonTextID;
		}
		else
		{
			m_CurrentMuteButtonText = m_MuteButtonTextID;
		}
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
		TextWidget uiGamecardText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("GamercardText"));
		TextWidget uiBackText 		= TextWidget.Cast(layoutRoot.FindAnyWidget("BackText"));
		TextWidget uiSelectText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("SelectText"));
		
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
		if (m_PlayerAlive)
		{
			if (ShouldRestartBeVisible(player))
			{
				SetFocus(m_RestartButton);
			}
			else
			{
				SetFocus(m_ContinueButton);
			}
		}
		else
		{
			SetFocus(m_RestartDeadButton);
		}
	}
	
	protected void UpdateControlsElements()
	{
		bool toolbarShow = !GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer() || GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		bool onlineOpen = IsOnlineOpen();
		layoutRoot.FindAnyWidget("toolbar_bg").Show(toolbarShow);
		layoutRoot.FindAnyWidget("top").Show(!onlineOpen);
		
		//! Continue/back button handling depending on current view
		if (onlineOpen)
		{
			layoutRoot.FindAnyWidget("bottom").Show(!toolbarShow);
		}
		else
		{
			layoutRoot.FindAnyWidget("bottom").Show(true);
		}
		
		RichTextWidget continueBtnIcon = RichTextWidget.Cast(layoutRoot.FindAnyWidget("continuebtn_icon"));
		string contineBtnText = "";
		continueBtnIcon.Show(!toolbarShow);

		RichTextWidget toolbarTextWidget = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ContextToolbarText"));
		string toolbarText = "";
		
		//! Set toolbar icons and text if toolbar is visible (controller is used as input device)
		if (toolbarShow)
		{
			if (!FeedbackDialogVisible())
			{
				if (m_SelectAvailable)
				{
					toolbarText += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", m_SelectButtonTextID, EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
				}

				if (m_MuteAvailable)
				{
					toolbarText += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlX", m_CurrentMuteButtonText, EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
				}

				if (m_BackAvailable)
				{
					toolbarText += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", m_BackButtonTextID, EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
				}

				if (m_GamercardAvailable)
				{
					toolbarText += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlY", m_OpenGameCardButtonTextID, EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
				}

				if (!IsOnlineOpen())
				{
					toolbarText += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIThumbRight", "#layout_main_menu_feedback", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
				}
			}
			else
			{
				toolbarText = InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#close", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR);
			}
		}
		else
		{
			//! Set continue button icon if toolbar is not visible (mouse and keyboard is used as input device).
			contineBtnText = InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER);
		}
		
		toolbarTextWidget.SetText(toolbarText);
		continueBtnIcon.SetText(contineBtnText);
		
		//! Continue button label depends on current view status
		string continueBtnLabel = "#main_menu_continue";
		if (onlineOpen)
			continueBtnLabel = "#layout_character_creation_back";

		TextWidget continueBtnTextWidget = TextWidget.Cast(layoutRoot.FindAnyWidget("continuebtn_label"));
		continueBtnTextWidget.SetText(continueBtnLabel);
		layoutRoot.FindAnyWidget("dayz_logo").Show(!onlineOpen);
	}
	
	override void OnHide()
	{
		super.OnHide();
		if (FeedbackDialogVisible())
		{
			ToggleFeedbackDialog();
		}
	}
}
