class MissionBase extends MissionBaseWorld
{
	PluginDeveloper 		m_ModuleDeveloper;
	PluginKeyBinding		m_ModuleKeyBinding
	PluginAdditionalInfo	m_ModuleServerInfo;
	
	ref WidgetEventHandler 	m_WidgetEventHandler;
	ref WorldData			m_WorldData;
	ref WorldLighting		m_WorldLighting;
	
	ref array<PlayerBase> m_DummyPlayers = new array<PlayerBase>;

	void MissionBase()
	{
		SetDispatcher(new DispatcherCaller);
		
		PluginManagerInit();

		m_WidgetEventHandler = new WidgetEventHandler;
		
		//Debug.DestroyAllShapes();

		//TODO clea up after Gamescom
		m_ModuleServerInfo = PluginAdditionalInfo.Cast( GetPlugin(PluginAdditionalInfo) );
		//
		SoundSetMap.Init();

		if (GetGame().IsServer())
		{
			InitialiseWorldData();
		}
		else
		{
			GetDayZGame().GetAnalyticsClient().RegisterEvents();
			m_WorldLighting	= new WorldLighting;
		}
		
		// There is a possibility different maps/servers may be using different effects
		SEffectManager.Cleanup();
	}

	void ~MissionBase()
	{
		PluginManagerDelete();
		
		if ( GetGame().IsClient() )
		{
			GetDayZGame().GetAnalyticsClient().UnregisterEvents();	
		}
		TriggerEffectManager.DestroyInstance();
	}
	
	void InitialiseWorldData()
	{
		string worldName = "empty";
		GetGame().GetWorldName(worldName);
		worldName.ToLower();

		switch ( worldName )
		{
			case "chernarusplus":
				m_WorldData = new ChernarusPlusData;
				break;

			case "enoch":
				m_WorldData = new EnochData;
				break;

			default:
				m_WorldData = new ChernarusPlusData;
				break;
		}
	}

	override WorldLighting GetWorldLighting()
	{
		return m_WorldLighting;
	}
	
	override WorldData GetWorldData()
	{
		return m_WorldData;
	}
	
