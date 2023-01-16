class InputDeviceDisconnectWarningMenu extends UIScriptedMenu
{
	private ref array<int> m_DisconnectedDevices;
	private ref array<int> m_DeviceOrder = {EUAINPUT_DEVICE_CONTROLLER,EUAINPUT_DEVICE_MOUSE,EUAINPUT_DEVICE_KEYBOARD};
	private ref map<int,string> m_DeviceMessages;
	private bool m_CanClose;
	private bool m_Initialized;
	private string m_DevicesText;
	private string m_CaptionText;
	private string m_ConfirmationText;
	
	private WrapSpacerWidget 	m_WrapperWidget;
	private TextWidget 			m_CaptionWidget;
	private RichTextWidget 		m_DeviceListWidget;
	private RichTextWidget 		m_ActionTextWidget;
	
	void InputDeviceDisconnectWarningMenu()
	{
		g_Game.GetMission().AddActiveInputExcludes({"gamepaddisconnect"});
		
		m_DisconnectedDevices = new array<int>;
		m_CanClose = false;
		
		BuildMessages();
	}
	
	void ~InputDeviceDisconnectWarningMenu()
	{
		if (g_Game)
		{
			PPERequesterBank.GetRequester(PPERequester_ControllerDisconnectBlur).Stop();
			g_Game.DeleteTitleScreen(); // Removes any outstanding gamepad identification propmt; the gamepad is already recognized by now
			if (g_Game.GetMission())
				g_Game.GetMission().RemoveActiveInputExcludes({"gamepaddisconnect"},true);
		}
	}
	
	override Widget Init()
	{
		layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/notifications/day_z_input_device_warning.layout");
		m_WrapperWidget = WrapSpacerWidget.Cast(layoutRoot.FindAnyWidget("Wrapper"));
		m_CaptionWidget = TextWidget.Cast(layoutRoot.FindAnyWidget("Caption"));
		m_DeviceListWidget = RichTextWidget.Cast(layoutRoot.FindAnyWidget("DeviceList"));
		m_ActionTextWidget = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ActionText"));
		
		g_Game.GetMission().GetOnInputDeviceConnected().Insert(UpdateDisconnectedDevices);
		g_Game.GetMission().GetOnInputDeviceDisconnected().Insert(UpdateDisconnectedDevices);
		
		UpdateDisconnectedDevices();
		if (g_Game.GetUIManager() && g_Game.GetUIManager().IsDialogVisible())
		{
			g_Game.GetUIManager().CloseDialog();
		}
		
		PPERequesterBank.GetRequester(PPERequester_ControllerDisconnectBlur).Start();
		
		return layoutRoot;
	}
	
	private void BuildMessages()
	{
		m_DeviceMessages = new map<int,string>;
		m_DeviceMessages.Insert(EUAINPUT_DEVICE_CONTROLLER,string.Format("<image set=\"dayz_gui\" name=\"icon_Gamepad\" />"));
		m_DeviceMessages.Insert(EUAINPUT_DEVICE_MOUSE,string.Format("<image set=\"dayz_gui\" name=\"icon_Mouse\" />"));
		m_DeviceMessages.Insert(EUAINPUT_DEVICE_KEYBOARD,string.Format("<image set=\"dayz_gui\" name=\"icon_Keyboard\" />"));
	}
	
	//! Composes message depending on context
	private void UpdateDisconnectTexts()
	{
		int deviceCount = m_DisconnectedDevices.Count();
		int count = m_DeviceOrder.Count();
		int idx;
		int device;
		bool gamepadInactive = false;
		
		m_DevicesText = string.Empty;
		for (int i = 0; i < count; i++)
		{
			device = m_DeviceOrder[i];
			idx = m_DisconnectedDevices.Find(device);
			if (idx != -1) //is disconnected and relevant
			{
				m_DevicesText += m_DeviceMessages.Get(device) + " ";
				if (device == EUAINPUT_DEVICE_CONTROLLER)
				{
					gamepadInactive = true;
					if (deviceCount > 1)
					{
						m_DevicesText += "| ";
					}
				}
			}
		}
		
		if (deviceCount > 1)
		{
			m_CaptionText = "#str_inputdevice_notification_header_multiple";
			m_ConfirmationText = "#str_inputdevice_notification_prompt_multiple";
		}
		else
		{
			m_CaptionText = "#str_inputdevice_notification_header_single";
			m_ConfirmationText = "#str_inputdevice_notification_prompt_single";
		}
	}
	
	private void SetDisconnectTexts()
	{
		m_CaptionWidget.SetText(m_CaptionText);
		m_DeviceListWidget.SetText(m_DevicesText);
		m_ActionTextWidget.SetText(m_ConfirmationText);
		
		m_WrapperWidget.Update();
	}
	
	private void UpdateDisconnectedDevices(int device = -1)
	{
		m_DisconnectedDevices.Clear();
		m_CanClose = g_Game.GetInput().AreAllAllowedInputDevicesActive(m_DisconnectedDevices);
		UpdateWarningMenu(device);
		m_Initialized = true;
	}
	
	private void UpdateWarningMenu(int device)
	{
		if (m_CanClose)
		{
			Close();
		}
		else if (layoutRoot)
		{
			UpdateDisconnectTexts();
			SetDisconnectTexts();
		}
	}
	
	//back-up close handling
	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		bool ret = super.OnMouseButtonUp(w,x,y,button);
		
		if (m_CanClose)
		{
			Close();
		}
		return ret | m_CanClose;
	}
	
	//back-up close handling
	override bool OnKeyUp(Widget w, int x, int y, int key)
	{
		bool ret = super.OnKeyUp(w,x,y,key);
		
		if (m_CanClose)
		{
			Close();
		}
		return ret | m_CanClose;
	}
}