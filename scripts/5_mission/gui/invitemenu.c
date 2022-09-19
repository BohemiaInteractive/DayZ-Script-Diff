class InviteMenu extends UIScriptedMenu
{	
	private TextWidget		m_LogoutTimetext;
	private TextWidget		m_Info;
	private int m_iTime;

	void InviteMenu()
	{
		m_iTime = 15;
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_invite_dialog.layout");
		
		m_LogoutTimetext = TextWidget.Cast( layoutRoot.FindAnyWidget("logoutTimeText") );
		m_Info = TextWidget.Cast( layoutRoot.FindAnyWidget("txtInfo") );
		m_LogoutTimetext.SetText(m_iTime.ToString());
		
		layoutRoot.FindAnyWidget("toolbar_bg").Show(true);
		RichTextWidget toolbar_b = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		
		// player should sit down if possible
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious()) 
		{
			player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SITA);
			player.GetEmoteManager().GetEmoteLauncher().SetForced(EmoteLauncher.FORCE_DIFFERENT);
		}
		
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( UpdateTime, 1000, true );
		return layoutRoot;
	}
	
	override void OnShow()
	{
		super.OnShow();
	}

	override void Update( float timeslice )
	{
		if( GetGame().GetInput().LocalPress( "UAUIBack", false ) )
		{
			Cancel();
			Close();
		}
		
		if (m_iTime <= 0)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
			string ip;
			int port;
			OnlineServices.GetInviteServerInfo( ip, port );
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).Call(g_Game.ConnectFromJoin, ip, port);
			//Close();
		}
	}
		
	void UpdateTime()
	{
		if (m_iTime > 0)
		{
			m_iTime -= 1;
			m_LogoutTimetext.SetText(m_iTime.ToString());	
		}
	}

	void Cancel()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
		
		g_Game.SetGameState( DayZGameState.IN_GAME );
		g_Game.SetLoadState( DayZLoadState.CONNECT_CONTROLLER_SELECT );
		Close();
	}
}