class TabberUI extends ScriptedWidgetEventHandler
{
	protected bool					m_FirstInit = true;
	protected Widget				m_Root;
	protected Widget				m_TabControlsRoot;
	
	protected int 					m_TabsCount;
	protected ref map<int, Widget>	m_TabControls;
	protected ref map<int, Widget>	m_Tabs;
	
	protected int					m_SelectedIndex;
	protected float					m_ResolutionMultiplier;
	protected bool					m_CanSwitch;
	
	ref ScriptInvoker				m_OnTabSwitch = new ScriptInvoker();
	ref ScriptInvoker				m_OnAttemptTabSwitch = new ScriptInvoker();
	ref Timer						m_InitTimer;
	
	protected void OnInputPresetChanged()
	{
		Init();

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
			m_TabControlsRoot.FindAnyWidget("ConsoleControls").Show(true);
		break;

		default:
			if (GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
			{
				m_TabControlsRoot.FindAnyWidget("ConsoleControls").Show(false);
			}
		break;
		}
	}
	
	void Init()
	{
		int x, y;
		GetScreenSize( x, y );
		m_ResolutionMultiplier	= y / 1080;
		m_CanSwitch = true;
		m_TabsCount	= 0;
		
		Widget tab_controls	= m_Root.FindAnyWidget("Tab_Control_Container");
		if ( tab_controls )
		{
			Widget tab_child	= tab_controls.GetChildren();
			
			while ( tab_child )
			{
				m_TabsCount++;
				tab_child = tab_child.GetSibling();
			}
			
			for ( int i = 0; i < m_TabsCount; i++ )
			{
				Widget tab_control	= tab_controls.FindAnyWidget("Tab_Control_" + i);
				Widget tab_widget	= m_Root.FindAnyWidget("Tab_" + i);
				if (tab_control && tab_widget)
				{
					tab_control.SetHandler(this);
					m_TabControls.Insert(i, tab_control);
					m_Tabs.Insert(i, tab_widget);
				}
				else
				{
					Error("Tabber could not find correctly named tab or control at index " + i);
				}
			}
			
			AlignTabbers();
			#ifdef PLATFORM_CONSOLE
			UpdateControlsElements();
			#endif
			
			SelectTabControl(0);
			
			m_InitTimer.Run(0.01, this, "AlignTabbers");
		}
		
		GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		
		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());
	}

	void OnWidgetScriptInit( Widget w )
	{
		m_TabsCount				= 0;
		m_TabControls			= new map<int, Widget>;
		m_Tabs					= new map<int, Widget>;
		
		m_Root					= w;
		m_InitTimer				= new Timer();
		m_TabControlsRoot		= m_Root.FindAnyWidget("TabControls");
		
		Init();
	}
	
	void AlignTabbers()
	{
		float total_size;
		float x, y;
		
		Widget tab_controls_container = m_TabControlsRoot.FindAnyWidget( "Tab_Control_Container" );
		Widget tab_controls_scroller = m_TabControlsRoot.FindAnyWidget( "Tab_Control_Scroller" );
		
		m_TabControlsRoot.Update();
		tab_controls_container.Update();
		
		Widget tab_child = tab_controls_container.GetChildren();
		while ( tab_child )
		{
			if ( tab_child.IsVisible() )
			{
				TextWidget tab_text = TextWidget.Cast( tab_child.FindAnyWidget( tab_child.GetName() + "_Title" ) );
				int t_x, t_y;
				tab_text.Update();
				tab_text.GetTextSize( t_x, t_y );
				tab_child.SetSize( t_x + 10 * m_ResolutionMultiplier, 1 );
				tab_controls_container.Update();
				
				total_size += ( t_x + 10 * m_ResolutionMultiplier );
			}
			
			tab_child = tab_child.GetSibling();
		}
		
		tab_child = tab_controls_container.GetChildren();
		
		float x_f_c, y_f_c;
		tab_controls_container.GetScreenPos( x_f_c, y_f_c );
		
		while ( tab_child )
		{
			Widget tab_bg = tab_child.FindAnyWidget( tab_child.GetName() + "_Background" );
			tab_child.GetScreenPos( x, y );
			tab_bg.SetPos( ( x_f_c - x ), 0 );
			tab_bg.SetSize( total_size, 1 );
			
			tab_child = tab_child.GetSibling();
		}
		
		m_TabControlsRoot.GetSize( x, y );
		
		m_TabControlsRoot.SetSize( total_size, y );
		tab_controls_container.Update();
		if (tab_controls_scroller)
			tab_controls_scroller.Update();
		m_TabControlsRoot.Update();
	}
	
	int AddTab( string name )
	{
		int new_index = m_Tabs.Count();
		Widget tab = GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/tabber_prefab/tab.layout", m_Root );
		Widget control = GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/tabber_prefab/tab_control.layout", m_Root.FindAnyWidget( "Tab_Control_Container" ) );
		TextWidget control_text = TextWidget.Cast( control.FindAnyWidget( "Tab_Control_x_Title" ) );
		
		tab.SetName( "Tab_" + new_index );
		control.SetName( "Tab_Control_" + new_index );
		control_text.SetName( "Tab_Control_" + new_index + "_Title" );
		control.FindAnyWidget( "Tab_Control_x_Background" ).SetName( "Tab_Control_" + new_index + "_Background" );
		
		control_text.SetText( name );
		
		control.SetHandler( this );
		m_TabControls.Insert( new_index, control );
		m_Tabs.Insert( new_index, tab );
		
		AlignTabbers();
		
		return new_index;
	}
	
	void RemoveTab( int index )
	{

	}
	
	
	Widget GetTab( int index )
	{
		return m_Tabs.Get( index );
	}
	
	int GetTabCount()
	{
		return m_Tabs.Count();
	}
	
	bool CanSwitchTab()
	{
		return m_CanSwitch;
	}
	
	void SetCanSwitch(bool value)
	{
		m_CanSwitch = value;
	}
	
	void PerformSwitchTab(int index)
	{
		DeselectTabControl( m_SelectedIndex );
		DeselectTabPanel( m_SelectedIndex );
		
		SelectTabControl( index );
		SelectTabPanel( index );
		
		m_SelectedIndex = index;
		m_OnTabSwitch.Invoke( m_SelectedIndex );
		
		if ( m_FirstInit )
		{
			AlignTabbers();
			m_FirstInit = false;
		}
	}
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		int index = m_TabControls.GetKeyByValue( w );
		if ( m_SelectedIndex == index )
		{
			return false;
		}
		
		Widget tab_control = m_TabControls.Get( index );
		if ( tab_control )
		{			
			Widget tab_title = TextWidget.Cast(tab_control.FindAnyWidget( tab_control.GetName() + "_Title" ));
			tab_title.SetColor( ARGB(255, 255, 0, 0) );
			tab_control.SetColor( ARGB(255, 0, 0 ,0) );
		}
		
		return false;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		int index = m_TabControls.GetKeyByValue( w );
		if ( m_SelectedIndex == index )
		{
			return false;
		}
		
		Widget tab_control = m_TabControls.Get( index );
		if ( tab_control )
		{			
			Widget tab_title = TextWidget.Cast(tab_control.FindAnyWidget( tab_control.GetName() + "_Title" ));
			tab_title.SetColor( ARGB(255, 255, 255, 255) );
			tab_control.SetColor( ARGB(0, 0, 0 ,0) );
		}
		return false;
	}
	
	override bool OnMouseButtonUp( Widget w, int x, int y, int button )
	{
		if ( button == MouseState.LEFT )
		{
			int index = m_TabControls.GetKeyByValue( w );
			if ( m_SelectedIndex != index )
			{
				m_OnAttemptTabSwitch.Invoke( m_SelectedIndex, index );
				if ( CanSwitchTab() )
				{
					PerformSwitchTab(index);
					
					return true;
				}
			}
		}
		
		return false;
	}
	
	override bool OnChildAdd( Widget w, Widget child )
	{
		if ( w == m_Root.FindAnyWidget( "Tab_Control_Container" ) )
		{
			AlignTabbers();
			return true;
		}
		return false;
	}
	
	override bool OnChildRemove( Widget w, Widget child )
	{
		if ( w == m_Root.FindAnyWidget( "Tab_Control_Container" ) )
		{
			AlignTabbers();
			return true;
		}
		return false;
	}
	
	void EnableTabControl( int index, bool enable )
	{
		Widget tab_control = m_Root.FindAnyWidget( "Tab_Control_" + index );
		if ( tab_control )
			tab_control.Show( enable );
		AlignTabbers();
	}
	
	void SelectTabControl( int index )
	{
		Widget tab_control = m_TabControls.Get( index );
		if ( tab_control )
		{
			/*
			Widget tab_bg = tab_control.FindAnyWidget( tab_control.GetName() + "_Background" );
			if( tab_bg )
			{
				tab_bg.Show( true );
			}
			*/
			
			Widget tab_title = TextWidget.Cast(tab_control.FindAnyWidget( tab_control.GetName() + "_Title" ));
			
			int color_title = ARGB(255, 255, 0, 0);
			int color_backg = ARGB(255, 0, 0 ,0);
			
			#ifdef PLATFORM_CONSOLE
				color_title = ARGB(255, 255, 255, 255);
				color_backg = ARGB(255, 200, 0 ,0);
			#endif
			
			tab_title.SetColor( color_title );
			tab_control.SetColor( color_backg );
		}
	}
	
	void SelectTabPanel( int index )
	{
		Widget tab = m_Tabs.Get( index );
		if ( tab )
		{
			tab.Show( true );
		}
	}
	
	void DeselectTabControl( int index )
	{
		Widget tab_control = m_TabControls.Get( index );
		if ( tab_control )
		{
			/*
			Widget tab_bg = tab_control.FindAnyWidget( tab_control.GetName() + "_Background" );
			if( tab_bg )
			{
				tab_bg.Show( false );
			}
			*/
			Widget tab_title = TextWidget.Cast(tab_control.FindAnyWidget( tab_control.GetName() + "_Title" ));
			tab_title.SetColor( ARGB(255, 255, 255,255) );
			tab_control.SetColor( ARGB(0, 0, 0 ,0) );
		}
	}
	
	void DeselectTabPanel( int index )
	{
		Widget tab = m_Tabs.Get( index );
		if ( tab )
		{
			tab.Show( false );
		}
	}
	
	void DeselectAll()
	{
		for (int i = 0; i < m_TabControls.Count(); i++)
		{
			DeselectTabControl(i);
			DeselectTabPanel(i);
		}
	}
	
	void NextTab()
	{
		int next_index = m_SelectedIndex + 1;
		
		while ( next_index < m_Tabs.Count() && !m_TabControls[next_index].IsVisible() )
		{
			next_index++;
		}
		
		if ( next_index >= m_Tabs.Count() )
		{
			next_index = 0;
		}
		
		if ( m_SelectedIndex != next_index )
		{
			m_OnAttemptTabSwitch.Invoke( m_SelectedIndex, next_index );
			if ( CanSwitchTab() )
			{
				PerformSwitchTab(next_index);
			}
		}
		
		if ( m_FirstInit )
		{
			AlignTabbers();
			m_FirstInit = false;
		}
	}
	
	void PreviousTab()
	{
		int next_index = m_SelectedIndex - 1;
		
		if ( next_index < 0 )
		{
			next_index = m_TabControls.Count() - 1;
		}
		
		while ( next_index > 0 && !m_TabControls[next_index].IsVisible() )
		{
			next_index--;
		}
		
		if ( m_SelectedIndex != next_index )
		{
			m_OnAttemptTabSwitch.Invoke( m_SelectedIndex, next_index );
			if ( CanSwitchTab() )
			{
				PerformSwitchTab(next_index);
			}
		}
		
		if ( m_FirstInit )
		{
			AlignTabbers();
			m_FirstInit = false;
		}
	}
	
	int GetSelectedIndex()
	{
		return m_SelectedIndex;
	}
	
	void RefreshTab(bool performInitAlignment = false)
	{
		m_FirstInit = performInitAlignment;
		DeselectAll();
		PerformSwitchTab(m_SelectedIndex);
	}
	
	protected void UpdateControlsElements()
	{
		Widget xbControls = m_Root.FindAnyWidget("ConsoleControls");
		if (xbControls)
		{
			xbControls.Show(m_TabsCount > 1);
	
			RichTextWidget toolbar_lb = RichTextWidget.Cast(xbControls.FindAnyWidget("TabLeftControl"));
			RichTextWidget toolbar_rb = RichTextWidget.Cast(xbControls.FindAnyWidget("TabRightControl"));
			if (toolbar_lb)
			{
				toolbar_lb.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUITabLeft", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
			}

			if (toolbar_rb)
			{
				toolbar_rb.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUITabRight", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
			}
		}
	}
	
	//! useful if we want to disable actual tabs for whatever reason
	void DisableTabs(bool disable)
	{
		foreach (Widget w : m_Tabs)
		{
			if (disable)
			{
				w.SetFlags( WidgetFlags.IGNOREPOINTER );
			}
			else
			{
				w.ClearFlags( WidgetFlags.IGNOREPOINTER );
			}
			w.Enable(!disable);
		}
	}
}