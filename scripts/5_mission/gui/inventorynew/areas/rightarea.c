class RightArea: Container
{
	ref PlayerContainer	m_PlayerContainer;
	protected Widget	m_ContentParent;
	
	protected ScrollWidget						m_ScrollWidget;
	protected Widget							m_UpIcon;
	protected Widget							m_DownIcon;
	
	protected ref SizeToChild					m_ContentResize;
	protected bool								m_ShouldChangeSize = true;
	protected bool								m_ProcessGridMovement;
	
	void RightArea(LayoutHolder parent)
	{
		m_MainWidget.Show(true);
		m_ScrollWidget	= ScrollWidget.Cast( m_MainWidget.FindAnyWidget( "Scroller" ) );
		m_MainWidget		= m_MainWidget.FindAnyWidget("Content");
		//m_ContentParent		= m_RootWidget.FindAnyWidget("ContentParent");
		m_PlayerContainer	= new PlayerContainer(this, false);
		m_PlayerContainer.SetPlayer(PlayerBase.Cast(GetGame().GetPlayer()));
		m_Body.Insert(m_PlayerContainer);
		m_ActiveIndex = 0;
		m_ProcessGridMovement = false;
		
		m_UpIcon		= m_RootWidget.FindAnyWidget( "Up" );
		m_DownIcon		= m_RootWidget.FindAnyWidget( "Down" );
		
		//m_ContentParent.GetScript( m_ContentResize );
		
		RecomputeOpenedContainers();
	}
	
	PlayerContainer GetPlayerContainer()
	{
		return m_PlayerContainer;
	}
	
	override void DraggingOverHeader(Widget w, int x, int y, Widget receiver)
	{
		m_PlayerContainer.DraggingOverHeader(w, x, y, receiver);
	}
	
	override bool Select()
	{
		return m_PlayerContainer.Select();
	}
	
	override bool SelectItem()
	{
		return m_PlayerContainer.SelectItem();
	}
	
	override bool Combine()
	{
		return m_PlayerContainer.Combine();
	}
	
	override bool SplitItem()
	{
		return m_PlayerContainer.SplitItem();
	}
	
	override bool EquipItem()
	{
		return m_PlayerContainer.EquipItem();
	}
	
	override bool TransferItem()
	{
		return m_PlayerContainer.TransferItem();
	}
	
	override bool InspectItem()
	{
		return m_PlayerContainer.InspectItem();
	}
	
	override bool TransferItemToVicinity()
	{
		return m_PlayerContainer.TransferItemToVicinity();
	}
	
	override void UnfocusGrid()
	{
		m_PlayerContainer.UnfocusGrid();
	}
	
	bool HasEntityContainerVisible(EntityAI entity)
	{
		return m_PlayerContainer.HasEntityContainerVisible(entity);
	}
	
	void SwapItemsInOrder(int slot1, int slot2)
	{
		m_PlayerContainer.SwapItemsInOrder(slot1, slot2);
	}
	
	bool IsPlayerEquipmentActive()
	{
		return m_PlayerContainer.IsPlayerEquipmentActive();
	}
	
	void ExpandCollapseContainer()
	{
		m_PlayerContainer.ExpandCollapseContainer();
	}
	
	override void Refresh()
	{
		m_ScrollWidget.Update();
		m_PlayerContainer.Refresh();
		m_ShouldChangeSize = true;
	}

	override void SetLayoutName()
	{
		#ifdef PLATFORM_CONSOLE
			m_LayoutName = WidgetLayoutName.RightAreaXbox;
		#else
			switch (InventoryMenu.GetWidthType())
			{
			case ScreenWidthType.NARROW:
				m_LayoutName = WidgetLayoutName.RightAreaNarrow;
			break;
			case ScreenWidthType.MEDIUM:
				m_LayoutName = WidgetLayoutName.RightAreaMedium;
			break;
			case ScreenWidthType.WIDE:
				m_LayoutName = WidgetLayoutName.RightAreaWide;
			break;
			}
		#endif
			
	}

	override void SetParentWidget()
	{
		m_ParentWidget = m_Parent.GetMainWidget().FindAnyWidget("RightPanel");
	}

	override void OnShow()
	{
		super.OnShow();
		Refresh();
	}
	
	override void UpdateInterval()
	{
		super.UpdateInterval();
		m_PlayerContainer.UpdateInterval();
		
		CheckScrollbarVisibility();
	}
	
		
	override void MoveGridCursor(int direction)
	{
		super.MoveGridCursor(direction);
		m_ProcessGridMovement = true;
		UpdateSelectionIcons();
	}
	
	void MoveUpDownIcons()
	{
		if (m_UpIcon && m_DownIcon)
		{
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
		/*else
		{
			ErrorEx("up/down icons not present!");
		}*/
	}

	void ScrollToActiveContainer()
	{
		if (m_ProcessGridMovement)
		{
			float x, y, y_s;
			m_ScrollWidget.GetScreenPos( x, y );
			m_ScrollWidget.GetScreenSize( x, y_s );
			float target_pos = y + y_s;
			float f_y,f_h;
			float amount;
			f_y = GetFocusedContainerYScreenPos( true );
			f_h = GetFocusedContainerHeight( true );
			float next_pos	= f_y + f_h;
			
			if( next_pos > ( y + y_s ) )
			{
				amount	= y + f_y;
				m_ScrollWidget.VScrollToPos( m_ScrollWidget.GetVScrollPos() + amount + 2 );
			}
			else if( f_y < y )
			{
				amount = f_y - y;
				m_ScrollWidget.VScrollToPos( m_ScrollWidget.GetVScrollPos() + amount - 2 );
			}
			m_ProcessGridMovement = false;
		}
		else if (!m_ScrollWidget.IsScrollbarVisible())
		{
			m_ScrollWidget.VScrollToPos01( 0.0 );
		}
		else if (m_ScrollWidget.GetVScrollPos01() > 1.0)
		{
			m_ScrollWidget.VScrollToPos01( 1.0 );
		}
	}
	
	override void UpdateSelectionIcons()
	{
		ScrollToActiveContainer();
		MoveUpDownIcons();
	}
	
	float GetCurrentContainerTopY()
	{
		float x, y;
		GetMainWidget().Update();
		GetMainWidget().GetScreenPos( x, y );
		float cont_screen_pos = GetFocusedContainerYScreenPos();
		
		return cont_screen_pos - y;
	}
	
	float GetCurrentContainerBottomY()
	{
		float x, y;
		GetMainWidget().Update();
		GetMainWidget().GetScreenPos( x, y );
		
		float cont_screen_pos		= GetFocusedContainerYScreenPos();
		float cont_screen_height	= GetFocusedContainerHeight();
		return cont_screen_pos - y + cont_screen_height;
	}
	
	void CheckScrollbarVisibility()
	{
		m_MainWidget.Update();
		m_RootWidget.Update();
		m_ScrollWidget.Update();
		
		UpdateSelectionIcons();
	}
}