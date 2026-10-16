enum ScriptConsoleTabID
{
	GENERAL,
	ITEMS,
	CAMERA,
	CONFIGS,
	OUTPUT,
	SCRIPT_CLIENT,
	SCRIPT_SERVER,
	SOUNDS,
	VICINITY,
	WEATHER,
	
	COUNT   //! KEEP AS LAST ENTRY!
}

class ScriptConsoleTabRegistryEntry
{
	int m_ID;
	string m_DisplayName;
	string m_PanelWidgetName;
	typename m_TabClass;
	
	void ScriptConsoleTabRegistryEntry(int id, string displayName, string panelWidgetName, typename tabClass)
	{
		m_ID = id;
		m_DisplayName = displayName;
		m_PanelWidgetName = panelWidgetName;
		m_TabClass = tabClass;
	}
}

class ScriptConsoleTabRegistry
{
	protected static ref array<ref ScriptConsoleTabRegistryEntry> s_Entries;
	
	static void Register(int id, string displayName, string widgetName, typename tabClass)
	{
		if (!s_Entries)
			s_Entries = new array<ref ScriptConsoleTabRegistryEntry>();
			
		s_Entries.Insert(new ScriptConsoleTabRegistryEntry(id, displayName, widgetName, tabClass));
	}
	
	static array<ref ScriptConsoleTabRegistryEntry> GetAll()
	{
		return s_Entries;
	}
	
	static void InitializeRegistry()
	{
		if (s_Entries && s_Entries.Count() > 0)
			s_Entries.Clear();
			
		// Register new tabs here
		Register(ScriptConsoleTabID.GENERAL, "General", "GeneralPanel", ScriptConsoleGeneralTab);
		Register(ScriptConsoleTabID.ITEMS, "Items", "ItemsPanel", ScriptConsoleItemsTab);
		Register(ScriptConsoleTabID.CAMERA, "Camera", "CameraPanel", ScriptConsoleCameraTab);
		Register(ScriptConsoleTabID.CONFIGS, "Configs", "ConfigsPanel", ScriptConsoleConfigTab);
		Register(ScriptConsoleTabID.OUTPUT, "Prints", "OutputPanel", ScriptConsoleOutputTab);
		Register(ScriptConsoleTabID.SCRIPT_CLIENT, "EnfScript Client", "EnScriptPanel", ScriptConsoleEnfScriptTab);
		Register(ScriptConsoleTabID.SCRIPT_SERVER, "EnfScript Server", "EnScriptPanel", ScriptConsoleEnfScriptServerTab);
		Register(ScriptConsoleTabID.SOUNDS, "Sounds", "SoundsPanel", ScriptConsoleSoundsTab);
		Register(ScriptConsoleTabID.VICINITY, "Vicinity", "VicinityPanel", ScriptConsoleVicinityTab);
		Register(ScriptConsoleTabID.WEATHER, "Weather", "WeatherPanel", ScriptConsoleWeatherTab);
	}
}

class ScriptConsole extends UIScriptedMenu
{
	protected static const string HINTS_PATH_DEFAULT	= "scripts/data/internal/script_console_hints.json";
	protected static const string HINTS_PATH_OPTIONAL 	= "$mission:script_console_hints.json";
	const string NO_HINT_TEXT 							= "No hint";
	
	// Widgets
	protected Widget 			m_CurrentHoverWidget;
	protected Widget 			m_HintWidgetRoot;
	protected ImageWidget 		m_HintWidgetBackground;
	protected Widget 			m_EditTooltipRoot;
	protected RichTextWidget 	m_HintWidget;
	protected ButtonWidget 		m_HintOkButton;
	protected ButtonWidget 		m_HintCancelButton;
	protected ButtonWidget 		m_HintClearButton;
	protected EditBoxWidget 	m_HintInputText;
	
	protected Widget			m_HeaderPanelPC;
	protected GridSpacerWidget	m_ButtonsMenu;
	protected ButtonWidget 		m_CloseConsoleButton;
	protected GridSpacerWidget	m_ContextMenuGrid;
	protected ButtonWidget		m_ContextMenuButton;
	protected TextWidget		m_ContectMenuButtonLabel;
	#ifdef PLATFORM_CONSOLE
	protected Widget 			m_HeaderPanelConsole;
	protected GridSpacerWidget 	m_ConsoleButtonsMenuGrid;
	#endif
	