	override UIScriptedMenu CreateScriptedMenu(int id)
	{
		UIScriptedMenu menu = NULL;

		switch (id)
		{
		case MENU_MAIN:
#ifdef PLATFORM_CONSOLE			
			menu = new MainMenuConsole;
#else
			menu = new MainMenu;
#endif
			break;
		case MENU_INGAME:
#ifdef PLATFORM_CONSOLE
			menu = new InGameMenuXbox;
#else
			menu = new InGameMenu;
#endif
			break;
		case MENU_CHARACTER:
			menu = new CharacterCreationMenu;
			break;
		case MENU_OPTIONS:
			menu = new OptionsMenu;
			break;
//		case MENU_***: removed - can be recycled to something else
//			menu = new ***;
//			break;
		case MENU_STARTUP:
			menu = new StartupMenu;
			break;
		case MENU_LOADING:
			menu = new LoadingMenu;
			break;
		case MENU_INVENTORY:
			menu = new InventoryMenu;
			break;
		case MENU_INSPECT:
			menu = new InspectMenuNew;
			break;
		case MENU_EARLYACCESS:
			menu = new EarlyAccessMenu;
			break;
		case MENU_SCRIPTCONSOLE:
			menu = new ScriptConsole;
			break;
		case MENU_SCRIPTCONSOLE_DIALOG_PRESET_NAME:
			menu = new ScriptConsoleNewPresetDialog;
			break;
		case MENU_SCRIPTCONSOLE_DIALOG_PRESET_RENAME:
			menu = new ScriptConsoleRenamePresetDialog;
			break;
		case MENU_CHAT_INPUT:
			menu = new ChatInputMenu;
			break;
		case MENU_CONTROLS_PRESET:
			menu = new PresetsMenu;
			break;
		case MENU_NOTE:
			menu = new NoteMenu;
			break;
		case MENU_MAP:
			menu = new MapMenu;
			break;
		case MENU_BOOK:
			menu = new BookMenu;
			break;
		case MENU_SCENE_EDITOR:
			menu = new SceneEditorMenu;
			break;
		case MENU_HELP_SCREEN:
			menu = new HelpScreen;
			break;
		case MENU_GESTURES:
			menu = new GesturesMenu;
			break;	
		case MENU_LOGOUT:
			menu = new LogoutMenu;
			break;
		case MENU_TITLE_SCREEN:
			menu = new TitleScreenMenu;
			break;
		case MENU_XBOX_CONTROLS:
			menu = new ControlsXboxNew;
			break;
		case MENU_RADIAL_QUICKBAR:
			menu = new RadialQuickbarMenu;
			break;			
		case MENU_SERVER_BROWSER:
			menu = new ServerBrowserMenuNew;
			break;
		case MENU_LOGIN_QUEUE:
			menu = new LoginQueueBase;
			break;
		case MENU_LOGIN_TIME:
			menu = new LoginTimeBase;
			break;
		case MENU_CAMERA_TOOLS:
			menu = new CameraToolsMenu;
			break;
		case MENU_VIDEO:
			menu = new MainMenuVideo;
			break;
		case MENU_KEYBINDINGS:
			menu = new KeybindingsMenu;
			break;
		case MENU_TUTORIAL:
			menu = new TutorialsMenu;
			break;
		case MENU_CREDITS:
			menu = new CreditsMenu;
			break;
		case MENU_INVITE_TIMER:
			menu = new InviteMenu;
			break;
		case MENU_WARNING_ITEMDROP:
			menu = new ItemDropWarningMenu;
			break;
		case MENU_WARNING_TELEPORT:
			menu = new PlayerRepositionWarningMenu;
			break;
		case MENU_RESPAWN_DIALOGUE:
			menu = new RespawnDialogue;
			break;
		case MENU_CONNECT_ERROR:
			menu = new ConnectErrorScriptModuleUI;
			break;
		}

		if (menu)
		{
			menu.SetID(id);
		}

		return menu;
	}

	override UIScriptedWindow CreateScriptedWindow(int id)
	{
		UIScriptedWindow window = NULL;

		switch (id)
		{
		case GUI_WINDOW_MISSION_LOADER:
			window = new MissionLoader( GUI_WINDOW_MISSION_LOADER );
			break;
		}

		return window;
	}

	void SpawnItems()
	{
		/*vector player_pos = "2558 16 2854";
		if (g_Game.GetPlayer())
		{
			g_Game.GetPlayer().GetPosition();
		}

		Print(player_pos);
		
		Print("Spawning items.");
		ref TStringArray items = new TStringArray;
		items.Insert("Hoodie_Blue");
		items.Insert("WoolCoat_Red"); 
		items.Insert("Raincoat_Orange"); 
		items.Insert("PressVest_Blue"); 
		items.Insert("Gorka_pants_summer"); 
		items.Insert("MilitaryBoots_Black");
		items.Insert("WoodAxe");
		items.Insert("Container_FirstAidKit");
		items.Insert("Consumable_DuctTape");
		items.Insert("Fruit_AppleFresh");
		items.Insert("ItemBookHunger");
		
		for (int i = 0; i < items.Count(); i++)
		{
			string item = items.Get(i);
			vector item_pos = player_pos;
			int x = i % 4;
			int z = i / 4;
			item_pos[0] = item_pos[0] + (x * 1);
			item_pos[2] = item_pos[2] + (z * 1);
			g_Game.CreateObject(item, item_pos, false);
		}*/
	}

	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);
		
#ifdef DIAG_DEVELOPER
		if ( PluginKeyBinding.instance )
		{
			PluginKeyBinding.instance.OnKeyPress(key);
		}
