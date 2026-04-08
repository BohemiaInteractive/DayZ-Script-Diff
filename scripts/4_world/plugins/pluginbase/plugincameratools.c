class PluginCameraTools extends PluginBase
{
	protected static PluginCameraTools m_Instance;
 	protected static bool m_IsOpen;
	
	override void OnInit()
	{
		m_Instance = this;
	}
	
	static PluginCameraTools GetInstance()
	{
		return m_Instance;
	}
	
	//===========================================
	// ToggleCameraTools
	//===========================================
	void ToggleCameraTools()
	{
		if( !m_IsOpen )
		{
			g_Game.GetUIManager().EnterScriptedMenu( MENU_CAMERA_TOOLS, null );
		}
		else
		{
			g_Game.GetUIManager().Back();
		}
		m_IsOpen = !m_IsOpen;
	}
	
	//===========================================
	// CursorHide
	//===========================================
	void CursorHide()
	{
		if( g_Game.GetUIManager().GetMenu() )
			g_Game.GetUIManager().GetMenu().OnHide();
	}
	
	//===========================================
	// CursorShow
	//===========================================
	void CursorShow()
	{
		if( g_Game.GetUIManager().GetMenu() )
			g_Game.GetUIManager().GetMenu().OnShow();
	}
	
	void Save()
	{
		if( g_Game.GetUIManager().GetMenu() )
			g_Game.GetUIManager().GetMenu().OnKeyPress( null, 0, 0, KeyCode.KC_S );
	}
	
	void PlayPause()
	{
		if( g_Game.GetUIManager().GetMenu() )
			g_Game.GetUIManager().GetMenu().OnKeyPress( null, 0, 0, KeyCode.KC_P );
	}
	
	void AddKeyframe()
	{
		if( g_Game.GetUIManager().GetMenu() )
			g_Game.GetUIManager().GetMenu().OnKeyPress( null, 0, 0, KeyCode.KC_Q );
	}
	
	void DeleteKeyframe()
	{
		if( g_Game.GetUIManager().GetMenu() )
			g_Game.GetUIManager().GetMenu().OnKeyPress( null, 0, 0, KeyCode.KC_E );
	}
}
