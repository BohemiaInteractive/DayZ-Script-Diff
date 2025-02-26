enum DisconnectSessionFlags
{
	NONE = 0,
	JOIN_ERROR_ENABLED = 1,
	JOIN_ERROR_CHECK = 2,
	DISCONNECT_ERROR_ENABLED = 4,
	SELECT_USER = 8,
	CLOSE_MENUS = 16,
	IGNORE_WHEN_IN_GAME = 32,
	ALWAYS_FORCE = 64,
}

const int DISCONNECT_SESSION_FLAGS_FORCE = int.MAX & ~DisconnectSessionFlags.IGNORE_WHEN_IN_GAME;
const int DISCONNECT_SESSION_FLAGS_JOIN = int.MAX & ~DisconnectSessionFlags.JOIN_ERROR_CHECK;
const int DISCONNECT_SESSION_FLAGS_ALL = int.MAX;

class ProjectileStoppedInfo : Managed
{
	proto native Object GetSource();
	proto native vector GetPos();
	proto native vector GetInVelocity();
	proto native string GetAmmoType();
	proto native float GetProjectileDamage();
}

class CollisionInfoBase: ProjectileStoppedInfo
{
	proto native vector GetSurfNormal();
}

class ObjectCollisionInfo: CollisionInfoBase
{
	proto native Object GetHitObj();
	proto native vector GetHitObjPos();
	proto native vector GetHitObjRot();
	proto native int GetComponentIndex();
}

class TerrainCollisionInfo: CollisionInfoBase
{
	proto native bool GetIsWater();
}

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
		if (m_Mappings)
			m_Mappings.Find(hash,sound_set);
		return sound_set;
	}
};


class LoginScreenBase extends UIScriptedMenu
{
	protected ref UiHintPanelLoading m_HintPanel;
	protected bool m_IsStatic;
	protected float m_HintTimeAccu;
	
	override void Update(float timeslice)
	{
		if (m_HintPanel)
		{
			m_HintTimeAccu += timeslice;
			if (CanChangeHintPage(m_HintTimeAccu))
			{
				m_HintPanel.ShowRandomPage();
				m_HintTimeAccu = 0;
			}
		}
		
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			Leave();
		}
	}
	
	protected void Leave()
	{
		g_Game.SetGameState(DayZGameState.MAIN_MENU);
		g_Game.SetLoadState(DayZLoadState.MAIN_MENU_START);
		
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(GetGame().DisconnectSessionForce);
		
		Close();
	}
	
	protected bool CanChangeHintPage(float timeAccu);
	
	bool IsStatic()
	{
		return m_IsStatic;
	}
	
	override bool IsHandlingPlayerDeathEvent()
	{
		return false;
	}
}

class LoginQueueBase extends LoginScreenBase
{
	protected TextWidget m_txtPosition;
	protected TextWidget m_txtNote;
	protected ButtonWidget m_btnLeave;
	protected int m_iPosition = -1;
	
	void LoginQueueBase()
	{		
		g_Game.SetKeyboardHandle(this);	
	}

	void ~LoginQueueBase()
	{
		g_Game.SetKeyboardHandle(NULL);	
	}
	
	override Widget Init()
	{		
		layoutRoot 		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/dialog_queue_position.layout");
		m_HintPanel	= new UiHintPanelLoading(layoutRoot.FindAnyWidget("hint_frame0"));
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
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		if (w == m_btnLeave)
		{
			Leave();
			return true;
		}
		return false;
	}
	
	void Show()
	{
		if (layoutRoot)
		{
			layoutRoot.Show(true);
		}
	}
	
	void Hide()
	{
		if (layoutRoot)
			layoutRoot.Show(false);
		m_HintPanel = null;
	}
	
	void SetPosition(int position)
	{
		if (position != m_iPosition)
		{
			m_iPosition = position;
			m_txtPosition.SetText(position.ToString());
		}
	}
	
	override protected bool CanChangeHintPage(float timeAccu)
	{
		return timeAccu >= GameConstants.LOADING_SCREEN_HINT_INTERVAL;
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
};

class LoginTimeBase extends LoginScreenBase
{
	protected TextWidget m_txtDescription;
	protected TextWidget m_txtLabel;
	protected ButtonWidget m_btnLeave;
	
	protected bool m_IsRespawn;
	
	private ref FullTimeData m_FullTime;
	
	void LoginTimeBase()
	{
		g_Game.SetKeyboardHandle(this);
		m_IsRespawn = false;
		
		m_FullTime = new FullTimeData();
	}

	void ~LoginTimeBase()
	{
		if (g_Game)
			g_Game.SetKeyboardHandle(null);
		m_FullTime = null;
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
		
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		if (w == m_btnLeave)
		{
			Leave();
			return true;
		}
		return false;
	}
		
	void Show()
	{
		if (layoutRoot)
		{
			layoutRoot.Show(true);
			m_HintPanel	= new UiHintPanelLoading(layoutRoot.FindAnyWidget("hint_frame0"));
		}
	}
	
	void Hide()
	{
		if (layoutRoot)
			layoutRoot.Show(false);
		m_HintPanel = null;
	}
	
	void SetTime(int time)
	{
		string text = "";
		TimeConversions.ConvertSecondsToFullTime(time, m_FullTime);
		if (!m_IsRespawn)
			text = "#menu_loading_in_";
		else
			text = "#dayz_game_spawning_in_";
		
		if (m_FullTime.m_Days > 0)
			text += "dhms";
		else if (m_FullTime.m_Hours > 0)
			text += "hms";
		else if (m_FullTime.m_Minutes > 0)
			text += "ms";
		else
			text += "s";
		
		text = Widget.TranslateString(text);
		text = string.Format(text, m_FullTime.m_Seconds, m_FullTime.m_Minutes, m_FullTime.m_Hours, m_FullTime.m_Days);
		m_txtLabel.SetText(text);
		
		if (m_IsRespawn && time <= 1)
			GetGame().SetLoginTimerFinished();
	}
		
	void SetStatus(string status)
	{
		m_txtDescription.SetText(status);
	}
	
	void SetRespawn(bool value)
	{
		m_IsRespawn = value;
	}
	
	bool IsRespawn()
	{
		return m_IsRespawn;
	}
	
	override protected bool CanChangeHintPage(float timeAccu)
	{
		return timeAccu >= GameConstants.LOADING_SCREEN_HINT_INTERVAL && m_FullTime.m_Seconds >= GameConstants.LOADING_SCREEN_HINT_INTERVAL_MIN;
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
		if (!m_WidgetRoot.IsVisible())
		{
			if (g_Game.GetUIManager().IsDialogVisible())
			{
				g_Game.GetUIManager().HideDialog();
			}
			
			m_WidgetRoot.Show(true);
			m_TextWidgetTitle.SetText("");
		}
	}
	
