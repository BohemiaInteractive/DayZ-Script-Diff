
class LogoutMenu extends UIScriptedMenu
{	
	private TextWidget m_LogoutTimeText;
	private TextWidget m_DescriptionText;
	private ButtonWidget m_bLogoutNow;
	private ButtonWidget m_bCancel;
	private ButtonWidget m_bCancelConsole;
	private int m_iTime;

	void LogoutMenu()
	{
		m_iTime = 0;
		g_Game.SetKeyboardHandle(this);
	}

	void ~LogoutMenu()
	{
		g_Game.SetKeyboardHandle(NULL);
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_logout_dialog.layout");
		
		m_LogoutTimeText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtLogoutTime"));
		m_DescriptionText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtDescription"));
		m_bLogoutNow 		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bLogoutNow"));
		m_bCancel 			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bCancel"));
		
		#ifdef PLATFORM_CONSOLE
		m_bCancel.Show(false);
		m_bLogoutNow.Show(false);
		
		layoutRoot.FindAnyWidget("toolbar_bg").Show(true);
		RichTextWidget toolbar_b = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
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
			player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SITA);
			player.GetEmoteManager().GetEmoteLauncher().SetForced(EmoteLauncher.FORCE_DIFFERENT);
		}
		
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
		else if (w == m_bCancel)
		{
			Hide();
			Cancel();
			return true;
		}

		return false;
	}
	
	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		super.OnKeyDown( w, x, y, key);
		
		switch(key)
		{
			// TODO: just temporary until new inputs are done
			case KeyCode.KC_W:
			case KeyCode.KC_S:
			case KeyCode.KC_A:
			case KeyCode.KC_D:
			case KeyCode.KC_UP:
			case KeyCode.KC_DOWN:
			case KeyCode.KC_LEFT:
			case KeyCode.KC_RIGHT:
			case KeyCode.KC_ESCAPE:
				Cancel();
		}
		
		return true;
	}

	override void Update( float timeslice )
	{
		if( GetGame().GetInput().LocalPress( "UAUIBack", false ) )
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
		m_LogoutTimeText.SetText(m_iTime.ToString() + " #layout_logout_dialog_until_logout");
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
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
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
}