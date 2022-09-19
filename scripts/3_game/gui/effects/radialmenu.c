enum RadialMenuControlType
{
	MOUSE,
	CONTROLLER	
}

class RadialMenu : ScriptedWidgetEventHandler
{
	protected Widget					m_Parent;
	protected Widget 					m_ItemCardsContainer;
	protected Widget					m_RadialSelector;
	protected ImageWidget				m_RadialSelectorImage;
	protected ImageWidget 				m_RadialSelectorPointerImage;
	protected int 						m_RadialSelectorOriginalColor;
	protected int 						m_RadialSelectorDisabledColor;
	protected Widget 					m_SelectedObject;
	protected ref map<Widget, float> 	m_RadialItemCards;
	
	protected float 					m_AngleRadOffset;
	protected ref Timer 				m_UpdateTimer;
	
	//widget
	static const string 				RADIAL_SELECTOR	 			= "RadialSelector";
	static const string 				RADIAL_SELECTOR_IMAGE		= "SelectorImage";
	static const string 				RADIAL_SELECTOR_POINTER		= "SelectorPointer";
	static const string 				RADIAL_DELIMITER_CONTAINER 	= "RadialDelimiterContainer";
	static const string 				RADIAL_ITEM_CARD_CONTAINER 	= "RadialItemCardContainer";

	//controls
	protected RadialMenuControlType 	m_ControlType;
	
	protected float 					m_ControllerAngle;
	protected float 					m_ControllerTilt;

	//controller
	protected int 						m_ControllerTimout;
	protected bool						m_IsControllerTimoutEnabled 		= true;			//enables/disables controller deselect timeout reset
	protected const float				CONTROLLER_DESELECT_TIMEOUT 		= 1000;			//timeout [ms] after which selection is automatically deselect when controller is not active
	protected const float 				CONTROLLER_TILT_TRESHOLD_SELECT		= 0.8;			//tilt value (0.0-1.0) for controller sticks after which the selection will be selected
	protected const float 				CONTROLLER_TILT_TRESHOLD_EXECUTE	= 1.0;			//tilt value (0.0-1.0) for controller sticks after which the selection will be executed 
	
	//mouse
	protected bool 						m_WidgetInitialized;
	protected const float 				MOUSE_SAFE_ZONE_RADIUS = 50;		//Radius [px] of safe zone where every previous selection is deselected
	
	//References
	protected float 					m_RadiusOffset;					//Radius [% of the main container size]
	protected float 					m_ExecuteDistanceOffset;		//Distance offset [% of the main container size] after which the selection will be automatically executed
	protected float						m_OffsetFromTop;				//first item in the menu won't be directly on top but offset by a rad angle value (clock-wise)
	protected float						m_ItemCardRadiusOffset;			//Radius [% of the main container size] for item cards
	protected string					m_DelimiterLayout;				//layout file name with path
	
	ref UIScriptedMenu					m_RegisteredClass;	
	ref static RadialMenu				m_Instance;
	
	/*
	
			RADIAL MENU EVENTS
	
	Mouse:
		OnMouseSelect
		OnMouseDeselect
		OnMouseExecute
	
	Controller:
		OnControllerSelect
		OnControllerDeselect
		OnControllerExecute
		OnControllerPressSelect
		OnControllerPressBack
	
	Common:
		OnControlsChanged		- controls has been changed (mouse<->controller)
	
	*/
	
	
	//============================================
	// RadialMenu
	//============================================
	void RadialMenu()
	{
		m_Instance = this;
		
		//set default control type
		#ifdef PLATFORM_CONSOLE
				m_ControlType = RadialMenuControlType.CONTROLLER;
		#endif				
		#ifdef PLATFORM_WINDOWS
				m_ControlType = RadialMenuControlType.MOUSE;
		#endif				
			
		//radial cards
		m_RadialItemCards = new map<Widget, float>;
		m_UpdateTimer = new Timer();
		m_UpdateTimer.Run( 0.01, this, "Update", NULL, true );
	}

	void ~RadialMenu()
	{
	}
	
	static RadialMenu GetInstance()
	{
		return m_Instance;
	}
	
