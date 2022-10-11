class LeftArea: Container
{
	protected Widget					m_UpIcon;
	protected Widget					m_DownIcon;
	protected Widget					m_ContentParent;
	protected ref VicinityContainer		m_VicinityContainer;
	protected ScrollWidget				m_ScrollWidget;
	protected ref SizeToChild			m_ContentResize;
	protected bool						m_ShouldChangeSize = true;
	protected bool						m_IsProcessing = false; // Prevents refreshing every time a child is added while it is still processing
	
	void LeftArea(LayoutHolder parent )
	{
		m_MainWidget.Show(true, false);
		
		m_ContentParent	= m_MainWidget.FindAnyWidget("ContentParent");
		m_ContentParent.GetScript(m_ContentResize);
		
		m_ScrollWidget	= ScrollWidget.Cast(m_MainWidget.FindAnyWidget("Scroller"));
		m_MainWidget	= m_MainWidget.FindAnyWidget("Content");
		
		m_UpIcon		= m_RootWidget.FindAnyWidget("Up");
		m_DownIcon		= m_RootWidget.FindAnyWidget("Down");
		
		m_VicinityContainer = new VicinityContainer(this, false);
		m_Body.Insert(m_VicinityContainer);
		m_ActiveIndex = 0;
		
		WidgetEventHandler.GetInstance().RegisterOnChildAdd(m_MainWidget, this, "OnChildAdd");
		WidgetEventHandler.GetInstance().RegisterOnChildRemove(m_MainWidget, this, "OnChildRemove");
		RecomputeOpenedContainers();
	}
	
	override void UnfocusGrid()
	{
		Container active_container;
		for ( int i = 0; i < m_OpenedContainers.Count(); i++ )
		{
			active_container = Container.Cast( m_OpenedContainers.Get( i ) );
			active_container.UnfocusGrid();
		}
	}
	
	override bool IsActive()
	{
		if( m_OpenedContainers.Count() <= m_ActiveIndex )
		{
			m_ActiveIndex = 0;
		}
		Container active_container = Container.Cast( m_OpenedContainers.Get( m_ActiveIndex ) );
		return active_container.IsActive( );
	}
	
	override void SetActive( bool active )
	{
		super.SetActive( active );
		
		UpdateSelectionIcons();
	}
	
	override ScrollWidget GetScrollWidget()
	{
		return m_ScrollWidget;
	}
	
	override void UpdateSelectionIcons()
	{
		ScrollToActiveContainer();
		m_UpIcon.Show( m_IsActive );
		m_DownIcon.Show( m_IsActive );
		if( m_IsActive )
		{
			float x, y;
			m_UpIcon.GetScreenSize( x, y );
			
			float top_y		= GetCurrentContainerTopY();
			m_UpIcon.SetPos( 0, Math.Clamp( top_y, 0, 99999 ) );
			
			#ifndef PLATFORM_CONSOLE
			float x2, y2;
			m_DownIcon.GetScreenSize( x2, y2 );
			float bottom_y	= GetCurrentContainerBottomY() - y2;
			
			float diff		= bottom_y - ( top_y + y );
			if( diff < 0 )
			{
				top_y += diff / 2;
				bottom_y -= diff / 2;
			}
			m_DownIcon.SetPos( 0, bottom_y );
			#endif
		}
	}
	
	float GetCurrentContainerTopY()
	{
		float x, y, cont_screen_pos;
		m_MainWidget.GetScreenPos( x, y );
		if ( m_OpenedContainers.IsValidIndex( m_ActiveIndex ) )
			cont_screen_pos = Container.Cast( m_OpenedContainers.Get( m_ActiveIndex ) ).GetFocusedContainerYScreenPos();

		return cont_screen_pos - y;
	}
	
	float GetCurrentContainerBottomY()
	{
		float x, y, cont_screen_pos, cont_screen_height;
		m_MainWidget.GetScreenPos( x, y );
		
		if ( m_OpenedContainers.IsValidIndex( m_ActiveIndex ) )
		{
			cont_screen_pos = Container.Cast( m_OpenedContainers.Get( m_ActiveIndex ) ).GetFocusedContainerYScreenPos();
			cont_screen_height = Container.Cast( m_OpenedContainers.Get( m_ActiveIndex ) ).GetFocusedContainerHeight();
		}
			
		return cont_screen_pos - y + cont_screen_height;
	}
	
	override void ExpandCollapseContainer()
	{
		Container c = GetFocusedContainer();		
		if (c)
		{
			c.ExpandCollapseContainer();
		}
		
		Refresh();
	}
	
	void OnLeftPanelDropReceived( Widget w, int x, int y, Widget receiver )
	{
		m_VicinityContainer.OnLeftPanelDropReceived( w, x, y, receiver );
	}
	
	override void DraggingOverHeader( Widget w, int x, int y, Widget receiver )
	{
		m_VicinityContainer.DraggingOverHeader( w, x, y, receiver );
	}
	
	override void SetLayoutName()
	{
		#ifdef PLATFORM_CONSOLE
			m_LayoutName = WidgetLayoutName.LeftAreaXbox;
		#else
			switch ( InventoryMenu.GetWidthType() )
			{
				case ScreenWidthType.NARROW:
				{
					m_LayoutName = WidgetLayoutName.LeftAreaNarrow;
					break;
				}
				case ScreenWidthType.MEDIUM:
				{
					m_LayoutName = WidgetLayoutName.LeftAreaMedium;
					break;
				}
				case ScreenWidthType.WIDE:
				{
					m_LayoutName = WidgetLayoutName.LeftAreaWide;
					break;
				}
			}
		#endif
			
	}
	
	override EntityAI GetFocusedItem()
	{
		EntityAI item = GetFocusedContainer().GetFocusedItem();
		return item;
	}
	
	VicinityContainer GetVicinityContainer()
	{
		return m_VicinityContainer;
	}
	
	override void SetParentWidget()
	{
		m_ParentWidget = m_Parent.GetMainWidget().FindAnyWidget( "LeftPanel" );
	}
	
	override void OnShow()
	{
		super.OnShow();
		Refresh();
	}
	
	override void Refresh()
	{
		super.Refresh();
		
		m_MainWidget.Update();
		m_RootWidget.Update();
		m_ScrollWidget.Update();
		
		UpdateSelectionIcons();
		m_ShouldChangeSize = true;
	}
	
	override void UpdateInterval()
	{
		m_IsProcessing = true;
		super.UpdateInterval();
		m_IsProcessing = false;
		
		float x, y;
		float x2, y2;
		m_ContentParent.GetScreenSize( x, y );
		m_MainWidget.GetScreenSize( x2, y2 );
		if ( y2 != y )
			m_ShouldChangeSize = true;
		bool changed_size;
		//if ( m_ShouldChangeSize )
			m_ContentResize.ResizeParentToChild( changed_size );
		if ( changed_size || m_ShouldChangeSize )
		{
			m_MainWidget.Update();
			m_RootWidget.Update();
			m_ScrollWidget.Update();
			m_ShouldChangeSize = false;
		}
		CheckScrollbarVisibility();
	}

	override bool OnChildRemove( Widget w, Widget child )
	{
		if (!m_IsProcessing)
			Refresh();
		return true;
	}
	
	override bool OnChildAdd( Widget w, Widget child )
	{
		if (!m_IsProcessing)
			Refresh();
		return true;
	}
}
