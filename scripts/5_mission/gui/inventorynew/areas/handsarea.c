class HandsArea: Container
{
	protected ScrollWidget			m_Scroller;
	protected ref HandsContainer	m_HandsContainer;
	protected ref SizeToChild		m_HandsResizer;
	
	protected bool					m_ShouldChangeSize = true;

	void HandsArea( LayoutHolder parent )
	{
		m_HandsContainer = new HandsContainer( this );
		m_Body.Insert(m_HandsContainer);
		
		if ( m_ImmedUpdate )
			m_MainWidget.Update();
		
		m_ParentWidget.GetScript( m_HandsResizer );
		m_Scroller = ScrollWidget.Cast( m_ParentWidget );
		RecomputeOpenedContainers();
	}

	bool IsSwapActive()
	{
		Widget swap_cursor = m_MainWidget.FindAnyWidget("SwapCursor");
		
		if ( swap_cursor )
			return swap_cursor.IsVisible();
		else
			return false;
	}

	bool IsCombineActive()
	{
		Widget combine_cursor = m_MainWidget.FindAnyWidget("CombineCursor");

		if ( combine_cursor )
			return combine_cursor.IsVisible();
		else
			return false;
	}
	
	override ScrollWidget GetScrollWidget()
	{
		return m_Scroller;
	}
	
	override void UpdateSelectionIcons()
	{
		ScrollToActiveContainer();
	}

	override void UpdateInterval()
	{
		if (!m_Scroller.IsScrollbarVisible())
		{
			m_Scroller.VScrollToPos01(0.0);
		}
		else if (m_Scroller.GetVScrollPos01() > 1.0)
		{
			m_Scroller.VScrollToPos01(1.0);
		}
		m_HandsContainer.UpdateInterval();
		
		float x, y;
		float x2, y2;
		m_Scroller.GetScreenSize( x, y );
		m_MainWidget.GetScreenSize( x2, y2 );
		if ( y2 != y )
			m_ShouldChangeSize = true;
		
		bool changed_size;
		if ( m_ShouldChangeSize && m_HandsResizer.ResizeParentToChild( changed_size, InventoryMenu.GetHeight() * 0.5 ) )
			m_Scroller.SetAlpha( 0.3921 );
		else
			m_Scroller.SetAlpha( 0 );
		
		if ( changed_size )
			m_ShouldChangeSize = false;
	}

	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.HandsArea;
	}

	override void SetParentWidget()
	{
		m_ParentWidget = m_Parent.GetMainWidget().FindAnyWidget( "HandsPanel" );
	}

	override void OnShow()
	{
		super.OnShow();
		m_HandsContainer.OnShow();
	}

	override void Refresh()
	{
		m_ShouldChangeSize = true;
	}
	
	void DraggingOverHandsPanel( Widget w, int x, int y, Widget receiver )
	{
		if ( w == NULL )
		{
			return;
		}
		
		ItemPreviewWidget ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		
		if( !ipw )
		{
			string name = w.GetName();
			name.Replace( "PanelWidget", "Render" );
			ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		}
		
		if( !ipw )
		{
			ipw = ItemPreviewWidget.Cast( w );
		}
		
		if( !ipw || !ipw.IsInherited( ItemPreviewWidget ) )
		{
			return;
		}
		
		if( ipw.GetItem() && GetGame().GetPlayer().GetHumanInventory().CanAddEntityIntoHands( ipw.GetItem() ) )
		{
			ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
		}
		else
		{
			ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
		}
	}
	
	void OnHandsPanelDropReceived( Widget w, int x, int y, Widget receiver )
	{
		ItemPreviewWidget ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		if( !ipw )
		{
		  string name = w.GetName();
		  name.Replace( "PanelWidget", "Render" );
		  ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		}
		
		if( !ipw )
		{
			ipw = ItemPreviewWidget.Cast( w );
		}
		
		if( !ipw || !ipw.GetItem() || !ipw.IsInherited( ItemPreviewWidget ) )
		{
			return;
		}
		
		if( !ipw.GetItem().GetInventory().CanRemoveEntity() )
			return;
		
		if( GetGame().GetPlayer().GetHumanInventory().CanAddEntityIntoHands( ipw.GetItem() ) )
		{
			ItemBase item_base = ItemBase.Cast( ipw.GetItem() );
			float stackable = item_base.GetTargetQuantityMax(-1);
			
			if( stackable == 0 || item_base.GetQuantity() <= stackable )
			{
				GetGame().GetPlayer().PredictiveTakeEntityToHands( item_base );		
			}
			else if( stackable != 0 && stackable <= item_base.GetQuantity() )
			{
				item_base.SplitIntoStackMaxHandsClient( PlayerBase.Cast( GetGame().GetPlayer() ) );
			}
		}
	}
}