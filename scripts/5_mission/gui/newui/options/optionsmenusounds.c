class OptionsMenuSounds extends ScriptedWidgetEventHandler
{
	protected Widget						m_Root;
	
	protected Widget						m_SettingsRoot;
	protected Widget						m_DetailsRoot;
	protected TextWidget					m_DetailsLabel;
	protected RichTextWidget				m_DetailsText;
	
	protected ref NumericOptionsAccess		m_MasterOption;
	protected ref NumericOptionsAccess		m_EffectsOption;
	protected ref NumericOptionsAccess		m_VOIPOption;
	protected ref NumericOptionsAccess  	m_VOIPThresholdOption;
	protected ref NumericOptionsAccess		m_MusicOption;
	protected ref ListOptionsAccess			m_InputModeOption;
	
	protected ref OptionSelectorSlider		m_MasterSelector;
	protected ref OptionSelectorSlider		m_EffectsSelector;
	protected ref OptionSelectorSlider		m_VOIPSelector;
	protected ref OptionSelectorLevelMarker	m_VOIPThresholdSelector;
	protected ref OptionSelectorSlider		m_MusicSelector;
	protected ref OptionSelectorMultistate	m_InputModeSelector;
	
	protected ref Timer 					m_AudioLevelTimer;
	protected GameOptions					m_Options;
	protected OptionsMenu					m_Menu;
	protected MissionGameplay				m_MissionGameplay;
	protected VONManagerBase				m_VonManager;
	
	protected ref map<int, ref Param2<string, string>> m_TextMap;
	
	private bool 							m_WasMicCapturing;
	
	void OptionsMenuSounds(Widget parent, Widget details_root, GameOptions options, OptionsMenu menu)
	{
		m_Root					= GetGame().GetWorkspace().CreateWidgets(GetLayoutName(), parent);
		m_DetailsRoot			= details_root;
		m_DetailsLabel			= TextWidget.Cast( m_DetailsRoot.FindAnyWidget("details_label"));
		m_DetailsText			= RichTextWidget.Cast( m_DetailsRoot.FindAnyWidget("details_content"));	
		m_Options				= options;
		m_Menu					= menu;
		
		m_MasterOption			= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_MASTER_VOLUME));
		m_EffectsOption			= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_EFFECTS_SLIDER));
		m_MusicOption			= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_MUSIC_SLIDER));
		m_VOIPOption			= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_VON_SLIDER));
		m_VOIPThresholdOption	= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_VON_THRESHOLD_SLIDER));
		m_InputModeOption		= ListOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_VON_INPUT_MODE));
		
		m_MissionGameplay		= MissionGameplay.Cast(GetGame().GetMission());
		m_VonManager 			= VONManager.GetInstance();
		m_AudioLevelTimer = new Timer();
		m_AudioLevelTimer.Run(0.1, this, "UpdateAudioLevel", NULL, true); 
		
		m_Root.FindAnyWidget( "master_setting_option" ).SetUserID(OptionAccessType.AT_OPTIONS_MASTER_VOLUME);
		m_Root.FindAnyWidget( "effects_setting_option" ).SetUserID(OptionAccessType.AT_OPTIONS_EFFECTS_SLIDER);
		m_Root.FindAnyWidget( "music_setting_option" ).SetUserID(OptionAccessType.AT_OPTIONS_MUSIC_SLIDER);
		m_Root.FindAnyWidget( "voip_output_setting_option" ).SetUserID(OptionAccessType.AT_OPTIONS_VON_SLIDER);
		m_Root.FindAnyWidget( "voip_threshold_setting_option" ).SetUserID(OptionAccessType.AT_OPTIONS_VON_THRESHOLD_SLIDER);
		m_Root.FindAnyWidget( "voip_selection_setting_option" ).SetUserID(OptionAccessType.AT_OPTIONS_VON_INPUT_MODE);
		
		FillTextMap();
		
		array<string> inputModeValues = { "#STR_Controls_PushToTalk", "#STR_USRACT_UAVOICEOVERNETTOGGLE" };
		
		m_MasterSelector			= new OptionSelectorSlider(m_Root.FindAnyWidget("master_setting_option" ), m_MasterOption.ReadValue(), this, false, m_MasterOption.GetMin(), m_MasterOption.GetMax());
		m_EffectsSelector			= new OptionSelectorSlider(m_Root.FindAnyWidget("effects_setting_option" ), m_EffectsOption.ReadValue(), this, false, m_EffectsOption.GetMin(), m_EffectsOption.GetMax());
		m_VOIPSelector				= new OptionSelectorSlider(m_Root.FindAnyWidget("voip_output_setting_option" ), m_VOIPOption.ReadValue(), this, false, m_VOIPOption.GetMin(), m_VOIPOption.GetMax());
		m_VOIPThresholdSelector  	= new OptionSelectorLevelMarker(m_Root.FindAnyWidget("voip_threshold_setting_option" ), m_VOIPThresholdOption.ReadValue(), this, false, m_VOIPThresholdOption.GetMin(), m_VOIPThresholdOption.GetMax());
		m_MusicSelector				= new OptionSelectorSlider(m_Root.FindAnyWidget("music_setting_option" ), m_MusicOption.ReadValue(), this, false, m_MusicOption.GetMin(), m_MusicOption.GetMax());
		m_InputModeSelector			= new OptionSelectorMultistate(m_Root.FindAnyWidget("voip_selection_setting_option" ), m_InputModeOption.GetIndex(), this, false, inputModeValues);
		
		m_MasterSelector.m_OptionChanged.Insert(UpdateMaster);
		m_EffectsSelector.m_OptionChanged.Insert(UpdateEffects);
		m_VOIPSelector.m_OptionChanged.Insert(UpdateVOIP);
		m_VOIPThresholdSelector.m_OptionChanged.Insert(UpdateVOIPThreshold);
		m_MusicSelector.m_OptionChanged.Insert(UpdateMusic);
		m_InputModeSelector.m_OptionChanged.Insert(UpdateInputMode);
		
		if (m_MissionGameplay)
		{
			// event to monitor when options get reverted directly from C++
			m_VOIPThresholdOption.GetEvents().Event_OnRevert.Insert(m_VonManager.OnVOIPThresholdChanged);
			m_VonManager.m_OnVonStateEvent.Insert(OnVonStateEvent);
			m_VonManager.m_OnPartyChatChangedEvent.Insert(OnPartyChatChangedEvent);
			m_VOIPThresholdSelector.m_OptionChanged.Insert(m_VonManager.OnVOIPThresholdChanged);
		}
		
		float x, y, y2;
		m_Root.FindAnyWidget("sound_settings_scroll").GetScreenSize(x, y);
		m_Root.FindAnyWidget("sound_settings_root").GetScreenSize(x, y2);
		int f = (y2 > y);
		m_Root.FindAnyWidget("sound_settings_scroll").SetAlpha(f);
		
		m_Root.SetHandler(this);
		
		CGame game = GetGame();
		m_WasMicCapturing = game.IsMicCapturing();

		// do not enable mic capture if user in party chat
		if (!game.IsInPartyChat())
		{
			game.EnableMicCapture(true);
		}
	}
	
	void ~OptionsMenuSounds()
	{
		m_VonManager.m_OnVonStateEvent.Remove(OnVonStateEvent);	
		m_VonManager.m_OnPartyChatChangedEvent.Remove(OnPartyChatChangedEvent);
		
		if (m_MissionGameplay)
		{
			// reset mic to previous capturing state
			GetGame().EnableMicCapture(m_WasMicCapturing);		
		}

		m_AudioLevelTimer.Stop();
	}
	
	string GetLayoutName()
	{
		#ifdef PLATFORM_CONSOLE
			return "gui/layouts/new_ui/options/xbox/sound_tab.layout";
		#else
		#ifdef PLATFORM_WINDOWS
			return "gui/layouts/new_ui/options/pc/sound_tab.layout";
		#endif
		#endif
	}
	
	void Focus()
	{
		#ifdef PLATFORM_CONSOLE
			SetFocus(m_MasterSelector.GetParent());
		#endif
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if ( w && w.IsInherited(ScrollWidget))
		{
			return false;
		}
		
		m_Menu.ColorHighlight(w);
		
		return true;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if ( w && w.IsInherited(ScrollWidget))
		{
			return false;
		}
		
		m_Menu.ColorNormal(w);
		return true;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		OptionsMenu menu = OptionsMenu.Cast(GetGame().GetUIManager().GetMenu());
		if (menu)
		{
			menu.OnFocus(w, x, y);
		}
		if (w)
		{
			Param2<string, string> p = m_TextMap.Get(w.GetUserID());
			if (p)
			{
				m_DetailsRoot.Show(true);
				m_DetailsLabel.SetText(p.param1);
				m_DetailsText.SetText(p.param2);
				
				//float lines = m_DetailsText.GetContentHeight();
				//m_DetailsText.SetSize( 1, lines );
				
				m_DetailsText.Update();
				m_DetailsLabel.Update();
				m_DetailsRoot.Update();
				return true;
			}
			
			if (w.IsInherited(SliderWidget))
			{
				return true;
			}
		}
		m_DetailsRoot.Show(false);
		return (w != null);
	}

	void OnVonStateEvent()
	{		
		// changing VON mode may disable mic capture,
		// but mic should capture the entire time this menu is open (unless user in party chat)
		if (!GetGame().IsInPartyChat())
		{
			// force enable mic capture
			GetGame().EnableMicCapture(true);
		}
		
		UpdateWasMicCapturing();
	}
	
	void OnPartyChatChangedEvent()
	{
		auto game = GetGame();
		if (!game.IsInPartyChat())
		{
			game.EnableMicCapture(true);
		}
		
		UpdateWasMicCapturing();
	}
	
	// updates the mic capture state to reset to, when this menu closes
	void UpdateWasMicCapturing()
	{
		// if user is in party chat, then mic should not capture
		if (GetGame().IsInPartyChat())
		{
			m_WasMicCapturing = false;
		}
		
		// otherwise, mic should capture ONLY if Voice Activation is enabled
		else
		{
			m_WasMicCapturing = m_MissionGameplay.IsVoNActive();
		}
	}
	
	bool IsChanged()
	{
		return false;
	}
	
	void Apply()
	{
		
	}
	
	void Revert()
	{
		if (m_MasterOption)
		{
			m_MasterSelector.SetValue(m_MasterOption.ReadValue(), true);
		}
		if (m_EffectsOption)
		{
			m_EffectsSelector.SetValue(m_EffectsOption.ReadValue(),true);
		}
		if (m_VOIPOption)
		{
			m_VOIPSelector.SetValue(m_VOIPOption.ReadValue(), true);
		}
		if (m_VOIPThresholdOption)
		{
			m_VOIPThresholdSelector.SetValue(m_VOIPThresholdOption.ReadValue(), true);
		}
		if (m_MusicOption)
		{
			m_MusicSelector.SetValue(m_MusicOption.ReadValue(), true);
		}
		if (m_InputModeOption)
		{
			m_InputModeSelector.SetValue(m_InputModeOption.GetIndex(), false);
		}
	}
	
	void SetToDefaults()
	{
		if (m_MasterOption)
		{
			m_MasterSelector.SetValue(m_MasterOption.GetDefault(), true);
		}
		if (m_EffectsOption)
		{
			m_EffectsSelector.SetValue(m_EffectsOption.GetDefault(), true);
		}
		if (m_VOIPOption)
		{
			m_VOIPSelector.SetValue(m_VOIPOption.GetDefault(), true);
		}
		if (m_VOIPThresholdOption)
		{
			m_VOIPThresholdSelector.SetValue(m_VOIPThresholdOption.GetDefault(), true);
		}
		if (m_MusicOption)
		{
			m_MusicSelector.SetValue(m_MusicOption.GetDefault(), true);
		}
		if (m_InputModeOption)
		{
			m_InputModeOption.SetIndex(m_InputModeOption.GetDefaultIndex());
			m_InputModeSelector.SetValue( m_InputModeOption.GetIndex(), false );
		}
	}
	
	void ReloadOptions()
	{
		m_Menu.ReloadOptions();
	}
	
	void SetOptions(GameOptions options)
	{
		m_Options = options;
		
		m_MasterOption			= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_MASTER_VOLUME));
		m_EffectsOption			= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_EFFECTS_SLIDER));
		m_MusicOption			= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_MUSIC_SLIDER));
		m_VOIPOption			= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_VON_SLIDER));
		m_InputModeOption		= ListOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_VON_INPUT_MODE));
		m_VOIPThresholdOption   = NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_VON_THRESHOLD_SLIDER));
		
		Revert();
	}
	
	void UpdateMaster(float value)
	{
		m_MasterOption.WriteValue(value);
		m_Menu.OnChanged();
	}
	
	void UpdateEffects(float value)
	{
		m_EffectsOption.WriteValue(value);
		m_Menu.OnChanged();
	}
	
	void UpdateVOIP(float value)
	{
		m_VOIPOption.WriteValue(value);
		m_Menu.OnChanged();
	}
	
	void UpdateVOIPThreshold(float value)
	{
		m_VOIPThresholdOption.WriteValue(value);
		m_Menu.OnChanged();
	}
	
	void UpdateMusic(float value)
	{
		m_MusicOption.WriteValue(value);
		m_Menu.OnChanged();
	}
	
	void UpdateInputMode(int newIndex)
	{
		m_InputModeOption.SetIndex(newIndex);
		m_Menu.OnChanged();
	}
	
	void UpdateAudioLevel()
	{	
		m_VOIPThresholdSelector.SetSlider2Value(GetGame().GetSoundScene().GetAudioLevel());
	}
	
	
	void FillTextMap()
	{
		m_TextMap = new map<int, ref Param2<string, string>>;
	
		m_TextMap.Insert(OptionAccessType.AT_OPTIONS_MASTER_VOLUME, new Param2<string, string>("#STR_sound_tab_master_tip_header", "#STR_sound_tab_master_tip"));
		m_TextMap.Insert(OptionAccessType.AT_OPTIONS_EFFECTS_SLIDER, new Param2<string, string>("#STR_sound_tab_effects_tip_header", "#STR_sound_tab_effects_tip"));
		m_TextMap.Insert(OptionAccessType.AT_OPTIONS_MUSIC_SLIDER, new Param2<string, string>("#STR_sound_tab_music_tip_header", "#STR_sound_tab_music_tip"));

		m_TextMap.Insert(OptionAccessType.AT_OPTIONS_VON_SLIDER, new Param2<string, string>("#STR_sound_tab_voice_output_tip_header", "#STR_sound_tab_voice_output_tip"));
		m_TextMap.Insert(OptionAccessType.AT_OPTIONS_VON_THRESHOLD_SLIDER, new Param2<string, string>("#STR_sound_tab_voice_threshold_tip_header", "#STR_sound_tab_voice_threshold_tip"));
		m_TextMap.Insert(OptionAccessType.AT_OPTIONS_VON_INPUT_MODE, new Param2<string, string>("#STR_sound_tab_voice_mode_tip_header", "#STR_sound_tab_voice_mode_tip")); 
	}
}