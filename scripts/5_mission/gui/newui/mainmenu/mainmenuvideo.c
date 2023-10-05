class MainMenuVideo extends UIScriptedMenu
{
	protected string 				m_BackButtonTextID;
	
	protected VideoWidget			m_Video;
	
	override Widget Init()
	{
		layoutRoot 				= GetGame().GetWorkspace().CreateWidgets("gui/layouts/xbox/video_menu.layout");
		m_Video					= VideoWidget.Cast(layoutRoot.FindAnyWidget("video"));
		
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		
		m_Video.Load("video\\DayZ_onboarding_MASTER.mp4");
			
		m_Video.Play();
		
		m_Video.SetCallback(VideoCallback.ON_END, StopVideo);
		
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
	
	void StopVideo()
	{
		if (m_Video)
		{
			m_Video.Unload();
			GetGame().GetUIManager().Back();
		}
	}
	
	void PlayPauseVideo()
	{
		if (m_Video)
		{
			if (m_Video.IsPlaying())
			{
				m_Video.Pause();
			}
			else
			{
				m_Video.Play();
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
			m_Video.Unload();
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