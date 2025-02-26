class ConnectionDialogue extends UIScriptedMenu
{	
	protected bool m_DebugMonitorHidden;

	protected MultilineTextWidget m_Description;
	protected ButtonWidget m_DisconnectBtn;
	#ifdef PLATFORM_CONSOLE
	protected RichTextWidget m_DisconnectBtnLabel;
	protected Widget m_ConsoleToolbar;
	#endif
	
	void ConnectionDialogue() {}
	
	void ~ConnectionDialogue()
	{
		#ifdef PLATFORM_CONSOLE
		if (GetGame().GetMission())
		{
			GetGame().GetMission().GetOnInputDeviceChanged().Remove(OnInputDeviceChanged);
		}
		#endif
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_connection_dialogue.layout");
		m_DisconnectBtn = ButtonWidget.Cast(layoutRoot.FindAnyWidget("DCButton"));
		#ifdef PLATFORM_CONSOLE
		m_DisconnectBtnLabel = RichTextWidget.Cast(layoutRoot.FindAnyWidget("DCButtonLabel"));
		m_ConsoleToolbar = layoutRoot.FindAnyWidget("ConsoleToolbar");
		#endif
		
		#ifdef PLATFORM_CONSOLE
		if (GetGame().GetMission())
		{
			GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		}
		#endif
		
		RichTextWidget toolbar_text = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ContextToolbarText"));
		string context = string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", "#main_menu_exit", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));		
		toolbar_text.SetText(context);
		
		#ifdef PLATFORM_CONSOLE
		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());
		#endif
		
		return layoutRoot;
	}
	
	#ifdef PLATFORM_CONSOLE
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		switch (pInputDeviceType)
		{
			case EInputDeviceType.CONTROLLER:
			{
				GetGame().GetUIManager().ShowUICursor(false);
				m_DisconnectBtnLabel.SetText("#main_menu_exit");
				SetFocus(m_DisconnectBtn);
			}
			break;
			default:
			{
				if (GetGame().GetInput().IsEnabledMouseAndKeyboard())
				{
					GetGame().GetUIManager().ShowUICursor(true);
					m_DisconnectBtnLabel.SetText(string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", "#main_menu_exit", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_NORMAL)));
					SetFocus(null);
				}
			}
			break;
		}

		UpdateControlsElementVisibility();
	}

	protected void UpdateControlsElementVisibility()
	{
		bool toolbarShow = !GetGame().GetInput().IsEnabledMouseAndKeyboard() || GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		m_ConsoleToolbar.Show(toolbarShow);
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (w == m_DisconnectBtn)
		{
			m_DisconnectBtn.SetColor(ARGB(255, 255, 0, 0));
			return true;
		}
		return false;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (w == m_DisconnectBtn)
		{
			m_DisconnectBtn.SetColor(ARGB(0, 0, 0, 0));
			return true;
		}
		return false;
	}
	#endif
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);

		if (w == m_DisconnectBtn)
		{
			CloseDialog();
		}

		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w == m_DisconnectBtn)
		{
			m_DisconnectBtn.SetColor(ARGB(255, 255, 0, 0));
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w == m_DisconnectBtn)
		{
			m_DisconnectBtn.SetColor(ARGB(0, 0, 0, 0));
			return true;
		}
		return false;
	}
	
	override void OnShow()
	{
		super.OnShow();

		SetFocus(null);
		PPERequesterBank.GetRequester(PPERequester_LatencyBlur).Start();
		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (mission)
		{
			mission.GetHud().ShowHud(false);
			mission.GetHud().ShowQuickBar(false);
			mission.AddActiveInputExcludes({"menu"});
			mission.AddActiveInputRestriction(EInputRestrictors.INVENTORY);
			GetUApi().SupressNextFrame(true);
			
			//! Hide debug monitor if it is shown
			if (mission && mission.m_DebugMonitor && mission.m_DebugMonitor.IsVisible())
			{
				mission.m_DebugMonitor.Hide();
				m_DebugMonitorHidden = true;
			}
		}
		
		if (GetDayZGame())
		{
			GetDayZGame().SetConnectivityStatState(EConnectivityStatType.CONN_LOST, EConnectivityStatLevel.LEVEL1);
		}
		
		#ifdef PLATFORM_PS4
		OnlineServices.SetMultiplayState(false);
		#endif
	}
	
	override void OnHide()
	{
		super.OnHide();

		PPERequesterBank.GetRequester(PPERequester_LatencyBlur).Stop();
		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (mission)
		{
			// make sure certain input restrictions are removed if present
			mission.RemoveActiveInputExcludes({"menu"}, true);
			mission.RemoveActiveInputRestriction(EInputRestrictors.INVENTORY);
			mission.GetHud().ShowHud(true);
			mission.GetHud().ShowQuickBar(true);
			
			//! Show debug monitor again if it was shown before
			if (m_DebugMonitorHidden)
			{
				if (mission.m_DebugMonitor)
				{
					mission.m_DebugMonitor.Show();
				}
				m_DebugMonitorHidden = false;
			}
		}
		
		if (GetDayZGame())
		{
			GetDayZGame().SetConnectivityStatState(EConnectivityStatType.CONN_LOST, EConnectivityStatLevel.OFF);
		}
		
		#ifdef PLATFORM_PS4
		OnlineServices.SetMultiplayState(true);
		#endif
	}
	
	override void Update(float timeslice)
	{
		#ifdef PLATFORM_CONSOLE
		if (GetUApi().GetInputByID(UAUISelect).LocalValue())
		{
			CloseDialog();
			return;
		}
		#endif
	}
	
	void CloseDialog()
	{
		if (GetGame())
		{
			GetGame().GetUIManager().CloseMenu(MENU_CONNECTION_DIALOGUE);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(GetGame().DisconnectSessionForce);
		}
	}
}