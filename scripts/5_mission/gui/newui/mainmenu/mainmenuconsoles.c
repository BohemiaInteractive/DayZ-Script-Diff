#ifndef BUILD_EXPERIMENTAL
	#define ENABLE_CAROUSEL
#endif

class MainMenuConsole extends UIScriptedMenu
{
	protected ref MainMenuVideo		m_Video;
	
	protected MissionMainMenu		m_Mission;
	protected DayZIntroScenePC 		m_ScenePC;
	
	protected TextWidget			m_PlayerName;
	protected TextWidget			m_Version;
	
	protected Widget				m_MainMenuPanel;
	protected Widget				m_DialogPanel;
	protected Widget				m_ChangeAccount;
	protected Widget				m_CustomizeCharacter;
	protected Widget				m_PlayVideo;
	protected Widget				m_Tutorials;
	protected Widget				m_Options;
	protected Widget				m_Exit;
	protected Widget				m_Controls;
	protected Widget				m_Play;
	protected Widget				m_MessageButton;
	protected Widget				m_ShowFeedback;
	protected ImageWidget			m_FeedbackQRCode;
	protected ImageWidget			m_FeedbackPlatformIcon;
	protected ButtonWidget			m_FeedbackClose;
	protected RichTextWidget		m_FeedbackCloseLabel;
	
	protected ref Widget			m_LastFocusedButton;
	
	protected Widget 				m_DlcFrame;
		
	protected ref NewsCarousel      m_NewsCarousel;

	protected Widget 				m_NewsCarouselFrame;
	protected Widget				m_DisplayCarousel;
	
	protected ScreenWidthType		m_WidthType;
	protected int					m_Width, m_Height;

