class MainMenuVideo extends UIScriptedMenu
{
	protected string 				m_BackButtonTextID;
	
	protected VideoWidget			m_Video;
	protected ref Timer				m_VideoPlayTimer;
	protected ref WidgetFadeTimer	m_VideoFadeTimer;
	bool							m_IsPaused;
	
	override Widget Init()
	{
		layoutRoot 				= GetGame().GetWorkspace().CreateWidgets("gui/layouts/xbox/video_menu.layout");
		m_Video					= VideoWidget.Cast(layoutRoot.FindAnyWidget("video"));
		
		m_VideoPlayTimer		= new Timer();
		m_VideoFadeTimer		= new WidgetFadeTimer();
		
		#ifdef PLATFORM_PS4
			m_Video.LoadVideo("/app0/video/DayZ_onboarding_MASTER.mp4", 0);
		#else
			m_Video.LoadVideo("G:\\video\\DayZ_onboarding_MASTER.mp4", 0);
		#endif
		m_Video.Play(VideoCommand.REWIND);
		m_Video.Play(VideoCommand.PLAY);
		m_VideoFadeTimer.FadeIn(m_Video, 1.5);
		m_VideoPlayTimer.Run(0.005, this, "PlayVideoLoop", null, true);
		
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		
		return layoutRoot;
	}
	
	void ~MainMenuVideo()
	{
	}
	
	//after show
	override void OnShow()
	{
		super.OnShow();
		GetGame().GetUIManager().ShowUICursor(false);
		GetGame().GetSoundScene().SetSoundVolume(0.0,0.0);
		
		UpdateControlsElements();
	}

	//after hide
	override void OnHide()
	{
		super.OnHide();
		GetGame().GetUIManager().ShowUICursor(true);
		GetGame().GetSoundScene().SetSoundVolume(1.0,1.0);
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		UpdateControlsElements();
	}
	
	void PlayVideoLoop()
	{
		if (m_Video && !m_IsPaused && !m_Video.Play(VideoCommand.ISPLAYING))
		{
			StopVideo();
		}
	}
	
	void StopVideo()
	{
		if (m_Video)
		{
			m_VideoPlayTimer.Stop();
			m_Video.Play(VideoCommand.KILL);
			GetGame().GetUIManager().Back();
		}
	}
	
	void PlayPauseVideo()
	{
		if (m_Video)
		{
			if (m_Video.Play(VideoCommand.ISPLAYING))
			{
				m_Video.Play(VideoCommand.STOP);
				m_IsPaused = true;
			}
			else
			{
				m_Video.Play(VideoCommand.PLAY);
				m_IsPaused = false;
			}
		}
	}
	
	override void Update(float timeslice)
	{
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			StopVideo();
		}
	}
	
	override void OnVisibilityChanged(bool isVisible)
	{
		if (!isVisible)
		{
			m_Video.Play(VideoCommand.KILL);
		}
	}
	
	//Deprecated
	protected void LoadFooterButtonTexts()
	{
	}
	
	//Deprecated
	protected void LoadTextStrings()
	{
	}
	
	protected void UpdateControlsElements()
	{
		RichTextWidget toolbar_text = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ContextToolbarText"));
		string text;
		
		if (GetGame().GetInput().IsEnabledMouseAndKeyboard() && GetGame().GetInput().GetCurrentInputDevice() == EInputDeviceType.MOUSE_AND_KEYBOARD)
		{
			text = "ESC " + "#menu_back";
		}
		else
		{
			text = string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#menu_back", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		}
		
		toolbar_text.SetText(text);
	}
}