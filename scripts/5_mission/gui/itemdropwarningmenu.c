class WarningMenuBase : UIScriptedMenu
{
	protected ButtonWidget m_OkButton;
	protected MultilineTextWidget m_Description;

	void WarningMenuBase()
	{
		if (GetGame().GetMission())
		{
			GetGame().GetMission().AddActiveInputExcludes({"menu"});

			GetGame().GetMission().GetHud().ShowHudUI(false);
			GetGame().GetMission().GetHud().ShowQuickbarUI(false);
		}
	}

	void ~WarningMenuBase()
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
		layoutRoot 		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_dropped_items.layout");
		m_OkButton 		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bOK"));
		m_Description 	= MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("txtDescription"));
		m_Description.Show(true);	

		string text = Widget.TranslateString(GetText());
		m_Description.SetText(text);
		
		if (GetGame().GetMission())
		{		
			GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
			GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		}
		
		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());

		return layoutRoot;
	}
	
	string GetText()
	{
		return "";
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w.GetUserID() == IDC_OK)
		{
			Close();
			return true;
		}

		return false;
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);

		#ifdef PLATFORM_CONSOLE
		if (GetUApi().GetInputByID(UAUISelect).LocalPress())
			Close();
		#endif
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
		string context = string.Format(" %1", InputUtils.GetRichtextButtonIconFromInputAction("UAUISelect", "#early_access_alpha_understand", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		
		toolbarText.SetText(context);
	}
	
	protected void UpdateControlsElementVisibility()
	{
		bool toolbarShow = false;
		#ifdef PLATFORM_CONSOLE
		toolbarShow = !GetGame().GetInput().IsEnabledMouseAndKeyboard() || GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		#endif
		
		layoutRoot.FindAnyWidget("BottomConsoleToolbar").Show(toolbarShow);
		m_OkButton.Show(!toolbarShow);
	}
}

class ItemDropWarningMenu : WarningMenuBase
{
	override string GetText()
	{
		return "#str_item_drop_notification";
	}
}

class PlayerRepositionWarningMenu : WarningMenuBase
{
	override string GetText()
	{
		return "#str_position_change_notification";
	}
}