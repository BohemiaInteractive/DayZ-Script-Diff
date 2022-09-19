class PlayerPreview: LayoutHolder
{
	protected ref PlayerPreviewWidget m_CharacterPanelWidget;

	protected int m_CharacterRotationX;
	protected int m_CharacterRotationY;
	protected int m_CharacterScaleDelta;
	protected vector m_CharacterOrientation;
	protected bool m_IsHolding;
	
	void PlayerPreview( LayoutHolder parent )
	{
		m_CharacterPanelWidget = PlayerPreviewWidget.Cast( m_Parent.GetMainWidget().FindAnyWidget( "CharacterPanelWidget" ) );

		WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown( m_Parent.GetMainWidget().FindAnyWidget( "CharacterPanel" ),  this, "MouseButtonDown" );
		WidgetEventHandler.GetInstance().RegisterOnMouseWheel( m_Parent.GetMainWidget().FindAnyWidget( "CharacterPanel" ),  this, "MouseWheel" );
		
		m_CharacterScaleDelta = 1;
		m_CharacterPanelWidget.SetPlayer( GetGame().GetPlayer() );
		m_CharacterPanelWidget.SetModelPosition( "0 0 0.605" );
		m_CharacterPanelWidget.SetSize( 1.34, 1.34 );  // default scale
		UpdateScale();
	}
	
	void RefreshPlayerPreview()
	{
		m_CharacterPanelWidget.Refresh();
	}
	
	void UpdateRotation( int mouse_x, int mouse_y, bool is_dragging )
	{
		if ( m_CharacterPanelWidget )
		{
			vector orientation = m_CharacterOrientation;
			orientation[1] = orientation[1] - ( m_CharacterRotationX - mouse_x );
			
			m_CharacterPanelWidget.SetModelOrientation( orientation );
	
			if ( !is_dragging )
			{	
				m_CharacterOrientation = orientation;
			}
		}
	}

	void UpdateScale()
	{
		if ( m_CharacterPanelWidget )
		{
			float w, h;
			m_CharacterPanelWidget.GetSize( w, h );
			w = w + ( m_CharacterScaleDelta / 25 );
			h = h + ( m_CharacterScaleDelta / 25 );
			if ( w > 0.62 && w < 3 )
			{
				m_CharacterPanelWidget.SetSize( w, h );
			}
			else if ( w < 0.62 )
			{
				m_CharacterPanelWidget.SetSize( 0.62, 0.62 );
			}
			else if ( w > 3 )
			{
				m_CharacterPanelWidget.SetSize( 3, 3 );
			}
		}
	}
	
	bool MouseButtonDown(Widget w, int x, int y, int button)
	{
		g_Game.GetMousePos( m_CharacterRotationX, m_CharacterRotationY );
		m_IsHolding = true;
		return true;
	}
	
	bool MouseWheel(Widget w, int x, int y, int wheel)
	{
		m_CharacterScaleDelta = wheel;
		UpdateScale();
		return true;
	}
	
	override void UpdateInterval()
	{
		// item in hands update
		m_CharacterPanelWidget.UpdateItemInHands(GetGame().GetPlayer().GetHumanInventory().GetEntityInHands());
		
		PlayerBase realPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		DayZPlayer dummyPlayer = m_CharacterPanelWidget.GetDummyPlayer();
		if ( realPlayer && dummyPlayer )
		{
			// injury animation update
			HumanCommandAdditives hca = dummyPlayer.GetCommandModifier_Additives();
			//dummyPlayer.UpdateDummyPlayerProxyVisibility(realPlayer.FindAttachmentBySlotName("Shoulder"), realPlayer.FindAttachmentBySlotName("Melee"));
			if ( hca && realPlayer.m_InjuryHandler )
				hca.SetInjured(realPlayer.m_InjuryHandler.GetInjuryAnimValue(), realPlayer.m_InjuryHandler.IsInjuryAnimEnabled());
		}
		
		if ( m_IsHolding )
		{
			int mouse_x;
			int mouse_y;
				
			GetMousePos(mouse_x, mouse_y);
			
			if ( GetMouseState(MouseState.LEFT) & 0x80000000 )
			{
				UpdateRotation( mouse_x, mouse_y, true );
			}
			else
			{
				UpdateRotation( mouse_x, mouse_y, false );
				m_IsHolding = false;
			}
		}
	}
}
