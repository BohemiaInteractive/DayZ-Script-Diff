class VONManagerBase : Managed
{
	protected bool					m_VoNToggled;
	ref 	  ScriptInvoker			m_OnVonStateEvent;
	ref 	  ScriptInvoker			m_OnPartyChatChangedEvent;
	
	void VONManagerBase()
	{
		m_VoNToggled = false;
	}
	
	void HideVoiceNotification();
	void ShowVoiceNotification(int level, bool fading);
	void HandleInput(Input inp);
	void OnVOIPThresholdChanged();
	void OnEvent(EventType eventTypeId, Param params);
	
	bool IsVonToggled()
	{
		return m_VoNToggled;
	}
}

class VONManagerImplementation : VONManagerBase
{
	void VONManagerImplementation()
	{
		m_OnVonStateEvent = new ScriptInvoker();
		m_OnPartyChatChangedEvent = new ScriptInvoker();
	}
	
	void ~VONManagerImplementation()
	{
		m_OnVonStateEvent.Clear();
		m_OnPartyChatChangedEvent.Clear();
	}
	
	/**
	\brief Hides the VON notification completely and immediately
	*/
	override void HideVoiceNotification()
	{
		if (g_Game.IsMissionMainMenu())
		{
			return;
		}
		
		Mission mission = g_Game.GetMission();
		mission.GetMicrophoneIcon().Show(false);
		mission.HideVoiceLevelWidgets();
	}
	
	/**
	\brief Shows the voice notification
		\param level the loudness of the player voice. 0 = whisper, 1 = normal, 2 = shout
		\param fading specifies whether icon should slowly fade to invisibility after being displayed
	*/
	override void ShowVoiceNotification(int level, bool fading)
	{
		if (g_Game.IsMissionMainMenu())
		{
			return;
		}
		
		Mission mission = g_Game.GetMission();
		ImageWidget micIcon = mission.GetMicrophoneIcon();
		WidgetFadeTimer micTimer = mission.GetMicWidgetFadeTimer();
		map<int,ImageWidget> voiceLeveWidgets = mission.GetVoiceLevelWidgets();
		map<int,ref WidgetFadeTimer> voiceLevelTimers = mission.GetVoiceLevelTimers();
		
		// microphone icon
		micTimer.Stop();
		micIcon.SetAlpha(1.0); 
		micIcon.Show(true);
		
		if (fading)
		{
			micTimer.FadeOut(micIcon, 3.0);
		}
		
		// range icons
		for( int n = 0; n < voiceLeveWidgets.Count(); n++ )
		{
			int voiceKey = voiceLeveWidgets.GetKey(n);
			ImageWidget voiceWidget = voiceLeveWidgets.Get(n);
			
			// stop fade timer since it will be refreshed
			WidgetFadeTimer timer = voiceLevelTimers.Get(n);		
			timer.Stop();
		
			// show widgets according to the level
			if ( voiceKey <= level )
			{
				voiceWidget.SetAlpha(1.0); // reset from possible previous fade out 
				voiceWidget.Show(true);
				
				if (fading)
				{
					timer.FadeOut(voiceWidget, 3.0);
				}
			}
			else
			{
				voiceWidget.Show(false);
			}
		}
	}
	
	/**
	\brief Handles some VON related input
		\param inp input to handle
	*/
	override void HandleInput(Input inp)
	{
#ifdef PLATFORM_MSSTORE
		// ignore VON-related input if user is in an xbox party
		if (GetGame().IsInPartyChat())
		{
			return;
		}
#endif
#ifdef PLATFORM_XBOX
		// ignore VON-related input if user is in an xbox party
		if (g_Game.IsInPartyChat())
		{
			return;
		}
#endif
#ifdef PLATFORM_MSSTORE
		// ignore VON-related input if user is in an xbox party
		if (g_Game.IsInPartyChat())
		{
			return;
		}
#endif

		int oldLevel = g_Game.GetVoiceLevel();
		if (oldLevel == -1) //VoN system not initialized!
			return;
				
		int newLevel = -1;
		
		if (inp.LocalPress_ID(UAVoiceDistanceUp,false))
		{
			newLevel = ( oldLevel + 1 ) % ( VoiceLevelShout + 1 );
		}
		
		if (inp.LocalPress_ID(UAVoiceDistanceDown,false))
		{
			newLevel = oldLevel - 1;
			if (newLevel < VoiceLevelWhisper) //nah...
			{
				newLevel = VoiceLevelShout;
			}
		}
		
		if (newLevel > -1)
		{
			g_Game.SetVoiceLevel(newLevel);
			if (g_Game.GetMission().IsVoNActive()) // icon is already visible, just update the range
			{
				UpdateVoiceIcon();
			}
			else // Show the icon and let it fade out
			{
				int level = g_Game.GetVoiceLevel();
				ShowVoiceNotification(level, true);
			}
		}
	}
	