	//Set handler
	void OnWidgetScriptInit( Widget w )
	{
		m_ItemCardsContainer = w.FindAnyWidget( RADIAL_ITEM_CARD_CONTAINER );
		m_RadialSelector = w.FindAnyWidget( RADIAL_SELECTOR );
		m_RadialSelectorImage = ImageWidget.Cast( m_RadialSelector.FindAnyWidget( RADIAL_SELECTOR_IMAGE ) );
		m_RadialSelectorPointerImage = ImageWidget.Cast( m_RadialSelector.FindAnyWidget( RADIAL_SELECTOR_POINTER ) );
		
		m_RadialSelectorOriginalColor = m_RadialSelectorImage.GetColor();
		m_RadialSelectorDisabledColor = ARGB(255,150,150,150);
		
		//parent
		m_Parent = w;
		m_Parent.SetHandler( this );
	}

	//controls
	void SetControlType( RadialMenuControlType type )
	{
		if ( m_ControlType != type )
		{
			m_ControlType = type;
			GetGame().GameScript.CallFunction( m_RegisteredClass, "OnControlsChanged", NULL, type );
		}
	}
	
	bool IsUsingMouse()
	{
		if ( m_ControlType == RadialMenuControlType.MOUSE )
		{
			return true;
		}
		
		return false;
	}

	bool IsUsingController()
	{
		if ( m_ControlType == RadialMenuControlType.CONTROLLER )
		{
			return true;
		}
		
		return false;
	}
	
	void SetWidgetInitialized(bool state)
	{
		m_WidgetInitialized = state;
	}
	
	bool IsWidgetInitialized()
	{
		return m_WidgetInitialized;
	}
			
	//============================================
	// Setup
	//============================================	
	void RegisterClass( UIScriptedMenu class_name )
	{
		m_RegisteredClass = class_name;
		if (m_UpdateTimer && !m_UpdateTimer.IsRunning())
			m_UpdateTimer.Run( 0.01, this, "Update", NULL, true );
	}	

	//Set radial menu parameters
	//Radius offset [% of the main container size]
	void SetRadiusOffset( float radius_offset )
	{
		m_RadiusOffset = radius_offset;
	}
	
	//Distance offset [% of the main container size] after which the selection will be automatically executed
	void SetExecuteDistOffset( float execute_dist_offset )
	{
		m_ExecuteDistanceOffset	= execute_dist_offset;
	}
	
	//First item in the menu won't be directly on top but offset by a rad angle value (clock-wise)
	void SetOffsetFromTop( float offset_from_top )
	{
		m_OffsetFromTop	= offset_from_top;
	}
	
	//Radius [% of the main container size] for item cards	
	void SetItemCardRadiusOffset( float item_card_radius_offset )
	{
		m_ItemCardRadiusOffset = item_card_radius_offset;
	}
	
	//Enable/Disable controller timeout
	void ActivateControllerTimeout( bool state )
	{
		m_IsControllerTimoutEnabled = state;
	}
	
	void SetWidgetProperties( string delimiter_layout )
	{
		m_DelimiterLayout = delimiter_layout;
	}
	
