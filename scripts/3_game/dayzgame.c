class CrashSoundSets
{
	static ref map<int, string> m_Mappings = new map<int, string>;
	
	static void RegisterSoundSet(string sound_set)
	{
		m_Mappings.Set(sound_set.Hash(), sound_set);
	}
	
	static string GetSoundSetByHash(int hash)
	{
		string sound_set;
		if(m_Mappings)
			m_Mappings.Find(hash,sound_set);
		return sound_set;
	}
};


class LoginQueueBase extends UIScriptedMenu
{
	protected TextWidget m_txtPosition;
	protected TextWidget m_txtNote;
	protected ButtonWidget m_btnLeave;
	
	protected int m_iPosition; // position in login queue
	protected bool m_IsStatic;
	
	void LoginQueueBase()
	{		
		m_iPosition = -1;
		m_IsStatic = false;
		g_Game.SetKeyboardHandle(this);	
	}

	void ~LoginQueueBase()
	{
		g_Game.SetKeyboardHandle(NULL);	
	}
	
	override Widget Init()
	{		
		layoutRoot 		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/dialog_queue_position.layout");

		m_txtPosition	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtPosition"));
		m_txtNote 		= TextWidget.Cast(layoutRoot.FindAnyWidget("txtNote"));
		m_btnLeave 		= ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLeave"));
		m_txtNote.Show(true);
		layoutRoot.FindAnyWidget("notification_root").Show(false);
		
		#ifdef PLATFORM_CONSOLE
		layoutRoot.FindAnyWidget("toolbar_bg").Show(true);
		RichTextWidget toolbar_b = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		#ifdef PLATFORM_XBOX
		#ifdef BUILD_EXPERIMENTAL
		layoutRoot.FindAnyWidget("notification_root").Show(true);
		#endif
		#endif
		#endif

		return layoutRoot;
	}
	
	override void Update(float timeslice)
	{
		if ( GetGame().GetInput().LocalPress("UAUIBack", false) )
		{
			Leave();
		}
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		if ( w == m_btnLeave )
		{
			Leave();
			return true;
		}
		return false;
	}
	
	void Show()
	{
		if (layoutRoot)
			layoutRoot.Show(true);
	}
	
	void Hide()
	{
		if (layoutRoot)
			layoutRoot.Show(false);
	}
	
	void SetPosition(int position)
	{
		if ( position != m_iPosition )
		{
			m_iPosition = position;
			m_txtPosition.SetText(position.ToString());
		}
	}
	
	bool IsStatic()
	{
		return m_IsStatic;
	}
	
	void Leave()
	{
		g_Game.SetGameState( DayZGameState.MAIN_MENU );
		g_Game.SetLoadState( DayZLoadState.MAIN_MENU_START );

		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(GetGame().DisconnectSessionForce);
		
		Close();
	}
};


//! LoginQueue position when using -connect since mission is not created yet 
class LoginQueueStatic extends LoginQueueBase
{
	void LoginQueueStatic()
	{
		Init();
		
		m_IsStatic = true;
	}
	
	void ~LoginQueueStatic()
	{
	}
};

class LoginTimeBase extends UIScriptedMenu
{
	protected TextWidget m_txtDescription;
	protected TextWidget m_txtLabel;
	protected ButtonWidget m_btnLeave;
	protected bool m_IsStatic;
	protected bool m_IsRespawn;
	
	void LoginTimeBase()
	{
		g_Game.SetKeyboardHandle(this);
		m_IsStatic = false;	
		m_IsRespawn = false;
	}

	void ~LoginTimeBase()
	{
		g_Game.SetKeyboardHandle(NULL);	
	}
	
	override Widget Init()
	{
		layoutRoot 			= GetGame().GetWorkspace().CreateWidgets("gui/layouts/dialog_login_time.layout");
		
		m_txtDescription 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtDescription"));
		m_txtLabel 			= TextWidget.Cast(layoutRoot.FindAnyWidget("txtLabel"));
		m_btnLeave 			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLeave"));
		m_txtDescription.Show(true);
		layoutRoot.FindAnyWidget("notification_root").Show(false);
		
		#ifdef PLATFORM_CONSOLE
		layoutRoot.FindAnyWidget("toolbar_bg").Show(true);
		RichTextWidget toolbar_b = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		#ifdef PLATFORM_XBOX
		#ifdef BUILD_EXPERIMENTAL
		layoutRoot.FindAnyWidget("notification_root").Show(true);
		#endif
		#endif
		#endif

		return layoutRoot;
	}
	
	override void Update(float timeslice)
	{
		if ( GetGame().GetInput().LocalPress("UAUIBack", false) )
		{
			Leave();
		}
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		if ( w == m_btnLeave )
		{
			Leave();
			return true;
		}
		return false;
	}
	
	void Show()
	{
		if (layoutRoot)
			layoutRoot.Show(true);
	}
	
	void Hide()
	{
		if (layoutRoot)
			layoutRoot.Show(false);
	}
	
	void SetTime(int time)
	{
		if (!m_IsRespawn)
			m_txtLabel.SetText("#menu_loading_in " + time.ToString() + " #dayz_game_seconds");
		else
			m_txtLabel.SetText("#dayz_game_spawning_in " + time.ToString() + " #dayz_game_seconds");
	}
		
	void SetStatus(string status)
	{
		m_txtDescription.SetText(status);
	}
	
	void SetRespawn(bool value)
	{
		m_IsRespawn = value;
	}
	
	bool IsStatic()
	{
		return m_IsStatic;
	}
	
	bool IsRespawn()
	{
		return m_IsRespawn;
	}
	
	void Leave()
	{
		g_Game.SetGameState( DayZGameState.MAIN_MENU );
		g_Game.SetLoadState( DayZLoadState.MAIN_MENU_START );
		
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(GetGame().DisconnectSessionForce);
		
		Close();
	}
};

//! LoginTime when using -connect since mission is not created yet 
class LoginTimeStatic extends LoginTimeBase
{
	void LoginTimeStatic()
	{
		Init();
		
		m_IsStatic = true;
	}
	
	void ~LoginTimeStatic()
	{
	}
};

class LoadingScreen
{
	ref Widget m_WidgetRoot;
	//Widget m_FrameWidgetSmall;
	//Widget m_FrameWidgetLarge;
	//ProgressBarWidget m_ProgressBarSmall;
	//ProgressBarWidget m_ProgressBarLarge;
	//ProgressBarWidget m_ProgressBar;
	TextWidget m_TextWidgetTitle;
	TextWidget m_TextWidgetStatus;
	TextWidget m_ModdedWarning;
	ImageWidget m_ImageWidgetBackground;
	DayZGame m_DayZGame;
	float m_LastProgressUpdate;
	
	ImageWidget m_ImageLogoMid;
	ImageWidget m_ImageLogoCorner;
	ImageWidget m_ImageLoadingIcon;
	ImageWidget m_ImageBackground;
	ProgressBarWidget m_ProgressLoading;
	float m_ImageLoadingIconRotation;
	TextWidget m_ProgressText;
	
	//ref Timer m_Timer = new Timer(CALL_CATEGORY_SYSTEM);
	
	int m_Counter;
	ref Timer m_Timer;
	
	void LoadingScreen(DayZGame game)
	{
		
		m_DayZGame = game;
		
		m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets("gui/layouts/loading.layout");
		//m_FrameWidgetSmall = m_WidgetRoot.FindAnyWidget("ProgressBarFrameSmall");
		//m_FrameWidgetLarge = m_WidgetRoot.FindAnyWidget("ProgressBarFrameLarge");
		//Class.CastTo(m_ProgressBarSmall, m_WidgetRoot.FindAnyWidget("ProgressBarWidgetSmall"));
		//Class.CastTo(m_ProgressBarLarge, m_WidgetRoot.FindAnyWidget("ProgressBarWidgetLarge"));
		Class.CastTo(m_ImageLogoMid, m_WidgetRoot.FindAnyWidget("ImageLogoMid"));
		Class.CastTo(m_ImageLogoCorner, m_WidgetRoot.FindAnyWidget("ImageLogoCorner"));
		
		Class.CastTo(m_TextWidgetTitle, m_WidgetRoot.FindAnyWidget("TextWidget"));
		Class.CastTo(m_TextWidgetStatus, m_WidgetRoot.FindAnyWidget("StatusText"));
		Class.CastTo(m_ImageWidgetBackground, m_WidgetRoot.FindAnyWidget("ImageBackground"));
		Class.CastTo(m_ImageLoadingIcon, m_WidgetRoot.FindAnyWidget("ImageLoadingIcon"));
		Class.CastTo(m_ModdedWarning, m_WidgetRoot.FindAnyWidget("ModdedWarning"));
		
		m_ImageBackground = ImageWidget.Cast( m_WidgetRoot.FindAnyWidget("ImageBackground") );
		m_ProgressLoading = ProgressBarWidget.Cast( m_WidgetRoot.FindAnyWidget("LoadingBar") );
		
		string tmp;
		m_ProgressText = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("ProgressText"));
		if (GetGame())
		{
			m_ProgressText.Show( GetGame().CommandlineGetParam("loadingTest", tmp) );
		}
		m_WidgetRoot.FindAnyWidget("notification_root").Show(false);
		
		#ifdef PLATFORM_CONSOLE
		#ifdef PLATFORM_XBOX
		#ifdef BUILD_EXPERIMENTAL
			Widget exp_notifiaction = m_WidgetRoot.FindAnyWidget("notification_root");
			if (exp_notifiaction)
			{
				exp_notifiaction.Show(true);
			}
		#endif
		#endif
		#endif
		
		m_ModdedWarning.Show( g_Game.ReportModded() );
		m_ImageLogoMid.Show(true);
		m_ImageLogoCorner.Show(false);
		
		//m_ImageLoadingIcon.Show(true);
		//m_ProgressText.Show(true);
		
		//m_ProgressBar = m_ProgressBarLarge;
		//m_ProgressBar.SetCurrent(0);
		//m_FrameWidgetSmall.Show( false );
		//m_ImageWidgetBackground.LoadImageFile( 0, GetRandomLoadingBackground() );
		m_ImageWidgetBackground.Show( true );		
		m_Counter = 0;
				
		// lighten up your desktop
		game.GetBacklit().LoadingAnim();
		
