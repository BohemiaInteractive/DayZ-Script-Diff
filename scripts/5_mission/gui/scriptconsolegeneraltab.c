class ScriptConsoleGeneralTab : ScriptConsoleTabBase
{
	static int 		m_ObjectsScope = 2;
	
	protected static float 	DEBUG_MAP_ZOOM = 1;
	protected static bool 	SHOW_OTHERS = 0;
	
	protected const string  	DEFAULT_POS_XYZ = "<1,2,3>";
	protected vector 			m_MapPos;
	protected bool 				m_PlayerPosRefreshBlocked;

	protected ref array<ref RemotePlayerStatDebug> m_PlayerDebugStats = new array<ref RemotePlayerStatDebug>;

	protected MissionGameplay		m_MissionGameplay;
	protected PluginDeveloper 		m_Developer;

	protected SliderWidget 			m_TimeSlider;
	protected ref Timer 			m_LateInit = new Timer();
	
	protected bool 					m_UpdatePlayerPositions;
	
	//-------------------------------- WIDGETS ---------------------------------------
	protected EditBoxWidget 		m_TeleportX;
	protected EditBoxWidget 		m_TeleportY;
	protected EditBoxWidget 		m_TeleportXYZ;
	protected EditBoxWidget 		m_DateYear;
	protected EditBoxWidget 		m_DateMonth;
	protected EditBoxWidget 		m_DateDay;
	protected EditBoxWidget 		m_DateHour;
	protected EditBoxWidget 		m_DateMinute;

	protected ButtonWidget 		m_LocationAddButton;
	protected ButtonWidget 		m_LocationRemoveButton;
	protected ButtonWidget 		m_TeleportButton;
	protected ButtonWidget 		m_ButtonCopyPos;
	protected ButtonWidget 		m_DiagDrawButton;
	protected ButtonWidget 		m_DiagToggleButton;

	protected CheckBoxWidget		m_LogsEnabled;
	protected CheckBoxWidget		m_HudDCharStats;
	protected CheckBoxWidget		m_HudDCharLevels;
	protected CheckBoxWidget		m_HudDCharStomach;
	protected CheckBoxWidget		m_HudDCharModifiers;
	protected CheckBoxWidget		m_HudDCharAgents;
	protected CheckBoxWidget		m_HudDCharDebug;
	protected CheckBoxWidget		m_HudDFreeCamCross;
	protected CheckBoxWidget		m_HudDVersion;
	
	protected CheckBoxWidget		m_ShowOthers;
	
	protected CheckBoxWidget		m_HudDTemperature;
	protected MapWidget 			m_DebugMapWidget;
	
	protected TextWidget	 		m_PlayerCurPos;
	protected TextWidget	 		m_MouseCurPos;
	protected TextWidget	 		m_PlayerMouseDiff;

	protected TextListboxWidget 	m_DiagToggleTextListbox;
	protected TextListboxWidget	m_PositionsListbox;
	protected TextListboxWidget 	m_DiagDrawmodeTextListbox;
	protected TextListboxWidget 	m_HelpTextListboxWidget;
	//-----------------------------------------------------------------------------------
	
	void ScriptConsoleGeneralTab(Widget root, ScriptConsole console)
	{
		m_MissionGameplay			= MissionGameplay.Cast(GetGame().GetMission());
		
		m_DiagToggleTextListbox = TextListboxWidget.Cast(root.FindAnyWidget("DiagToggle"));
		m_DiagToggleButton = ButtonWidget.Cast(root.FindAnyWidget("DiagToggleButton"));

		m_DiagDrawmodeTextListbox = TextListboxWidget.Cast(root.FindAnyWidget("DiagDrawmode"));
		m_DiagDrawButton = ButtonWidget.Cast(root.FindAnyWidget("DiagDrawButton"));

		m_DebugMapWidget = MapWidget.Cast(root.FindAnyWidget("MapWidget"));
		
		m_PositionsListbox	= TextListboxWidget.Cast(root.FindAnyWidget("PositionsList"));
		m_TeleportButton	= ButtonWidget.Cast(root.FindAnyWidget("ButtonTeleport"));
		m_ButtonCopyPos		= ButtonWidget.Cast(root.FindAnyWidget("Button_CopyPos"));

		m_TeleportX			= EditBoxWidget.Cast(root.FindAnyWidget("TeleportX"));
		m_TeleportY			= EditBoxWidget.Cast(root.FindAnyWidget("TeleportY"));
		m_TeleportXYZ		= EditBoxWidget.Cast(root.FindAnyWidget("TeleportXYZ"));
		m_PlayerCurPos		= TextWidget.Cast(root.FindAnyWidget("PlayerPosLabel"));
		m_PlayerMouseDiff	= TextWidget.Cast(root.FindAnyWidget("PlayerMouseDiff"));
		m_MouseCurPos		= TextWidget.Cast(root.FindAnyWidget("MousePosLabel"));
		m_LogsEnabled		= CheckBoxWidget.Cast(root.FindAnyWidget("cbx_LogsEnabled"));
		m_HudDCharStats		= CheckBoxWidget.Cast(root.FindAnyWidget("cbx_CharacterStats"));
		m_HudDCharLevels	= CheckBoxWidget.Cast(root.FindAnyWidget("cbx_CharacterLevels"));
		m_HudDCharStomach	= CheckBoxWidget.Cast(root.FindAnyWidget("cbx_CharacterStomach"));
		m_HudDCharModifiers	= CheckBoxWidget.Cast(root.FindAnyWidget("cbx_CharacterModifiers"));
		m_HudDCharAgents	= CheckBoxWidget.Cast(root.FindAnyWidget("cbx_CharacterAgents"));
		m_HudDCharDebug		= CheckBoxWidget.Cast(root.FindAnyWidget("cbx_CharacterDebug"));
		m_HudDFreeCamCross	= CheckBoxWidget.Cast(root.FindAnyWidget("cbx_FreeCamCross"));
		m_HudDTemperature	= CheckBoxWidget.Cast(root.FindAnyWidget("cbx_Temp"));
		m_HudDVersion		= CheckBoxWidget.Cast(root.FindAnyWidget("cbx_Version"));
		
		m_LocationAddButton	= ButtonWidget.Cast(root.FindAnyWidget("AddButton"));
		//m_LocationAddButton.SetHandler(ToolTipEventHandler.GetInstance());
		m_LocationRemoveButton	= ButtonWidget.Cast(root.FindAnyWidget("RemoveButton"));

		m_TimeSlider		= SliderWidget.Cast(root.FindAnyWidget("TimeSlider"));
		m_DateYear			= EditBoxWidget.Cast(root.FindAnyWidget("DateYear"));
		m_DateMonth			= EditBoxWidget.Cast(root.FindAnyWidget("DateMonth"));
		m_DateDay			= EditBoxWidget.Cast(root.FindAnyWidget("DateDay"));
		m_DateHour			= EditBoxWidget.Cast(root.FindAnyWidget("DateHour"));
		m_DateMinute		= EditBoxWidget.Cast(root.FindAnyWidget("DateMinute"));
		
		
		m_ShowOthers		= CheckBoxWidget.Cast(root.FindAnyWidget("ShowOthersCheckbox"));

		m_HelpTextListboxWidget = TextListboxWidget.Cast(root.FindAnyWidget("HelpTextListboxWidget"));
		m_Developer					= PluginDeveloper.Cast(GetPlugin(PluginDeveloper));
		
		Init();
		LateInit();
		
	}
	
	void ~ScriptConsoleGeneralTab()
	{
		DEBUG_MAP_ZOOM = m_DebugMapWidget.GetScale();

		PluginRemotePlayerDebugClient plugin_remote_client = PluginRemotePlayerDebugClient.Cast(GetPlugin(PluginRemotePlayerDebugClient));
		if (SHOW_OTHERS && plugin_remote_client && GetGame().GetPlayer())
			plugin_remote_client.RequestPlayerInfo(PlayerBase.Cast(GetGame().GetPlayer()), 0);
	}

	protected void Init()
	{
		
		
		
		// Update checkbox Character Values
		m_HudDCharStats.SetChecked(m_ConfigDebugProfile.GetCharacterStatsVisible());
		m_HudDCharLevels.SetChecked(m_ConfigDebugProfile.GetCharacterLevelsVisible());
		m_HudDCharStomach.SetChecked(m_ConfigDebugProfile.GetCharacterStomachVisible());
		m_HudDCharModifiers.SetChecked(m_ConfigDebugProfile.GetCharacterModifiersVisible());
		m_HudDCharAgents.SetChecked(m_ConfigDebugProfile.GetCharacterAgentsVisible());
		m_HudDCharDebug.SetChecked(m_ConfigDebugProfile.GetCharacterDebugVisible());
		m_HudDFreeCamCross.SetChecked(m_ConfigDebugProfile.GetFreeCameraCrosshairVisible());
		m_HudDVersion.SetChecked(m_ConfigDebugProfile.GetVersionVisible());
		m_HudDTemperature.SetChecked(m_ConfigDebugProfile.GetTempVisible());
		

		m_LogsEnabled.SetChecked(m_ConfigDebugProfile.GetLogsEnabled());
		
		TStringArray diag_names = new TStringArray;
		GetGame().GetDiagModeNames(diag_names);
		int i;
		for (i = 0; i < diag_names.Count(); i++)
		{
			m_DiagToggleTextListbox.AddItem(diag_names.Get(i), NULL, 0);
		}

		GetGame().GetDiagDrawModeNames(diag_names);
		for (i = 0; i < diag_names.Count(); i++)
		{
			m_DiagDrawmodeTextListbox.AddItem(diag_names.Get(i), NULL, 0);
		}
		RefreshLocations();
		
		UpdateHudDebugSetting();

		if (GetGame().GetPlayer())
		{
			m_DebugMapWidget.SetScale(DEBUG_MAP_ZOOM);
			m_DebugMapWidget.SetMapPos(GetGame().GetPlayer().GetWorldPosition());
		}
		m_TeleportXYZ.SetText(DEFAULT_POS_XYZ);
		
		m_LateInit.Run(0.05, this, "LateInit", null, false);
		
		int year,month,day,hour,minute;
		GetGame().GetWorld().GetDate(year,month, day, hour, minute);
		RefreshDateWidgets(year,month, day, hour, minute);
	}
	
	
	void RefreshDateWidgets(int year, int month, int day, int hour, int minute)
	{
		float time01 = Math.InverseLerp(0,60*24 - 1, (hour * 60) + minute);
		m_DateYear.SetText(year.ToString());
		m_DateMonth.SetText(month.ToString());
		m_DateDay.SetText(day.ToString());
		m_DateHour.SetText(hour.ToString());
		m_DateMinute.SetText(minute.ToString());
		m_TimeSlider.SetCurrent(time01 * 100);
	}
	
	bool IsLocationNameAvailable(string name)
	{
		int count = m_PositionsListbox.GetNumItems();
		for (int i = 0; i < count; i++)
		{
			LocationParams data;
			m_PositionsListbox.GetItemData(i,0,data);
			if (data.param1 == name)
				return false;
		}
		return true;
	}

	
	
	
	void LateInit()
	{
		m_ShowOthers.SetChecked(SHOW_OTHERS);
		
		if (m_ConfigDebugProfile)
		{
			int prevRow = m_ConfigDebugProfile.GetSpawnLocIndex();
			if (prevRow < m_PositionsListbox.GetNumItems())
				m_PositionsListbox.SelectRow(prevRow);
			
		}
		PluginRemotePlayerDebugClient plugin_remote_client = PluginRemotePlayerDebugClient.Cast(GetPlugin(PluginRemotePlayerDebugClient));
		if (SHOW_OTHERS && plugin_remote_client && GetGame().GetPlayer())
		{
			plugin_remote_client.RequestPlayerInfo(PlayerBase.Cast(GetGame().GetPlayer()), 1);
			m_UpdatePlayerPositions = 1;
		}
		
	}
	
	void Teleport(PlayerBase player, vector position)
	{
		if (position[1] == 0)
			position[1] = GetGame().SurfaceY(position[0], position[2]);
		m_Developer.Teleport(player, position);
	}
	
	void RefreshLocations()
	{
		m_PositionsListbox.ClearItems();
		
		array<ref LocationParams> locData = new array<ref LocationParams>;
		m_ConfigDebugProfile.GetLocationsData(locData,true);
		m_ConfigDebugProfileFixed.GetLocationsData(locData,false);
		foreach (LocationParams dta: locData)
		{
			string name = dta.param1;
			if (!dta.param2)
			{
				name = "[" + name + "]";
			}
			m_PositionsListbox.AddItem(name,dta,0);
		}
	}

	
	string GetCurrentLocationName()
	{
		string name;
		LocationParams prms;
		GetCurrentPositionData(prms);
		if (prms)
			name =prms.param1;
		return name;
	}
	vector GetCurrentLocationPos()
	{
		LocationParams prms;
		GetCurrentPositionData(prms);
		if (prms)
			return prms.param3;
		else 
			return vector.Zero;
	}
	
	void GetCurrentPositionData(out LocationParams data)
	{
		if (m_PositionsListbox.GetSelectedRow() != -1)
		{
			m_PositionsListbox.GetItemData(m_PositionsListbox.GetSelectedRow(), 0, data);
		}
	}
	
	bool IsCurrentPositionCustom()
	{
		LocationParams prms;
		GetCurrentPositionData(prms);
		if (prms)
			return prms.param2;
		else 
			return false;
	}
	
	int GetCurrentPositionIndex()
	{
		return m_PositionsListbox.GetSelectedRow();
	}
	
	bool IsCurrentPositionValid()
	{
		return (m_PositionsListbox.GetSelectedRow() != -1);
	}
	
	void UpdateHudDebugSetting();
	
	void RefreshPlayerPosEditBoxes()
	{
		if (!GetGame().GetPlayer())
		{
			return;
		}

		vector player_pos = GetGame().GetPlayer().GetPosition();
		SetMapPos(player_pos);
	}

	void UpdateTime(bool slider_used)
	{
		Param5<int,int,int,int,int> p5 = new Param5<int,int,int,int,int>(0,0,0,0,0);
		int year, month, day, hour, minute;
		
		year = m_DateYear.GetText().ToInt();
		month = m_DateMonth.GetText().ToInt();
		day = m_DateDay.GetText().ToInt();
		
		if (slider_used)
		{
			int time_minutes = Math.Lerp(0, (24*60) - 1, m_TimeSlider.GetCurrent()/100);
			hour = time_minutes / 60;
			minute = time_minutes % 60;
		}
		else
		{
			hour = m_DateHour.GetText().ToInt();
			minute = m_DateMinute.GetText().ToInt();
		}

		p5.param1 = year;
		p5.param2 = month;
		p5.param3 = day;
		p5.param4 = hour;
		p5.param5 = minute;
		
		RefreshDateWidgets(year, month, day, hour, minute);
		GetGame().GetWorld().SetDate(year, month, day, hour, minute);

		if (GetGame().GetPlayer())
		{
			GetGame().GetPlayer().RPCSingleParam(ERPCs.DEV_RPC_SET_TIME, p5, true);
		}
	}
	
	
	void UpdateMousePos()
	{
		int x,y;
		GetMousePos(x,y);
		vector mouse_pos, world_pos;
		mouse_pos[0] = x;
		mouse_pos[1] = y;
		world_pos = m_DebugMapWidget.ScreenToMap(mouse_pos);
		world_pos[1] = GetGame().SurfaceY(world_pos[0], world_pos[2]);
		
		if (m_MouseCurPos)
		{
			m_MouseCurPos.SetText("Mouse: "+ MiscGameplayFunctions.TruncateToS(world_pos[0]) +", "+ MiscGameplayFunctions.TruncateToS(world_pos[1]) +", "+ MiscGameplayFunctions.TruncateToS(world_pos[2]));
		}
		if (m_PlayerMouseDiff && GetGame().GetPlayer())
		{
			vector player_pos = GetGame().GetPlayer().GetWorldPosition();
			//player_pos[1] = 0;
			float dst = (world_pos - player_pos).Length();

			m_PlayerMouseDiff.SetText("Distance: " + MiscGameplayFunctions.TruncateToS(dst));
		}
	}


	void SetMapPos(vector pos)
	{
		m_MapPos = pos;
		m_PlayerCurPos.SetText("Pos: "+  MiscGameplayFunctions.TruncateToS(pos[0]) +", "+ MiscGameplayFunctions.TruncateToS(pos[1]) +", "+ MiscGameplayFunctions.TruncateToS(pos[2]));
	}
	
	vector GetMapPos()
	{
		return m_MapPos;
	}
	
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		super.OnMouseButtonDown(w,x,y,button);
		
		if (w ==  m_DebugMapWidget)
		{
			if (button == 0)
			{
				m_PlayerPosRefreshBlocked = true;
				int mouse_x,mouse_y;
				GetMousePos(mouse_x,mouse_y);
				vector mouse_pos, world_pos;
				mouse_pos[0] = mouse_x;
				mouse_pos[1] = mouse_y;
				world_pos = m_DebugMapWidget.ScreenToMap(mouse_pos);
				world_pos[1] = GetGame().SurfaceY(world_pos[0], world_pos[2]);
				
				SetMapPos(world_pos);
			}
			else if (button == 1 && GetGame().GetPlayer())
			{
				SetMapPos(GetGame().GetPlayer().GetWorldPosition());
			}
		}
		return true;
	}
	

	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		super.OnKeyDown(w, x, y, key);
		return false;
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		
		if (w == m_ShowOthers && GetGame().GetPlayer())
		{
			PluginRemotePlayerDebugClient plugin_remote_client = PluginRemotePlayerDebugClient.Cast(GetPlugin(PluginRemotePlayerDebugClient));
			if (m_ShowOthers.IsChecked())
			{
				plugin_remote_client.RequestPlayerInfo(PlayerBase.Cast(GetGame().GetPlayer()), 1);
				m_UpdatePlayerPositions = 1;
				SHOW_OTHERS = true;
			}
			else
			{
				plugin_remote_client.RequestPlayerInfo(PlayerBase.Cast(GetGame().GetPlayer()), 0);
				m_UpdatePlayerPositions = 0;
				SHOW_OTHERS = false;
			}
			return true;
		}
		else if (w == m_TimeSlider || w == m_DateDay || w == m_DateYear || w == m_DateMonth || w == m_DateHour || w == m_DateMinute)
		{
			UpdateTime(w == m_TimeSlider);
			return true;
		}
		return false;
	}
	
	override bool OnItemSelected(Widget w, int x, int y, int row, int  column,	int  oldRow, int  oldColumn)
	{
		super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
		if (w == m_PositionsListbox)
		{
			vector position = GetCurrentLocationPos();
			m_TeleportX.SetText(position[0].ToString());
			m_TeleportY.SetText(position[2].ToString());
			m_TeleportXYZ.SetText(position.ToString());
			if (IsCurrentPositionValid())
			{
				m_ConfigDebugProfile.SetSpawnLocIndex(GetCurrentPositionIndex());
			}
			
			return true;
		}
		return false;
	}
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		super.OnDoubleClick(w, x, y, button);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		
		if (w ==  m_DebugMapWidget)
		{
			vector screen_to_map = m_DebugMapWidget.ScreenToMap(Vector(x,y, 0));
			float pos_y_a = GetGame().SurfaceY(screen_to_map[0], screen_to_map[2]);
			float pos_y_b = GetGame().SurfaceRoadY(screen_to_map[0], screen_to_map[2]);
			float pos_y = Math.Max(pos_y_a, pos_y_b);
			screen_to_map[1] = pos_y;
			m_Developer.Teleport(player, screen_to_map);
			return true;
		}
		
		if (w == m_PositionsListbox)
		{
			vector position = GetCurrentLocationPos();
			Teleport(player, position);
			return true;
		}
		
		if (w == m_TeleportXYZ)
		{
			EditBoxWidget.Cast(w).SetText("");
			return true;
		}
		return false;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		int i;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

		PluginDeveloper module_dev = PluginDeveloper.Cast(GetPlugin(PluginDeveloper));

		if (w == m_PositionsListbox)
		{
			
			vector position = GetCurrentLocationPos();
			
			m_TeleportX.SetText(position[0].ToString());
			m_TeleportY.SetText(position[2].ToString());
			
			if (IsCurrentPositionValid())
			{
				m_ConfigDebugProfile.SetSpawnLocIndex(GetCurrentPositionIndex());
			}
			
			return true;
		}
		else if (w == m_TeleportButton)
		{
			float pos_x = m_TeleportX.GetText().ToFloat();
			float pos_z = m_TeleportY.GetText().ToFloat();
			float pos_y = GetGame().SurfaceY(pos_x, pos_z);
			vector v = Vector(pos_x, pos_y, pos_z);
			if (m_TeleportXYZ.GetText() != "" && m_TeleportXYZ.GetText() != DEFAULT_POS_XYZ)
			{
				string pos = m_TeleportXYZ.GetText();
				v = pos.BeautifiedToVector();
			}
			Teleport(player, v);
			return true;
		}
		else if (w == m_ButtonCopyPos)
		{
			GetGame().CopyToClipboard(GetMapPos().ToString());
			return true;
		}
		else if (w == m_LogsEnabled)
		{
			//Log("m_LogsEnabled: "+ToString(m_LogsEnabled.IsChecked()));

			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetLogsEnabled(m_LogsEnabled.IsChecked());
				LogManager.SetLogsEnabled(m_LogsEnabled.IsChecked());
			}

			return true;
		}
		else if (w == m_HudDCharStats)
		{
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetCharacterStatsVisible(m_HudDCharStats.IsChecked());
			}

			// Refresh UI by new settings
			m_MissionGameplay.GetHudDebug().RefreshByLocalProfile();

			return true;
		}
		else if (w == m_HudDCharLevels)
		{
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetCharacterLevelsVisible(m_HudDCharLevels.IsChecked());
			}

			// Refresh UI by new settings
			m_MissionGameplay.GetHudDebug().RefreshByLocalProfile();

			return true;
		}
		else if (w == m_HudDCharStomach)
		{
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetCharacterStomachVisible(m_HudDCharStomach.IsChecked());
			}

			// Refresh UI by new settings
			m_MissionGameplay.GetHudDebug().RefreshByLocalProfile();

			return true;
		}
		else if (w == m_HudDVersion)
		{
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetVersionVisible(m_HudDVersion.IsChecked());
			}

			// Refresh UI by new settings
			m_MissionGameplay.GetHudDebug().RefreshByLocalProfile();

			return true;
		}
		else if (w == m_HudDTemperature)
		{
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetTempVisible(m_HudDTemperature.IsChecked());
			}

			// Refresh UI by new settings
			m_MissionGameplay.GetHudDebug().RefreshByLocalProfile();

			return true;
		}
		else if (w == m_HudDCharModifiers)
		{
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetCharacterModifiersVisible(m_HudDCharModifiers.IsChecked());
			}

			// Refresh UI by new settings
			m_MissionGameplay.GetHudDebug().RefreshByLocalProfile();

			return true;
		}
		else if (w == m_HudDCharAgents)
		{
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetCharacterAgentsVisible(m_HudDCharAgents.IsChecked());
			}

			// Refresh UI by new settings
			m_MissionGameplay.GetHudDebug().RefreshByLocalProfile();

			return true;
		}
		else if (w == m_HudDCharDebug)
		{
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetCharacterDebugVisible(m_HudDCharDebug.IsChecked());
			}

			// Refresh UI by new settings
			m_MissionGameplay.GetHudDebug().RefreshByLocalProfile();

			return true;
		}
		else if (w == m_HudDFreeCamCross)
		{
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetFreeCameraCrosshairVisible(m_HudDFreeCamCross.IsChecked());
			}

			// Refresh UI by new settings
			m_MissionGameplay.GetHudDebug().RefreshByLocalProfile();

			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(GetGame().GetMission().RefreshCrosshairVisibility);

			return true;
		}
		
		else if (w == m_DiagToggleButton)
		{
			int toggle_row_index = m_DiagToggleTextListbox.GetSelectedRow();
			bool toggle_state = GetGame().GetDiagModeEnable(toggle_row_index);
			GetGame().SetDiagModeEnable(toggle_row_index, !toggle_state);
			return true;
		}
		else if (w == m_DiagDrawButton)
		{
			int draw_row_index = m_DiagDrawmodeTextListbox.GetSelectedRow();
			GetGame().SetDiagDrawMode(draw_row_index);
			return true;
		}
		// TOUCHED THIS
		else if (w == m_LocationAddButton)
		{
			ScriptConsoleAddLocation menu = ScriptConsoleAddLocation.Cast(g_Game.GetUIManager().EnterScriptedMenu(MENU_LOC_ADD, m_ScriptConsole));
			menu.SetPosition(GetMapPos());
			return true;
		}
		else if (w == m_LocationRemoveButton)
		{
			m_ConfigDebugProfile.CustomLocationsRemove(GetCurrentLocationName());
			RefreshLocations();
			return true;
		}
		return false;
	}
	
	override void OnRPCEx(int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPCEx(rpc_type, ctx);
		#ifdef DIAG_DEVELOPER
		switch (rpc_type)
		{
			case ERPCs.DEV_PLAYER_DEBUG_DATA:
			{
				ctx.Read(m_PlayerDebugStats);
				break;
			}
		}
		#endif
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		
		m_DebugMapWidget.ClearUserMarks();

		if (m_UpdatePlayerPositions)
		{
			foreach (RemotePlayerStatDebug rpd: m_PlayerDebugStats)
			{
				if (rpd.m_Player != player)
				{
					vector dir = rpd.m_Pos - player.GetWorldPosition();
					dir[1] = 0;
					string dist = ((int)dir.Length()).ToString();
					string text = rpd.m_Name + " " +dist +"m.";
					m_DebugMapWidget.AddUserMark(rpd.m_Pos, text , COLOR_BLUE,"\\dz\\gear\\navigation\\data\\map_tree_ca.paa");
				}
			}
		}
		if (player)
		{
			vector player_pos = player.GetWorldPosition();
			m_DebugMapWidget.AddUserMark(player_pos,"You", COLOR_RED,"\\dz\\gear\\navigation\\data\\map_tree_ca.paa");
			if (player_pos != GetMapPos())
				m_DebugMapWidget.AddUserMark(GetMapPos(),"Pos", COLOR_BLUE,"\\dz\\gear\\navigation\\data\\map_tree_ca.paa");
		}
		UpdateMousePos();
		if (!m_PlayerPosRefreshBlocked)
			RefreshPlayerPosEditBoxes();
		
		
		foreach (MapMarker marker: ScriptConsole.m_MarkedEntities)
		{
			m_DebugMapWidget.AddUserMark(marker.GetMarkerPos(),marker.GetMarkerText(), marker.GetMarkerColor(), MapMarkerTypes.GetMarkerTypeFromID(marker.GetMarkerIcon()));
		}
	}
}
