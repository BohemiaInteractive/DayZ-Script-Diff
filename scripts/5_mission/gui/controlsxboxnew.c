
//used as UserID in the layout. Values assigned for convenience here
enum EConsoleButtonsControls
{
	INVALID = 0, //assumed unassigned value, ignored (dividers, formatting elements etc.)
	BUTTON_MENU = 1,
	BUTTON_VIEW = 2,
	BUTTON_A = 4,
	BUTTON_B = 8,
	BUTTON_X = 16,
	BUTTON_Y = 32,
	BUTTON_PAD_UP = 64,
	BUTTON_PAD_DOWN = 128,
	BUTTON_PAD_LEFT = 256,
	BUTTON_PAD_RIGHT = 512,
	BUTTON_SHOULDER_LEFT = 1024,
	BUTTON_SHOULDER_RIGHT = 2048,
	BUTTON_THUMB_LEFT = 4096,
	BUTTON_THUMB_RIGHT = 8192,
	BUTTON_TRIGGER_LEFT = 16384,
	BUTTON_TRIGGER_RIGHT = 32768,
	BUTTON_GROUP_RIGHT_SIDE_COMMON = 65536,
	BUTTON_GROUP_PAD_COMMON = 131072
}

typedef map<int,ref array<Widget>> TButtonPairingInfo; //<button_mask,<associated widgets on the respective side>>

class ControlsXboxNew extends UIScriptedMenu
{
	protected string 					m_BackButtonTextID;
	protected string 					m_NextPresetText;
	protected int 						m_CurrentTabIdx = -1;
	protected int 						m_CurrentPresetVariant = -1;
	
	protected ButtonWidget 				m_Back;
	protected ImageWidget 				m_ControlsLayoutImage;
	
	//-------------
	protected CanvasWidget				m_CanvasWidget;
	protected TabberUI					m_TabScript;
	protected Widget					m_TabberWidget;
	protected Widget 					m_ControlsImage;
	protected Widget 					m_PlatformHolder; //controls container for selected platform
	protected Widget 					m_VariantWidget;
	
	protected ref map<int,Widget> 					m_ImageMarkerStructure;
	protected ref map<int,Widget> 					m_CategoryStructure;
	protected ref map<int,ref TButtonPairingInfo> 	m_AreasLR; //left/right area holders
	
	protected const int 				AREA_LEFT = 1;
	protected const int 				AREA_RIGHT = 2;
	protected const int 				PLATFORM_ADJUST_X1 = 1000;
	protected const int 				PLATFORM_ADJUST_PS = 2000;
	
	//============================================
	// ControlsXboxNew
	//============================================
	void ~ControlsXboxNew()
	{
		PPERequesterBank.GetRequester(PPERequesterBank.REQ_MENUEFFECTS).Stop();
	}
	
	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		UpdateControlsElementVisibility();
		#endif
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		bool mk = GetGame().GetInput().IsEnabledMouseAndKeyboard();
		bool mkServer = GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer();
		
		switch (pInputDeviceType)
		{
		case EInputDeviceType.CONTROLLER:
			if (mk && mkServer)
			{
				GetGame().GetUIManager().ShowUICursor(false);
			}
		break;

		default:
			if (GetGame().GetInput().IsEnabledMouseAndKeyboard())
			{
				GetGame().GetUIManager().ShowUICursor(true);
			}
		break;
		}
		