		ProgressAsync.SetProgressData(m_ProgressLoading);
		ProgressAsync.SetUserData(m_ImageBackground);
	}
	
	void OnTimer()
	{
		if ( GetGame() )
		{
			Print("Loading Update: "+ GetGame().GetTickTime());	
		}
		else
		{
			Print("Loading Update...");	
		}
	}
	
	void Inc()
	{
		m_LastProgressUpdate = m_DayZGame.GetTickTime();
		
		m_Counter++;
		//Print("Loading Inc: "+ m_Counter);
		if ( m_Counter == 1 )
		{
			Show();
		}
	}
	
	void Dec()
	{
		m_Counter = m_Counter - 1;
	
		if ( m_Counter <= 0 )
		{
			m_Counter = 0;
			
			if ( m_Timer )
			{
				delete m_Timer;
			}
			
			/*
			m_Timer = new Timer();
			m_Timer.Run(2, this,"EndLoading", NULL, false);
			*/
			EndLoading();
		}
	}
	
	void EndLoading()
	{
		ProgressAsync.SetProgressData(null);
		ProgressAsync.SetUserData(null);
		m_WidgetRoot.Show( false );
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call( g_Game.CheckDialogs );
	}
	
	bool IsLoading()
	{
		if ( m_WidgetRoot.IsVisible() )
		{
			return true;
		}
	
		return false;
	}
	
	void SetTitle( string title )
	{
		m_TextWidgetTitle.SetText( title );
	}
	/*
	void HideTitle()
	{
		m_TextWidgetTitle.Show(false);
	}
	
	void ShowTitle()
	{
		m_TextWidgetTitle.Show(true);
	}
	*/
	
	void SetStatus( string status )
	{
		m_TextWidgetStatus.SetText( status );
	}
	
	void SetProgress( float val )
	{
		//m_ProgressText.SetText(val.ToString() + " %");
		
		float time_delta = m_DayZGame.GetTickTime() - m_LastProgressUpdate;
		
		//m_ProgressLoading.SetCurrent( val * 100 );
		//m_ImageBackground.SetMaskProgress( val );
		
//Print("SetProgress delta: "+ time_delta);
		
		if ( m_Counter > 0 )
		{
			//m_ImageLoadingIconRotation += 15;
			//m_ImageLoadingIcon.SetRotation(0, 0, m_ImageLoadingIconRotation);
			
			//m_ProgressBar.SetCurrent( val * 100 );
		}
		
		m_LastProgressUpdate = m_DayZGame.GetTickTime();
	}
	
	void OnUpdate( float timeslice )
	{

	}
	
	void Show()
	{
		Widget lIcon = m_ImageBackground;
		Widget pText = m_ProgressLoading;
		m_ProgressText.SetText("");
		m_ProgressLoading.SetCurrent( 0.0 );
		m_ImageBackground.SetMaskProgress( 0.0 );
		
		if ( !m_WidgetRoot.IsVisible() )
		{
			if ( m_DayZGame.GetUIManager().IsDialogVisible() )
			{
				m_DayZGame.GetUIManager().HideDialog();
			}
			
			if ( m_DayZGame.GetMissionState() == DayZGame.MISSION_STATE_MAINMENU )
			{
				//m_ProgressBar = m_ProgressBarSmall;
				m_ImageLogoMid.Show(false);
				m_ImageLogoCorner.Show(false);				
				//m_FrameWidgetSmall.Show( true );
				//m_FrameWidgetLarge.Show( false );
				m_ImageWidgetBackground.Show( true );		
				m_TextWidgetStatus.Show(true);
				//lIcon = null;
				//pText = null;
			}
			else
			{
				//m_ProgressBar = m_ProgressBarLarge;
				m_ImageLogoMid.Show(true);
				m_ImageLogoCorner.Show(false);
				//m_FrameWidgetSmall.Show( false );
				//m_FrameWidgetLarge.Show( true );
				m_ImageWidgetBackground.Show( true );
				m_TextWidgetStatus.Show(false);
			}
			
			m_WidgetRoot.Show( true );
			//m_ProgressBar.SetCurrent( 0 );
			m_TextWidgetTitle.SetText( "" );
			m_TextWidgetStatus.SetText( "" );
		}
		
		ProgressAsync.SetProgressData(pText);
		ProgressAsync.SetUserData(lIcon);
	}
	
	void Hide(bool force)
	{
		if (force)
		{
			while (m_Counter > 0)
			{
				Dec();
			}
		}

		if ( m_Counter <= 0 /*&& m_WidgetRoot.IsVisible()*/ )
		{
			m_WidgetRoot.Show( false );
			
			//Print("***** m_WidgetRoot.Show false");
			
			ProgressAsync.SetUserData(null);
			ProgressAsync.SetProgressData(null);
		}
	}
};

class ConnectionLost
{
	private ref Widget m_WidgetRoot;
	private TextWidget m_TextWidgetTitle;
	private float m_duration;
		
	void ConnectionLost(DayZGame game)
	{		
		m_WidgetRoot = game.GetWorkspace().CreateWidgets("gui/layouts/day_z_connection_lost.layout");
		m_WidgetRoot.Show(false);
		
		Class.CastTo(m_TextWidgetTitle, m_WidgetRoot.FindAnyWidget("TextWidget"));
		m_duration = 0.0;
	}
	
	void Show()
	{
		if ( !m_WidgetRoot.IsVisible() )
		{
			if ( g_Game.GetUIManager().IsDialogVisible() )
			{
				g_Game.GetUIManager().HideDialog();
			}
			
			m_WidgetRoot.Show( true );
			m_TextWidgetTitle.SetText( "" );
		}
	}
	
	void Hide()
	{
		if ( m_WidgetRoot.IsVisible() )
		{
			m_WidgetRoot.Show( false );
		}
	}
	
	void SetText(string text)
	{
		m_TextWidgetTitle.SetText(text);
	}
	
	float GetDuration()
	{
		return m_duration;
	}
	
	void SetDuration(float duration)
	{
		m_duration = duration;
	}
};

typedef Param3<string, bool, bool> DayZProfilesOption;
class DayZProfilesOptions
{
	private ref map<EDayZProfilesOptions, ref DayZProfilesOption> m_DayZProfilesOptions;
	private DayZGame m_Game

	void RegisterProfileOption(EDayZProfilesOptions option, string profileOptionName, bool def = true)
	{
		if ( !m_DayZProfilesOptions )
		{
			m_DayZProfilesOptions = new map<EDayZProfilesOptions, ref DayZProfilesOption>;
		}
		
		if ( !m_DayZProfilesOptions.Contains(option) )
		{
			//! init of DayZProfilesOption - profileOptionName, value from Profiles files, or use default value
			bool profileVal = GetGame().GetProfileValueBool(profileOptionName, def);

			m_DayZProfilesOptions.Set(option, new DayZProfilesOption(profileOptionName, profileVal, def));
			SetProfileOption(option, profileVal);
		}
	}
	
	void ResetOptions()
	{
		if ( !m_DayZProfilesOptions )
		{
			m_DayZProfilesOptions = new map<EDayZProfilesOptions, ref DayZProfilesOption>;
		}
		foreach ( EDayZProfilesOptions e_opt, DayZProfilesOption r_opt : m_DayZProfilesOptions )
		{
			bool profileVal = GetGame().GetProfileValueBool(r_opt.param1, r_opt.param3);
			SetProfileOption(e_opt, profileVal);
		}	
	}
	
	void SetProfileOption(EDayZProfilesOptions option, bool value)
	{
		//Print("SetProfileOption");
		if ( m_DayZProfilesOptions && m_DayZProfilesOptions.Contains(option) )
		{
			DayZProfilesOption po = m_DayZProfilesOptions.Get(option);
			
			//! saves value to mem and to config
			po.param2 = value;
			GetGame().SetProfileValueBool(po.param1, value);
			GetGame().SaveProfile();
		}	
	}

	bool GetProfileOption(EDayZProfilesOptions option)
	{
		if ( m_DayZProfilesOptions && m_DayZProfilesOptions.Contains(option) )
		{
			DayZProfilesOption po = m_DayZProfilesOptions.Get(option);
			
			//! returns saved profile option
			return po.param2;
		}
		
		return true;
	}
	
	bool GetProfileOptionDefault(EDayZProfilesOptions option)
	{
		if ( m_DayZProfilesOptions && m_DayZProfilesOptions.Contains(option) )
		{
			DayZProfilesOption po = m_DayZProfilesOptions.Get(option);
			
			//! returns default value
			return po.param3;
		}
		
		ErrorEx("Invalid profile option id! Returning 'true'.",ErrorExSeverity.WARNING);
		return true;
	}
	
	map<EDayZProfilesOptions, ref DayZProfilesOption> GetProfileOptionMap()
	{
		if ( m_DayZProfilesOptions )
		{			
			//! returns whole options map
			return m_DayZProfilesOptions;
		}
		return NULL;
	}
}

enum DayZGameState
{
	UNDEFINED,
	MAIN_MENU,
	JOIN,
	PARTY,
	CONNECT,
	CONNECTING,
	IN_GAME
}

enum DayZLoadState
{
	UNDEFINED,
	MAIN_MENU_START,
	MAIN_MENU_CONTROLLER_SELECT,
	MAIN_MENU_USER_SELECT,
	JOIN_START,
	JOIN_CONTROLLER_SELECT,
	JOIN_USER_SELECT,
	PARTY_START,
	PARTY_CONTROLLER_SELECT,
	PARTY_USER_SELECT,
	CONNECT_START,
	CONNECT_USER_SELECT,
	CONNECT_CONTROLLER_SELECT,
	MISSION_START,
	MISSION_USER_SELECT,
	MISSION_CONTROLLER_SELECT
}

class DayZGame extends CGame
{
	const int MISSION_STATE_MAINMENU = 0;
	const int MISSION_STATE_GAME = 1;
	const int MISSION_STATE_FINNISH = 2;
	
	private int m_MissionState;
	
	//HK stuff
	protected DayZGameState						m_GameState;
	protected DayZLoadState						m_LoadState;
	protected ref NotificationUI				m_Notifications;
	protected bool								m_FirstConnect = true;
	//End HK stuff
	
	ref LoadingScreen m_loading;
	private ref LoginTimeBase m_LoginTimeScreen;
	private ref LoginQueueBase m_LoginQueue;
	private int m_LoginTime;
	
	private ref ConnectionLost m_connectionLost;
	private ref TimerQueue m_timerQueue[CALL_CATEGORY_COUNT];
	private ref ScriptCallQueue m_callQueue[CALL_CATEGORY_COUNT];
	private ref ScriptInvoker m_updateQueue[CALL_CATEGORY_COUNT];
	private ref DragQueue m_dragQueue;
	private ref DayZProfilesOptions m_DayZProfileOptions;
	private bool m_early_access_dialog_accepted;
	private UIScriptedMenu m_keyboard_handler;
	private ScriptModule m_mission_module;
	private string 	m_MissionPath;
	private string 	m_MissionFolderPath;
	private bool	m_IsCtrlHolding;
	private bool	m_IsWinHolding;
	private bool	m_IsLeftAltHolding;
	private bool	m_IsRightAltHolding;
	
	private bool	m_IsWorldWetTempUpdateEnabled = true;
	private bool	m_IsFoodDecayEnabled = true;
	private float	m_FoodDecayModifier;
	
	static bool		m_ReportModded;
	private bool	m_IsStressTest;
	private bool 	m_AimLoggingEnabled;
	int 			m_OriginalCharactersCount;
	private string 	m_PlayerName;
	private bool 	m_IsNewCharacter;
	private bool 	m_IsConnecting;
	private bool	m_ConnectFromJoin;
	private bool	m_ShouldShowControllerDisconnect;
	private int		m_PreviousGamepad;
	private float	m_UserFOV;
	float 	m_volume_sound;
	float 	m_volume_speechEX;
	float 	m_volume_music;
	float 	m_volume_VOIP;
	float 	m_volume_radio;
	
	float 	m_PreviousEVValue;
	float 	m_EVValue = 0;
	
	static ref AmmoCamParams m_AmmoShakeParams = new AmmoCamParams();//TODO: make static, reuse
	
	static ref ScriptInvoker Event_OnRPC = new ScriptInvoker();
	
	private ref Backlit m_Backlit;
	
	private ref array<string> m_CharClassNames = new array<string>();
	
	//Used for helicrash sound
	private ref EffectSound m_CrashSound;
	
	//Used for Artillery sound
	private ref EffectSound m_ArtySound;
	private const int MIN_ARTY_SOUND_RANGE = 300; // The distance under which sound is no longer heard
	