	// Member variables
	protected bool				m_HintEditMode;
	protected float 			m_HoverTime;
	protected bool				m_HoverSuccessTriggered;
	protected float 			m_PrevMouseX;
	protected float 			m_PrevMouseY;
	ScriptConsoleTabBase 		m_SelectedHandler;
	protected int 				m_SelectedTab = ScriptConsoleTabID.GENERAL;
	
	static ref array<ref MapMarker> m_MarkedEntities = new array<ref MapMarker>();
	protected ref map<int, ref ScriptConsoleTabBase> m_TabHandlersByID = new map<int, ref ScriptConsoleTabBase>();
	
	static ref JsonHintsData m_JsonData;
	
	protected ref map<int, ref ScriptConsoleMenuElement> m_ContextMenuElements = new map<int, ref ScriptConsoleMenuElement>();
	protected EInputDeviceType m_CurrentInputDevice;
		
	void ScriptConsole()
	{
		#ifndef SERVER
		#ifndef NO_GUI
		if (g_Game && g_Game.GetMission() && g_Game.GetMission().GetHud())
		{
			g_Game.GetMission().GetHud().ShowHudPlayer(false);
			g_Game.GetMission().GetHud().ShowQuickbarPlayer(false);
			g_Game.GetMission().GetHud().HideVehicleInfo();
		}
		#endif
		#endif
		PluginItemDiagnostic plugin = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));
		if (plugin)
			plugin.OnScriptMenuOpened(true);
	}

	void ~ScriptConsole()
	{
		#ifndef SERVER
		#ifndef NO_GUI
		if (g_Game && g_Game.GetMission() && g_Game.GetMission().GetHud())
		{

			g_Game.GetMission().GetHud().ShowHudPlayer(true);
			g_Game.GetMission().GetHud().ShowQuickbarPlayer(true);
			g_Game.GetMission().GetHud().ShowVehicleInfo();
			g_Game.GetMission().EnableAllInputs(true);
			
		}
		if (m_HintWidgetRoot)
			m_HintWidgetRoot.Unlink();
		#endif
		#endif
		PluginItemDiagnostic plugin = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));
		if (plugin)
			plugin.OnScriptMenuOpened(false);
	}
	
	
	ScriptConsoleTabBase GetSelectedHandler()
	{
		return m_SelectedHandler;
	}
	
	static void SaveData()
	{
		string errorMessage;
		if (!JsonFileLoader<JsonHintsData>.SaveFile(HINTS_PATH_OPTIONAL, m_JsonData, errorMessage))
			ErrorEx(errorMessage);
	}
	
	protected static JsonHintsData GetData()
	{
		string path = HINTS_PATH_OPTIONAL;
		if (!FileExist(path))
		{
			path = HINTS_PATH_DEFAULT;
			//ErrorEx(string.Format("Using default hints file: %1", path), ErrorExSeverity.INFO);
		}
		
		string errorMessage;
		JsonHintsData data;
		if (!JsonFileLoader<JsonHintsData>.LoadFile(path, data, errorMessage))
			ErrorEx(errorMessage);
		
		return data;
	}
	
	void SetHintText(string text, Widget w)
	{
		if (m_JsonData && m_JsonData.WidgetHintBindings && w)
		{
			int hash = GetWidgetCombinedHash(w);
			m_JsonData.WidgetHintBindings.Set(hash, text);
		}
		HoverInterrupt();
	}
	
	void RegisterTab(int id, ScriptConsoleTabBase handler)
	{
		handler.Init(id);
		m_TabHandlersByID.Insert(id, handler);
	}
	
	override Widget Init()
	{
		m_ConfigDebugProfile = PluginConfigDebugProfile.Cast(GetPlugin(PluginConfigDebugProfile));

		layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console.layout");
		m_EditTooltipRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console_tooltip_edit.layout", layoutRoot);
		m_EditTooltipRoot.Show(false);
		m_HintOkButton = ButtonWidget.Cast(m_EditTooltipRoot.FindAnyWidget("ButtonOk"));
		m_HintCancelButton = ButtonWidget.Cast(m_EditTooltipRoot.FindAnyWidget("ButtonCancel"));
		m_HintClearButton = ButtonWidget.Cast(m_EditTooltipRoot.FindAnyWidget("ButtonClear"));
		m_HintInputText = EditBoxWidget.Cast(m_EditTooltipRoot.FindAnyWidget("InputText"));
		
		m_HeaderPanelPC = layoutRoot.FindAnyWidget("HeaderBarPC");
		m_ButtonsMenu = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("ButtonsContextMenu"));
		m_ContextMenuGrid = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("ContextMenuGrid"));
		m_ContextMenuButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonContextMenu"));
		m_ContectMenuButtonLabel = TextWidget.Cast(layoutRoot.FindAnyWidget("ButtonContextMenuLabel"));
		m_CloseConsoleButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CloseConsoleButton"));
		#ifdef PLATFORM_CONSOLE
		m_HeaderPanelPC.Show(false);
		m_ButtonsMenu.Show(false);
		
		m_HeaderPanelConsole = layoutRoot.FindAnyWidget("HeaderBarConsole");
		m_ConsoleButtonsMenuGrid = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("TabButtonsGrid"));
		
		m_HeaderPanelConsole.Show(true);
		#endif
		
		BuildContextMenu();
		
		// load data from profile
		int lastOpenedTab = m_ConfigDebugProfile.GetTabSelected();
		if (lastOpenedTab >= 0 && lastOpenedTab < ScriptConsoleTabID.COUNT)
			m_SelectedTab = m_ConfigDebugProfile.GetTabSelected();
		SelectTabByID(m_SelectedTab);
		
		m_JsonData = GetData();
		
		#ifdef PLATFORM_CONSOLE
		g_Game.GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		OnInputDeviceChanged(g_Game.GetInput().GetCurrentInputDevice());
		#endif
		
		return layoutRoot;
	}
	
	#ifdef PLATFORM_CONSOLE
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		m_CurrentInputDevice = pInputDeviceType;
		BuildContextMenu();
		
		switch (pInputDeviceType)
		{
			case EInputDeviceType.CONTROLLER:
			{
				m_HeaderPanelPC.Show(false);
				m_ButtonsMenu.Show(false);
				m_HeaderPanelConsole.Show(true);
				g_Game.GetUIManager().ShowUICursor(false);
				g_Game.GetMission().RemoveActiveInputExcludes({"menu_move"});
				break;
			}
			default: // Mouse & Keyboard
			{
				m_HeaderPanelConsole.Show(false);
				m_HeaderPanelPC.Show(true);
				m_ButtonsMenu.Show(true);
				g_Game.GetUIManager().ShowUICursor(true);
				g_Game.GetMission().AddActiveInputExcludes({"menu_move"});
				break;
			}
		}
		
		// Notify current tab of input change
		if (m_SelectedHandler)
			m_SelectedHandler.OnInputDeviceChanged(pInputDeviceType);
	}
	#endif
	
	protected void BuildContextMenu()
	{
		if (m_ContextMenuElements && m_ContextMenuElements.Count() > 0)
		{
			foreach (ScriptConsoleMenuElement element : m_ContextMenuElements)
			{
				element.GetRoot().Unlink();
				delete element.GetRoot();
			}
			
			m_ContextMenuElements.Clear();
			m_ContextMenuElements = new map<int, ref ScriptConsoleMenuElement>();
		}
		
		ScriptConsoleTabRegistry.InitializeRegistry();
		
		array<ref ScriptConsoleTabRegistryEntry> tabEntries = ScriptConsoleTabRegistry.GetAll();
		foreach (ScriptConsoleTabRegistryEntry entry : tabEntries)
		{
			RegisterContextMenuElement(entry.m_ID, entry.m_DisplayName);
		}
	}
	
	protected void RegisterContextMenuElement(int id, string name)
	{
		GridSpacerWidget buttonsGrid = m_ContextMenuGrid;
		#ifdef PLATFORM_CONSOLE
		if (m_CurrentInputDevice == EInputDeviceType.CONTROLLER)
			buttonsGrid = m_ConsoleButtonsMenuGrid;
		else
			buttonsGrid = m_ContextMenuGrid;
		#endif
		ScriptConsoleMenuElement menuElement = new ScriptConsoleMenuElement(id, buttonsGrid, name, this);
		m_ContextMenuElements.Insert(id, menuElement);
		if (m_SelectedHandler && m_SelectedHandler.GetID() == id)
			menuElement.Select();
	}
		
	protected void HideContextSubElements()
	{
		foreach (ScriptConsoleMenuElement element : m_ContextMenuElements)
		{
			if (element.HasSubElements())
			{
				element.HideGrid();
			}
		}
	}
	
	protected bool IsContextElementButton(Widget w)
	{
		if (w == m_ContextMenuButton)
			return true;
	
		foreach (ScriptConsoleMenuElement element : m_ContextMenuElements)
		{
			if (w == element.GetButton())
				return true;
			// Also check sub-elements
			if (element.IsSubElementButton(w))
				return true;
		}
	
		return false;
	}
	
	protected void HideHint()
	{
		if (m_HintWidgetRoot)
			m_HintWidgetRoot.Unlink();
	}
	
	int GetWidgetCombinedHash(Widget w)
	{
		string nameThis = w.GetName();
		string nameParent = "";
		
		if (w.GetParent())
		{
			nameParent = w.GetParent().GetName();
		}
		
		string namesCombined = nameThis + nameParent;
		return namesCombined.Hash();
	}
	
	protected string GetMessage()
	{
		int hash = GetWidgetCombinedHash(m_CurrentHoverWidget);
		
		if (m_JsonData && m_JsonData.WidgetHintBindings)
		{
			if (m_JsonData.WidgetHintBindings.Contains(hash))
			{
				return m_JsonData.WidgetHintBindings.Get(hash);
			}
		}
		//return "";
		//return "No hint" + hash.ToString();
		return NO_HINT_TEXT;
	}
	
	protected void HoverSuccess()
	{
		m_HoverSuccessTriggered = true;
		DisplayHint(GetMessage());
	}
	
	protected void HoverInterrupt()
	{
		m_HoverSuccessTriggered = false;

		m_HoverTime = 0;
		m_HintEditMode = false;
		HideHint();
	}
	
	override void OnShow()
	{
		super.OnShow();
		
		if (g_Game && g_Game.GetMission())
		{
			// Inputs excluded when in script console menu
			array<string> inputExcludes = {
				// Menu
				"movement",
				"aiming",
				"gestures",
				"stances",
				"optics",
				"actions", 
				"hotkey",
				"UAGear",
				"UAVoiceLevel",
				"UAVoiceModifierHelper",
				"UAVoiceDistanceUp",
				"UAVoiceDistanceDown",
				"UAVoiceOverNet",
				"UAVoiceOverNetToggle",
				"UAVoiceOverNetMute",
				// Inventory
				"gestures",
	    		"stances",
	    		"optics",
	   			"actions",
				// Buldozer
				"UABuldResetCamera",
				"UABuldTurbo",
				"UABuldSlow",
				"UABuldRunScript",
				"UABuldSelectToggle",
				"UABuldFreeLook",
				"UABuldSelect",
				"UABuldSelectAddMod",
				"UABuldSelectRemoveMod",
				"UABuldModifySelected",
				"UABuldCycleMod",
				"UABuldRotationXAxisMod",
				"UABuldRotationZAxisMod",
				"UABuldCoordModCycle",
				"UABuldSampleTerrainHeight",
				"UABuldSetTerrainHeight",
				"UABuldScaleMod",
				"UABuldElevateMod",
				"UABuldSmoothMod",
				"UABuldFlattenMod",
				"UABuldBrushRatioUp",
				"UABuldBrushRatioDown",
				"UABuldBrushOuterUp",
				"UABuldBrushOuterDown",
				"UABuldBrushStrengthUp",
				"UABuldBrushStrengthDown",
				"UABuldToggleNearestObjectArrow",
				"UABuldCycleBrushMod",
				"UABuldSelectionType",
				"UABuldCreateLastSelectedObject",
				"UABuldDuplicateSelection",
				"UABuldDeleteSelection",
				"UABuldUndo",
				"UABuldRedo",
				"UABuldMoveLeft",
				"UABuldMoveRight",
				"UABuldMoveForward",
				"UABuldMoveBack",
				"UABuldMoveUp",
				"UABuldMoveDown",
				"UABuldLeft",
				"UABuldRight",
				"UABuldForward",
				"UABuldBack",
				"UABuldLookLeft",
				"UABuldLookRight",
				"UABuldLookUp",
				"UABuldLookDown",
				"UABuldZoomIn",
				"UABuldZoomOut",
				"UABuldTextureInfo",
				"UABuldViewerMoveForward",
				"UABuldViewerMoveBack",
				"UABuldViewerMoveLeft",
				"UABuldViewerMoveRight",
				"UABuldViewerMoveUp",
				"UABuldViewerMoveDown",
				"UABuldObjectRotateLeft",
				"UABuldObjectRotateRight",
				"UABuldObjectRotateForward",
				"UABuldObjectRotateBack",
				"UABuldPreviousAnimation",
				"UABuldNextAnimation",
				"UABuldRecedeAnimation",
				"UABuldAdvanceAnimation",
				"UABuldLinkCamToTerrain",
				"UABuldCopyTileCoord",
				"UABuldMarkUnderground",
				"UABuldRemoveUnderground"
			};
			
			if (m_SelectedHandler && ScriptConsoleCameraTab.Cast(m_SelectedHandler))
			{
				inputExcludes.Remove(0);
			}
					
			g_Game.GetMission().AddActiveInputExcludes(inputExcludes);
		}
	}
		
	override void OnHide()
	{
		super.OnHide();
				
		HoverInterrupt();
	}

	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		super.OnKeyPress(w, x, y, key);

		if (m_SelectedHandler && m_SelectedHandler.OnKeyPress(w,x,y,key))
			return true;
		return false;

	}
	
	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		super.OnKeyDown(w, x, y, key);
		
		if (m_SelectedHandler && m_SelectedHandler.OnKeyDown(w,x,y,key))
			return true;
		return false;

	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		int mouseX, mouseY;
		GetMousePos(mouseX,mouseY);
		float dist = Math.Sqrt(Math.AbsFloat(mouseX - m_PrevMouseX) + Math.AbsFloat(mouseY - m_PrevMouseY));
		m_PrevMouseX = mouseX;
		m_PrevMouseY = mouseY;
		
		if (dist < 1 && m_CurrentHoverWidget && !m_HoverSuccessTriggered)
		{
			m_HoverTime += timeslice;
			if (m_HoverTime > 1)
			{
				HoverSuccess();
			}
		}
		
		if(dist > 1 && m_HoverSuccessTriggered)
			HoverInterrupt();
			
		if (!g_Game.IsMultiplayer() && KeyState(KeyCode.KC_RCONTROL) && KeyState(KeyCode.KC_NUMPAD0) && m_HintWidgetRoot && m_HintWidgetRoot.IsVisible())
		{
			ClearKey(KeyCode.KC_NUMPAD0);
			m_EditTooltipRoot.Show(true);
			string text = GetMessage();
			if (text == NO_HINT_TEXT)
				text = "";
			m_HintInputText.SetText(text);
		}
		
		foreach (ScriptConsoleTabBase handler : m_TabHandlersByID)
		{
			if (handler.IsSelected())
			{
				handler.Update(timeslice);
			}
		}
		
		UAInputAPI inputAPI = GetUApi();
		if (g_Game && inputAPI.GetInputByID(UAUIBack).LocalPress())
		{
			g_Game.GetUIManager().Back();
			return;
		}
		if (inputAPI.GetInputByID(UAUITabRight).LocalPress())
		{
			m_SelectedTab++;
			if (m_SelectedTab >= ScriptConsoleTabID.COUNT)
				m_SelectedTab = 0;
			
			SelectTabByID(m_SelectedTab);
		}
		else if (inputAPI.GetInputByID(UAUITabLeft).LocalPress())
		{
			m_SelectedTab--;
			if (m_SelectedTab < 0)
				m_SelectedTab = ScriptConsoleTabID.COUNT - 1;
			
			SelectTabByID(m_SelectedTab);
		}
	}
		
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		super.OnMouseButtonDown(w,x,y,button);
		
		if (m_SelectedHandler && m_SelectedHandler.OnMouseButtonDown(w,x,y,button))
			return true;

		return false;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		if (w == m_HintOkButton)
		{
			SetHintText(m_HintInputText.GetText(), m_CurrentHoverWidget);
			HoverInterrupt();
			m_EditTooltipRoot.Show(false);
			SaveData();
		}		
		else if (w == m_HintCancelButton)
		{
			HoverInterrupt();
			m_EditTooltipRoot.Show(false);
			
		}
		else if (w == m_HintClearButton)
		{
			m_HintInputText.SetText("");
		}
		#ifndef PLATFORM_CONSOLE
		else if (w == m_CloseConsoleButton)
		{
			Close();
			g_Game.GetMission().EnableAllInputs(true);
			return true;
		}
		#endif
		
		if (m_SelectedHandler && m_SelectedHandler.OnClick(w,x,y,button))
			return true;
		return false;
	}
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		super.OnDoubleClick(w, x, y, button);

		if (m_SelectedHandler && m_SelectedHandler.OnDoubleClick(w,x,y,button))
				return true; 
		
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		super.OnMouseLeave(w, enterW, x, y);
		
		if (!m_EditTooltipRoot.IsVisible())
			HoverInterrupt();
		
		// Close context menu if we're not entering any context menu related widget
		if (enterW && !IsContextElementButton(enterW) && !IsChildOfWidget(enterW, m_ButtonsMenu) && w && !IsContextElementButton(w) && !IsChildOfWidget(w, m_ButtonsMenu) && enterW != m_ContextMenuGrid && w != m_ContextMenuGrid)
		{
			m_ContextMenuGrid.Show(false);
			HideContextSubElements();
			m_ContectMenuButtonLabel.SetColor(ARGB(255, 255, 255, 255));
		}
		else if (w == m_ContextMenuButton)
		{
			m_ContectMenuButtonLabel.SetColor(ARGB(255, 255, 255, 255));
		}
			
		if (m_SelectedHandler && m_SelectedHandler.OnMouseLeave(w, enterW, x, y))
			return true;
		
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		super.OnMouseEnter(w, x, y);
		
		if (!m_EditTooltipRoot.IsVisible())
			m_CurrentHoverWidget = w;
		
		// Close context menu if we're not entering any context menu related widget
		if (w && !IsContextElementButton(w) && !IsChildOfWidget(w, m_ButtonsMenu) && w != m_ButtonsMenu)
		{
			m_ContextMenuGrid.Show(false);
			HideContextSubElements();
			m_ContectMenuButtonLabel.SetColor(ARGB(255, 255, 255, 255));
		}
		else if (w == m_ContextMenuButton)
		{
			m_ContextMenuGrid.Show(true);
			m_ContectMenuButtonLabel.SetColor(ARGB(255, 0, 0, 0));
		}
		
		if (m_SelectedHandler && m_SelectedHandler.OnMouseEnter(w ,x, y))
			return true;
		
		#ifdef PLATFORM_CONSOLE
		return false;
		#endif
		return true;
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		
		if (m_SelectedHandler && m_SelectedHandler.OnChange(w,x,y,finished))
			return true; 
		
		return false;
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int  column,	int  oldRow, int  oldColumn)
	{
		super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);

		if (m_SelectedHandler && m_SelectedHandler.OnItemSelected(w, x, y, row, column,oldRow, oldColumn))
			return true;
		
		return false;
	}
	
	ScriptConsoleTabBase GetTabHandler(typename tabType)
	{
		foreach (ScriptConsoleTabBase handler : m_TabHandlersByID)
		{
			if (tabType == handler.Type())
			{
				return handler;
			}
		}
		return null;
	}
		
	void SelectTabByID(int tabId)
	{
		ScriptConsoleTabBase tab = m_TabHandlersByID.Get(tabId);
		
		if (!tab)
		{
			// Find the tab entry in registry
			ScriptConsoleTabRegistryEntry foundEntry = null;
			array<ref ScriptConsoleTabRegistryEntry> tabEntries = ScriptConsoleTabRegistry.GetAll();
			foreach (ScriptConsoleTabRegistryEntry entry : tabEntries)
			{
				if (entry.m_ID == tabId)
				{
					foundEntry = entry;
					break;
				}
			}
			
			if (foundEntry)
			{
				// Get the button and panel widget
				ButtonWidget elementButton = GetContextElementButton(tabId);
				Widget panel = layoutRoot.FindAnyWidget(foundEntry.m_PanelWidgetName);
				if (panel && elementButton)
				{
					// Instantiate the tab using typename
					tab = ScriptConsoleTabBase.Cast(foundEntry.m_TabClass.Spawn());
					tab.InitializeTab(panel, this, elementButton);
					
					// Register tab
					RegisterTab(foundEntry.m_ID, tab);
				}
			}
		}
		
		if (tab)
		{
			SelectTab(tab);
			
			foreach (int elementID, ScriptConsoleMenuElement element : m_ContextMenuElements)
			{
				element.SetTextAlignment(m_CurrentInputDevice);
				
				if (elementID == tabId)
				{
					element.Select();
				}
				else
				{
					element.Deselect();
				}
			}
		}
	}
	
	void SelectTab(ScriptConsoleTabBase selectedHandler)
	{
		foreach (ScriptConsoleTabBase handler: m_TabHandlersByID)
		{
			handler.Select(handler == selectedHandler, selectedHandler);
		}
		
		ScriptConsoleCameraTab cameraTab;
		ScriptConsoleWeatherTab weatherTab;
		bool isCameraTab = Class.CastTo(cameraTab, selectedHandler);
		bool isWeatherTab = Class.CastTo(weatherTab, selectedHandler);
		
		if (isCameraTab || isWeatherTab)
		{
			// Remove alpha background for camera tab
			ShowMenuBackground(false);
			if (isCameraTab)
				g_Game.GetMission().RemoveActiveInputExcludes({"movement"}, true);
		}
		else
		{
			// Add back alpha background
			ShowMenuBackground(true);
			g_Game.GetMission().AddActiveInputExcludes({"movement"});
		}
		
		m_SelectedHandler = selectedHandler;
		m_ConfigDebugProfile.SetTabSelected(selectedHandler.GetID());
	}
	
	ButtonWidget GetContextElementButton(int tabID)
	{
		ButtonWidget buttonW;
		ScriptConsoleMenuElement element = m_ContextMenuElements.Get(tabID);
		if (element)
			buttonW = element.GetButton();
		
		return buttonW;
	}
	
	void ShowMenuBackground(bool state)
	{
		if (!state)
		{
			GetLayoutRoot().FindAnyWidget("MenuWindow").SetColor(ARGB(0, 0, 0, 0));
		}
		else
		{
			GetLayoutRoot().FindAnyWidget("MenuWindow").SetColor(ARGB(128, 0, 0, 0));
		}
	}
	
	protected bool IsChildOfWidget(Widget child, Widget parent)
	{
		if (!child || !parent)
			return false;
	
		Widget currentChild = parent.GetChildren();
		while (currentChild)
		{
			if (currentChild == child)
				return true;
			currentChild = currentChild.GetSibling();
		}
		return false;
	}
	
	protected void DisplayHint(string message)
	{
		if (message)
		{
			m_HintWidgetRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console_hint.layout");
			m_HintWidgetBackground = ImageWidget.Cast(m_HintWidgetRoot.FindAnyWidget("Background"));
			m_HintWidget = RichTextWidget.Cast(m_HintWidgetRoot.FindAnyWidget("HintText"));
		
			m_HintWidgetRoot.Show(true);
			m_HintWidget.SetText(message);
			
			int offsetX = 0;
			int offsetY = 10;
			
			int screenW, screenH;
			GetScreenSize(screenW, screenH);
	
			int mouseX, mouseY;
			GetMousePos(mouseX,mouseY);
			
			float relativeX = mouseX / screenW;
			float relativeY = mouseY / screenH;

			int width, height;
			m_HintWidget.GetTextSize(width, height);
			if (relativeX > 0.8)
				offsetX = -width - offsetX;
			if (relativeY > 0.8)
				offsetY = -height - offsetY;
			
			m_HintWidgetRoot.SetPos(mouseX + offsetX ,mouseY + offsetY);
			m_HintWidgetBackground.SetScreenSize(width + 5, height + 5);
			
		}
	}

	override void OnRPCEx(int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPCEx(rpc_type, ctx);
		#ifdef DIAG_DEVELOPER
		
		foreach (ScriptConsoleTabBase handler : m_TabHandlersByID)
		{
			handler.OnRPCEx(rpc_type,ctx);
		}
		#endif

	}
		
	PluginConfigDebugProfile m_ConfigDebugProfile;
}