	private void UpdateVoiceIcon()
	{
		Mission mission = g_Game.GetMission();
		int rangeLevel = g_Game.GetVoiceLevel();
			
		if (mission.IsVoNActive())
		{
			if (m_VoNToggled)
			{
				if (VONManager.IsVoiceThresholdMinimum())
				{
					ShowVoiceNotification(rangeLevel, false);
				}
				else
				{
					ShowVoiceNotification(rangeLevel, true);
				}
			}
			else
			{
				ShowVoiceNotification(rangeLevel, false);
			}
		}
		else
		{
			HideVoiceNotification();
		}
	}
	
	/**
	\brief Fires every time VOIP threshold value changes
	*/
	override void OnVOIPThresholdChanged()
	{		
		UpdateVoiceIcon();
	}
	
	/**
	\brief Handles VON-related events
		\param eventTypeId event that fired
		\param params event-specific parameters
	*/
	override void OnEvent(EventType eventTypeId, Param params)
	{
		Mission mission = g_Game.GetMission();
		switch (eventTypeId)
		{
			case VONUserStartedTransmittingAudioEventTypeID:
			{
				// only handle this if we are in Voice Activation mode, so ignore if in PTT mode
				if (m_VoNToggled)
				{
					if (!VONManager.IsVoiceThresholdMinimum())
					{
						ShowVoiceNotification(g_Game.GetVoiceLevel(), false);
					}
				}
				break;
			}
			
			case VONUserStoppedTransmittingAudioEventTypeID:
			{
				// only handle this if we are in Voice Activation mode, so ignore if in PTT mode
				if (m_VoNToggled)
				{
					if (!VONManager.IsVoiceThresholdMinimum())
					{
						HideVoiceNotification();
					}
				}
				break;
			}
			
			case VONStateEventTypeID:
			{
				if (!mission)
				{
					break;
				}
				
				VONStateEventParams vonStateParams = VONStateEventParams.Cast( params );
				mission.SetVoNActive(vonStateParams.param1);
				m_VoNToggled = vonStateParams.param2;
				
				UpdateVoiceIcon();
				
				m_OnVonStateEvent.Invoke();
				break;
			}
			
			case PartyChatStatusChangedEventTypeID:
			{
				m_OnPartyChatChangedEvent.Invoke();
				break;
			}
			
			case VONStartSpeakingEventTypeID:
			{
				VONStartSpeakingEventParams vonStartParams;
				if (Class.CastTo(vonStartParams, params))
				{
					GetDayZGame().AddVoiceNotification(vonStartParams);
				}
				break;
			}
			
			case VONStopSpeakingEventTypeID:
			{
				VONStopSpeakingEventParams vonStopParams;
				if (Class.CastTo(vonStopParams, params))
				{
					GetDayZGame().RemoveVoiceNotification(vonStopParams);
				}
				break;
			}
			
			case MPSessionPlayerReadyEventTypeID:
			{
				UpdateVoiceIcon();
				break;
			}	
		}
	}
}

//! Manager class which handles Voice-over-network functionality while player is connected to a server
class VONManager
{	
	private static ref VONManagerBase m_VONManager = new VONManagerBase();

	/**
	\brief Main way to access VONManager functionality from script
		\return \p Instance of VONManagerImplementation if logged on to server, or VONManagerBase otherwise
	*/
	static VONManagerBase GetInstance()
	{
		return m_VONManager;
	}
	
	/**
	\brief Initializes VONManager, runs when user first connects to a server
	*/
	static void Init()
	{
		delete m_VONManager;
		m_VONManager = new VONManagerImplementation();
	}
	
	/**
	\brief Uninitializes VONManager, runs when user disconnects from server
	*/
	static void CleanupInstance()
	{
		delete m_VONManager;
		m_VONManager = new VONManagerBase();
	}
	
	/**
	\brief Specifies whether VON mode is toggled or not
		\return \p True if in Voice Activation mode, False if in Push-to-Talk mode
	*/
	static bool IsVONToggled()
	{
		return m_VONManager.IsVonToggled();
	}
	
	/**
	\brief Specifies whether user's voice activation threshold value is equal to the minimum voice activation threshold value
		\return \p True if threshold minimum, false otherwise
	*/
	static bool IsVoiceThresholdMinimum()
	{
		GameOptions gameOptions = new GameOptions();
		NumericOptionsAccess noa;
		Class.CastTo(noa, gameOptions.GetOptionByType( OptionAccessType.AT_OPTIONS_VON_THRESHOLD_SLIDER ));

		return noa.ReadValue() <= g_Game.GetSoundScene().GetSilenceThreshold();
	}
}
