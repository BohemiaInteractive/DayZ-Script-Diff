class MainMenuVideo extends UIScriptedMenu
{
	protected string 				m_BackButtonTextID;
	
	protected TextWidget			m_PlayPauseText;
	protected VideoWidget			m_Video;
	protected ref Timer				m_VideoPlayTimer;
	protected ref WidgetFadeTimer	m_VideoFadeTimer;
	bool							m_IsPaused;
	
	override Widget Init()
	{
		layoutRoot 				= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/xbox/video_menu.layout" );
		m_Video					= VideoWidget.Cast( layoutRoot.FindAnyWidget( "video" ) );
		m_PlayPauseText			= TextWidget.Cast( layoutRoot.FindAnyWidget( "PauseVideoText" ) );
		
		m_VideoPlayTimer		= new Timer();
		m_VideoFadeTimer		= new WidgetFadeTimer();
		
		layoutRoot.FindAnyWidget("ConsoleToolbar").Show(true);
		RichTextWidget toolbar_b = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		
		#ifdef PLATFORM_PS4
			m_Video.LoadVideo( "/app0/video/DayZ_onboarding_MASTER.mp4", 0 );
		#else
			m_Video.LoadVideo( "G:\\video\\DayZ_onboarding_MASTER.mp4", 0 );
		#endif
		m_Video.Play( VideoCommand.REWIND );
		m_Video.Play( VideoCommand.PLAY );
		m_VideoFadeTimer.FadeIn( m_Video, 1.5 );
		m_VideoPlayTimer.Run( 0.005, this, "PlayVideoLoop", null, true );
		
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
	}

	//after hide
	override void OnHide()
	{
		super.OnHide();
		GetGame().GetUIManager().ShowUICursor(true);
	}
	
	void PlayVideoLoop()
	{
		if( m_Video && !m_IsPaused && !m_Video.Play( VideoCommand.ISPLAYING ) )
		{
			StopVideo();
		}
	}
	
	void StopVideo()
	{
		if( m_Video )
		{
			m_VideoPlayTimer.Stop();
			m_Video.Play( VideoCommand.KILL );
			GetGame().GetUIManager().Back();
		}
	}
	
	void PlayPauseVideo()
	{
		if( m_Video )
		{
			if( m_Video.Play( VideoCommand.ISPLAYING ) )
			{
				m_Video.Play( VideoCommand.STOP );
				//m_PlayPauseText.SetText( "Play" );
				m_IsPaused = true;
			}
			else
			{
				m_Video.Play( VideoCommand.PLAY );
				//m_PlayPauseText.SetText( "Pause" );
				m_IsPaused = false;
			}
		}
	}
	
	override void Update(float timeslice)
	{
		if( GetGame().GetInput().LocalPress("UAUICtrlX",false) )
		{
			//PlayPauseVideo();
		}
		if( GetGame().GetInput().LocalPress("UAUIBack",false) )
		{
			StopVideo();
		}
	}
	
	override void OnVisibilityChanged(bool isVisible)
	{
		if ( !isVisible )
		{
			m_Video.Play( VideoCommand.KILL );
		}
	}
	
	/// Initial texts load for the footer buttons
	protected void LoadFooterButtonTexts()
	{
		TextWidget uiBackText 	= TextWidget.Cast(layoutRoot.FindAnyWidget( "BackText" ));		
		
		if (uiBackText)
		{
			uiBackText.SetText(m_BackButtonTextID);
		}
	}
	/// Set correct bottom button texts based on platform (ps4 vs xbox texts)
	protected void LoadTextStrings()
	{
		#ifdef PLATFORM_PS4
			m_BackButtonTextID = "ps4_ingame_menu_back";
		#else 
			m_BackButtonTextID = "STR_rootFrame_toolbar_bg_ConsoleToolbar_Back_BackText0";	
		#endif
	}
	
}