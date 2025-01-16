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
	Widget 						m_ButtonsWindowWidget;
	ScriptConsoleTabBase 		m_SelectedHandler;
	protected int 				m_Id;
	
	static ref array<ref MapMarker> 	m_MarkedEntities = new array<ref MapMarker>();
	int 								m_SelectedTab;
	protected ref map<Widget, ref ScriptConsoleTabBase> m_TabHandlers = new map<Widget, ref ScriptConsoleTabBase>;
	protected ref map<int, ref ScriptConsoleTabBase> m_TabHandlersByID = new map<int, ref ScriptConsoleTabBase>;

	ButtonWidget 					m_CloseConsoleButton;

	
	protected static const string HINTS_PATH_DEFAULT		= "scripts/data/internal/script_console_hints.json";
	protected static const string HINTS_PATH_OPTIONAL 		= "$mission:script_console_hints.json";
	
	static ref JsonHintsData m_JsonData;
	
	const string NO_HINT_TEXT = "No hint";
	
	
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
			Print("setting: " + text);
		}
		HoverInterrupt();
	}
	
	void RegisterTab(ScriptConsoleTabBase handler)
	{
		m_TabHandlers.Insert(handler.GetButton(), handler);
		m_TabHandlersByID.Insert(m_Id, handler);
		handler.Init(m_Id);
		m_Id++;
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
		//(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
		RegisterTab(new ScriptConsoleItemsTab(layoutRoot.FindAnyWidget("ItemsPanel"),this,ButtonWidget.Cast(layoutRoot.FindAnyWidget("ItemsButtonWidget"))));
		RegisterTab(new ScriptConsoleConfigTab(layoutRoot.FindAnyWidget("ConfigsPanel"),this,ButtonWidget.Cast(layoutRoot.FindAnyWidget("ConfigsButtonWidget"))));
		RegisterTab(new ScriptConsoleEnfScriptTab(layoutRoot.FindAnyWidget("EnScriptPanel"),this,ButtonWidget.Cast(layoutRoot.FindAnyWidget("EnScriptButtonWidget"))));
		RegisterTab(new ScriptConsoleEnfScriptServerTab(layoutRoot.FindAnyWidget("EnScriptPanel"),this,ButtonWidget.Cast(layoutRoot.FindAnyWidget("EnScriptButtonWidgetServer"))));
		RegisterTab(new ScriptConsoleGeneralTab(layoutRoot.FindAnyWidget("GeneralPanel"),this,ButtonWidget.Cast(layoutRoot.FindAnyWidget("GeneralButtonWidget"))));
		RegisterTab(new ScriptConsoleOutputTab(layoutRoot.FindAnyWidget("OutputPanel"),this,ButtonWidget.Cast(layoutRoot.FindAnyWidget("OutputButtonWidget"))));
		RegisterTab(new ScriptConsoleVicinityTab(layoutRoot.FindAnyWidget("VicinityPanel"),this,ButtonWidget.Cast(layoutRoot.FindAnyWidget("VicinityWidget"))));
		RegisterTab(new ScriptConsoleSoundsTab(layoutRoot.FindAnyWidget("SoundsPanel"),this,ButtonWidget.Cast(layoutRoot.FindAnyWidget("SoundsWidget"))));
		RegisterTab(new ScriptConsoleWeatherTab(layoutRoot.FindAnyWidget("WeatherPanel"),this,ButtonWidget.Cast(layoutRoot.FindAnyWidget("WeatherButtonWidget"))));
		RegisterTab(new ScriptConsoleCameraTab(layoutRoot.FindAnyWidget("CameraPanel"),this,ButtonWidget.Cast(layoutRoot.FindAnyWidget("CameraButtonWidget"))));

		m_CloseConsoleButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CloseConsoleButtonWidget"));

		// load data from profile
		m_SelectedTab = m_ConfigDebugProfile.GetTabSelected();
		SelectTabByID(m_SelectedTab);
		
		m_JsonData = GetData();

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

	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		super.OnKeyPress(w, x, y, key);

		if (m_SelectedHandler.OnKeyPress(w,x,y,key))
			return true;
		return false;

	}
	
	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		super.OnKeyDown(w, x, y, key);
		
		if (m_SelectedHandler.OnKeyDown(w,x,y,key))
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
		
		if (GetGame() && GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			GetGame().GetUIManager().Back();
			return;
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
		
		foreach (ScriptConsoleTabBase handler: m_TabHandlers)
		{
			if (handler.IsSelected())
			{
				handler.Update(timeslice);
			}
		}
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		super.OnMouseButtonDown(w,x,y,button);
		
		if (m_SelectedHandler.OnMouseButtonDown(w,x,y,button))
			return true;

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

		SelectTabByButton(w);

		if (m_SelectedHandler.OnClick(w,x,y,button))
			return true;
		return false;
	}
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		super.OnDoubleClick(w, x, y, button);

		if (m_SelectedHandler.OnDoubleClick(w,x,y,button))
				return true; 
		
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		super.OnMouseLeave(w, enterW, x, y);
		
		if (!m_EditTooltipRoot.IsVisible())
			HoverInterrupt();
		
		if (m_SelectedHandler.OnMouseLeave(w, enterW, x, y))
			return true;
		
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		super.OnMouseEnter(w, x, y);
		if (!m_EditTooltipRoot.IsVisible())
			m_CurrentHoverWidget = w;
		
		if (m_SelectedHandler.OnMouseEnter(w ,x, y))
			return true;
		
		#ifdef PLATFORM_CONSOLE
		return false;
		#endif
		return true;
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		
		if (m_SelectedHandler.OnChange(w,x,y,finished))
			return true; 
		
		return false;
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int  column,	int  oldRow, int  oldColumn)
	{
		super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);

		if (m_SelectedHandler.OnItemSelected(w, x, y, row, column,oldRow, oldColumn))
			return true;
		
		return false;
	}
	
	ScriptConsoleTabBase GetTabHandler(typename tabType)
	{
		foreach (ScriptConsoleTabBase handler: m_TabHandlers)
		{
			if (tabType == handler.Type())
			{
				return handler;
			}
		}
		return null;
	}

	void SelectTabByID(int id)
	{
		ScriptConsoleTabBase tab = m_TabHandlersByID.Get(id);
		if (tab)
			SelectTab(tab);
	}
	
	void SelectTabByButton(Widget button)
	{
		ScriptConsoleTabBase tab = m_TabHandlers.Get(button);
		if (tab)
			SelectTab(tab);
	}

	void SelectTab(ScriptConsoleTabBase selectedHandler)
	{
		foreach (ScriptConsoleTabBase handler:m_TabHandlers)
		{
			handler.Select(handler == selectedHandler, selectedHandler);
		}
		
		if (ScriptConsoleCameraTab.Cast(selectedHandler))
		{
			// Remove alpha background for camera tab
			ShowMenuBackground(false);
			GetGame().GetMission().RemoveActiveInputExcludes({"movement"}, true);
		}
		else
		{
			// Add back alpha background
			ShowMenuBackground(true);
			GetGame().GetMission().AddActiveInputExcludes({"movement"});
		}
		
		m_SelectedHandler = selectedHandler;
		m_ConfigDebugProfile.SetTabSelected(selectedHandler.GetID());
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
		
		foreach (ScriptConsoleTabBase handler:m_TabHandlers)
		{
			handler.OnRPCEx(rpc_type,ctx);
		}
		#endif

	}
	
	override void OnShow()
	{
		super.OnShow();
		
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
			"UABuldAdvanceAnimation"
		};

		if (m_SelectedHandler && ScriptConsoleCameraTab.Cast(m_SelectedHandler))
		{
			inputExcludes.Remove(0);
		}
				
		GetGame().GetMission().AddActiveInputExcludes(inputExcludes);
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

