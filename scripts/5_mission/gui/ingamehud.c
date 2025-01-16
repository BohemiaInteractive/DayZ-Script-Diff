class IngameHud extends Hud
{
	protected const float						FADE_IN_TIME = 0.3;
	protected const float						FADE_OUT_TIME = 0.3;
	protected const float						HIDE_MENU_TIME = 5;

	protected ref map<int,string>				m_StatesWidgetNames;
	protected ref map<int,ImageWidget>			m_StatesWidgets;  // [key] ImageWidget

	protected ref map<ImageWidget, int>			m_TendencyStatusCritical;	//array of icons that are blinking due to critical tendency status
	protected const float						TENDENCY_BLINK_TIME = 0.25;
	protected float								m_BlinkTime;
	
	protected ref map<int,string>				m_BadgesWidgetNames;
	protected ref map<int,int>					m_BadgesSupportedLevels;
	protected ref map<int,int>					m_BadgesWidgetDisplay;
	protected ref map<int,ImageWidget>			m_BadgesWidgets;  // [key] ImageWidget
	protected bool								m_IsTemperatureVisible;
	protected float								m_TemperatureTimer;
	protected float								m_TemperatureShowTime = 30;
	protected ImageWidget						m_HeatBufferPlus;

	protected bool								m_IsStaminaVisible;
	protected float								m_StaminaTimer;
	protected float								m_StaminaShowTime = 0.15;
	
	protected Widget							m_HudPanelWidget;		// always visible
	protected Widget							m_QuickbarWidget;
	protected Widget 							m_VehicleHudPanels;
	protected ref InventoryQuickbar				m_Quickbar;
			
	protected VehicleHudBase 						m_ActiveVehicleHUD;
	protected ref map<string, ref VehicleHudBase>	m_VehicleHudMap; 
		
	protected Widget 							m_GameStatusIconsPanel;					
	protected ImageWidget 						m_HighPingA;					
	protected ImageWidget 						m_HighPingB;					
	protected ImageWidget 						m_LowServerPerfA;			
	protected ImageWidget 						m_LowServerPerfB;			
	protected ImageWidget 						m_ConnectionLost;			
	
	protected Widget							m_Notifiers;
	protected TextWidget						m_BloodType;
	protected TextWidget						m_BloodPosType;
	protected Widget							m_Badges;
	protected ref Timer							m_HideTimer;
	protected ref WidgetFadeTimer				m_FadeTimerCrosshair;
	protected ref WidgetFadeTimer				m_FadeTimerActionCursor;
//	protected ref WidgetFadeTimer				m_FadeTimerWeaponMode;
//	protected ref WidgetFadeTimer				m_FadeTimerZeroing;
	protected ref WidgetFadeTimer				m_FadeTimerZeroingOld;
	protected ref WidgetFadeTimer				m_FadeTimerMagazineName;
	protected ref WidgetFadeTimer				m_FadeTimerWeaponName;
	protected ref WidgetFadeTimer				m_FadeTimerWeaponStats;
	protected ref WidgetFadeTimer				m_FadeTimerQuickbar;
	protected ref WidgetFadeTimer				m_FadeTimerWalkieTalkie;
	protected ref WidgetFadeTimer				m_FadeTimerWalkieTalkieText;
	
	protected ProgressBarWidget					m_Stamina;
	protected Widget							m_StaminaBackground;
	protected Widget							m_Presence;
	protected Widget							m_StanceProne;
	protected Widget							m_StanceStand;
	protected Widget							m_StanceStandWalk;
	protected Widget							m_StanceCrouch;
	protected Widget							m_StanceCar;
	protected Widget							m_PresenceLevel0;
	protected Widget							m_PresenceLevel1;
	protected Widget							m_PresenceLevel2;
	protected Widget							m_PresenceLevel3;
	protected Widget							m_PresenceLevel4;
	protected ref Timer							myTimer; //TEMP
	protected ref array<ref WidgetFadeTimer>	m_FadeTimers = new array<ref WidgetFadeTimer>;
	protected Widget							m_MouseSingleAction;
	protected Widget							m_MouseContinuousAction;
	protected Widget							m_RadialProgressBar1;
	protected Widget							m_RadialProgressBarCrossHair;
	protected Widget							m_RadialProgressBar;
	protected Widget							m_CursorWidget;
	protected ImageWidget						m_CursorIcon;
	protected Widget							m_WalkieTalkie;
	
	protected Widget							m_ActionWidget;
	protected TextWidget						m_ActionDesc;
	protected TextWidget						m_ActionItemDesc;
	protected Widget							m_ActionItemQuantityWrapper
	protected ImageWidget						m_ActionHealthMark;
	protected ProgressBarWidget					m_ActionQuantityBar;
	protected Widget							m_ActionQuantityBox;
	protected TextWidget						m_ActionQuantityBoxText;
	protected ImageWidget						m_ActionIcon;
	protected TextWidget						m_ActionButtonText;

	protected Widget							m_ActionIconFrame;
	protected Widget							m_ActionMultipleItemsFrame;
	
	protected Widget							m_ActionTarget;
	protected ActionTargetsCursor				m_ActionTargetsCursor;
	
	// CrossHairs
	protected ImageWidget						m_PermanentCrossHair;
	protected bool								m_ZeroingKeyPressed;
	
	protected Widget 							m_HitIndicatorUp;
	protected Widget 							m_HitIndicatorDown;
	protected Widget 							m_HitIndicatorLeft;
	protected Widget 							m_HitIndicatorRight;
	protected Widget 							m_HitIndicatorSingle;
	
	ref array<ref HitDirectionEffectBase> 		m_HitDirEffectArray;
	
	protected ref IngameHudHeatBuffer			m_HeatBufferUI;
	protected ref IngameHudVisibility			m_HudVisibility;
	
	int			m_PlayerSpineIndex;
	PlayerBase	m_CurrentTaggedPlayer;
	Widget		m_PlayerTag;
	TextWidget	m_PlayerTagText;
		
	void IngameHud()
	{
		m_FadeTimerCrosshair			= new WidgetFadeTimer;
		m_FadeTimerActionCursor			= new WidgetFadeTimer;
	 	m_FadeTimerQuickbar			= new WidgetFadeTimer;
	 	m_FadeTimerWalkieTalkie		= new WidgetFadeTimer;
		m_FadeTimerWalkieTalkieText	= new WidgetFadeTimer;
		
		m_StatesWidgets					= new map<int, ImageWidget>; // [key] widgetName
		m_StatesWidgetNames				= new map<int, string>;
		
		m_TendencyStatusCritical		= new map<ImageWidget, int>;
		
		m_BadgesWidgets					= new map<int, ImageWidget>; // [key] widgetName
		m_BadgesWidgetNames				= new map<int, string>;
		m_BadgesSupportedLevels			= new map<int, int>;
		m_BadgesWidgetDisplay			= new map<int, int>;
		
		m_VehicleHudMap 				= new map<string, ref VehicleHudBase>;
		
		m_HudVisibility 				= new IngameHudVisibility();
	}
	
	override void Init( Widget hud_panel_widget )
	{
		m_HudVisibility.Init(hud_panel_widget);
		m_HudPanelWidget = hud_panel_widget;
		
		//Quickbar
		m_QuickbarWidget		= m_HudPanelWidget.FindAnyWidget("QuickbarGrid");
		m_QuickbarWidget.Show( false );
				
		//TEMPORARY HACK!!! player is not present when Hud is being initialized 
		myTimer = new Timer( CALL_CATEGORY_GAMEPLAY );
		myTimer.Run( 1, this, "InitQuickbar" );
		
		m_CursorWidget = m_HudPanelWidget.FindAnyWidget("CursorIcons");
		m_CursorWidget.Show(true);
		Class.CastTo(m_CursorIcon, m_HudPanelWidget.FindAnyWidget("Cursor"));

		//Permanent Crosshair
		Class.CastTo(m_PermanentCrossHair, m_HudPanelWidget.FindAnyWidget("PermanentCrossHair"));
		
		m_WalkieTalkie = m_HudPanelWidget.FindAnyWidget("WalkieTalkie");
		
		//Panels
		Class.CastTo(m_Stamina, m_HudPanelWidget.FindAnyWidget("StaminaBar"));
		m_Presence						= m_HudPanelWidget.FindAnyWidget("PresencePanel");
		m_Badges						= hud_panel_widget.FindAnyWidget("BadgesPanel");
		m_Notifiers						= m_HudPanelWidget.FindAnyWidget("NotifiersPanel");
		m_BloodType						= TextWidget.Cast( m_HudPanelWidget.FindAnyWidget("BloodType") );
		m_BloodPosType					= TextWidget.Cast( m_HudPanelWidget.FindAnyWidget("BloodPosType") );
				
		// Init vehicle HUDs
		m_VehicleHudPanels			= m_HudPanelWidget.FindAnyWidget("VehicleHUDPanels");
		
		CarHud carHud = new CarHud();
		m_VehicleHudMap.Insert("VehicleTypeCar", carHud);	
		
		BoatHud boatHud = new BoatHud();
		m_VehicleHudMap.Insert("VehicleTypeBoat", boatHud);
	
		foreach (VehicleHudBase vehHud : m_VehicleHudMap)
		{
			vehHud.Init(m_VehicleHudPanels);
			vehHud.HidePanel();
		}
				
		m_StaminaBackground				= m_HudPanelWidget.FindAnyWidget("StaminaBackground");
		m_StaminaBackground.Show(true);
		m_StanceProne					= m_HudPanelWidget.FindAnyWidget("StanceProne");
		m_StanceCrouch					= m_HudPanelWidget.FindAnyWidget("StanceCrouch");
		m_StanceStand					= m_HudPanelWidget.FindAnyWidget("StanceStand");
		m_StanceStandWalk				= m_HudPanelWidget.FindAnyWidget("StanceStandWalk");
		m_StanceCar						= m_HudPanelWidget.FindAnyWidget("StanceCar");

		m_ActionTarget					= m_HudPanelWidget.FindAnyWidget("ActionTargetsCursorWidget");
		//! gets scripted handler from widget
		m_ActionTarget.GetScript(m_ActionTargetsCursor);

		// heat buffer plus sign
		m_HeatBufferPlus = ImageWidget.Cast( m_HudPanelWidget.FindAnyWidget( "HeatBuffer1" ) );
		
		//Game State Icons
		m_GameStatusIconsPanel			= m_HudPanelWidget.FindAnyWidget("GameStatusIcons");
		m_HighPingA						= ImageWidget.Cast(m_HudPanelWidget.FindAnyWidget("HighPingA"));
		m_HighPingB						= ImageWidget.Cast(m_HudPanelWidget.FindAnyWidget("HighPingB"));
		m_LowServerPerfA				= ImageWidget.Cast(m_HudPanelWidget.FindAnyWidget("LowServerPerfA"));
		m_LowServerPerfB				= ImageWidget.Cast(m_HudPanelWidget.FindAnyWidget("LowServerPerfB"));
		m_ConnectionLost				= ImageWidget.Cast(m_HudPanelWidget.FindAnyWidget("ConnectionLost"));
		
		if ( m_HeatBufferPlus )
			m_HeatBufferPlus.Show( false );

		#ifndef NO_GUI
		m_Timer = new Timer( CALL_CATEGORY_GAMEPLAY );
		m_Timer.Run(0.333, this, "RefreshQuickbar", NULL, true );
		#endif
		InitBadgesAndNotifiers();
		
		m_PresenceLevel0 = hud_panel_widget.FindAnyWidget("Presence0");
		m_PresenceLevel1 = hud_panel_widget.FindAnyWidget("Presence1");
		m_PresenceLevel2 = hud_panel_widget.FindAnyWidget("Presence2");
		m_PresenceLevel3 = hud_panel_widget.FindAnyWidget("Presence3");
		m_PresenceLevel4 = hud_panel_widget.FindAnyWidget("Presence4");
		m_PresenceLevel0.Show( false );
		m_PresenceLevel1.Show( false );
		m_PresenceLevel2.Show( false );
		m_PresenceLevel3.Show( false );
		m_PresenceLevel4.Show( false );
				
		if (!g_Game.GetProfileOption(EDayZProfilesOptions.HUD))
			ShowHud(false);
		
		if (!g_Game.GetProfileOption(EDayZProfilesOptions.HUD_VEHICLE))
			SetVehicleHudDisabled(true);
		
		MissionGameplay.Cast(GetGame().GetMission()).GetConnectivityInvoker().Insert(OnConnectionIconsSettingsChanged);
		m_GameStatusIconsPanel.Show(g_Game.GetProfileOption(EDayZProfilesOptions.CONNECTIVITY_INFO));
		//ShowQuickBar(GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer() && g_Game.GetProfileOption(EDayZProfilesOptions.QUICKBAR)); //unreliable
	}
	
	void InitBadgesAndNotifiers()
	{
		// state notifiers
		m_StatesWidgetNames.Clear();
		m_StatesWidgets.Clear();
		m_StatesWidgetNames.Set( NTFKEY_THIRSTY, "Thirsty" );
		m_StatesWidgetNames.Set( NTFKEY_HUNGRY, "Hungry" );
		m_StatesWidgetNames.Set( NTFKEY_SICK, "Health" );
		m_StatesWidgetNames.Set( NTFKEY_BLEEDISH, "Blood" );
		m_StatesWidgetNames.Set( NTFKEY_FEVERISH, "Temperature" );
	
		m_Notifiers.Show( true );
		m_Badges.Show( true );

		int i = 0;
		int key = 0;
		for ( i = 0; i < m_StatesWidgetNames.Count(); i++ )
		{
			string widget_name = m_StatesWidgetNames.GetElement(i);
			key = m_StatesWidgetNames.GetKey(i);
			ImageWidget w;
			Class.CastTo(w,  m_Notifiers.FindAnyWidget( String( "Icon" + widget_name ) ) );
			m_StatesWidgets.Set( key, w );
			w.Show( true );
			for ( int y = 0; y < 5; y++ )
			{
				w.LoadImageFile( y, "set:dayz_gui image:icon" + widget_name + y );
			}
		
			w.SetImage( 0 );
			float alpha = w.GetAlpha();
			w.SetColor( ARGB( alpha * 255, 220, 220, 220 ) );	//white
			m_TendencyStatusCritical.Remove( w );
			// clear all arrows
			for ( int x = 1; x < 4; x++ )
			{
				Class.CastTo(w,  m_Notifiers.FindAnyWidget( String( widget_name + "ArrowUp" + x.ToString() ) ) );
				w.Show( false );
				Class.CastTo(w,  m_Notifiers.FindAnyWidget( String( widget_name + "ArrowDown" + x.ToString() ) ) );
				w.Show( false );
			}
		
			
		}

		// badges
		m_BadgesWidgetNames.Clear();
		m_BadgesWidgets.Clear();
		m_BadgesWidgetDisplay.Clear();
		m_BadgesSupportedLevels.Clear();
		
		m_BadgesWidgetNames.Set( NTFKEY_FRACTURE, "Fracture" );
		m_BadgesWidgetNames.Set( NTFKEY_STUFFED, "Stomach" );
		m_BadgesWidgetNames.Set( NTFKEY_SICK, "Sick" );
		m_BadgesWidgetNames.Set( NTFKEY_WETNESS, "Wetness" );
		m_BadgesWidgetNames.Set( NTFKEY_POISONED, "Poisoned" );
		m_BadgesWidgetNames.Set( NTFKEY_BLEEDISH, "Bleeding" );
		m_BadgesWidgetNames.Set( NTFKEY_LIVES, "Shock" );
		m_BadgesWidgetNames.Set( NTFKEY_PILLS, "Pills" );
		m_BadgesWidgetNames.Set( NTFKEY_LEGS, "InjuredLegs" );
		
		// any badges not included bellow are just ON/OFF
		m_BadgesSupportedLevels.Insert(NTFKEY_LEGS,3);
	
		for ( i = 0; i < m_BadgesWidgetNames.Count(); i++ )
		{
			string badge_name = m_BadgesWidgetNames.GetElement(  i);
			key = m_BadgesWidgetNames.GetKey( i );
			ImageWidget badge_widget;


			Class.CastTo(badge_widget,  m_Badges.FindAnyWidget( badge_name ) );
			
			if (m_BadgesSupportedLevels.Contains(key))
			{
				int badgeCountMax = m_BadgesSupportedLevels.Get(key);
				
				for ( int q = 0; q < badgeCountMax; q++ )
				{
					badge_widget.LoadImageFile( q, "set:dayz_gui image:icon" + badge_name + q );
				}
			}

			m_BadgesWidgets.Set( key, badge_widget );
			badge_widget.Show( false );
			m_BadgesWidgetDisplay.Set( key, false );
		}
		m_HudVisibility.SetContextFlag(EHudContextFlags.NO_BADGE, true);
	}
	
	void OnConnectionIconsSettingsChanged(bool enabled)
	{
		m_GameStatusIconsPanel.Show(enabled);
	}
	
	override void OnResizeScreen()
	{
		float x, y;
		m_HudPanelWidget.GetScreenSize( x, y );
		m_HudPanelWidget.Update();
		m_Badges.Update();
		m_Notifiers.SetPos( 0, 0 );
	}
	
	override void OnPlayerLoaded()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			Error("OnPlayerLoaded: Cannot retreive player");
	
		if (!player.IsUnconscious())	// uncon stop is not called when player respawns
			OnUnconsciousStop();
		
		player.GetOnUnconsciousStart().Insert(OnUnconsciousStart);
		player.GetOnUnconsciousStop().Insert(OnUnconsciousStop);
	}
	
	protected void OnUnconsciousStart()
	{
		m_HudVisibility.SetContextFlag(EHudContextFlags.UNCONSCIOUS, true);
	}
	
	protected void OnUnconsciousStop()
	{
		m_HudVisibility.SetContextFlag(EHudContextFlags.UNCONSCIOUS, false);
	}


	override void Show( bool show )
	{
		m_HudPanelWidget.Show(show);
	}

	override void ShowWalkieTalkie( bool show )
	{
		m_FadeTimerWalkieTalkie.Stop();
		m_FadeTimerWalkieTalkieText.Stop();
		m_WalkieTalkie.Show( show );
	}
	
	override void ShowWalkieTalkie( int fadeOutSeconds )
	{
		m_WalkieTalkie.Show(true);
		m_FadeTimerWalkieTalkie.Stop();
		m_FadeTimerWalkieTalkieText.Stop();
		m_FadeTimerWalkieTalkie.FadeOut( m_WalkieTalkie.FindAnyWidget("Icon") , fadeOutSeconds );
		m_FadeTimerWalkieTalkieText.FadeOut( m_WalkieTalkie.FindAnyWidget("Text") , fadeOutSeconds );
	}
	
	override void SetWalkieTalkieText( string text )
	{
		TextWidget txt;
		Class.CastTo(txt, m_WalkieTalkie.FindAnyWidget("Text"));
		txt.SetText(text);
	}
	
	override void SetCursorIcon( string icon )
	{
		if ( icon.Length() == 0 || icon == CursorIcons.None )
			m_CursorIcon.Show(false);
		else
		{
			m_CursorIcon.LoadImageFile( 0, icon );
			m_CursorIcon.Show(true);
		}
	}

	override void ShowCursor()
	{
		m_CursorWidget.Show( true );
	}
	
	override void HideCursor()
	{
		m_CursorWidget.Show( false );
	}
	
	override void SetCursorIconScale( string type, float percentage )
	{
		float x, y;
		m_CursorIcon.FindAnyWidget( type ).GetParent().GetSize( x, y );
		m_CursorIcon.FindAnyWidget( type ).GetParent().SetSize( x * percentage, y * percentage );
	}
	
	override void SetCursorIconOffset( string type, float x, float y )
	{
		m_CursorIcon.FindAnyWidget( type ).GetParent().SetPos( x, y );
	}
	
	override void SetCursorIconSize( string type, float x, float y )
	{
		m_CursorIcon.FindAnyWidget( type ).GetParent().SetSize( x, y );
	}

