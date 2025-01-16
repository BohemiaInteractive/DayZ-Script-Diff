class MissionBase extends MissionBaseWorld
{
	PluginDeveloper 		m_ModuleDeveloper;
	PluginKeyBinding		m_ModuleKeyBinding
	
	ref WidgetEventHandler 	m_WidgetEventHandler;
	ref WorldData			m_WorldData;
	ref WorldLighting		m_WorldLighting;
	
	ref array<PlayerBase> m_DummyPlayers = new array<PlayerBase>;

	autoptr ObjectSnapCallback m_InventoryDropCallback;
	
	protected ref DynamicMusicPlayer m_DynamicMusicPlayer;
	protected ref DynamicMusicPlayerRegistry m_DynamicMusicPlayerRegistry;

	void MissionBase()
	{
		SetDispatcher(new DispatcherCaller);
		
		PluginManagerInit();

		m_WidgetEventHandler = new WidgetEventHandler();

		m_InventoryDropCallback = new EntityPlacementCallback();
		SoundSetMap.Init();
		
		GetDayZGame().GetYieldDataInitInvoker().Insert(InitWorldYieldDataDefaults);
		InitialiseWorldData();
		
		#ifndef SERVER
		GetDayZGame().GetAnalyticsClient().RegisterEvents();
		m_WorldLighting	= new WorldLighting();
		
		m_DynamicMusicPlayer = new DynamicMusicPlayer(m_DynamicMusicPlayerRegistry);
			#ifdef DIAG_DEVELOPER
			GetOnTimeChanged().Insert(m_DynamicMusicPlayer.SetTimeOfDate);
			#endif
		AmmoTypesAPI.Init();
		#endif
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			OutdoorThermometerManager.Init();
		}
		GetOnInputDeviceConnected().Insert(UpdateInputDevicesAvailability);
		GetOnInputDeviceDisconnected().Insert(UpdateInputDevicesAvailability);
		
		// There is a possibility different maps/servers may be using different effects
		SEffectManager.Cleanup();
		GetUApi().UpdateControls();
	}

	void ~MissionBase()
	{
		// prevent use after free with typename being destructed when switching mission / exitting game
		SetDispatcher(null);

		GetDayZGame().GetYieldDataInitInvoker().Remove(InitWorldYieldDataDefaults);
		
		PluginManagerDelete();
		
		if ( GetGame().IsClient() )
		{
			GetDayZGame().GetAnalyticsClient().UnregisterEvents();	
		}
		TriggerEffectManager.DestroyInstance();
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			OutdoorThermometerManager.Cleanup();
		}
		
		#ifndef SERVER
			AmmoTypesAPI.Cleanup();
			ExplosivesBase.Cleanup();
		#endif
	}

	override ObjectSnapCallback GetInventoryDropCallback()
	{
		return m_InventoryDropCallback;
	}
	
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		
		#ifndef SERVER
		m_DynamicMusicPlayer.OnUpdate(timeslice);
		#endif
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			OutdoorThermometerManager.Update(timeslice);
		}
	}
	
	override void OnEvent(EventType eventTypeId, Param params)
	{
		super.OnEvent(eventTypeId, params);
		
		m_DynamicMusicPlayer.OnGameEvent(eventTypeId, params);
	}
	
	void InitialiseWorldData()
	{
		string worldName = "empty";
		GetGame().GetWorldName(worldName);
		worldName.ToLower();
		
		switch (worldName)
		{
			case "chernarusplus":
				m_WorldData = new ChernarusPlusData();
				m_DynamicMusicPlayerRegistry = new DynamicMusicPlayerRegistryChernarus();
				break;

			case "enoch":
				m_WorldData = new EnochData();
				m_DynamicMusicPlayerRegistry = new DynamicMusicPlayerRegistryEnoch();
				break;

			case "sakhal":
				m_WorldData = new SakhalData();
				m_DynamicMusicPlayerRegistry = new DynamicMusicPlayerRegistrySakhal();
				break;
			
			#ifdef PLATFORM_CONSOLE
			case "mainmenuscenexbox":
				m_WorldData = new MainMenuWorldData();
				m_DynamicMusicPlayerRegistry = new DynamicMusicPlayerRegistry();
				break
			#endif

			default:
				m_WorldData = new ChernarusPlusData();
				m_DynamicMusicPlayerRegistry = new DynamicMusicPlayerRegistry();
				break;
		}
	}
	
	//! Used to initialize defaults to WorldData base class from 4_World module. For proper init of your custom world, use 'InitYieldBank' method in the inherited world data
	void InitWorldYieldDataDefaults(CatchYieldBank bank)
	{
		//catch yield data beyond 3_Game level, defaults used for ChernarusPlusData
		if (bank)
		{
			//fishies
			bank.RegisterYieldItem(new YieldItemCarp(42));
			bank.RegisterYieldItem(new YieldItemMackerel(42));
			bank.RegisterYieldItem(new YieldItemSardines(15));
			bank.RegisterYieldItem(new YieldItemBitterlings(15));
			
			//fishy junk
			bank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Brown"));
			bank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Grey"));
			bank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Green"));
			bank.RegisterYieldItem(new YieldItemJunk(1,"Wellies_Black"));
			bank.RegisterYieldItem(new YieldItemJunkEmpty(1,"Pot"));
			
			//non-fishies
			bank.RegisterYieldItem(new YieldItemDeadRabbit(4));
			bank.RegisterYieldItem(new YieldItemDeadRooster(1));
			bank.RegisterYieldItem(new YieldItemDeadChicken_White(1));
			bank.RegisterYieldItem(new YieldItemDeadChicken_Spotted(1));
			bank.RegisterYieldItem(new YieldItemDeadChicken_Brown(1));
			bank.RegisterYieldItem(new YieldItemDeadFox(2));
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
	
	override DynamicMusicPlayer GetDynamicMusicPlayer()
	{
		return m_DynamicMusicPlayer;
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
		case MENU_SCRIPTCONSOLE_UNIVERSAL_INFO_DIALOG:
			menu = new ScriptConsoleUniversalInfoDialog;
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
		case MENU_WARNING_INPUTDEVICE_DISCONNECT:
			menu = new InputDeviceDisconnectWarningMenu;
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
		case MENU_LOC_ADD:
			menu = new ScriptConsoleAddLocation;
			break;	
		case MENU_MISSION_LOADER:
			menu = new MissionLoader;
			break;
		case MENU_CONNECTION_DIALOGUE:
			menu = new ConnectionDialogue;
			break;
		}

		if (menu)
		{
			menu.SetID(id);
		}

		return menu;
	}

	void SpawnItems();
	
	void UpdateInputDevicesAvailability()
	{
		g_Game.RefreshMouseCursorVisibility();
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

	#ifdef DIAG_DEVELOPER
	void UpdateInputDeviceDiag()
	{
		DisplayInputDebug(DiagMenu.GetBool(DiagMenuIDs.MISC_INPUT_DEVICE_DISCONNECT_DBG));
	}
	
	void DisplayInputDebug(bool show)
	{
		DbgUI.BeginCleanupScope();     
		DbgUI.Begin("InputDeviceDebug", 60, 60);
		
		if (show)
		{
			DbgUI.Text("Gamepad: " + g_Game.GetInput().IsActiveGamepadSelected());
			DbgUI.Text("Mouse: " + g_Game.GetInput().IsMouseConnected());
			DbgUI.Text("Keyboard: " + g_Game.GetInput().IsKeyboardConnected());
		}
		
		DbgUI.End();
		DbgUI.EndCleanupScope();
	}
	#endif
	
	//! DEPRECATED
	PluginAdditionalInfo	m_ModuleServerInfo;
}

class MissionDummy extends MissionBase
{
}
