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
	
	void OptionsMenu()
	{
		
	}
	
	override Widget Init()
	{
		m_Options		= new GameOptions();
		
		#ifdef PLATFORM_XBOX
		layoutRoot		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/options/xbox/options_menu.layout", null);
		#else
		#ifdef PLATFORM_PS4
		layoutRoot		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/options/ps/options_menu.layout", null);
		#else
		#ifdef PLATFORM_WINDOWS
		layoutRoot		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/options/pc/options_menu.layout", null);
		#endif
		#endif
		#endif
		
		layoutRoot.FindAnyWidget("Tabber").GetScript(m_Tabber);
		
		m_Details		= layoutRoot.FindAnyWidget("settings_details");
		m_Version		= TextWidget.Cast(layoutRoot.FindAnyWidget("version"));
		
		m_GameTab		= new OptionsMenuGame(layoutRoot.FindAnyWidget("Tab_0"), m_Details, m_Options, this);
		m_SoundsTab		= new OptionsMenuSounds(layoutRoot.FindAnyWidget("Tab_1"), m_Details, m_Options, this);
		
		#ifdef PLATFORM_XBOX
		m_ControlsTab	= new OptionsMenuControls(layoutRoot.FindAnyWidget("Tab_2"), m_Details, m_Options, this);
		#else
		m_VideoTab		= new OptionsMenuVideo(layoutRoot.FindAnyWidget("Tab_2"), m_Details, m_Options, this);
		m_ControlsTab	= new OptionsMenuControls(layoutRoot.FindAnyWidget("Tab_3"), m_Details, m_Options, this);
		#endif
		
		m_Apply			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("apply"));
		m_Back			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("back"));
		m_Reset			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("reset"));
		m_Defaults		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("defaults"));
		
		m_ModalLock = false;
		m_CanApplyOrReset = false;
		m_CanToggle = false;
		
		string version;
		GetGame().GetVersion(version);
		#ifdef PLATFORM_CONSOLE
		version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		#else
		version = "#main_menu_version" + " " + version;
		#endif
		m_Version.SetText( version );
		
		#ifdef PLATFORM_WINDOWS
			SetFocus( layoutRoot );
		#else
			ToggleFocus();
		#endif
		
		m_Tabber.m_OnTabSwitch.Insert(OnTabSwitch);
		m_Tabber.m_OnAttemptTabSwitch.Insert(OnAttemptTabSwitch);
		
		GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		OnChanged();
		
		return layoutRoot;
	}
	
	void ~OptionsMenu()
	{
	}
	
	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		#endif
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		if ( button == MouseState.LEFT )
		{
			switch (w)
			{
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
	
	void OnTabSwitch( int tab )
	{
		switch ( tab )
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
		m_ControlsTab.Apply();
		
		//if ( m_GameTab.IsChanged() )
		{
			m_GameTab.Apply();
		}
		
		if ( m_Options.IsChanged() || m_GameTab.IsChanged() )
		{
			m_Options.Test();
			m_Options.Apply();
		}
		
		// save input configuration
		GetUApi().Export();
		
		if (GetGame().GetInput().IsEnabledMouseAndKeyboard()) //useless on consoles
		{
			m_Apply.SetFlags( WidgetFlags.IGNOREPOINTER );
			ColorDisable(m_Apply);
			m_Reset.SetFlags( WidgetFlags.IGNOREPOINTER );
			ColorDisable(m_Reset);
		}
		
		m_CanApplyOrReset = false;
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		#endif
		
		if ( m_Options.NeedRestart() )
			g_Game.GetUIManager().ShowDialog("#main_menu_configure", "#menu_restart_needed", 117, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
	}
	
	void Back()
	{
		if (!g_Game.GetUIManager().IsDialogVisible() && !g_Game.GetUIManager().IsModalVisible())
		{
			if (IsAnyTabChanged())
			{
				g_Game.GetUIManager().ShowDialog("#main_menu_configure", "#main_menu_configure_desc", 1337, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
			}
			else
			{
				m_Options.Revert();
				GetGame().EndOptionsVideo();
				GetGame().GetUIManager().Back();
			}
		}
	}
	
	void OnAttemptTabSwitch(int source, int target)
	{
		bool changed = IsAnyTabChanged();
		if ( changed )
		{
			if (!g_Game.GetUIManager().IsDialogVisible() && !g_Game.GetUIManager().IsModalVisible())
			{
				int id = target + DIALOG_TAB_OFFSET;
				g_Game.GetUIManager().ShowDialog("#main_menu_configure", "#main_menu_configure_desc", id, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
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
		bool changed = ( m_Options.IsChanged() || m_GameTab.IsChanged() || m_SoundsTab.IsChanged() || m_ControlsTab.IsChanged() );
		#ifndef PLATFORM_XBOX
		changed |= m_VideoTab.IsChanged();
		#endif
		
		return changed;
	}
	
	void OnChanged()
	{
		bool changed = IsAnyTabChanged();
		
		if (GetGame().GetInput().IsEnabledMouseAndKeyboard())
		{
			if ( changed )
			{
				m_Reset.ClearFlags( WidgetFlags.IGNOREPOINTER );
				ColorNormal( m_Reset );
				m_Apply.ClearFlags( WidgetFlags.IGNOREPOINTER );
				ColorNormal( m_Apply );
			}
			else
			{
				m_Apply.SetFlags( WidgetFlags.IGNOREPOINTER );
				ColorDisable( m_Apply );
				m_Reset.SetFlags( WidgetFlags.IGNOREPOINTER );
				ColorDisable( m_Reset );
			}
		}
		
		m_CanApplyOrReset = changed;
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
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
		
		if ( m_Options.IsChanged() )
			m_Options.Revert();
		
		if (GetGame().GetInput().IsEnabledMouseAndKeyboard())
		{
			m_Apply.SetFlags( WidgetFlags.IGNOREPOINTER );
			ColorDisable( m_Apply );
			m_Reset.SetFlags( WidgetFlags.IGNOREPOINTER );
			ColorDisable( m_Reset );
		}
		
		m_CanApplyOrReset = false;
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		#endif
	}
	
	void ResetCurrentTab()
	{
		if ( m_Options.IsChanged() )
		{
			m_Options.Revert();
		}
		
		switch ( m_ActiveTabIdx )
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
		
		if ( m_Options.IsChanged() )
		{
			m_Options.Revert();
		}
		
		if (GetGame().GetInput().IsEnabledMouseAndKeyboard())
		{
			m_Apply.SetFlags( WidgetFlags.IGNOREPOINTER );
			ColorDisable( m_Apply );
			m_Reset.SetFlags( WidgetFlags.IGNOREPOINTER );
			ColorDisable( m_Reset );
		}
		
		m_CanApplyOrReset = false;
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
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
	
	void PerformSetToDefaults()
	{
		switch ( m_ActiveTabIdx )
		{
			case 0:
			{
				m_GameTab.SetToDefaults();
				break;
			}
			case 1:
			{
				m_SoundsTab.SetToDefaults();
				break;
			}
			case 2:
			{
				#ifdef PLATFORM_XBOX
					m_ControlsTab.SetToDefaults();
				#else
					m_VideoTab.SetToDefaults();
				#endif
				break;
			}
			case 3:
			{
				#ifndef PLATFORM_XBOX
					m_ControlsTab.SetToDefaults();
				#endif
				break;
			}
		}
		
		if (GetGame().GetInput().IsEnabledMouseAndKeyboard())
		{
			m_Reset.ClearFlags( WidgetFlags.IGNOREPOINTER );
			ColorNormal( m_Reset );
			m_Apply.ClearFlags( WidgetFlags.IGNOREPOINTER );
			ColorNormal( m_Apply );
		}
		
		m_CanApplyOrReset = true;
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
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
		
		if ( m_GameTab )
			m_GameTab.SetOptions( m_Options );
		if ( m_SoundsTab )
			m_SoundsTab.SetOptions( m_Options );
		if ( m_ControlsTab )
			m_ControlsTab.SetOptions( m_Options );

		#ifndef PLATFORM_XBOX
			if ( m_VideoTab )
				m_VideoTab.SetOptions( m_Options );
		#endif
	}
	
	void ReloadVideoOptions()
	{
		#ifndef PLATFORM_XBOX
			if ( m_VideoTab )
				m_VideoTab.SetOptions( m_Options );
		#endif
	}
	
	override bool OnModalResult( Widget w, int x, int y, int code, int result )
	{
		bool ret = false;
		
		if ( code == 1337 )
		{
			if ( result == 2 )
			{
				m_Options.Revert();
				GetGame().EndOptionsVideo();
				GetGame().GetUIManager().Back();
			}
			ret = true;
		}
		else if ( code == 117 )
		{
			g_Game.RequestRestart(IDC_MAIN_QUIT);
		}
		else if ( code == MODAL_ID_DEFAULT )
		{
			if ( result == 2 )
			{
				PerformSetToDefaults();
			}
		}
		else if ( code >= DIALOG_TAB_OFFSET )
		{
			if ( result == 2 )
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
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if ( w && IsFocusable( w ) )
		{
			ColorHighlight( w );
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		if ( w && IsFocusable( w ) )
		{
			ColorNormal( w );
			return true;
		}
		return false;
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		if ( w && IsFocusable( w ) )
		{
			ColorHighlight( w );
			return true;
		}
		else if ( y == 1 )
		{
			SliderFocus();
		}
		else
		{
			ToggleFocus();
		}
		
		return false;
	}
	
	override bool OnFocusLost( Widget w, int x, int y )
	{
		if ( w && IsFocusable( w ) )
		{
			ColorNormal( w );
			return true;
		}
		return false;
	}
	
	bool IsFocusable( Widget w )
	{
		if ( w )
		{
			return ( w == m_Apply || w == m_Back || w == m_Reset || w == m_Defaults );
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
		
		#ifdef PLATFORM_CONSOLE
			layoutRoot.FindAnyWidget( "play_panel_root" ).Show( GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer() );
		#endif
	}
	
	override void OnShow()
	{
		super.OnShow();
		m_GameTab.Focus();
		Refresh();
	}
	
	override void Update( float timeslice )
	{
		super.Update( timeslice );
		
		if (m_ModalLock)
		{
			m_ModalLock = false;
			return;
		}
		
		if ( GetGame().GetInput().LocalPress("UAUITabLeft",false) )
		{
			m_Tabber.PreviousTab();
		}
		else if ( GetGame().GetInput().LocalPress("UAUITabRight",false) )
		{
			m_Tabber.NextTab();
		}
		else if ( GetGame().GetInput().LocalPress("UAUICtrlX",false) )
		{
			if (m_CanApplyOrReset)
			{
				Apply();
			}
		}
		else if ( GetGame().GetInput().LocalPress("UAUICredits",false) )
		{
			if (m_CanApplyOrReset)
			{
				ResetCurrentTab();
			}
			
		}
		else if ( GetGame().GetInput().LocalPress("UAUICtrlY",false) )
		{
			//SetToDefaults();
			PerformSetToDefaults();
		}
		else if ( GetGame().GetInput().LocalPress("UAUIBack",false) )
		{
			Back();
		}
	}
	
	//Coloring functions (Until WidgetStyles are useful)
	void ColorHighlight( Widget w )
	{
		if ( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			button.SetTextColor( ARGB( 255, 200, 0, 0 ) );
		}
		
		w.SetColor( ARGB( 255, 0, 0, 0) );
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_label" ) );
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text_1" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindAnyWidget( w.GetName() + "_image" ) );
		Widget option		= Widget.Cast( w.FindAnyWidget( w.GetName() + "_option_wrapper" ) );
		Widget option_label = w.FindAnyWidget( "option_label" );
		
		if ( text1 )
		{
			text1.SetColor( ARGB( 255, 255, 0, 0 ) );
		}
		
		if ( text2 )
		{
			text2.SetColor( ARGB( 255, 255, 0, 0 ) );
		}
		
		if ( text3 )
		{
			text3.SetColor( ARGB( 255, 255, 0, 0 ) );
			w.SetAlpha(1);
		}
		
		if ( image )
		{
			image.SetColor( ARGB( 255, 200, 0, 0 ) );
		}
		
		if ( option )
		{
			option.SetColor( ARGB( 255, 255, 0, 0 ) );
		}
		
		if ( option_label )
		{
			option_label.SetColor( ARGB( 255, 255, 0, 0 ) );
		}
	}
	
	void ColorNormal( Widget w )
	{
		if ( (w.GetFlags() & WidgetFlags.IGNOREPOINTER) == WidgetFlags.IGNOREPOINTER )
		{
			return;
		}
		
		if ( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			button.SetTextColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text_1" ) );
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_label" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindAnyWidget( w.GetName() + "_image" ) );
		Widget option		= w.FindAnyWidget( w.GetName() + "_option_wrapper" );
		Widget option_label = w.FindAnyWidget( "option_label" );
		
		if ( text1 )
		{
			text1.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if ( text2 )
		{
			text2.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if ( text3 )
		{
			text3.SetColor( ARGB( 255, 255, 255, 255 ) );
			w.SetAlpha(0);
		}
		
		if ( image )
		{
			image.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if ( option )
		{
			option.SetColor( ARGB( 150, 255, 255, 255 ) );
		}
		
		if ( option_label )
		{
			option_label.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
	}
	
	void ColorDisable( Widget w )
	{
		#ifdef PLATFORM_WINDOWS
		SetFocus( null );
		#endif
		
		if ( w )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			if ( button )
			{
				button.SetTextColor( ColorManager.COLOR_DISABLED_TEXT );
			}
		}
	}
	
	protected void UpdateControlsElements()
	{
		bool toolbarShow = !GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer();
		
		if (toolbarShow)
		{
			RichTextWidget toolbar_text = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ContextToolbarText"));
			string text = "";
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
			text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#STR_settings_menu_root_toolbar_bg_ConsoleToolbar_Back_BackText0", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
			toolbar_text.SetText(text);
		}
		else
		{
			RichTextWidget toolbar_b2	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon0"));
			RichTextWidget toolbar_x2	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("ApplyIcon0"));
			RichTextWidget toolbar_y2	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("ResetIcon0"));
			RichTextWidget toolbar_def2	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("DefaultIcon0"));
			toolbar_b2.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER));
			toolbar_x2.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlX", "", EUAINPUT_DEVICE_CONTROLLER));
			toolbar_y2.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUICredits", "", EUAINPUT_DEVICE_CONTROLLER));
			toolbar_def2.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlY", "", EUAINPUT_DEVICE_CONTROLLER));
		}
		
		layoutRoot.FindAnyWidget("toolbar_bg").Show(toolbarShow);
	}
}
