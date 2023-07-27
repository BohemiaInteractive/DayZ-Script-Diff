class MainMenuDlcHandlerBase extends ScriptedWidgetEventHandler
{
	protected const string 	TEXT_OWNED = "#layout_dlc_owned";
	protected const string 	TEXT_UNOWNED = "#layout_dlc_unowned";
	
	protected bool 			m_FileLoaded;
	protected bool 			m_VideoPaused;
	protected int 			m_ColorBackgroundOriginal;
	
	protected Widget 		m_Root;
	protected Widget 		m_BannerFrame;
	protected Widget 		m_Background;
	protected Widget 		m_StoreButton;
	protected Widget 		m_GamepadStoreImage;
	protected TextWidget 	m_TitleTextDlc;
	protected TextWidget 	m_DescriptionTextDlc;
	protected VideoWidget 	m_VideoWidget;
	protected ref Timer		m_VideoPlayTimer; //...
	protected ref Timer		m_PlaybackStartTimer;
	protected ref ModInfo 	m_ThisModInfo;
	protected ref JsonDataDLCInfo 	m_DlcInfo;
	
	protected ref BannerHandlerBase m_BannerHandler;
	
	void MainMenuDlcHandlerBase(ModInfo info, Widget parent, JsonDataDLCInfo DlcInfo)
	{
		CreateRootWidget(parent);
		m_Root.SetHandler(this);
		m_DlcInfo = DlcInfo;
		m_ThisModInfo = info;
		Init();
		
		#ifdef PLATFORM_CONSOLE
		GetGame().GetContentDLCService().m_OnChange.Insert(OnDLCChange);
		if (GetGame().GetMission())
		{
			GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		}
		#endif
	}
	
	void ~MainMenuDlcHandlerBase()
	{
		#ifdef PLATFORM_CONSOLE
		if (GetGame().GetContentDLCService())
			GetGame().GetContentDLCService().m_OnChange.Remove(OnDLCChange);
		#endif
	}
	
	void Init()
	{
		m_Background = m_Root;
		m_StoreButton = m_Root.FindAnyWidget("dlc_openStore");
		#ifdef PLATFORM_XBOX
			m_GamepadStoreImage = m_Root.FindAnyWidget("image_button_xbox");
		#endif
		#ifdef PLATFORM_PS4
			m_GamepadStoreImage = m_Root.FindAnyWidget("image_button_ps");
		#endif
		m_VideoWidget = VideoWidget.Cast(m_Root.FindAnyWidget("dlc_Video"));
		m_VideoPlayTimer = new Timer();
		m_PlaybackStartTimer = new Timer();
		m_FileLoaded = false;
		m_VideoPaused = false;
		m_BannerFrame = m_Root.FindAnyWidget("dlc_BannerFrame");
		m_BannerHandler = new BannerHandlerBase(m_BannerFrame);
		m_TitleTextDlc = TextWidget.Cast(m_Root.FindAnyWidget("dlc_title"));
		m_DescriptionTextDlc = TextWidget.Cast(m_Root.FindAnyWidget("dlc_Description"));
		m_ColorBackgroundOriginal = m_Background.GetColor();
		
		UpdateOwnedStatus();
		SetDlcData();
	}
	
	void CreateRootWidget(Widget parent)
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/dlc_panels/DLC_Panel.layout", parent);
	}
	
	void ShowInfoPanel(bool show)
	{
		m_Root.Show(show);
		OnPanelVisibilityChanged();
	}
	
	bool IsInfoPanelVisible()
	{
		return m_Root.IsVisible();
	}
	
	void OnPanelVisibilityChanged()
	{
		if (IsInfoPanelVisible())
			StartVideo();
		else
			KillVideo();
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		m_ThisModInfo.GoToStore();
		return super.OnClick(w,x,y,button);
	}
	
	void SetDlcData()
	{
		m_TitleTextDlc.SetText(m_DlcInfo.HeaderText);
		m_DescriptionTextDlc.SetText(m_DlcInfo.DescriptionText);
	}
	
	void LoadVideoFile()
	{
		if (m_FileLoaded)
			return;
		
		string path;
		#ifdef PLATFORM_WINDOWS
			path = ".\\video\\" + m_DlcInfo.VideoFileName;
		#endif
		#ifdef PLATFORM_PS4
			path = "/app0/video/" + m_DlcInfo.VideoFileName;
		#endif
		#ifdef PLATFORM_XBOX
			path = "G:\\video\\" + m_DlcInfo.VideoFileName;
		#endif
		if (m_DlcInfo.VideoFileName != "")
			m_VideoWidget.LoadVideo(path,0);
		
		m_FileLoaded = true;
	}
	
	void CheckVideoLoopTimer()
	{
		if (!m_VideoWidget.Play(VideoCommand.ISPLAYING))
		{
			CheckFileLoaded();
			PerformPlayVideo();
		}
	}
	
	void CheckFileLoaded()
	{
		if (!m_VideoPaused)
		{
			m_FileLoaded = false;
		}
	}
	
	void PerformPlayVideo()
	{
		LoadVideoFile();
		//m_VideoWidget.Play(VideoCommand.REPEAT);
		m_VideoWidget.Play(VideoCommand.PLAY);
		m_VideoPaused = false;
	}
	
	void StartVideo()
	{
		PerformPlayVideo();
		m_VideoPlayTimer.Run(0.005, this, "CheckVideoLoopTimer", null, true);
	}
	
	void StopVideo()
	{
		m_VideoPaused = m_VideoWidget.Play(VideoCommand.ISPLAYING);
		CheckFileLoaded();
		LoadVideoFile(); //in case the video stopped at the exact spot
		m_VideoWidget.Play(VideoCommand.STOP);
		m_VideoPlayTimer.Stop();
	}
	
	void KillVideo()
	{
		m_VideoWidget.Play(VideoCommand.KILL);
		m_VideoPlayTimer.Stop();
		m_VideoPaused = false;
		m_FileLoaded = false;
		LoadVideoFile();
	}
	
	protected void ColorFocussed(Widget w, int x, int y)
	{
		m_Background.SetColor(ARGB(255,54,16,16));
	}
	
	protected void ColorUnfocussed(Widget w, Widget enterW, int x, int y)
	{
		m_Background.SetColor(m_ColorBackgroundOriginal);
	}
	
	protected void UpdateOwnedStatus()
	{
		if (m_ThisModInfo)
		{
			if (m_ThisModInfo.GetIsOwned())
			{
				m_BannerHandler.SetBannerColor(Colors.COLOR_LIVONIA_EMERALD_GREEN);
				m_BannerHandler.SetBannerText(TEXT_OWNED);
			}
			else
			{
				m_BannerHandler.SetBannerColor(Colors.COLOR_DAYZ_RED);
				m_BannerHandler.SetBannerText(TEXT_UNOWNED);
			}
		}
	}
	
	protected void OnDLCChange()
	{
		UpdateOwnedStatus();
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		m_GamepadStoreImage.Show(!GetGame().GetInput().IsEnabledMouseAndKeyboard() || pInputDeviceType == EInputDeviceType.CONTROLLER);
	}
	
	ModInfo GetModInfo()
	{
		return m_ThisModInfo;
	}
}

