class HudDebugWinCharDebug extends HudDebugWinBase
{
	private PluginDeveloper m_ModuleDeveloper;
	
	private TextWidget 		m_PlayerPosTextWidget;
	private TextWidget 		m_ClipboardTextWidget;

	//============================================
	// HudDebugWinCharDebug
	//============================================
	void HudDebugWinCharDebug(Widget widget_root)
	{	
		m_PlayerPosTextWidget = TextWidget.Cast( widget_root.FindAnyWidget("txt_PlayerPos") );
		m_ClipboardTextWidget = TextWidget.Cast( widget_root.FindAnyWidget("txt_Clipboard") );
	}

	//============================================
	// ~HudDebugWinCharDebug
	//============================================
	void ~HudDebugWinCharDebug()
	{
	}
	
	//============================================
	// Update
	//============================================
	override void Update()
	{
		super.Update();
		
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( player != NULL )
		{
			vector pos = player.GetPosition();
			string pos_str = "Pos: " + pos[0].ToString() + " " + pos[2].ToString();
			m_PlayerPosTextWidget.SetText(pos_str);
		}
		
		string clipboard;
		GetGame().CopyFromClipboard(clipboard);
		clipboard = clipboard.Substring( 0, Math.Min( clipboard.Length(), 128 ) );	//max 128 chars
		clipboard = "Clipboard: " + clipboard;
		m_ClipboardTextWidget.SetText(clipboard);
	}

	//============================================
	// GetWinType
	//============================================
	override int GetType()
	{
		return HudDebug.HUD_WIN_CHAR_DEBUG;
	}
}
