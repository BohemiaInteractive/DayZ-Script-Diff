class KeybindingsMenu extends UIScriptedMenu
{
	protected TabberUI							m_Tabber;
	protected ref DropdownPrefab				m_KBDropdown; //DEPRECATED
	protected ref OptionSelectorMultistate		m_PresetSelector;
	protected ref KeybindingsContainer 			m_GroupsContainer;
	protected ref array<ref KeybindingsGroup>	m_Tabs; //DEPRECATED
	
	protected TextWidget						m_Version;
	protected ButtonWidget						m_Apply;
	protected ButtonWidget						m_Back;
	protected ButtonWidget						m_Undo;
	protected ButtonWidget						m_Defaults;
	
	protected int								m_CurrentSettingKeyIndex = -1;
	protected int								m_CurrentSettingAlternateKeyIndex = -1;
	protected int								m_OriginalPresetIndex;
	protected int								m_TargetPresetIndex;
	protected ref array<int>					m_SetKeybinds;
	
	const int MODAL_ID_BACK = 1337;
	const int MODAL_ID_DEFAULT = 100;
	const int MODAL_ID_PRESET_CHANGE = 200;
	
	override Widget Init()
	{
		Input input = GetGame().GetInput();
		layoutRoot			= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/options/pc/keybinding_menu.layout", null );
		
		m_Version			= TextWidget.Cast( layoutRoot.FindAnyWidget( "version" ) );
		m_Apply				= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "apply" ) );
		m_Back				= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "back" ) );
		m_Undo				= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "reset" ) );
		m_Defaults			= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "defaults" ) );
		
		layoutRoot.FindAnyWidget( "Tabber" ).GetScript( m_Tabber );
		
		string version;
		GetGame().GetVersion( version );
		#ifdef PLATFORM_CONSOLE
			version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		#else
			version = "#main_menu_version" + " " + version;
		#endif
		m_Version.SetText( version );

		#ifdef PLATFORM_PS4
			string back = "circle";
			if( GetGame().GetInput().GetEnterButton() == GamepadButton.A )
			{
				back = "circle";
			}
			else
			{
				back = "cross";
			}
			ImageWidget toolbar_b = layoutRoot.FindAnyWidget( "BackIcon" );
			toolbar_b.LoadImageFile( 0, "set:playstation_buttons image:" + back );
		#endif
		
		InitInputSortingMap();
		CreateTabs();
		CreateGroupContainer();
		
		InitPresets( -1, layoutRoot.FindAnyWidget( "group_header" ), input );
		m_Tabber.m_OnTabSwitch.Insert(UpdateTabContent);
		m_Tabber.SelectTabControl( 0 );
		m_Tabber.SelectTabPanel( 0 );
		g_Game.SetKeyboardHandle( this );
		m_Tabber.RefreshTab(true);
		
		ColorDisabled( m_Apply );
		m_Apply.SetFlags( WidgetFlags.IGNOREPOINTER );
		ColorDisabled( m_Undo );
		m_Undo.SetFlags( WidgetFlags.IGNOREPOINTER );
		ColorWhite( m_Defaults, null );
		m_Defaults.ClearFlags( WidgetFlags.IGNOREPOINTER );
		
		return layoutRoot;
	}
	
	void CreateTabs()
	{
		int sort_count = InputUtils.GetInputActionSortingMap().Count();
		for (int i = 0; i < sort_count; i++)
		{
			if (InputUtils.GetInputActionSortingMap().GetElement(i) && InputUtils.GetInputActionSortingMap().GetElement(i).Count() > 0)
			{
				string group_name = GetUApi().SortingLocalization(InputUtils.GetInputActionSortingMap().GetKey(i));
				group_name = Widget.TranslateString("#" + group_name); //oof
				m_Tabber.AddTab( group_name );
			}
		}
		
		if (InputUtils.GetUnsortedInputActions() && InputUtils.GetUnsortedInputActions().Count() > 0)
		{
			m_Tabber.AddTab( Widget.TranslateString("#layout_pc_keybinding_unsorted") );
		}
		m_Tabber.DisableTabs(true);
	}
	
	void CreateGroupContainer()
	{
		m_GroupsContainer = new KeybindingsContainer(-1,GetGame().GetInput(),layoutRoot.FindAnyWidget("TabContentsHolder"),this);
	}
	
	void UpdateTabContent( int tab_index )
	{
		m_GroupsContainer.SwitchSubgroup(tab_index);
	}
	
	void ClearKeybind( int key_index )
	{
		ColorWhite( m_Apply, null );
		m_Apply.ClearFlags( WidgetFlags.IGNOREPOINTER );
		ColorWhite( m_Undo, null );
		m_Undo.ClearFlags( WidgetFlags.IGNOREPOINTER );
	}
	
	void ClearAlternativeKeybind( int key_index )
	{
		ColorWhite( m_Apply, null );
		m_Apply.ClearFlags( WidgetFlags.IGNOREPOINTER );
		ColorWhite( m_Undo, null );
		m_Undo.ClearFlags( WidgetFlags.IGNOREPOINTER );
	}
	
	void StartEnteringKeybind( int key_index )
	{
		m_CurrentSettingAlternateKeyIndex	= -1;
		m_CurrentSettingKeyIndex			= key_index;
	}
	
	void CancelEnteringKeybind()
	{
		m_GroupsContainer.CancelEnteringKeybind();
		m_CurrentSettingKeyIndex = -1;
	}
	
	void StartEnteringAlternateKeybind( int key_index )
	{
		m_CurrentSettingKeyIndex			= -1;
		m_CurrentSettingAlternateKeyIndex	= key_index;
	}
	
	void CancelEnteringAlternateKeybind()
	{
		m_GroupsContainer.CancelEnteringAlternateKeybind();
		m_CurrentSettingAlternateKeyIndex = -1;
	}
	
	void ConfirmKeybindEntry( TIntArray new_keys )
	{
		m_CurrentSettingKeyIndex = -1;
		ColorWhite( m_Apply, null );
		m_Apply.ClearFlags( WidgetFlags.IGNOREPOINTER );
		ColorWhite( m_Undo, null );
		m_Undo.ClearFlags( WidgetFlags.IGNOREPOINTER );
	}
	
	void ConfirmAlternateKeybindEntry( TIntArray new_keys )
	{
		m_CurrentSettingAlternateKeyIndex = -1;
		ColorWhite( m_Apply, null );
		m_Apply.ClearFlags( WidgetFlags.IGNOREPOINTER );
		ColorWhite( m_Undo, null );
		m_Undo.ClearFlags( WidgetFlags.IGNOREPOINTER );
	}
	
	override void Update(float timeslice)
	{
		if( GetGame().GetInput().LocalPress("UAUIBack",false) )
		{
			Back();
		}
		
		if( m_GroupsContainer )
		{
			m_GroupsContainer.Update( timeslice );
		}
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		if( button == MouseState.LEFT )
		{
			if( w == m_Apply )
			{
				Apply();
				return true;
			}
			else if( w == m_Back )
			{
				Back();
				return true;
			}
			else if( w == m_Undo )
			{
				Reset();
				return true;
			}
			else if( w == m_Defaults )
			{
				SetToDefaults();
				return true;
			}
		}
		return false;
	}
	
	void Apply()
	{
		ColorDisabled( m_Apply );
		m_Apply.SetFlags( WidgetFlags.IGNOREPOINTER );
		ColorDisabled( m_Undo );
		m_Undo.SetFlags( WidgetFlags.IGNOREPOINTER );
		ColorWhite( m_Defaults, null );
		m_Defaults.ClearFlags( WidgetFlags.IGNOREPOINTER );
		
		m_GroupsContainer.Apply();
		
		// save input configuration
		GetUApi().Export();
	}
	
	void Back()
	{
		if( m_CurrentSettingKeyIndex != -1 )
		{
			CancelEnteringKeybind();
			return;
		}
		
		if( m_CurrentSettingAlternateKeyIndex != -1 )
		{
			CancelEnteringAlternateKeybind();
			return;
		}
		
		bool changed = m_GroupsContainer.IsChanged();

		if ( changed )
		{
			g_Game.GetUIManager().ShowDialog("#main_menu_configure", "#main_menu_configure_desc", MODAL_ID_BACK, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
		}
		else
		{
			GetGame().GetUIManager().Back();
		}
	}
	
	//! Undoes the unsaved changes and reverts to previous state. Does not reset to defaults!
	void Reset()
	{
		ColorDisabled( m_Apply );
		m_Apply.SetFlags( WidgetFlags.IGNOREPOINTER );
		ColorDisabled( m_Undo );
		m_Undo.SetFlags( WidgetFlags.IGNOREPOINTER );
		
		m_GroupsContainer.Reset();
	}
	
	void SetToDefaults()
	{
		g_Game.GetUIManager().ShowDialog("#menu_default_cap", "#menu_default_desc", MODAL_ID_DEFAULT, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
	}
	
	void PerformSetToDefaults()
	{
		ColorDisabled( m_Apply );
		m_Apply.SetFlags( WidgetFlags.IGNOREPOINTER );
		ColorDisabled( m_Undo );
		m_Undo.SetFlags( WidgetFlags.IGNOREPOINTER );
		ColorDisabled( m_Defaults );
		m_Defaults.SetFlags( WidgetFlags.IGNOREPOINTER );
		
		GetUApi().Revert();
		
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetMission().RefreshExcludes);
		
		m_GroupsContainer.Reset(true);
	}
	
	void DeferredDefaultsInit()
	{
		//GetGame().GetMission().RefreshExcludes();
	}
	
	override bool OnModalResult( Widget w, int x, int y, int code, int result )
	{
		if( code == MODAL_ID_BACK )
		{
			if( result == 2 )
			{
				Reset();
				GetGame().GetUIManager().Back();
			}
			return true;
		}
		else if (code == MODAL_ID_DEFAULT)
		{
			if( result == 2 )
			{
				PerformSetToDefaults();
			}
			return true;
		}
		else if (code == MODAL_ID_PRESET_CHANGE)
		{
			if( result == 2 )
			{
				Reset();
				m_PresetSelector.PerformSetOption(m_TargetPresetIndex);
			}
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
	}
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if( w && IsFocusable( w ) )
		{
			ColorRed( w );
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		if( w && IsFocusable( w ) )
		{
			if ( (w.GetFlags() & WidgetFlags.DISABLED) || (w.GetFlags() & WidgetFlags.IGNOREPOINTER) )
			{
				ColorDisabled(w);
			}
			else
			{
				ColorWhite( w, enterW );
			}
			return true;
		}
		return false;
	}
	
	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		return super.OnMouseWheel(w, x, y, wheel);
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		if( w && IsFocusable( w ) )
		{
			ColorRed( w );
			return true;
		}
		return false;
	}
	
	override bool OnFocusLost( Widget w, int x, int y )
	{
		if( w && IsFocusable( w ) )
		{
			if ( (w.GetFlags() & WidgetFlags.DISABLED) || (w.GetFlags() & WidgetFlags.IGNOREPOINTER) )
			{
				ColorDisabled(w);
			}
			else
			{
				ColorWhite( w, null );
			}
			return true;
		}
		return false;
	}
	
	bool IsFocusable( Widget w )
	{
		if( w )
		{
			return ( w == m_Apply || w == m_Back || w == m_Undo || w == m_Defaults );
		}
		return false;
	}
	
	//Coloring functions (Until WidgetStyles are useful)
	void ColorRed( Widget w )
	{
		SetFocus( w );
		
		ButtonWidget button = ButtonWidget.Cast( w );
		if( button && button != m_Apply )
		{
			button.SetTextColor( ARGB( 255, 200, 0, 0 ) );
		}
	}
	
	void ColorWhite( Widget w, Widget enterW )
	{
		#ifdef PLATFORM_WINDOWS
		SetFocus( null );
		#endif
		
		ButtonWidget button = ButtonWidget.Cast( w );
		if ( button )
		{
			if ( button.GetFlags() & WidgetFlags.DISABLED )
			{
				button.SetTextColor(ColorManager.COLOR_DISABLED_TEXT);
			}
			else
			{
				button.SetTextColor(ColorManager.COLOR_NORMAL_TEXT);
			}
		}
	}
	
	void ColorDisabled( Widget w )
	{
		#ifdef PLATFORM_WINDOWS
		SetFocus( null );
		#endif
		
		ButtonWidget button = ButtonWidget.Cast(w);
		if ( button )
		{
			button.SetTextColor(ColorManager.COLOR_DISABLED_TEXT);
		}
	}
	
	protected void InitInputSortingMap()
	{
		InputUtils.InitInputMetadata();
	}
	
	void InitPresets( int index, Widget parent, Input input )
	{
		Widget kb_root = parent.FindAnyWidget( "keyboard_dropown" );
		if (kb_root)
		{
			kb_root.Show(false);
		}
		
		array<string> opt1			= new array<string>;
		string profile_text;
		
		for (int i = 0; i < input.GetProfilesCount(); i++)
		{
			input.GetProfileName( i, profile_text );
			opt1.Insert( profile_text );
		}
		
		int current_idx = input.GetCurrentProfile();
		m_OriginalPresetIndex = current_idx;
		m_PresetSelector = new OptionSelectorMultistate( layoutRoot.FindAnyWidget( "profile_setting_option" ), current_idx, null, false, opt1 );
		m_PresetSelector.m_AttemptOptionChange.Insert( OnAttemptSelectPreset );
		m_PresetSelector.m_OptionChanged.Insert( OnSelectKBPreset );
	}
	
	void OnAttemptSelectPreset( int index )
	{
		bool changed = m_GroupsContainer.IsChanged() && m_OriginalPresetIndex != index;
		m_TargetPresetIndex = index;
		
		if (changed)
		{
			g_Game.GetUIManager().ShowDialog("#main_menu_configure", "#main_menu_configure_desc", MODAL_ID_PRESET_CHANGE, DBT_YESNO, DBB_YES, DMT_QUESTION, this);
		}
		
		m_PresetSelector.SetCanSwitch(!changed);
	}
	
	void OnSelectKBPreset( int index )
	{
		m_OriginalPresetIndex = index;
		m_GroupsContainer.OnSelectKBPreset(index);
		string profile_text;
		GetGame().GetInput().GetProfileName( index, profile_text );
	}
	
//////////////////////////////////////////////////
// 				OBSOLETE METHODS 				//
//////////////////////////////////////////////////
	KeybindingsContainer GetCurrentTab()
	{
		return m_GroupsContainer;
	}
	
	void AddGroup( int index, Input input )
	{
	}
}