	static ref NoiseParams m_NoiseParams = new NoiseParams();
	#ifdef DEVELOPER
	static bool m_IsPreviewSpawn;//! we set this to 'true' when spawning item through Script Console only for item preview purposes
	ref CameraToolsMenuServer m_CameraToolsMenuServer;
	#endif
	// CGame override functions
	void DayZGame()
	{
		PPEManagerStatic.CreateManagerStatic();
		
#ifdef PLATFORM_CONSOLE
		SetMainMenuWorld("MainMenuSceneXbox");
#endif
		m_MissionState = MISSION_STATE_GAME;
		
		m_keyboard_handler = null;
		
		#ifdef DEVELOPER
		m_early_access_dialog_accepted = true;
		#endif
		
		for (int i = 0; i < CALL_CATEGORY_COUNT; i++)
		{
			m_callQueue[i] = new ScriptCallQueue;
			m_updateQueue[i] = new ScriptInvoker;
			m_timerQueue[i] = new TimerQueue;
		}
		
		m_dragQueue = new DragQueue;
		
		m_LoginTime 		 = 0;
		
		string tmp;
		if ( CommandlineGetParam("stresstest", tmp) )
		{
			m_IsStressTest = true;
		}
		if ( CommandlineGetParam("doAimLogs", tmp) )
		{
			m_AimLoggingEnabled = true;
		}
		
		/*m_ParamNewMenu= false;
		if ( CommandlineGetParam("enNewMenu", tmp) )
		{
			m_ParamNewMenu = true;
		}*/	
		
		// initialize backlit effects
		m_Backlit = new Backlit();
		m_Backlit.OnInit(this);
		
		m_ReportModded = GetModToBeReported();

	#ifndef NO_GUI
		if (m_loading == null)
		{
			m_loading = new LoadingScreen(this);
		}
		
		m_loading.Show();
	#endif
			
		Debug.Init();
		Component.Init();
		LogTemplates.Init();
		CachedObjectsParams.Init();
		CachedObjectsArrays.Init();

		m_DayZProfileOptions = new DayZProfilesOptions;
		GetCallQueue(CALL_CATEGORY_GUI).Call(DeferredInit);
		GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(GlobalsInit);
		//m_isTileSet = true;
		
		string path = "cfgVehicles";
		string child_name = "";
		int count = ConfigGetChildrenCount( path );
		
		for (int p = 0; p < count; ++p)
		{
			ConfigGetChildName( path, p, child_name );
			
			if (ConfigGetInt(path + " " + child_name + " scope") == 2 && IsKindOf(child_name, "SurvivorBase"))
				m_CharClassNames.Insert(child_name);
		}
		
		m_IsConnecting = false;
		m_ConnectFromJoin = false;
	}
	
	// ------------------------------------------------------------
	private void ~DayZGame()
	{
		PPEManagerStatic.DestroyManagerStatic();
		
		g_Game = null;
		SetDispatcher(null);
		Print("~DayZGame()");
	}
	
	// ------------------------------------------------------------
	void DeferredInit()
	{
		GameOptions opt = new GameOptions;
		opt.Initialize();
		
		RegisterProfilesOptions();
		m_UserFOV = GetUserFOVFromConfig();
		
		m_volume_sound = GetSoundScene().GetSoundVolume();
		m_volume_speechEX = GetSoundScene().GetSpeechExVolume();
		m_volume_music = GetSoundScene().GetMusicVolume();
		m_volume_VOIP = GetSoundScene().GetVOIPVolume();
		m_volume_radio = GetSoundScene().GetRadioVolume();
		
		//m_CharacterData = new MenuDefaultCharacterData;
		PPEManagerStatic.GetPPEManager().Init();
		GetMenuDefaultCharacterData();
	}
	// ------------------------------------------------------------
	void GlobalsInit()
	{
		if ( GetCEApi() )
		{
			m_IsWorldWetTempUpdateEnabled = ( GetCEApi().GetCEGlobalInt( "WorldWetTempUpdate" ) == 1 );
			
			m_FoodDecayModifier = GetCEApi().GetCEGlobalFloat( "FoodDecay" );
			
			if ( m_FoodDecayModifier == float.MIN)//check for legacy INT format, if value == float.MIN when read as FLOAT, it is of type INT, so we read it as such bellow
			{
				m_FoodDecayModifier = GetCEApi().GetCEGlobalInt( "FoodDecay" );
			}

			//Print("IsFoodDecayEnabled():" + IsFoodDecayEnabled());
			//Print("m_FoodDecayModifier:" + m_FoodDecayModifier);
		}
	}
	
	// ------------------------------------------------------------
	void RegisterProfilesOptions()
	{
		m_DayZProfileOptions.RegisterProfileOption(EDayZProfilesOptions.CROSSHAIR, SHOW_CROSSHAIR);
		m_DayZProfileOptions.RegisterProfileOption(EDayZProfilesOptions.HUD , SHOW_HUD);
		m_DayZProfileOptions.RegisterProfileOption(EDayZProfilesOptions.QUICKBAR, SHOW_QUICKBAR);
		m_DayZProfileOptions.RegisterProfileOption(EDayZProfilesOptions.SERVER_MESSAGES, SYSTEM_CHAT_MSG);
		m_DayZProfileOptions.RegisterProfileOption(EDayZProfilesOptions.USERS_CHAT, DIRECT_CHAT_MSG);
		m_DayZProfileOptions.RegisterProfileOption(EDayZProfilesOptions.RADIO_CHAT, RADIO_CHAT_MSG);
		m_DayZProfileOptions.RegisterProfileOption(EDayZProfilesOptions.GAME_MESSAGES, GAME_CHAT_MSG, false);
		m_DayZProfileOptions.RegisterProfileOption(EDayZProfilesOptions.ADMIN_MESSAGES, ADMIN_CHAT_MSG, false);
		m_DayZProfileOptions.RegisterProfileOption(EDayZProfilesOptions.PLAYER_MESSAGES, PLAYER_CHAT_MSG, false);
		m_DayZProfileOptions.RegisterProfileOption(EDayZProfilesOptions.SERVERINFO_DISPLAY, SHOW_SERVERINFO, true);
		m_DayZProfileOptions.RegisterProfileOption(EDayZProfilesOptions.BLEEDINGINDICATION, ENABLE_BLEEDINGINDICATION, true);
	}
	
	void ResetProfileOptions()
	{
		m_DayZProfileOptions.ResetOptions();
	}
	
	//! Called from C++
	void SetMissionPath(string path)
	{
		m_MissionPath = path;
		
		int pos_end = 0;
		int pos_cur = 0;
				
		while ( pos_cur != -1 )
		{
			pos_end = pos_cur;
			pos_cur = path.IndexOfFrom(pos_cur + 1 , "\\");
		}
		
		m_MissionFolderPath = path.Substring(0, pos_end);	
	}
	
	string GetMissionPath()
	{
		return m_MissionPath;
	}
	
	string GetMissionFolderPath()
	{
		return m_MissionFolderPath;
	}
	
	override ScriptCallQueue GetCallQueue(int call_category)
	{
		return m_callQueue[call_category];
	}

	override ScriptInvoker GetUpdateQueue(int call_category)
	{
		return m_updateQueue[call_category];
	}
	
	override TimerQueue GetTimerQueue(int call_category)
	{
		return m_timerQueue[call_category];
	}

	override DragQueue GetDragQueue()
	{
		return m_dragQueue;
	}
	
	// ------------------------------------------------------------
	void OnGameplayDataHandlerLoad()
	{
	
	}
	

	// ------------------------------------------------------------
	int GetMissionState()
	{
		return m_MissionState;
	}
	
	// ------------------------------------------------------------
	void SetMissionState( int state )
	{
		m_MissionState = state;
	}
	
	// ------------------------------------------------------------
	bool GetProfileOption(EDayZProfilesOptions option)
	{
		return m_DayZProfileOptions.GetProfileOption(option);
	}
	
	bool GetProfileOptionDefault(EDayZProfilesOptions option)
	{
		return m_DayZProfileOptions.GetProfileOptionDefault(option);
	}
	
	void SetProfileOption(EDayZProfilesOptions option, bool value)
	{
		m_DayZProfileOptions.SetProfileOption(option, value);
	}
	
	map<EDayZProfilesOptions, ref DayZProfilesOption> GetProfileOptionMap()
	{
		return m_DayZProfileOptions.GetProfileOptionMap();
	}
	
	bool IsStressTest()
	{
		return m_IsStressTest;
	}
	
	bool IsAimLogEnabled()
	{
		return m_AimLoggingEnabled;
	}
	
	void SetGameState( DayZGameState state )
	{
		m_GameState = state;
	}
	
	DayZGameState GetGameState()
	{
		return m_GameState;
	}
	
	void SetLoadState( DayZLoadState state )
	{
		m_LoadState = state;
	}
	
	DayZLoadState GetLoadState()
	{
		return m_LoadState;
	}
	
	static bool ReportModded()
	{
		return m_ReportModded;
	}
	
	Backlit GetBacklit()
	{
		return m_Backlit;
	}
	
	// ------------------------------------------------------------
	override bool IsInventoryOpen()
	{
#ifndef NO_GUI
		if (GetUIManager().FindMenu(MENU_INVENTORY) != NULL)
		{
			return true;
		}
#endif
		return false;
	}
	
	// ------------------------------------------------------------
	void EarlyAccessDialog(UIScriptedMenu parent)
	{
		if (!m_early_access_dialog_accepted)
		{
			g_Game.GetUIManager().EnterScriptedMenu(MENU_EARLYACCESS, parent);
			m_early_access_dialog_accepted = true;
		}
	}
	
	// ------------------------------------------------------------
	//! create custom main menu part (submenu)
	override UIScriptedMenu CreateScriptedMenu(int id)
	{
	#ifndef NO_GUI
		Mission mission = GetMission();
		if (mission)
		{
			return mission.CreateScriptedMenu(id);
		}
	#endif
		return NULL;
	}
		
	// ------------------------------------------------------------
	void ReloadMission()
	{
	#ifdef DEVELOPER
		Print("Reloading mission module!");
		CreateMission(m_MissionPath);
	#endif
	}
	
		// ------------------------------------------------------------
	void CancelLoginQueue()
	{
		if (m_LoginQueue)
		{
			if (m_LoginQueue.IsStatic())
			{
				m_LoginQueue.Hide();
				delete m_LoginQueue;
			}
			else
			{
				m_LoginQueue.Close();
			}
		}
	}
	// ------------------------------------------------------------
	void CancelLoginTimeCountdown()
	{
		GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.LoginTimeCountdown);
		