	void Hide()
	{
		if (m_WidgetRoot.IsVisible())
		{
			m_WidgetRoot.Show(false);
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
typedef DayZProfilesOption DayZProfilesOptionBool;
typedef Param3<string, int, int> DayZProfilesOptionInt;
typedef Param3<string, float, float> DayZProfilesOptionFloat;

class DayZProfilesOptions
{
	private ref map<EDayZProfilesOptions, ref DayZProfilesOptionBool> m_DayZProfilesOptionsBool;
	private ref map<EDayZProfilesOptions, ref DayZProfilesOptionInt> m_DayZProfilesOptionsInt;
	private ref map<EDayZProfilesOptions, ref DayZProfilesOptionFloat> m_DayZProfilesOptionsFloat;
	
	static ref ScriptInvoker m_OnIntOptionChanged 	= new ScriptInvoker();
	static ref ScriptInvoker m_OnBoolOptionChanged 	= new ScriptInvoker();
	static ref ScriptInvoker m_OnFloatOptionChanged = new ScriptInvoker();
	
	void DayZProfilesOptions()
	{
		m_DayZProfilesOptionsBool 	= new map<EDayZProfilesOptions, ref DayZProfilesOptionBool>();
		m_DayZProfilesOptionsInt 	= new map<EDayZProfilesOptions, ref DayZProfilesOptionInt>();
		m_DayZProfilesOptionsFloat 	= new map<EDayZProfilesOptions, ref DayZProfilesOptionFloat>();
	}

	void RegisterProfileOption(EDayZProfilesOptions option, string profileOptionName, bool def = true)
	{
		if (!m_DayZProfilesOptionsBool.Contains(option))
		{
			//! init of DayZProfilesOption - profileOptionName, value from Profiles files, or use default value
			bool profileVal = GetProfileValueBool(profileOptionName, def);

			m_DayZProfilesOptionsBool.Set(option, new DayZProfilesOptionBool(profileOptionName, profileVal, def));
			SetProfileOptionBool(option, profileVal);
		}
	}
	
	void RegisterProfileOptionBool(EDayZProfilesOptions option, string profileOptionName, bool defaultValue = true)
	{
		RegisterProfileOption(option, profileOptionName, defaultValue);
	}
	
	void RegisterProfileOptionInt(EDayZProfilesOptions option, string profileOptionName, int defaultValue = 0)
	{
		if (!m_DayZProfilesOptionsInt.Contains(option))
		{
			//! init of DayZProfilesOption - profileOptionName, value from Profiles files, or use default value
			string outValue;
			GetGame().GetProfileString(profileOptionName, outValue);
			int value = outValue.ToInt();

			m_DayZProfilesOptionsInt.Set(option, new DayZProfilesOptionInt(profileOptionName, value, defaultValue));
			SetProfileOptionInt(option, value);
		}
	}
	
	void RegisterProfileOptionFloat(EDayZProfilesOptions option, string profileOptionName, float defaultValue = 0.0)
	{
		if (!m_DayZProfilesOptionsFloat.Contains(option))
		{
			//! init of DayZProfilesOption - profileOptionName, value from Profiles files, or use default value
			string outValue;
			GetGame().GetProfileString(profileOptionName, outValue);
			float value = outValue.ToFloat();

			m_DayZProfilesOptionsFloat.Set(option, new DayZProfilesOptionFloat(profileOptionName, value, defaultValue));
			SetProfileOptionFloat(option, value);
		}
	}

	void ResetOptionsBool()
	{
		if (!m_DayZProfilesOptionsBool)
		{
			m_DayZProfilesOptionsBool = new map<EDayZProfilesOptions, ref DayZProfilesOptionBool>();
		}

		foreach (EDayZProfilesOptions e_opt, DayZProfilesOptionBool r_opt : m_DayZProfilesOptionsBool)
		{
			bool profileVal = GetProfileValueBool(r_opt.param1, r_opt.param3);
			SetProfileOptionBool(e_opt, profileVal);
		}	
	}
	
	void ResetOptions()
	{
		ResetOptionsBool();
	}
	
	void ResetOptionsInt()
	{
		if (!m_DayZProfilesOptionsInt)
		{
			m_DayZProfilesOptionsInt = new map<EDayZProfilesOptions, ref DayZProfilesOptionInt>();
		}

		foreach (EDayZProfilesOptions e_opt, DayZProfilesOptionInt r_opt : m_DayZProfilesOptionsInt)
		{
			string outValue;
			GetGame().GetProfileString(r_opt.param1, outValue);
			int value = outValue.ToInt();
			SetProfileOptionInt(e_opt, value);
		}	
	}
	
	void ResetOptionsFloat()
	{
		if (!m_DayZProfilesOptionsFloat)
		{
			m_DayZProfilesOptionsFloat = new map<EDayZProfilesOptions, ref DayZProfilesOptionFloat>();
		}

		foreach (EDayZProfilesOptions e_opt, DayZProfilesOptionFloat r_opt : m_DayZProfilesOptionsFloat)
		{
			string outValue;
			GetGame().GetProfileString(r_opt.param1, outValue);
			float value = outValue.ToFloat();
			SetProfileOptionFloat(e_opt, value);
		}	
	}
	
	void SetProfileOption(EDayZProfilesOptions option, bool value)
	{
		if (m_DayZProfilesOptionsBool && m_DayZProfilesOptionsBool.Contains(option))
		{
			DayZProfilesOptionBool po = m_DayZProfilesOptionsBool.Get(option);
			
			po.param2 = value;
			GetGame().SetProfileString(po.param1, value.ToString());
			GetGame().SaveProfile();
			
			m_OnBoolOptionChanged.Invoke(po.param1, value);
		}	
	}

	void SetProfileOptionBool(EDayZProfilesOptions option, bool value)
	{
		SetProfileOption(option, value);
	}
	
	void SetProfileOptionInt(EDayZProfilesOptions option, int value)
	{
		if (m_DayZProfilesOptionsInt && m_DayZProfilesOptionsInt.Contains(option))
		{
			DayZProfilesOptionInt po = m_DayZProfilesOptionsInt.Get(option);
			
			po.param2 = value;
			GetGame().SetProfileString(po.param1, value.ToString());
			GetGame().SaveProfile();
			
			m_OnIntOptionChanged.Invoke(option, value);
		}	
	}
	
	void SetProfileOptionFloat(EDayZProfilesOptions option, float value)
	{
		if (m_DayZProfilesOptionsFloat && m_DayZProfilesOptionsFloat.Contains(option))
		{
			DayZProfilesOptionFloat po = m_DayZProfilesOptionsFloat.Get(option);
			
			po.param2 = value;
			GetGame().SetProfileString(po.param1, value.ToString());
			GetGame().SaveProfile();
			
			m_OnFloatOptionChanged.Invoke(po.param1, value);
		}	
	}

	bool GetProfileOption(EDayZProfilesOptions option)
	{
		if (m_DayZProfilesOptionsBool && m_DayZProfilesOptionsBool.Contains(option))
		{
			DayZProfilesOptionBool po = m_DayZProfilesOptionsBool.Get(option);
			return po.param2;
		}
		
		return true;
	}
	
	bool GetProfileOptionBool(EDayZProfilesOptions option)
	{
		return GetProfileOption(option);
	}
	
	int GetProfileOptionInt(EDayZProfilesOptions option)
	{
		if (m_DayZProfilesOptionsInt && m_DayZProfilesOptionsInt.Contains(option))
		{
			DayZProfilesOptionInt po = m_DayZProfilesOptionsInt.Get(option);
			return po.param2;
		}
		
		return 0;
	}
	
	float GetProfileOptionFloat(EDayZProfilesOptions option)
	{
		if (m_DayZProfilesOptionsFloat && m_DayZProfilesOptionsFloat.Contains(option))
		{
			DayZProfilesOptionFloat po = m_DayZProfilesOptionsFloat.Get(option);
			return po.param2;
		}
		
		return 0.0;
	}
	
	bool GetProfileOptionDefault(EDayZProfilesOptions option)
	{
		return GetProfileOptionDefaultBool(option);
	}
	
	bool GetProfileOptionDefaultBool(EDayZProfilesOptions option)
	{
		if (m_DayZProfilesOptionsBool && m_DayZProfilesOptionsBool.Contains(option))
		{
			DayZProfilesOptionBool po = m_DayZProfilesOptionsBool.Get(option);
			return po.param3;
		}
		
		ErrorEx("Invalid profile option id! Returning 'true'.", ErrorExSeverity.WARNING);
		return true;
	}
	
	int GetProfileOptionDefaultInt(EDayZProfilesOptions option)
	{
		if (m_DayZProfilesOptionsInt && m_DayZProfilesOptionsInt.Contains(option))
		{
			DayZProfilesOptionInt po = m_DayZProfilesOptionsInt.Get(option);
			return po.param3;
		}
		
		ErrorEx("Invalid profile option id! Returning '0'.", ErrorExSeverity.WARNING);
		return 0;
	}
	
	float GetProfileOptionDefaultFloat(EDayZProfilesOptions option)
	{
		if (m_DayZProfilesOptionsFloat && m_DayZProfilesOptionsFloat.Contains(option))
		{
			DayZProfilesOptionFloat po = m_DayZProfilesOptionsFloat.Get(option);
			return po.param3;
		}
		
		ErrorEx("Invalid profile option id! Returning '0.0'.", ErrorExSeverity.WARNING);
		return 0.0;
	}
	
	map<EDayZProfilesOptions, ref DayZProfilesOptionBool> GetProfileOptionMap()
	{
		if (m_DayZProfilesOptions)
			return m_DayZProfilesOptions;

		return null;
	}

	//!
	//! DEPRECATED
	private ref map<EDayZProfilesOptions, ref DayZProfilesOption> m_DayZProfilesOptions;
	private DayZGame m_Game;
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

class LoadingScreen
{
	ref Widget m_WidgetRoot;
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
	
	int m_Counter;
	ref Timer m_Timer;
	ref UiHintPanelLoading m_HintPanel;
	void LoadingScreen(DayZGame game)
	{
		m_DayZGame = game;
		
		m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets("gui/layouts/loading.layout");
		Class.CastTo(m_ImageLogoMid, m_WidgetRoot.FindAnyWidget("ImageLogoMid"));
		Class.CastTo(m_ImageLogoCorner, m_WidgetRoot.FindAnyWidget("ImageLogoCorner"));
		
		Class.CastTo(m_TextWidgetTitle, m_WidgetRoot.FindAnyWidget("TextWidget"));
		Class.CastTo(m_TextWidgetStatus, m_WidgetRoot.FindAnyWidget("StatusText"));
		Class.CastTo(m_ImageWidgetBackground, m_WidgetRoot.FindAnyWidget("ImageBackground"));
		Class.CastTo(m_ImageLoadingIcon, m_WidgetRoot.FindAnyWidget("ImageLoadingIcon"));
		Class.CastTo(m_ModdedWarning, m_WidgetRoot.FindAnyWidget("ModdedWarning"));

		m_ImageBackground = ImageWidget.Cast(m_WidgetRoot.FindAnyWidget("ImageBackground"));
		m_ProgressLoading = ProgressBarWidget.Cast(m_WidgetRoot.FindAnyWidget("LoadingBar"));
		
		string tmp;
		m_ProgressText = TextWidget.Cast(m_WidgetRoot.FindAnyWidget("ProgressText"));
		if (GetGame())
		{
			m_ProgressText.Show(GetGame().CommandlineGetParam("loadingTest", tmp));
		}
		m_WidgetRoot.FindAnyWidget("notification_root").Show(false);

		#ifdef PLATFORM_CONSOLE
		#ifdef PLATFORM_XBOX
		#ifdef BUILD_EXPERIMENTAL
			Widget expNotification = m_WidgetRoot.FindAnyWidget("notification_root");
			if (expNotification)
			{
				expNotification.Show(true);
			}
		#endif
		#endif
		#endif
		
		m_ModdedWarning.Show(g_Game.ReportModded());
		m_ImageLogoMid.Show(true);
		m_ImageLogoCorner.Show(false);
		
		m_ImageWidgetBackground.Show(true);		
		m_Counter = 0;
				
		// lighten up your desktop
		game.GetBacklit().LoadingAnim();
		
		ProgressAsync.SetProgressData(m_ProgressLoading);
		ProgressAsync.SetUserData(m_ImageBackground);
	}

	//! DEPRECATED
	void OnTimer();
	
	void Inc()
	{
		m_LastProgressUpdate = m_DayZGame.GetTickTime();
		m_Counter++;
		if (m_Counter == 1)
		{
			Show();
		}
	}
	
	void Dec()
	{
		m_Counter = m_Counter - 1;
	
		if (m_Counter <= 0)
		{
			m_Counter = 0;
			EndLoading();
			m_HintPanel = null;
		}
	}
	
	void EndLoading()
	{
		ProgressAsync.SetProgressData(null);
		ProgressAsync.SetUserData(null);
		m_WidgetRoot.Show(false);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(g_Game.CheckDialogs);
	}
	
	bool IsLoading()
	{
		return m_WidgetRoot.IsVisible();
	}
	
	void SetTitle(string title)
	{
		m_TextWidgetTitle.SetText(title);
	}
	
	void SetStatus(string status)
	{
		m_TextWidgetStatus.SetText(status);
	}
	
	void SetProgress(float val)
	{
		float time_delta = m_DayZGame.GetTickTime() - m_LastProgressUpdate;
		
		m_LastProgressUpdate = m_DayZGame.GetTickTime();
	}
	
	void OnUpdate(float timeslice)
	{

	}
	
	void ShowEx(DayZGame game)
	{
		if (!m_HintPanel)
		{
			m_HintPanel					= new UiHintPanelLoading(m_WidgetRoot.FindAnyWidget("hint_frame"));
			m_HintPanel.Init(game);
		}

		Show();
	}
	
	void Show()
	{
		Widget lIcon = m_ImageBackground;
		Widget pText = m_ProgressLoading;
		m_ProgressText.SetText("");
		m_ProgressLoading.SetCurrent(0.0);
		m_ImageBackground.SetMaskProgress(0.0);
		
		if (!m_WidgetRoot.IsVisible())
		{
			if (m_DayZGame.GetUIManager().IsDialogVisible())
			{
				m_DayZGame.GetUIManager().HideDialog();
			}
			
			if (m_DayZGame.GetMissionState() == DayZGame.MISSION_STATE_MAINMENU)
			{
				m_ImageLogoMid.Show(false);
				m_ImageLogoCorner.Show(false);				
				m_ImageWidgetBackground.Show(true);		
				m_TextWidgetStatus.Show(true);
			}
			else
			{
				m_ImageLogoMid.Show(true);
				m_ImageLogoCorner.Show(false);
				m_ImageWidgetBackground.Show(true);
				m_TextWidgetStatus.Show(false);
			}
			
			m_WidgetRoot.Show(true);
			m_TextWidgetTitle.SetText("");
			m_TextWidgetStatus.SetText("");
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

		if (m_Counter <= 0)
		{
			m_WidgetRoot.Show(false);
			ProgressAsync.SetUserData(null);
			ProgressAsync.SetProgressData(null);
			m_HintPanel = null;
		}
	}
};


class DayZGame extends CGame
{
	protected ref BillboardSetHandler m_BillboardSetHandler;
	
	const int MISSION_STATE_MAINMENU = 0;
	const int MISSION_STATE_GAME = 1;
	const int MISSION_STATE_FINNISH = 2;
	
	bool m_AutotestEnabled;
	
	private const int STATS_COUNT = EConnectivityStatType.COUNT;
	private EConnectivityStatLevel m_ConnectivityStatsStates[STATS_COUNT];
	
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
	private ref ScriptInvoker m_postUpdateQueue[CALL_CATEGORY_COUNT];
	private ref DragQueue m_dragQueue;
	private ref ScriptInvoker m_YieldDataInitInvoker;
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
	private bool	m_CursorDesiredVisibilityScript = true;
	private int		m_PreviousGamepad;
	private float	m_UserFOV;

	private float	m_DeltaTime;

	float 	m_volume_sound;
	float 	m_volume_speechEX;
	float 	m_volume_music;
	float 	m_volume_VOIP;
	float 	m_volume_radio;
	
	float 	m_PreviousEVValue;
	float 	m_EVValue = 0;
	
	#ifdef DIAG_DEVELOPER
	private static ref ServerFpsStatsUpdatedEventParams m_ServerFpsStatsParams; 
	#endif
	
	static ref AmmoCamParams m_AmmoShakeParams = new AmmoCamParams();//TODO: make static, reuse
	
	static ref ScriptInvoker Event_OnRPC = new ScriptInvoker();
	
	private ref Backlit m_Backlit;
	
	private ref array<string> m_CharClassNames = new array<string>();
	private ref array<int> m_ConnectedInputDeviceList; //has to be owned here, 'Input' is a native class
	
	//Used for helicrash sound
	private ref EffectSound m_CrashSound;
	
	//Used for Artillery sound
	private ref EffectSound m_ArtySound;
	private const int MIN_ARTY_SOUND_RANGE = 300; // The distance under which sound is no longer heard
	
	static ref NoiseParams m_NoiseParams = new NoiseParams();
	static ref TemperatureAccessManager m_TAManager = new TemperatureAccessManager();
	#ifdef DEVELOPER
	static bool m_IsPreviewSpawn;//! we set this to 'true' when spawning item through Script Console only for item preview purposes
	#endif
	#ifdef DIAG_DEVELOPER
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
			m_callQueue[i] 		= new ScriptCallQueue();
			m_updateQueue[i] 	= new ScriptInvoker();
			m_timerQueue[i] 	= new TimerQueue();
			
			m_postUpdateQueue[i] = new ScriptInvoker();
		}
		
		m_dragQueue = new DragQueue;
		
		m_LoginTime 		 = 0;
		
		string tmp;
		if (CommandlineGetParam("stresstest", tmp))
		{
			m_IsStressTest = true;
		}

		if (CommandlineGetParam("doAimLogs", tmp))
		{
			m_AimLoggingEnabled = true;
		}
		
		// initialize backlit effects
		m_Backlit = new Backlit();
		m_Backlit.OnInit(this);
		
		m_ReportModded = GetModToBeReported();

	#ifndef NO_GUI
		if (m_loading == null)
		{
			m_loading = new LoadingScreen(this);
		}

		if (m_loading)
		{
			m_loading.ShowEx(this);
		}

		RefreshMouseCursorVisibility();
	#endif
			
		Debug.Init();
		Component.Init();
		CachedObjectsParams.Init();
		CachedObjectsArrays.Init();
		BleedChanceData.InitBleedChanceData();
		GetUApi().PresetSelect(GetUApi().PresetCurrent());

		m_DayZProfileOptions = new DayZProfilesOptions();
		
		GetCallQueue(CALL_CATEGORY_GUI).Call(DeferredInit);
		GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(GlobalsInit);
		
		string path = "cfgVehicles";
		string child_name = "";
		int count = ConfigGetChildrenCount(path);
		
		for (int p = 0; p < count; ++p)
		{
			ConfigGetChildName(path, p, child_name);
			
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
		BleedChanceData.Cleanup();
		NotificationSystem.CleanupInstance();
		
		g_Game = null;
		SetDispatcher(null);
		Print("~DayZGame()");
	}
	
	// ------------------------------------------------------------
	void DeferredInit()
	{
		GameOptions opt = new GameOptions();
		opt.Initialize();
		
		GetInput().UpdateConnectedInputDeviceList();

		m_UserFOV = GetUserFOVFromConfig();
		
		m_volume_sound 		= GetSoundScene().GetSoundVolume();
		m_volume_speechEX 	= GetSoundScene().GetSpeechExVolume();
		m_volume_music 		= GetSoundScene().GetMusicVolume();
		m_volume_VOIP 		= GetSoundScene().GetVOIPVolume();
		m_volume_radio 		= GetSoundScene().GetRadioVolume();
		
		PPEManagerStatic.GetPPEManager().Init();
		GetMenuDefaultCharacterData();
	}
	
	// ------------------------------------------------------------
	void GlobalsInit()
	{
		if (GetCEApi())
		{
			m_IsWorldWetTempUpdateEnabled = (GetCEApi().GetCEGlobalInt("WorldWetTempUpdate") == 1);
			
			m_FoodDecayModifier = GetCEApi().GetCEGlobalFloat("FoodDecay");
			
			//check for legacy INT format, if value == float.MIN when read as FLOAT, it is of type INT, so we read it as such below	
			if (m_FoodDecayModifier == float.MIN)
			{
				m_FoodDecayModifier = GetCEApi().GetCEGlobalInt("FoodDecay");
			}
		}
		
		//we need to perform the load here as some objects behaving correctly after spawn is dependent on CE being initialized before spawning them
		ObjectSpawnerHandler.OnGameplayDataHandlerLoad();
	}
	
	// ------------------------------------------------------------
	void RegisterProfilesOptions()
	{
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.CROSSHAIR, SHOW_CROSSHAIR);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.HUD, SHOW_HUD);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.HUD_VEHICLE, SHOW_HUD_VEHICLE);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.QUICKBAR, SHOW_QUICKBAR);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.SERVER_MESSAGES, SYSTEM_CHAT_MSG);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.USERS_CHAT, DIRECT_CHAT_MSG);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.RADIO_CHAT, RADIO_CHAT_MSG);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.GAME_MESSAGES, GAME_CHAT_MSG, false);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.ADMIN_MESSAGES, ADMIN_CHAT_MSG, false);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.PLAYER_MESSAGES, PLAYER_CHAT_MSG, false);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.SERVERINFO_DISPLAY, SHOW_SERVERINFO, true);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.BLEEDINGINDICATION, ENABLE_BLEEDINGINDICATION, true);
		m_DayZProfileOptions.RegisterProfileOptionBool(EDayZProfilesOptions.CONNECTIVITY_INFO, SHOW_CONNECTIVITYINFO, true);
		
		m_DayZProfileOptions.RegisterProfileOptionFloat(EDayZProfilesOptions.HUD_BRIGHTNESS, HUD_BRIGHTNESS, 0.0);
		
		m_DayZProfileOptions.RegisterProfileOptionInt(EDayZProfilesOptions.AMBIENT_MUSIC_MODE, OPTIONS_SOUND_AMBIENT_SOUND_MODE, 0);
	}
	
	void ResetProfileOptions()
	{
		m_DayZProfileOptions.ResetOptionsBool();
		m_DayZProfileOptions.ResetOptionsInt();
		m_DayZProfileOptions.ResetOptionsFloat();
	}
	
	//! Called from C++
	void SetMissionPath(string path)
	{
		m_MissionPath = path;
		
		int pos_end = 0;
		int pos_cur = 0;
				
		while (pos_cur != -1)
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

	override ScriptInvoker GetPostUpdateQueue(int call_category)
	{
		return m_postUpdateQueue[call_category];
	}
	
	ScriptInvoker GetYieldDataInitInvoker()
	{
		if (!m_YieldDataInitInvoker)
			m_YieldDataInitInvoker = new ScriptInvoker();

		return m_YieldDataInitInvoker;
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
	void SetMissionState(int state)
	{
		m_MissionState = state;
	}
	
	// ------------------------------------------------------------
	bool GetProfileOption(EDayZProfilesOptions option)
	{
		return m_DayZProfileOptions.GetProfileOption(option);
	}
	 	
	bool GetProfileOptionBool(EDayZProfilesOptions option)
	{
		return GetProfileOption(option);
	}
	
	int GetProfileOptionInt(EDayZProfilesOptions option)
	{
		return m_DayZProfileOptions.GetProfileOptionInt(option);
	}
	
	float GetProfileOptionFloat(EDayZProfilesOptions option)
	{
		return m_DayZProfileOptions.GetProfileOptionFloat(option);
	}
	
	bool GetProfileOptionDefault(EDayZProfilesOptions option)
	{
		return m_DayZProfileOptions.GetProfileOptionDefaultBool(option);
	}
	
	bool GetProfileOptionDefaultBool(EDayZProfilesOptions option)
	{
		return GetProfileOptionDefault(option);
	}
	
	int GetProfileOptionDefaultInt(EDayZProfilesOptions option)
	{
		return m_DayZProfileOptions.GetProfileOptionDefaultInt(option);
	}
	
	float GetProfileOptionDefaultFloat(EDayZProfilesOptions option)
	{
		return m_DayZProfileOptions.GetProfileOptionDefaultFloat(option);
	}
	
	void SetProfileOption(EDayZProfilesOptions option, bool value)
	{
		m_DayZProfileOptions.SetProfileOptionBool(option, value);
	}
	
	void SetProfileOptionBool(EDayZProfilesOptions option, bool value)
	{
		SetProfileOption(option, value);
	}
	
	void SetProfileOptionInt(EDayZProfilesOptions option, int value)
	{
		m_DayZProfileOptions.SetProfileOptionInt(option, value);
	}
	
	void SetProfileOptionFloat(EDayZProfilesOptions option, float value)
	{
		m_DayZProfileOptions.SetProfileOptionFloat(option, value);
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
	
	void SetGameState(DayZGameState state)
	{
		m_GameState = state;
	}
	
	DayZGameState GetGameState()
	{
		return m_GameState;
	}
	
	void SetLoadState(DayZLoadState state)
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
	#ifdef ENABLE_LOGGING
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
	private void ClearConnectivityStates()
	{
		for (int i = 0; i < STATS_COUNT; i++)
			m_ConnectivityStatsStates[i] = 0;
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
				#ifndef SERVER
				ClearConnectivityStates();
				#endif
				m_FirstConnect = true;
				ClientData.ResetClientData();
				break;
			}
			case MPSessionEndEventTypeID:
			{
				LoadingHide();
				CancelLoginTimeCountdown();
				SetConnecting(false);
				SetGameState(DayZGameState.MAIN_MENU);
				m_FirstConnect = true;
				#ifdef PLATFORM_CONSOLE
				if (GetUserManager().GetSelectedUser())
				{
					OnlineServices.LeaveGameplaySession();
					OnlineServices.ClearCurrentServerInfo();
					if (GetGameState() == DayZGameState.IN_GAME)
					{
						SetLoadState(DayZLoadState.MAIN_MENU_START);
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
				CancelLoginQueue();
				LoadingHide(true);
				SetConnecting(false);
				ProgressAsync.DestroyAllPendingProgresses();
				
				if (GetGameState() == DayZGameState.CONNECTING)
				{
					SetGameState(DayZGameState.MAIN_MENU);
				}
				
				break;
			}
			case MPSessionPlayerReadyEventTypeID:
			{
				LoadingHide(true);
				ProgressAsync.DestroyAllPendingProgresses();
				
				SetGameState(DayZGameState.IN_GAME);
				
				// analytics - spawned
				StatsEventSpawnedData spawnData = new StatsEventSpawnedData();
				spawnData.m_CharacterId = g_Game.GetDatabaseID();
				spawnData.m_Lifetime = 0;
				spawnData.m_Position = vector.Zero;
				if (GetPlayer())
				{
					spawnData.m_Position = GetPlayer().GetPosition();
				}
				spawnData.m_DaytimeHour = 0;
				spawnData.m_Population = 0;
				Analytics.PlayerSpawned(spawnData);
				
				#ifdef PLATFORM_CONSOLE
				m_Notifications.ClearVoiceNotifications();
				OnlineServices.SetMultiplayState(true);
				#endif
				if (m_FirstConnect)
				{
					m_FirstConnect = false;
					if (GetHostAddress(address, port))
					{
						AddVisitedServer(address, port);
					}
					
					#ifdef PLATFORM_CONSOLE
						#ifndef PLATFORM_WINDOWS // if app is not on Windows with -XBOX parameter
							if (null != GetUserManager().GetSelectedUser())
							{
								OnlineServices.EnterGameplaySession();
								OnlineServices.LoadVoicePrivilege();
							}
						#endif
					#endif
				}
				
				UpdateInputDeviceDisconnectWarning();
				
				break;
			}
			case MPConnectionLostEventTypeID:
			{
				MPConnectionLostEventParams conLost_params;
				if (Class.CastTo(conLost_params, params))
				{
					int duration = conLost_params.param1;
					OnMPConnectionLostEvent(duration);
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
					GetCallQueue(CALL_CATEGORY_GUI).Call(GetMission().StartLogoutMenu, logoutParams.param1);						
				}
				break;
			}
			case SelectedUserChangedEventTypeID:
			{
				OnlineServices.Init();
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
			case ConnectingStartEventTypeID:
			{
				g_Game.SetGameState(DayZGameState.CONNECTING);
				SetConnecting(true);
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
					Print("### DLC Ownership failed !!! Map: " + dlcParams.param1);
				}
				break;
			}
			case ConnectivityStatsUpdatedEventTypeID:
			{
				ConnectivityStatsUpdatedEventParams connectivityStatsParams; 
				if (Class.CastTo(connectivityStatsParams, params))
				{
					PlayerIdentity playerIdentity = connectivityStatsParams.param1;
					
					int pingAvg = playerIdentity.GetPingAvg();
					if (pingAvg < GetWorld().GetPingWarningThreshold())
					{
						SetConnectivityStatState(EConnectivityStatType.PING, EConnectivityStatLevel.OFF);
					}
					else if (pingAvg < GetWorld().GetPingCriticalThreshold())
					{
						SetConnectivityStatState(EConnectivityStatType.PING, EConnectivityStatLevel.LEVEL1);
					}
					else
					{
						SetConnectivityStatState(EConnectivityStatType.PING, EConnectivityStatLevel.LEVEL2);
					}
				}
				break;
			}
			case ServerFpsStatsUpdatedEventTypeID:
			{
				ServerFpsStatsUpdatedEventParams serverFpsStatsParams; 
				if (Class.CastTo(serverFpsStatsParams, params))
				{
					#ifdef DIAG_DEVELOPER
					m_ServerFpsStatsParams = serverFpsStatsParams;
					#endif
					float fps = serverFpsStatsParams.param1;
					if (fps > GetWorld().GetServerFpsWarningThreshold())
					{
						SetConnectivityStatState(EConnectivityStatType.SERVER_PERF, EConnectivityStatLevel.OFF);
					}
					else if (fps > GetWorld().GetServerFpsCriticalThreshold())
					{
						SetConnectivityStatState(EConnectivityStatType.SERVER_PERF, EConnectivityStatLevel.LEVEL1);
					}
					else
					{
						SetConnectivityStatState(EConnectivityStatType.SERVER_PERF, EConnectivityStatLevel.LEVEL2);
					}
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
	
	void SetConnectivityStatState(EConnectivityStatType type, EConnectivityStatLevel level)
	{
		if (level != m_ConnectivityStatsStates[type])
		{
			if (OnConnectivityStatChange(type, level, m_ConnectivityStatsStates[type]))//before setting the prev. level to be the current level, we need to make sure we successfully set the icon
			{
				m_ConnectivityStatsStates[type] = level;
			}
		}
	}
	
	protected bool OnConnectivityStatChange(EConnectivityStatType type, EConnectivityStatLevel newLevel, EConnectivityStatLevel oldLevel)
	{
		if (!GetGame() || !GetGame().GetMission())
			return false;
		Hud hud = GetGame().GetMission().GetHud();
		if (!hud) 
			return false;

		hud.SetConnectivityStatIcon(type, newLevel);
		return true;
	}
	
	#ifdef DIAG_DEVELOPER
	private void DrawPerformanceStats(float pingAct, float pingAvg, float throttleInput, float throttleOutput)
	{
		DbgUI.Begin("Performance Stats", 720, 10);
		DbgUI.Text("pingAct:" + pingAct );
		int color = COLOR_WHITE;
		if ( pingAvg >= GetWorld().GetPingCriticalThreshold())
			color = COLOR_RED;
		else if ( pingAvg >= GetWorld().GetPingWarningThreshold())
			color = COLOR_YELLOW;

		DbgUI.ColoredText(color, "pingAvg:" + pingAvg);
		DbgUI.Text("Ping Warning Threshold:" + GetWorld().GetPingWarningThreshold());
		DbgUI.Text("Ping Critical Threshold:" + GetWorld().GetPingCriticalThreshold());
		DbgUI.PlotLive("pingAvg history:", 300, 125, pingAvg, 100, 100 );
		DbgUI.Text("Server Fps Warning Threshold:" + GetWorld().GetServerFpsWarningThreshold());
		DbgUI.Text("Server Fps Critical Threshold:" + GetWorld().GetServerFpsCriticalThreshold());
		if (m_ServerFpsStatsParams)// SERVER FPS
		{
			color = COLOR_WHITE;
			if ( m_ServerFpsStatsParams.param1 <= GetWorld().GetServerFpsCriticalThreshold())
				color = COLOR_RED;
			else if ( m_ServerFpsStatsParams.param1 <= GetWorld().GetServerFpsWarningThreshold())
				color = COLOR_YELLOW;

			DbgUI.ColoredText(color, "serverFPS:" + m_ServerFpsStatsParams.param1.ToString() );
			DbgUI.PlotLive("serverFPS history:", 300, 125, m_ServerFpsStatsParams.param1, 100, 100 );

			color = COLOR_WHITE;
			DbgUI.ColoredText(COLOR_WHITE, "serverFrameTime:" + m_ServerFpsStatsParams.param2.ToString() );
			DbgUI.PlotLive("serverFrameTime history:", 300, 75, m_ServerFpsStatsParams.param2, 100, 100 );

			color = COLOR_WHITE;
			if (m_ServerFpsStatsParams.param3 > 0)
				color = COLOR_RED;

			DbgUI.ColoredText(color, "physStepsSkippedServer:" + m_ServerFpsStatsParams.param3.ToString() );
			DbgUI.PlotLive("physStepsSkippedServer history:", 300, 75, m_ServerFpsStatsParams.param3, 100, 100 );

			color = COLOR_WHITE;
			if (m_ServerFpsStatsParams.param4 > 0)
				color = COLOR_RED;
			DbgUI.ColoredText(color, "physStepsSkippedClient:" + m_ServerFpsStatsParams.param4.ToString() );
			DbgUI.PlotLive("physStepsSkippedClient history:", 300, 75, m_ServerFpsStatsParams.param4, 100, 100 );
		}

		DbgUI.Text("throttleInput:" + throttleInput);
		DbgUI.PlotLive("throttleInput history:", 300, 75, throttleInput, 100, 50 );
		DbgUI.Text("throttleOutput:" + throttleOutput);
		DbgUI.PlotLive("throttleOutput history:", 300, 75, throttleOutput, 100, 50 );
		DbgUI.End();
	}
	#endif
	
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
		if (m_LoginTime > 0)
		{
			if (m_LoginTimeScreen)
				m_LoginTimeScreen.SetTime(m_LoginTime);

			m_LoginTime--;
		}
		else
		{
			// stop the call loop
			CancelLoginTimeCountdown();
		}
	}
	
	// ------------------------------------------------------------
	void OnRespawnEvent(int time)
	{	
		// use login time screen for respawn timer
		if (time >= 0)
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
		
		//GetMenuData().RequestGetDefaultCharacterData();
		GetMenuDefaultCharacterData().SerializeCharacterData(ctx.GetWriteContext());
		StoreLoginData(ctx.GetWriteContext());
	}
	
	// ------------------------------------------------------------
	void EnterLoginQueue(UIMenuPanel parent)
	{
		m_LoginQueue = LoginQueueBase.Cast(GetUIManager().EnterScriptedMenu(MENU_LOGIN_QUEUE, parent)); 		
	}
	
	// ------------------------------------------------------------
	void EnterLoginTime(UIMenuPanel parent)
	{			 	
		m_LoginTimeScreen = LoginTimeBase.Cast(GetUIManager().EnterScriptedMenu(MENU_LOGIN_TIME, parent)); 		
	}
	
	// ------------------------------------------------------------
	void OnMPConnectionLostEvent(int duration)
	{
		if (duration >= 0)//(-1 means conn. reestablished)
			SetConnectivityStatState(EConnectivityStatType.CONN_LOST, EConnectivityStatLevel.LEVEL1);
		else
			SetConnectivityStatState(EConnectivityStatType.CONN_LOST, EConnectivityStatLevel.OFF);
		
		#ifdef PLATFORM_PS4
		//PSN Set multiplay state	
		if (duration < 0 && GetGameState() == DayZGameState.IN_GAME)
		{
			OnlineServices.SetMultiplayState(true);
		}
		else
		{
			OnlineServices.SetMultiplayState(false);
		}
		#endif
	}
	
	// ------------------------------------------------------------
	void LoadProgressUpdate(int progressState, float progress, string title)
	{
	#ifndef NO_GUI
		switch (progressState)
		{
		case PROGRESS_START:
			{
			#ifndef NO_GUI	
				// get out of the black screen immediately
				GetUIManager().ScreenFadeOut(0);
			#endif
				m_loading.Inc();
				m_loading.SetTitle(title);
				if (m_loading.m_HintPanel)
					m_loading.m_HintPanel.ShowRandomPage();
				
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
		Math.Randomize(-1);
	}
	
	// ------------------------------------------------------------
	override void OnActivateMessage()
	{
		RefreshMouseCursorVisibility();
	}

	// ------------------------------------------------------------
	override void OnDeactivateMessage()
	{
		RefreshMouseCursorVisibility();
	}
	
	// ------------------------------------------------------------
	override bool OnInitialize()
	{
		ParticleList.PreloadParticles();
		
		RegisterProfilesOptions();
		SetHudBrightness(GetHUDBrightnessSetting());
		
		InitNotifications();
		m_Visited = new TStringArray;
		GetProfileStringList("SB_Visited", m_Visited);
		
		if (GetLoadState() == DayZLoadState.UNDEFINED)
		{
			string param;
			
			if (GetCLIParam("join", param))
			{
				JoinLaunch();
				#ifndef PLATFORM_PS4
				CreateTitleScreen();
				#endif
			}
			else if (GetCLIParam("connect", param))
			{
				ConnectLaunch();
			}
			else if (GetCLIParam("autotest", param))
			{
				AutoTestLaunch(param);
			}
			else if (GetCLIParam("mission", param))
			{
				MissionLaunch();
			}
			else if (GetCLIParam("party", param))
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
		NotificationSystem.InitInstance();
		m_Notifications = new NotificationUI();
	}
	
	protected ref Widget		m_IntroMenu;
	protected ref Widget		m_GamepadDisconnectMenu; //DEPRECATED
	protected int				m_PrevBlur;
	
	protected string			m_DatabaseID;
	
	protected string			m_ConnectAddress;
	protected int				m_ConnectPort;
	protected int				m_ConnectSteamQueryPort;
	protected string			m_ConnectPassword;
	
	protected const int			MAX_VISITED = 50;
	protected ref TStringArray	m_Visited;
	
	string GetDatabaseID()
	{
		return m_DatabaseID;
	}
	
	void SetDatabaseID(string id)
	{
		m_DatabaseID = id;
		if (GetUIManager().GetMenu())
		{
			GetUIManager().GetMenu().Refresh();
		}
	}
	
	void CreateTitleScreen()
	{
		DeleteTitleScreen();
		m_IntroMenu = GetWorkspace().CreateWidgets("gui/layouts/xbox/day_z_title_screen.layout");
		RichTextWidget text_widget = RichTextWidget.Cast(m_IntroMenu.FindAnyWidget("InputPromptText"));
		m_IntroMenu.FindAnyWidget("notification_root").Show(false);
		if (text_widget)
		{
			string text = Widget.TranslateString("#console_start_game");
			#ifdef PLATFORM_XBOX
				BiosUserManager user_manager = GetGame().GetUserManager();
				if (user_manager && user_manager.GetSelectedUser())
					text_widget.SetText(string.Format(text, "<image set=\"xbox_buttons\" name=\"A\" />"));
				else
					text_widget.SetText(string.Format(text, "<image set=\"xbox_buttons\" name=\"A\" />"));
			#endif
					
			#ifdef PLATFORM_PS4
				string confirm = "cross";
				if (GetGame().GetInput().GetEnterButton() == GamepadButton.A)
				{
					confirm = "cross";
				}
				else
				{
					confirm = "circle";
				}
				text_widget.SetText(string.Format(text, "<image set=\"playstation_buttons\" name=\"" + confirm + "\" />"));
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
		if (m_IntroMenu)
		{
			delete m_IntroMenu;
		}
	}
	
	bool ShouldShowControllerDisconnect()
	{
		return m_ShouldShowControllerDisconnect;
	}
	
	void UpdateInputDeviceDisconnectWarning()
	{
		#ifdef PLATFORM_CONSOLE
		if (!GetUIManager().IsMenuOpen(MENU_WARNING_INPUTDEVICE_DISCONNECT))
		{
			m_ShouldShowControllerDisconnect = !GetInput().AreAllAllowedInputDevicesActive();
			if (m_ShouldShowControllerDisconnect)
			{
				GetCallQueue(CALL_CATEGORY_GUI).Call(GetUIManager().EnterScriptedMenu,MENU_WARNING_INPUTDEVICE_DISCONNECT,GetUIManager().GetMenu());
			}
		}
		#endif
	}
	
	void JoinLaunch()
	{
		SetGameState(DayZGameState.JOIN);
		SetLoadState(DayZLoadState.JOIN_START);
		
		#ifdef PLATFORM_CONSOLE
			string join_param;
			if (GetCLIParam("join", join_param))
			{
				BiosUserManager user_manager = GetUserManager();
				user_manager.ParseJoinAsync(join_param);
			}
		#endif
	}
	
	void ConnectLaunch()
	{
		BiosUserManager user_manager = GetUserManager();
		if (user_manager.GetTitleInitiator())
		{
			user_manager.SelectUserEx(user_manager.GetTitleInitiator());
		}
		
		SetGameState(DayZGameState.CONNECT);
		SetLoadState(DayZLoadState.CONNECT_START);
		
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
		SetGameState(DayZGameState.PARTY);
		SetLoadState(DayZLoadState.PARTY_START);
		BiosUserManager user_manager = GetGame().GetUserManager();
		
		string param;
		if (GetCLIParam("party", param))
		{
			user_manager.ParsePartyAsync(param);
			StartRandomCutscene(GetMainMenuWorld());
		}
	}
	
	void MainMenuLaunch()
	{
#ifdef PLATFORM_WINDOWS
		BiosUserManager user_manager = GetUserManager();
		if (user_manager)
		{
			if (user_manager.GetTitleInitiator())
			{
				user_manager.SelectUserEx(user_manager.GetTitleInitiator());
			}
		}
#endif
		
		SetGameState(DayZGameState.MAIN_MENU);
		SetLoadState(DayZLoadState.MAIN_MENU_START);		
		StartRandomCutscene(GetMainMenuWorld());		
		DeleteTitleScreen();
	}
	
	void MissionLaunch()
	{
		BiosUserManager user_manager = GetUserManager();
		if (user_manager)
		{
			if (user_manager.GetTitleInitiator())
			{
				user_manager.SelectUserEx(user_manager.GetTitleInitiator());
			}
		}
		
		SetGameState(DayZGameState.IN_GAME);
		SetLoadState(DayZLoadState.MISSION_START);

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
	
	void AutoTestLaunch(string param)
	{
		if (!AutotestConfigHandler.LoadData(param))
			AutoTestFixture.LogRPT("Failed to load autotest configuration, continue with mission load.");
		else
			m_AutotestEnabled = true;

		string mission;
		GetCLIParam("mission", mission);
		if (!mission)
		{
			AutoTestFixture.LogRPT("Parameter 'mission' is not set on CLI.");
			RequestExit(IDC_MAIN_QUIT);
		}

		PlayMission(mission);
	}
	
	void SelectUser(int gamepad = -1)
	{
		BiosUserManager user_manager = GetUserManager();
		if (user_manager)
		{
			BiosUser selected_user;
			if (gamepad > -1)
			{
				GetInput().GetGamepadUser(gamepad, selected_user);
				#ifdef PLATFORM_PS4
				if (selected_user)
				#endif
				{
					if (user_manager.SelectUserEx(selected_user))
					{
						GetGame().GetInput().IdentifyGamepad(GamepadButton.BUTTON_NONE);
						GetInput().SelectActiveGamepad(gamepad);
					}
					else
					{
						selected_user = user_manager.GetSelectedUser();
					}
					
					#ifdef PLATFORM_PS4
					if (!selected_user.IsOnline())
					{
						user_manager.LogOnUserAsync(selected_user); 
						return;
					}
					#endif
				}
				#ifdef PLATFORM_PS4
				else
				{
					GetInput().ResetActiveGamepad();
					GamepadCheck();
				}
				#endif
			}
			
			if (!selected_user)
				selected_user = user_manager.GetSelectedUser();
			
			if (!selected_user)
			{
				user_manager.PickUserAsync();
				return;
			}
			
			user_manager.SelectUserEx(selected_user);
			
			switch (GetLoadState())
			{
				case DayZLoadState.JOIN_START:
				{
					SetLoadState(DayZLoadState.JOIN_USER_SELECT);				
					break;
				}
				case DayZLoadState.PARTY_START:
				{
					SetLoadState(DayZLoadState.PARTY_USER_SELECT);
					break;
				}
				case DayZLoadState.MAIN_MENU_START:
				{
					SetLoadState(DayZLoadState.MAIN_MENU_USER_SELECT);
					break;
				}
				case DayZLoadState.CONNECT_START:
				{
					SetLoadState(DayZLoadState.CONNECT_USER_SELECT);
					break;
				}
				case DayZLoadState.MISSION_START:
				{
					SetLoadState(DayZLoadState.MISSION_USER_SELECT);
					break;
				}
				default:
					break;
			}
			
			SelectGamepad();
			g_Game.SetHudBrightness(g_Game.GetHUDBrightnessSetting());
		}
	}
	
	void SetPreviousGamepad(int gamepad)
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
		if (GetInput().IsActiveGamepadSelected())
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
			if (GetUserManager().GetSelectedUser())
			{
				int gamepad = GetInput().GetUserGamepad(GetUserManager().GetSelectedUser());
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
				if (!m_IntroMenu && !(GetUIManager().GetMenu() && GetUIManager().GetMenu().GetID() == MENU_TITLE_SCREEN))
					CreateTitleScreen();
				GetInput().IdentifyGamepad(GetInput().GetEnterButton());
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
		
		if (user_manager)
		{
			BiosUser selected_user = user_manager.GetSelectedUser();
			if (selected_user)
			{
				OnlineServices.SetBiosUser(selected_user);
				SetPlayerName(selected_user.GetName());
				SetUserFOV(GetUserFOVFromConfig());
				#ifdef PLATFORM_CONSOLE
				SetPlayerGameName(selected_user.GetName());
				user_manager.GetUserDatabaseIdAsync();
				#endif
			}
			
			if (GetUIManager().GetMenu())
			{
				GetUIManager().GetMenu().Refresh();
			}
		}
		
		switch (GetLoadState())
		{
			case DayZLoadState.JOIN_USER_SELECT:
			{
				SetLoadState(DayZLoadState.JOIN_CONTROLLER_SELECT);
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
				SetLoadState(DayZLoadState.CONNECT_CONTROLLER_SELECT);
				OnlineServices.LoadMPPrivilege();
				break;
			}
			case DayZLoadState.MAIN_MENU_USER_SELECT:
			{
				SetLoadState(DayZLoadState.MAIN_MENU_CONTROLLER_SELECT);
				DeleteTitleScreen();
				GetUIManager().EnterScriptedMenu(MENU_MAIN, GetUIManager().GetMenu());
				break;
			}
			case DayZLoadState.MISSION_USER_SELECT:
			{
				SetLoadState(DayZLoadState.MISSION_CONTROLLER_SELECT);
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
		if (GetLoadState() == DayZLoadState.JOIN_CONTROLLER_SELECT)
		{
			SetGameState(DayZGameState.CONNECTING);
			OnlineServices.GetSession();
		}
		else
		{
			if (GetGameState() != DayZGameState.CONNECTING)
			{
				switch (GetLoadState())
				{
					case DayZLoadState.CONNECT_CONTROLLER_SELECT:
					{
						SetGameState(DayZGameState.CONNECTING);
						ConnectFromCLI();
						break;
					}
					case DayZLoadState.PARTY_CONTROLLER_SELECT:
					{
						SetGameState(DayZGameState.CONNECTING);
						Connect();
						break;
					}
					case DayZLoadState.MAIN_MENU_CONTROLLER_SELECT:
					{
						SetGameState(DayZGameState.CONNECTING);
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
	}
	
	bool GetLastVisitedServer(out string ip, out int port)
	{
		if (m_Visited)
		{
			if (m_Visited.Count() > 0)
			{
				string uid = m_Visited.Get(m_Visited.Count() - 1);
				TStringArray output = new TStringArray;
				uid.Split(":", output);
				ip = output[0];
				port = output[1].ToInt();
				return true;
			}
		}
		return false;
	}
	
	void AddVisitedServer(string ip, int port)
	{
		string uid = ip + ":" + port;
		if (m_Visited)
		{
			int pos = m_Visited.Find(uid);
			 
			if (pos < 0)
			{
				if (m_Visited.Count() == MAX_VISITED)
					m_Visited.Remove(0);
				m_Visited.Insert(uid);
			}
			else
			{
				// if item is not saved as last server, move it
				if (pos != (m_Visited.Count() - 1))
				{
					m_Visited.Remove(pos);
					m_Visited.Insert(uid);
				}
			}
			SetProfileStringList("SB_Visited", m_Visited);
			SaveProfile();
		}
	}
	
	bool IsVisited(string ip, int port)
	{
		string uid = ip + ":" + port;
		int index = m_Visited.Find(uid);
		return (index >= 0);
	}
	
	void RefreshCurrentServerInfo()
	{
		string addr;
		int port;
		if (GetHostAddress(addr, port))
		{
			m_ConnectAddress = addr;
			m_ConnectPort = port;
		}
		OnlineServices.GetCurrentServerInfo(m_ConnectAddress, m_ConnectPort);
	}

	void Connect()
	{
		SetConnecting(true);
		
		DeleteTitleScreen();
		string addr;
		int port;
		if (GetHostAddress(addr, port))
		{
			if (m_ConnectAddress == addr && m_ConnectPort == port)
				return;
		}
		
		string connectAddress = m_ConnectAddress;
		
		if (m_ConnectSteamQueryPort)
			connectAddress = string.Format("%1:%2:%3", m_ConnectAddress, m_ConnectPort, m_ConnectSteamQueryPort);

		if (Connect(GetUIManager().GetMenu(), connectAddress, m_ConnectPort, m_ConnectPassword) != 0)
			DisconnectSessionScript(true);
	}
	
	void DisconnectSessionScript(bool displayJoinError = false)
	{
		DisconnectSessionFlags flags = DisconnectSessionFlags.SELECT_USER | DisconnectSessionFlags.IGNORE_WHEN_IN_GAME;
		if (displayJoinError)
		{
			flags |= DisconnectSessionFlags.JOIN_ERROR_ENABLED;
			flags |= DisconnectSessionFlags.JOIN_ERROR_CHECK;
		}
		
		DisconnectSessionEx(flags);
	}
	
	void DisconnectSessionEx(DisconnectSessionFlags flags)
	{
		if (flags & DisconnectSessionFlags.SELECT_USER && OnlineServices.GetBiosUser())
		{
			GetGame().GetUserManager().SelectUserEx(OnlineServices.GetBiosUser());
		}
		
		if (flags & DisconnectSessionFlags.JOIN_ERROR_ENABLED)
		{
			if (!(flags & DisconnectSessionFlags.JOIN_ERROR_CHECK) || GetGameState() == DayZGameState.JOIN)
			{
				NotificationSystem.AddNotification(NotificationType.JOIN_FAIL_GET_SESSION, NotificationSystem.DEFAULT_TIME_DISPLAYED);
			}
		}
		
		if (flags & DisconnectSessionFlags.IGNORE_WHEN_IN_GAME && GetGameState() == DayZGameState.IN_GAME)
		{
			return;
		}
		
		if (flags & DisconnectSessionFlags.CLOSE_MENUS && GetGame().GetUIManager())
		{
			GetGame().GetUIManager().CloseAllSubmenus();
			
			if ( GetGame().GetUIManager().IsDialogVisible() )
			{
				GetGame().GetUIManager().CloseDialog();
			}
		}
			
		if (flags & DisconnectSessionFlags.ALWAYS_FORCE)
		{
			DisconnectSessionForce();
		}
		
		if (GetGame().GetMission())
		{
			if (g_Game.GetGameState() != DayZGameState.MAIN_MENU)
			{	
				if (flags & DisconnectSessionFlags.DISCONNECT_ERROR_ENABLED)
				{
					NotificationSystem.AddNotification(NotificationType.DISCONNECTED, NotificationSystem.DEFAULT_TIME_DISPLAYED);
				}
				
				GetGame().GetMission().AbortMission();

				SetGameState(DayZGameState.MAIN_MENU);
				SetLoadState(DayZLoadState.MAIN_MENU_CONTROLLER_SELECT);
				
				GamepadCheck();
			}
		}
		
		else
		{
			MainMenuLaunch();
		}
	}
	
	void ConnectFromServerBrowser(string ip, int port, string password = "")
	{
		m_ConnectAddress	= ip;
		m_ConnectPort		= port;
		m_ConnectPassword	= password;
		m_ConnectFromJoin	= false;
		OnlineServices.LoadMPPrivilege();
	}
	
	void ConnectFromServerBrowserEx(string ip, int port, int steamQueryPort, string password = "")
	{
		m_ConnectSteamQueryPort = steamQueryPort;
		ConnectFromServerBrowser(ip, port, password);
	}
	
	void ConnectFromJoin(string ip, int port)
	{
		m_ConnectAddress	= ip;
		m_ConnectPort		= port;
		m_ConnectFromJoin	= true;
		Connect();
	}
	
	void ConnectFromCLI()
	{
		string port;
		if (GetCLIParam("connect", m_ConnectAddress))
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
	
		if (key == KeyCode.KC_LCONTROL) 
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
		if (key == KeyCode.KC_LCONTROL) 
		{
			m_IsCtrlHolding = false;
		}
		
		if (key == KeyCode.KC_LWIN) 
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
	float GetDeltaT()
	{
		return m_DeltaTime;
	}
	
	// ------------------------------------------------------------
	override void OnUpdate(bool doSim, float timeslice)
	{
		m_DeltaTime = timeslice;

		Mission mission = GetMission();
		bool gameIsRunning = false;
	
		if (doSim && mission && !mission.IsPaused())
		{
			gameIsRunning = true;
		}
	
		if (doSim && mission)
		{
			mission.OnUpdate(timeslice);
			
			// update local weather effects
			if ( IsClient() || !IsMultiplayer() )
			{
				WorldData worldData = mission.GetWorldData();
				if ( worldData )
					worldData.UpdateWeatherEffects( GetWeather(), timeslice );
			}
		}
		
		SEffectManager.OnUpdate(timeslice);
	
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
		
		NotificationSystem.Update(timeslice);
		if (m_Notifications)
		{
			m_Notifications.Update(timeslice);
		}
		
		#ifndef SERVER
		#ifdef DIAG_DEVELOPER
		if (GetGame().IsMultiplayer() && GetPlayer() && DiagMenu.GetBool(DiagMenuIDs.MISC_CONNECTION_STATS))
		{
			PlayerIdentity playerIdentity = GetPlayer().GetIdentity();
			
			if (playerIdentity)
			{
				int pingAct = playerIdentity.GetPingAct();
				int pingAvg = playerIdentity.GetPingAvg();

				float throttleInput = playerIdentity.GetInputThrottle();
				float throttleOutput = playerIdentity.GetOutputThrottle();
				
				DrawPerformanceStats(pingAct, pingAvg, throttleInput, throttleOutput);
			}
		}
		#endif
		#endif
		
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
		Mission mission = GetMission();
		bool gameIsRunning = false;
	
		if (doSim && mission && !mission.IsPaused())
		{
			gameIsRunning = true;
		}
		
		GetPostUpdateQueue(CALL_CATEGORY_SYSTEM).Invoke(timeslice);
	
	#ifndef NO_GUI	
		if (m_loading && m_loading.IsLoading())
		{
		}
		else if (m_LoginTimeScreen && m_LoginTimeScreen.IsStatic())
		{
		}
		else
		{
			GetPostUpdateQueue(CALL_CATEGORY_GUI).Invoke(timeslice);
		}
	#endif
	
		if (gameIsRunning)
		{
			GetPostUpdateQueue(CALL_CATEGORY_GAMEPLAY).Invoke(timeslice);
		}
	}
	
	// ------------------------------------------------------------
	override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, target, rpc_type, ctx);
		Event_OnRPC.Invoke(sender, target, rpc_type, ctx);
		
		//Print("["+ GetGame().GetTime().ToString() +"] => DayZGame::OnRPC = "+ EnumTools.EnumToString(ERPCs,rpc_type));
		
		if (target)
		{
			// call rpc on target
			target.OnRPC(sender, rpc_type, ctx);
		}
		else
		{
			switch (rpc_type)
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
				case ERPCs.RPC_PLAYERRESTRICTEDAREAS_SYNC:
				{
					CfgPlayerRestrictedAreaHandler.OnRPC(ctx);
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
					
					NotificationSystem.AddNotification(type, show_time, detail_text);
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
					
					NotificationSystem.AddNotificationExtended(show_time_ext, title_text_ext, detail_text_ext, icon_ext);
					break;
				}
				
				
				case ERPCs.RPC_SOUND_HELICRASH:
				{
					bool playSound;
					vector pos;
					string sound_set;
					
					//Helicrash is a world event, we want anyone to be able to hear it
					Param3<bool, vector, int> playCrashSound = new Param3<bool, vector, int>(false, "0 0 0",0);
					if (ctx.Read(playCrashSound))
					{
						playSound = playCrashSound.param1;
						pos = playCrashSound.param2;
						sound_set = CrashSoundSets.GetSoundSetByHash(playCrashSound.param3);
					}
					
					if (playSound)
					{
						m_CrashSound = SEffectManager.PlaySound(sound_set, pos, 0.1, 0.1);
						m_CrashSound.SetAutodestroy(true);
					}
				
				break;
				}
				//Random off map artillery barrage
				case ERPCs.RPC_SOUND_ARTILLERY:
				{
					vector position;
					Param1<vector> playArtySound = new Param1<vector>(vector.Zero);
					if (ctx.Read(playArtySound))
					{
						position = playArtySound.param1;
						if (position == vector.Zero)
							break;
					}
					else
						break;
					
					if (!GetGame().GetPlayer())
						break;
					
					if (vector.DistanceSq(GetGame().GetPlayer().GetPosition(), position) <= (MIN_ARTY_SOUND_RANGE * MIN_ARTY_SOUND_RANGE))
						break;
					
					m_ArtySound = SEffectManager.PlaySound("Artillery_Distant_Barrage_SoundSet", position, 0.1, 0.1);
					m_ArtySound.SetAutodestroy(true);
				
				break;
				}
				case ERPCs.RPC_SOUND_CONTAMINATION:
				{
					vector soundPos;
					
					Param1<vector> playContaminatedSound = new Param1<vector>(vector.Zero);
					if (ctx.Read(playContaminatedSound))
					{
						soundPos = playContaminatedSound.param1;
						if (soundPos == vector.Zero)
							break;
					}
					else
						break;
					
					if (!GetGame().GetPlayer())
						break;
					
					EffectSound closeArtySound = SEffectManager.PlaySound("Artillery_Close_SoundSet", soundPos);
					closeArtySound.SetAutodestroy(true);
					
					// We add camera shake upon shell detonation
					float distance_to_player = vector.DistanceSq(soundPos, GetGame().GetPlayer().GetPosition());
					if (distance_to_player <= GameConstants.CAMERA_SHAKE_ARTILLERY_DISTANCE2)
					{
						float strength_factor = Math.InverseLerp(GameConstants.CAMERA_SHAKE_ARTILLERY_DISTANCE, 0, Math.Sqrt(distance_to_player));
						DayZPlayerCamera camera = GetGame().GetPlayer().GetCurrentCamera();
						if (camera)
							camera.SpawnCameraShake(strength_factor * 4);
					}
					
					ParticleManager.GetInstance().PlayInWorld(ParticleList.CONTAMINATED_AREA_GAS_SHELL, soundPos);
					break;
				}
				// Single artillery shot to announce dynamic contaminated area
				case ERPCs.RPC_SOUND_ARTILLERY_SINGLE:
				{
					vector soundPosition;
					vector delayedSoundPos;
					float soundDelay;
					
					Param3<vector, vector, float> playArtyShotSound = new Param3<vector, vector, float>(vector.Zero, vector.Zero, 0);
					if (ctx.Read(playArtyShotSound))
					{
						soundPosition = playArtyShotSound.param1;
						delayedSoundPos = playArtyShotSound.param2;
						soundDelay = playArtyShotSound.param3;
						if (soundPosition == vector.Zero)
							break;
					}
					else
						break;
					
					if (!GetGame().GetPlayer())
						break;
					
					m_ArtySound = SEffectManager.PlaySound("Artillery_Distant_SoundSet", soundPosition, 0.1, 0.1);
					m_ArtySound.SetAutodestroy(true);
					
					// We remove the amount of time the incoming sound lasts
					soundDelay -= 5;
					// We convert to milliseconds
					soundDelay *= 1000;
					GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DelayedMidAirDetonation, soundDelay, false, delayedSoundPos[0], delayedSoundPos[1], delayedSoundPos[2]);
					break;
				}
				case ERPCs.RPC_SET_BILLBOARDS:
				{
					if (!m_BillboardSetHandler)
						m_BillboardSetHandler = new BillboardSetHandler();
					
					Param1<int> indexP = new Param1<int>(-1);
					if (ctx.Read(indexP))
					{
						int index = indexP.param1;
						m_BillboardSetHandler.OnRPCIndex(index);
					}
					break;
				}
				#endif
				#endif

				case ERPCs.RPC_USER_SYNC_PERMISSIONS:
				{
					map<string, bool> mute_list;
					if (ctx.Read(mute_list))
					{
						for (int i = 0; i < mute_list.Count(); i++)
						{
							string uid = mute_list.GetKey(i);
							bool mute = mute_list.GetElement(i);
							MutePlayer(uid, sender.GetPlainId(), mute);
						}
					}
					break;
				}
				
				/*
				case ERPCs.RPC_SERVER_RESPAWN_MODE:
				{
					int mode;
					if (ctx.Read(mode) && !IsServer())
					{
						GetMission().SetRespawnModeClient(mode);
					}
				}
				*/

				#ifdef DEVELOPER
				case ERPCs.DEV_SET_WEATHER:
				{
					Param1<DebugWeatherRPCData> p1data = new Param1<DebugWeatherRPCData>(null);
					
					if ( ctx.Read(p1data) )
					{
						DebugWeatherRPCData data = p1data.param1;
						
						if (data.m_FogValue >= 0)
							GetGame().GetWeather().GetFog().Set(data.m_FogValue, data.m_FogInterpolation, data.m_FogDuration);
						
						if (data.m_OvercastValue >= 0)
							GetGame().GetWeather().GetOvercast().Set(data.m_OvercastValue, data.m_OvercastInterpolation, data.m_OvercastDuration);
						
						if (data.m_RainValue >= 0)
							GetGame().GetWeather().GetRain().Set(data.m_RainValue, data.m_RainInterpolation, data.m_RainDuration);
						
						if (data.m_SnowfallValue >= 0)
							GetGame().GetWeather().GetSnowfall().Set(data.m_SnowfallValue, data.m_SnowfallInterpolation, data.m_SnowfallDuration);
						
						if (data.m_VolFogDistanceDensity >= 0)
							GetGame().GetWeather().SetDynVolFogDistanceDensity(data.m_VolFogDistanceDensity, data.m_VolFogDistanceDensityTime);
						
						if (data.m_VolFogHeightDensity >= 0)
							GetGame().GetWeather().SetDynVolFogHeightDensity(data.m_VolFogHeightDensity, data.m_VolFogHeightDensityTime);
						
						if (data.m_VolFogHeightBias >= -500)
							GetGame().GetWeather().SetDynVolFogHeightBias(data.m_VolFogHeightBias, data.m_VolFogHeightBiasTime);
						
						if (data.m_WindMagnitudeValue >= 0)
							GetGame().GetWeather().GetWindMagnitude().Set(data.m_WindMagnitudeValue, data.m_WindDInterpolation, data.m_WindDDuration);
						
						if (data.m_WindDirectionValue >= -3.14)
							GetGame().GetWeather().GetWindDirection().Set(data.m_WindDirectionValue, data.m_WindDInterpolation, data.m_WindDDuration);
					}
					else
					{
						ErrorEx("Failed to read weather debug data");
					}
					break;
				}
				#endif
				
				
				#ifdef DIAG_DEVELOPER
				#ifdef SERVER
				case ERPCs.DIAG_CAMERATOOLS_CAM_DATA:
				{
					if (!m_CameraToolsMenuServer)
					{
						m_CameraToolsMenuServer = new CameraToolsMenuServer;
					}
					m_CameraToolsMenuServer.OnRPC(rpc_type, ctx);
					break;
				}
				
				case ERPCs.DIAG_CAMERATOOLS_CAM_SUBSCRIBE:
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
	
	void DelayedMidAirDetonation(float x, float y, float z)
	{
		EffectSound artilleryFallSound = SEffectManager.PlaySound("Artillery_Fall_SoundSet", Vector(x, y, z));
		artilleryFallSound.SetAutodestroy(true);
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
		return m_loading && m_loading.IsLoading();
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
		m_loading.ShowEx(this);
		#endif
	}
	
	// ------------------------------------------------------------
	void LoadingHide(bool force = false)
	{
		#ifndef NO_GUI
			m_loading.Hide(force);
			// turn the lights back on
			PPEManagerStatic.GetPPEManager().StopAllEffects();
			#ifdef PLATFORM_CONSOLE
			if (!IsLoading())
			{
				if (m_LoadState != DayZLoadState.MAIN_MENU_START && m_LoadState != DayZLoadState.MAIN_MENU_USER_SELECT)
				{
					UpdateInputDeviceDisconnectWarning();
				}
			}
			#endif
		#endif
	}

	// ------------------------------------------------------------
	override string CreateDefaultPlayer()
	{				
		if (m_CharClassNames.Count() > 0)
			return m_CharClassNames[0];

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
		if (IsServer())
		{
			//NoiseParams npar = new NoiseParams();
			m_NoiseParams.LoadFromPath(string.Format("cfgAmmo %1 NoiseExplosion", ammoType));
			
			float multiplier = hitInfo.GetSurfaceNoiseMultiplier();
			if (multiplier == 0)
				multiplier = 1;
			
			GetNoiseSystem().AddNoiseTarget(pos, 21, m_NoiseParams, multiplier * GetGame().GetWeather().GetNoiseReductionByWeather());
		}
	}
	
	// ------------------------------------------------------------
	void ExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal,
		float energyFactor, float explosionFactor, bool isWater, string ammoType)
	{
		#ifndef SERVER
		if (source)
		{
			if (GetGame().GetPlayer() == null)
				return;
			source.OnExplosionEffects(source, directHit, componentIndex, surface, pos, surfNormal, energyFactor, explosionFactor, isWater, ammoType);
			
			if (source.ShootsExplosiveAmmo() )
			{
				int particleID = AmmoTypesAPI.GetExplosionParticleID(ammoType, surface);
				if (particleID > -1)
				{
					ParticleManager.GetInstance().PlayInWorld(particleID, pos);
				}
			}
				
			
			float distance_to_player = vector.Distance(pos, GetGame().GetPlayer().GetPosition());
			m_AmmoShakeParams.Load(ammoType);
			
			if (distance_to_player < m_AmmoShakeParams.m_Radius)
			{
				float dist01 = Math.InverseLerp(0, m_AmmoShakeParams.m_Radius, distance_to_player);
				float modifier = Math.Lerp(m_AmmoShakeParams.m_ModifierClose, m_AmmoShakeParams.m_ModifierFar,dist01);
				
				GetGame().GetPlayer().GetCurrentCamera().SpawnCameraShake(modifier * m_AmmoShakeParams.m_Strength);
			}
		}
		#endif
	}
	
	// ------------------------------------------------------------
	void OnProjectileStopped(ProjectileStoppedInfo info)
	{
		string simulation;
		
		GetGame().ConfigGetText("cfgAmmo " + info.GetAmmoType() + " simulation", simulation);	

		if (simulation == "shotArrow")
		{
			string pile;
			
			GetGame().ConfigGetText("cfgAmmo " + info.GetAmmoType() + " spawnPileType", pile);
			
			EntityAI arrow = EntityAI.Cast(GetGame().CreateObjectEx(pile, info.GetPos(), ECE_DYNAMIC_PERSISTENCY));
			arrow.PlaceOnSurface();	
			arrow.SetFromProjectile(info);
		}
	}
	
	const float ARROW_PIERCE_DEPTH = 0.05;
	
	// ------------------------------------------------------------
	void OnProjectileStoppedInTerrain(TerrainCollisionInfo info)
	{
		string simulation;
		
		if (info.GetIsWater())
			return;
		
		GetGame().ConfigGetText("cfgAmmo " + info.GetAmmoType() + " simulation", simulation);
		if (simulation == "shotArrow")
		{
			string pile;
			GetGame().ConfigGetText("cfgAmmo " + info.GetAmmoType() + " spawnPileType", pile);
			vector pos = info.GetPos();
			vector dir = -info.GetInVelocity();
			
			dir.Normalize();
			pos -= dir * ARROW_PIERCE_DEPTH;
			
			EntityAI arrow = EntityAI.Cast(GetGame().CreateObjectEx(pile, pos, ECE_KEEPHEIGHT|ECE_DYNAMIC_PERSISTENCY));
			arrow.SetDirection(dir);
			arrow.SetFromProjectile(info);
		}
	}
	
	// ------------------------------------------------------------
	void OnProjectileStoppedInObject(ObjectCollisionInfo info)
	{
		string simulation;
		
		GetGame().ConfigGetText("cfgAmmo " + info.GetAmmoType() + " simulation", simulation);	
		if (simulation == "shotArrow")
		{
			string pile;
			GetGame().ConfigGetText("cfgAmmo " + info.GetAmmoType() + " spawnPileType", pile);
			
			EntityAI arrow = null;
			EntityAI ent = EntityAI.Cast(info.GetHitObj());
			if (ent)
			{
				EntityAI parent = ent.GetHierarchyParent();
				if (parent && parent.IsPlayer())
				{
					arrow = EntityAI.Cast(GetGame().CreateObjectEx(pile, parent.GetPosition(), ECE_DYNAMIC_PERSISTENCY));
					arrow.PlaceOnSurface();	
					arrow.SetFromProjectile(info);
	
					return;
				}
			}
			
			vector pos = info.GetPos();
			vector dir = -info.GetInVelocity();
			
			dir.Normalize();
			pos -= dir * ARROW_PIERCE_DEPTH;
			
			arrow = EntityAI.Cast(GetGame().CreateObjectEx(pile, pos, ECE_KEEPHEIGHT|ECE_DYNAMIC_PERSISTENCY));
			arrow.SetDirection(dir);
			arrow.SetFromProjectile(info);
			
			info.GetHitObj().AddArrow(arrow, info.GetComponentIndex(), info.GetHitObjPos(), info.GetHitObjRot());
		}
	}
	
	// ------------------------------------------------------------
	void FirearmEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal,
		 vector exitPos, vector inSpeed, vector outSpeed, bool isWater, bool deflected, string ammoType) 
	{
		#ifndef SERVER
		ImpactEffectsData impactEffectsData = new ImpactEffectsData();
		impactEffectsData.m_DirectHit 		= directHit;
		impactEffectsData.m_ComponentIndex 	= componentIndex;
		impactEffectsData.m_Surface 		= surface;
		impactEffectsData.m_Position		= pos;
		impactEffectsData.m_ImpactType		= ImpactTypes.UNKNOWN;
		impactEffectsData.m_SurfaceNormal	= surfNormal;
		impactEffectsData.m_ExitPosition	= exitPos;
		impactEffectsData.m_InSpeed			= inSpeed;
		impactEffectsData.m_OutSpeed		= outSpeed;
		impactEffectsData.m_IsDeflected		= deflected;
		impactEffectsData.m_AmmoType		= ammoType;
		impactEffectsData.m_IsWater			= isWater;

		if (directHit)
		{
			directHit.OnReceivedHit(impactEffectsData);
		}

		ImpactMaterials.EvaluateImpactEffectEx(impactEffectsData);
		#endif
		
		
		if (IsServer())
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
			m_NoiseParams.LoadFromPath("cfgAmmo " + ammoType + " NoiseHit");
			
			float surfaceCoef = SurfaceGetNoiseMultiplier(directHit, pos, componentIndex);
			float coefAdjusted = surfaceCoef * inSpeed.Length() / ConfigGetFloat("cfgAmmo " + ammoType + " initSpeed");
			if (coefAdjusted == 0)
				coefAdjusted = 1;
			
			GetNoiseSystem().AddNoiseTarget(pos, 10, m_NoiseParams, coefAdjusted * GetGame().GetWeather().GetNoiseReductionByWeather()); // Leave a ping for 5 seconds
		}
	}
	
	// ------------------------------------------------------------
	void CloseCombatEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal,
		 bool isWater, string ammoType) 
	{
		#ifndef SERVER
		ImpactEffectsData impactEffectsData = new ImpactEffectsData();
		impactEffectsData.m_DirectHit 		= directHit;
		impactEffectsData.m_ComponentIndex 	= componentIndex;
		impactEffectsData.m_Surface 		= surface;
		impactEffectsData.m_Position		= pos;
		impactEffectsData.m_ImpactType		= ImpactTypes.MELEE;
		impactEffectsData.m_SurfaceNormal	= Vector(Math.RandomFloat(-1,1), Math.RandomFloat(-1,1), Math.RandomFloat(-1,1));
		impactEffectsData.m_ExitPosition	= "0 0 0";
		impactEffectsData.m_InSpeed			= "0 0 0";
		impactEffectsData.m_OutSpeed		= "0 0 0";
		impactEffectsData.m_IsDeflected		= false;
		impactEffectsData.m_AmmoType		= ammoType;
		impactEffectsData.m_IsWater			= isWater;

		if (directHit)
			directHit.OnReceivedHit(impactEffectsData);
		
		ImpactMaterials.EvaluateImpactEffectEx(impactEffectsData);
		#endif
		
		// add hit noise
		if (IsServer())
		{
			m_NoiseParams.LoadFromPath("cfgAmmo " + ammoType + " NoiseHit");
			
			float surfaceCoef = SurfaceGetNoiseMultiplier(directHit, pos, componentIndex);
			if (surfaceCoef == 0)
				surfaceCoef = 1;
			
			GetNoiseSystem().AddNoisePos(EntityAI.Cast(source), pos, m_NoiseParams, surfaceCoef * GetGame().GetWeather().GetNoiseReductionByWeather());
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
		if (pFov < OPTIONS_FIELD_OF_VIEW_MIN)
			pFov = OPTIONS_FIELD_OF_VIEW_MIN;

		if (pFov > OPTIONS_FIELD_OF_VIEW_MAX)
			pFov = OPTIONS_FIELD_OF_VIEW_MAX;
		
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
		if (gameOptions && Class.CastTo(noa,gameOptions.GetOptionByType(OptionAccessType.AT_OPTIONS_FIELD_OF_VIEW)))
		{
			return noa.ReadValue();
		}
		return 1.0;
	}
	
	float GetFOVByZoomType(ECameraZoomType type)
	{
		switch (type)
		{
			case ECameraZoomType.NONE:
				return GetUserFOV();
			case ECameraZoomType.NORMAL:
				return Math.Min(GetUserFOV(), GameConstants.DZPLAYER_CAMERA_FOV_EYEZOOM);
			case ECameraZoomType.SHALLOW:
				return Math.Min(GetUserFOV(),GameConstants.DZPLAYER_CAMERA_FOV_EYEZOOM_SHALLOW);
			default:
				return GetUserFOV();
		}
		return GetUserFOV();
	}
	
	void SetHudBrightness(float value)
	{
		Widget.SetLV(value);
		Widget.SetTextLV(value);
	}
	
	float GetHUDBrightnessSetting()
	{
		return g_Game.GetProfileOptionFloat(EDayZProfilesOptions.HUD_BRIGHTNESS);
	}
	
	// Check if ammo is compatible with a weapon in hands 
	static bool CheckAmmoCompability(EntityAI weaponInHand, EntityAI ammo)
	{	
		TStringArray ammo_names = new TStringArray;										// Array of ammo types (their name) that can be used with weapon in hand
		
		string cfg_path = "CfgWeapons " + weaponInHand.GetType() + " chamberableFrom";	// Create config path 
		GetGame().ConfigGetTextArray(cfg_path, ammo_names);								// Get ammo types 
			
		foreach (string ammo_name : ammo_names)				// for every ammo in ammo string compare passed ammo 
		{
			if (ammo.GetType() == ammo_name)					
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
		language_option = ListOptionsAccess.Cast(options.GetOptionByType(OptionAccessType.AT_OPTIONS_LANGUAGE));
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
		#ifdef DIAG_DEVELOPER
		
		if (FeatureTimeAccel.GetFeatureTimeAccelEnabled(ETimeAccelCategories.FOOD_DECAY))
		{
			return m_FoodDecayModifier * FeatureTimeAccel.GetFeatureTimeAccelValue();
		}
		#endif
		return m_FoodDecayModifier;
	}
	
	array<int> GetConnectedInputDeviceList()
	{
		if (!m_ConnectedInputDeviceList)
		{
			m_ConnectedInputDeviceList = new array<int>;
		}
		return m_ConnectedInputDeviceList;
	}
	
	void SetMouseCursorDesiredVisibility(bool visible)
	{
		m_CursorDesiredVisibilityScript = visible;

		RefreshMouseCursorVisibility();
	}
	
	bool GetMouseCursorDesiredVisibility()
	{
		return m_CursorDesiredVisibilityScript;
	}
	
	//! extend as needed, only game focus and M&K setting (consoles only!) are checked natively
	bool CanDisplayMouseCursor()
	{
		//! Only checking on console because while loading on PC, the mouse might not be detected
#ifdef PLATFORM_CONSOLE
		if (GetInput())
		{
			return GetInput().IsMouseConnected();
		}
#endif

		//! Platform defaults
#ifdef PLATFORM_CONSOLE
		return false;
#else
		return true;
#endif
	}
	
	void RefreshMouseCursorVisibility()
	{
#ifndef NO_GUI
#ifdef FEATURE_CURSOR
		if (!IsAppActive())
		{
			ShowCursorWidget(true);
		}
		else
#endif
		{
			bool showCursor = m_CursorDesiredVisibilityScript && CanDisplayMouseCursor();
			
			UIManager ui = GetUIManager();
			if (ui)
			{
				//! Handles app active-ness
				ui.ShowCursor(showCursor);
			}
			else
			{
				//! Fallback, just in-case
				ShowCursorWidget(showCursor);
			}
		}
#endif
	}

	BillboardSetHandler GetBillboardHandler()
	{
		return m_BillboardSetHandler;
	}
	
	///////////////
	//DEPRECATED//
	//////////////
	void CreateGamepadDisconnectMenu();
	void DeleteGamepadDisconnectMenu();
};

DayZGame g_Game;

DayZGame GetDayZGame()
{
	return g_Game;
}
