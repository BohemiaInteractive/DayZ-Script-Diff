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
			GetGame().GetUIManager().EnterScriptedMenu( MENU_CAMERA_TOOLS, null );
		}
		else
		{
			GetGame().GetUIManager().Back();
		}
		m_IsOpen = !m_IsOpen;
	}
	
	//===========================================
	// CursorHide
	//===========================================
	void CursorHide()
	{
		if( GetGame().GetUIManager().GetMenu() )
			GetGame().GetUIManager().GetMenu().OnHide();
	}
	
	//===========================================
	// CursorShow
	//===========================================
	void CursorShow()
	{
		if( GetGame().GetUIManager().GetMenu() )
			GetGame().GetUIManager().GetMenu().OnShow();
	}
	
	void Save()
	{
		if( GetGame().GetUIManager().GetMenu() )
			GetGame().GetUIManager().GetMenu().OnKeyPress( null, 0, 0, KeyCode.KC_S );
	}
	
	void PlayPause()
	{
		if( GetGame().GetUIManager().GetMenu() )
			GetGame().GetUIManager().GetMenu().OnKeyPress( null, 0, 0, KeyCode.KC_P );
	}
	
	void AddKeyframe()
	{
		if( GetGame().GetUIManager().GetMenu() )
			GetGame().GetUIManager().GetMenu().OnKeyPress( null, 0, 0, KeyCode.KC_Q );
	}
	
	void DeleteKeyframe()
	{
		if( GetGame().GetUIManager().GetMenu() )
			GetGame().GetUIManager().GetMenu().OnKeyPress( null, 0, 0, KeyCode.KC_E );
	}
}