		if (m_LoginTimeScreen)
		{
			if (m_LoginTimeScreen.IsStatic())
			{
				m_LoginTimeScreen.Hide();
				delete m_LoginTimeScreen;
			}
			else
			{
				m_LoginTimeScreen.Close();
			}
		}
	}
	
	// ------------------------------------------------------------
	override void OnEvent(EventType eventTypeId, Param params)
	{
		string address;
		int port;
		int high, low;
		
		switch (eventTypeId)
		{
			case StartupEventTypeID:
			{
				#ifndef SERVER
				// Just call it, to create the global instance if it doesn't exist yet
				ParticleManager.GetInstance();
				#endif
				break;
			}
			case MPSessionStartEventTypeID:
			{
				m_FirstConnect = true;
				ClientData.ResetClientData();
				break;
			}
			case MPSessionEndEventTypeID:
			{
				LoadingHide();
				CancelLoginTimeCountdown();
				SetConnecting( false );
				m_FirstConnect = true;
				#ifdef PLATFORM_CONSOLE
					if ( GetUserManager().GetSelectedUser() )
					{
						OnlineServices.LeaveGameplaySession();
						OnlineServices.ClearCurrentServerInfo();
						if ( GetGameState() == DayZGameState.IN_GAME )
						{
							SetGameState( DayZGameState.MAIN_MENU );
							SetLoadState( DayZLoadState.MAIN_MENU_START );
						}
					}
					m_Notifications.ClearVoiceNotifications();
				#endif
				
				// analytics - disconnected player
				StatsEventDisconnectedData discData = new StatsEventDisconnectedData();
				discData.m_CharacterId = g_Game.GetDatabaseID();
				discData.m_Reason = "quit";
				Analytics.PlayerDisconnected(discData);
				break;
			}
			case MPSessionFailEventTypeID:
			{
				LoadingHide(true);
				SetConnecting( false );
				ProgressAsync.DestroyAllPendingProgresses();
				
				if ( GetGameState() == DayZGameState.CONNECTING )
				{
					SetGameState( DayZGameState.MAIN_MENU );
				}
				
				break;
			}
			case MPSessionPlayerReadyEventTypeID:
			{
				LoadingHide(true);
				ProgressAsync.DestroyAllPendingProgresses();
				
				SetGameState( DayZGameState.IN_GAME );
	
				// fade out from the black screen
				#ifndef NO_GUI
				if ( GetUIManager().ScreenFadeVisible() )
				{
					GetUIManager().ScreenFadeOut(0.5);
				}
				#endif
				
				// analytics - spawned
				StatsEventSpawnedData spawnData = new StatsEventSpawnedData();
				spawnData.m_CharacterId = g_Game.GetDatabaseID();
				spawnData.m_Lifetime = 0;
				spawnData.m_Position = vector.Zero;
				if ( GetPlayer() )
				{
					spawnData.m_Position = GetPlayer().GetPosition();
				}
				spawnData.m_DaytimeHour = 0;
				spawnData.m_Population = 0;
				Analytics.PlayerSpawned(spawnData);
				
				#ifdef PLATFORM_CONSOLE
				m_Notifications.ClearVoiceNotifications();
				OnlineServices.SetMultiplayState( true );
				#endif
				if ( m_FirstConnect )
				{
					m_FirstConnect = false;
					if ( GetHostAddress( address, port ) )
					{
						AddVisitedServer( address, port );
					}
					
					#ifdef PLATFORM_CONSOLE
						#ifndef PLATFORM_WINDOWS // if app is not on Windows with -XBOX parameter
							if ( null != GetUserManager().GetSelectedUser() )
							{
								OnlineServices.EnterGameplaySession();
								OnlineServices.LoadVoicePrivilege();
							}
						#endif
					#endif
				}
					
				if ( m_ShouldShowControllerDisconnect && !GetWorld().IsMouseAndKeyboardEnabledOnServer() )
					CreateGamepadDisconnectMenu();
				
				break;
			}
			case MPConnectionLostEventTypeID:
			{
				MPConnectionLostEventParams conLost_params;
				if (Class.CastTo(conLost_params, params))
				{
					int duration = conLost_params.param1;
					OnMPConnectionLostEvent(duration);
				
					#ifdef PLATFORM_PS4
					//PSN Set multiplay state	
					if ( duration < 0 && GetGameState() == DayZGameState.IN_GAME )
					{
						OnlineServices.SetMultiplayState( true );
						//Print("=== PLATFORM_PS4 - SetMultiplayState ACTIVE [MPConnectionLostEventTypeID]");
					}
					else
					{
						OnlineServices.SetMultiplayState( false );
						//Print("=== PLATFORM_PS4 - SetMultiplayState NOT ACTIVE [MPConnectionLostEventTypeID]");
					}
					#endif
				}
				break;
			}
			case WorldCleaupEventTypeID:
			{
				LoadingShow();
				break;
			}
			case DialogQueuedEventTypeID:
			{
				GetCallQueue(CALL_CATEGORY_SYSTEM).Call(g_Game.CheckDialogs);
				break;
			}
			case ChatMessageEventTypeID:
			{
				ChatMessageEventParams chat_params;
				if (Class.CastTo(chat_params, params))
				{
					
				}
				break;
			}
			case ProgressEventTypeID:
			{
				ProgressEventParams prog_params;
				if (Class.CastTo(prog_params, params))
					LoadProgressUpdate(prog_params.param1, prog_params.param2, prog_params.param3);
				break;
			}
			case LoginTimeEventTypeID:
			{
				LoginTimeEventParams loginTimeParams;
				if (Class.CastTo(loginTimeParams, params))
				{
					OnLoginTimeEvent(loginTimeParams.param1);
				}
				break;
			}	
			case RespawnEventTypeID:
			{
				RespawnEventParams respawnParams;
				if (Class.CastTo(respawnParams, params))
				{
					OnRespawnEvent(respawnParams.param1);
				}	
				break;
			}
			case PreloadEventTypeID:
			{
				PreloadEventParams preloadParams;
				if (Class.CastTo(preloadParams, params))
				{
					OnPreloadEvent(preloadParams.param1);
				}	
				break;
			}
			case LogoutEventTypeID:
			{
				LogoutEventParams logoutParams;
				if (Class.CastTo(logoutParams, params))
				{
					GetCallQueue(CALL_CATEGORY_GUI).Call( GetMission().StartLogoutMenu, logoutParams.param1 );						
				}
				break;
			}
			case LoginStatusEventTypeID:
			{
				LoginStatusEventParams loginStatusParams;
				Class.CastTo(loginStatusParams, params);
	
				string msg1 = loginStatusParams.param1;
				string msg2 = loginStatusParams.param2;
				string finalMsg;
					
				// write either to login time screen or loading screen
				if (m_LoginTimeScreen)
				{
					finalMsg = msg1;
					// login time screen supports two lines
					if (msg2.Length() > 0)
						finalMsg += "\n" + msg2;
						
					m_LoginTimeScreen.SetStatus(finalMsg);
				}
				else if (m_loading)
				{
					// loading only one line, but it's a long one
					finalMsg = msg1 + " " + msg2;
					m_loading.SetStatus(finalMsg);
				}
				break;
			}
			case ConnectingAbortEventTypeID:
			{
				g_Game.SetGameState(DayZGameState.MAIN_MENU);
				SetConnecting(false);
				if (m_ConnectFromJoin)
				{
					m_ConnectFromJoin = false;
					AbortMission();
				}
				break;
			}
			case DLCOwnerShipFailedEventTypeID:
			{
				DLCOwnerShipFailedParams dlcParams;
				if (Class.CastTo(dlcParams, params))
				{
					Print( "### DLC Ownership failed !!! Map: " + dlcParams.param1 );
				}
				break;
			}
		}
		
		VONManager.GetInstance().OnEvent(eventTypeId, params);

		Mission mission = GetMission();
		if (mission)
		{
			mission.OnEvent(eventTypeId, params);
		}
		
		ErrorModuleHandler emh = ErrorModuleHandler.GetInstance();
		if (emh)
			emh.OnEvent(eventTypeId, params);
	}
	
	void AddVoiceNotification(VONStopSpeakingEventParams vonStartParams)
	{
		m_Notifications.AddVoiceNotification(vonStartParams.param2, vonStartParams.param1);
	}
	
	void RemoveVoiceNotification(VONStopSpeakingEventParams vonStopParams)
	{
		m_Notifications.RemoveVoiceNotification(vonStopParams.param2);
	}
	
	// ------------------------------------------------------------	
	void UpdateLoginQueue(float timeslice)
	{
		int pos = GetUIManager().GetLoginQueuePosition();
		
		//! Display login queue position dialog
		if (!m_LoginQueue && pos > 0)
		{	
			GetUIManager().CloseAll();
			
			if (GetMission())
			{
				UIScriptedMenu parent = GetUIManager().GetMenu();
				EnterLoginQueue(parent);
			}
			else
			{
				m_LoginQueue = new LoginQueueStatic();
				GetUIManager().ShowScriptedMenu(m_LoginQueue, null);
			}
		}
		if (m_LoginQueue)
		{
			m_LoginQueue.SetPosition(pos);

			//! manually update static login queue dialog
			LoginQueueStatic loginQueue;
			if (LoginQueueBase.CastTo(loginQueue, m_LoginQueue))
			{
				loginQueue.Update(timeslice);
			}
		}
	}
	
	// ------------------------------------------------------------
	void OnLoginTimeEvent(int loginTime)
	{
#ifndef NO_GUI
		// remove login queue if exits	
		CancelLoginQueue();	
		
		GetUserManager().GetUserDatabaseIdAsync();
		
		m_LoginTime = loginTime;
		
		// timer for login
		if (m_LoginTime > 0)
		{	
			if (!m_LoginTimeScreen)
			{
				GetUIManager().CloseAll();
				
				if (GetMission())
				{
					UIScriptedMenu parent = GetUIManager().GetMenu();
					EnterLoginTime(parent);
				}
				else
				{
					m_LoginTimeScreen = new LoginTimeStatic();
					GetUIManager().ShowScriptedMenu(m_LoginTimeScreen, null);
				}
			}
			
			m_LoginTimeScreen.SetTime(m_LoginTime);
			m_LoginTimeScreen.Show();
			
			GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.LoginTimeCountdown, 1000, true);
		}