#endif
	}
	
	int m_WidgetsTotal;
	int m_WidgetsInvisible;
	
	void DumpCurrentUILayout()
	{
		UIScriptedMenu current_menu = GetGame().GetUIManager().GetMenu();
		
		if ( current_menu )
		{
			Widget widget_root = current_menu.GetLayoutRoot();
			
			if ( widget_root )
			{
				m_WidgetsTotal = 0;
				m_WidgetsInvisible = 0;
				
				Print( widget_root.GetName() +" ("+ widget_root.GetTypeName() +")");
				
				DumpWidget(widget_root.GetChildren(), 1);
				
				Print( "Widgets TOTAL: "+ m_WidgetsTotal.ToString() +" INVISIBLE: "+ m_WidgetsInvisible.ToString() +" VISIBLE: "+ (m_WidgetsTotal - m_WidgetsInvisible).ToString() );
			}
		}
	}
	
	void DumpWidget(Widget w, int tabs)
	{
		if ( !w )
		{
			return;
		}
	
		m_WidgetsTotal++;
		
		string tmp;
		for (int i = 0; i < tabs; i++)
		{
			tmp += "  ";
		}
		
		string invisible = "";
		
		if ( !w.IsVisibleHierarchy() )
		{
			invisible = "[invisible]";
			m_WidgetsInvisible++;
		}
		
		Print( tmp +"- "+ w.GetName() +" ("+ w.GetTypeName() +") "+ invisible );
		
		bool collapse = false;
		if ( w.GetChildren() )
		{
			collapse = true;
			Print(tmp +"{");
		}
		
		DumpWidget(w.GetChildren(), tabs + 1);
			
		if ( collapse )
		{
			Print(tmp +"}");
		}
			
		DumpWidget(w.GetSibling(), tabs);
	}	

	override void OnKeyRelease(int key)
	{
		super.OnKeyRelease(key);
		
		if ( GetGame().IsDebug() )
		{
			if ( PluginKeyBinding.instance != NULL )
			{
				PluginKeyBinding.instance.OnKeyRelease(key);
			}
		}
	}
	
	override void OnMouseButtonPress(int button)
	{
		super.OnMouseButtonPress(button);
		
		if ( GetGame().IsDebug() )
		{
			if ( PluginKeyBinding.instance != NULL )
			{
				PluginKeyBinding.instance.OnMouseButtonPress(button);
			}
		}
	}
	
	override void OnMouseButtonRelease(int button)
	{
		super.OnMouseButtonRelease(button);
		
		if ( GetGame().IsDebug() )
		{
			if ( PluginKeyBinding.instance != NULL )
			{
				PluginKeyBinding.instance.OnMouseButtonRelease(button);
			}
		}
	}

	override void OnEvent(EventType eventTypeId, Param params)
	{
		super.OnEvent(eventTypeId, params);
		
		/*
		switch(eventTypeId)
		{
		case ScriptLogEventTypeID:
			if ( GetGame().IsDebug() )
			{
				Param1<string> log_params = params;
				OnLog(log_params.param1);
			}
			
			break;
		}
		*/
	}

	void OnLog(string msg_log)
	{
		if ( PluginDeveloper.GetInstance() )
		{
			if ( GetGame().IsServer() && !GetGame().IsMultiplayer() )
			{
				PluginDeveloper.GetInstance().PrintLogClient(msg_log);
			}
			else if ( GetGame().IsMultiplayer() && GetGame().IsClient() )
			{
				PluginDeveloper.GetInstance().PrintLogClient(msg_log);
			}
		}
	}
		
	void UpdateDummyScheduler()
	{
		for(int i = 0; i < m_DummyPlayers.Count(); i++)
		{
			if (m_DummyPlayers.Get(i))
			{
				m_DummyPlayers.Get(i).OnTick();
			}
		}
	}
	
	override void AddDummyPlayerToScheduler(Man player)
	{
		m_DummyPlayers.Insert(PlayerBase.Cast( player ));
	}

}

class MissionDummy extends MissionBase
{
	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);
		if (key == KeyCode.KC_Q)
		{
			// PlayerBase player = GetGame().GetPlayer();
		}
	}
}