	//============================================
	// Visual
	//============================================	
	//hide_selector => shows/hides radial selector when refreshing radial menu
	void Refresh( bool hide_selector = true )
	{
		int item_cards_count = GetItemCardsCount();
		if ( item_cards_count > 0 ) m_AngleRadOffset = 2 * Math.PI / item_cards_count;		
		float angle_rad = -Math.PI / 2;
		
		//--PARAM top offset--
		if ( m_OffsetFromTop != 0 )
		{
			angle_rad = angle_rad + m_OffsetFromTop;
		}
		//--------------------
		
		//delete all delimiters
		Widget delimiters_panel = m_Parent.FindAnyWidget( RADIAL_DELIMITER_CONTAINER );
		if ( delimiters_panel )
		{
			Widget del_child = delimiters_panel.GetChildren();
			while ( del_child )
			{
				Widget child_to_destroy1 = del_child;
				del_child = del_child.GetSibling();
				
				delete child_to_destroy1;
			}
		}
		
		//Position item cards, crate radial delimiters
		Widget item_cards_panel = m_Parent.FindAnyWidget( RADIAL_ITEM_CARD_CONTAINER );
		Widget item_card = item_cards_panel.GetChildren();
		
		//get radius
		float original_r = GetRadius();
		float item_cards_r = original_r;
		
		//--PARAM top offset--....
		if ( m_ItemCardRadiusOffset != 0 )
		{
			item_cards_r = item_cards_r * m_ItemCardRadiusOffset;
			if ( item_cards_r < 0 ) item_cards_r = 0;				//min radius is 0
		}
		
		m_RadialItemCards.Clear();
		for ( int i = 0; i < item_cards_count; ++i )
		{
			//position item cards
			if ( item_card )
			{
				//creates circle from simple widget items
				float pos_x = item_cards_r * Math.Cos( angle_rad );
				float pos_y = item_cards_r * Math.Sin( angle_rad );
				
				pos_x = pos_x / original_r;
				pos_y =	pos_y / original_r;
				
				item_card.SetPos( pos_x, pos_y );
				
				//store item card
				m_RadialItemCards.Insert( item_card, angle_rad );
				
				//get next child
				item_card = item_card.GetSibling();
			}
			//-------------------------
			
			//create delimiter
			if ( item_cards_count > 1 && delimiters_panel && m_DelimiterLayout )
			{
				Widget delimiter_widget = GetGame().GetWorkspace().CreateWidgets( m_DelimiterLayout, delimiters_panel );
				float delim_angle_rad = angle_rad + ( m_AngleRadOffset / 2 );
				delimiter_widget.SetPos( 0, 0 );
				delimiter_widget.SetRotation( 0, 0, GetAngleInDegrees( delim_angle_rad ) + 90 );
			}
						
			//calculate next angle
			angle_rad += m_AngleRadOffset;
		}
		
		//hide selector on refresh
		if ( hide_selector )
		{
			HideRadialSelector();
		}
	}

	//Radial selector
	protected void ShowRadialSelector( Widget selected_item )
	{
		if ( m_RadialSelector && selected_item )
		{
			int item_count = m_RadialItemCards.Count();
			if ( item_count > 1 )
			{
				int angle_deg = GetAngleInDegrees( m_RadialItemCards.Get( selected_item ) );
				m_RadialSelector.SetRotation( 0, 0, angle_deg + 90 );			//rotate widget according to its desired rotation
				
				//set radial selector size
				float progress = ( 1 / item_count ) * 2;
				m_RadialSelectorImage.SetMaskProgress( progress );
				
				m_RadialSelector.Show( true );
				
				bool grey_selector = selected_item.GetFlags() & WidgetFlags.DISABLED;
				if (!grey_selector)
				{
					m_RadialSelectorImage.SetColor(m_RadialSelectorDisabledColor);
					m_RadialSelectorPointerImage.SetColor(m_RadialSelectorDisabledColor);
				}
				else
				{
					m_RadialSelectorImage.SetColor(m_RadialSelectorOriginalColor);
					m_RadialSelectorPointerImage.SetColor(m_RadialSelectorOriginalColor);
				}
			}
		}
	}
	
	protected void HideRadialSelector()
	{
		if ( m_RadialSelector )
		{
			m_RadialSelector.Show( false );
		}
	}
	
	//============================================
	// Widget size calculations
	//============================================	
	protected int GetItemCardsCount()
	{
		Widget child = m_ItemCardsContainer.GetChildren();
		int count = 0;
		
		while ( child )
		{
			++count;
			
			child = child.GetSibling();
		}
		
		return count;
	}
	
	protected float GetRadius()
	{
		float radius = Math.AbsFloat( GetParentMinSize() * 0.5 );
		
		//PARAM --radius--
		if ( m_RadiusOffset > 0 )
		{
			return radius * m_RadiusOffset;
		}
		//----------------
		
		return radius;
	}
	
	protected void GetParentCenter( out float center_x, out float center_y )
	{
		if ( m_Parent )
		{
			float wx;
			float wy;
			m_Parent.GetScreenPos( wx, wy );
			
			float ww;
			float wh;
			m_Parent.GetScreenSize( ww, wh );
			
			center_x = wx + ww / 2;	//center
			center_y = wy + wh / 2;
		}
	}
	
