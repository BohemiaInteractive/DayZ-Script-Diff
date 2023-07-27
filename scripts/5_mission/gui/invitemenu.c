class InviteMenu extends UIScriptedMenu
{	
	private TextWidget m_LogoutTimeText;
	private TextWidget m_DescriptionText;
	private ButtonWidget m_bCancel;
	private ButtonWidget m_bCancelConsole;
	private int m_iTime;
	
	private ref FullTimeData m_FullTime;

	void InviteMenu()
	{
		m_iTime = 15;

		m_FullTime = new FullTimeData();
	}
	
	void ~InviteMenu()
	{
		m_FullTime = null;
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_invite_dialog.layout");
		
		m_LogoutTimeText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtLogoutTime"));
		m_DescriptionText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtDescription"));
		m_bCancel 			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bCancel"));
		
		#ifdef PLATFORM_CONSOLE
		m_bCancel.Show(false);
		
		layoutRoot.FindAnyWidget("toolbar_bg").Show(true);
		RichTextWidget toolbar_b = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		#else
		m_bCancel.Show(true);
		layoutRoot.FindAnyWidget("toolbar_bg").Show(false);
		#endif
		
		// player should sit down if possible
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious()) 
		{
			player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SITA);
			player.GetEmoteManager().GetEmoteLauncher().SetForced(EmoteLauncher.FORCE_DIFFERENT);
		}
		
		SetTime(m_iTime);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UpdateTime, 1000, true);

		return layoutRoot;
	}

	override void Update(float timeslice)
	{
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			Cancel();
			Close();
		}
		
		if (m_iTime <= 0)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
			string ip;
			int port;
			OnlineServices.GetInviteServerInfo(ip, port);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(g_Game.ConnectFromJoin, ip, port);
		}
	}
	
	void SetTime(int time)
	{
		m_iTime = time;
		string text = "#layout_logout_dialog_until_logout_";

		TimeConversions.ConvertSecondsToFullTime(time, m_FullTime);
		
		if (m_FullTime.m_Days > 0)
			text += "dhms";
		else if (m_FullTime.m_Hours > 0)
			text += "hms";
		else if (m_FullTime.m_Minutes > 0)
			text += "ms";
		else
			text += "s";
		
		text = Widget.TranslateString(text);
		text = string.Format(text, m_FullTime.m_Seconds, m_FullTime.m_Minutes, m_FullTime.m_Hours, m_FullTime.m_Days);
		m_LogoutTimeText.SetText(text);
	}
		
	void UpdateTime()
	{
		if (m_iTime > 0)
		{
			m_iTime -= 1;
			SetTime(m_iTime);
		}
	}

	void Cancel()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
		
		g_Game.SetGameState(DayZGameState.IN_GAME);
		g_Game.SetLoadState(DayZLoadState.CONNECT_CONTROLLER_SELECT);
		Close();
	}
}