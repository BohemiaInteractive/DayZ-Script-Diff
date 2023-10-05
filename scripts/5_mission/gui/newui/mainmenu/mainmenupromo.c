//!base class for promo implementation
class MainMenuDlcHandlerBase extends ScriptedWidgetEventHandler
{
	protected const string 	TEXT_OWNED = "#layout_dlc_owned";
	protected const string 	TEXT_UNOWNED = "#layout_dlc_unowned";
	
	protected int 			m_ColorBackgroundOriginal;
	
	protected Widget 		m_Root;
	protected Widget 		m_BannerFrame;
	protected Widget 		m_Background;
	protected Widget 		m_StoreButton;
	protected Widget 		m_GamepadStoreImage;
	protected ImageWidget 	m_DlcPromotionImage;
	protected TextWidget 	m_TitleTextDlc;
	protected MultilineTextWidget 	m_DescriptionTextDlc;
	protected VideoWidget 	m_VideoWidget;
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
		m_VideoWidget.Show(false);
		m_DlcPromotionImage = ImageWidget.Cast(m_Root.FindAnyWidget("dlc_ImageMain"));
		m_DlcPromotionImage.Show(true);
		m_BannerFrame = m_Root.FindAnyWidget("dlc_BannerFrame");
		m_BannerHandler = new BannerHandlerBase(m_BannerFrame);
		m_TitleTextDlc = TextWidget.Cast(m_Root.FindAnyWidget("dlc_title"));
		m_DescriptionTextDlc = MultilineTextWidget.Cast(m_Root.FindAnyWidget("dlc_Description"));
		m_DescriptionTextDlc.SetLineBreakingOverride(LinebreakOverrideMode.LINEBREAK_WESTERN);
		m_ColorBackgroundOriginal = m_Background.GetColor();
		
		UpdateAllPromotionInfo();
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
		UpdateAllPromotionInfo();
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		m_ThisModInfo.GoToStore();
		return super.OnClick(w,x,y,button);
	}
	
	void LoadVideoFile()
	{
		if (m_VideoWidget.GetState() != VideoState.NONE)
			return;
		
		string path = "video\\" + m_DlcInfo.VideoFileName;
		
		if (m_DlcInfo.VideoFileName != "")
			m_VideoWidget.Load(path, true);
	}
	
	void PerformPlayVideo()
	{
		LoadVideoFile();
		m_VideoWidget.Play();
	}
	
	void StartVideo()
	{
		PerformPlayVideo();
	}
	
	void StopVideo()
	{
		m_VideoWidget.Stop();
	}
	
	void KillVideo()
	{
		m_VideoWidget.Unload();
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
	
	//updates on language change etc.
	protected void UpdateAllPromotionInfo()
	{
		UpdateDlcData();
		UpdateOwnedStatus();
		UpdateIconVisibility();
	}
	
	protected void UpdateDlcData()
	{
		m_TitleTextDlc.SetText(m_DlcInfo.HeaderText);
		m_DescriptionTextDlc.SetText(m_DlcInfo.DescriptionText);
	}
	
	protected void UpdateIconVisibility()
	{
		#ifdef PLATFORM_CONSOLE
		m_GamepadStoreImage.Show(!GetGame().GetInput().IsEnabledMouseAndKeyboard() || GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER);
		#endif
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		UpdateIconVisibility();
	}
	
	ModInfo GetModInfo()
	{
		return m_ThisModInfo;
	}
}
