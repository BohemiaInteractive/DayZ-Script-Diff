class InviteMenu extends UIScriptedMenu
{	
	private TextWidget m_LogoutTimeText;
	private MultilineTextWidget m_DescriptionText;
	private ButtonWidget m_bCancel;
	private ButtonWidget m_bCancelConsole;
	private int m_iTime;
	
	private ref FullTimeData m_FullTime;

	void InviteMenu()
	{
		m_iTime = 15;

		m_FullTime = new FullTimeData();

		if (GetGame().GetMission())
		{
			GetGame().GetMission().AddActiveInputExcludes({"menu"});

			GetGame().GetMission().GetHud().ShowHudUI(false);
			GetGame().GetMission().GetHud().ShowQuickbarUI(false);
		}
	}
	
	void ~InviteMenu()
	{
		if (GetGame() && GetGame().GetMission())
		{
			GetGame().GetMission().RemoveActiveInputExcludes({"menu"},true);
			
			GetGame().GetMission().GetHud().ShowHudUI(true);
			GetGame().GetMission().GetHud().ShowQuickbarUI(true);
	
			GetGame().GetMission().GetOnInputPresetChanged().Remove(OnInputPresetChanged);
			GetGame().GetMission().GetOnInputDeviceChanged().Remove(OnInputDeviceChanged);
		}
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_invite_dialog.layout");
		
		m_LogoutTimeText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtLogoutTime"));
		m_DescriptionText 	= MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("txtDescription"));
		m_bCancel 			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bCancel"));
		
		m_DescriptionText.SetText("#layout_logout_dialog_note_invite");
		m_DescriptionText.Update();
		
		// player should sit down if possible
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious()) 
		{
			player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SITA);
			player.GetEmoteManager().GetEmoteLauncher().SetForced(EmoteLauncher.FORCE_DIFFERENT);
		}
		
		if (GetGame().GetMission())
		{		
			GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
			GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		}
		
		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());
		
		SetTime(m_iTime);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UpdateTime, 1000, true);

		return layoutRoot;
	}

	override void Update(float timeslice)
	{
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
			Cancel();
		
		if (m_iTime <= 0)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
			string ip;
			int port;
			OnlineServices.GetInviteServerInfo(ip, port);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(g_Game.ConnectFromJoin, ip, port);
		}
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w.GetUserID() == IDC_CANCEL)
		{
			Cancel();
			return true;
		}

		return false;
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
	
	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		#endif
	}

	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		UpdateControlsElements();
		UpdateControlsElementVisibility();
	}
	
	protected void UpdateControlsElements()
	{
		RichTextWidget toolbarText = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ContextToolbarText"));
		string context = string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#dialog_cancel", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		
		toolbarText.SetText(context);
	}
	
	protected void UpdateControlsElementVisibility()
	{
		bool toolbarShow = false;
		#ifdef PLATFORM_CONSOLE
		toolbarShow = !GetGame().GetInput().IsEnabledMouseAndKeyboard() || GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		#endif
		
		layoutRoot.FindAnyWidget("BottomConsoleToolbar").Show(toolbarShow);
		m_bCancel.Show(!toolbarShow);
	}
}