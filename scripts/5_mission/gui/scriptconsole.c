class ScriptConsole extends UIScriptedMenu
{
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
		
		if (GetGame() && GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			GetGame().GetUIManager().Back();
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
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		super.OnMouseEnter(w, x, y);
		
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