		UpdateControlsElementVisibility();
	}
	
	void Back()
	{
		GetGame().GetUIManager().Back();
	}
	
	void UpdateTabContent(int tab_index)
	{
		Widget w;
		//hide old
		if (m_CurrentTabIdx != -1)
		{
			m_VariantWidget.Show(false);
			while (m_VariantWidget.GetParent())
			{
				m_VariantWidget = m_VariantWidget.GetParent();
				m_VariantWidget.Show(false);
			}
		}
		
		//show new
		w = FindChildByID(m_CategoryStructure[tab_index],InputUtils.GetConsolePresetID());
		w.Show(true);
		m_VariantWidget = w;
		
		while (w.GetParent())
		{
			w = w.GetParent();
			w.Show(true);
		}
		
		DrawConnectingLines(tab_index);
		m_CurrentTabIdx = tab_index;
	}
	
	protected void DrawConnectingLines(int index)
	{
		//disconnected for now, to be finished
		return;
		
		m_CanvasWidget.Clear();
		
		//TODO drawing over nyah
		m_VariantWidget;
		m_AreasLR = new map<int,ref TButtonPairingInfo>;
		
		Widget wid_side; //left or right area
		Widget wid_spacer; //item in the L/R areas
		wid_side = m_VariantWidget.GetChildren();
		typename t = EConsoleButtonsControls;
		int side_idx;
		int enum_value;
		array<Widget> items_raw;
		array<Widget> items_filtered;
		
		while (wid_side)
		{
			TButtonPairingInfo button_mapping = new TButtonPairingInfo;
			
			side_idx = wid_side.GetUserID();
			wid_spacer = wid_side.GetChildren(); //dig into the side first..
			
			for (int i = 1; i < EnumTools.GetEnumSize(EConsoleButtonsControls); i++)
			{
				items_raw = new array<Widget>;
				items_filtered = new array<Widget>;
				t.GetVariableValue(null, i, enum_value); //TODO
				
				FindAllChildrenByID(wid_spacer,enum_value,items_raw);
				if (FilterByVisible(items_raw,items_filtered) > 0) //if there are any button-relevant items..
				{
					button_mapping.Insert(enum_value,items_filtered);
				}
			}
			m_AreasLR.Insert(side_idx,button_mapping);
			
			wid_side = wid_side.GetSibling();
		}
	}
	
	//============================================
	// Init
	//============================================
	override Widget Init()
	{
		m_CategoryStructure = new map<int,Widget>;
		m_ImageMarkerStructure = new map<int,Widget>;
		
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/xbox/Controls_Screen.layout");
		#ifdef PLATFORM_XBOX
			m_ControlsImage = layoutRoot.FindAnyWidget("XboxControlsImage");
		#else
		#ifdef PLATFORM_PS4
			m_ControlsImage = layoutRoot.FindAnyWidget("PSControlsImage");
		#endif
		#endif
		m_ControlsImage.Show(true);
		m_TabberWidget = layoutRoot.FindAnyWidget("Tabber");
		m_TabberWidget.GetScript(m_TabScript);
		m_TabScript.m_OnTabSwitch.Insert(UpdateTabContent);
		m_CanvasWidget = CanvasWidget.Cast(layoutRoot.FindAnyWidget("CanvasUniversal"));
		m_Back = ButtonWidget.Cast(layoutRoot.FindAnyWidget("back"));
		
		UpdateToolbarText();
		UpdateControlsElements();
		UpdateControlsElementVisibility();
		
		PPERequester_MenuEffects requester;
		Class.CastTo(requester,PPERequesterBank.GetRequester(PPERequesterBank.REQ_MENUEFFECTS));
		requester.SetVignetteIntensity(0.6);
		
		ComposeData();
		UpdateTabContent(0);
		
		GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		
		return layoutRoot;
	}
	
	override void OnShow()
	{
		super.OnShow();
		
		SetFocus(null);
		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button == MouseState.LEFT)
		{
			if (w == m_Back)
			{
				Back();
				return true;
			}
		}
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (IsFocusable(w))
		{
			ColorHighlight(w);
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (IsFocusable(w))
		{
			ColorNormal(w);
			return true;
		}
		return false;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		if (IsFocusable(w))
		{
			ColorHighlight(w);
			return true;
		}
		return false;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (IsFocusable(w))
		{
			ColorNormal(w);
			return true;
		}
		return false;
	}
	
	bool IsFocusable(Widget w)
	{
		return (w && w == m_Back);
	}
	
	override void Update(float timeslice)
	{
		if (GetUApi().GetInputByID(UAUITabLeft).LocalPress())
		{
			m_TabScript.PreviousTab();
		}
		
		if (GetUApi().GetInputByID(UAUITabRight).LocalPress())
		{
			m_TabScript.NextTab();
		}
		
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			Back();
		}
		
		if (GetUApi().GetInputByID(UASwitchPreset).LocalPress())
		{
			PerformSwitchPreset();
			m_TabScript.RefreshTab();
		}
	}
	
	//! Inits data structure
	protected void ComposeData()
	{
		if (m_CategoryStructure)
		{
			m_CategoryStructure = null;
			m_CategoryStructure = new map<int,Widget>;
		}
		
		if (m_ImageMarkerStructure)
		{
			m_ImageMarkerStructure = null;
			m_ImageMarkerStructure = new map<int,Widget>;
		}
		
		Widget w = m_ControlsImage.GetChildren();
		m_ImageMarkerStructure.Set(w.GetUserID(),w);
		
		while (w.GetSibling())
		{
			w = w.GetSibling();
			m_ImageMarkerStructure.Set(w.GetUserID(),w); //inits button markers with their IDs
		}
		
		InputUtils.UpdateConsolePresetID();
		
		#ifdef PLATFORM_XBOX
			m_PlatformHolder = FindChildByID(layoutRoot,PLATFORM_ADJUST_X1);
		#else
			m_PlatformHolder = FindChildByID(layoutRoot,PLATFORM_ADJUST_PS);
		#endif
		
		//categories
		Widget category_widget = m_PlatformHolder.GetChildren();
		m_CategoryStructure.Set(category_widget.GetUserID(),category_widget);
		
		while (category_widget.GetSibling())
		{
			category_widget = category_widget.GetSibling();
			m_CategoryStructure.Set(category_widget.GetUserID(),category_widget);
		}
	}
	
	protected void PerformSwitchPreset()
	{
		Print("PerformSwitchPreset - 1");
		int index;
		string preset_text;
		UAInputAPI inputAPI = GetUApi();
		
		index = inputAPI.PresetCurrent() + 1;
		if (index >= inputAPI.PresetCount())
		{
			index = 0;
		}
		
		inputAPI.SupressNextFrame(true);
		inputAPI.PresetSelect(index);
		UpdateToolbarText();
		
		GetGame().GetMission().GetOnInputPresetChanged().Invoke();
		
		#ifdef PLATFORM_WINDOWS
			GetUApi().Export(); //works on emulated consoles (-xbox,-ps4)
		#else
			GetUApi().SaveInputPresetMiscData(); //default console functionality
		#endif
		
		InputUtils.UpdateConsolePresetID();
		UpdateControlsElements();
	}
	
	protected void UpdateToolbarText()
	{
		UAInputAPI inputAPI = GetUApi();
		int target_idx = inputAPI.PresetCurrent() + 1;
		int count = inputAPI.PresetCount();
		if (target_idx >= inputAPI.PresetCount())
		{
			target_idx = 0;
		}
		
		m_NextPresetText = inputAPI.PresetName(target_idx);
		if (m_NextPresetText == InputUtils.PRESET_OLD)
		{
			m_NextPresetText = "#STR_UAPRESET_ChangeTo_0";
		}
		else if (m_NextPresetText == InputUtils.PRESET_NEW)
		{
			m_NextPresetText = "#STR_UAPRESET_ChangeTo_1";
		}
		else
		{
			m_NextPresetText = "Invalid console preset name: " + m_NextPresetText;
		}
	}
	
	//!Finds immediate child widget with a corresponding userID
	protected Widget FindChildByID(Widget wid, int user_id)
	{
		Widget ret = wid.GetChildren();
		while (ret)
		{
			if (ret.GetUserID() == user_id)
			{
				return ret;
			}
			ret = ret.GetSibling();
		}
		return ret;
	}
	
	//!Finds all immediate children widgets with corresponding userIDs
	protected bool FindAllChildrenByID(Widget wid, int user_id, out array<Widget> results)
	{
		Widget child = wid.GetChildren();
		while (child)
		{
			if (child.GetUserID() == user_id)
			{
				results.Insert(child);
			}
			child = child.GetSibling();
		}
		return (results && results.Count() > 0);
	}
	
	//! returns count
	protected int FilterByVisible(array<Widget> input, array<Widget> filtered)
	{
		for (int i = 0; i < input.Count(); i++)
		{
			if (input[i].IsVisible())
			{
				filtered.Insert(input[i]);
			}
		}
		
		return filtered.Count();
	}
	
	void ColorHighlight(Widget w)
	{
		if (!w)
			return;
				
		int color_pnl = ARGB(255, 0, 0, 0);
		int color_lbl = ARGB(255, 255, 0, 0);
		
		#ifdef PLATFORM_CONSOLE
			color_pnl = ARGB(255, 200, 0, 0);
			color_lbl = ARGB(255, 255, 255, 255);
		#endif
		
		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}
	
	void ColorNormal(Widget w)
	{
		if (!w)
			return;
		
		int color_pnl = ARGB(0, 0, 0, 0);
		int color_lbl = ARGB(255, 255, 255, 255);
		
		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}
	
	void ButtonSetText(Widget w, string text)
	{
		if (!w)
			return;
				
		TextWidget label = TextWidget.Cast(w.FindWidget(w.GetName() + "_label"));
		
		if (label)
		{
			label.SetText(text);
		}
		
	}
	
	void ButtonSetColor(Widget w, int color)
	{
		if (!w)
			return;
		
		Widget panel = w.FindWidget(w.GetName() + "_panel");
		
		if (panel)
		{
			panel.SetColor(color);
		}
	}
	
	void ButtonSetTextColor(Widget w, int color)
	{
		if (!w)
			return;

		TextWidget label	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		TextWidget text		= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text"));
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text_1"));
				
		if (label)
		{
			label.SetColor(color);
		}
		
		if (text)
		{
			text.SetColor(color);
		}
		
		if (text2)
		{
			text2.SetColor(color);
		}
	}

	protected void UpdateControlsElements()
	{
		RichTextWidget toolbar_switch	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("ChangePresetText"));
		toolbar_switch.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UASwitchPreset", m_NextPresetText, EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		
		RichTextWidget toolbar_text = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ContextToolbarText"));
		string text = string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#STR_settings_menu_root_toolbar_bg_ConsoleToolbar_Back_BackText0", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		toolbar_text.SetText(text);
		
		RichTextWidget toolbar_b2	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon0"));
		toolbar_b2.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
	}
	
	protected void UpdateControlsElementVisibility()
	{
		bool toolbarShow = false;
		#ifdef PLATFORM_CONSOLE
		toolbarShow = !GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer() || GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		#endif
		
		layoutRoot.FindAnyWidget("toolbar_bg").Show(toolbarShow);
		layoutRoot.FindAnyWidget("play_panel_root").Show(!toolbarShow);
	}
}
