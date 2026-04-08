class OptionsMenu extends UIScriptedMenu
{
	const int MODAL_ID_DEFAULT = 100;
	const int DIALOG_TAB_OFFSET = 1400;

	protected TabberUI					m_Tabber;
	protected ref OptionsMenuGame		m_GameTab;
	protected ref OptionsMenuSounds		m_SoundsTab;
	protected ref OptionsMenuVideo		m_VideoTab;
	protected ref OptionsMenuControls	m_ControlsTab;
	
	protected ref GameOptions			m_Options;
	
	protected ButtonWidget				m_Apply;
	protected ButtonWidget				m_Back;
	protected ButtonWidget				m_Reset; //undo
	protected ButtonWidget				m_Defaults; //defaults
	
	protected Widget					m_Details;
	protected TextWidget				m_Version;
	
	protected int 						m_ActiveTabIdx = 0;
	protected bool 						m_ModalLock;
	protected bool 						m_CanApplyOrReset;
	protected bool 						m_CanToggle;
	
	#ifdef PLATFORM_MSSTORE
	protected ButtonWidget					m_GamepadControls;
	protected ButtonWidget					m_KeyboardBindings;
	#endif
	
	void OptionsMenu()
	{
		
	}
	
	override Widget Init()
	{
		m_Options		= new GameOptions();
		
		#ifdef PLATFORM_MSSTORE
			layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/options/msstore/options_menu.layout", null);
		#else
			#ifdef PLATFORM_XBOX
				layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/options/xbox/options_menu.layout", null);
			#else
				#ifdef PLATFORM_PS4
					layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/options/ps/options_menu.layout", null);
				#else
					#ifdef PLATFORM_WINDOWS
						layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/options/pc/options_menu.layout", null);
					#endif
				#endif
			#endif
		#endif
		
		layoutRoot.FindAnyWidget("Tabber").GetScript(m_Tabber);
		
		m_Details		= layoutRoot.FindAnyWidget("settings_details");
		m_Version		= TextWidget.Cast(layoutRoot.FindAnyWidget("version"));
		
		m_GameTab		= new OptionsMenuGame(layoutRoot.FindAnyWidget("Tab_0"), m_Details, m_Options, this);
		m_SoundsTab		= new OptionsMenuSounds(layoutRoot.FindAnyWidget("Tab_1"), m_Details, m_Options, this);
		
		#ifdef PLATFORM_MSSTORE
			m_VideoTab		= new OptionsMenuVideo(layoutRoot.FindAnyWidget("Tab_2"), m_Details, m_Options, this);
			m_ControlsTab	= new OptionsMenuControls(layoutRoot.FindAnyWidget("Tab_3"), m_Details, m_Options, this);
			m_GamepadControls = ButtonWidget.Cast(layoutRoot.FindAnyWidget("gamepad_controls"));
			m_KeyboardBindings = ButtonWidget.Cast(layoutRoot.FindAnyWidget("keyboard_bindings"));
		#else
			#ifdef PLATFORM_XBOX
				m_ControlsTab	= new OptionsMenuControls(layoutRoot.FindAnyWidget("Tab_2"), m_Details, m_Options, this);
			#else
				m_VideoTab		= new OptionsMenuVideo(layoutRoot.FindAnyWidget("Tab_2"), m_Details, m_Options, this);
				m_ControlsTab	= new OptionsMenuControls(layoutRoot.FindAnyWidget("Tab_3"), m_Details, m_Options, this);
			#endif
		#endif
		
		m_Apply			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("apply"));
		m_Back			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("back"));
		m_Reset			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("reset"));
		m_Defaults		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("defaults"));
		
		m_ModalLock = false;
		m_CanApplyOrReset = false;
		m_CanToggle = false;
		
		string version;
		g_Game.GetVersion(version);
		#ifdef PLATFORM_CONSOLE
		version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		#else
		version = "#main_menu_version" + " " + version;
		#endif
		m_Version.SetText(version);
		
		#ifdef PLATFORM_WINDOWS
		SetFocus(layoutRoot);
		#else
		ToggleFocus();
		#endif
		
		m_Tabber.m_OnTabSwitch.Insert(OnTabSwitch);
		m_Tabber.m_OnAttemptTabSwitch.Insert(OnAttemptTabSwitch);
		
		g_Game.GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		g_Game.GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		OnChanged();
		
		return layoutRoot;
	}
	
	void ~OptionsMenu()
	{
	}
	
	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElementVisibility();
		#endif
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		#ifdef PLATFORM_CONSOLE
		bool mk = g_Game.GetInput().IsEnabledMouseAndKeyboard();
		bool mkServer = g_Game.GetInput().IsEnabledMouseAndKeyboardEvenOnServer();
		
		switch (pInputDeviceType)
		{
		case EInputDeviceType.CONTROLLER:
			if (mk && mkServer)
			{
				g_Game.GetUIManager().ShowUICursor(false);
			}
		break;

		default:
			if (mk && mkServer)
			{
				g_Game.GetUIManager().ShowUICursor(true);
			}
		break;
		}
		
		UpdateControlsElementVisibility();
		#endif
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button == MouseState.LEFT)
		{
			switch (w)
			{
				#ifdef PLATFORM_MSSTORE
				case m_GamepadControls:
				{
					EnterScriptedMenu(MENU_XBOX_CONTROLS);
					return true;
				}

				case m_KeyboardBindings:
				{
					EnterScriptedMenu( MENU_KEYBINDINGS );
					return true;
				}
				#endif

				case m_Apply:
				{
					Apply();
					return true;
				}
				case m_Back:
				{
					Back();
					return true;
				}
				case m_Reset:
				{
					ResetCurrentTab();
					return true;
				}
				case m_Defaults:
				{
					//SetToDefaults();
					PerformSetToDefaults();
					return true;
				}
			}
		}
		return false;
	}
	
	void OnTabSwitch(int tab)
	{
		switch (tab)
		{
			case 0:
			{
				m_GameTab.Focus();
				break;
			}
			case 1:
			{
				m_SoundsTab.Focus();
				break;
			}
			case 2:
			{
				#ifdef PLATFORM_XBOX
				m_ControlsTab.Focus();
				#else
				m_VideoTab.Focus();
				#endif
				break;
			}
			case 3:
			{
				#ifndef PLATFORM_XBOX
				m_ControlsTab.Focus();
				#endif
				break;
			}
		}
		
		m_ActiveTabIdx = tab;
	}
	
	void Apply()
	{
		if (m_ControlsTab.IsChanged())
			m_ControlsTab.Apply();
	
		if (m_SoundsTab.IsChanged())
			m_SoundsTab.Apply();
		
		if (m_GameTab.IsChanged())
			m_GameTab.Apply();
		
		if (m_Options.IsChanged() || m_GameTab.IsChanged())
		{
			m_Options.Test();
			m_Options.Apply();
		}
		
		// save input configuration
		GetUApi().Export();
		
		if (g_Game.GetInput().IsEnabledMouseAndKeyboard()) //useless on consoles
		{
			m_Apply.SetFlags(WidgetFlags.IGNOREPOINTER);
			ColorDisable(m_Apply);
			m_Reset.SetFlags(WidgetFlags.IGNOREPOINTER);
			ColorDisable(m_Reset);
		}
		
		m_CanApplyOrReset = false;
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		UpdateControlsElementVisibility();
		
		IngameHud hud;
		if (g_Game.GetMission() && Class.CastTo(hud,g_Game.GetMission().GetHud()))
		{
			hud.ShowQuickBar(g_Game.GetInput().IsEnabledMouseAndKeyboardEvenOnServer());
		}
		#endif
		
		if (m_Options.NeedRestart())
			g_Game.GetUIManager().ShowDialog("#main_menu_configure", "#menu_restart_needed", 117, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
	}
	
	void Back()
	{
		if (!g_Game.GetUIManager().IsDialogVisible() && !g_Game.GetUIManager().IsModalVisible())
		{
			if (IsAnyTabChanged())
			{
				g_Game.GetUIManager().ShowDialog("#main_menu_configure", "#main_menu_configure_desc", 1337, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
				#ifdef PLATFORM_CONSOLE
				UpdateControlsElements();
				#endif
			}
			else
			{
				m_Options.Revert();
				g_Game.EndOptionsVideo();
				g_Game.GetUIManager().Back();
			}
		}
	}
	
	void OnAttemptTabSwitch(int source, int target)
	{
		bool changed = IsAnyTabChanged();
		if (changed)
		{
			if (!g_Game.GetUIManager().IsDialogVisible() && !g_Game.GetUIManager().IsModalVisible())
			{
				int id = target + DIALOG_TAB_OFFSET;
				g_Game.GetUIManager().ShowDialog("#main_menu_configure", "#main_menu_configure_desc", id, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
				#ifdef PLATFORM_CONSOLE
				UpdateControlsElements();
				#endif
			}
		}
		else
		{
			ResetCurrentTab();
		}
		
		m_Tabber.SetCanSwitch(!changed);
	}
	
	bool IsAnyTabChanged()
	{
		bool changed = (m_Options.IsChanged() || m_GameTab.IsChanged() || m_SoundsTab.IsChanged() || m_ControlsTab.IsChanged());
		#ifndef PLATFORM_XBOX
		changed |= m_VideoTab.IsChanged();
		#endif
		
		return changed;
	}
	
	void OnChanged()
	{
		bool changed = IsAnyTabChanged();
		
		if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
		{
			if (changed)
			{
				m_Reset.ClearFlags(WidgetFlags.IGNOREPOINTER);
				ColorNormal(m_Reset);
				m_Apply.ClearFlags(WidgetFlags.IGNOREPOINTER);
				ColorNormal(m_Apply);
			}
			else
			{
				m_Apply.SetFlags(WidgetFlags.IGNOREPOINTER);
				ColorDisable(m_Apply);
				m_Reset.SetFlags(WidgetFlags.IGNOREPOINTER);
				ColorDisable(m_Reset);
			}
		}
		
		m_CanApplyOrReset = changed;
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		UpdateControlsElementVisibility();
		#endif
		
		m_Tabber.AlignTabbers();
	}
	
	//resets it all
	void Reset()
	{
		m_Options.Revert();
		m_GameTab.Revert();
		m_SoundsTab.Revert();
		m_ControlsTab.Revert();
		#ifndef PLATFORM_XBOX
		m_VideoTab.Revert();
		#endif
		
		if (m_Options.IsChanged())
			m_Options.Revert();
		
		if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
		{
			m_Apply.SetFlags(WidgetFlags.IGNOREPOINTER);
			ColorDisable(m_Apply);
			m_Reset.SetFlags(WidgetFlags.IGNOREPOINTER);
			ColorDisable(m_Reset);
		}
		
		m_CanApplyOrReset = false;
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		UpdateControlsElementVisibility();
		#endif
	}
	
	void ResetCurrentTab()
	{
		if (m_Options.IsChanged())
		{
			m_Options.Revert();
		}
		
		switch (m_ActiveTabIdx)
		{
			case 0:
			{
				m_GameTab.Revert();
				break;
			}
			case 1:
			{
				m_SoundsTab.Revert();
				break;
			}
			case 2:
			{
				#ifdef PLATFORM_XBOX
					m_ControlsTab.Revert();
				#else
					m_VideoTab.Revert();
				#endif
				break;
			}
			case 3:
			{
				#ifndef PLATFORM_XBOX
					m_ControlsTab.Revert();
				#endif
				break;
			}
		}
		
		if (m_Options.IsChanged())
		{
			m_Options.Revert();
		}
		
		if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
		{
			m_Apply.SetFlags(WidgetFlags.IGNOREPOINTER);
			ColorDisable(m_Apply);
			m_Reset.SetFlags(WidgetFlags.IGNOREPOINTER);
			ColorDisable(m_Reset);
		}
		
		m_CanApplyOrReset = false;
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		UpdateControlsElementVisibility();
		#endif
		
		m_Tabber.AlignTabbers();
	}
	
	void SetToDefaults()
	{
		if (!g_Game.GetUIManager().IsDialogVisible() && !g_Game.GetUIManager().IsModalVisible())
		{
			g_Game.GetUIManager().ShowDialog("#menu_default_cap", "TODO - reset options to default", MODAL_ID_DEFAULT, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
		}
	}

	void FocusLastActiveTab()
	{
		switch (m_ActiveTabIdx)
		{
			case 0:
				m_GameTab.Focus();
				break;

			case 1:
				m_SoundsTab.Focus();
				break;

			case 2:
				#ifdef PLATFORM_XBOX
				m_ControlsTab.Focus();
				#else
				m_VideoTab.Focus();
				#endif
				break;

			case 3:
				#ifndef PLATFORM_XBOX
				m_ControlsTab.Focus();
				#endif
				break;
		}
	}
	
	void PerformSetToDefaults()
	{
		switch (m_ActiveTabIdx)
		{
			case 0:
				m_GameTab.SetToDefaults();
				break;

			case 1:
				m_SoundsTab.SetToDefaults();
				break;

			case 2:
				#ifdef PLATFORM_XBOX
				m_ControlsTab.SetToDefaults();
				#else
				m_VideoTab.SetToDefaults();
				#endif
				break;

			case 3:
				#ifndef PLATFORM_XBOX
				m_ControlsTab.SetToDefaults();
				#endif
				break;
		}
		
		if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
		{
			m_Reset.ClearFlags(WidgetFlags.IGNOREPOINTER);
			ColorNormal(m_Reset);
			m_Apply.ClearFlags(WidgetFlags.IGNOREPOINTER);
			ColorNormal(m_Apply);
		}
		
		m_CanApplyOrReset = true;
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		UpdateControlsElementVisibility();
		#endif
	}
	
	void SliderFocus()
	{
		#ifdef PLATFORM_CONSOLE
		m_CanToggle = false;
		UpdateControlsElements();
		#endif
	}
	
	void ToggleFocus()
	{
		#ifdef PLATFORM_CONSOLE
		m_CanToggle = true;
		UpdateControlsElements();
		#endif
	}
	
	//! Controls visibility and sometimes even state of specific, dependent options across sub-menus
	void ToggleDependentOptions(int mode, bool state)
	{
		m_GameTab.ToggleDependentOptions(mode,state);
		m_SoundsTab.ToggleDependentOptions(mode,state);
		m_ControlsTab.ToggleDependentOptions(mode,state);
		#ifndef PLATFORM_XBOX
		m_VideoTab.ToggleDependentOptions(mode,state);
		#endif
	}
	
	void ReloadOptions()
	{
		m_Options = new GameOptions();
		
		if (m_GameTab)
			m_GameTab.SetOptions(m_Options);
		if (m_SoundsTab)
			m_SoundsTab.SetOptions(m_Options);
		if (m_ControlsTab)
			m_ControlsTab.SetOptions(m_Options);

		#ifndef PLATFORM_XBOX
		if (m_VideoTab)
			m_VideoTab.SetOptions(m_Options);
		#endif
	}
	
	void ReloadVideoOptions()
	{
		#ifndef PLATFORM_XBOX
		if (m_VideoTab)
			m_VideoTab.SetOptions(m_Options);
		#endif
	}
	
	override bool OnModalResult(Widget w, int x, int y, int code, int result)
	{
		bool ret = false;
		
		if (code == 1337)
		{
			if (result == 2)
			{
				m_Options.Revert();
				g_Game.EndOptionsVideo();
				g_Game.GetUIManager().Back();
			}
			ret = true;
		}
		else if (code == 117)
		{
			g_Game.RequestRestart(IDC_MAIN_QUIT);
		}
		else if (code == MODAL_ID_DEFAULT)
		{
			if (result == 2)
			{
				PerformSetToDefaults();
			}
		}
		else if (code >= DIALOG_TAB_OFFSET)
		{
			if (result == 2)
			{
				int id = code - DIALOG_TAB_OFFSET;
				//m_Options.Revert();
				ResetCurrentTab();
				m_Tabber.PerformSwitchTab(id);
			}
			ret = true;
		}
		
		m_ModalLock = ret; //prevents dialog being shown on the next update
		return ret;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w && IsFocusable(w))
		{
			ColorHighlight(w);
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w && IsFocusable(w))
		{
			ColorNormal(w);
			return true;
		}
		return false;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		if (w && IsFocusable(w))
		{
			ColorHighlight(w);
			return true;
		}
		else if (y == 1)
		{
			SliderFocus();
		}
		else
		{
			ToggleFocus();
		}
		
		return false;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (w && IsFocusable(w))
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
			return (w == m_Apply || w == m_Back || w == m_Reset || w == m_Defaults);
		}
		return false;
	}
	
	override void Refresh()
	{
		string version;
		g_Game.GetVersion(version);
		#ifdef PLATFORM_CONSOLE
		version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		#else
		version = "#main_menu_version" + " " + version;
		#endif
		
		m_Version.SetText(version);
		
		#ifdef PLATFORM_CONSOLE
		OnInputDeviceChanged(g_Game.GetInput().GetCurrentInputDevice());
		UpdateControlsElementVisibility();
		#endif
	}
	
	override void OnShow()
	{
		super.OnShow();
		#ifdef PLATFORM_MSSTORE
	 	// MSStore: When going back from keybindings/gamepad controls, game tab somehow gained focus.
		// Focus last active tab
		FocusLastActiveTab();
		#else
		m_GameTab.Focus();
		#endif
		Refresh();
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		if (m_ModalLock)
		{
			m_ModalLock = false;
			#ifdef PLATFORM_CONSOLE
			UpdateControlsElements();
			#endif
			return;
		}
		
		if (g_Game.GetUIManager().IsDialogVisible())
		{
			return;
		}
		
		if (GetUApi().GetInputByID(UAUITabLeft).LocalPress())
		{
			m_Tabber.PreviousTab();
		}
		else if (GetUApi().GetInputByID(UAUITabRight).LocalPress())
		{
			m_Tabber.NextTab();
		}
		else if (GetUApi().GetInputByID(UAUICtrlX).LocalPress())
		{
			if (m_CanApplyOrReset)
			{
				Apply();
			}
		}
		else if (GetUApi().GetInputByID(UAUICredits).LocalPress())
		{
			if (m_CanApplyOrReset)
			{
				ResetCurrentTab();
			}
			
		}
		else if (GetUApi().GetInputByID(UAUICtrlY).LocalPress())
		{
			PerformSetToDefaults();
		}
		else if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			Back();
		}
	}
	
	//Coloring functions (Until WidgetStyles are useful)
	void ColorHighlight(Widget w)
	{
		if ((w.GetFlags() & WidgetFlags.IGNOREPOINTER) == WidgetFlags.IGNOREPOINTER)
		{
			return;
		}

		if (w.IsInherited(ButtonWidget))
		{
			ButtonWidget button = ButtonWidget.Cast(w);
			button.SetTextColor(ARGB(255, 200, 0, 0));
		}
		
		w.SetColor(ARGB(255, 0, 0, 0));
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text"));
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text_1"));
		ImageWidget image	= ImageWidget.Cast(w.FindAnyWidget(w.GetName() + "_image"));
		Widget option		= Widget.Cast(w.FindAnyWidget(w.GetName() + "_option_wrapper"));
		Widget option_label = w.FindAnyWidget("option_label");
		
		if (text1)
		{
			text1.SetColor(ARGB(255, 255, 0, 0));
		}
		
		if (text2)
		{
			text2.SetColor(ARGB(255, 255, 0, 0));
		}
		
		if (text3)
		{
			text3.SetColor(ARGB(255, 255, 0, 0));
			w.SetAlpha(1);
		}
		
		if (image)
		{
			image.SetColor(ARGB(255, 200, 0, 0));
		}
		
		if (option)
		{
			option.SetColor(ARGB(255, 255, 0, 0));
		}
		
		if (option_label)
		{
			option_label.SetColor(ARGB(255, 255, 0, 0));
		}
	}
	
	void ColorNormal(Widget w)
	{
		if ((w.GetFlags() & WidgetFlags.IGNOREPOINTER) == WidgetFlags.IGNOREPOINTER)
		{
			return;
		}
		
		if (w.IsInherited(ButtonWidget))
		{
			ButtonWidget button = ButtonWidget.Cast(w);
			button.SetTextColor(ARGB(255, 255, 255, 255));
		}
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text"));
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text_1"));
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		ImageWidget image	= ImageWidget.Cast(w.FindAnyWidget(w.GetName() + "_image"));
		Widget option		= w.FindAnyWidget(w.GetName() + "_option_wrapper");
		Widget option_label = w.FindAnyWidget("option_label");
		
		if (text1)
		{
			text1.SetColor(ARGB(255, 255, 255, 255));
		}
		
		if (text2)
		{
			text2.SetColor(ARGB(255, 255, 255, 255));
		}
		
		if (text3)
		{
			text3.SetColor(ARGB(255, 255, 255, 255));
			w.SetAlpha(0);
		}
		
		if (image)
		{
			image.SetColor(ARGB(255, 255, 255, 255));
		}
		
		if (option)
		{
			option.SetColor(ARGB(150, 255, 255, 255));
		}
		
		if (option_label)
		{
			option_label.SetColor(ARGB(255, 255, 255, 255));
		}
	}
	
	void ColorDisable(Widget w)
	{
		#ifdef PLATFORM_WINDOWS
		SetFocus(null);
		#endif
		
		if (w)
		{
			ButtonWidget button = ButtonWidget.Cast(w);
			if (button)
			{
				button.SetTextColor(ColorManager.COLOR_DISABLED_TEXT);
			}
		}
	}
	
	protected void UpdateControlsElements()
	{
		#ifdef PLATFORM_CONSOLE
		RichTextWidget toolbar_text = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ContextToolbarText"));
		string text = "";
		if (g_Game.GetUIManager().IsDialogVisible() || g_Game.GetUIManager().IsDialogQueued())
		{
			text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", "#dialog_confirm", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
			
		}
		else
		{
			if (m_CanToggle)
			{
				text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", "#dialog_change", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
			}
			if (m_CanApplyOrReset)
			{
				text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlX", "#STR_settings_menu_root_toolbar_bg_ConsoleToolbar_Apply_ApplyText0", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
			}
			text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlY", "#menu_default", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
			if (m_CanApplyOrReset)
			{
				text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUICredits", "#menu_undo", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
			}
		}
		text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#STR_settings_menu_root_toolbar_bg_ConsoleToolbar_Back_BackText0", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		toolbar_text.SetText(text);
		#endif
	}
	
	protected void UpdateControlsElementVisibility()
	{
		bool toolbarShow = false;
		#ifdef PLATFORM_CONSOLE
		toolbarShow = !g_Game.GetInput().IsEnabledMouseAndKeyboardEvenOnServer() || g_Game.GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		#endif
		
		layoutRoot.FindAnyWidget("toolbar_bg").Show(toolbarShow);
		layoutRoot.FindAnyWidget("play_panel_root").Show(!toolbarShow);
	}
}
