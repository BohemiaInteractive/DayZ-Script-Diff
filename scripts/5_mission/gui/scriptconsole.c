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
			m_HintWidgetRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console_hint.layout");
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


class ScriptConsole extends UIScriptedMenu
{
	protected bool				m_HintEditMode;
	protected float 			m_HoverTime;
	protected bool				m_HoverSuccessTriggered;
	protected Widget 			m_CurrentHoverWidget;
	protected Widget 			m_HintWidgetRoot;
	protected ImageWidget 		m_HintWidgetBackground;
	protected Widget 			m_EditTooltipRoot;
	protected RichTextWidget 	m_HintWidget;
	protected ButtonWidget 		m_HintOkButton;
	protected ButtonWidget 		m_HintCancelButton;
	protected ButtonWidget 		m_HintClearButton;
	protected EditBoxWidget 	m_HintInputText;
	protected float 			m_PrevMouseX;
	protected float 			m_PrevMouseY;
	
	//-------------------------------------
	static const int TAB_GENERAL = 0;
	static const int TAB_ITEMS = 1;
	static const int TAB_CONFIGS = 2;
	static const int TAB_ENSCRIPT = 3;	
	static const int TAB_ENSCRIPT_SERVER = 4;
	static const int TAB_OUTPUT = 5;
	static const int TAB_VICINITY = 6;
	static const int TAB_SOUNDS = 7;
	static const int TAB_WEATHER = 8;
	// -----------------------
	static const int TABS_COUNT = 9;
	//-------------------------------------
	
	static ref array<ref MapMarker> 	m_MarkedEntities = new array<ref MapMarker>();
	int 								m_SelectedTab;
	protected ref ScriptConsoleTabBase 	m_TabHandlers[TABS_COUNT];

	Widget 							m_Tabs[TABS_COUNT];
	Widget 							m_ButtonsWindowWidget;
	ButtonWidget 					m_CloseConsoleButton;
	ButtonWidget 					m_Tab_buttons[TABS_COUNT];

	
	protected static const string HINTS_PATH_DEFAULT		= "Scripts/Data/internal/script_console_hints.json";
	protected static const string HINTS_PATH_OPTIONAL 		= "$mission:script_console_hints.json";
	
	static ref JsonHintsData m_JsonData;
	
	const string NO_HINT_TEXT = "No hint";
	
	static void SaveData()
	{
		JsonFileLoader<JsonHintsData>.JsonSaveFile(HINTS_PATH_OPTIONAL, m_JsonData );
	}
	
	
	protected static JsonHintsData GetData()
	{
		string path = HINTS_PATH_OPTIONAL;
		if ( !FileExist( path ) )
			path = HINTS_PATH_DEFAULT;
		
		if ( !FileExist( path ) )
			return null; //TODO: ERROR HANDLING
		
		JsonHintsData data;
		JsonFileLoader<JsonHintsData>.JsonLoadFile( path, data );
		
		return data;
	}
	
	void SetHintText(string text, Widget w)
	{
		if (m_JsonData && m_JsonData.WidgetHintBindings && w)
		{
			int hash = GetWidgetCombinedHash(w);
			m_JsonData.WidgetHintBindings.Set(hash, text);
			Print("setting: " + text);
		}
		HoverInterrupt();
	}
	
