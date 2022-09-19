class ClosableHeader: Header
{
	protected float			m_SquareSize;
	protected int			m_DefaultSort;
	
	protected Widget		m_MovePanel;
	protected Widget		m_PanelWidget;
	protected Widget		m_DragRender;
	protected ButtonWidget	m_MoveUp;
	protected ButtonWidget	m_MoveDown;
	
	protected bool			m_IsInLocalEquipment;
	
	void ClosableHeader( LayoutHolder parent, string function_name )
	{
		m_MovePanel	= GetMainWidget().FindAnyWidget( "MovePanel" );
		m_PanelWidget	= GetMainWidget().FindAnyWidget( "PanelWidget" );
		m_DragRender	= GetMainWidget().FindAnyWidget( "Drag_Render" );
		m_MoveUp	= ButtonWidget.Cast( GetMainWidget().FindAnyWidget( "MoveUp" ) );
		m_MoveDown	= ButtonWidget.Cast( GetMainWidget().FindAnyWidget( "MoveDown" ) );
		
		WidgetEventHandler.GetInstance().RegisterOnClick( m_MoveUp,  m_Parent, "MoveContainerUp" );
		WidgetEventHandler.GetInstance().RegisterOnClick( m_MoveDown,  m_Parent, "MoveContainerDown" );
		WidgetEventHandler.GetInstance().RegisterOnMouseLeave( m_MoveUp,  this, "MouseLeave" );
		WidgetEventHandler.GetInstance().RegisterOnMouseLeave( m_MoveDown,  this, "MouseLeave" );
		
		WidgetEventHandler.GetInstance().RegisterOnDrag( GetMainWidget() ,  this, "OnDragHeader" );
		WidgetEventHandler.GetInstance().RegisterOnDrop( GetMainWidget() ,  this, "OnDropHeader" );
		
		WidgetEventHandler.GetInstance().RegisterOnMouseEnter( GetMainWidget(),  this, "MouseEnter" );
		WidgetEventHandler.GetInstance().RegisterOnMouseLeave( GetMainWidget(),  this, "MouseLeave" );
		
		float temp;
		GetMainWidget().GetScreenSize( temp, m_SquareSize );
		
		m_DefaultColor			= m_PanelWidget.GetColor();
		m_DefaultFontSize		= 20;
		
		if( GetParent() && GetParent().GetParent() && GetParent().GetParent().GetParent() )
			m_IsInLocalEquipment	= GetParent().GetParent().GetParent().IsInherited( RightArea );
	}
	
	void ~ClosableHeader()
	{
		if( InventoryItem.Cast( m_Entity ) )
			InventoryItem.Cast( m_Entity ).GetOnItemFlipped().Remove( UpdateFlip );
	}

	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.ClosableHeader;
	}
	
	override void SetName( string name )
	{
		name.ToUpper();
		
		m_HeaderText.SetText( name );
		m_HeaderText.Update();
		
		float x, y;
		m_HeaderText.GetScreenSize( x, y );
		m_PanelWidget.SetSize( 1, y + InventoryMenu.GetHeightMultiplied( 10 ) );
	}
	
	void OnDragHeader( Widget w, int x, int y )
	{
		ClosableContainer parent = ClosableContainer.Cast( m_Parent );
		if( parent && GetMainWidget() && m_Entity )
		{
			ItemPreviewWidget item_preview_drag = ItemPreviewWidget.Cast( m_DragRender );
			item_preview_drag.SetFlags(WidgetFlags.EXACTPOS );
			m_DefaultSort = GetRootWidget().GetSort();
			item_preview_drag.GetParent().GetParent().GetParent().GetParent().GetParent().GetParent().GetParent().GetParent().AddChild( GetRootWidget() );
			if( item_preview_drag && m_Entity )
			{
				item_preview_drag.SetItem( m_Entity );
				item_preview_drag.SetView( m_Entity.GetViewIndex() );
			}
			
			ItemManager.GetInstance().SetIsDragging( true );
			
			parent.HideContent( true );
			
			m_PanelWidget.Show( false );
			if( item_preview_drag )
				item_preview_drag.Show( true );
			
			int ww, hh;
			GetGame().GetInventoryItemSize( InventoryItem.Cast( m_Entity ), ww, hh );
			if( item_preview_drag )
			{
				m_Entity.GetOnItemFlipped().Insert( UpdateFlip );
				if( m_Entity.GetInventory().GetFlipCargo() )
				{
					item_preview_drag.SetSize( hh * m_SquareSize, ww * m_SquareSize );
				}
				else
				{
					item_preview_drag.SetSize( ww * m_SquareSize, hh * m_SquareSize );
				}
			}
		}
	}
	
	void UpdateFlip( bool flipped )
	{
		ItemPreviewWidget item_preview_drag = ItemPreviewWidget.Cast( m_DragRender );
		if( InventoryItem.Cast( m_Entity ) && item_preview_drag )
		{
			int ww, hh;
			GetGame().GetInventoryItemSize( InventoryItem.Cast( m_Entity ), ww, hh );
			if( flipped )
			{
				item_preview_drag.SetSize( hh * m_SquareSize, ww * m_SquareSize );
			}
			else
			{
				item_preview_drag.SetSize( ww * m_SquareSize, hh * m_SquareSize );
			}
		}
	}
	
	void OnDropHeader( Widget w )
	{
		ClosableContainer parent = ClosableContainer.Cast( m_Parent );
		parent.GetMainWidget().AddChild( GetRootWidget() );
		GetRootWidget().SetSort( 0 );
		if( parent )
		{
			//
			ItemPreviewWidget item_preview_drag = ItemPreviewWidget.Cast( m_DragRender );
			if( item_preview_drag )
			{
				item_preview_drag.SetItem( null );//TODO: is this safe?
			}
			//
			
			m_PanelWidget.Show( true );
			m_DragRender.Show( false );
			
			parent.ShowContent( true );
			
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().SetIsDragging( false );
		}
		if( InventoryItem.Cast( m_Entity ) )
			InventoryItem.Cast( m_Entity ).GetOnItemFlipped().Remove( UpdateFlip );
	}
	
	bool MouseEnter(Widget w, int x, int y)
	{
		if( m_IsInLocalEquipment && !GetDragWidget() )
		{
			#ifdef PLATFORM_CONSOLE
			m_MovePanel.Show( GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer() );
			#else
			m_MovePanel.Show( true );
			#endif
		}
		PrepareOwnedTooltip( m_Entity, x, y );
		return true;
	}

	bool MouseLeave( Widget w, Widget enter_w, int x, int y	)
	{
		if( enter_w != m_MoveUp && enter_w != m_MoveDown && enter_w != m_CollapseButton )
		{
			if( m_IsInLocalEquipment )
			{
				m_MovePanel.Show( false );
			}
		}
		HideOwnedTooltip();
		return true;
	}
	
	override void SetActive( bool active )
	{
		super.SetActive( active );
		if( active )
		{
			m_PanelWidget.SetColor( ARGBF( 1, 1, 0, 0 ) );
		}
		else
		{
			m_PanelWidget.SetColor( m_DefaultColor );
		}
	}
}
