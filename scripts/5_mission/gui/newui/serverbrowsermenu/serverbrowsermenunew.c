const int MAX_FAVORITES = 25;

#ifdef PLATFORM_CONSOLE
const int SERVER_BROWSER_PAGE_SIZE = 22;
#else
const int SERVER_BROWSER_PAGE_SIZE = 5;
#endif

class ServerBrowserMenuNew extends UIScriptedMenu
{
	protected Widget				m_Play;
	protected TextWidget			m_PlayButtonLabel;
	protected TextWidget			m_OpenStoreButtonLabel;
	
	protected Widget				m_Back;
	protected Widget				m_CustomizeCharacter;
	protected TextWidget			m_PlayerName;
	
	protected TabberUI				m_Tabber;
	protected ref ServerBrowserTab	m_OfficialTab;
	protected ref ServerBrowserTab	m_CommunityTab;
	protected ref ServerBrowserTab  m_FavoritesTab;
	protected ref ServerBrowserTab	m_LANTab;
	
	protected TabType				m_IsRefreshing = TabType.NONE;
	protected ref TStringArray		m_Favorites;
	protected ServerBrowserEntry	m_SelectedServer;
	
	override Widget Init()
	{
		#ifdef PLATFORM_CONSOLE
		layoutRoot 		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/server_browser/xbox/server_browser.layout");
		m_FavoritesTab  = new ServerBrowserFavoritesTabConsolePages(layoutRoot.FindAnyWidget("Tab_0"), this, TabType.FAVORITE);
		m_OfficialTab	= new ServerBrowserTabConsolePages(layoutRoot.FindAnyWidget("Tab_1"), this, TabType.OFFICIAL);
		m_CommunityTab	= new ServerBrowserTabConsolePages(layoutRoot.FindAnyWidget("Tab_2"), this, TabType.COMMUNITY);
		#else
		layoutRoot 		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/server_browser/pc/server_browser.layout");
		m_FavoritesTab  = new ServerBrowserFavoritesTabPc(layoutRoot.FindAnyWidget("Tab_0"), this, TabType.FAVORITE);
		m_OfficialTab	= new ServerBrowserTabPc(layoutRoot.FindAnyWidget("Tab_1"), this, TabType.OFFICIAL);
		m_CommunityTab	= new ServerBrowserTabPc(layoutRoot.FindAnyWidget("Tab_2"), this, TabType.COMMUNITY);
		m_LANTab		= new ServerBrowserTabPc(layoutRoot.FindAnyWidget("Tab_3"), this, TabType.LAN);
		#endif
		
		layoutRoot.FindAnyWidget("Tabber").GetScript(m_Tabber);
		
		m_Play					= layoutRoot.FindAnyWidget("play");
		m_PlayButtonLabel		= TextWidget.Cast(m_Play.FindAnyWidget("play_label"));
		m_OpenStoreButtonLabel	= TextWidget.Cast(m_Play.FindAnyWidget("open_store_label"));
		m_Back					= layoutRoot.FindAnyWidget("back_button");
		m_CustomizeCharacter	= layoutRoot.FindAnyWidget("customize_character");
		m_PlayerName			= TextWidget.Cast(layoutRoot.FindAnyWidget("character_name_text"));
		m_Favorites 			= new TStringArray;
		
		#ifndef PLATFORM_CONSOLE
		layoutRoot.FindAnyWidget("customize_character").Show(false);
		layoutRoot.FindAnyWidget("character").Show(false);
		#endif
		
		Refresh();
		
		string version;
		GetGame().GetVersion(version);
		
		#ifdef PLATFORM_CONSOLE
		version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		if (GetGame().GetInput().IsEnabledMouseAndKeyboard())
		{
			layoutRoot.FindAnyWidget("toolbar_bg").Show(false);
		}

		RichTextWidget playPanelBack = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon0"));
		playPanelBack.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_NORMAL));

		RichTextWidget playPanelPlay = RichTextWidget.Cast(layoutRoot.FindAnyWidget("PlayIcon0"));
		playPanelPlay.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_NORMAL));
		#else
		version = "#main_menu_version" + " " + version;
		#endif
		
		OnlineServices.m_ServersAsyncInvoker.Insert(OnLoadServersAsync);
		OnlineServices.m_ServerModLoadAsyncInvoker.Insert(OnLoadServerModsAsync);
		LoadFavoriteServers();
		
		m_Tabber.m_OnTabSwitch.Insert(OnTabSwitch);
		
		m_FavoritesTab.RefreshList();

		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		//Sort init
		TextWidget sort_text = TextWidget.Cast(layoutRoot.FindAnyWidget("SortText"));
		sort_text.SetText("#str_serverbrowserroot_toolbar_bg_consoletoolbar_sort_sorttext0");
		#endif
		
		PPERequesterBank.GetRequester(PPERequester_ServerBrowserBlur).Start(new Param1<float>(0.5));
		
		GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);

		return layoutRoot;
	}
	
	void ~ServerBrowserMenuNew()
	{
		#ifdef PLATFORM_CONSOLE
		SaveFavoriteServersConsoles();
		#endif

		OnlineServices.m_ServersAsyncInvoker.Remove(OnLoadServersAsync);
		OnlineServices.m_ServerModLoadAsyncInvoker.Remove(OnLoadServerModsAsync);
		m_Tabber.m_OnTabSwitch.Remove(OnTabSwitch);
		PPERequesterBank.GetRequester(PPERequester_ServerBrowserBlur).Stop();
	}
	
	TStringArray GetFavoritedServerIds()
	{
		return m_Favorites;
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
			#ifdef PLATFORM_CONSOLE
			UpdateControlsElements();
			layoutRoot.FindAnyWidget("toolbar_bg").Show(true);
			layoutRoot.FindAnyWidget("ConsoleControls").Show(true);
			layoutRoot.FindAnyWidget("PlayIcon0").Show(false);
			layoutRoot.FindAnyWidget("BackIcon0").Show(false);
			if (GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
			{
				GetGame().GetUIManager().ShowUICursor(false);
			}
			#endif
		break;

		default:
			#ifdef PLATFORM_CONSOLE
			if (GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
			{
				layoutRoot.FindAnyWidget("toolbar_bg").Show(false);
				layoutRoot.FindAnyWidget("ConsoleControls").Show(false);
				layoutRoot.FindAnyWidget("PlayIcon0").Show(true);
				layoutRoot.FindAnyWidget("BackIcon0").Show(true);
				GetGame().GetUIManager().ShowUICursor(true);
			}
			#endif
		break;
		}
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button == MouseState.LEFT)
		{
			if (w == m_Play)
			{
				Play();
				return true;
			}
			else if (w == m_Back)
			{
				Back();
				return true;
			}
			else if (w == m_CustomizeCharacter)
			{
				CustomizeCharacter();
				return true;
			}
		}
		return false;
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
	
	void SetServersLoadingTab(TabType refreshing)
	{
		m_IsRefreshing = refreshing;
		
		OnlineServices.m_ServersAsyncInvoker.Remove(OnLoadServersAsync);
		OnlineServices.m_ServersAsyncInvoker.Insert(OnLoadServersAsync, EScriptInvokerInsertFlags.NONE);
	}

	TabType GetServersLoadingTab()
	{
		return m_IsRefreshing;
	}
	
	void AddFavoritesToFilter(GetServersInput input)
	{
		foreach (string id : m_Favorites)
		{
			array<string> output = new array<string>;
			id.Split(":", output);
			if (output.Count() == 2)
			{
				string ip = output[0];
				int port = output[1].ToInt();
				input.AddFavourite(ip, port);
			}
		}
	}

	bool IsFavorited(string server_id)
	{
		int index = -1;
		if (m_Favorites)
		{
			index = m_Favorites.Find(server_id);		
		}
		return (index >= 0);
	}
	
	// Returns whether server was favorited or not
	bool SetFavoriteConsoles(string ipAddress, int port, bool favorite)
	{
		if (favorite && m_Favorites.Count() >= MAX_FAVORITES)
		{
			g_Game.GetUIManager().ShowDialog("#layout_notification_info_warning", "#STR_MaxFavouriteReached", 0, DBT_OK, DBB_YES, DMT_EXCLAMATION, this);
			return false;
		}

		AddFavorite(ipAddress, port, favorite);
		SaveFavoriteServersConsoles();
		
		return favorite;
	}
	
	void AddFavorite(string ipAddress, int port, bool favorite)
	{
		string serverId = ipAddress + ":" + port;
		bool isFavorited = IsFavorited(serverId);
		
		if (favorite && !isFavorited)
		{
			m_Favorites.Insert(serverId);
		}
		else if (isFavorited)
		{
			m_Favorites.RemoveItem(serverId);
			m_OfficialTab.Unfavorite(serverId);
			m_CommunityTab.Unfavorite(serverId);
			m_FavoritesTab.Unfavorite(serverId);
#ifndef PLATFORM_CONSOLE
			m_LANTab.Unfavorite(serverId);
#endif
		}
	}
	
	void Back()
	{
		GetGame().GetUIManager().Back();
	}
	
	void ShowYButton(bool show)
	{
		RichTextWidget yIcon = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ResetIcon"));
		TextWidget yText = TextWidget.Cast(layoutRoot.FindAnyWidget("ResetText"));
		
		if (yIcon)
		{
			yIcon.Show(show);
		}
		
		if (yText)
		{
			yText.Show(show);
		}
	}
	
	void UpdateYButtonLabel(string text)
	{
		TextWidget yText = TextWidget.Cast(layoutRoot.FindAnyWidget("ResetText"));
		if (yText)
		{
			yText.SetText(text);
			yText.Update();
		}
	}
	
	void ShowAButton(bool show)
	{
		RichTextWidget aIcon = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ConnectIcon"));
		TextWidget aText = TextWidget.Cast(layoutRoot.FindAnyWidget("ConnectText"));
		
		if (aIcon)
		{
			aIcon.Show(show);
		}
		
		if (aText)
		{
			aText.Show(show);
		}	
	}
	
	void UpdateAButtonLabel(string text)
	{
		TextWidget aText = TextWidget.Cast(layoutRoot.FindAnyWidget("ConnectText"));
		if (aText)
		{
			aText.SetText(text);
			aText.Update();
		}
	}
	
	void UpdateXButtonLabel(string text)
	{
		TextWidget xText = TextWidget.Cast(layoutRoot.FindAnyWidget("RefreshText"));
		if (xText)
		{
			xText.SetText(text);
			xText.Update();
		}
	}
	
	void ShowThumbRButton(bool show)
	{
		RichTextWidget trIcon = RichTextWidget.Cast(layoutRoot.FindAnyWidget("SwitchIcon"));
		TextWidget trText = TextWidget.Cast(layoutRoot.FindAnyWidget("SwitchText"));
		
		if (trIcon)
		{
			trIcon.Show(show);
		}
		
		if (trText)
		{
			trText.Show(show);
		}
	}
	
	void UpdateThumbRButtonLabel(string text)
	{
		TextWidget trText = TextWidget.Cast(layoutRoot.FindAnyWidget("SwitchText"));
		if (trText)
		{
			trText.SetText(text);
			trText.Update();
		}
	}
	
	void FilterFocus(bool focus)
	{
	#ifdef PLATFORM_CONSOLE
		if (focus)
		{
			ShowThumbRButton(false);
			string aButtonLabel;
			if (GetServersLoadingTab() == TabType.FAVORITE)
			{
			#ifdef PLATFORM_PS4
				aButtonLabel = "#ps4_ingame_menu_select";
			#else
				aButtonLabel = "#layout_xbox_ingame_menu_select";
			#endif
			}
			else
			{
				aButtonLabel = "#dialog_change";
			}

			UpdateAButtonLabel(aButtonLabel);
			UpdateXButtonLabel("#server_browser_menu_refresh");
			UpdateYButtonLabel("#server_details_header");
		}
	#endif
	}

	void DetailsFocus(bool focus)
	{
	#ifdef PLATFORM_CONSOLE
		if (focus)
		{
			ShowThumbRButton(false);
			UpdateYButtonLabel("#STR_server_browser_tab_root_details_show_server_filters");
			string aButtonLabel;
		#ifdef PLATFORM_PS4
			aButtonLabel = "#ps4_ingame_menu_select";
		#else
			aButtonLabel = "#layout_xbox_ingame_menu_select";
		#endif
			UpdateAButtonLabel(aButtonLabel);
		}
	#endif
	}
	
	void BackButtonFocus()
	{
		SetFocus(m_Back);
	}

	void ServerListFocus(bool focus, bool favorite)
	{
	#ifdef PLATFORM_CONSOLE		
		if (focus)
		{
			UpdateAButtonLabel("#server_browser_menu_connect");

			string trText;
			if (favorite)
			{
				trText = "#server_browser_menu_unfavorite";
			}
			else
			{
				trText = "#server_browser_menu_favorite";
			}
			
			UpdateThumbRButtonLabel(trText);
		}
	#endif
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
	
	override void Refresh()
	{
		string name;
		
		#ifdef PLATFORM_CONSOLE
			if (GetGame().GetUserManager() && GetGame().GetUserManager().GetSelectedUser())
			{
				name = GetGame().GetUserManager().GetSelectedUser().GetName();
				if (name.LengthUtf8() > 18)
				{
					name = name.SubstringUtf8(0, 18);
					name += "...";
				}
			}
		#else
			g_Game.GetPlayerNameShort(14, name);
		#endif
		
		if (m_PlayerName)
			m_PlayerName.SetText(name);
		
		string version;
		GetGame().GetVersion(version);
		#ifdef PLATFORM_CONSOLE
			version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		#else
			version = "#main_menu_version" + " " + version;
		#endif
	}
	
	override void Update(float timeslice)
	{
		if (!GetGame().GetUIManager().IsDialogVisible() && !GetDayZGame().IsConnecting())
		{
			if (GetUApi().GetInputByID(UAUIThumbRight).LocalPress())
			{
				GetSelectedTab().PressThumbRight();
			}
			
			if (GetUApi().GetInputByID(UAUITabLeft).LocalPress())
			{
				m_Tabber.PreviousTab();
			}
			
			if (GetUApi().GetInputByID(UAUITabRight).LocalPress())
			{
				m_Tabber.NextTab();
			}
			
			if (GetUApi().GetInputByID(UAUINextDown).LocalPress())
			{
				GetSelectedTab().PressSholderLeft();
			}
			
			if (GetUApi().GetInputByID(UAUINextUp).LocalPress())
			{
				GetSelectedTab().PressSholderRight();
			}
			
			if (GetUApi().GetInputByID(UAUISelect).LocalPress())
			{
				GetSelectedTab().PressA();
			}
			
			if (GetUApi().GetInputByID(UAUICtrlX).LocalPress())
			{
				GetSelectedTab().PressX();
			}
			
			if (GetUApi().GetInputByID(UAUICtrlY).LocalPress())
			{
				GetSelectedTab().PressY();
			}
			
			if (GetUApi().GetInputByID(UAUILeft).LocalPress())
			{
				GetSelectedTab().Left();
			}
			
			// LEFT HOLD
			if (GetUApi().GetInputByID(UAUILeft).LocalHold())
			{
				GetSelectedTab().LeftHold();
			}
			
			// LEFT RELEASE
			if (GetUApi().GetInputByID(UAUILeft).LocalRelease())
			{
				GetSelectedTab().LeftRelease();
			}
			
			if (GetUApi().GetInputByID(UAUIRight).LocalPress())
			{
				GetSelectedTab().Right();
			}
			
			// RIGHT HOLD
			if (GetUApi().GetInputByID(UAUIRight).LocalHold())
			{
				GetSelectedTab().RightHold();
			}
			
			// RIGHT RELEASE
			if (GetUApi().GetInputByID(UAUIRight).LocalRelease())
			{
				GetSelectedTab().RightRelease();
			}
			
			if (GetUApi().GetInputByID(UAUIUp).LocalPress())
			{
				GetSelectedTab().Up();
			}
			
			if (GetUApi().GetInputByID(UAUIDown).LocalPress())
			{
				GetSelectedTab().Down();
			}
	
			if (GetUApi().GetInputByID(UAUIBack).LocalPress())
			{
				Back();
			}
		}
		
		super.Update(timeslice);
	}
	
	bool IsFocusable(Widget w)
	{
		if (w)
		{
			return (w == m_Play || w == m_CustomizeCharacter || w == m_Back);
		}
		return false;
	}
	
	void LoadFavoriteServers()
	{		
		m_Favorites = new TStringArray;
		
#ifdef PLATFORM_WINDOWS
		OnlineServices.GetFavoriteServers(m_Favorites);
#else
		GetGame().GetProfileStringList("SB_Favorites", m_Favorites);
		
		// ignore any ids that do not follow correct IP:PORT format
		for (int i = 0; i < m_Favorites.Count(); ++i)
		{
			string id = m_Favorites[i];
			array<string> output = new array<string>;
						
			id.Split(":", output);
			if (output.Count() != 2)
			{
				m_Favorites.Remove(i);
				--i;
			}
		}
		
		// only handle MAX_FAVORITES on consoles
		if (m_Favorites.Count() > MAX_FAVORITES)
		{
			// favorites are ordered by when they were favorited
			// so keep most recent favorites and ignore oldest favorites
			m_Favorites.Invert();
			m_Favorites.Resize(MAX_FAVORITES);
			m_Favorites.Invert();
		}
#endif
	}
	
	void SaveFavoriteServersConsoles()
	{
		GetGame().SetProfileStringList("SB_Favorites", m_Favorites);
		GetGame().SaveProfile();
	}
	
	void SelectServer(ServerBrowserEntry server)
	{
		if (m_SelectedServer)
		{
			m_SelectedServer.Deselect();						
		}
				
		m_SelectedServer = server;
		
		string mapNM = m_SelectedServer.GetMapToRun();
		if (!g_Game.VerifyWorldOwnership(mapNM))
		{
			m_PlayButtonLabel.Show(false);
			m_OpenStoreButtonLabel.Show(true);
		}
		else 
		{
			m_PlayButtonLabel.Show(true);
			m_OpenStoreButtonLabel.Show(false);
		}
	}
	
	void DeselectCurrentServer()
	{
		if (m_SelectedServer)
			m_SelectedServer.Deselect();
		
		m_SelectedServer = null;
	}
	
	void Connect(ServerBrowserEntry server)
	{
		SelectServer(server);
		#ifdef PLATFORM_CONSOLE
		SaveFavoriteServersConsoles();
		#endif
		Play();
	}
	
	void Play()
	{
		if (m_SelectedServer)
		{
			string mapNM = m_SelectedServer.GetMapToRun();
			
			if (!g_Game.VerifyWorldOwnership(mapNM))
			{				
				/*JsonDataDLCList data = DlcDataLoader.GetData();
				foreach (JsonDataDLCInfo dlcInfo : data.DLCs)
				{
					// fetch mod info
					// if server runs this dlcInfo && !info.GetIsOwned()
					//info.GoToStore();
				}*/
		
				GetGame().GetUIManager().ShowDialog("#server_browser_connect_label", "#mod_detail_info_warning", 232, DBT_OK, DBB_NONE, DMT_INFO, GetGame().GetUIManager().GetMenu());
				g_Game.GoBuyWorldDLC(mapNM);
				return;
			}
			
			string ip = m_SelectedServer.GetIP();
			int port = m_SelectedServer.GetPort();
			int steamQueryPort = m_SelectedServer.GetSteamQueryPort();
			g_Game.ConnectFromServerBrowserEx(ip, port, steamQueryPort, "");
		}
	}
	
	void CustomizeCharacter()
	{
		PPERequesterBank.GetRequester(PPERequester_ServerBrowserBlur).Stop();
		EnterScriptedMenu(MENU_CHARACTER);
	}
	
	// Unused?!
	void NextCharacter()
	{
		
	}
	
	// Unused?!
	void PreviousCharacter()
	{
		
	}
	
	ServerBrowserTab GetSelectedTab()
	{
		switch (m_Tabber.GetSelectedIndex())
		{
			case 0:
			{
				return m_FavoritesTab;
			}
			case 1:
			{
				return m_OfficialTab;
			}
			case 2:
			{
				return m_CommunityTab;
			}
			case 3:
			{
				return m_LANTab;
			}
		}
		return null;
	}
	
	void OnTabSwitch()
	{
	#ifdef PLATFORM_CONSOLE
		ShowThumbRButton(false);
	#endif
		
		LoadFavoriteServers();
		SetServersLoadingTab(TabType.NONE);
		
		if (GetSelectedTab().IsNotInitialized())
		{
			GetSelectedTab().RefreshList();
		}
		
		GetSelectedTab().Focus();
		
	#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		if (GetSelectedTab().GetTabType() != TabType.FAVORITE)
		{
			ShowYButton(true);
		}
		else
		{
			ShowYButton(false);
		}
	#endif			
	}
	
	void OnLoadServerModsAsync(GetServerModListResult result_list)
	{
		if (GetSelectedTab())
		{
			GetSelectedTab().OnLoadServerModsAsync(result_list.m_Id, result_list.m_Mods);
		}
	}
	
	void OnLoadServersAsync(GetServersResult result_list, EBiosError error, string response)
	{		
		#ifdef PLATFORM_WINDOWS
			#ifdef PLATFORM_CONSOLE
				GetSelectedTab().OnLoadServersAsyncConsole(result_list, error, response);
			#else
				GetSelectedTab().OnLoadServersAsyncPC(result_list, error, response);
			#endif
		#else
			GetSelectedTab().OnLoadServersAsyncConsole(result_list, error, response);
		#endif
	}
	
	//Coloring functions (Until WidgetStyles are useful)
	void ColorHighlight(Widget w)
	{
		if (!w)
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
	
	void ColorNormal(Widget w)
	{
		if (!w)
			return;
		
		int color_pnl = ARGB(0, 0, 0, 0);
		int color_lbl = ARGB(255, 255, 255, 255);
		
		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}
	
	void ButtonSetText(Widget w, string text)
	{
		if (!w)
			return;
				
		TextWidget label = TextWidget.Cast(w.FindWidget(w.GetName() + "_label"));
		
		if (label)
		{
			label.SetText(text);
		}
		
	}
	
	void ButtonSetColor(Widget w, int color)
	{
		if (!w)
			return;
		
		Widget panel = w.FindWidget(w.GetName() + "_panel");
		
		if (panel)
		{
			panel.SetColor(color);
		}
	}
	
	void ButtonSetTextColor(Widget w, int color)
	{
		if (!w)
			return;

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

	protected void UpdateControlsElements()
	{
		RichTextWidget toolbar_a = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ConnectIcon"));
		RichTextWidget toolbar_b = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		RichTextWidget toolbar_x = RichTextWidget.Cast(layoutRoot.FindAnyWidget("RefreshIcon"));
		RichTextWidget toolbar_y = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ResetIcon"));
		RichTextWidget toolbar_tr = RichTextWidget.Cast(layoutRoot.FindAnyWidget("SwitchIcon"));
		toolbar_a.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		toolbar_x.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlX", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		toolbar_y.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlY", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		toolbar_tr.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIThumbRight", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
	}
	
	override void OnShow()
	{
		super.OnShow();
		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());
	}
	
	override void OnHide()
	{
		super.OnHide();
		PPERequesterBank.GetRequester(PPERequester_ServerBrowserBlur).Stop();
	}
}