class ScriptConsoleToolTipEventHandler : ScriptedWidgetEventHandler
{
	reference string HintMessage;
	protected Widget m_Root;
	
	
	protected float 			m_HoverTime;
	protected bool				m_HoverSuccessTriggered;
	protected Widget 			m_CurrentHoverWidget;
	protected Widget 			m_HintWidgetRoot;
	protected ImageWidget 		m_HintWidgetBackground;
	protected RichTextWidget 	m_HintWidget;
	
	protected ref Timer m_Timer;
	
	void OnWidgetScriptInit(Widget w)
	{
		m_Root = w;
		m_Root.SetHandler(this);
		m_Root.SetFlags(WidgetFlags.VEXACTPOS);
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		m_Timer = new Timer();
		m_Timer.Run(0.1, this, "Tick", NULL, true);
		
		m_CurrentHoverWidget = w;
		return true;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{	
		HoverInterrupt();
		return true;
	}
	
	protected bool Tick()
	{
		if (!m_Root.IsVisibleHierarchy())
			HoverInterrupt();
		if (m_CurrentHoverWidget && !m_HoverSuccessTriggered)
		{
			m_HoverTime += 0.1;
			if (m_HoverTime > 1)
			{
				HoverSuccess();
			}
		}
		return true;
	}

	protected void DisplayHint(string message)
	{
		if (message)
		{
			m_HintWidgetRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console_hint.layout");
			m_HintWidgetBackground = ImageWidget.Cast(m_HintWidgetRoot.FindAnyWidget("Background"));
			m_HintWidget = RichTextWidget.Cast(m_HintWidgetRoot.FindAnyWidget("HintText"));
			
			m_HintWidgetRoot.Show(true);
			m_HintWidget.SetText(message);
			
			int offsetX = 0;
			int offsetY = 10;
			
			int screenW, screenH;
			GetScreenSize(screenW, screenH);
	
			int mouseX, mouseY;
			GetMousePos(mouseX,mouseY);
			
			float relativeX = mouseX / screenW;
			float relativeY = mouseY / screenH;

			int width, height;
			m_HintWidget.GetTextSize(width, height);
			if (relativeX > 0.8)
				offsetX = -width - offsetX;
			if (relativeY > 0.8)
				offsetY = -height - offsetY;
			
			m_HintWidgetRoot.SetPos(mouseX + offsetX ,mouseY + offsetY);
			m_HintWidgetBackground.SetScreenSize(width + 5, height + 5);
			
		}
	}
	
	protected void HideHint()
	{
		if (m_HintWidgetRoot)
			m_HintWidgetRoot.Show(false);
	}
	
	
	protected string GetMessage()
	{
		return HintMessage;
	}
	
	protected void HoverSuccess()
	{
		m_HoverSuccessTriggered = true;
		DisplayHint(GetMessage());
	}
	
	protected void HoverInterrupt()
	{
		m_Timer = null;
		m_HoverSuccessTriggered = false;
		m_CurrentHoverWidget = null;
		m_HoverTime = 0;
		HideHint();
	}
}

class JsonHintsData
{
	ref map<int, string> WidgetHintBindings;
}

