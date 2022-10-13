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
	protected ref map<int,int>					m_BadgesWidgetDisplay;
	protected ref map<int,ImageWidget>			m_BadgesWidgets;  // [key] ImageWidget
	protected bool								m_AnyBadgeVisible;
	protected bool								m_IsTemperatureVisible;
	protected float								m_TemperatureTimer;
	protected float								m_TemperatureShowTime = 30;
	protected ImageWidget						m_HeatBufferPlus;

	protected bool								m_IsStaminaVisible;
	protected float								m_StaminaTimer;
	protected float								m_StaminaShowTime = 0.15;
	
	protected ref map<int,string>				m_VehicleGearTable;
	protected ref map<int,string>				m_VehicleGearTableAuto;

	protected Widget							m_HudPanelWidget;
	protected Widget							m_LeftHudPanelWidget;
	protected Widget							m_QuickbarWidget;
	protected ref InventoryQuickbar				m_Quickbar;
	
	protected Widget							m_VehiclePanel;
	
	protected ImageWidget						m_VehicleRPMPointer;
	protected ImageWidget						m_VehicleRPMDial;
	protected ImageWidget						m_VehicleRPMRedline;
	protected ImageWidget						m_VehicleSpeedPointer;
	protected Widget							m_VehicleTemperatureIndicator;
	protected ImageWidget						m_VehicleTemperaturePointer;
	protected ImageWidget						m_VehicleTemperatureLight;
	protected ImageWidget						m_VehicleFuelPointer;
	protected ImageWidget						m_VehicleFuelLight;
	
	protected TextWidget						m_VehicleSpeedValue;
	
	protected TextWidget						m_VehicleCurrentGearValue;
	protected TextWidget						m_VehicleNextGearValue;
	protected TextWidget						m_VehiclePrevGearValue;
	
	protected ImageWidget						m_VehicleBatteryLight;
	protected ImageWidget						m_VehicleEngineLight;
	protected ImageWidget						m_VehicleOilLight;
	protected ImageWidget						m_VehicleHandBrakeLight;
	protected ImageWidget						m_VehicleWheelLight;
	
	protected bool 								m_VehicleDamageZoneHitEngineState;
	protected float								m_VehicleDamageZonesHitTimer;
	
	protected bool								m_InVehicleAsDriver;
	protected CarScript							m_CurrentVehicle;
	
	protected Widget							m_Notifiers;
	protected TextWidget						m_BloodType;
	protected TextWidget						m_BloodPosType;
	protected Widget							m_BadgeNotifierDivider;
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
	protected Widget							m_StancePanel;
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
	
	protected bool								m_HudHideUI;
	protected bool								m_HudHidePlayer;
	protected bool								m_HudInventory;
	protected bool								m_HudState; //options-driven global setting
	protected bool								m_QuickbarHideUI;
	protected bool								m_QuickbarHidePlayer;
	protected bool								m_QuickbarState; //options-driven global setting
	protected bool								m_IsQuickbarVisible;
	protected bool								m_Faded;
	protected bool								m_ZeroingKeyPressed;
	
	protected Widget 							m_HitIndicatorUp;
	protected Widget 							m_HitIndicatorDown;
	protected Widget 							m_HitIndicatorLeft;
	protected Widget 							m_HitIndicatorRight;
	protected Widget 							m_HitIndicatorSingle;
	
	ref array<ref HitDirectionEffectBase> 		m_HitDirEffectArray;
		
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
		m_BadgesWidgetDisplay			= new map<int, int>;
		
		m_VehicleDamageZonesHitTimer	= 0;

		m_VehicleGearTable				= new map<int, string>;
		m_VehicleGearTable.Set(-1, "");
		m_VehicleGearTable.Set(CarGear.REVERSE, "R");
		m_VehicleGearTable.Set(CarGear.NEUTRAL, "N");
		m_VehicleGearTable.Set(CarGear.FIRST, "1");
		m_VehicleGearTable.Set(CarGear.SECOND, "2");
		m_VehicleGearTable.Set(CarGear.THIRD, "3");
		m_VehicleGearTable.Set(CarGear.FOURTH, "4");
		m_VehicleGearTable.Set(CarGear.FIFTH, "5");
		m_VehicleGearTable.Set(CarGear.SIXTH, "6");
		m_VehicleGearTable.Set(CarGear.SEVENTH, "7");
		m_VehicleGearTable.Set(CarGear.EIGTH, "8");
		
		m_VehicleGearTableAuto			= new map<int, string>;
		m_VehicleGearTableAuto.Set(-1, "");
		m_VehicleGearTableAuto.Set(CarAutomaticGearboxMode.R, "R");
		m_VehicleGearTableAuto.Set(CarAutomaticGearboxMode.N, "N");
		m_VehicleGearTableAuto.Set(CarAutomaticGearboxMode.D, "D");
	}
	
	override void Init( Widget hud_panel_widget )
	{
		m_HudPanelWidget = hud_panel_widget;
		m_HudPanelWidget.Show( true );

		//Quickbar
		m_QuickbarWidget		= m_HudPanelWidget.FindAnyWidget("QuickbarGrid");
		m_QuickbarWidget.Show( false );
		
		//Left HUD Panel
		m_LeftHudPanelWidget	= m_HudPanelWidget.FindAnyWidget("LeftHUDPanel");
		
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
		m_BadgeNotifierDivider			= m_HudPanelWidget.FindAnyWidget("BadgeNotifierDivider");
		m_BloodType						= TextWidget.Cast( m_HudPanelWidget.FindAnyWidget("BloodType") );
		m_BloodPosType					= TextWidget.Cast( m_HudPanelWidget.FindAnyWidget("BloodPosType") );
		
		m_VehiclePanel					= m_HudPanelWidget.FindAnyWidget("VehiclePanel");
		
		m_VehicleRPMPointer				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("RPMPointer") );
		m_VehicleRPMDial				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("RPMDial") );
		m_VehicleRPMRedline				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("RPMDialRedline") );
		m_VehicleSpeedPointer			= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("SpeedPointer") );
		m_VehicleSpeedValue				= TextWidget.Cast( m_VehiclePanel.FindAnyWidget("SpeedValue") );
		
		m_VehicleCurrentGearValue		= TextWidget.Cast( m_VehiclePanel.FindAnyWidget("Current") );
		m_VehicleNextGearValue			= TextWidget.Cast( m_VehiclePanel.FindAnyWidget("Next") );
		m_VehiclePrevGearValue			= TextWidget.Cast( m_VehiclePanel.FindAnyWidget("Prev") );
		
		m_VehicleBatteryLight			= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("BatteryLight"));
		m_VehicleEngineLight			= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("EngineLight"));
		m_VehicleOilLight				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("OilLight"));
		m_VehicleHandBrakeLight			= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("HandBrakeLight"));
		m_VehicleWheelLight				= ImageWidget.Cast( m_VehiclePanel.FindAnyWidget("WheelLight"));
		
		m_VehicleTemperatureIndicator	= m_VehiclePanel.FindAnyWidget("TemperatureIndicator");
		m_VehicleTemperaturePointer		= ImageWidget.Cast(m_VehiclePanel.FindAnyWidget("TemperaturePointer"));
		m_VehicleTemperatureLight		= ImageWidget.Cast(m_VehiclePanel.FindAnyWidget("TemperatureLight"));
		m_VehicleFuelPointer			= ImageWidget.Cast(m_VehiclePanel.FindAnyWidget("FuelPointer"));
		m_VehicleFuelLight				= ImageWidget.Cast(m_VehiclePanel.FindAnyWidget("FuelLight"));
		
		m_StaminaBackground				= m_HudPanelWidget.FindAnyWidget("StaminaBackground");
		m_StaminaBackground.Show(true);
		m_StanceProne					= m_HudPanelWidget.FindAnyWidget("StanceProne");
		m_StanceCrouch					= m_HudPanelWidget.FindAnyWidget("StanceCrouch");
		m_StanceStand					= m_HudPanelWidget.FindAnyWidget("StanceStand");
		m_StanceStandWalk				= m_HudPanelWidget.FindAnyWidget("StanceStandWalk");
		m_StanceCar						= m_HudPanelWidget.FindAnyWidget("StanceCar");
		m_StancePanel					= m_HudPanelWidget.FindAnyWidget("StancePanel");

		m_ActionTarget					= m_HudPanelWidget.FindAnyWidget("ActionTargetsCursorWidget");
		//! gets scripted handler from widget
		m_ActionTarget.GetScript(m_ActionTargetsCursor);

		// heat buffer plus sign
		m_HeatBufferPlus = ImageWidget.Cast( m_HudPanelWidget.FindAnyWidget( "HeatBuffer" ) );
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
		
		SetLeftStatsVisibility( true );
		m_HudState = g_Game.GetProfileOption( EDayZProfilesOptions.HUD );
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
		m_BadgesWidgetNames.Set( NTFKEY_FRACTURE, "Fracture" );
		m_BadgesWidgetNames.Set( NTFKEY_STUFFED, "Stomach" );
		m_BadgesWidgetNames.Set( NTFKEY_SICK, "Sick" );
		m_BadgesWidgetNames.Set( NTFKEY_WETNESS, "Wetness" );
		m_BadgesWidgetNames.Set( NTFKEY_POISONED, "Poisoned" );
		m_BadgesWidgetNames.Set( NTFKEY_BLEEDISH, "Bleeding" );
		m_BadgesWidgetNames.Set( NTFKEY_LIVES, "Shock" );
		m_BadgesWidgetNames.Set( NTFKEY_PILLS, "Pills" );
	
		for ( i = 0; i < m_BadgesWidgetNames.Count(); i++ )
		{
			string badge_name = m_BadgesWidgetNames.GetElement(  i);
			key = m_BadgesWidgetNames.GetKey( i );
			ImageWidget badge_widget;
			Class.CastTo(badge_widget,  m_Badges.FindAnyWidget( badge_name ) );
			m_BadgesWidgets.Set( key, badge_widget );
			badge_widget.Show( false );
			m_BadgesWidgetDisplay.Set( key, false );
		}
		m_AnyBadgeVisible = false;
		m_BadgeNotifierDivider.Show(false);
	
	}
	
	override void OnResizeScreen()
	{
		float x, y;
		m_HudPanelWidget.GetScreenSize( x, y );
		m_HudPanelWidget.Update();
		m_Badges.Update();
		m_Notifiers.SetPos( 0, 0 );
	}

	override void Show( bool show )
	{
		m_HudPanelWidget.Show( show );
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

#ifdef DEVELOPER
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
		m_AnyBadgeVisible = false;
		for ( int i = 0; i < m_BadgesWidgetDisplay.Count(); i++ )
		{
			int badge_key = m_BadgesWidgetDisplay.GetKey( i );
			string badge_name = m_BadgesWidgetNames.Get( badge_key );
			ImageWidget badge_widget
			Class.CastTo(badge_widget,  m_Badges.FindAnyWidget( badge_name ) );
			if ( badge_widget )
			{
				if ( m_BadgesWidgetDisplay.Get( badge_key ) > 0 )
				{
					badge_widget.Show( true );
					m_AnyBadgeVisible = true;
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
		
		RefreshHudVisibility();
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
	
	int		m_VehicleGearCount = -1;
	float	m_TimeSinceLastEngineLightChange;
	bool	m_VehicleHasOil;
	bool	m_VehicleHasCoolant;
	
	override void ShowVehicleInfo()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			HumanCommandVehicle hcv = player.GetCommand_Vehicle();
			if (hcv)
			{
				m_CurrentVehicle = CarScript.Cast(hcv.GetTransport());
				if (m_CurrentVehicle)
				{
					m_InVehicleAsDriver	= true;					
					m_VehicleGearCount 	= m_CurrentVehicle.GetGearsCount();
					m_VehicleOilLight.Show(m_VehicleHasOil);

					float rpm_value_red = m_CurrentVehicle.EngineGetRPMRedline() / m_CurrentVehicle.EngineGetRPMMax();
					m_VehicleRPMDial.SetMaskProgress(rpm_value_red);
					m_VehicleRPMRedline.SetMaskProgress(1 - rpm_value_red);

					m_HudPanelWidget.FindAnyWidget("PlayerPanel").Show(false);
					m_Presence.Show(false);
					m_StancePanel.Show(false);

					m_VehiclePanel.Show(true);
				}
			}
		}
	}
	
	override void HideVehicleInfo()
	{
		m_HudPanelWidget.FindAnyWidget("PlayerPanel").Show(true);
		m_Presence.Show(true);
		m_StancePanel.Show(true);
		m_VehiclePanel.Show(false);
		
		m_InVehicleAsDriver	= false;
		m_CurrentVehicle	= null;
		m_VehicleGearCount	= -1;
	}
	
	void RefreshVehicleHud( float timeslice )
	{
		if (m_CurrentVehicle && !GetGame().GetUIManager().GetMenu())
		{
			m_VehiclePanel.Show(true);
			float rpm_value		= m_CurrentVehicle.EngineGetRPM() / m_CurrentVehicle.EngineGetRPMMax();
			float rpm_value_red	= m_CurrentVehicle.EngineGetRPMRedline() / m_CurrentVehicle.EngineGetRPMMax();
			float speed_value	= m_CurrentVehicle.GetSpeedometerAbsolute() / 200;
			
			m_VehicleRPMPointer.SetRotation(0, 0, rpm_value * 270 - 130, true);
			m_VehicleSpeedPointer.SetRotation(0, 0, speed_value * 260 - 130, true);
			m_VehicleSpeedValue.SetText(Math.AbsInt(m_CurrentVehicle.GetSpeedometer()).ToString());

			int engineHealthLevel	= m_CurrentVehicle.GetHealthLevel("Engine");
			int fuelTankHealthLevel	= m_CurrentVehicle.GetHealthLevel("FuelTank");
			bool newHealth			= false;
			
			//! engine
			if (m_CurrentVehicle.HasEngineZoneReceivedHit())
			{
				if (m_TimeSinceLastEngineLightChange > 0.35)
				{
					m_VehicleEngineLight.Show(!m_VehicleEngineLight.IsVisible());
					if (engineHealthLevel <= GameConstants.STATE_WORN)
					{
						m_VehicleEngineLight.SetColor(Colors.WHITE);
					}
					else
					{
						m_VehicleEngineLight.SetColor(ItemManager.GetItemHealthColor(engineHealthLevel));
					}
					
					m_VehicleEngineLight.SetAlpha(1);
					m_TimeSinceLastEngineLightChange = 0;
				}
				
				m_TimeSinceLastEngineLightChange += timeslice;
				newHealth = true;
			}
			else if (engineHealthLevel > GameConstants.STATE_WORN)
			{
				m_VehicleEngineLight.SetColor(ItemManager.GetItemHealthColor(engineHealthLevel));
				m_VehicleEngineLight.SetAlpha(1);
				m_VehicleEngineLight.Show(true);
			}
			else
			{
				m_VehicleEngineLight.Show(false);
			}
			
			//! fuel tank
			if (fuelTankHealthLevel <= GameConstants.STATE_WORN)
			{
				m_VehicleFuelLight.SetColor(Colors.WHITE);
				m_VehicleFuelLight.SetAlpha(1);
				m_VehicleFuelLight.Show(true);	
			}
			else if (fuelTankHealthLevel > GameConstants.STATE_WORN)
			{
				m_VehicleFuelLight.SetColor(ItemManager.GetItemHealthColor(fuelTankHealthLevel));
				m_VehicleFuelLight.SetAlpha(1);
				m_VehicleFuelLight.Show(true);
			}
			
			//! temperature
			if (m_CurrentVehicle.IsVitalRadiator())
			{
				m_VehicleTemperatureIndicator.Show(true);
				if (m_CurrentVehicle.HasRadiator())
				{
					int radiatorHealthLevel = m_CurrentVehicle.GetRadiator().GetHealthLevel("");
					if (radiatorHealthLevel <= GameConstants.STATE_WORN)
					{
						m_VehicleTemperatureLight.SetColor(Colors.WHITE);
						m_VehicleTemperatureLight.SetAlpha(1);
						m_VehicleTemperatureLight.Show(true);	
					}
					else if (radiatorHealthLevel > GameConstants.STATE_WORN)
					{
						m_VehicleTemperatureLight.SetColor(ItemManager.GetItemHealthColor(radiatorHealthLevel));
						m_VehicleTemperatureLight.SetAlpha(1);
						m_VehicleTemperatureLight.Show(true);
					}
				}
				else
				{
					m_VehicleTemperatureLight.SetColor(Colors.COLOR_RUINED);
					m_VehicleTemperatureLight.SetAlpha(1);
					m_VehicleTemperatureLight.Show(true);
				}
			}
			else
			{
				m_VehicleTemperatureIndicator.Show(false);
			}
			
			m_VehicleHandBrakeLight.Show(m_CurrentVehicle.IsHandbrakeActive());
			m_VehicleWheelLight.Show(m_CurrentVehicle.WheelIsAnyLocked());

			
			int engaged_gear	= -1;
			int prev_gear		= -1;
			int next_gear		= -1;
			
			if (m_CurrentVehicle.GearboxGetType() == CarGearboxType.MANUAL)
			{
				engaged_gear = m_CurrentVehicle.GetGear();
				prev_gear = engaged_gear - 1;
				next_gear = engaged_gear + 1;

				if (engaged_gear == CarGear.NEUTRAL)
				{
					prev_gear = CarGear.REVERSE;
				}
				else if (engaged_gear == CarGear.REVERSE)
				{
					prev_gear = -1;
					next_gear = CarGear.NEUTRAL;
				}

				m_VehicleCurrentGearValue.SetText(m_VehicleGearTable.Get(engaged_gear));
				m_VehicleNextGearValue.Show(next_gear < m_VehicleGearCount);
			
				m_VehicleNextGearValue.SetText(m_VehicleGearTable.Get(next_gear));
				m_VehiclePrevGearValue.SetText(m_VehicleGearTable.Get(prev_gear));
			}
			else
			{
				engaged_gear = m_CurrentVehicle.GearboxGetMode();
				prev_gear = engaged_gear - 1;
				next_gear = engaged_gear + 1;
				
				m_VehicleCurrentGearValue.SetText(m_VehicleGearTableAuto.Get(engaged_gear));
				m_VehicleNextGearValue.Show(next_gear < EnumTools.GetEnumSize(CarAutomaticGearboxMode));
			
				m_VehicleNextGearValue.SetText(m_VehicleGearTableAuto.Get(next_gear));
				m_VehiclePrevGearValue.SetText(m_VehicleGearTableAuto.Get(prev_gear));
			}
			
			// refresh backlit
			GetDayZGame().GetBacklit().RefreshVehicleLayout(engaged_gear, newHealth);
			
			m_VehicleFuelPointer.SetRotation( 0, 0, m_CurrentVehicle.GetFluidFraction( CarFluid.FUEL ) * 260 - 130, true );
			m_VehicleTemperaturePointer.SetRotation( 0, 0, -1 * m_CurrentVehicle.GetFluidFraction( CarFluid.COOLANT ) * 260 + 130, true );

			//! general hit timer reset			
			if (m_VehicleDamageZonesHitTimer > 1)
			{
				m_VehicleDamageZoneHitEngineState = false;
				m_VehicleDamageZonesHitTimer = 0;
			}
			
			m_VehicleDamageZonesHitTimer += timeslice;
		}
		else
		{
			m_VehiclePanel.Show(false);
		}
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
	
	bool IsQuickbarVisible()
	{
		return m_IsQuickbarVisible;
	}
	
	void RefreshQuickbarVisibility()
	{
		m_IsQuickbarVisible = !m_QuickbarHideUI && !m_QuickbarHidePlayer && m_QuickbarState;
		m_QuickbarWidget.Show( m_IsQuickbarVisible );
	}
	
	void RefreshHudVisibility()
	{
		bool genericShow = ( !m_HudHidePlayer && !m_HudHideUI && m_HudState ) || m_HudInventory;
		
		SetLeftStatsVisibility( genericShow );
		m_Badges.Show( genericShow );
		m_Notifiers.Show( genericShow );
		m_BadgeNotifierDivider.Show( genericShow && m_AnyBadgeVisible );
	}
	
	bool IsHideQuickbarPlayer()
	{
		return m_QuickbarHidePlayer;
	}
	
	bool IsHideHudPlayer()
	{
		return m_HudHidePlayer;
	}
	
	override void ShowQuickbarUI( bool show )
	{
		m_QuickbarHideUI = !show;
		//if (m_QuickbarState)
		{
			RefreshQuickbarVisibility();
		}
	}
	
	override void ShowQuickbarPlayer( bool show )
	{
		m_QuickbarHidePlayer = !show;
		//if (m_QuickbarState)
		{
			RefreshQuickbarVisibility();
		}
	}
	
	override void ShowHudPlayer( bool show )
	{
		m_HudHidePlayer = !show;
		if (m_HudState)
		{
			RefreshHudVisibility();
		}
	}
	
	override void ShowHudUI( bool show )
	{
		m_HudHideUI = !show;
		if (m_HudState)
		{
			RefreshHudVisibility();
		}
	}
	
	override void ShowHudInventory( bool show )
	{
		m_HudInventory = show;
		RefreshHudVisibility();
	}

	override void ToggleHeatBufferPlusSign( bool show )
	{
		m_HeatBufferPlus.Show( show );
	}

	//! global setting; is quickbar visibility allowed?
	override void ShowQuickBar( bool show )
	{
		m_QuickbarState = show;
		//shown by default
		m_QuickbarHidePlayer = false;
		RefreshQuickbarVisibility();
	}
	
	override void ShowHud( bool show )
	{
		m_HudState = show;
		m_HudHidePlayer = false;
		RefreshHudVisibility();
	}
	
	bool GetQuickBarState()
	{
		return m_QuickbarState;
	}
	
	bool GetHudState()
	{
		return m_HudState;
	}
	
	//! eg. stamina bar...
	void SetLeftStatsVisibility(bool visible)
	{
		m_LeftHudPanelWidget.Show(visible);
	}
	
	override void RefreshQuickbar( bool itemChanged = false )
	{
		if ((!m_QuickbarState || m_QuickbarHidePlayer || m_QuickbarHideUI) && !itemChanged) // Don't update when it is hidden or disabled
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
	
	int			m_PlayerSpineIndex;
	PlayerBase	m_CurrentTaggedPlayer;
	Widget		m_PlayerTag;
	TextWidget	m_PlayerTagText;
	
	void ShowPlayerTag( float timeslice )
	{
		if( m_CurrentTaggedPlayer && m_CurrentTaggedPlayer.GetIdentity() )
		{
			if( !m_PlayerTag )
			{
				m_PlayerTag = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/hud/hud_player_tag.layout");
				m_PlayerTagText = TextWidget.Cast( m_PlayerTag.FindAnyWidget( "TagText" ) );
				//m_PlayerTagText.SetText( m_CurrentTaggedPlayer.GetIdentity().GetName() );
			}
			m_PlayerSpineIndex = m_CurrentTaggedPlayer.GetBoneIndex( "Spine2" );
			vector player_pos = m_CurrentTaggedPlayer.GetBonePositionWS( m_PlayerSpineIndex );
			vector screen_pos = GetGame().GetScreenPosRelative( player_pos );
			
			if( screen_pos[2] > 0 )
			{
				if( screen_pos[0] > 0 && screen_pos[0] < 1 )
				{
					if( screen_pos[1] > 0 && screen_pos[1] < 1 )
					{
						m_PlayerTagText.SetAlpha( Math.Clamp( m_PlayerTagText.GetAlpha() + timeslice * 10, 0, 1 ) );
						m_PlayerTag.SetPos( 0.55, 0.55 );
						m_PlayerTagText.SetText( m_CurrentTaggedPlayer.GetIdentity().GetName() );
						
						//m_PlayerTagText.SetSize( 1, 1 - screen_pos[2] / 25  );
						return;
					}
				}
			}
		}
		
		if( m_PlayerTag )
		{
			float new_alpha = Math.Clamp( m_PlayerTagText.GetAlpha() - timeslice * 10, 0, 1 );
			m_PlayerTagText.SetAlpha( Math.Clamp( m_PlayerTagText.GetAlpha() - timeslice * 10, 0, 1 ) );
			if( new_alpha == 0 )
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
		
		RefreshVehicleHud( timeslice );
		
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
	
//------------------------------------------
//ye olde unused methods
	
	//DEPRECATED
	void HideQuickbarTimer()
	{
	}
}