#endif
	}
	
	// ------------------------------------------------------------
	void LoginTimeCountdown()
	{
		// countdown on the login screen
		if (m_LoginTimeScreen)
		{
			if (m_LoginTime > 0)
			{
				m_LoginTimeScreen.SetTime(m_LoginTime);
				m_LoginTime--;
			}
			else
			{
				// stop the call loop
				CancelLoginTimeCountdown();
			}
		}
	}
	
	// ------------------------------------------------------------
	void OnRespawnEvent(int time)
	{	
		// use login time screen for respawn timer
		if (time > 0)
		{	
			m_LoginTime = time;
			if (!m_LoginTimeScreen)
			{
				GetUIManager().CloseAll();
				
				UIScriptedMenu parent = GetUIManager().GetMenu();
				EnterLoginTime(parent);
			}
			
			m_LoginTimeScreen.SetRespawn(true);
			m_LoginTimeScreen.SetTime(m_LoginTime);
			m_LoginTimeScreen.Show();
			
			GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.LoginTimeCountdown, 1000, true);
		}
		if (GetPlayer())
			GetPlayer().StopDeathDarkeningEffect();
		PPERequesterBank.GetRequester(PPERequester_DeathDarkening).Start(new Param1<float>(1.0));
	}
	
	// ------------------------------------------------------------
	void OnPreloadEvent(vector pos)
	{
		// cancel only login time (respawn time is parallel with preload, but login time is not)
		if (m_LoginTimeScreen && !m_LoginTimeScreen.IsRespawn())
			CancelLoginTimeCountdown();
		
		// tell game to continue
		StoreLoginDataPrepare();
	}
		
	// ------------------------------------------------------------
	// Serialize and send default character information to server (must be called)
	void StoreLoginDataPrepare()
	{
		ScriptReadWriteContext ctx = new ScriptReadWriteContext;
		//Print("StoreLoginDataPrepare");
		
		//GetMenuData().RequestGetDefaultCharacterData();
		GetMenuDefaultCharacterData().SerializeCharacterData(ctx.GetWriteContext());
		StoreLoginData(ctx.GetWriteContext());
	}
	
	// ------------------------------------------------------------
	void EnterLoginQueue(UIMenuPanel parent)
	{
		m_LoginQueue = LoginQueueBase.Cast( GetUIManager().EnterScriptedMenu(MENU_LOGIN_QUEUE, parent) ); 		
	}
	
	// ------------------------------------------------------------
	void EnterLoginTime(UIMenuPanel parent)
	{			 	
		m_LoginTimeScreen = LoginTimeBase.Cast( GetUIManager().EnterScriptedMenu(MENU_LOGIN_TIME, parent) ); 		
	}
	
	// ------------------------------------------------------------
	void OnMPConnectionLostEvent(int duration)
	{
#ifndef NO_GUI
		if( duration < 0 )
		{
			if( m_connectionLost )
				m_connectionLost.Hide();
			
			return;
		}
		
		// initialize new message
		if (!m_connectionLost )
		{
			m_connectionLost = new ConnectionLost(this);
		}
		
		if (duration >= 5)
		{
			// show message
			m_connectionLost.Show();
			
			// update message
			if (m_connectionLost.GetDuration() != duration)
			{
				// use STR_MP_NO_MESSAGE 
				m_connectionLost.SetText("#dayz_game_no_message" + " %1 " + duration.ToString() + " " + "#dayz_game_seconds");	
				m_connectionLost.SetDuration(duration);
			}
		}
		else
		{
			// hide unnecessary message
			m_connectionLost.Hide();	
		}
#endif
	}
	
	// ------------------------------------------------------------
	void LoadProgressUpdate(int progressState, float progress, string title)
	{
	#ifndef NO_GUI
		switch(progressState)
		{
		case PROGRESS_START:
			{
			#ifndef NO_GUI	
				// get out of the black screen immediately
				GetUIManager().ScreenFadeOut(0);
			#endif
				m_loading.Inc();
				m_loading.SetTitle(title);
			}
			break;
	
		case PROGRESS_FINISH:
			{
				m_loading.Dec();
			}
			break;
	
		case PROGRESS_PROGRESS:
			{
				m_loading.SetProgress(progress);
			}
			break;
	
		case PROGRESS_UPDATE:
			{
				m_loading.SetProgress(0);
			}
			break;
		}
	#endif
	}
	
	// ------------------------------------------------------------
	override void OnAfterCreate()
	{
		Math.Randomize( -1 );
	}
	
	// ------------------------------------------------------------
	override void OnActivateMessage()
	{
	}

	// ------------------------------------------------------------
	override void OnDeactivateMessage()
	{
	}		
	
	// ------------------------------------------------------------
	override bool OnInitialize()
	{
		ParticleList.PreloadParticles();
		
		InitNotifications();
		m_Visited = new TStringArray;
		GetProfileStringList( "SB_Visited", m_Visited );
		
		if ( GetLoadState() == DayZLoadState.UNDEFINED )
		{
			string param;
			
			if ( GetCLIParam("join", param) )
			{
				JoinLaunch();
				#ifndef PLATFORM_PS4
				CreateTitleScreen();
				#endif
			}
			else if ( GetCLIParam("connect", param) )
			{
				ConnectLaunch();
			}
			else if ( GetCLIParam("mission", param) )
			{
				MissionLaunch();
			}
			else if ( GetCLIParam("party", param) )
			{
				PartyLaunch();
			}
			else
			{
				MainMenuLaunch();
			}
			
			return true;
		}
		
		return false;
	}
	
	void InitNotifications()
	{
		new NotificationSystem;
		m_Notifications = new NotificationUI;
	}
	
	protected ref Widget		m_IntroMenu;
	protected ref Widget		m_GamepadDisconnectMenu;
	protected int				m_PrevBlur;
	
	protected string			m_DatabaseID;
	
	protected string			m_ConnectAddress;
	protected int				m_ConnectPort;
	protected string			m_ConnectPassword;
	
	protected const int			MAX_VISITED = 50;
	protected ref TStringArray	m_Visited;
	
	string GetDatabaseID()
	{
		return m_DatabaseID;
	}
	
	void SetDatabaseID( string id )
	{
		m_DatabaseID = id;
		if( GetUIManager().GetMenu() )
		{
			GetUIManager().GetMenu().Refresh();
		}
	}
	
	void CreateTitleScreen()
	{
		DeleteTitleScreen();
		m_IntroMenu = GetWorkspace().CreateWidgets("gui/layouts/xbox/day_z_title_screen.layout");
		RichTextWidget text_widget = RichTextWidget.Cast( m_IntroMenu.FindAnyWidget("InputPromptText") );
		m_IntroMenu.FindAnyWidget("notification_root").Show(false);
		if (text_widget)
		{
			string text = Widget.TranslateString( "#console_start_game" );
			#ifdef PLATFORM_XBOX
				BiosUserManager user_manager = GetGame().GetUserManager();
				if( user_manager && user_manager.GetSelectedUser() )
					text_widget.SetText( string.Format( text, "<image set=\"xbox_buttons\" name=\"A\" />" ) );
				else
					text_widget.SetText( string.Format( text, "<image set=\"xbox_buttons\" name=\"A\" />" ) );
			#endif
					
			#ifdef PLATFORM_PS4
				string confirm = "cross";
				if( GetGame().GetInput().GetEnterButton() == GamepadButton.A )
				{
					confirm = "cross";
				}
				else
				{
					confirm = "circle";
				}
				text_widget.SetText( string.Format( text, "<image set=\"playstation_buttons\" name=\"" + confirm + "\" />" ) );
			#endif
		}
		
		#ifdef PLATFORM_CONSOLE
		#ifdef PLATFORM_XBOX
		#ifdef BUILD_EXPERIMENTAL
			m_IntroMenu.FindAnyWidget("notification_root").Show(true);
		#endif
		#endif
		#endif
	}
	
	void DeleteTitleScreen()
	{
		if( m_IntroMenu )
		{
			delete m_IntroMenu;
		}
	}
	
	bool ShouldShowControllerDisconnect()
	{
		return m_ShouldShowControllerDisconnect;
	}
	
	void CreateGamepadDisconnectMenu()
	{
		if (m_GamepadDisconnectMenu)
		{
			//DeleteGamepadDisconnectMenu();
			m_ShouldShowControllerDisconnect = false;
			return;
		}
		
		if ((m_GameState != DayZGameState.IN_GAME && !GetInput().IsEnabledMouseAndKeyboard()) || ( m_GameState == DayZGameState.IN_GAME && !GetWorld().IsMouseAndKeyboardEnabledOnServer() ))
		{
			PPERequesterBank.GetRequester(PPERequester_ControllerDisconnectBlur).Start();
			m_GamepadDisconnectMenu = GetWorkspace().CreateWidgets("gui/layouts/xbox/day_z_gamepad_connect.layout");
			RichTextWidget text_widget = RichTextWidget.Cast( m_GamepadDisconnectMenu.FindAnyWidget("Text") );
			TextWidget caption_widget = TextWidget.Cast( m_GamepadDisconnectMenu.FindAnyWidget("Caption") );
			if (text_widget)
			{
				string text = Widget.TranslateString( "#console_reconect" );
				#ifdef PLATFORM_XBOX
					text_widget.SetText( string.Format( text, "<image set=\"xbox_buttons\" name=\"A\" />" ) );
					caption_widget.SetText("#STR_TitleScreenLayout_Caption0");
				#endif

				#ifdef PLATFORM_PS4
					string confirm = "cross";
					if( GetInput().GetEnterButton() == GamepadButton.A )
					{
						confirm = "cross";
					}
					else
					{
						confirm = "circle";
					}
					text_widget.SetText( string.Format( text, "<image set=\"playstation_buttons\" name=\"" + confirm + "\" />" ) );
					caption_widget.SetText("#ps4_STR_TitleScreenLayout_Caption0");
				#endif
			}
		}

		m_ShouldShowControllerDisconnect = true;
	}
	
	void DeleteGamepadDisconnectMenu()
	{
		PPERequesterBank.GetRequester(PPERequester_ControllerDisconnectBlur).Stop();
		if( m_GamepadDisconnectMenu )
			delete m_GamepadDisconnectMenu;
		if( GetUIManager().IsDialogVisible() )
			GetUIManager().CloseDialog();
		m_ShouldShowControllerDisconnect = false;
	}
	
	void JoinLaunch()
	{
		SetGameState( DayZGameState.JOIN );
		SetLoadState( DayZLoadState.JOIN_START );
		
		#ifdef PLATFORM_CONSOLE
			string join_param;
			if( GetCLIParam("join", join_param) )
			{
				BiosUserManager user_manager = GetUserManager();
				user_manager.ParseJoinAsync( join_param );
			}
		#endif
	}
	
	void ConnectLaunch()
	{
		BiosUserManager user_manager = GetUserManager();
		if ( user_manager.GetTitleInitiator() )
		{
			user_manager.SelectUserEx( user_manager.GetTitleInitiator() );
		}
		
		SetGameState( DayZGameState.CONNECT );
		SetLoadState( DayZLoadState.CONNECT_START );
		
		#ifndef PLATFORM_WINDOWS
			#ifdef PLATFORM_CONSOLE
				CreateTitleScreen();
				GamepadCheck();
			#endif
		#else
			ConnectFromCLI();
		#endif
	}
	
	void PartyLaunch()
	{
		SetGameState( DayZGameState.PARTY );
		SetLoadState( DayZLoadState.PARTY_START );
		BiosUserManager user_manager = GetGame().GetUserManager();
		
		string param;
		if ( GetCLIParam("party", param) )
		{
			user_manager.ParsePartyAsync( param );
			StartRandomCutscene(GetMainMenuWorld());
		}
	}
	
	void MainMenuLaunch()
	{
		SetGameState( DayZGameState.MAIN_MENU );
		SetLoadState( DayZLoadState.MAIN_MENU_START );
		
		//#ifdef PLATFORM_CONSOLE
			//StartRandomCutscene("Staroye");
		//#else
			StartRandomCutscene(GetMainMenuWorld());
		//#endif
		
		DeleteTitleScreen();
	}
	
	void MissionLaunch()
	{
		BiosUserManager user_manager = GetUserManager();
		if ( user_manager )
		{
			if ( user_manager.GetTitleInitiator() )
			{
				user_manager.SelectUserEx( user_manager.GetTitleInitiator() );
			}
		}
		
		SetGameState( DayZGameState.IN_GAME );
		SetLoadState( DayZLoadState.MISSION_START );
		

		#ifndef PLATFORM_WINDOWS
			#ifdef PLATFORM_CONSOLE			
				#ifndef DEVELOPER
					CreateTitleScreen();
					GamepadCheck();
					return;
				#endif
			#endif
		#endif

		string mission;
		GetCLIParam("mission", mission);
		PlayMission(mission);
	}
	
	void SelectUser( int gamepad = -1 )
	{
		BiosUserManager user_manager = GetUserManager();
		if ( user_manager )
		{
			BiosUser selected_user;
			if ( gamepad > -1 )
			{
				GetInput().GetGamepadUser( gamepad, selected_user );
				#ifdef PLATFORM_PS4
				if ( selected_user && selected_user.IsOnline() )
				#endif
				{
					GetGame().GetInput().IdentifyGamepad(GamepadButton.BUTTON_NONE);
					GetInput().SelectActiveGamepad( gamepad );
					user_manager.SelectUserEx( selected_user );
				}
				#ifdef PLATFORM_PS4
				else if ( selected_user )
				{
					user_manager.SelectUserEx( selected_user );
					user_manager.LogOnUserAsync( selected_user ); 
					return;
				}
				else
				{
					GetInput().ResetActiveGamepad();
					GamepadCheck();
				}
				#endif
			}
			
			if ( !selected_user )
				selected_user = user_manager.GetSelectedUser();
			
			switch ( GetLoadState() )
			{
				case DayZLoadState.JOIN_START:
				{
					SetLoadState( DayZLoadState.JOIN_USER_SELECT );
					
					OnlineServices.Init();
					SelectGamepad();
					return;
				}
				case DayZLoadState.PARTY_START:
				{
					SetLoadState( DayZLoadState.PARTY_USER_SELECT );
					
					OnlineServices.Init();
					SelectGamepad();
					return;
				}
				case DayZLoadState.MAIN_MENU_START:
				{
					if ( !selected_user )
					{
						user_manager.PickUserAsync();
						return;
					}
					user_manager.SelectUserEx( selected_user );
					SetLoadState( DayZLoadState.MAIN_MENU_USER_SELECT );
					
					OnlineServices.Init();
					SelectGamepad();
					return;
				}
				case DayZLoadState.CONNECT_START:
				{
					if ( !selected_user )
					{
						user_manager.PickUserAsync();
						return;
					}
					user_manager.SelectUserEx( selected_user );
					SetLoadState( DayZLoadState.CONNECT_USER_SELECT );
					
					OnlineServices.Init();
					SelectGamepad();
					return;
				}
				case DayZLoadState.MISSION_START:
				{
					if ( !selected_user )
					{
						user_manager.PickUserAsync();
						return;
					}
					user_manager.SelectUserEx( selected_user );
					SetLoadState( DayZLoadState.MISSION_USER_SELECT );
					
					OnlineServices.Init();
					SelectGamepad();
					return;
				}
				default:
				{
					/*
					if( !selected_user )
					{
						SetLoadState( DayZLoadState.MAIN_MENU_START );
						user_manager.PickUserAsync();
						return;
					}*/
				}
			}
		}
	}
	
	void SetPreviousGamepad( int gamepad )
	{
		m_PreviousGamepad = gamepad;
	}
	
	int GetPreviousGamepad()
	{
		return m_PreviousGamepad;
	}
	
	void GamepadCheck()
	{
#ifndef AUTOTEST
		if ( GetInput().IsActiveGamepadSelected() )
		{
#endif
			DeleteTitleScreen();
			SelectUser();
#ifndef AUTOTEST
		}
		else
		{
	#ifdef PLATFORM_CONSOLE
		#ifndef PLATFORM_WINDOWS
			#ifdef PLATFORM_PS4
			if ( GetUserManager().GetSelectedUser() )
			{
				int gamepad = GetInput().GetUserGamepad( GetUserManager().GetSelectedUser() );
				if (gamepad > -1)
				{
					SelectUser(gamepad);
				}
				else
				{
					if (!m_IntroMenu && !(GetGame().GetUIManager().GetMenu() && GetGame().GetUIManager().GetMenu().GetID() == MENU_TITLE_SCREEN))
						CreateTitleScreen();
					GetGame().GetInput().IdentifyGamepad(GetGame().GetInput().GetEnterButton());
				}
			}
			else
			#endif
			{
				if ( !m_IntroMenu && !( GetUIManager().GetMenu() && GetUIManager().GetMenu().GetID() == MENU_TITLE_SCREEN ) )
					CreateTitleScreen();
				GetInput().IdentifyGamepad( GetInput().GetEnterButton() );
			}
		#endif
	#endif
		}
#endif
	}
	
	void SelectGamepad()
	{
		ResetProfileOptions();
		BiosUserManager user_manager = GetUserManager();
		
		if ( user_manager )
		{
			BiosUser selected_user = user_manager.GetSelectedUser();
			if ( selected_user )
			{
				OnlineServices.SetBiosUser(selected_user);
				SetPlayerName( selected_user.GetName() );
				SetUserFOV(GetUserFOVFromConfig());
				#ifdef PLATFORM_CONSOLE
				SetPlayerGameName( selected_user.GetName() );
				user_manager.GetUserDatabaseIdAsync();
				#endif
			}
			
			if ( GetUIManager().GetMenu() )
			{
				GetUIManager().GetMenu().Refresh();
			}
		}
		
		switch ( GetLoadState() )
		{
			case DayZLoadState.JOIN_USER_SELECT:
			{
				SetLoadState(DayZLoadState.JOIN_CONTROLLER_SELECT );
				OnlineServices.LoadMPPrivilege();
				break;
			}
			case DayZLoadState.PARTY_USER_SELECT:
			{
				SetLoadState(DayZLoadState.PARTY_CONTROLLER_SELECT);
				DeleteTitleScreen();
				GetUIManager().EnterScriptedMenu(MENU_SERVER_BROWSER, GetUIManager().GetMenu());
				break;
			}
			case DayZLoadState.CONNECT_USER_SELECT:
			{
				SetLoadState(DayZLoadState.CONNECT_CONTROLLER_SELECT );
				OnlineServices.LoadMPPrivilege();
				break;
			}
			case DayZLoadState.MAIN_MENU_USER_SELECT:
			{
				SetLoadState( DayZLoadState.MAIN_MENU_CONTROLLER_SELECT );
				DeleteTitleScreen();
				GetUIManager().EnterScriptedMenu( MENU_MAIN, GetUIManager().GetMenu() );
				break;
			}
			case DayZLoadState.MISSION_USER_SELECT:
			{
				SetLoadState( DayZLoadState.MISSION_CONTROLLER_SELECT );
				DeleteTitleScreen();
				string mission;
				GetCLIParam("mission", mission);
				PlayMission(mission);
				break;
			}
		}
	}
	
	void TryConnect()
	{
		if ( GetGameState() != DayZGameState.CONNECTING )
		{
			switch ( GetLoadState() )
			{
				case DayZLoadState.JOIN_CONTROLLER_SELECT:
				{
					SetGameState( DayZGameState.CONNECTING );
					OnlineServices.GetSession();
					break;
				}
				case DayZLoadState.CONNECT_CONTROLLER_SELECT:
				{
					SetGameState( DayZGameState.CONNECTING );
					ConnectFromCLI();
					break;
				}
				case DayZLoadState.PARTY_CONTROLLER_SELECT:
				{
					SetGameState( DayZGameState.CONNECTING );
					Connect();
					break;
				}
				case DayZLoadState.MAIN_MENU_CONTROLLER_SELECT:
				{
					SetGameState( DayZGameState.CONNECTING );
					Connect();
					break;
				}
			}
		}
		else
		{
			string address;
			int port;
			if (GetHostAddress(address, port))
			{
				if (m_ConnectAddress == address && m_ConnectPort == port)
					ErrorModuleHandler.ThrowError(ErrorCategory.ConnectErrorScript, EConnectErrorScript.ALREADY_CONNECTING_THIS);
				else
					ErrorModuleHandler.ThrowError(ErrorCategory.ConnectErrorScript, EConnectErrorScript.ALREADY_CONNECTING, string.Format("%1:%2", address, port));
			}
			else	
			{
				DisconnectSessionForce();
				DisconnectSessionScript();
				TryConnect();
			}
		}
	}
	
	bool GetLastVisitedServer( out string ip, out int port )
	{
		if( m_Visited )
		{
			if( m_Visited.Count() > 0 )
			{
				string uid = m_Visited.Get( m_Visited.Count() - 1 );
				TStringArray output = new TStringArray;
				uid.Split( ":", output );
				ip = output[0];
				port = output[1].ToInt();
				return true;
			}
		}
		return false;
	}
	
	void AddVisitedServer( string ip, int port )
	{
		string uid = ip + ":" + port;
		if( m_Visited )
		{
			int pos = m_Visited.Find( uid );
			 
			if( pos < 0 )
			{
				if( m_Visited.Count() == MAX_VISITED )
					m_Visited.Remove( 0 );
				m_Visited.Insert( uid );
			}
			else
			{
				// if item is not saved as last server, move it
				if( pos != ( m_Visited.Count() - 1 ))
				{
					m_Visited.Remove( pos );
					m_Visited.Insert( uid );
				}
			}
			SetProfileStringList( "SB_Visited", m_Visited );
			SaveProfile();
		}
	}
	
	bool IsVisited( string ip, int port )
	{
		string uid = ip + ":" + port;
		int index = m_Visited.Find( uid );
		return ( index >= 0 );
	}
	
	void RefreshCurrentServerInfo()
	{
		string addr;
		int port;
		if ( GetHostAddress( addr, port ) )
		{
			m_ConnectAddress = addr;
			m_ConnectPort = port;
		}
		OnlineServices.GetCurrentServerInfo( m_ConnectAddress, m_ConnectPort );
	}

	void Connect()
	{
		SetConnecting(true);
		
		DeleteTitleScreen();
		string addr;
		int port;
		if ( GetHostAddress( addr, port ) )
		{
			if ( m_ConnectAddress == addr && m_ConnectPort == port )
				return;
		}

		if ( Connect( GetUIManager().GetMenu(), m_ConnectAddress, m_ConnectPort, m_ConnectPassword ) != 0 )
			DisconnectSessionScript(true);
	}
	
	void DisconnectSessionScript(bool displayJoinError = false)
	{
		if ( OnlineServices.GetBiosUser() )
			GetGame().GetUserManager().SelectUserEx( OnlineServices.GetBiosUser() );
			
		if ( g_Game.GetGameState() != DayZGameState.IN_GAME )
		{
			if ( GetGame().GetMission() )
			{
				if ( g_Game.GetGameState() != DayZGameState.MAIN_MENU )
				{
				//	GetGame().GetUIManager().CloseAllSubmenus();
					GetGame().GetMission().AbortMission();
						
					if ( displayJoinError && g_Game.GetGameState() == DayZGameState.JOIN )
						NotificationSystem.AddNotification( NotificationType.JOIN_FAIL_GET_SESSION, 6 );
						
					g_Game.SetGameState( DayZGameState.MAIN_MENU );
					g_Game.SetLoadState( DayZLoadState.MAIN_MENU_CONTROLLER_SELECT );
					
					g_Game.GamepadCheck();
					return;
				}
			}
			else
			{
 				g_Game.MainMenuLaunch();
			}
		}
	}
	
	void ConnectFromServerBrowser( string ip, int port, string password = "" )
	{
		m_ConnectAddress	= ip;
		m_ConnectPort		= port;
		m_ConnectPassword	= password;
		m_ConnectFromJoin	= false;
		OnlineServices.LoadMPPrivilege();
	}
	
	void ConnectFromJoin( string ip, int port )
	{
		m_ConnectAddress	= ip;
		m_ConnectPort		= port;
		m_ConnectFromJoin	= true;
		Connect();
	}
	
	void ConnectFromCLI()
	{
		string port;
		if ( GetCLIParam("connect", m_ConnectAddress) )
		{
			GetCLIParam("port", port);
			m_ConnectPort = port.ToInt();
			
			GetCLIParam("password", m_ConnectPassword);
			
			m_ConnectFromJoin = false;
			Connect();
		}
	}
	
	bool IsLeftCtrlDown()
	{
		return m_IsCtrlHolding;
	}
	
	// ------------------------------------------------------------
	override void OnKeyPress(int key)
	{
	
		if ( key == KeyCode.KC_LCONTROL ) 
		{
			m_IsCtrlHolding = true;
		}

		if (key == KeyCode.KC_LMENU)
		{
			m_IsLeftAltHolding = true;
		}
			
		if (key == KeyCode.KC_RMENU)
		{
			m_IsRightAltHolding = true;
		}
	
		if (m_keyboard_handler)
		{
			m_keyboard_handler.OnKeyDown(NULL, 0, 0, key);
		}
		
		Mission mission = GetMission();
		if (mission)
		{
			mission.OnKeyPress(key);
		}
	
#ifdef DEVELOPER
		if ((m_IsLeftAltHolding || m_IsLeftAltHolding) && key == KeyCode.KC_F4)
		{
			RequestExit(0);
		}
#endif
		
	}
	
	// ------------------------------------------------------------
	override void OnKeyRelease(int key)
	{
		if ( key == KeyCode.KC_LCONTROL ) 
		{
			m_IsCtrlHolding = false;
		}
		
		if ( key == KeyCode.KC_LWIN ) 
		{
			m_IsWinHolding = false;
		}

		if (key == KeyCode.KC_LMENU || key == KeyCode.KC_RMENU)
		{
			m_IsLeftAltHolding = false;
		}

		if (key == KeyCode.KC_RMENU)
		{
			m_IsRightAltHolding  = false;
		}

		if (m_keyboard_handler)
		{
			m_keyboard_handler.OnKeyUp(NULL, 0, 0, key);
		}
	
		Mission mission = GetMission();
		if (mission)
		{
			mission.OnKeyRelease(key);
		}
	}
	
	// ------------------------------------------------------------	
	override void OnMouseButtonPress(int button)
	{
		Mission mission = GetMission();
		if (mission)
		{
			mission.OnMouseButtonPress(button);
		}
	}
	
	// ------------------------------------------------------------
	override void OnMouseButtonRelease(int button)
	{
		Mission mission = GetMission();
		if (mission)
		{
			mission.OnMouseButtonRelease(button);
		}
	}
	
	// ------------------------------------------------------------
	override void OnDeviceReset()
	{
		m_IsCtrlHolding = false;
		m_IsWinHolding = false;
		m_IsLeftAltHolding = false;
		m_IsRightAltHolding  = false;
	}
	
	// ------------------------------------------------------------
	override void OnUpdate(bool doSim, float timeslice)
	{
		Mission mission = GetMission();
		bool gameIsRunning = false;
	
		if (doSim && mission && !mission.IsPaused())
		{
			gameIsRunning = true;
		}
	
		if (doSim && mission)
		{
			mission.OnUpdate(timeslice);
		}
	
		// queues and timers update
		GetCallQueue(CALL_CATEGORY_SYSTEM).Tick(timeslice);
		GetUpdateQueue(CALL_CATEGORY_SYSTEM).Invoke(timeslice);
		GetTimerQueue(CALL_CATEGORY_SYSTEM).Tick(timeslice);
	
	#ifndef NO_GUI	
		if (m_IsConnecting)
			UpdateLoginQueue(timeslice);
		
		if (m_loading && m_loading.IsLoading())
		{
			m_loading.OnUpdate(timeslice);
		}
		else if (m_LoginTimeScreen && m_LoginTimeScreen.IsStatic())
		{
			m_LoginTimeScreen.Update(timeslice);
		}
		else
		{
			GetCallQueue(CALL_CATEGORY_GUI).Tick(timeslice);
			GetUpdateQueue(CALL_CATEGORY_GUI).Invoke(timeslice);
			GetTimerQueue(CALL_CATEGORY_GUI).Tick(timeslice);
			GetDragQueue().Tick();
		}
		
		NotificationSystem.Update( timeslice );
		if( m_Notifications )
		{
			m_Notifications.Update( timeslice );
		}
	#endif
	
		if (gameIsRunning)
		{
			GetCallQueue(CALL_CATEGORY_GAMEPLAY).Tick(timeslice);
			GetUpdateQueue(CALL_CATEGORY_GAMEPLAY).Invoke(timeslice);
			GetTimerQueue(CALL_CATEGORY_GAMEPLAY).Tick(timeslice);
		}
	}
	
	// ------------------------------------------------------------
	override void OnPostUpdate(bool doSim, float timeslice)
	{
		
	}
	
	// ------------------------------------------------------------
	override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, target, rpc_type, ctx);
		Event_OnRPC.Invoke( sender, target, rpc_type, ctx );
		
		//Print("["+ GetGame().GetTime().ToString() +"] => DayZGame::OnRPC = "+ EnumTools.EnumToString(ERPCs,rpc_type) );
		
		if (target)
		{
			// call rpc on target
			target.OnRPC(sender, rpc_type, ctx);
		}
		else
		{
			switch ( rpc_type )
			{
				#ifndef SERVER
				#ifndef NO_GUI
				case ERPCs.RPC_CFG_GAMEPLAY_SYNC:
				{
					CfgGameplayHandler.OnRPC(null, ctx);
					break;
				}
				case ERPCs.RPC_UNDERGROUND_SYNC:
				{
					UndergroundAreaLoader.OnRPC(ctx);
					break;
				}
				case ERPCs.RPC_SEND_NOTIFICATION:
				{
					NotificationType type;
					float show_time;
					string detail_text;
					
					ctx.Read(type);
					ctx.Read(show_time);
					ctx.Read(detail_text);
					
					NotificationSystem.AddNotification( type, show_time, detail_text );
					break;
				}
				case ERPCs.RPC_SEND_NOTIFICATION_EXTENDED:
				{
					float show_time_ext;
					string title_text_ext;
					string detail_text_ext;
					string icon_ext;
					
					ctx.Read(show_time_ext);
					ctx.Read(title_text_ext);
					ctx.Read(detail_text_ext);
					ctx.Read(icon_ext);
					
					NotificationSystem.AddNotificationExtended( show_time_ext, title_text_ext, detail_text_ext, icon_ext );
					break;
				}
				
				
				case ERPCs.RPC_SOUND_HELICRASH:
				{
					bool playSound;
					vector pos;
					string sound_set;
					
					//Helicrash is a world event, we want anyone to be able to hear it
					Param3<bool, vector, int> playCrashSound = new Param3<bool, vector, int>( false, "0 0 0",0 );
					if ( ctx.Read( playCrashSound ) )
					{
						playSound = playCrashSound.param1;
						pos = playCrashSound.param2;
						sound_set = CrashSoundSets.GetSoundSetByHash(playCrashSound.param3);
					}
					
					if ( playSound )
					{
						m_CrashSound = SEffectManager.PlaySound( sound_set, pos, 0.1, 0.1 );
						m_CrashSound.SetAutodestroy( true );
					}
				
				break;
				}
				//Random off map artillery barrage
				case ERPCs.RPC_SOUND_ARTILLERY:
				{
					vector position;
					Param1<vector> playArtySound = new Param1<vector>( vector.Zero );
					if ( ctx.Read( playArtySound ) )
					{
						position = playArtySound.param1;
						if ( position == vector.Zero )
							break;
					}
					else
						break;
					
					if ( !GetGame().GetPlayer() )
						break;
					
					if ( vector.DistanceSq( GetGame().GetPlayer().GetPosition(), position ) <= ( MIN_ARTY_SOUND_RANGE * MIN_ARTY_SOUND_RANGE ) )
						break;
					
					m_ArtySound = SEffectManager.PlaySound( "Artillery_Distant_Barrage_SoundSet", position, 0.1, 0.1 );
					m_ArtySound.SetAutodestroy( true );
				
				break;
				}
				case ERPCs.RPC_SOUND_CONTAMINATION:
				{
					vector soundPos;
					
					Param1<vector> playContaminatedSound = new Param1<vector>( vector.Zero );
					if ( ctx.Read( playContaminatedSound ) )
					{
						soundPos = playContaminatedSound.param1;
						if ( soundPos == vector.Zero )
							break;
					}
					else
						break;
					
					if ( !GetGame().GetPlayer() )
						break;
					
					EffectSound closeArtySound = SEffectManager.PlaySound( "Artillery_Close_SoundSet", soundPos );
					closeArtySound.SetAutodestroy( true );
					
					// We add camera shake upon shell detonation
					float distance_to_player = vector.DistanceSq( soundPos, GetGame().GetPlayer().GetPosition() );
					if ( distance_to_player <= GameConstants.CAMERA_SHAKE_ARTILLERY_DISTANCE2 )
					{
						float strength_factor = Math.InverseLerp( GameConstants.CAMERA_SHAKE_ARTILLERY_DISTANCE, 0, Math.Sqrt( distance_to_player ) );
						DayZPlayerCamera camera = GetGame().GetPlayer().GetCurrentCamera();
						if ( camera )
							camera.SpawnCameraShake( strength_factor * 4 );
					}
					
					ParticleManager.GetInstance().PlayInWorld( ParticleList.CONTAMINATED_AREA_GAS_SHELL, soundPos );
					break;
				}
				// Single artillery shot to announce dynamic contaminated area
				case ERPCs.RPC_SOUND_ARTILLERY_SINGLE:
				{
					vector soundPosition;
					vector delayedSoundPos;
					float soundDelay;
					
					Param3<vector, vector, float> playArtyShotSound = new Param3<vector, vector, float>( vector.Zero, vector.Zero, 0 );
					if ( ctx.Read( playArtyShotSound ) )
					{
						soundPosition = playArtyShotSound.param1;
						delayedSoundPos = playArtyShotSound.param2;
						soundDelay = playArtyShotSound.param3;
						if ( soundPosition == vector.Zero )
							break;
					}
					else
						break;
					
					if ( !GetGame().GetPlayer() )
						break;
					
					m_ArtySound = SEffectManager.PlaySound( "Artillery_Distant_SoundSet", soundPosition, 0.1, 0.1 );
					m_ArtySound.SetAutodestroy( true );
					
					// We remove the amount of time the incoming sound lasts
					soundDelay -= 5;
					// We convert to milliseconds
					soundDelay *= 1000;
					GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( DelayedMidAirDetonation, soundDelay, false, delayedSoundPos[0], delayedSoundPos[1], delayedSoundPos[2] );
					break;
				}
				#endif
				#endif

				case ERPCs.RPC_USER_SYNC_PERMISSIONS:
				{
					map<string, bool> mute_list;
					if ( ctx.Read( mute_list ) )
					{
						for ( int i = 0; i < mute_list.Count(); i++ )
						{
							string uid = mute_list.GetKey( i );
							bool mute = mute_list.GetElement( i );
							MutePlayer( uid, sender.GetPlainId(), mute );
						}
					}
					break;
				}
				
				/*
				case ERPCs.RPC_SERVER_RESPAWN_MODE:
				{
					int mode;
					if ( ctx.Read(mode) && !IsServer() )
					{
						GetMission().SetRespawnModeClient(mode);
					}
				}
				*/

				#ifdef DEVELOPER
				case ERPCs.DEV_RPC_ITEM_DIAG_BUTTON:
				{
					if (ctx.Read(CachedObjectsParams.PARAM1_INT))
					{
						EntityAI entity = EntityAI.Cast(_item);
						if (entity)
						{
							entity.OnDebugButtonPressServer(CachedObjectsParams.PARAM1_INT.param1);
						}
					}	

					break;
				}
				#ifdef SERVER
				case ERPCs.DEV_CAMERA_TOOLS_CAM_DATA:
				{
					if (!m_CameraToolsMenuServer)
					{
						m_CameraToolsMenuServer = new CameraToolsMenuServer;
					}
					m_CameraToolsMenuServer.OnRPC(rpc_type, ctx);
					break;
				}
				
				case ERPCs.DEV_CAMERA_TOOLS_CAM_SUBSCRIBE:
				{
					if (!m_CameraToolsMenuServer)
					{
						m_CameraToolsMenuServer = new CameraToolsMenuServer;
					}
					m_CameraToolsMenuServer.OnRPC(rpc_type, ctx);
					break;
				}
				#endif
				#endif

			}
			// global rpc's handling
		}
	}
	
	void DelayedMidAirDetonation( float x, float y, float z )
	{
		EffectSound artilleryFallSound = SEffectManager.PlaySound( "Artillery_Fall_SoundSet", Vector( x, y, z ) );
		artilleryFallSound.SetAutodestroy( true );
	}
	
	// ------------------------------------------------------------
	void CheckDialogs()
	{
		#ifndef NO_GUI	
		Mission mission = GetMission();
		if (mission && !m_loading.IsLoading() && GetUIManager().IsDialogQueued())
		{
			mission.Pause();
			GetUIManager().ShowQueuedDialog();
		}
		#endif
	}
	
	//! Returns true when connecting to server
	void SetConnecting(bool value)
	{
		m_IsConnecting = value;
	}
	
	bool IsConnecting()
	{
		return m_IsConnecting;
	}
	
	// ------------------------------------------------------------
	bool IsLoading()
	{
		if (m_loading && m_loading.IsLoading())
		{
			return true;
		}
		
		return false;
	}
	
	// ------------------------------------------------------------
	void SetKeyboardHandle(UIScriptedMenu handler)
	{
		m_keyboard_handler = handler;
	}
	
	// ------------------------------------------------------------
	void LoadingShow()
	{
		#ifndef NO_GUI	
			m_loading.Show();
		#endif
	}
	
	// ------------------------------------------------------------
	void LoadingHide(bool force = false)
	{
		#ifndef NO_GUI
			m_loading.Hide(force);
			// turn the lights back on
			SetEVValue(0);
			#ifdef PLATFORM_CONSOLE
			if( !GetInput().IsActiveGamepadSelected() )
			{
				if( !IsLoading() )
				{
					if( m_LoadState != DayZLoadState.MAIN_MENU_START && m_LoadState != DayZLoadState.MAIN_MENU_USER_SELECT )
					{
						CreateGamepadDisconnectMenu();
					}
					
					#ifdef PLATFORM_XBOX
					GetInput().IdentifyGamepad( GetInput().GetEnterButton() );
					#endif
				}
			}
			#endif
		#endif
	}

	// ------------------------------------------------------------
	override string CreateDefaultPlayer()
	{				
		if ( m_CharClassNames.Count() > 0 )
			return m_CharClassNames[0];
		else
			return "";
	}
	
	// ------------------------------------------------------------
	override string CreateRandomPlayer()
	{
		return m_CharClassNames.GetRandomElement();
	}
	
	// ------------------------------------------------------------
	override TStringArray ListAvailableCharacters()
	{
		return m_CharClassNames;
	}
	
	// ------------------------------------------------------------
	void ExplosionEffectsEx(Object source, Object directHit, int componentIndex, float energyFactor, float explosionFactor, HitInfo hitInfo)
	{
		vector pos = hitInfo.GetPosition();
		string ammoType = hitInfo.GetAmmoType();
		
		// Call legacy method
		ExplosionEffects(source, directHit, componentIndex, hitInfo.GetSurface(), pos, hitInfo.GetSurfaceNormal(), energyFactor, explosionFactor, hitInfo.IsWater(), ammoType);
		
		// add explosion noise
		if ( IsServer() )
		{
			//NoiseParams npar = new NoiseParams();
			m_NoiseParams.LoadFromPath(string.Format("cfgAmmo %1 NoiseExplosion", ammoType));
			
			GetNoiseSystem().AddNoiseTarget(pos, 21, m_NoiseParams, hitInfo.GetSurfaceNoiseMultiplier());
		}
	}
	
	// ------------------------------------------------------------
	void ExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal,
		float energyFactor, float explosionFactor, bool isWater, string ammoType)
	{
		#ifndef SERVER
		if ( source )
		{
			source.OnExplosionEffects(source, directHit, componentIndex, surface, pos, surfNormal, energyFactor, explosionFactor, isWater, ammoType);
			
			if (source && source.ShootsExplosiveAmmo() && ammoType == "Explosion_40mm_Ammo")
			{
				ParticleManager.GetInstance().PlayInWorld(ParticleList.EXPLOSION_LANDMINE, pos);
			}
			
			
			float distance_to_player = vector.Distance(pos, GetGame().GetPlayer().GetPosition());
			m_AmmoShakeParams.Load(ammoType);
			
			if (distance_to_player < m_AmmoShakeParams.m_Radius)
			{
				float dist01 = Math.InverseLerp(0, m_AmmoShakeParams.m_Radius, distance_to_player);
				float modifier = Math.Lerp(m_AmmoShakeParams.m_ModifierClose, m_AmmoShakeParams.m_ModifierFar,dist01);
				
				/*
				Print(">>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<");
				Print(modifier);
				Print(ammoType);
				*/
				
				GetGame().GetPlayer().GetCurrentCamera().SpawnCameraShake(modifier * m_AmmoShakeParams.m_Strength);
			}
		}
		#endif
		/*
		Print("ExplosionEffects");
		Print("##################################");
		Print(directHit);
		Print(componentIndex);
		Print(surface);
		Print(pos);
		Print(surfNormal);
		Print(energyFactor);
		Print(explosionFactor);
		Print(isWater);
		Print(ammoType);
		*/
	}
	
	// ------------------------------------------------------------
	void FirearmEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal,
		 vector exitPos, vector inSpeed, vector outSpeed, bool isWater, bool deflected, string ammoType) 
	{
		#ifndef SERVER
		// if local player was hit
		Object player = GetPlayer();
		if (directHit && player && directHit == player)
		{
			player.OnPlayerRecievedHit();
			float shake_strength = Math.InverseLerp(0, 500, inSpeed.Length());
			//Print(shake_strenght);
			//GetGame().GetPlayer().GetCurrentCamera().SpawnCameraShake(shake_strength,3,5,30);
			GetGame().GetPlayer().GetCurrentCamera().SpawnCameraShake(shake_strength);
		}
		//Print("Distance: " + vector.Distance(source.GetPosition(),pos));
		ImpactMaterials.EvaluateImpactEffect(directHit, componentIndex, surface, pos, ImpactTypes.UNKNOWN, surfNormal, exitPos, inSpeed, outSpeed, deflected, ammoType, isWater);
		#endif
		
		
		if ( IsServer() )
		{
			if (source && source.ShootsExplosiveAmmo() && !deflected && outSpeed == vector.Zero)
			{
				if (ammoType == "Bullet_40mm_ChemGas")
				{
					GetGame().CreateObject("ContaminatedArea_Local", pos);
				}
				else if (ammoType == "Bullet_40mm_Explosive")
				{
					DamageSystem.ExplosionDamage(EntityAI.Cast(source), null, "Explosion_40mm_Ammo", pos, DamageType.EXPLOSION);
				}
			}
			
			
			// add hit noise
			//NoiseParams npar = new NoiseParams();
			m_NoiseParams.LoadFromPath("cfgAmmo " + ammoType + " NoiseHit");
			
			float surfaceCoef = SurfaceGetNoiseMultiplier(directHit, pos, componentIndex);
			float coefAdjusted = surfaceCoef * inSpeed.Length() / ConfigGetFloat("cfgAmmo " + ammoType + " initSpeed");
			GetNoiseSystem().AddNoiseTarget(pos, 10, m_NoiseParams, coefAdjusted); // Leave a ping for 5 seconds
			
			//float noiseCfg = ConfigGetFloat("cfgAmmo " + ammoType + " NoiseHit strength");
			//Print("noiseCfg: " + noiseCfg + "| surface: " + surface + "| surfaceCoef: " + surfaceCoef + "| coefAdjusted: " + coefAdjusted + "| total noise generated: " + (coefAdjusted * noiseCfg * 1.7) );
		}
	}
	
	// ------------------------------------------------------------
	void CloseCombatEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal,
		 bool isWater, string ammoType) 
	{
		#ifndef SERVER
		// if local player was hit
		Object player = GetPlayer();
		if (directHit && player && directHit == player)
		{
			player.OnPlayerRecievedHit();
		}
		
		ImpactMaterials.EvaluateImpactEffect(directHit, componentIndex, surface, pos, ImpactTypes.MELEE, Vector(Math.RandomFloat(-1,1), Math.RandomFloat(-1,1), Math.RandomFloat(-1,1)), "0 0 0", "0 0 0", "0 0 0", false, ammoType, isWater);
		#endif
		
		// add hit noise
		if ( IsServer() )
		{
			//NoiseParams npar = new NoiseParams();
			m_NoiseParams.LoadFromPath("cfgAmmo " + ammoType + " NoiseHit");
			
			float surfaceCoef = SurfaceGetNoiseMultiplier(directHit, pos, componentIndex);
			GetNoiseSystem().AddNoisePos(EntityAI.Cast(source), pos, m_NoiseParams, surfaceCoef);
		}
	}
	
	void UpdateVoiceLevel(int level)
	{
		GetMission().UpdateVoiceLevelWidgets(level);	
	}
	
	void InitCharacterMenuDataInfo(int menudata_count)
	{
		m_OriginalCharactersCount = menudata_count;
	}
	
	void SetPlayerGameName(string name)
	{
		m_PlayerName = name;
	}
	
	string GetPlayerGameName()
	{
		return m_PlayerName;
	}
	
	void SetNewCharacter(bool state)
	{
		m_IsNewCharacter = state;
	}
	
	bool IsNewCharacter()
	{
		return m_IsNewCharacter;
	}
	
	void SetUserFOV(float pFov)
	{
		if ( pFov < OPTIONS_FIELD_OF_VIEW_MIN ) pFov = OPTIONS_FIELD_OF_VIEW_MIN;
		if ( pFov > OPTIONS_FIELD_OF_VIEW_MAX ) pFov = OPTIONS_FIELD_OF_VIEW_MAX;
		
		m_UserFOV = pFov;
	}
	
	float GetUserFOV()
	{
		return m_UserFOV;
	}
	
	static float GetUserFOVFromConfig()
	{
		GameOptions gameOptions = new GameOptions;
		NumericOptionsAccess noa;
		if ( gameOptions && Class.CastTo(noa,gameOptions.GetOptionByType( OptionAccessType.AT_OPTIONS_FIELD_OF_VIEW )) )
		{
			return noa.ReadValue();
		}
		return 1.0;
	}
	
	// Check if ammo is compatible with a weapon in hands 
	static bool CheckAmmoCompability(EntityAI weaponInHand, EntityAI ammo)
	{	
		TStringArray ammo_names = new TStringArray;										// Array of ammo types (their name) that can be used with weapon in hand
		
		string cfg_path = "CfgWeapons " + weaponInHand.GetType() + " chamberableFrom";	// Create config path 
		GetGame().ConfigGetTextArray(cfg_path, ammo_names);								// Get ammo types 
			
		foreach (string ammo_name : ammo_names)				// for every ammo in ammo string compare passed ammo 
		{
			if(ammo.GetType() == ammo_name)					
			{
				 return true;			
			}
		}
		
		// if no ammo from the array matches with ammo passed, return false 
		return false;
	}
	
	void SetEVValue(float value)
	{
		m_PreviousEVValue = m_EVValue;
		SetEVUser(value);
		m_EVValue = value;
	}
	
	float GetCurrentEVValue()
	{
		return m_EVValue;
	}
	
	float GetPreviousEVValue()
	{
		return m_PreviousEVValue;
	}
	
	int GetCurrentDisplayLanguageIdx()
	{
		ListOptionsAccess	language_option;
		GameOptions options = new GameOptions();
		language_option = ListOptionsAccess.Cast( options.GetOptionByType( OptionAccessType.AT_OPTIONS_LANGUAGE ) );
		int idx = -1;
		if (language_option)
		{
			idx = language_option.GetIndex();
		}
		
		return idx;
	}
	
	bool IsWorldWetTempUpdateEnabled()
	{
		return m_IsWorldWetTempUpdateEnabled;
	}
	
	bool IsFoodDecayEnabled()
	{
		return (GetFoodDecayModifier() != 0);
	}	
	
	float GetFoodDecayModifier()
	{
		return m_FoodDecayModifier;
	}
};


DayZGame g_Game;

DayZGame GetDayZGame()
{
	return g_Game;
}