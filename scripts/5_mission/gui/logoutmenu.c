
class LogoutMenu extends UIScriptedMenu
{	
	private TextWidget m_LogoutTimeText;
	private TextWidget m_DescriptionText;
	private ButtonWidget m_bLogoutNow;
	private ButtonWidget m_bCancel;
	#ifdef PLATFORM_CONSOLE
	private ButtonWidget m_bCancelConsole;
	#endif
	private int m_iTime;
	
	private ref FullTimeData m_FullTime;

	void LogoutMenu()
	{
		m_iTime = 0;
		g_Game.SetKeyboardHandle(this);
		
		m_FullTime = new FullTimeData();
	}

	void ~LogoutMenu()
	{
		g_Game.SetKeyboardHandle(null);
		if (GetGame().GetMission())
			Cancel(); //cancels request on irregular close (player death, suicide, some mass-menu closure...)

		m_FullTime = null;
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetEmoteManager()) 
		{
			player.GetEmoteManager().SetClientLoggingOut(false);
		}
		
		#ifdef PLATFORM_CONSOLE
		if (GetGame().GetMission())
		{
			GetGame().GetMission().GetOnInputDeviceChanged().Remove(OnInputDeviceChanged);
		}
		#endif
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_logout_dialog.layout");
		
		m_LogoutTimeText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtLogoutTime"));
		m_DescriptionText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtDescription"));
		m_bLogoutNow 		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bLogoutNow"));
		m_bCancel 			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bCancel"));
		
		#ifdef PLATFORM_CONSOLE
		m_bCancelConsole	= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bCancelConsole"));
		m_bCancel.Show(false);
		m_bLogoutNow.Show(false);
		#else
		m_bCancel.Show(true);
		m_bLogoutNow.Show(true);
		layoutRoot.FindAnyWidget("toolbar_bg").Show(false);
		#endif
		
		UpdateInfo();
		
		// player should sit down if possible
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious()) 
		{
			player.GetEmoteManager().SetClientLoggingOut(true);
		}

		#ifdef PLATFORM_CONSOLE
		if (GetGame().GetMission())
		{
			GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		}
		
		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());
		#endif

		return layoutRoot;
	}

	void Show()
	{
		if (layoutRoot)
			layoutRoot.Show(true);
	}
	
	void Hide()
	{
		if (layoutRoot)
			layoutRoot.Show(false);
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w == m_bLogoutNow)
		{
			GetGame().GetMission().AbortMission();
			
			return true;
		}
		#ifdef PLATFORM_CONSOLE
		else if (w == m_bCancelConsole)
		#else
		else if (w == m_bCancel)
		#endif
		{
			Hide();
			Cancel();
			return true;
		}

		return false;
	}
	
	override void Update(float timeslice)
	{
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			Hide();
			Cancel();
		}		
	}
		
	void SetLogoutTime()
	{
		m_LogoutTimeText.SetText(" ");
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
			SetTime(--m_iTime);
		}
		else
		{
			Exit();
		}
	}
	
	void UpdateInfo()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player.IsRestrained() || player.IsUnconscious())
		{
			// display killInfo
			m_DescriptionText.SetText("#layout_logout_dialog_note_killed");
		}
		else
		{
			// hide killInfo
			m_DescriptionText.SetText("#layout_logout_dialog_note");
		}
	}

	void Exit()
	{		
		// exit menu and logout screen
		GetGame().GetMission().Continue();	

		// stop updating of logout screen
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);

		// go back to main menu
		GetGame().GetMission().AbortMission();
	}

	void Cancel()
	{
		GetGame().GetMission().Continue();	
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateTime);
		
		// request logout cancel from server
		GetGame().LogoutRequestCancel();
	}
	
	#ifdef PLATFORM_CONSOLE
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		UpdateControlsElementVisibility();
	}

	protected void UpdateControlsElementVisibility()
	{
		bool toolbarShow = false;
		toolbarShow = !GetGame().GetInput().IsEnabledMouseAndKeyboard() || GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		layoutRoot.FindAnyWidget("toolbar_bg").Show(toolbarShow);
		m_bCancelConsole.Show(!toolbarShow);

		if (toolbarShow)
		{
			RichTextWidget toolbar_b = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
			toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		}
	}
	#endif
}