	void ScriptConsole()
	{
		#ifndef SERVER
		if (GetGame() && GetGame().GetMission() && GetGame().GetMission().GetHud())
		{
			GetGame().GetMission().GetHud().ShowHudPlayer(false);
			GetGame().GetMission().GetHud().ShowQuickbarPlayer(false);
		}
		#endif
		PluginItemDiagnostic plugin = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));
		if (plugin)
			plugin.OnScriptMenuOpened(true);
	}

	void ~ScriptConsole()
	{
		#ifndef SERVER
		if (GetGame() && GetGame().GetMission() && GetGame().GetMission().GetHud())
		{
			GetGame().GetMission().GetHud().ShowHudPlayer(true);
			GetGame().GetMission().GetHud().ShowQuickbarPlayer(true);
		}
		if (m_HintWidgetRoot)
			m_HintWidgetRoot.Unlink();
		#endif
		PluginItemDiagnostic plugin = PluginItemDiagnostic.Cast(GetPlugin(PluginItemDiagnostic));
		if (plugin)
			plugin.OnScriptMenuOpened(false);
		
		if (GetGame() && GetGame().GetMission())
		{
			GetGame().GetMission().EnableAllInputs(true);
		}
	}
	
	override Widget Init()
	{
		m_ConfigDebugProfile = PluginConfigDebugProfile.Cast(GetPlugin(PluginConfigDebugProfile));

		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console.layout");
		m_EditTooltipRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console_tooltip_edit.layout", layoutRoot);
		m_EditTooltipRoot.Show(false);
		m_HintOkButton = ButtonWidget.Cast(m_EditTooltipRoot.FindAnyWidget("ButtonOk"));
		m_HintCancelButton = ButtonWidget.Cast(m_EditTooltipRoot.FindAnyWidget("ButtonCancel"));
		m_HintClearButton = ButtonWidget.Cast(m_EditTooltipRoot.FindAnyWidget("ButtonClear"));
		m_HintInputText = EditBoxWidget.Cast(m_EditTooltipRoot.FindAnyWidget("InputText"));

		m_ButtonsWindowWidget = layoutRoot.FindAnyWidget("TabButtons");
		m_ButtonsWindowWidget.Show(true);
		
		m_Tabs[TAB_ITEMS] = layoutRoot.FindAnyWidget("ItemsPanel");
		m_Tabs[TAB_CONFIGS] = layoutRoot.FindAnyWidget("ConfigsPanel");
		m_Tabs[TAB_ENSCRIPT] = layoutRoot.FindAnyWidget("EnScriptPanel");
		m_Tabs[TAB_ENSCRIPT_SERVER] = layoutRoot.FindAnyWidget("EnScriptPanel");
		m_Tabs[TAB_GENERAL] = layoutRoot.FindAnyWidget("GeneralPanel");
		m_Tabs[TAB_OUTPUT] = layoutRoot.FindAnyWidget("OutputPanel");
		m_Tabs[TAB_VICINITY] = layoutRoot.FindAnyWidget("VicinityPanel");
		m_Tabs[TAB_SOUNDS] = layoutRoot.FindAnyWidget("SoundsPanel");
		m_Tabs[TAB_WEATHER] = layoutRoot.FindAnyWidget("WeatherPanel");

		m_Tab_buttons[TAB_ITEMS] = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ItemsButtonWidget"));
		m_Tab_buttons[TAB_CONFIGS] = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ConfigsButtonWidget"));
		m_Tab_buttons[TAB_ENSCRIPT] = ButtonWidget.Cast(layoutRoot.FindAnyWidget("EnScriptButtonWidget"));
		m_Tab_buttons[TAB_ENSCRIPT_SERVER] = ButtonWidget.Cast(layoutRoot.FindAnyWidget("EnScriptButtonWidgetServer"));
		m_Tab_buttons[TAB_GENERAL] = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GeneralButtonWidget"));
		m_Tab_buttons[TAB_OUTPUT] = ButtonWidget.Cast(layoutRoot.FindAnyWidget("OutputButtonWidget"));
		m_Tab_buttons[TAB_VICINITY] = ButtonWidget.Cast(layoutRoot.FindAnyWidget("VicinityWidget"));
		m_Tab_buttons[TAB_SOUNDS] = ButtonWidget.Cast(layoutRoot.FindAnyWidget("SoundsWidget"));
		m_Tab_buttons[TAB_WEATHER] = ButtonWidget.Cast(layoutRoot.FindAnyWidget("WeatherButtonWidget"));
		
		m_CloseConsoleButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CloseConsoleButtonWidget"));


		// load data from profile
		m_SelectedTab = m_ConfigDebugProfile.GetTabSelected();
		SelectTab(m_SelectedTab);

		return layoutRoot;
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
		m_JsonData = GetData();
		m_HoverSuccessTriggered = true;
		DisplayHint(GetMessage());
	}
	
	protected void HoverInterrupt()
	{
		m_HoverSuccessTriggered = false;
		//m_CurrentHoverWidget = null;
		m_HoverTime = 0;
		m_HintEditMode = false;
		HideHint();
	}

	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		super.OnKeyPress(w, x, y, key);
		return false;
	}
	
	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		super.OnKeyDown(w, x, y, key);
		for (int z =0; z < TABS_COUNT; z++)
		{
			if (m_TabHandlers[z])
				if ( m_TabHandlers[z].OnKeyDown(w,x,y,key))
					return true;
		}
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
		
		if (GetGame() && GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			GetGame().GetUIManager().Back();
		}
		
		
	
		if (!GetGame().IsMultiplayer() && KeyState(KeyCode.KC_RCONTROL) && KeyState(KeyCode.KC_NUMPAD0) && m_HintWidgetRoot && m_HintWidgetRoot.IsVisible())
		{
			ClearKey(KeyCode.KC_NUMPAD0);
			m_EditTooltipRoot.Show(true);
			string text = GetMessage();
			if (text == NO_HINT_TEXT)
				text = "";
			m_HintInputText.SetText(text);
			
		}
		
		for (int i =0; i < TABS_COUNT; i++)
		{
			if (m_TabHandlers[i])
				m_TabHandlers[i].Update(timeslice);
		}
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		super.OnMouseButtonDown(w,x,y,button);
		
		for (int z =0; z < TABS_COUNT; z++)
		{
			if (m_TabHandlers[z])
				m_TabHandlers[z].OnMouseButtonDown(w,x,y,button);
		}
		return false;
	}
	
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);

		if (w == m_CloseConsoleButton)
		{		
			Close();
			GetGame().GetMission().EnableAllInputs(true);
			return true;
		}
		else if (w == m_HintOkButton)
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

		for (int z =0; z < TABS_COUNT; z++)
		{
			if (m_TabHandlers[z])
				if ( m_TabHandlers[z].OnClick(w,x,y,button))
					return true;
		}

		// tabs
		for (int i = 0; i < TABS_COUNT; i++)
		{
			if (w == m_Tab_buttons[i])
			{
				SelectTab(i);
				return true;
			}
		}

		return false;
	}
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		super.OnDoubleClick(w, x, y, button);

		for (int z =0; z < TABS_COUNT; z++)
		{
			if (m_TabHandlers[z])
				if ( m_TabHandlers[z].OnDoubleClick(w,x,y,button))
					return true;
		}
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		super.OnMouseLeave(w, enterW, x, y);
		if(!m_EditTooltipRoot.IsVisible())
			HoverInterrupt();
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		super.OnMouseEnter(w, x, y);
		if (!m_EditTooltipRoot.IsVisible())
			m_CurrentHoverWidget = w;
		
		
		
		#ifdef PLATFORM_CONSOLE
		return false;
		#endif
		return true;
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		
		for (int i =0; i < TABS_COUNT; i++)
		{
			if (m_TabHandlers[i])
				m_TabHandlers[i].OnChange(w,x,y,finished);
		}

		return false;
	}
		
	override bool OnItemSelected(Widget w, int x, int y, int row, int  column,	int  oldRow, int  oldColumn)
	{
		super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
		
		for (int z =0; z < TABS_COUNT; z++)
		{
			if (m_TabHandlers[z])
				if ( m_TabHandlers[z].OnItemSelected(w,x,y,row,column, oldRow, oldColumn))
					return true;
		}

		return false;
	}
	
	ScriptConsoleTabBase GetTabHandler(int tabID)
	{
		if (tabID >= 0 && tabID < TABS_COUNT)
			return m_TabHandlers[tabID];
		return null;
	}

	void SelectPreviousTab()
	{
		int currTab = m_SelectedTab;
		currTab = (currTab - 1) % TABS_COUNT;
		if (currTab < 0)
		{
			currTab = TABS_COUNT - 1;
		}

		while (currTab != m_SelectedTab)
		{
			if (m_Tab_buttons[currTab] != NULL)
			{
				SelectTab(currTab);
				return;
			}

			currTab = (currTab - 1) % TABS_COUNT;
			if (currTab < 0)
			{
				currTab = TABS_COUNT - 1;
			}
		}
	}
	
	void SelectNextTab()
	{
		int currTab = m_SelectedTab;
		currTab = (currTab + 1) % TABS_COUNT;
		
		while (currTab != m_SelectedTab)
		{
			if (m_Tab_buttons[currTab] != NULL)
			{
				SelectTab(currTab);	
				return;		
			}
			
			currTab = (currTab + 1) % TABS_COUNT;
		}
	}

	void SelectTab(int tab_id)
	{
		if (tab_id < 0 || tab_id >= TABS_COUNT)
			tab_id = 0;
		int i = 0;
		Widget tab = m_Tabs[tab_id];
		for (i = 0; i < TABS_COUNT; i++)
		{
			Widget t = m_Tabs[i];

			m_TabHandlers[i] = null;

			if (i == tab_id)
			{
				t.Show(true);
				
			}
			else
			{
				t.Show(false);
			}
		}
		
		
		for (i = 0; i < TABS_COUNT; i++)
		{
			ButtonWidget button = m_Tab_buttons[i];
			button.SetState(i == tab_id);
		}
		
		m_SelectedTab = tab_id;
		m_ConfigDebugProfile.SetTabSelected(m_SelectedTab);
		
		OnTabSeleted(m_SelectedTab);
	}

	
	void OnTabSeleted(int tabID)
	{
		Widget tab = m_Tabs[tabID];
		
		if (tabID == TAB_WEATHER)
		{ 
			m_TabHandlers[TAB_WEATHER] = new ScriptConsoleWeatherTab(layoutRoot, this);
		}
		else if (tabID == TAB_VICINITY)
		{ 
			m_TabHandlers[TAB_VICINITY] = new ScriptConsoleVicinityTab(layoutRoot, this);
		}
		else if (tabID == TAB_SOUNDS)
		{
			m_TabHandlers[TAB_SOUNDS] = new ScriptConsoleSoundsTab(layoutRoot, this);
		}
		else if (tabID == TAB_ENSCRIPT)
		{
			tab.Show(true);//since both buttons point to the same tab, this will keep the tab visible for either tab selection(it's a hack)
			m_TabHandlers[TAB_ENSCRIPT] = new ScriptConsoleEnfScriptTab(layoutRoot, this, false);
		}
		else if (tabID == TAB_ENSCRIPT_SERVER)
		{
			m_TabHandlers[TAB_ENSCRIPT] = new ScriptConsoleEnfScriptTab(layoutRoot, this, true);
			
		}
		else if (tabID == TAB_OUTPUT)
		{
			m_TabHandlers[TAB_OUTPUT] = new ScriptConsoleOutputTab(layoutRoot, this);
		}
		else if (tabID == TAB_CONFIGS)
		{
			m_TabHandlers[TAB_CONFIGS] = new ScriptConsoleConfigTab(layoutRoot, this);
		}
		
		else if (tabID == TAB_ITEMS)
		{
			m_TabHandlers[TAB_ITEMS] = new ScriptConsoleItemsTab(layoutRoot, this);
		}
		else if (tabID == TAB_GENERAL)
		{
			m_TabHandlers[TAB_GENERAL] = new ScriptConsoleGeneralTab(layoutRoot, this);
		}
	}
	
	protected void DisplayHint(string message)
	{
		if (message)
		{
			m_HintWidgetRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/script_console/script_console_hint.layout");
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
		for (int z =0; z < TABS_COUNT; z++)
		{
			if (m_TabHandlers[z])
				m_TabHandlers[z].OnRPCEx(rpc_type,ctx);
		}
		#endif

	}

	PluginConfigDebugProfile m_ConfigDebugProfile;
}
