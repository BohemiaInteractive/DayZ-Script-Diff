class HudDebugWinVersion extends HudDebugWinBase
{
	private PluginDeveloper m_ModuleDeveloper;
	
	private TextWidget 		m_VersionTextWidget;

	//============================================
	// HudDebugWinVersion
	//============================================
	void HudDebugWinVersion(Widget widget_root)
	{	
		m_VersionTextWidget = TextWidget.Cast( widget_root.FindAnyWidget("txt_Version") );
		
		string version;
		g_Game.GetVersion(version);
		m_VersionTextWidget.SetText(string.Format("exe: %1 | scripts: %2", version, GetPBOAPI().GetPBOVersion("dta\\scripts.pbo")));
	}

	//============================================
	// ~HudDebugWinVersion
	//============================================
	void ~HudDebugWinVersion()
	{
	}
	
	//============================================
	// Update
	//============================================
	override void Update()
	{
		super.Update();
	}

	//============================================
	// GetWinType
	//============================================
	override int GetType()
	{
		return HudDebug.HUD_WIN_VERSION;
	}
}