	protected float GetParentMinSize()
	{
		if ( m_Parent )
		{
			float size_x;
			float size_y;
			m_Parent.GetScreenSize( size_x, size_y );
			
			return Math.Min( size_x, size_y );
		}
		
		return 0;
	}

	//============================================
	// Angle calculations
	//============================================	
	//get object by angle (degrees)
	protected Widget GetObjectByDegAngle( float deg_angle )
	{
		for ( int i = 0; i < m_RadialItemCards.Count(); ++i )
		{
			Widget w = m_RadialItemCards.GetKey( i );
			float w_angle = GetAngleInDegrees( m_RadialItemCards.Get( w ) );
			float offset = GetAngleInDegrees( m_AngleRadOffset ) / 2;
			float min_angle = w_angle - offset;
			float max_angle = w_angle + offset;
			
			if ( min_angle < 0 ) min_angle += 360;	//clamp 0-360
			if ( max_angle > 360 ) max_angle -= 360;
			
			if ( min_angle > max_angle )		//angle radius is in the cycling point 360->
			{
				if ( min_angle <= deg_angle )	//is cursor position also before this point
				{
					if ( deg_angle > max_angle )
					{
						return w;
					}
				}
				else							//is cursor position after this point
				{
					if ( deg_angle < max_angle )
					{
						return w;
					}
				}
			}
			else
			{
				if ( deg_angle >= min_angle && deg_angle < max_angle )	//min, max angles are within 0-360 radius
				{
					return w;
				}				
			}
		}
		
		return NULL;
	}
	
	//returns GUI compatible mouse-to-parent angle
	protected float GetMousePointerAngle()
	{
		int mouse_x;
		int mouse_y;
		GetGame().GetMousePos( mouse_x, mouse_y );
		
		float center_x;
		float center_y;
		GetParentCenter( center_x, center_y );

		float tan_x = mouse_x - center_x;
		float tan_y = mouse_y - center_y;
		float angle = Math.Atan2( tan_y, tan_x );
		
		return angle;
	}
	
	//returns distance from parent center
	protected float GetMouseDistance()
	{
		int mouse_x;
		int mouse_y;
		GetGame().GetMousePos( mouse_x, mouse_y );
		
		float center_x;
		float center_y;
		GetParentCenter( center_x, center_y );

		float distance = vector.Distance( Vector( mouse_x, mouse_y, 0 ), Vector( center_x, center_y, 0 ) );
		
		return distance;
	}
	
	//return angle 0-360 deg
	protected float GetAngleInDegrees( float rad_angle )
	{
		float rad_deg = rad_angle * Math.RAD2DEG;
		
		int angle_mp = rad_deg / 360;
		
		if ( rad_deg < 0 )
		{
			rad_deg = rad_deg - ( 360 * angle_mp );
			rad_deg += 360;
		}
		
		return rad_deg;
	}

