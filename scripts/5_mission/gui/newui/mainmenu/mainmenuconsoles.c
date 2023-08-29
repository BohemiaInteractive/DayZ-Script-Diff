class MainMenuConsole extends UIScriptedMenu
{
	protected ref MainMenuVideo		m_Video;
	
	protected MissionMainMenu		m_Mission;
	protected DayZIntroScenePC 		m_ScenePC;
	
	protected TextWidget			m_PlayerName;
	protected TextWidget			m_Version;
	
	protected Widget				m_ChangeAccount;
	protected Widget				m_CustomizeCharacter;
	protected Widget				m_PlayVideo;
	protected Widget				m_Tutorials;
	protected Widget				m_Options;
	protected Widget				m_Controls;
	protected Widget				m_Play;
	protected Widget				m_MessageButton;
	
	protected ref Widget			m_LastFocusedButton;
	
	protected ref array<ref ModInfo> 				m_AllDLCs;
	protected Widget 								m_DlcFrame;
	protected ref map<string,ref ModInfo> 			m_AllDlcsMap;
	protected ref JsonDataDLCList 					m_DlcData;
	protected ref array<ref MainMenuDlcHandlerBase> m_DlcHandlers;
	protected ref MainMenuDlcHandlerBase 			m_DisplayedDlcHandler;

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/main_menu_console.layout");
		
		m_PlayerName				= TextWidget.Cast(layoutRoot.FindAnyWidget("character_name_xbox"));
		
		m_ChangeAccount				= layoutRoot.FindAnyWidget("choose_account");
		m_CustomizeCharacter		= layoutRoot.FindAnyWidget("customize_character");
		m_PlayVideo					= layoutRoot.FindAnyWidget("play_video");
		m_Tutorials					= layoutRoot.FindAnyWidget("tutorials");
		m_Options					= layoutRoot.FindAnyWidget("options");
		m_Controls					= layoutRoot.FindAnyWidget("controls");
		m_Play						= layoutRoot.FindAnyWidget("play");
		m_MessageButton				= layoutRoot.FindAnyWidget("message_button");
		
		m_DlcFrame 					= layoutRoot.FindAnyWidget("dlc_Frame");
		m_Version					= TextWidget.Cast(layoutRoot.FindAnyWidget("version"));
		m_Mission					= MissionMainMenu.Cast(GetGame().GetMission());
		m_LastFocusedButton			= m_Play;
				
		GetGame().GetUIManager().ScreenFadeOut(1);

		string launch_done;
		if (!GetGame().GetProfileString("FirstLaunchDone", launch_done) || launch_done != "true")
		{
			GetGame().SetProfileString("FirstLaunchDone", "true");
			GetGame().GetUIManager().ShowDialog("#main_menu_tutorial", "#main_menu_tutorial_desc", 555, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
			GetGame().SaveProfile();
		}
		
		UpdateControlsElementVisibility();
		LoadMods();		
		Refresh();
		
		if (GetGame().GetMission())
		{
			GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
			GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		}
		
		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());
		
		GetGame().GetContentDLCService().m_OnChange.Insert(OnDLCChange);
		
		return layoutRoot;
	}
	
	void ~MainMenuConsole()
	{
		if (GetGame().GetMission())
		{
			GetGame().GetMission().GetOnInputPresetChanged().Remove(OnInputPresetChanged);
			GetGame().GetMission().GetOnInputDeviceChanged().Remove(OnInputDeviceChanged);
		}
		
		if (GetGame().GetContentDLCService())
			GetGame().GetContentDLCService().m_OnChange.Remove(OnDLCChange);
	}
	
	void OnDLCChange(EDLCId dlcId)
	{
		m_AllDLCs = null;
		LoadMods();
	}
	
	void LoadMods()
	{
		if (m_AllDLCs != null)
			return;
		
		m_AllDLCs = new array<ref ModInfo>;
		
		GetGame().GetModInfos(m_AllDLCs);
		if (m_AllDLCs.Count() > 0)
		{
			m_AllDLCs.Remove(m_AllDLCs.Count() - 1);
			m_AllDLCs.Invert();
		}
		
		FilterDLCs(m_AllDLCs);
		PopulateDlcFrame();
		
		UpdateControlsElements();
	}
	
	//! leaves ONLY DLCs
	void FilterDLCs(inout array<ref ModInfo> modArray)
	{
		if (!m_AllDlcsMap)
			m_AllDlcsMap = new map<string,ref ModInfo>;
		m_AllDlcsMap.Clear();		
		
		int count = modArray.Count();
		for (int i = count - 1; i > -1; i--)
		{
			if (!modArray[i].GetIsDLC())
				modArray.Remove(i);
			
			m_AllDlcsMap.Set(modArray[i].GetName(),modArray[i]);
		}
	}
	
	void PopulateDlcFrame()
	{
		if (!m_DlcHandlers)
			m_DlcHandlers = new array<ref MainMenuDlcHandlerBase>();
		
		m_DlcData = DlcDataLoader.GetData();
		int count = m_DlcData.DLCs.Count();
		JsonDataDLCInfo data;
		ModInfo info;
		
		for (int i = 0; i < count; i++)
		{
			data = m_DlcData.DLCs[i];
			info = m_AllDlcsMap.Get(data.Name);
			MainMenuDlcHandlerBase handler = new MainMenuDlcHandlerBase(info, m_DlcFrame, data);
			
			if (data.Name == "Livonia DLC")
			{
				handler.ShowInfoPanel(true);
				m_DisplayedDlcHandler = handler;//TODO: carousel will take care of this later
			}
			else
				handler.ShowInfoPanel(false);
			
			m_DlcHandlers.Insert(handler);
		}
	}

	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		#endif
	}

	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		UpdateControlsElements();
		UpdateControlsElementVisibility();
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_Play)
		{
			m_LastFocusedButton = m_Play;
			OpenMenuServerBrowser();
			return true;
		}
		else if (w == m_Options)
		{
			m_LastFocusedButton = m_Options;
			OpenMenuOptions();					
			return true;
		}
		else if (w == m_PlayVideo)
		{
			m_Mission.StopMusic();
			m_LastFocusedButton = m_PlayVideo;
			OpenMenuPlayVideo();
			return true;
		}
		else if (w == m_Tutorials)
		{
			m_LastFocusedButton = m_Tutorials;
			OpenMenuTutorials();
			return true;
		}
		else if (w == m_Controls)
		{
			m_LastFocusedButton = m_Controls;
			OpenMenuControls();
			return true;
		}
		else if (w == m_CustomizeCharacter)
		{
			m_LastFocusedButton = m_CustomizeCharacter;
			OpenMenuCustomizeCharacter();
			return true;
		}
		else if (w == m_ChangeAccount)
		{
			m_LastFocusedButton = m_ChangeAccount;
			ChangeAccount();
			return true;
		}
		else if (w == m_MessageButton)
		{
			OpenCredits();
			return true;
		}
		return false;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		ColorHighlight(w);
		return true;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		ColorNormal(w);
		return true;
	}
	
	override void Refresh()
	{
		string name;
		
		if (GetGame().GetUserManager() && GetGame().GetUserManager().GetSelectedUser())
		{
			name = GetGame().GetUserManager().GetSelectedUser().GetName();
			if (name.LengthUtf8() > 18)
			{
				name = name.SubstringUtf8(0, 18);
				name += "...";
			}
		}
		m_PlayerName.SetText(name);		
		
		string version;
		GetGame().GetVersion(version);
		m_Version.SetText("#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")");
		
		if (GetGame().GetMission() != null)	// if missionMainMenu still exist
		{
			if (m_Mission && !m_Mission.GetMenuMusic())
			{
				m_Mission.PlayMusic();
			}
		}
	}	
	
	override void OnShow()
	{
		GetDayZGame().GetBacklit().MainMenu_OnShow();
	
		SetFocus(m_LastFocusedButton);

		LoadMods();
		Refresh();
		
		if (m_ScenePC && m_ScenePC.GetIntroCamera())
		{
			m_ScenePC.GetIntroCamera().LookAt(m_ScenePC.GetIntroCharacter().GetPosition() + Vector(0, 1, 0));
		}
		if (m_DisplayedDlcHandler)
			m_DisplayedDlcHandler.ShowInfoPanel(true);
		
		super.OnShow();
		#ifdef PLATFORM_CONSOLE
			#ifndef PLATFORM_PS4
			layoutRoot.FindAnyWidget("choose_account").Show(GetGame().GetInput().IsEnabledMouseAndKeyboard());
			#endif
		layoutRoot.FindAnyWidget("ButtonHolderCredits").Show(GetGame().GetInput().IsEnabledMouseAndKeyboard());
		UpdateControlsElements();
		UpdateControlsElementVisibility();
		#endif
	}
	
	override void OnHide()
	{
		if (m_DisplayedDlcHandler)
			m_DisplayedDlcHandler.ShowInfoPanel(false);
		GetDayZGame().GetBacklit().MainMenu_OnHide();
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		if (g_Game.GetLoadState() != DayZGameState.CONNECTING && !GetGame().GetUIManager().IsDialogVisible())
		{
		#ifndef PLATFORM_CONSOLE
			if (GetUApi().GetInputByID(UAUIBack).LocalPress())
			{
				Exit();
			}
		#else
			if (GetUApi().GetInputByID(UAUICredits).LocalPress())
			{
				OpenCredits();
			}
		#endif
		}
		
		#ifdef PLATFORM_XBOX
		if (GetUApi().GetInputByID(UAUICtrlY).LocalPress())
		{
			ChangeAccount();
		}
		#endif
		if (GetUApi().GetInputByID(UAUICtrlX).LocalPress())
		{
			if (CanStoreBeOpened())
			{
				m_DisplayedDlcHandler.GetModInfo().GoToStore();
			}
		}
	}
	
	bool CanStoreBeOpened()
	{
		return m_DisplayedDlcHandler != null;
	}
	
	void OpenMenuServerBrowser()
	{
		EnterScriptedMenu(MENU_SERVER_BROWSER);
	}

	void OpenMenuControls()
	{
		EnterScriptedMenu(MENU_XBOX_CONTROLS);
	}
	
	void OpenMenuOptions()
	{
		EnterScriptedMenu(MENU_OPTIONS);
	}
	
	void OpenMenuPlayVideo()
	{
		EnterScriptedMenu(MENU_VIDEO);
	}
	
	void OpenMenuTutorials()
	{
		EnterScriptedMenu(MENU_TUTORIAL);
	}
	
	void OpenMenuCustomizeCharacter()
	{
		EnterScriptedMenu(MENU_CHARACTER);
	}
	
	void OpenCredits()
	{
		EnterScriptedMenu(MENU_CREDITS);
		m_Mission.OnMenuEnter(MENU_CREDITS);
	}
	
	void ChangeAccount()
	{
		BiosUserManager user_manager = GetGame().GetUserManager();
		if (user_manager)
		{
			g_Game.SetLoadState(DayZLoadState.MAIN_MENU_START);
			#ifndef PLATFORM_WINDOWS
			user_manager.SelectUserEx(null);
			#endif
			GetGame().GetUIManager().Back();
		}
	}
	
	void Exit()
	{
		GetGame().GetUIManager().ShowDialog("#main_menu_exit", "#main_menu_exit_desc", IDC_MAIN_QUIT, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
	}
		
	//Coloring functions (Until WidgetStyles are useful)
	void ColorHighlight(Widget w)
	{
		if (!w)
			return;
		
		int color_pnl = ARGB(255, 200, 0, 0);
		int color_lbl = ARGB(255, 255, 255, 255);
		
		ButtonSetColor(w, color_pnl);
		ButtonSetAlphaAnim(w);
		ButtonSetTextColor(w, color_lbl);
	}
	
	void ColorNormal(Widget w)
	{
		if (!w)
			return;
		
		int color_pnl = ARGB(0, 0, 0, 0);
		int color_lbl = ARGB(255, 255, 255, 255);
		
		ButtonSetColor(w, color_pnl);
		ButtonSetAlphaAnim(null);
		ButtonSetTextColor(w, color_lbl);
	}
	
	override bool OnModalResult(Widget w, int x, int y, int code, int result)
	{
		if (code == IDC_MAIN_QUIT)
		{
			if (result == 2)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(g_Game.RequestExit, IDC_MAIN_QUIT);
			}
			
			return true;
		}
		else if (code == 555)
		{
			if (result == 2)
			{
				OpenMenuTutorials();
			}
		}
		return false;
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
	
	void ButtonSetAlphaAnim(Widget w)
	{
		if (!w)
			return;
		
		Widget panel = w.FindWidget(w.GetName() + "_panel");
		
		if (panel)
		{
			SetWidgetAnimAlpha(panel);
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
		RichTextWidget toolbar_text = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ContextToolbarText"));
		string context = InputUtils.GetRichtextButtonIconFromInputAction("UAUICredits", "#menu_credits", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR);
		
#ifndef PLATFORM_PS4
		context += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlY", "#layout_xbox_main_menu_toolbar_account", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
#endif
		context += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", "#layout_xbox_main_menu_toolbar_select", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		
		toolbar_text.SetText(context);
	}
	
	protected void UpdateControlsElementVisibility()
	{
		bool toolbarShow = false;
		#ifdef PLATFORM_CONSOLE
		toolbarShow = !GetGame().GetInput().IsEnabledMouseAndKeyboard() || GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		#endif
		
		layoutRoot.FindAnyWidget("toolbar_bg").Show(toolbarShow);
	}
}
