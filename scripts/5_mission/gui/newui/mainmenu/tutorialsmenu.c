class TutorialsMenu extends UIScriptedMenu
{
	protected const string PATH_MOUSEKEY 		= "scripts/data/pagedatatutorials.json";
	protected const string PATH_MOUSEKEY_720P 	= "scripts/data/pagedatatutorials_720p.json";
	protected const string PATH_X1_OLD 			= "xbox/pagedatatutorials.json";
	protected const string PATH_X1_OLD_720P 	= "xbox/pagedatatutorials_720p.json";
	protected const string PATH_X1_NEW 			= "xbox/pagedatatutorialsalternate.json";
	protected const string PATH_X1_NEW_720p 	= "xbox/pagedatatutorialsalternate_720p.json";
	protected const string PATH_PS_OLD 			= "ps4/pagedatatutorials.json";
	protected const string PATH_PS_NEW 			= "ps4/pagedatatutorialsalternate.json";
	
	protected const float MIN_LINE_SCALE		= 1.0;
	protected const float MAX_LINE_SCALE		= 1.5;
	protected const float WITDH_SCALE_RES		= 1920.0;
	protected const float HEIGHT_SCALE_RES		= 1080.0;
	protected const float LINE_THICKNESS_BASE 	= 2.0;
	protected const float BRANCH_OFFSET_BASE 	= 50.0;
	
	protected const int EXACT_TEXT_SIZE_LOW_RES = 32;

	protected string 					m_BackButtonTextID;
	
	protected Widget					m_InfoTextLeft;
	protected Widget					m_InfoTextRight;
	protected ButtonWidget				m_Back;
	
	protected ImageWidget 				m_ControlsLayoutImage;
	protected const int 				TABS_COUNT = 4;
	protected ImageWidget 				m_tab_images[TABS_COUNT];
	protected TabberUI					m_TabScript;
	//protected ref TutorialKeybinds		m_KeybindsTab;
	protected int m_CurrentTabIndex = 0;
	protected Input m_Input;
	
	protected bool m_LowResTutorialMode;
	
	void TutorialsMenu()
	{
		m_Input = g_Game.GetInput();
	}
	
	//============================================
	// Init
	//============================================
	override Widget Init()
	{
		m_LowResTutorialMode = IsLowResTutorialMode();
		
		#ifdef PLATFORM_CONSOLE
		if (m_LowResTutorialMode)
			layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/tutorials/xbox/tutorials_720p.layout");
		else
			layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/tutorials/xbox/tutorials.layout");
		#else
		if (m_LowResTutorialMode)
			layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/tutorials/pc/tutorials_720p.layout");
		else
			layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/tutorials/pc/tutorials.layout");
		#endif
		
		m_InfoTextLeft	= layoutRoot.FindAnyWidget("InfoTextLeft");
		m_InfoTextRight	= layoutRoot.FindAnyWidget("InfoTextRight");
		
		m_Back			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("back"));
		
		layoutRoot.FindAnyWidget("Tabber").GetScript(m_TabScript);
		m_TabScript.m_OnTabSwitch.Insert(DrawConnectingLines);
		
		//! Commented for now as there is no reason to show the tab with mouse and keyboard inputs on console platforms and we don't show the tab on PC or MSS version also
		//! If we ever decide to remap all PlayStation related input keys so we can localize them correctly we can reanable this tab and display the correct bindigs always.
		/*#ifdef PLATFORM_CONSOLE
		if (m_Input.IsEnabledMouseAndKeyboard())
		{
			m_KeybindsTab = new TutorialKeybinds(layoutRoot.FindAnyWidget("Tab_6"), this);
			m_TabScript.EnableTabControl(6, true);
		}
		#endif*/
		
		m_tab_images[0] = ImageWidget.Cast(layoutRoot.FindAnyWidget("MovementTabBackdropImageWidget"));
		m_tab_images[1] = ImageWidget.Cast(layoutRoot.FindAnyWidget("WeaponsAndActionsBackdropImageWidget"));
		m_tab_images[2] = ImageWidget.Cast(layoutRoot.FindAnyWidget("InventoryTabBackdropImageWidget"));
		m_tab_images[3] = ImageWidget.Cast(layoutRoot.FindAnyWidget("MenusTabBackdropImageWidget"));
		
		UpdateControlsElements();
		UpdateControlsElementVisibility();
		
		PPERequesterBank.GetRequester(PPERequester_TutorialMenu).Start(new Param1<float>(0.6));
		DrawConnectingLines(0);
		
		g_Game.GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		g_Game.GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		
		return layoutRoot;
	}
	
	void ~TutorialsMenu()
	{
		m_TabScript.m_OnTabSwitch.Remove(DrawConnectingLines);
		PPERequesterBank.GetRequester(PPERequester_TutorialMenu).Stop();
	}
	
	protected bool IsLowResTutorialMode()
	{
		int screenW, screenH;
		GetScreenSize(screenW, screenH);
		return screenW <= 1280 || screenH <= 720;
	}
	
	protected float GetTutorialLineScale()
	{
		int screenW, screenH;
		GetScreenSize(screenW, screenH);
	
		if (screenW <= 0 || screenH <= 0)
			return MIN_LINE_SCALE;
	
		float widthScale = WITDH_SCALE_RES / screenW;
		float heightScale = HEIGHT_SCALE_RES / screenH;
		float scale = Math.Max(widthScale, heightScale);
		scale = Math.Clamp(scale, MIN_LINE_SCALE, MAX_LINE_SCALE);
		
		return scale;
	}
	
	protected float GetTutorialLineThickness()
	{
		return LINE_THICKNESS_BASE * GetTutorialLineScale();
	}
	
	protected float GetTutorialBranchOffset()
	{
		return BRANCH_OFFSET_BASE * GetTutorialLineScale();
	}
	
	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		RefreshControlMappings();
		/*if (m_KeybindsTab)
		{
			m_KeybindsTab.Rebuild();
		}*/
		#endif
	}
	
	protected void RefreshControlMappings()
	{
		UpdateControlsElements();
		UpdateControlsElementVisibility();
		DrawConnectingLines(m_CurrentTabIndex);
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		bool mk = m_Input.IsEnabledMouseAndKeyboard();
		bool mkServer = m_Input.IsEnabledMouseAndKeyboardEvenOnServer();

		switch (pInputDeviceType)
		{
		case EInputDeviceType.CONTROLLER:
			if (mk && mkServer)
			{
				g_Game.GetUIManager().ShowUICursor(false);
			}
		break;

		default:
			if (mk && mkServer)
			{
				g_Game.GetUIManager().ShowUICursor(true);
			}
		break;
		}
		
		RefreshControlMappings();
		/*if (m_KeybindsTab)
		{
			m_KeybindsTab.Rebuild();
		}*/
	}
	
	override void OnShow()
	{
		super.OnShow();
		
		SetFocus(null);
		OnInputDeviceChanged(m_Input.GetCurrentInputDevice());
	}
	
	void Back()
	{
		g_Game.GetUIManager().Back();
	}
	
	void DrawConnectingLines(int index)
	{
		m_CurrentTabIndex = index;
		
		if (index == 6)
		{
			m_InfoTextLeft.Show(false);
			m_InfoTextRight.Show(false);
		}
		else
		{
			m_InfoTextLeft.Show(true);
			m_InfoTextRight.Show(true);
			array<ref JsonControlMappingInfo> controlMappingInfo = new array<ref JsonControlMappingInfo>;
			array<ref array <ref JsonControlMappingInfo>> tabArray = new array<ref array <ref JsonControlMappingInfo>>;
			
			map<string, ref array<int>> buttonMarkerGroupsUnfiltered = new map<string, ref array<int>>;
			map<string, ref array<int>> buttonMarkerGroups = new map<string, ref array<int>>;
			
			float textWidgetPosX, textWidgetPosY;
			float textWidgetWidth, textWidgetHeight;
			float dotPosX, dotPosY;
			float dotWidth, dotHeight;
			float drawPosX, drawPosY;
			float lineThickness;
			float branchOffset;
			
			CanvasWidget canvasWidget = CanvasWidget.Cast(layoutRoot.FindAnyWidget("CanvasWidget_" + index));
			canvasWidget.Clear();
			lineThickness = GetTutorialLineThickness();
			branchOffset = GetTutorialBranchOffset();
			controlMappingInfo = GetControlMappingInfo();
			
			for (int i = 0; i < m_TabScript.GetTabCount(); i++)
			{
				tabArray.Insert(new array<ref JsonControlMappingInfo>);
				for (int j = 0; j < 30; j++)
				{
					tabArray[i].Insert(NULL);
				}
			}
			
			// insert json info to array by index, so it is sorted
			int controlMappingInfoCount = controlMappingInfo.Count();
			for (i = 0; i < controlMappingInfoCount; i++)
			{
				JsonControlMappingInfo info = controlMappingInfo.Get(i);
				tabArray[info.m_TabID][info.m_TextWidgetID] = info;
			}
			
			// create group of buttons which are connected together with line
			for (int l = 0; l < controlMappingInfoCount; l++)
			{
				JsonControlMappingInfo info1 = controlMappingInfo[l];
				string buttonName = info1.m_ButtonName;
				int textWidgetID = info1.m_TextWidgetID;
				if (info1.m_TabID != index)
				{
					continue;
				}
				if (!buttonMarkerGroupsUnfiltered.Contains(buttonName))
				{
					buttonMarkerGroupsUnfiltered.Insert(buttonName, new array<int>);
					buttonMarkerGroupsUnfiltered.Get(buttonName).Insert(textWidgetID);
				}
				else
				{
					buttonMarkerGroupsUnfiltered.Get(buttonName).Insert(textWidgetID);
				}
			}
			
			// we want groups which are bigger than 1
			for (l = 0; l < buttonMarkerGroupsUnfiltered.Count(); l++)
			{
				if (buttonMarkerGroupsUnfiltered.GetElement(l).Count() > 1)
				{
					string key = buttonMarkerGroupsUnfiltered.GetKey(l);
					buttonMarkerGroups.Insert(buttonMarkerGroupsUnfiltered.GetKey(l), buttonMarkerGroupsUnfiltered.Get(key));
				}
			}
			
			// hide all button markers
			Widget xboxControlsImage = layoutRoot.FindAnyWidget("Markers_" + index);
			
			Widget panelWidget;
			Widget buttonMarkerWidget;
			
			for (l = 0; l < tabArray[index].Count(); l++)
			{
				panelWidget = layoutRoot.FindAnyWidget("PanelWidget" + l);
				if (tabArray[index][l] != NULL)
				{
					RichTextWidget textWidget = RichTextWidget.Cast(panelWidget.FindAnyWidget("TextWidget" + l));
					buttonMarkerWidget = layoutRoot.FindAnyWidget("button_marker_" + tabArray[index][l].m_ButtonName);
						
					if (m_LowResTutorialMode)
						textWidget.SetTextExactSize(EXACT_TEXT_SIZE_LOW_RES);
					
					textWidget.SetText(tabArray[index][l].m_InfoText);
					
					panelWidget.Show(true);
					panelWidget.Update();
					
					if (!buttonMarkerGroups.Contains(tabArray[index][l].m_ButtonName))
					{
						panelWidget.GetScreenPos(textWidgetPosX, textWidgetPosY);
						panelWidget.GetScreenSize(textWidgetWidth,textWidgetHeight);
						
						buttonMarkerWidget.GetScreenPos(dotPosX, dotPosY);
						buttonMarkerWidget.GetScreenSize(dotWidth, dotHeight);
						
						drawPosY = textWidgetPosY + textWidgetHeight / 2;
						
						if (l < 15)
						{
							drawPosX = textWidgetPosX + textWidgetWidth - 1;
						}
						else
						{
							drawPosX = textWidgetPosX;
						}
						
						canvasWidget.DrawLine(drawPosX, drawPosY, dotPosX + dotWidth / 2, drawPosY, lineThickness, ARGBF(0.6, 1, 1, 1));
						canvasWidget.DrawLine(dotPosX + dotWidth / 2, drawPosY, dotPosX + dotWidth / 2, dotPosY + dotHeight / 2, lineThickness, ARGBF(0.6, 1, 1, 1));
					}
				}
				else
				{
					panelWidget.Show(false);
				}
				panelWidget.Update();
			}
			
			// draw connecting lines 
			for (l = 0; l <  buttonMarkerGroups.Count(); l++)
			{
				textWidgetPosX = 0;
				textWidgetPosY = 0;
				textWidgetWidth = 0;
				textWidgetHeight = 0;
				float groupPointX = 0, groupPointY = 0;
				float firstX = 0, firstY = 0;
				
				array<int> element = buttonMarkerGroups.GetElement(l);
				string keyName = buttonMarkerGroups.GetKey(l);
				buttonMarkerWidget = layoutRoot.FindAnyWidget("button_marker_" + keyName);
				
				int elementsCount = element.Count();
				for (int g = 0; g < elementsCount; g++)
				{
					panelWidget = layoutRoot.FindAnyWidget("PanelWidget" + element[g]);
					
					panelWidget.GetScreenPos(textWidgetPosX, textWidgetPosY);
					panelWidget.GetScreenSize(textWidgetWidth, textWidgetHeight);
					
					if (g == 0)
					{
						if (element[0] < 15)
						{
							firstX = textWidgetPosX + textWidgetWidth + branchOffset;
						}
						else
						{
							firstX = textWidgetPosX - branchOffset;
						}
						firstY = textWidgetPosY + textWidgetHeight/2;
						
					}
					
					groupPointX += textWidgetPosX;
					groupPointY += textWidgetPosY;
					
					if (element[0] < 15)
					{
						canvasWidget.DrawLine(textWidgetPosX + textWidgetWidth - 1, textWidgetPosY + textWidgetHeight / 2, textWidgetPosX + textWidgetWidth + branchOffset, textWidgetPosY + textWidgetHeight / 2, lineThickness, ARGBF(0.6, 1, 1, 1));
					}
					else
					{
						canvasWidget.DrawLine(textWidgetPosX, textWidgetPosY + textWidgetHeight / 2, textWidgetPosX - branchOffset, textWidgetPosY + textWidgetHeight / 2, lineThickness, ARGBF(0.6, 1, 1, 1));
					}
				}
				
				if (element[0] < 15)
				{
					groupPointX = groupPointX / elementsCount + textWidgetWidth + branchOffset;
				}
				else
				{
					groupPointX = groupPointX / elementsCount - branchOffset;
				}
				
				groupPointY = groupPointY/elementsCount + textWidgetHeight/2;
				
				buttonMarkerWidget.GetScreenPos(dotPosX, dotPosY);
				buttonMarkerWidget.GetScreenSize(dotWidth, dotHeight);
				
				canvasWidget.DrawLine(groupPointX, groupPointY, dotPosX + dotWidth / 2, groupPointY, lineThickness, ARGBF(0.6, 1, 1, 1));
				canvasWidget.DrawLine(dotPosX + dotWidth / 2, groupPointY, dotPosX + dotWidth / 2, dotPosY, lineThickness, ARGBF(0.6, 1, 1, 1));
				
				if (element[0] < 15)
				{
					canvasWidget.DrawLine(firstX, firstY, textWidgetPosX + textWidgetWidth + branchOffset, textWidgetPosY + textWidgetHeight / 2, lineThickness, ARGBF(0.6, 1, 1, 1));
				}
				else
				{
					canvasWidget.DrawLine(firstX, firstY, textWidgetPosX - branchOffset, textWidgetPosY + textWidgetHeight / 2, lineThickness, ARGBF(0.6, 1, 1, 1));
				}
			}
		}
	}
	
	protected array<ref JsonControlMappingInfo> GetControlMappingInfo()
	{
		array<ref JsonControlMappingInfo> controlMappingInfo = new array<ref JsonControlMappingInfo>;
		string filePath;
		
		//! PC variant (also used for MSS build when mouse and keyboard are then active input device).
		if (!m_LowResTutorialMode)
			filePath = PATH_MOUSEKEY;
		else
			filePath = PATH_MOUSEKEY_720P;
		
		string profileName = "";
		m_Input.GetProfileName(m_Input.GetCurrentProfile(), profileName);
		
	#ifdef PLATFORM_CONSOLE
		bool controllerActive = m_Input.GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		if (controllerActive)
		{
			if (profileName == "#STR_UAPRESET_0" || profileName == "#STR_USER")
			{
				#ifdef PLATFORM_MSSTORE
				if (!m_LowResTutorialMode)
					filePath =	PATH_X1_OLD;
				else
					filePath =	PATH_X1_OLD_720P;
				#endif
				#ifdef PLATFORM_XBOX
				if (!m_LowResTutorialMode)
					filePath =	PATH_X1_OLD;
				else
					filePath =	PATH_X1_OLD_720P;
				#endif
				#ifdef PLATFORM_PS4
					filePath =	PATH_PS_OLD;
				#endif
			}
			else if (profileName == "#STR_UAPRESET_1")
			{
				#ifdef PLATFORM_MSSTORE
				if (!m_LowResTutorialMode)
					filePath =	PATH_X1_NEW;
				else
					filePath =	PATH_X1_NEW_720p;
				#endif
				#ifdef PLATFORM_XBOX
				if (!m_LowResTutorialMode)
					filePath =	PATH_X1_NEW;
				else
					filePath =	PATH_X1_NEW_720p;
				#endif
				#ifdef PLATFORM_PS4
					filePath =	PATH_PS_NEW;
				#endif
			}
			else
			{
				ErrorEx("Invalid file path!");
				filePath =	"";
			}
		}
	#endif
		FileHandle file_handle = OpenFile(filePath, FileMode.READ);
		JsonSerializer js = new JsonSerializer();
		
		string js_error = "";
		string line_content = "";
		string content = "";
		if (file_handle)
		{
			while (FGets(file_handle,  line_content) >= 0)
			{
				content += line_content;
			}
			CloseFile(file_handle);
			
			if (js.ReadFromString(controlMappingInfo, content, js_error))
			{
				return controlMappingInfo;
			}
			else
			{
				ErrorEx("JSON ERROR => [TutorialsMenu]: " + js_error);
			}
		}
		else
		{
			ErrorEx("FILEHANDLE ERROR => [TutorialsMenu]: " + js_error);
		}
		
		return controlMappingInfo;
	}
	
	override void Update(float timeslice)
	{
		if (GetUApi().GetInputByID(UAUITabLeft).LocalPress())
		{
			m_TabScript.PreviousTab();
		}
		
		//RIGHT BUMPER - TAB RIGHT
		if (GetUApi().GetInputByID(UAUITabRight).LocalPress())
		{
			m_TabScript.NextTab();
		}
		
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			Back();
		}
	}
	
		/// Initial texts load for the footer buttons
	protected void LoadFooterButtonTexts()
	{
		TextWidget uiBackText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("BackText"));		
		
		if (uiBackText)
		{
			uiBackText.SetText(m_BackButtonTextID);
		}
	}
	/// Set correct bottom button texts based on platform (ps4 vs xbox texts)
	protected void LoadTextStrings()
	{
		#ifdef PLATFORM_PS4
			m_BackButtonTextID = "ps4_ingame_menu_back";
		#else 
			m_BackButtonTextID = "STR_rootFrame_toolbar_bg_ConsoleToolbar_Back_BackText0";	
		#endif
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
	
	//Coloring functions (Until WidgetStyles are useful)
	void ColorHighlight(Widget w)
	{
		if (!w)
			return;
		
		int color_pnl = ARGB(255, 0, 0, 0);
		int color_lbl = ARGB(255, 255, 0, 0);
		int color_img = ARGB(255, 200, 0, 0);
		
		#ifdef PLATFORM_CONSOLE
			color_pnl = ARGB(255, 200, 0, 0);
			color_lbl = ARGB(255, 255, 255, 255);
		#endif
		
		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
		ImagenSetColor(w, color_img);
	}
	
	void ColorNormal(Widget w)
	{
		if (!w)
			return;
		
		int color_pnl = ARGB(0, 0, 0, 0);
		int color_lbl = ARGB(255, 255, 255, 255);
		int color_img = ARGB(255, 255, 255, 255);
		
		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
		ImagenSetColor(w, color_img);
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
	
	void ImagenSetColor(Widget w, int color)
	{
		if (!w)
			return;
		
		Widget panel = w.FindWidget(w.GetName() + "_image");
		
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
		#ifdef PLATFORM_CONSOLE
		RichTextWidget toolbar_text = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ContextToolbarText"));
		if (toolbar_text)
		{
			string text = string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#STR_settings_menu_root_toolbar_bg_ConsoleToolbar_Back_BackText0", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
			toolbar_text.SetText(text);
		}
		#endif
	}
	
	protected void UpdateControlsElementVisibility()
	{
		bool toolbarShow = false;
		#ifdef PLATFORM_CONSOLE
		toolbarShow = !m_Input.IsEnabledMouseAndKeyboardEvenOnServer() || m_Input.GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		#endif
		
		#ifdef PLATFORM_CONSOLE
		layoutRoot.FindAnyWidget("toolbar_bg").Show(toolbarShow);
		#endif
		layoutRoot.FindAnyWidget("play_panel_root").Show(!toolbarShow);
	}
}
