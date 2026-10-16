class InviteMenu extends UIScriptedMenu
{	
	//! How long to wait for the server to answer the logout request before joining regardless
	protected const int LOGOUT_REPLY_TIMEOUT_MS = 5000;
	//! How much of the logout is left when we start joining, see UpdateTime
	protected const int JOIN_LEAD_TIME_S = 1;

	private TextWidget m_LogoutTimeText;
	private MultilineTextWidget m_DescriptionText;
	private int m_iTime;
	private bool m_bJoining;

	private ref FullTimeData m_FullTime;

	void InviteMenu()
	{
		// the countdown is driven by the server, it is unknown until it answers the logout request
		m_iTime = 0;

		m_FullTime = new FullTimeData();

		if (g_Game.GetMission())
		{
			g_Game.GetMission().AddActiveInputExcludes({"menu"});

			g_Game.GetMission().GetHud().ShowHudUI(false);
			g_Game.GetMission().GetHud().ShowQuickbarUI(false);
		}
	}
	
	void ~InviteMenu()
	{
		if (g_Game && g_Game.GetMission())
		{
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.OnLogoutReplyTimeout);

			// deliberately no LogoutRequestCancel here, see Init

			g_Game.GetMission().RemoveActiveInputExcludes({"menu"},true);

			g_Game.GetMission().GetHud().ShowHudUI(true);
			g_Game.GetMission().GetHud().ShowQuickbarUI(true);
		}
	}
	
	override Widget Init()
	{
		layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/day_z_invite_dialog.layout");
		
		m_LogoutTimeText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtLogoutTime"));
		m_DescriptionText 	= MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("txtDescription"));

		m_DescriptionText.SetText("#layout_logout_dialog_note_invite");
		m_DescriptionText.Update();

		// accepting the invite commits the player to the join, there is no way back out of it
		layoutRoot.FindAnyWidget("bCancel").Show(false);
		layoutRoot.FindAnyWidget("BottomConsoleToolbar").Show(false);

		// a logout the player started themselves may already be running. Take it over before
		// asking for our own, otherwise the server ignores the request as a duplicate and the
		// stale dialog cancels the logout we are waiting on. Releasing the old dialog clears the
		// emote launcher, so this has to happen before we start our own sit emote.
		MissionGameplay mission = MissionGameplay.Cast(g_Game.GetMission());
		if (mission)
			mission.TakeOverLogoutMenu();

		// player should sit down if possible
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player && player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious()) 
		{
			player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SITA);
			player.GetEmoteManager().GetEmoteLauncher().SetForced(EmoteLauncher.FORCE_DIFFERENT);
		}
		
		// The target server only accepts us once this server has released the account on the
		// lobby backend, which happens at the end of the regular logout, so request one and
		// wait for it instead of dropping the connection straight away.
		// Once this request is out the logout is final. Nothing here ever sends
		// LogoutRequestCancel, not even on an irregular close, because being able to time that
		// cancel against the server's deadline is exploitable. If this dialog is destroyed
		// before the join (lag dialogue, death, respawn or login screens) the server's own
		// deadline resolves the session and the character is saved as usual.
		g_Game.LogoutRequestTime();
		SetLogoutTime();
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OnLogoutReplyTimeout, LOGOUT_REPLY_TIMEOUT_MS, false);

		return layoutRoot;
	}

	void SetLogoutTime()
	{
		m_LogoutTimeText.SetText(" ");
	}

	//! Called once the server answered the logout request with the time it needs to log us out
	void StartCountdown(int time)
	{
		if (m_bJoining)
			return;

		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.OnLogoutReplyTimeout);

		if (time > JOIN_LEAD_TIME_S)
		{
			SetTime(time);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UpdateTime, 1000, true);
		}
		else
		{
			// nothing worth waiting for, the server is logging us out right away
			JoinTargetServer();
		}
	}

	//! The server did not answer, which it does not when a logout is already under way. Joining
	//! now can still be refused by the lobby, but it is better than waiting here forever.
	protected void OnLogoutReplyTimeout()
	{
		Print("[Invite]: No logout time received from the server, joining the target server anyway");
		JoinTargetServer();
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
		m_iTime -= 1;

		// Leave before the server reaches its own logout deadline. Once it gets there it
		// terminates the session, which takes us to the main menu and loses the invite. The
		// lobby refusal caused by leaving early is absorbed by the retry in the connect itself.
		if (m_iTime > JOIN_LEAD_TIME_S)
			SetTime(m_iTime);
		else
			JoinTargetServer();
	}

	protected void JoinTargetServer()
	{
		m_bJoining = true;
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.OnLogoutReplyTimeout);

		string ip;
		int port;
		OnlineServices.GetInviteServerInfo(ip, port);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(g_Game.ConnectFromJoin, ip, port);
	}
}