class VideoPlayer extends ScriptedWidgetEventHandler
{
	protected Widget 		m_Root;
	
	protected ButtonWidget	m_PlayButton;
	protected ButtonWidget	m_PauseButton;
	
	protected ButtonWidget	m_StopButton;
	
	protected ButtonWidget	m_OnceButton;
	protected ButtonWidget	m_RepeatButton;
	
	protected ButtonWidget		m_LoadButton;
	protected GridSpacerWidget	m_LoadVideo;
	
	protected SliderWidget 	m_Progress;
	
	protected TextWidget	m_CurrentTime;
	protected TextWidget	m_TotalTime;
	
	protected ImageWidget 		m_Buffering;
	/*protected*/ VideoWidget 	m_VideoWidget;
	
	void VideoPlayer(Widget parent)
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/video_player.layout", parent);
		m_Root.SetHandler(this);
		m_Root.SetSort(333);
		Init();
	}
	
	void ~VideoPlayer()
	{
	}
	
	void Show(bool show)
	{
		m_Root.Show(show);
	}
	
	private void Init()
	{
		m_PlayButton 		= ButtonWidget.Cast(m_Root.FindAnyWidget("vp_PlayButton"));
		m_PauseButton 		= ButtonWidget.Cast(m_Root.FindAnyWidget("vp_PauseButton"));
		m_StopButton 		= ButtonWidget.Cast(m_Root.FindAnyWidget("vp_StopButton"));
		
		m_OnceButton 		= ButtonWidget.Cast(m_Root.FindAnyWidget("vp_OnceButton"));
		m_RepeatButton 		= ButtonWidget.Cast(m_Root.FindAnyWidget("vp_RepeatButton"));
		
		m_LoadButton 		= ButtonWidget.Cast(m_Root.FindAnyWidget("vp_LoadButton"));
		m_LoadVideo 		= GridSpacerWidget.Cast(m_Root.FindAnyWidget("vp_LoadVideo"));
		m_LoadVideo.Show(false);
		
		m_Progress 			= SliderWidget.Cast(m_Root.FindAnyWidget("vp_Progress"));
		m_Progress.SetCurrent(0);
		
		m_CurrentTime 		= TextWidget.Cast(m_Root.FindAnyWidget("vp_CurrentTime"));
		m_TotalTime 		= TextWidget.Cast(m_Root.FindAnyWidget("vp_TotalTime"));
		
		m_Buffering			= ImageWidget.Cast(m_Root.FindAnyWidget("vp_Buffering"));
		m_Buffering.Show(false);
		m_VideoWidget 		= VideoWidget.Cast(m_Root.FindAnyWidget("vp_Video"));
		
		m_VideoWidget.SetCallback(VideoCallback.ON_PLAY, OnPlaybackStart);
		m_VideoWidget.SetCallback(VideoCallback.ON_PAUSE, OnPlaybackStop);
		m_VideoWidget.SetCallback(VideoCallback.ON_STOP, OnPlaybackStop);
		m_VideoWidget.SetCallback(VideoCallback.ON_END, OnPlaybackStop);
		m_VideoWidget.SetCallback(VideoCallback.ON_LOAD, OnPlaybackStop);
		m_VideoWidget.SetCallback(VideoCallback.ON_SEEK, UpdateCurrentTime);
		m_VideoWidget.SetCallback(VideoCallback.ON_BUFFERING_START, OnBufferingStart);
		m_VideoWidget.SetCallback(VideoCallback.ON_BUFFERING_END, OnBufferingEnd);
	}
	
	private void InitVideoLoading()
	{	
		string path = "video\\*";
		
		string fileName;
		FileAttr fileAttr;
		
		FindFileHandle handle = FindFile(path, fileName, fileAttr, FindFileFlags.DIRECTORIES);
		
		if (fileName != "")
		{
			CreateVideoLoadingEntry(fileName);
		}
		
		while (FindNextFile(handle, fileName, fileAttr))
		{
			CreateVideoLoadingEntry(fileName);		
		}
		
		CloseFindFile(handle);
	}
	
	private void CreateVideoLoadingEntry(string entryName)
	{	
		Widget entry = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/video_player_entry.layout", m_LoadVideo);
		ButtonWidget entryButton = ButtonWidget.Cast(entry.GetChildren());
		entryButton.SetText(entryName);
		entryButton.SetUserID(333);
	}
	
	private void UpdateCurrentTime()
	{
		int time = m_VideoWidget.GetTime();
		UpdateTime(m_CurrentTime, time);
		
		m_Progress.SetCurrent(time);
	}
	
	// This can be an async op
	private void UpdateTotalTime()
	{
		int time = m_VideoWidget.GetTotalTime();
		
		if (time != 0)
		{
			UpdateTime(m_TotalTime, time);
			m_Progress.SetMinMax(0, time);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(UpdateTotalTime);
		}
	}
	
	private void UpdateTime(TextWidget widget, int time)
	{
		FullTimeData timeData = new FullTimeData();
		TimeConversions.ConvertSecondsToFullTime(time / 1000, timeData);
		widget.SetText(timeData.FormatedAsTimestamp());
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w == m_Progress)
		{
			m_VideoWidget.SetTime(m_Progress.GetCurrent(), finished);
		}
		
		return super.OnChange(w, x, y, finished);
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_PlayButton)
		{
			PlayVideo();
		}
		else if (w == m_PauseButton)
		{
			PauseVideo();
		}
		else if (w == m_StopButton)
		{
			StopVideo();
		}
		else if (w == m_OnceButton)
		{
			OnceVideo();
		}
		else if (w == m_RepeatButton)
		{
			RepeatVideo();
		}
		else if (w == m_LoadButton)
		{
			ToggleVideoSelection();
		}
		else if (w == m_Progress)
		{
			Print(x);
			Print(y);
			Print(button);
		}
		else if (w.GetUserID() == 333)
		{
			string name;
			ButtonWidget.Cast(w).GetText(name);
			LoadVideo(name);
			ToggleVideoSelection();
		}
		
		return super.OnClick(w, x, y, button);
	}
	
	protected void OnPlaybackStart()
	{
		m_PlayButton.Show(false);
		m_PauseButton.Show(true);
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UpdateCurrentTime, 0, true);
	}
	
	protected void OnPlaybackStop()
	{
		m_PlayButton.Show(true);
		m_PauseButton.Show(false);
		
		UpdateCurrentTime();
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(UpdateCurrentTime);
	}
	
	protected void OnBufferingStart()
	{
		m_Buffering.Show(true);
	}
	
	protected void OnBufferingEnd()
	{
		m_Buffering.Show(false);
	}
	
	void ToggleVideoSelection()
	{
		if (!m_LoadVideo.IsVisible())
		{
			InitVideoLoading();
		}
		else
		{
			Widget child = m_LoadVideo.GetChildren();
		
			while (child)
			{
				Widget c = child;
				child = child.GetSibling();
				c.Unlink();
			}
		}
		
		m_LoadVideo.Show(!m_LoadVideo.IsVisible());
	}
	
	void LoadVideo(string videoPath)
	{	
		string path;
		#ifdef PLATFORM_WINDOWS
			path = ".\\video\\";
		#endif
		#ifdef PLATFORM_PS4
			path = "/app0/video/";
		#endif
		#ifdef PLATFORM_XBOX
			path = "G:\\video\\";
		#endif
		
		m_VideoWidget.Load(path + videoPath, m_VideoWidget.IsLooping());
			
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UpdateTotalTime, 0, true);
	}
	
	void PlayVideo()
	{
		m_VideoWidget.Play();
	}
	
	void PauseVideo()
	{
		m_VideoWidget.Pause();
	}
	
	void StopVideo()
	{
		m_VideoWidget.Stop();
		m_PlayButton.Show(true);
		m_PauseButton.Show(false);
	}
	
	void OnceVideo()
	{
		m_VideoWidget.SetLooping(false);
		m_OnceButton.Show(false);
		m_RepeatButton.Show(true);
	}
	
	void RepeatVideo()
	{
		m_VideoWidget.SetLooping(true);
		m_RepeatButton.Show(false);
		m_OnceButton.Show(true);
	}
	
	void KillVideo()
	{
		m_VideoWidget.Unload();
	}
}
