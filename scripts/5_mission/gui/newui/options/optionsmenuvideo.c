class OptionsMenuVideo extends ScriptedWidgetEventHandler
{
	protected Widget						m_Root;
	
	protected Widget						m_SettingsRoot;
	protected Widget						m_DetailsRoot;
	protected TextWidget					m_DetailsLabel;
	protected RichTextWidget				m_DetailsText;
	
	protected GameOptions					m_Options;
	protected OptionsMenu					m_Menu;
	
	#ifdef PLATFORM_CONSOLE
		#ifdef PLATFORM_PS4
			protected ref OptionSelectorMultistate	m_FlipModeSelector;
		#endif
	#else
		//Overall
		protected ref OptionSelectorMultistate	m_OverallQualitySelector;
		
		//Screen
		protected ref OptionSelectorMultistate	m_DisplayModeSelector;
		protected ref OptionSelectorMultistate	m_ResolutionSelector;
		protected ref OptionSelectorSlider		m_BrightnessSelector;
		protected ref OptionSelectorMultistate	m_VSyncSelector;
		//protected ref OptionSelectorMultistate	m_ColorDepthSelector;
		
		//Scene
		protected ref OptionSelectorMultistate	m_ObjectDetailSelector;
		protected ref OptionSelectorMultistate	m_TerrainDetailSelector;
		protected ref OptionSelectorMultistate	m_TextureDetailSelector;
		protected ref OptionSelectorMultistate	m_ShadowDetailSelector;
		
		//Rendering
		protected ref OptionSelectorMultistate	m_TextureFilteringSelector;
		protected ref OptionSelectorMultistate	m_TerrainSurfaceDetailSelector;
		protected ref OptionSelectorMultistate	m_PPAASelector;
		protected ref OptionSelectorMultistate	m_HWAASelector;
		protected ref OptionSelectorMultistate	m_ATOCSelector;
		protected ref OptionSelectorMultistate	m_AOSelector;
		protected ref OptionSelectorMultistate	m_PPQualitySelector;
		protected ref OptionSelectorMultistate	m_SSRQualitySelector;
	#endif
	
	#ifdef PLATFORM_CONSOLE
		#ifdef PLATFORM_PS4
			protected ref ListOptionsAccess			m_FlipModeOption;
		#endif
	#else
		//Overall
		protected ref ListOptionsAccess			m_OverallQualityOption;
		
		//Screen
		protected ref ListOptionsAccess			m_DisplayModeOption;
		protected ref ListOptionsAccess			m_ResolutionOption;
		protected ref NumericOptionsAccess		m_BrightnessOption;
		protected ref ListOptionsAccess			m_VSyncOption;
		//protected ref ListOptionsAccess			m_ColorDepthOption;
		
		//Scene
		protected ref ListOptionsAccess			m_ObjectDetailOption;
		protected ref ListOptionsAccess			m_TerrainDetailOption;
		protected ref ListOptionsAccess			m_TextureDetailOption;
		protected ref ListOptionsAccess			m_ShadowDetailOption;
		
		//Rendering
		protected ref ListOptionsAccess			m_TextureFilteringOption;
		protected ref ListOptionsAccess			m_TerrainSurfaceDetailOption;
		protected ref ListOptionsAccess			m_PPAAOption;
		protected ref ListOptionsAccess			m_HWAAOption;
		protected ref ListOptionsAccess			m_ATOCOption;
		protected ref ListOptionsAccess			m_AOOption;
		protected ref ListOptionsAccess			m_PPQualityOption;	
		protected ref ListOptionsAccess			m_SSRQualityOption;	
	#endif
	
	protected ref map<int, ref Param2<string, string>> m_TextMap;
	
	void OptionsMenuVideo( Widget parent, Widget details_root, GameOptions options, OptionsMenu menu )
	{
		m_Root							= GetGame().GetWorkspace().CreateWidgets( GetLayoutName(), parent );
		
		m_DetailsRoot					= details_root;
		m_DetailsLabel					= TextWidget.Cast( m_DetailsRoot.FindAnyWidget( "details_label" ) );
		m_DetailsText					= RichTextWidget.Cast( m_DetailsRoot.FindAnyWidget( "details_content" ) );
		
		SetOptions( options );
		m_Menu							= menu;
		
		#ifdef PLATFORM_CONSOLE
			#ifdef PLATFORM_PS4
				m_Root.FindAnyWidget( "vsync_setting_option" ).SetUserID( OptionAccessType.AT_OPTIONS_FLIPMODE );
			#endif
		#else
			m_Root.FindAnyWidget( "overall_quality_setting_option" ).SetUserID( OptionAccessType.AT_QUALITY_PREFERENCE );
			m_Root.FindAnyWidget( "display_mode_setting_option" ).SetUserID( OptionAccessType.AT_OPTIONS_DISPLAY_MODE );
			m_Root.FindAnyWidget( "resolution_setting_option" ).SetUserID( OptionAccessType.AT_OPTIONS_RESOLUTION );
			m_Root.FindAnyWidget( "brightness_setting_option" ).SetUserID( OptionAccessType.AT_OPTIONS_BRIGHT_SLIDER );
			m_Root.FindAnyWidget( "vsync_setting_option" ).SetUserID( OptionAccessType.AT_VSYNC_VALUE );
			//m_Root.FindAnyWidget( "color_depth_setting_option" ).SetUserID( OptionAccessType.AT_HDR_DETAIL );
			
			//Scene
			m_Root.FindAnyWidget( "object_detail_setting_option" ).SetUserID( OptionAccessType.AT_OBJECTS_DETAIL );
			m_Root.FindAnyWidget( "terrain_detail_setting_option" ).SetUserID( OptionAccessType.AT_OPTIONS_TERRAIN );
			m_Root.FindAnyWidget( "texture_detail_setting_option" ).SetUserID( OptionAccessType.AT_TEXTURE_DETAIL );
			m_Root.FindAnyWidget( "shadow_detail_setting_option" ).SetUserID( OptionAccessType.AT_SHADOW_DETAIL );
			
			//Rendering
			m_Root.FindAnyWidget( "texture_filtering_setting_option" ).SetUserID( OptionAccessType.AT_ANISO_DETAIL );
			m_Root.FindAnyWidget( "terrain_surface_detail_setting_option" ).SetUserID( OptionAccessType.AT_OPTIONS_TERRAIN_SHADER );
			m_Root.FindAnyWidget( "ppaa_setting_option" ).SetUserID( OptionAccessType.AT_OPTIONS_FXAA_VALUE );
			m_Root.FindAnyWidget( "hwaa_setting_option" ).SetUserID( OptionAccessType.AT_FSAA_DETAIL );
			m_Root.FindAnyWidget( "atoc_setting_option" ).SetUserID( OptionAccessType.AT_ATOC_DETAIL );
			m_Root.FindAnyWidget( "ao_setting_option" ).SetUserID( OptionAccessType.AT_AMBIENT_OCCLUSION );
			m_Root.FindAnyWidget( "pp_setting_option" ).SetUserID( OptionAccessType.AT_POSTPROCESS_EFFECTS );
			m_Root.FindAnyWidget( "ssr_setting_option" ).SetUserID( OptionAccessType.AT_WATER_DETAIL );
		#endif
		
		FillTextMap();
		InitSelectors();
		RefreshCustom();
		GetGame().BeginOptionsVideo();
		
		float x, y, y2;
		m_Root.FindAnyWidget( "video_settings_scroll" ).GetScreenSize( x, y );
		m_Root.FindAnyWidget( "video_settings_root" ).GetScreenSize( x, y2 );
		int f = ( y2 > y );
		m_Root.FindAnyWidget( "video_settings_scroll" ).SetAlpha( f );
		
		m_Root.SetHandler( this );
	}
	
	void ~OptionsMenuVideo()
	{
		GetGame().EndOptionsVideo();
	}
	
	void InitSelectors()
	{
		#ifdef PLATFORM_CONSOLE
			#ifdef PLATFORM_PS4
				array<string> opt1				= { "#STR_ENABLED", "#STR_DISABLED", "#STR_SELECTIVE" };
				m_FlipModeSelector				= new OptionSelectorMultistate( m_Root.FindAnyWidget( "vsync_setting_option" ), m_FlipModeOption.GetIndex(), this, false, opt1 );
			#endif
		#else
			array<string> opt1			= { "#options_controls_disabled", "#options_controls_enabled" };
			array<string> opt2			= { "#options_video_fullscreen", "#options_video_windowed" };
			array<string> opt3			= { "#options_video_low", "#options_video_medium", "#options_video_high" };
			array<string> opt4			= { "#options_video_poor", "#options_video_low", "#options_video_medium", "#options_video_high", "#options_video_extreme" };
			array<string> opt41			= { "#options_video_low", "#options_video_medium", "#options_video_high", "#options_video_extreme" };
			array<string> opt5			= { "#options_video_poor", "#options_video_low", "#options_video_medium", "#options_video_high", "#options_video_extreme", "#options_video_custom" };
			array<string> opt6			= { "#options_controls_disabled", "#options_video_low", "#options_video_medium", "#options_video_high", "#options_video_extreme" };
			array<string> opt7			= { "#options_controls_disabled", "#options_video_low", "#options_video_medium", "#options_video_high" };
			array<string> opt8			= new array<string>;
			array<string> opt9			= { "#options_controls_disabled", "#options_video_low", "#options_video_high" };
			
			for ( int i = 0; i < m_ResolutionOption.GetItemsCount(); i++ )
			{
				string opt_text;
				m_ResolutionOption.GetItemText( i, opt_text );
				opt8.Insert( opt_text );
			}
			
			//Overall
			m_OverallQualitySelector		= new OptionSelectorMultistate( m_Root.FindAnyWidget( "overall_quality_setting_option" ), m_OverallQualityOption.GetIndex(), this, false, opt5 );
			
			//Screen
			m_DisplayModeSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "display_mode_setting_option" ), m_DisplayModeOption.GetIndex(), this, false, opt2 );
			m_ResolutionSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "resolution_setting_option" ), m_ResolutionOption.GetIndex(), this, false, opt8 );
			m_BrightnessSelector			= new OptionSelectorSlider( m_Root.FindAnyWidget( "brightness_setting_option" ), m_BrightnessOption.ReadValue(), this, false, m_BrightnessOption.GetMin(), m_BrightnessOption.GetMax() );
			m_VSyncSelector					= new OptionSelectorMultistate( m_Root.FindAnyWidget( "vsync_setting_option" ), m_VSyncOption.GetIndex(), this, false, opt1 );
			//m_ColorDepthSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "color_depth_setting_option" ), m_ColorDepthOption.GetIndex(), this, false, opt3 );
			
			//Scene
			m_ObjectDetailSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "object_detail_setting_option" ), m_ObjectDetailOption.GetIndex(), this, false, opt4 );
			m_TerrainDetailSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "terrain_detail_setting_option" ), m_TerrainDetailOption.GetIndex(), this, false, opt4 );
			m_TextureDetailSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "texture_detail_setting_option" ), m_TextureDetailOption.GetIndex(), this, false, opt4 );
			m_ShadowDetailSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "shadow_detail_setting_option" ), m_ShadowDetailOption.GetIndex(), this, false, opt4 );
			
			//Rendering
			m_TextureFilteringSelector		= new OptionSelectorMultistate( m_Root.FindAnyWidget( "texture_filtering_setting_option" ), m_TextureFilteringOption.GetIndex(), this, false, opt3 );
			m_TerrainSurfaceDetailSelector	= new OptionSelectorMultistate( m_Root.FindAnyWidget( "terrain_surface_detail_setting_option" ), m_TerrainSurfaceDetailOption.GetIndex(), this, false, opt41 );
			
			m_PPAASelector					= new OptionSelectorMultistate( m_Root.FindAnyWidget( "ppaa_setting_option" ), m_PPAAOption.GetIndex(), this, false, opt6 );
			m_HWAASelector					= new OptionSelectorMultistate( m_Root.FindAnyWidget( "hwaa_setting_option" ), m_HWAAOption.GetIndex(), this, false, opt7 );
			m_ATOCSelector					= new OptionSelectorMultistate( m_Root.FindAnyWidget( "atoc_setting_option" ), m_ATOCOption.GetIndex(), this, false, opt1 );
			m_AOSelector					= new OptionSelectorMultistate( m_Root.FindAnyWidget( "ao_setting_option" ), m_AOOption.GetIndex(), this, false, opt6 );
			m_PPQualitySelector				= new OptionSelectorMultistate( m_Root.FindAnyWidget( "pp_setting_option" ), m_PPQualityOption.GetIndex(), this, false, opt3 );
			m_SSRQualitySelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "ssr_setting_option" ), m_SSRQualityOption.GetIndex(), this, false, opt9 );
		#endif
		
		#ifdef PLATFORM_CONSOLE
			#ifdef PLATFORM_PS4
				m_FlipModeSelector.m_OptionChanged.Insert( OnFlipModeChanged );
			#endif
		#else
			//Overall
			m_OverallQualitySelector.m_OptionChanged.Insert( OnOverallQualityChanged );
			
			//Screen
			m_DisplayModeSelector.m_OptionChanged.Insert( OnDisplayModeChanged );
			m_ResolutionSelector.m_OptionChanged.Insert( OnResolutionChanged );
			m_BrightnessSelector.m_OptionChanged.Insert( OnBrightnessChanged );
			m_VSyncSelector.m_OptionChanged.Insert( OnVSyncChanged );
			//m_ColorDepthSelector.m_OptionChanged.Insert( OnColorDepthChanged );
			
			//Scene
			m_ObjectDetailSelector.m_OptionChanged.Insert( OnObjectDetailChanged );
			m_TerrainDetailSelector.m_OptionChanged.Insert( OnTerrainDetailChanged );
			m_TextureDetailSelector.m_OptionChanged.Insert( OnTextureDetailChanged );
			m_ShadowDetailSelector.m_OptionChanged.Insert( OnShadowDetailChanged );
			
			//Rendering
			m_TextureFilteringSelector.m_OptionChanged.Insert( OnTextureFilteringChanged );
			m_TerrainSurfaceDetailSelector.m_OptionChanged.Insert( OnTerrainSurfaceDetailChanged );
			m_PPAASelector.m_OptionChanged.Insert( OnPPAAChanged );
			m_HWAASelector.m_OptionChanged.Insert( OnHWAAChanged );
			m_ATOCSelector.m_OptionChanged.Insert( OnATOCChanged );
			m_AOSelector.m_OptionChanged.Insert( OnAOChanged );
			m_PPQualitySelector.m_OptionChanged.Insert( OnPPQualityChanged );
			m_SSRQualitySelector.m_OptionChanged.Insert( OnSSRQualityChanged );
		#endif
	}
	
	string GetLayoutName()
	{
		#ifdef PLATFORM_PS4
			return "gui/layouts/new_ui/options/ps/video_tab.layout";
		#else
			#ifdef PLATFORM_WINDOWS
				return "gui/layouts/new_ui/options/pc/video_tab.layout";
			#endif
		#endif		
	}
	
	void Focus()
	{
		#ifdef PLATFORM_PS4
			if (m_FlipModeSelector)
			{
				SetFocus( m_FlipModeSelector.GetParent() );
			}
		#endif
	}
	
	void ReloadOptions()
	{
		m_Menu.ReloadOptions();
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
		#ifndef PLATFORM_CONSOLE
			m_DisplayModeSelector.SetValue( m_DisplayModeOption.GetIndex(), false );
			m_ResolutionSelector.SetValue( m_ResolutionOption.GetIndex(), false );
			m_BrightnessSelector.SetValue( m_BrightnessOption.ReadValue(), false );
			m_VSyncSelector.SetValue( m_VSyncOption.GetIndex(), false );
			//m_ColorDepthSelector.SetValue( m_ColorDepthOption.GetIndex(), false );
			m_OverallQualitySelector.SetValue( m_OverallQualityOption.GetIndex(), false );
		#endif
		
		RefreshCustom();
		#ifndef PLATFORM_CONSOLE
			m_OverallQualitySelector.SetValue( m_OverallQualityOption.GetIndex(), false );
		#endif
	}
	
	void SetToDefaults()
	{
		#ifdef PLATFORM_CONSOLE
			#ifdef PLATFORM_PS4
				m_FlipModeSelector.SetValue( m_FlipModeOption.GetDefaultIndex(), false );
			#endif
		#else
			m_DisplayModeSelector.SetValue( m_DisplayModeOption.GetDefaultIndex(), false );
			m_ResolutionSelector.SetValue( m_ResolutionOption.GetDefaultIndex(), false );
			m_BrightnessSelector.SetValue( m_BrightnessOption.GetDefault(), false );
			m_VSyncSelector.SetValue( m_VSyncOption.GetDefaultIndex(), false );
			int idx = m_OverallQualityOption.GetDefaultIndex();
			m_OverallQualitySelector.SetValue( m_OverallQualityOption.GetDefaultIndex(), true );
		#endif
				
		RefreshCustom();
		//DefaultValuesToCustom();
	}
	
	void SetOptions( GameOptions options )
	{
		m_Options						= options;
		
		#ifdef PLATFORM_CONSOLE
			#ifdef PLATFORM_PS4
				m_FlipModeOption				= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_OPTIONS_FLIPMODE ) );
			#endif
		#else
			//Overall
			m_OverallQualityOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_QUALITY_PREFERENCE ) );
			
			//Screen
			m_DisplayModeOption				= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_OPTIONS_DISPLAY_MODE ) );
			m_ResolutionOption				= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_OPTIONS_RESOLUTION ) );
			m_BrightnessOption				= NumericOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_OPTIONS_BRIGHT_SLIDER ) );
			m_VSyncOption					= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_VSYNC_VALUE ) );
			//m_ColorDepthOption				= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_HDR_DETAIL ) );
			
			//Scene
			m_ObjectDetailOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_OBJECTS_DETAIL ) );
			m_TerrainDetailOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_OPTIONS_TERRAIN ) );
			m_TextureDetailOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_TEXTURE_DETAIL ) );
			m_ShadowDetailOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_SHADOW_DETAIL ) );
			
			//Rendering
			m_TextureFilteringOption		= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_ANISO_DETAIL ) );
			m_TerrainSurfaceDetailOption	= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_OPTIONS_TERRAIN_SHADER ) );
	
			m_PPAAOption					= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_OPTIONS_FXAA_VALUE ) );
			m_HWAAOption					= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_FSAA_DETAIL ) );
			m_ATOCOption					= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_ATOC_DETAIL ) );
			m_AOOption						= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_AMBIENT_OCCLUSION ) );
			m_PPQualityOption				= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_POSTPROCESS_EFFECTS ) );
			m_SSRQualityOption				= ListOptionsAccess.Cast( m_Options.GetOptionByType( OptionAccessType.AT_WATER_DETAIL ) );
		#endif
	}
	
	void ToggleDependentOptions(int mode, bool state)
	{
	}
	
	void InitDependentOptionsVisibility()
	{
	}
	
	void RefreshCustom()
	{
		#ifdef PLATFORM_CONSOLE
			#ifdef PLATFORM_PS4
				m_FlipModeSelector.SetValue( m_FlipModeOption.GetIndex(), false );
			#endif
		#else
			//Scene
			m_ObjectDetailSelector.SetValue( m_ObjectDetailOption.GetIndex(), false );
			m_TerrainDetailSelector.SetValue( m_TerrainDetailOption.GetIndex(), false );
			m_TextureDetailSelector.SetValue( m_TextureDetailOption.GetIndex(), false );
			m_ShadowDetailSelector.SetValue( m_ShadowDetailOption.GetIndex(), false );
			
			//Rendering
			m_TextureFilteringSelector.SetValue( m_TextureFilteringOption.GetIndex(), false );
			m_TerrainSurfaceDetailSelector.SetValue( m_TerrainSurfaceDetailOption.GetIndex(), false );
			m_PPAASelector.SetValue( m_PPAAOption.GetIndex(), false );
			m_HWAASelector.SetValue( m_HWAAOption.GetIndex(), false );
			if ( m_HWAAOption.GetIndex() == 0 )
			{
				m_ATOCSelector.Disable();
			}
			else
			{
				m_ATOCSelector.Enable();
			}
			m_ATOCSelector.SetValue( m_ATOCOption.GetIndex(), false );
			m_AOSelector.SetValue( m_AOOption.GetIndex(), false );
			m_PPQualitySelector.SetValue( m_PPQualityOption.GetIndex(), false );		
			m_SSRQualitySelector.SetValue( m_SSRQualityOption.GetIndex(), false );		
		#endif
	}
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if ( w && w.IsInherited( ScrollWidget ) )
		{
			return false;
		}
		
		m_Menu.ColorHighlight( w );
		
		return true;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		if ( w && w.IsInherited( ScrollWidget ) )
		{
			return false;
		}
		
		m_Menu.ColorNormal( w );
		return true;
	}
	
	void OnOptionChanged()
	{
		#ifndef PLATFORM_CONSOLE
			if ( m_OverallQualityOption.GetIndex() != 5 )
			{
				m_OverallQualitySelector.SetValue( 5 );
			}
		#endif
	}
	
	#ifdef PLATFORM_CONSOLE
		#ifdef PLATFORM_PS4
			void OnFlipModeChanged( int value )
			{
				m_FlipModeOption.SetIndex( value );
				OnOptionChanged();
				m_Menu.OnChanged();
			}
		#endif
	#else
		void OnOverallQualityChanged( int value )
		{
			m_OverallQualityOption.SetIndex( value );
			m_OverallQualityOption.Test(); //here's the bastard!
			m_Options.Test();
			//SetOptions(m_Options);
			
			ReloadOptions();
			
			//InitSelectors();
			m_OverallQualityOption.SetIndex( value );
			RefreshCustom();
			m_Menu.OnChanged();
		}
		
		void OnDisplayModeChanged( int value )
		{
			m_DisplayModeOption.SetIndex( value );
			m_Menu.OnChanged();
		}
		
		void OnResolutionChanged( int value )
		{
			m_ResolutionOption.SetIndex( value );
			m_Menu.OnChanged();
		}
		
		void OnBrightnessChanged( float value )
		{
			m_BrightnessOption.WriteValue( value );
			m_Menu.OnChanged();
		}
		
		void OnVSyncChanged( float value )
		{
			m_VSyncOption.SetIndex( value );
			m_Menu.OnChanged();
		}
		
		void OnColorDepthChanged( int value )
		{
			//m_ColorDepthOption.SetIndex( value );
			m_Menu.OnChanged();
		}
		
		void OnObjectDetailChanged( int value )
		{
			m_ObjectDetailOption.SetIndex( value );
			OnOptionChanged();
			m_Menu.OnChanged();
		}
		
		void OnTerrainDetailChanged( int value )
		{
			m_TerrainDetailOption.SetIndex( value );
			OnOptionChanged();
			m_Menu.OnChanged();
		}
		
		void OnTextureDetailChanged( int value )
		{
			m_TextureDetailOption.SetIndex( value );
			OnOptionChanged();
			m_Menu.OnChanged();
		}
		
		void OnShadowDetailChanged( int value )
		{
			m_ShadowDetailOption.SetIndex( value );
			OnOptionChanged();
			m_Menu.OnChanged();
		}
		
		void OnTextureFilteringChanged( int value )
		{
			m_TextureFilteringOption.SetIndex( value );
			OnOptionChanged();
			m_Menu.OnChanged();
		}
		
		void OnTerrainSurfaceDetailChanged( int value )
		{
			m_TerrainSurfaceDetailOption.SetIndex( value );
			OnOptionChanged();
			m_Menu.OnChanged();
		}
		
		void OnPPAAChanged( int value )
		{
			m_PPAAOption.SetIndex( value );
			OnOptionChanged();
			m_Menu.OnChanged();
		}
		
		void OnHWAAChanged( int value )
		{
			m_HWAAOption.SetIndex( value );
			if ( value == 0 )
			{
				m_ATOCSelector.Disable();
			}
			else
			{
				m_ATOCSelector.Enable();
			}
			OnOptionChanged();
			m_Menu.OnChanged();
		}
		
		void OnATOCChanged( int value )
		{
			m_ATOCOption.SetIndex( value );
			OnOptionChanged();
			m_Menu.OnChanged();
		}
		
		void OnAOChanged( int value )
		{
			m_AOOption.SetIndex( value );
			OnOptionChanged();
			m_Menu.OnChanged();
		}
		
		void OnPPQualityChanged( int value )
		{
			m_PPQualityOption.SetIndex( value );
			OnOptionChanged();
			m_Menu.OnChanged();
		}			
	
		void OnSSRQualityChanged( int value )
		{
			m_SSRQualityOption.SetIndex( value );
			OnOptionChanged();
			m_Menu.OnChanged();
		}			
	#endif
	
	override bool OnFocus( Widget w, int x, int y )
	{
		OptionsMenu menu = OptionsMenu.Cast( GetGame().GetUIManager().GetMenu() );
		if ( menu )
			menu.OnFocus( w, x, y );
		if ( w )
		{
			Param2<string, string> p = m_TextMap.Get( w.GetUserID() );
			if ( p )
			{
				m_DetailsRoot.Show( true );
				m_DetailsLabel.SetText( p.param1 );
				m_DetailsText.SetText( p.param2 );
				
				//float lines = m_DetailsText.GetContentHeight();
				//m_DetailsText.SetSize( 1, lines );
				
				m_DetailsText.Update();
				m_DetailsLabel.Update();
				m_DetailsRoot.Update();
				return true;
			}
			
			if ( w.IsInherited( SliderWidget ) )
			{
				return true;
			}
		}
		m_DetailsRoot.Show( false );
		return ( w != null );
	}
	
	void FillTextMap()
	{
		m_TextMap = new map<int, ref Param2<string, string>>;
		
		#ifdef PLATFORM_CONSOLE
			#ifdef PLATFORM_PS4
				m_TextMap.Insert( OptionAccessType.AT_OPTIONS_FLIPMODE, new Param2<string, string>( "#options_video_V-sync", "#options_video_V-sync_desc" ) );
			#endif
		#else
			m_TextMap.Insert( OptionAccessType.AT_QUALITY_PREFERENCE, new Param2<string, string>( "#options_video_quality", "#options_video_quality_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_OPTIONS_DISPLAY_MODE, new Param2<string, string>( "#options_video_display_mode", "#options_video_display_mode_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_OPTIONS_RESOLUTION, new Param2<string, string>( "#options_video_resolution", "#options_video_resolution_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_OPTIONS_BRIGHT_SLIDER, new Param2<string, string>( "#options_video_brightness", "#options_video_brightness_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_VSYNC_VALUE, new Param2<string, string>( "#options_video_V-sync", "#options_video_V-sync_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_HDR_DETAIL, new Param2<string, string>( "#options_video_color_depth", "#options_video_color_depth_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_OBJECTS_DETAIL, new Param2<string, string>( "#options_video_object_detail", "#options_video_object_detail_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_OPTIONS_TERRAIN, new Param2<string, string>( "#options_video_terrain_detail", "#options_video_terrain_detail_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_TEXTURE_DETAIL, new Param2<string, string>( "#options_video_texture_detail", "#options_video_texture_detail_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_SHADOW_DETAIL, new Param2<string, string>( "#options_video_shadow_detail", "#options_video_shadow_detail_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_ANISO_DETAIL, new Param2<string, string>( "#options_video_texture_filtering", "#options_video_texture_filtering_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_OPTIONS_TERRAIN_SHADER, new Param2<string, string>( "#options_video_terrain_surface_detail", "#options_video_terrain_surface_detail_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_OPTIONS_FXAA_VALUE, new Param2<string, string>( "#options_video_anti-aliasing", "#options_video_anti-aliasing_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_FSAA_DETAIL, new Param2<string, string>( "#options_video_anti-aliasing_hardware", "#options_video_anti-aliasing_hardware_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_ATOC_DETAIL, new Param2<string, string>( "#options_video_foliage_smoothing", "#options_video_foliage_smoothing_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_AMBIENT_OCCLUSION, new Param2<string, string>( "#options_video_ambient_occlusion", "#options_video_ambient_occlusion_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_POSTPROCESS_EFFECTS, new Param2<string, string>( "#options_video_post_process", "#options_video_post_process_desc" ) );
			m_TextMap.Insert( OptionAccessType.AT_WATER_DETAIL, new Param2<string, string>( "#STR_option_video_ssr_quality_tip_header", "#STR_option_video_ssr_quality_tip" ) );
		#endif
	}
}
