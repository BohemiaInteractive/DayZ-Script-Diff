
class ConsoleToolbarWidgetHandlerBase : ScriptedWidgetEventHandler
{
	Widget m_ToolbarWidget; //'toolbar_bg'
	RichTextWidget m_ToolbarText;
	
	void OnWidgetScriptInit(Widget w)
	{
		m_ToolbarWidget = w;
		m_ToolbarWidget.SetHandler(this);
		
		m_ToolbarText = RichTextWidget.Cast(m_ToolbarWidget.FindAnyWidget("ContextToolbarText"));
		#ifdef PLATFORM_CONSOLE
		if (g_Game.GetMission())
		{
			g_Game.GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		}
		#endif
		UpdateControlsElements();
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements(pInputDeviceType);
		#endif
	}
	
	protected void UpdateControlsElements(EInputDeviceType pInputDeviceType = EInputDeviceType.UNKNOWN)
	{
		#ifndef PLATFORM_CONSOLE
		m_ToolbarWidget.Show(false);
		#endif
	}
}

class PasswordMenuToolbarHandler : ConsoleToolbarWidgetHandlerBase
{
	override protected void UpdateControlsElements(EInputDeviceType pInputDeviceType = EInputDeviceType.UNKNOWN)
	{
		#ifdef PLATFORM_CONSOLE
		string text = "";
		text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlY", "#server_browser_show / #server_browser_hide", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlX", "#server_browser_menu_connect", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#STR_settings_menu_root_toolbar_bg_ConsoleToolbar_Back_BackText0", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		m_ToolbarText.SetText(text);
		
		bool toolbarShow = false;
		if (pInputDeviceType == EInputDeviceType.UNKNOWN)
		{
			toolbarShow = !g_Game.GetInput().IsEnabledMouseAndKeyboardEvenOnServer() || g_Game.GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		}
		else
		{
			toolbarShow = pInputDeviceType == EInputDeviceType.CONTROLLER;
		}
		m_ToolbarWidget.Show(toolbarShow);
		#endif
	}
};