#ifdef DIAG_DEVELOPER
	// Debug Crosshair
	override void SetPermanentCrossHair( bool show )
	{
		m_PermanentCrossHair.Show(show);
	}
#endif
	
	override void DisplayNotifier( int key, int tendency, int status )
	{
		ImageWidget w;
		if ( key == NTFKEY_FEVERISH )
		{
			DisplayTendencyTemp( key, tendency, status );
		}
		else
		{
			DisplayTendencyNormal( key, tendency, status );
		}
		
		// tendency arrows
		string arrow_name = "ArrowUp";
		if ( tendency < 0 )
		{
			arrow_name = "ArrowDown";
		}
		tendency = Math.AbsInt( tendency );

		for ( int x = 1; x < 4; x++ )
		{ 
			Class.CastTo(w,  m_Notifiers.FindAnyWidget( String(  m_StatesWidgetNames.Get( key ) + "ArrowUp" + x.ToString() ) ) );
			if( w )
				w.Show( false );
			Class.CastTo(w,  m_Notifiers.FindAnyWidget( String(  m_StatesWidgetNames.Get( key ) + "ArrowDown" + x.ToString() ) ) );
			if( w )
				w.Show( false );
		}
		
		if( tendency > 0 )
		{
			string widget_name = m_StatesWidgetNames.Get( key ) + arrow_name + Math.Clamp( tendency, 1, 3 );
			Class.CastTo(w, m_Notifiers.FindAnyWidget( widget_name ) );
			if( w )
				w.Show( true );
		}
	}
	
	void DisplayTendencyNormal( int key, int tendency, int status )
	{
		ImageWidget w;
		Class.CastTo(w,  m_Notifiers.FindAnyWidget( String( "Icon" + m_StatesWidgetNames.Get( key ) ) ) );
		
		if( w )
		{
			w.SetImage( Math.Clamp( status - 1, 0, 4 ) );
			float alpha = w.GetAlpha();
			
			switch( status )
			{
				case 3:
					w.SetColor( ARGB( alpha * 255, 220, 220, 0 ) );		//yellow
					m_TendencyStatusCritical.Remove( w );				//remove from blinking group
					break;
				case 4:
					w.SetColor( ARGB( alpha * 255, 220, 0, 0 ) );		//red
					m_TendencyStatusCritical.Remove( w );				//remove from blinking group
					break;
				case 5:
					if ( !m_TendencyStatusCritical.Contains( w ) )
					{
						m_TendencyStatusCritical.Insert( w, ARGB( alpha * 255, 220, 0, 0 ) );	//add to blinking group
					}
					break;
				default:
					w.SetColor( ARGB( alpha * 255, 220, 220, 220 ) );	//white
					m_TendencyStatusCritical.Remove( w );				//remove from blinking group
					break;
			}
		}	
	}
	
	void DisplayTendencyTemp( int key, int tendency, int status )
	{
		ImageWidget w = ImageWidget.Cast( m_Notifiers.FindAnyWidget( String( "Icon" + m_StatesWidgetNames.Get( key ) ) ) );
		TextWidget temp_top = TextWidget.Cast( m_Notifiers.FindAnyWidget( "TemperatureValueTop" ) );
		TextWidget temp_bot = TextWidget.Cast( m_Notifiers.FindAnyWidget( "TemperatureValueBottom" ) );
		//string temp = player..ToString() + "°C";
		float alpha = w.GetAlpha();
		
		if ( tendency < 0 )
		{
			temp_top.Show( true );
			temp_bot.Show( false );
		}
		else
		{
			temp_top.Show( false );
			temp_bot.Show( true );
		}
		
		switch( status )
		{
			case 2:
				w.SetColor( ARGB( alpha * 255, 220, 220, 0 ) );		//WARNING_PLUS
				m_TendencyStatusCritical.Remove( w );
				w.SetImage( 1 );
				
				break;
			case 3:
				w.SetColor( ARGB( alpha * 255, 220, 0, 0 ) );		//CRITICAL_PLUS
				m_TendencyStatusCritical.Remove( w );
				w.SetImage( 0 );
				break;
			case 4:
				if ( !m_TendencyStatusCritical.Contains( w ) )		//BLINKING_PLUS
				{
					m_TendencyStatusCritical.Insert( w, ARGB( alpha * 255, 220, 0, 0 ) );
				}
				w.SetImage( 0 );
				break;
			case 5:
				w.SetColor( ARGB( alpha * 255, 0, 206, 209 ) );		//WARNING_MINUS
				m_TendencyStatusCritical.Remove( w );
				w.SetImage( 3 );
				break;
			case 6:
				w.SetColor( ARGB( alpha * 255, 30, 144, 220 ) );	//CRITICAL_MINUS
				m_TendencyStatusCritical.Remove( w );
				w.SetImage( 4 );
				break;
			case 7:													//BLINKING_MINUS
				if ( !m_TendencyStatusCritical.Contains( w ) )
				{
					m_TendencyStatusCritical.Insert( w, ARGB( alpha * 255, 30, 144, 220 ) );
				}
				w.SetImage( 4 );
				break;				
			default:
				w.SetColor( ARGB( alpha * 255, 220, 220, 220 ) );	//DEFAULT
				m_TendencyStatusCritical.Remove( w );
				w.SetImage( 2 );
				break;
		}
	}
	
	override void DisplayBadge( int key, int value )
	{
		TextWidget bleed_count = TextWidget.Cast( m_Badges.FindAnyWidget( "BleedingCount" ) );
		
		m_BadgesWidgetDisplay.Set( key, value );
		m_HudVisibility.SetContextFlag(EHudContextFlags.NO_BADGE, true);
		for ( int i = 0; i < m_BadgesWidgetDisplay.Count(); i++ )
		{
			int badge_key = m_BadgesWidgetDisplay.GetKey( i );
			int badge_value = m_BadgesWidgetDisplay.Get( badge_key );
			string badge_name = m_BadgesWidgetNames.Get( badge_key );
			
			ImageWidget badge_widget
			Class.CastTo(badge_widget,  m_Badges.FindAnyWidget( badge_name ) );
			if ( badge_widget )
			{
				if ( badge_value > 0 )
				{
					if (m_BadgesSupportedLevels.Contains(badge_key))
					{
						int levelIndex = badge_value - 1;
						int maxSupportedIndex = m_BadgesSupportedLevels.Get(badge_key) - 1;
						if (maxSupportedIndex >= levelIndex)
							badge_widget.SetImage(levelIndex);
					}

					badge_widget.Show( true );
					m_HudVisibility.SetContextFlag(EHudContextFlags.NO_BADGE, false);
					if( badge_key == NTFKEY_BLEEDISH )
					{
						bleed_count.Show( true );
						bleed_count.SetText( m_BadgesWidgetDisplay.Get( badge_key ).ToString() );
					}
				}
				else
				{
					if( badge_key == NTFKEY_BLEEDISH )
					{
						bleed_count.Show( false );
					}
					badge_widget.Show( false );
				}
			}
		}
	}
	
	override void SetTemperature( string temp )
	{
		m_IsTemperatureVisible = true;
		TextWidget temp_top = TextWidget.Cast( m_Notifiers.FindAnyWidget( "TemperatureValueTop" ) );
		TextWidget temp_bot = TextWidget.Cast( m_Notifiers.FindAnyWidget( "TemperatureValueBottom" ) );
		temp_top.SetText( temp );
		temp_bot.SetText( temp );
		m_TemperatureTimer = 0;
	}
	
	void HideTemperature()
	{
		m_IsTemperatureVisible = false;
		TextWidget temp_top = TextWidget.Cast( m_Notifiers.FindAnyWidget( "TemperatureValueTop" ) );
		TextWidget temp_bot = TextWidget.Cast( m_Notifiers.FindAnyWidget( "TemperatureValueBottom" ) );
		temp_top.SetText( "" );
		temp_bot.SetText( "" );
		m_TemperatureTimer = 0;
	}
	
	override void SetStaminaBarVisibility( bool show )
	{
		//m_StaminaBackground.Show( show );
		//m_Stamina.Show( show );
		if ( show )
		{
			m_Stamina.SetAlpha(1);
			//m_Stamina.SetTextColor( ColorManager.COLOR_NORMAL_TEXT );
		}
		else
		{
			m_Stamina.SetAlpha(0.3);
			//m_Stamina.SetTextColor( ColorManager.RED_COLOR );
		}

		m_IsStaminaVisible = show;
		m_StaminaTimer = 0;
	}
	
	// state 0 = empty
	// state 1 = digesting
	// state 2 = full
	void SetStomachState( int state )
	{
		ImageWidget stomach = ImageWidget.Cast( m_Badges.FindAnyWidget( "Stomach" ) );
		stomach.LoadImageFile( 0, "set:dayz_gui image:iconStomach" + state );
	}

	override void SetStamina( int value , int range )
	{
		float sx, sy;
		float max = CfgGameplayHandler.GetStaminaMax();
		float percentage = 1;
		if (max != 0)
		{
			percentage =  range / max;
		}
		
		if (range != 0)
		{
			m_Stamina.SetCurrent(( value / range ) * 100);
		}
		else
		{
			m_Stamina.SetCurrent(0);
		}
		
		m_Stamina.GetSize(sx, sy);
		m_Stamina.SetSize(percentage, sy);
 		m_StaminaBackground.SetSize(1 - percentage, sy);

		// set health & blood values
		if (!GetGame().IsMultiplayer())
		{
			if (GetGame().GetPlayer())
			{
				PlayerBase player;
				Class.CastTo(player, GetGame().GetPlayer());

				if (player)
				{
					float h1 = player.GetHealth("","");
					float b1 = player.GetHealth("","Blood");

					GetDayZGame().GetBacklit().SetHealth(h1);
					GetDayZGame().GetBacklit().SetBlood(b1);
				}
			}
		}
		
		// update backlit
		GetDayZGame().GetBacklit().UpdatePlayer(false);		
	}

	bool KeyPress(int key)
	{
		return false;
	}
	
	void ZeroingKeyPress()
	{
		m_ZeroingKeyPressed = true;
	}
	
	override void DisplayStance( int stance )
	{
		PlayerBase player;
		Class.CastTo(player,  GetGame().GetPlayer() );
		Car car;
		if ( !Class.CastTo(car, player.GetDrivingVehicle()) )
		{
			if( stance == 1 )
			{
				bool is_walking;
				m_StanceStand.Show(!is_walking);
				m_StanceStandWalk.Show(is_walking);
				m_StanceCrouch.Show(false);
				m_StanceProne.Show(false);
				m_StanceCar.Show( false );
			}
			if( stance == 2 )
			{
				m_StanceStand.Show(false);
				m_StanceStandWalk.Show(false);
				m_StanceCrouch.Show(true);
				m_StanceProne.Show(false);
				m_StanceCar.Show( false );
			}
			if( stance == 3 )
			{
				m_StanceStand.Show(false);
				m_StanceStandWalk.Show(false);
				m_StanceCrouch.Show(false);
				m_StanceProne.Show(true);
				m_StanceCar.Show( false );
			}
		}
		else
		{
			m_StanceStand.Show(false);
			m_StanceCrouch.Show(false);
			m_StanceProne.Show(false);
			m_StanceCar.Show( true );
		}
	}
	
	override void DisplayPresence()
	{
		PlayerBase player;

		if ( Class.CastTo(player, GetGame().GetPlayer()) )
		{
			int presence_level = player.GetNoisePresenceInAI();
			m_PresenceLevel0.Show( false );
			m_PresenceLevel1.Show( false );
			m_PresenceLevel2.Show( false );
			m_PresenceLevel3.Show( false );
			m_PresenceLevel4.Show( false );

			if ( presence_level > 0 )
			{
				m_PresenceLevel0.Show( true );
			}
			if ( presence_level > 1 )
			{
				m_PresenceLevel1.Show( true );
			}
			if ( presence_level > 2 )
			{
				m_PresenceLevel2.Show( true );
			}
			if ( presence_level > 3 )
			{
				m_PresenceLevel3.Show( true );
			}
			if ( presence_level > 4 )
			{
				m_PresenceLevel4.Show( true );
			}
		}
	}
		
	override void ShowVehicleInfo()
	{	
		PlayerBase currentPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (!currentPlayer)
			return;
		
		Transport vehicle = Transport.Cast(currentPlayer.GetDrivingVehicle());
		if (!vehicle)
			return;
		
		VehicleHudBase vehHud = m_VehicleHudMap.Get(vehicle.GetVehicleType());
		if (vehHud)
		{
			vehHud.ShowVehicleInfo(currentPlayer);
			m_ActiveVehicleHUD = vehHud;
			m_ActiveVehicleHUD.ShowPanel();
			ShowVehicleHud(true);
		}
	}
	
	override void HideVehicleInfo()
	{		
		if (m_ActiveVehicleHUD)
		{
			ShowVehicleHud(false);
			m_ActiveVehicleHUD.HideVehicleInfo();
			m_ActiveVehicleHUD.HidePanel();
			m_ActiveVehicleHUD = null;			
			return;
		}
	}
	
	void RefreshVehicleHud(float timeslice)
	{			
		DayZPlayer currentPlayer = GetGame().GetPlayer();
		if (!currentPlayer)
			return;
			
		HumanCommandVehicle cmdVehicle = currentPlayer.GetCommand_Vehicle();
		if (cmdVehicle && cmdVehicle.GetTransport())
		{
			if (!m_HudVisibility.IsContextFlagActive(EHudContextFlags.VEHICLE))
				m_HudVisibility.SetContextFlag(EHudContextFlags.VEHICLE, true);
		}
		else if (m_HudVisibility.IsContextFlagActive(EHudContextFlags.VEHICLE))
			m_HudVisibility.SetContextFlag(EHudContextFlags.VEHICLE, false);
		
		if (m_ActiveVehicleHUD)
			m_ActiveVehicleHUD.RefreshVehicleHud(timeslice);				
	}
	
	void InitQuickbar()
	{
		if (m_Quickbar == NULL)
		{
			m_Quickbar = new InventoryQuickbar( m_QuickbarWidget );
		}
	}
	
	InventoryQuickbar GetQuickbar()
	{
		return m_Quickbar;
	}
	
	IngameHudVisibility GetHudVisibility()
	{
		return m_HudVisibility;
	}
	
	override void UpdateQuickbarGlobalVisibility()
	{
		#ifdef PLATFORM_CONSOLE
			ShowQuickBar(GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer());
		#else
			ShowQuickBar(g_Game.GetProfileOption(EDayZProfilesOptions.QUICKBAR));
		#endif
	}
			
	// disable quick bar visibility in options
	override void ShowQuickbarUI( bool show )
	{
		m_HudVisibility.SetContextFlag(EHudContextFlags.QUICKBAR_DISABLE, !show);
	}
	
	// hide quick bar
	override void ShowQuickbarPlayer( bool show )
	{
		m_HudVisibility.SetContextFlag(EHudContextFlags.QUICKBAR_HIDE, !show);
	}
	
	// hide left HUD elements
	override void ShowHudPlayer( bool show )
	{
		m_HudVisibility.SetContextFlag(EHudContextFlags.HUD_HIDE, !show);
	}
	
	// set visibility of left/right HUD when another menu opens/closes
	override void ShowHudUI( bool show )
	{
		m_HudVisibility.SetContextFlag(EHudContextFlags.MENU_OPEN, !show);
	}
		
	// switches with inventory open in cases where UI is otherwise hidden
	override void ShowHudInventory( bool show )
	{
		m_HudVisibility.SetContextFlag(EHudContextFlags.INVENTORY_OPEN, show);
	}

	//! global setting -> used to determine visibility based on platform
	override void ShowQuickBar( bool show )
	{
		m_HudVisibility.SetContextFlag(EHudContextFlags.QUICKBAR_GLOBAL, !show);
	}
	
	// disable HUD through options menu
	override void ShowHud( bool show )
	{
		m_HudVisibility.SetContextFlag(EHudContextFlags.HUD_DISABLE, !show);
		ShowHudPlayer(true);
	}
	
	//! enter/exit driver seat
	void ShowVehicleHud(bool state)
	{
		m_HudVisibility.SetContextFlag(EHudContextFlags.DRIVER, state);
	}
	
	//! disable vehicle HUD through options menu
	void SetVehicleHudDisabled(bool state)
	{
		m_HudVisibility.SetContextFlag(EHudContextFlags.VEHICLE_DISABLE, state);
	}
	
	override void SetConnectivityStatIcon(EConnectivityStatType type, EConnectivityStatLevel level)
	{
		if (type == EConnectivityStatType.PING)
		{
			switch (level)
			{
				case EConnectivityStatLevel.OFF:
					m_HighPingA.Show(false);
					m_HighPingB.Show(false);
					break;
				case EConnectivityStatLevel.LEVEL1:
					m_HighPingA.Show(true);
					m_HighPingB.Show(false);
					break;
				case EConnectivityStatLevel.LEVEL2:
					m_HighPingA.Show(false);
					m_HighPingB.Show(true);
					break;
			}
		}
		else if (type == EConnectivityStatType.SERVER_PERF)
		{
			switch (level)
			{
				case EConnectivityStatLevel.OFF:
					m_LowServerPerfA.Show(false);
					m_LowServerPerfB.Show(false);
					break;
				case EConnectivityStatLevel.LEVEL1:
					m_LowServerPerfA.Show(true);
					m_LowServerPerfB.Show(false);
					break;
				case EConnectivityStatLevel.LEVEL2:
					m_LowServerPerfA.Show(false);
					m_LowServerPerfB.Show(true);
					break;
			}
		}
		else if (type == EConnectivityStatType.CONN_LOST)
		{
			switch (level)
			{
				case EConnectivityStatLevel.OFF:
					m_ConnectionLost.Show(false);
					break;
				case EConnectivityStatLevel.LEVEL1:
					m_ConnectionLost.Show(true);
					break
			}
		}
	}
		
	override void RefreshQuickbar( bool itemChanged = false )
	{
		if (!itemChanged && !m_HudVisibility.IsElementVisible(EHudElement.QUICKBAR)) // Don't update when it is hidden or disabled
			return;
				
		if ( itemChanged )
		{
			UIManager manager = GetGame().GetUIManager();
		
			InventoryMenu inventory = InventoryMenu.Cast(manager.FindMenu( MENU_INVENTORY ));
			MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
			
			if ( mission )
			{
				inventory = mission.GetInventory();
			}
			
			if ( inventory )
			{
				inventory.RefreshQuickbar();
			}
		}
		
		if ( m_Quickbar ) 
		{
			m_Quickbar.UpdateItems( m_QuickbarWidget );
		}
	}
	
	Widget GetHudPanelWidget()
	{
		return m_HudPanelWidget;
	}
	
	override void UpdateBloodName()
	{
		PlayerBase player;
		Class.CastTo(player, GetGame().GetPlayer() );
		
		if( player )
		{
			string blood_name;
			bool positive;
			BloodTypes.GetBloodTypeName( player.GetBloodType(), blood_name, positive );
			bool blood_type_visible = player.HasBloodTypeVisible();
			
			if( blood_type_visible )
			{
				m_BloodType.SetText( blood_name );
				if( positive )
					m_BloodPosType.SetText( "+" );
				else
					m_BloodPosType.SetText( "-" );
			}
			else
			{
				m_BloodType.SetText( "" );
				m_BloodPosType.SetText( "" );
			}
		}
	}
	
	void RefreshPlayerTags()
	{
		if( GetGame().GetPlayer() )
		{
			bool found = false;
			vector head_pos = GetGame().GetCurrentCameraPosition();
			float distance;
			foreach( Man player : ClientData.m_PlayerBaseList )
			{
				vector target_player = player.GetPosition();
				distance = vector.Distance( head_pos, target_player );
				
				target_player[1] = target_player[1] + 1.2;
				
				if( distance <= 15 && player != GetGame().GetPlayer() )
				{
					vector screen_pos = GetGame().GetScreenPosRelative( target_player );
					vector end_pos = head_pos + GetGame().GetCurrentCameraDirection() * 25;
					RaycastRVParams params = new RaycastRVParams( head_pos, end_pos, GetGame().GetPlayer(), 0 );
					params.sorted = true;
					
					array<ref RaycastRVResult> results = new array<ref RaycastRVResult>;
					DayZPhysics.RaycastRVProxy( params, results );
					if( results.Count() > 0 )
					{
						if( results.Get( 0 ).obj == player )
						{
							m_CurrentTaggedPlayer = PlayerBase.Cast( player );
							found = true;
						}
					}
				}
			}
			if( !found )
			{
				m_CurrentTaggedPlayer = null;
			}
		}
	}
	
	void ShowPlayerTag( float timeslice )
	{
		if ( m_CurrentTaggedPlayer && m_CurrentTaggedPlayer.GetIdentity() )
		{
			if ( !m_PlayerTag )
			{
				m_PlayerTag = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/hud/hud_player_tag.layout");
				m_PlayerTagText = TextWidget.Cast( m_PlayerTag.FindAnyWidget( "TagText" ) );
			}
			m_PlayerSpineIndex = m_CurrentTaggedPlayer.GetBoneIndex( "Spine2" );
			vector player_pos = m_CurrentTaggedPlayer.GetBonePositionWS( m_PlayerSpineIndex );
			vector screen_pos = GetGame().GetScreenPosRelative( player_pos );
			
			if ( screen_pos[2] > 0 )
			{
				if ( screen_pos[0] > 0 && screen_pos[0] < 1 )
				{
					if ( screen_pos[1] > 0 && screen_pos[1] < 1 )
					{
						m_PlayerTagText.SetAlpha( Math.Clamp( m_PlayerTagText.GetAlpha() + timeslice * 10, 0, 1 ) );
						m_PlayerTag.SetPos( 0.55, 0.55 );
						m_PlayerTagText.SetText( m_CurrentTaggedPlayer.GetIdentity().GetPlainName() );
						
						//m_PlayerTagText.SetSize( 1, 1 - screen_pos[2] / 25  );
						return;
					}
				}
			}
		}
		
		if ( m_PlayerTag )
		{
			float new_alpha = Math.Clamp( m_PlayerTagText.GetAlpha() - timeslice * 10, 0, 1 );
			m_PlayerTagText.SetAlpha( Math.Clamp( m_PlayerTagText.GetAlpha() - timeslice * 10, 0, 1 ) );
			if ( new_alpha == 0 )
			{
				m_PlayerTagText.SetText( "" );
				m_CurrentTaggedPlayer = null;
			}
		}
	}
	
	override void Update( float timeslice )
	{
		super.Update( timeslice );

		m_ActionTargetsCursor.Update();
		DisplayPresence();
		//
		//modifiers - tendency status (critical)
		if ( m_BlinkTime > TENDENCY_BLINK_TIME )
		{
			//get color & alpha
			if ( m_TendencyStatusCritical.Count() > 0 )
			{
				for ( int i = 0; i < m_TendencyStatusCritical.Count(); ++i )
				{
					
					ImageWidget widget = m_TendencyStatusCritical.GetKey( i );
					int alpha = widget.GetAlpha() * 255;
					int color;
					int colorA = ARGB( alpha, 255, 255, 255 );
					int colorB = m_TendencyStatusCritical.Get( widget );
					
					//set new color to different color
					if ( widget.GetColor() == colorA )
					{
						color = colorB;
					}
					else
					{
						color = colorA;
					}

					widget.SetColor( color );
				}
			}
			
			m_BlinkTime = 0;	//reset
		}
		else
		{
			m_BlinkTime += timeslice;
		}
		
		if( m_IsTemperatureVisible )
		{
			m_TemperatureTimer += timeslice;
			if( m_TemperatureTimer > m_TemperatureShowTime )
			{
				HideTemperature();
			}
		}
		
		if ( !m_IsStaminaVisible )
		{
			m_StaminaTimer += timeslice;
			if ( m_StaminaTimer > m_StaminaShowTime )
			{
				SetStaminaBarVisibility( true );
			}
			
		}
		
		if ( m_HitDirEffectArray && m_HitDirEffectArray.Count() > 0 )
		{
			UpdateHitDirEffects(timeslice);
			CleanupHitDirEffects();
		}
		
		RefreshVehicleHud(timeslice);
		
		if (!m_HudVisibility.IsContextFlagActive(IngameHudVisibility.HUD_HIDE_FLAGS) && m_HeatBufferUI && m_HeatBufferUI.CanUpdate())
			m_HeatBufferUI.Update(timeslice);

		#ifdef PLATFORM_PS4
		RefreshPlayerTags();
		ShowPlayerTag(timeslice);
		#endif
	}
	
	override void SpawnHitDirEffect(DayZPlayer player, float hit_direction,float intensity_max)
	{
		if (HitDirectionEffectBase.m_Mode == HitDirectionModes.DISABLED)
		{
			return;
		}
		
		if (m_HitDirEffectArray == null)
		{
			m_HitDirEffectArray = new array<ref HitDirectionEffectBase>;
		}
		HitDirectionEffectBase hiteff = HitDirectionEffectBase.Cast(HitDirectionEffectBase.GetCurrentType().Spawn());
		hiteff.Init(player,hit_direction,intensity_max);
		m_HitDirEffectArray.Insert(hiteff);
	}
	
	void UpdateHitDirEffects( float timeslice )
	{
		for (int i = 0; i < m_HitDirEffectArray.Count(); i++)
		{
			m_HitDirEffectArray[i].Update(timeslice);
		}
	}
	
	void CleanupHitDirEffects()
	{
		for (int i = m_HitDirEffectArray.Count() - 1; i > -1; i--)
		{
			if ( m_HitDirEffectArray[i].DurationCheck() )
			{
				m_HitDirEffectArray.Remove(i);
			}
		}
	}
	
	override void InitHeatBufferUI(Man player)
	{
		m_HeatBufferUI = new IngameHudHeatBuffer(m_HudPanelWidget, PlayerBase.Cast(player));
	}

	void Debug()
	{
		foreach (int val:m_BadgesWidgetDisplay)
		{
			int key = m_BadgesWidgetDisplay.GetKeyByValue(val);
			Print(EnumTools.EnumToString(eDisplayElements, key));
			Print(val);
			Print("---------------------");
		}
	}
	