	//============================================
	// Update
	//============================================
	//mouse
	int last_time = -1;
	protected void Update()
	{
		if (this && !m_RegisteredClass)
		{
			m_UpdateTimer.Stop();
			return;
		}
		//get delta time
		if ( last_time < 0 )
		{
			last_time = GetGame().GetTime();
		}
		int delta_time = GetGame().GetTime() - last_time;
		last_time = GetGame().GetTime();
		
		//controls
		if ( this && m_RegisteredClass && m_RegisteredClass.IsVisible())
		{
			//mouse controls
			if ( IsUsingMouse() && m_WidgetInitialized )
			{
				float mouse_angle = GetMousePointerAngle();
				float mouse_distance = GetMouseDistance();
				
				//--PARAM --safe zone radius--
				if ( mouse_distance <= MOUSE_SAFE_ZONE_RADIUS )
				{
					//Deselect
					GetGame().GameScript.CallFunction( m_RegisteredClass, "OnMouseDeselect", NULL, m_SelectedObject );
					m_SelectedObject = NULL;
					//hide selector
					HideRadialSelector();
				}
				//----------------------------
				else
				{
					//Deselect
					GetGame().GameScript.CallFunction( m_RegisteredClass, "OnMouseDeselect", NULL, m_SelectedObject );
					
					//Select
					m_SelectedObject = GetObjectByDegAngle( GetAngleInDegrees( mouse_angle ) );
					GetGame().GameScript.CallFunction( m_RegisteredClass, "OnMouseSelect", NULL, m_SelectedObject );				
					//show selector
					ShowRadialSelector( m_SelectedObject );
					
					//PARAM --execute distance--
					if ( mouse_distance >= GetRadius() * m_ExecuteDistanceOffset )
					{
						//Execute
						GetGame().GameScript.CallFunction( m_RegisteredClass, "OnMouseExecute", NULL, m_SelectedObject );
					}
					//--------------------------
				}
			}
			//controller controls
			else if ( IsUsingController() )
			{
				UpdataControllerInput();
				
				//Controller tilt
				if ( m_ControllerAngle > -1 && m_ControllerTilt > -1 )
				{
					//Right analogue stick
					Widget w_selected = GetObjectByDegAngle( m_ControllerAngle );
					//Select
					if ( w_selected )
					{
						if ( w_selected != m_SelectedObject )
						{
							if ( m_ControllerTilt >= CONTROLLER_TILT_TRESHOLD_SELECT )
							{
								//Deselect
								GetGame().GameScript.CallFunction( m_RegisteredClass, "OnControllerDeselect", NULL, m_SelectedObject );
							
								//Select new object
								m_SelectedObject = w_selected;
								GetGame().GameScript.CallFunction( m_RegisteredClass, "OnControllerSelect", NULL, m_SelectedObject );
								//show selector
								ShowRadialSelector( m_SelectedObject );
							}
						}
					}
					else
					{
						GetGame().GameScript.CallFunction( m_RegisteredClass, "OnControllerDeselect", NULL, m_SelectedObject );
						m_SelectedObject = NULL;
						//hide selector
						HideRadialSelector();						
					}
					
					//Execute
					if ( m_ControllerTilt >= CONTROLLER_TILT_TRESHOLD_EXECUTE )
					{
						//Execute
						GetGame().GameScript.CallFunction( m_RegisteredClass, "OnControllerExecute", NULL, m_SelectedObject );
					}				
				}
				//if controller is giving no feedback
				else
				{
					if ( m_IsControllerTimoutEnabled )
					{
						m_ControllerTimout += delta_time;
						
						if ( m_ControllerTimout >= CONTROLLER_DESELECT_TIMEOUT )
						{
							GetGame().GameScript.CallFunction( m_RegisteredClass, "OnControllerDeselect", NULL, m_SelectedObject );
							m_SelectedObject = NULL;
							//hide selector
							HideRadialSelector();						
							
							m_ControllerTimout = 0;		//reset controller timeout
						}
					}
				}
				
				m_ControllerAngle = -1;				//reset angle and tilt
				m_ControllerTilt = -1;
			}
			
			m_WidgetInitialized = true;
		}
	}
	
	float NormalizeInvertAngle( float angle )
	{
		float new_angle = 360 - angle;
		int angle_mp = new_angle / 360;
			
		new_angle = new_angle - ( 360 * angle_mp );
			
		return new_angle;
	}
	
	//============================================
	// Controls
	//============================================
	void UpdataControllerInput()
	{
		Input input = GetGame().GetInput();
		
		//Controller radial
		float angle;
		float tilt;
		input.GetGamepadThumbDirection( GamepadButton.THUMB_RIGHT, angle, tilt );
		angle = NormalizeInvertAngle( angle * Math.RAD2DEG );
			
		m_ControllerAngle = angle;
		m_ControllerTilt = tilt;
		m_ControllerTimout	= 0;			//reset controller timeout
		
		//Controller buttons
		//Select (A,cross)
		if ( input.LocalPress( "UAUISelect", false ) )
		{
			GetGame().GameScript.CallFunction( m_RegisteredClass, "OnControllerPressSelect", NULL, m_SelectedObject );
		}
		
		//Back (B,circle)
		if ( input.LocalPress( "UAUIBack", false ) )
		{
			GetGame().GameScript.CallFunction( m_RegisteredClass, "OnControllerPressBack", NULL, m_SelectedObject );
		}
	}
}