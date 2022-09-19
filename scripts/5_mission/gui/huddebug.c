// *************************************************************************************
// ! PluginDayzPlayerDebugUI 
// *************************************************************************************
class HudDebugEventHandler extends ScriptedWidgetEventHandler
{
	HudDebug m_HudDebug;
	
	void HudDebugEventHandler( HudDebug hud_debug )
	{
		m_HudDebug = hud_debug;
	}
	
	HudDebug GetHudDebug()
	{
		return m_HudDebug;
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		super.OnClick( w, x, y, button );
		return GetHudDebug().OnClick( w, x, y, button );
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		//Print("OnFocus");
		return true;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		//Print("OnFocusLost");
		return true;
	}
	/*
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		Print("OnMouseEnter");
		return true;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		Print("OnMouseLeaves");
		return true;
	}*/
	
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		//Print("OnMouseButtonDown");
		return true;
	}
	
	
	override bool OnSelect(Widget w, int x, int y)
	{
		//Print("OnSelect");
		return true;
	}
	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		//Print("OnItemSelected");
		return true;
	}
	
	override bool OnModalResult(Widget w, int x, int y, int code, int result)
	{
		//Print("OnModalResult");
		return true;
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		//Print("OnChange " + finished);
		super.OnChange( w, x, y, finished );
		return GetHudDebug().OnChange( w, x, y, finished );
	}
}

class HudDebug extends Hud
{
	static const int HUD_WIN_UNDEFINED			= 0;
	static const int HUD_WIN_CHAR_STATS			= 1;
	static const int HUD_WIN_CHAR_MODIFIERS		= 2;
	static const int HUD_WIN_CHAR_AGENTS		= 3;
	static const int HUD_WIN_CHAR_DEBUG			= 4;
	static const int HUD_WIN_CHAR_LEVELS		= 5;
	static const int HUD_WIN_CHAR_STOMACH		= 6;
	static const int HUD_WIN_VERSION			= 7;
	static const int HUD_WIN_TEMPERATURE		= 8;
	
	Widget							m_WgtRoot;
	Widget							m_Crosshair;
	ref array<ref HudDebugWinBase>	m_Panels;
	ref Timer						m_TimerUpdate;
	ref HudDebugEventHandler 		m_HudDebugHandler;
	ref HudDebugWinCharModifiers 	m_WinCharModifiers;
	ref HudDebugWinCharStats		m_WinCharStats;
	ref HudDebugWinCharAgents 		m_WinCharAgents;
	
	//============================================
	// HudDebug
	//============================================
	void HudDebug()
	{
	}

	//============================================
	// ~HudDebug
	//============================================
	void ~HudDebug()
	{
		delete m_WgtRoot;
		m_TimerUpdate.Stop();
	}
	
	//============================================
	// Init
	//============================================
	override void Init( Widget hud_panel_widget )
	{
		m_WgtRoot = hud_panel_widget;
		m_WgtRoot.Show( true );
		
		// Crosshair widget root
		m_Crosshair = m_WgtRoot.FindAnyWidget( "wdw_Crosshair" );
		
		m_Panels = new array<ref HudDebugWinBase>;
		
		// Register Window Character Stats
		m_WinCharStats = new HudDebugWinCharStats( m_WgtRoot.FindAnyWidget( "wdw_CharacterStats" ) );
		m_Panels.Insert( m_WinCharStats );
		
		// Register Window Character Stats
		HudDebugWinCharLevels win_char_levels = new HudDebugWinCharLevels( m_WgtRoot.FindAnyWidget( "wdw_CharacterLevels" ) );
		m_Panels.Insert( win_char_levels );
		
		// Register Window Chracter Modifiers
		m_WinCharModifiers = new HudDebugWinCharModifiers( m_WgtRoot.FindAnyWidget( "wdw_CharacterModifiers" ) );
		m_Panels.Insert( m_WinCharModifiers );
		
		// Register Window Chracter Agents
		m_WinCharAgents = new HudDebugWinCharAgents( m_WgtRoot.FindAnyWidget( "wdw_CharacterAgents" ) );
		m_Panels.Insert( m_WinCharAgents );
		
		// Register Window Chracter Debug
		HudDebugWinCharDebug win_char_debug = new HudDebugWinCharDebug( m_WgtRoot.FindAnyWidget( "wdw_CharacterDebug" ) );
		m_Panels.Insert( win_char_debug );
				
		// Register Window Chracter Debug
		HudDebugWinCharStomach win_char_stomach = new HudDebugWinCharStomach( m_WgtRoot.FindAnyWidget( "wdw_CharacterStomach" ) );
		m_Panels.Insert( win_char_stomach );
		
		// Register Window Version
		HudDebugWinVersion win_version = new HudDebugWinVersion( m_WgtRoot.FindAnyWidget( "wdw_Version" ) );
		m_Panels.Insert( win_version );
		
		// Register Window Temperature
		HudDebugWinTemperature win_temp = new HudDebugWinTemperature( m_WgtRoot.FindAnyWidget( "wdw_Temp" ) );
		m_Panels.Insert( win_temp );
		
		
		RefreshByLocalProfile();
		RefreshCrosshairVisibility();
		
		m_TimerUpdate = new Timer();
		m_TimerUpdate.Run( 1.0, this, "Update", NULL, true );
		
		//set ui event handler
		m_HudDebugHandler = new HudDebugEventHandler( this );
		m_WgtRoot.SetHandler( m_HudDebugHandler );
	}
	