	override Widget Init()
	{
		#ifdef PLATFORM_MSSTORE
		layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/main_menu_msstore.layout");
		#else
		layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/main_menu_console.layout");
		#endif

		m_MainMenuPanel = layoutRoot.FindAnyWidget("main_menu_panel");
		m_PlayerName = TextWidget.Cast(layoutRoot.FindAnyWidget("character_name_xbox"));
		m_ChangeAccount	= layoutRoot.FindAnyWidget("choose_account");
		m_CustomizeCharacter = layoutRoot.FindAnyWidget("customize_character");
		m_PlayVideo	= layoutRoot.FindAnyWidget("play_video");
		m_Tutorials	= layoutRoot.FindAnyWidget("tutorials");
		m_Options = layoutRoot.FindAnyWidget("options");
		m_Exit = layoutRoot.FindAnyWidget("exit");
		m_Controls = layoutRoot.FindAnyWidget("controls");
		m_Play = layoutRoot.FindAnyWidget("play");
		m_MessageButton = layoutRoot.FindAnyWidget("message_button");
		
		m_DlcFrame = layoutRoot.FindAnyWidget("dlc_Frame");
		m_Version = TextWidget.Cast(layoutRoot.FindAnyWidget("version"));
		m_Mission = MissionMainMenu.Cast(g_Game.GetMission());
		m_ShowFeedback = layoutRoot.FindAnyWidget("feedback");
		m_FeedbackQRCode = ImageWidget.Cast(layoutRoot.FindAnyWidget("qr_image"));
		m_FeedbackClose = ButtonWidget.Cast(layoutRoot.FindAnyWidget("close_button"));
		m_FeedbackCloseLabel = RichTextWidget.Cast(layoutRoot.FindAnyWidget("close_button_label"));
		m_DialogPanel = layoutRoot.FindAnyWidget("main_menu_dialog");
		m_NewsCarouselFrame = layoutRoot.FindAnyWidget("carousel_Frame");
		
		m_LastFocusedButton	= m_Play;
				
		g_Game.GetUIManager().ScreenFadeOut(1);

		string launch_done;
		if (!g_Game.GetProfileString("FirstLaunchDone", launch_done) || launch_done != "true")
		{
			g_Game.SetProfileString("FirstLaunchDone", "true");
			g_Game.GetUIManager().ShowDialog("#main_menu_tutorial", "#main_menu_tutorial_desc", 555, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
			g_Game.SaveProfile();
		}
		
		UpdateControlsElementVisibility();
		LoadMods();
		Refresh();
		
		CheckWidth();

		#ifdef ENABLE_CAROUSEL
		m_NewsCarousel = new NewsCarousel(m_NewsCarouselFrame, this);
		#else
		m_NewsCarousel = null;
		#endif
		
		if (g_Game.GetMission())
		{
			g_Game.GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
			g_Game.GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		}
		
		OnInputDeviceChanged(g_Game.GetInput().GetCurrentInputDevice());
		
		g_Game.GetContentDLCService().m_OnChange.Insert(OnDLCChange);

		#ifdef PLATFORM_MSSTORE
		// Enable exit button
		m_Exit.Show(true);
		#endif
		
		#ifdef PLATFORM_CONSOLE
		#ifndef PLATFORM_PS4
		#ifdef PLATFORM_MSSTORE
		m_ChangeAccount.Show(false);
		#else
		m_ChangeAccount.Show(GetGame().GetInput().IsEnabledMouseAndKeyboard());
		#endif
		m_FeedbackQRCode.LoadImageFile(0, "gui/textures/feedback_qr_xbox.edds");
		#else
		m_FeedbackQRCode.LoadImageFile(0, "gui/textures/feedback_qr_ps.edds");
		#endif
		#endif
		
		#ifdef PLATFORM_CONSOLE
		#ifdef PLATFORM_XBOX
		m_ChangeAccount.Show(false);
		#endif
		#endif
		m_DlcFrame.Show(false);

		#ifdef PLATFORM_MSSTORE
		// Disable controls button on MS Store by default	
		m_Controls.Show(false);
		#endif

		return layoutRoot;
	}
	
	void ~MainMenuConsole()
	{
		if (g_Game.GetMission())
		{
			g_Game.GetMission().GetOnInputPresetChanged().Remove(OnInputPresetChanged);
			g_Game.GetMission().GetOnInputDeviceChanged().Remove(OnInputDeviceChanged);
		}
		
		if (g_Game.GetContentDLCService())
			g_Game.GetContentDLCService().m_OnChange.Remove(OnDLCChange);
	}
	
	void OnDLCChange(EDLCId dlcId)
	{
		MainMenuData.ClearAllDLCs();
		LoadMods();
		
		#ifdef ENABLE_CAROUSEL
		if (m_NewsCarousel)
		{
			m_NewsCarousel.Destroy();
			m_NewsCarousel = null;
			m_NewsCarousel = new NewsCarousel(m_NewsCarouselFrame, this);
		}
		#endif
	}
	
	void LoadMods()
	{
		MainMenuData.LoadMods();	
		UpdateControlsElements();
	}
	
	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		#endif
	}

	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		#ifndef PLATFORM_MSSTORE // No feedback or account switching on MS Store
		switch (pInputDeviceType)
		{
		case EInputDeviceType.CONTROLLER:
			if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
			{
				g_Game.GetUIManager().ShowUICursor(false);
				#ifdef PLATFORM_CONSOLE
				if (m_LastFocusedButton == m_ShowFeedback || !GetFocus() || GetFocus() == m_FeedbackClose)
				{
					SetFocus(m_Play);
				}

				m_FeedbackClose.Show(false);
				m_ShowFeedback.Show(false);
				#ifndef PLATFORM_PS4
				#ifndef PLATFORM_XBOX
				m_ChangeAccount.Show(false);
				if (m_LastFocusedButton == m_ChangeAccount)
				{
					SetFocus(m_Play);
				}
				#endif
				#endif
				#endif
			}
				
			#ifdef PLATFORM_MSSTORE
			// Enable controls button on MS Store by default	
			m_Controls.Show(true);
			#endif
		break;

		default:
			if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
			{
				g_Game.GetUIManager().ShowUICursor(true);
				#ifdef PLATFORM_CONSOLE
				m_ShowFeedback.Show(true);
				m_FeedbackClose.Show(true);
				m_FeedbackCloseLabel.SetText(string.Format("%1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#close", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_NORMAL)));
				#ifndef PLATFORM_PS4
				#ifndef PLATFORM_MSSTORE
				#ifndef PLATFORM_XBOX
				m_ChangeAccount.Show(true);
				#endif
				#endif
				#endif
				#endif

				#ifdef PLATFORM_MSSTORE
				// Disable controls button on MS Store
				m_Controls.Show(false);
				#endif
			}
		break;
		}
		#else
		switch (pInputDeviceType)
		{
		case EInputDeviceType.CONTROLLER:
			// Enable controls button on MS Store by default	
			m_Controls.Show(true);
		break;

		default:
			// Disable controls button on MS Store
			m_Controls.Show(false);
		break;
		}
		#endif

		UpdateControlsElements();
		UpdateControlsElementVisibility();
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!FeedbackDialogVisible())
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
			else if (w == m_Exit)
			{
				m_LastFocusedButton = m_Exit;
				Exit();
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
			#ifndef PLATFORM_MSSTORE // No account switching on MS Store
			#ifndef PLATFORM_XBOX
			else if (w == m_ChangeAccount)
			{
				m_LastFocusedButton = m_ChangeAccount;
				ChangeAccount();
				return true;
			}
			#endif
			#endif
			else if (w == m_MessageButton)
			{
				OpenCredits();
				return true;
			}
		}
		
		if (w == m_ShowFeedback || w == m_FeedbackClose)
		{
			m_LastFocusedButton = w;
			ToggleFeedbackDialog();
			return true;
		}
		return false;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		ColorHighlight(w);
		if (ButtonWidget.Cast(w))
		{
			m_LastFocusedButton = w;
		}
		return true;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		ColorNormal(w);
		return true;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w == m_FeedbackClose)
		{
			ColorHighlight(w);
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w == m_FeedbackClose)
		{
			ColorNormal(w);
			return true;
		}
		return true;
	}
	
	override void Refresh()
	{
		string name;
		
		if (g_Game.GetUserManager() && g_Game.GetUserManager().GetSelectedUser())
		{
			name = g_Game.GetUserManager().GetSelectedUser().GetName();
			if (name.LengthUtf8() > 18)
			{
				name = name.SubstringUtf8(0, 18);
				name += "...";
			}
		}
		m_PlayerName.SetText(name);		
		
		string version;
		g_Game.GetVersion(version);
		m_Version.SetText("#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")");
		
		if (m_DisplayedDlcHandler)
			m_DisplayedDlcHandler.UpdateAllPromotionInfo();
	}	
	
	override void OnShow()
	{
		#ifdef PLATFORM_MSSTORE
			#ifdef ENABLE_CAROUSEL
			// Hack: Recreate NewsCarousel to refresh it on menu show after applying options
			if (m_NewsCarousel)
			{
				m_NewsCarousel.Destroy();
				m_NewsCarousel = null;
				m_NewsCarousel = new NewsCarousel(m_NewsCarouselFrame, this);
			}
			#endif
		#endif

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
		layoutRoot.FindAnyWidget("ButtonHolderCredits").Show(g_Game.GetInput().IsEnabledMouseAndKeyboard());
		OnInputDeviceChanged(g_Game.GetInput().GetCurrentInputDevice());
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
		
		CheckWidth();
		
		if (g_Game.GetLoadState() != DayZGameState.CONNECTING && !g_Game.GetUIManager().IsDialogVisible())
		{
		#ifndef PLATFORM_CONSOLE
			if (GetUApi().GetInputByID(UAUIBack).LocalPress())
			{
				if (!g_Game.GetUIManager().IsDialogHiding())
					Exit();
			}
		#else
			if (GetUApi().GetInputByID(UAUIBack).LocalPress())
			{
				if (FeedbackDialogVisible())
				{
					ToggleFeedbackDialog();
				}
				else
				{
					EnterScriptedMenu(MENU_MAIN);
				}
			}

			if (GetUApi().GetInputByID(UAUICredits).LocalPress())
				OpenCredits();
		#endif
		}

		if (GetUApi().GetInputByID(UAUICtrlX).LocalPress())
		{
			if (CanStoreBeOpened())
				m_DisplayedDlcHandler.GetModInfo().GoToStore();
		}
		
		if (GetUApi().GetInputByID(UAUIThumbRight).LocalPress())
		{
			ToggleFeedbackDialog();
		}
		
		#ifdef ENABLE_CAROUSEL
		if (m_NewsCarousel)
		{
			if (GetUApi().GetInputByID(UAUICtrlX).LocalHold())
			{
				m_NewsCarousel.ShowPromotion();
			}
			
			if (GetUApi().GetInputByID(UAUIPadRight).LocalPress())
			{
				m_NewsCarousel.OnClickNextArticle();
			}
			
			if (GetUApi().GetInputByID(UAUIPadLeft).LocalPress())
			{
				m_NewsCarousel.OnClickPreviousArticle();
			}
			
			m_NewsCarousel.Update(timeslice);
		}
		#endif
	}
	
	protected void ToggleFeedbackDialog()
	{
		bool dialogVisible = FeedbackDialogVisible();
		m_DialogPanel.Show(!dialogVisible);
		m_MainMenuPanel.Show(dialogVisible);
		
		if (!dialogVisible)
		{
			PPERequesterBank.GetRequester(PPERequester_FeedbackBlur).Start();
		}
		else
		{
			PPERequesterBank.GetRequester(PPERequester_FeedbackBlur).Stop();
		}
		
		SetFocus(m_LastFocusedButton);
		UpdateControlsElements();
	}
	
	bool FeedbackDialogVisible()
	{
		return m_DialogPanel.IsVisible();
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
		BiosUserManager user_manager = g_Game.GetUserManager();
		if (user_manager)
		{
			g_Game.SetLoadState(DayZLoadState.MAIN_MENU_START);
			#ifndef PLATFORM_WINDOWS
			user_manager.SelectUserEx(null);
			#endif
			g_Game.GetUIManager().Back();
		}
	}
	
	void Exit()
	{
		g_Game.GetUIManager().ShowDialog("#main_menu_exit", "#main_menu_exit_desc", IDC_MAIN_QUIT, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
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
				g_Game.GetCallQueue(CALL_CATEGORY_GUI).Call(g_Game.RequestExit, IDC_MAIN_QUIT);
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
		string context;
		if (!FeedbackDialogVisible())
		{
			context = InputUtils.GetRichtextButtonIconFromInputAction("UAUICredits", "#menu_credits", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR);
			#ifndef PLATFORM_MSSTORE
			#ifndef PLATFORM_PS4
			#ifndef PLATFORM_XBOX
			context += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlY", "#layout_xbox_main_menu_toolbar_account", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
			#endif
			#endif
			#endif
			context += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", "#layout_xbox_main_menu_toolbar_select", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
			context += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIThumbRight", "#layout_main_menu_feedback", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		}
		else
		{
			context = InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#close", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR);
		}

		toolbar_text.SetText(context);
	}
	
	protected void UpdateControlsElementVisibility()
	{
		bool toolbarShow = false;
		#ifdef PLATFORM_CONSOLE
		toolbarShow = !g_Game.GetInput().IsEnabledMouseAndKeyboard() || g_Game.GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		#endif
		
		layoutRoot.FindAnyWidget("toolbar_bg").Show(toolbarShow);
	}
	
	void CheckWidth()
	{
		int w, h;
		ScreenWidthType widthType; 
		GetScreenSize(w, h);
		
		if(h > 0)
		{
			float ratio = w / h;
			if(ratio > 1.75)
				widthType = ScreenWidthType.WIDE;
			else if(ratio > 1.5)
				widthType = ScreenWidthType.MEDIUM;
			else
				widthType = ScreenWidthType.NARROW;
		}
		
		m_Width = w;
		m_Height = h;
		
		if (widthType != m_WidthType)
		{
			m_WidthType = widthType;
			#ifdef ENABLE_CAROUSEL
			if (m_NewsCarousel)
			{
				m_NewsCarousel.Destroy();
				m_NewsCarousel = null;
				m_NewsCarousel = new NewsCarousel(m_NewsCarouselFrame, this);
			}
			#endif
		}
	}
	
	//! DEPRICATED
	protected ref JsonDataDLCList 					m_DlcData;
	protected ref array<ref MainMenuDlcHandlerBase> m_DlcHandlers;
	protected ref MainMenuDlcHandlerBase 			m_DisplayedDlcHandler;
	protected ref array<ref ModInfo> 				m_AllDLCs;
	protected ref map<string, ref ModInfo> 			m_AllDlcsMap;
	
	[Obsolete("No replacement")]
	void PopulateDlcFrame()
	{
		if (!m_DlcHandlers)
			m_DlcHandlers = new array<ref MainMenuDlcHandlerBase>();
		else
		{
			// TODO: Would be better to update the parts that need updating instead of full recreation
			// Destroying and then reloading the same video is quite wasteful
			m_DlcHandlers.Clear();
		}
		
		m_DlcData = DlcDataLoader.GetData();
		int count = m_DlcData.DLCs.Count();
		JsonDataDLCInfo data;
		ModInfo info;
		
		for (int i = 0; i < count; i++)
		{
			data = m_DlcData.DLCs[i];
			info = m_AllDlcsMap.Get(data.Name);
			MainMenuDlcHandlerBase handler = new MainMenuDlcHandlerBase(info, m_DlcFrame, data);
			
			handler.ShowInfoPanel(true);
			m_DisplayedDlcHandler = handler;//TODO: carousel will take care of this later
			
			m_DlcHandlers.Insert(handler);
		}
	}
	
	[Obsolete("No replacement")]
	void FilterDLCs(inout array<ref ModInfo> modArray)
	{
		if (!m_AllDlcsMap)
			m_AllDlcsMap = new map<string,ref ModInfo>;
		
		m_AllDlcsMap.Clear();
		ModInfo info;
		int count = modArray.Count();
		for (int i = count - 1; i >= 0; i--)
		{
			info = modArray[i];
			if (!info.GetIsDLC())
				modArray.Remove(i);
			else
				m_AllDlcsMap.Set(info.GetName(), info);
		}
	}
}