//------------------------------------------
//ye olde unused methods
// everything below is DEPRECATED
	
	protected Widget m_VehiclePanel;
	protected Widget m_LeftHudPanelWidget;
	protected Widget m_PlayerPanelWidget;
	protected Widget m_StancePanel;
	protected Widget m_SpecializatonPanel;
	protected Widget m_SpecializationIcon;
	protected Widget m_BadgeNotifierDivider;
	protected bool	m_VehicleHudState;
	protected bool	m_VehicleHudDisabled;
	protected bool	m_IsVehicleHudVisible;
	protected bool	m_AnyBadgeVisible;
	protected bool	m_IsQuickbarVisible;
	protected bool	m_QuickbarHideUI;
	protected bool	m_QuickbarHidePlayer;
	protected bool	m_QuickbarState;
	protected bool	m_HudHideUI;
	protected bool	m_HudHidePlayer;
	protected bool	m_HudInventory;
	protected bool	m_HudState;
	protected bool	m_IsHudVisible;
	protected bool	m_Faded;
	protected bool 	m_IsInVehicle;
	protected ref array<VehicleHudBase>	m_VehicleHudArray;
	int		m_VehicleGearCount = -1;
	float	m_TimeSinceLastEngineLightChange;
	bool	m_VehicleHasOil;
	bool	m_VehicleHasCoolant;
	
	//DEPRECATED
	void HideQuickbarTimer();
	void RefreshVehicleHudVisibility();
	void RefreshQuickbarVisibility();
	void RefreshHudVisibility();
	void UpdateSpecialtyMeter(float x, float y);
	void SetLeftStatsVisibility(bool visible);
	bool IsQuickbarVisible(){return false;}
	bool IsHideQuickbarPlayer(){return false;}
	bool GetQuickBarState(){return false;}
	bool IsHudVisible(){return false;}
	bool IsHideHudPlayer(){return false;}
	bool GetHudState(){return false;}	
}