	//============================================
	// Update
	//============================================
	override void Update( float timeslice )
	{
		for ( int i = 0; i < m_Panels.Count(); ++i )
		{
			if ( m_Panels.Get( i ).IsVisible() )
			{
				m_Panels.Get( i ).Update();
			}
		}
	}

	//============================================
	// SetPanetVisible
	//============================================
	void SetPanelVisible(int panel_type, bool visible)
	{
		if ( visible )
		{
			PanelShow( panel_type );
		}
		else
		{
			PanelHide( panel_type );
		}
	}
	
	//============================================
	// PanelShow
	//============================================
	void PanelShow(int panel_type)
	{
		for ( int i = 0; i < m_Panels.Count(); ++i )
		{
			HudDebugWinBase panel = m_Panels.Get( i );
			
			if ( panel.GetType() == panel_type )
			{
				panel.Show();
			}
		}
	}

	//============================================
	// PanelHide
	//============================================
	void PanelHide(int panel_type)
	{
		for ( int i = 0; i < m_Panels.Count(); ++i )
		{
			HudDebugWinBase panel = m_Panels.Get( i );
			
			if ( panel.GetType() == panel_type )
			{
				panel.Hide();
			}
		}
	}

	//============================================
	// RefreshCrosshairVisibility
	//============================================
	void RefreshCrosshairVisibility()
	{
		PluginConfigDebugProfile module_cfg_profile = PluginConfigDebugProfile.Cast( GetPlugin( PluginConfigDebugProfile ) );
		
		if ( module_cfg_profile )
		{
			PluginDeveloper modul_dev = PluginDeveloper.Cast( GetPlugin( PluginDeveloper ) );

			if ( modul_dev.IsEnabledFreeCamera() )
			{
				m_Crosshair.Show( module_cfg_profile.GetFreeCameraCrosshairVisible() );
			}
			else
			{
				m_Crosshair.Show( false );
			}
		}
	}

	
	//============================================
	// HideCrosshairVisibility
	//============================================
	void HideCrosshairVisibility()
	{
		PluginConfigDebugProfile module_cfg_profile = PluginConfigDebugProfile.Cast( GetPlugin( PluginConfigDebugProfile ) );
		
		if ( module_cfg_profile )
		{
			PluginDeveloper modul_dev = PluginDeveloper.Cast( GetPlugin( PluginDeveloper ) );

			if ( modul_dev.IsEnabledFreeCamera() )
			{
				m_Crosshair.Show( false );
			}
		}
	}

	//============================================
	// RefreshByLocalProfile
	//============================================
	void RefreshByLocalProfile()
	{
		PluginConfigDebugProfile module_cfg_profile = PluginConfigDebugProfile.Cast( GetPlugin( PluginConfigDebugProfile ) );
		
		if ( module_cfg_profile )
		{
			SetPanelVisible( HudDebug.HUD_WIN_CHAR_STATS, 		module_cfg_profile.GetCharacterStatsVisible() );
			SetPanelVisible( HudDebug.HUD_WIN_CHAR_LEVELS, 		module_cfg_profile.GetCharacterLevelsVisible() );
			SetPanelVisible( HudDebug.HUD_WIN_CHAR_MODIFIERS, 	module_cfg_profile.GetCharacterModifiersVisible() );
			SetPanelVisible( HudDebug.HUD_WIN_CHAR_AGENTS,	 	module_cfg_profile.GetCharacterAgentsVisible() );
			SetPanelVisible( HudDebug.HUD_WIN_CHAR_DEBUG, 		module_cfg_profile.GetCharacterDebugVisible() );
			SetPanelVisible( HudDebug.HUD_WIN_CHAR_STOMACH, 	module_cfg_profile.GetCharacterStomachVisible() );
			SetPanelVisible( HudDebug.HUD_WIN_VERSION, 			module_cfg_profile.GetVersionVisible() );
			SetPanelVisible( HudDebug.HUD_WIN_TEMPERATURE, 		module_cfg_profile.GetTempVisible() );
		}
	}
	
	//============================================
	// IsInitialized
	//============================================
	bool IsInitialized()
	{
		if ( m_WgtRoot == NULL )
		{
			return false;
		}
		
		return false;
	}	
	
	//============================================
	// OnClick
	//============================================
	bool OnClick( Widget w, int x, int y, int button )
	{	
		//send OnClick to HudDebugWinCharModifiers
		if ( m_WinCharModifiers )
		{
			if (m_WinCharModifiers.OnClick( w, x, y, button ))
				return true;
		}
		
		if ( m_WinCharAgents )
		{
			if (m_WinCharAgents.OnClick( w, x, y, button ))
				return true;
		}
		
		if ( m_WinCharStats )
		{
			if (m_WinCharStats.OnClick( w, x, y, button ))
				return true;
		}
		
		return false;
	}
	
	bool OnChange(Widget w, int x, int y, bool finished)
	{

		if ( m_WinCharStats )
		{
			if (m_WinCharStats.OnChange( w, x, y, finished ))
				return true;
		}
		return false;
	}
	
}
