class Container extends LayoutHolder
{
	protected ref array<ref LayoutHolder>	m_Body;
	protected ref array<LayoutHolder>		m_OpenedContainers;
	protected int							m_ActiveIndex = 0;
	protected bool							m_LastIndex; //deprecated 
	protected bool							m_Closed;
	protected Container 					m_FocusedContainer;
	protected float							m_PrevAlpha;
	const int ITEMS_IN_ROW = 8;
	
	//protected int							m_RowCount;
	protected int							m_ColumnCount;
	
	protected int							m_FocusedColumn = 0;
	protected bool							m_ForcedHide;
	protected bool							m_ForcedShow; //used to override displayability condition, but 'm_ForcedHide' takes preference
	
	protected SlotsIcon						m_SlotIcon;
	protected EntityAI						m_Entity;
	
	const int SORT_ATTACHMENTS_OWN = 1; //direct attachments of the parent item
	const int SORT_CARGO_OWN = 2; //cargo of the parent item
	const int SORT_ATTACHMENTS_NEXT_OFFSET = 2;
	const int SORT_CARGO_NEXT_OFFSET = 3;

	void OnDropReceivedFromHeader( Widget w, int x, int y, Widget receiver );
	void DraggingOver( Widget w, int x, int y, Widget receiver );
	void DraggingOverHeader( Widget w, int x, int y, Widget receiver );
	void UpdateSpacer();
	Header GetHeader();
	void SetHeader(Header header);
	void CheckHeaderDragability();
	
	void Container( LayoutHolder parent )
	{
		m_Body = new array<ref LayoutHolder>;
		m_OpenedContainers = new array<LayoutHolder>;
		m_PrevAlpha = m_RootWidget.GetAlpha();
		m_SlotIcon = null;
		m_ForcedHide = false;
		m_ForcedShow = false;
		
		m_ActiveIndex = 0;
		m_IsActive = false;
	}
	
	Container GetFocusedContainer()
	{
		if (m_ActiveIndex < m_OpenedContainers.Count())
		{
			return Container.Cast(m_OpenedContainers[m_ActiveIndex]);
		}
		return null;
	}
	
	Container GetContainer(int index)
	{
		if (index < m_Body.Count())
		{
			return Container.Cast( m_Body[index] );
		}
		return null;
	}
	
	
	void SetFocusedContainer( Container cont )
	{
		m_FocusedContainer = cont;
	}
	
	SlotsIcon GetFocusedSlotsIcon()
	{
		Container c = GetFocusedContainer();
		if (c)
		{
			return c.GetFocusedSlotsIcon();
		}
		return null;
	}
	
	int GetActiveIndex()
	{
		return m_ActiveIndex;
	}
	
	void SetActiveIndex( int index )
	{
		m_ActiveIndex = index;
	}
	
	ScrollWidget GetScrollWidget()
	{
		return null;
	}
	
	void UpdateRadialIcon()
	{
		if ( m_SlotIcon )
		{
			m_SlotIcon.GetRadialIconPanel().Show( false );
		}
	}
	
	void SetSlotIcon( SlotsIcon icon )
	{
		m_SlotIcon = icon;
	}
	
	void SetDefaultFocus(bool while_micromanagment_mode = false)
	{
		m_ActiveIndex = 0;
	}
	
	void SetLastFocus()
	{
		m_ActiveIndex = 0;
		m_FocusedColumn = 0;
		if ( m_OpenedContainers.Count() > 0 )
		{
			m_ActiveIndex = m_OpenedContainers.Count() - 1;
		}
	}
	
	void Unfocus()
	{
	}
	
	void MoveGridCursor(int direction)
	{
		if ( direction == Direction.UP )
		{
			SetPreviousActive();
		}
		else if ( direction == Direction.DOWN )
		{
			SetNextActive();
		}
		else if ( direction == Direction.RIGHT )
		{
			SetNextRightActive();
		}
		else if ( direction == Direction.LEFT )
		{
			SetNextLeftActive();
		}
		
		UpdateSelectionIcons();
		
		Inventory.GetInstance().UpdateConsoleToolbar();
	}

	void ScrollToActiveContainer()
	{
		ScrollWidget sw = GetScrollWidget();
		if (sw)
		{
			float x, y, y_s;
			
			sw.GetScreenPos( x, y );
			sw.GetScreenSize( x, y_s );
			float f_y,f_h;
			float amount;
			f_y = GetFocusedContainerYScreenPos( true );
			f_h = GetFocusedContainerHeight( true );
			
			float next_pos	= f_y + f_h;
				
			if (next_pos > ( y + y_s ))
			{
				amount	=  sw.GetVScrollPos() + next_pos - ( y + y_s ) + 2;
				sw.VScrollToPos( amount );
			}
			else if (f_y < y)
			{
				amount = sw.GetVScrollPos() + f_y - y - 2;
				sw.VScrollToPos(amount);
			}
			
			//CheckScrollbarVisibility();
		}
	}
	
	void CheckScrollbarVisibility()
	{
		ScrollWidget sw = GetScrollWidget();
		if (sw)
		{
			if (!sw.IsScrollbarVisible())
			{
				sw.VScrollToPos01(0.0);
			}
			else if (sw.GetVScrollPos01() > 1.0)
			{
				sw.VScrollToPos01(1.0);
			}
		}
	}
	
	void Open()
	{
		m_Closed = false;
		UpdateSelectionIcons();
	}
	
	void Close()
	{
		m_Closed = true;
		UpdateSelectionIcons();
	}
	
	bool IsOpened()
	{
		return !m_Closed;
	}
	
	void SetOpenForSlotIcon(bool open, SlotsIcon icon = null/*m_SlotIcon*/)
	{
		if (!icon)
		{
			icon = m_SlotIcon;
		}
		
		if (icon)
		{
			icon.GetRadialIcon().Show(open);
			icon.GetRadialIconClosed().Show(!open);
		}
		/*else
		{
			ErrorEx("Dbg No Icon");
		}*/
	}
	
	void Toggle()
	{
		if (IsOpened())
		{
			Close();
		}
		else
		{
			Open();
		}
		SetOpenForSlotIcon(IsOpened());
	}
	
	float GetFocusedContainerHeight( bool contents = false )
	{
		float x, y, result;
		if( GetFocusedContainer() )
			y = GetFocusedContainer().GetFocusedContainerHeight( contents );
		else if( GetRootWidget() )
			GetRootWidget().GetScreenSize( x, y );
		
		result = y;
		
		if ( m_ActiveIndex == 0 )
		{
			if ( GetHeader() )
			{
				GetHeader().GetRootWidget().GetScreenSize( x, y );
				result += y;
			}
		}
		return result;
	}
	
	float GetFocusedContainerYPos( bool contents = false )
	{
		float x, y;
		if( GetFocusedContainer() )
			y = GetFocusedContainer().GetFocusedContainerYPos( contents );
		else if( GetRootWidget() )
			GetRootWidget().GetPos( x, y );
		
		return y;
	}
	
	float GetFocusedContainerYScreenPos( bool contents = false )
	{
		float x, y, result;
		if( GetFocusedContainer() )
			y = GetFocusedContainer().GetFocusedContainerYScreenPos( contents );
		else if( GetRootWidget() )
			GetRootWidget().GetScreenPos( x, y );
		
		
		result = y;
		
		if ( m_ActiveIndex == 0 )
		{
			if ( GetHeader() )
			{
				GetHeader().GetRootWidget().GetScreenPos( x, y );
				result = y;
			}
		}
		return result;
	}

	int Count()
	{
		return m_Body.Count();
	}
	
	bool SelectItem()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().SelectItem();
		return false;
	}
	
	bool Select()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().Select();
		return false;
	}
	
	bool OnSelectButton()
	{
		if(GetFocusedContainer())
		{
			return GetFocusedContainer().OnSelectButton();
		}
		return false;
	}
	
	bool Combine()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().Combine();
		return true;
	}
	
	bool TransferItemToVicinity()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().TransferItemToVicinity();
		return false;
	}
	
	bool TransferItem()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().TransferItem();
		return false;
	}
	
	bool InspectItem()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().InspectItem();
		return false;
	}
	
	bool SplitItem()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().SplitItem();
		return false;
	}
	
	bool EquipItem()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().EquipItem();
		return false;
	}
	
	bool CanEquip()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().CanEquip();
		return false;
	}
	
	bool CanCombine()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().CanCombine();
		return false;
	}
	
	bool CanCombineAmmo()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().CanCombineAmmo();
		return false;
	}
	
	bool IsEmpty()
	{
		return m_OpenedContainers.Count() == 0;
	}
	
	bool IsItemActive()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().IsItemActive();
		return false;
	}

	bool IsItemWithQuantityActive()
	{
		if( GetFocusedContainer() )
			return GetFocusedContainer().IsItemWithQuantityActive();
		return false;
	}
	
	EntityAI GetFocusedItem()
	{
		EntityAI item;
		if( GetFocusedContainer() )
			item = GetFocusedContainer().GetFocusedItem();

		return item;
	}
	
	EntityAI GetFocusedContainerEntity()
	{
		EntityAI item;
		if( GetFocusedContainer() )
			item = GetFocusedContainer().GetFocusedContainerEntity();
		return item;
	}
	
	int GetColumnCount()
	{
		return m_ColumnCount;
	}
	
	void SetColumnCount( int count )
	{
		m_ColumnCount = count;
	}
	
	int GetFocusedColumn()
	{
		return m_FocusedColumn;
	}
	
	void SetFocusedColumn( int column )
	{
		m_FocusedColumn = column;
	}
	
	override void UpdateInterval()
	{
		for ( int i = 0; i < m_Body.Count(); i++ )
		{
			if ( m_Body.Get( i ) )
				m_Body.Get( i ).UpdateInterval();
		}
		
		CheckHeaderDragability();
	}

	override void SetLastActive()
	{
		if (m_IsActive)
		{
			m_IsActive = true;
			if (m_OpenedContainers.Count())
			{
				SetLastFocus();
				if (!m_OpenedContainers[m_ActiveIndex].IsActive())
				{
					for (int i = 0; i < m_OpenedContainers.Count() - 1; i++)
					{
						if (m_OpenedContainers[i].IsActive())
						{
							m_OpenedContainers[i].SetActive(false);
						}
					}
					m_OpenedContainers[m_ActiveIndex].SetLastActive();
				}
				else
				{
					m_OpenedContainers[m_ActiveIndex].SetLastActive();
				}
			}
		}
		else
		{
			m_IsActive = true;
			if (GetHeader())
			{
				GetHeader().SetActive(m_IsActive);
			}
			SetLastFocus();
			if (m_OpenedContainers.Count())
			{
				Container c = Container.Cast(m_OpenedContainers.Get( m_ActiveIndex ));
				if (c)
				{
					c.SetLastActive( );
				}
			}
		}
	}
	
	override void SetFirstActive()
	{
		if (!m_IsActive)
		{
			SetActive(true);
		}
		else
		{
			if (m_OpenedContainers.Count())
			{
				SetDefaultFocus();
				if (!m_OpenedContainers[m_ActiveIndex].IsActive())
				{
					for (int i = 1; i < m_OpenedContainers.Count(); i++)
					{
						if (m_OpenedContainers[i].IsActive())
						{
							m_OpenedContainers[i].SetActive(false);
						}
					}
					m_OpenedContainers[m_ActiveIndex].SetActive(true);
				}
				else
				{
					m_OpenedContainers[m_ActiveIndex].SetFirstActive();
				}
			}
		}
	}
	
	override void SetActive(bool active)
	{
		if (!active)
		{
			HideOwnedTooltip();
		}
		
		if (!active && !m_IsActive)
			return;
		
		super.SetActive( active );
		if ( GetHeader() )
		{
			GetHeader().SetActive(active);
		}

		if(m_MainWidget.FindAnyWidget("SelectedContainer"))
		{
			m_MainWidget.FindAnyWidget("SelectedContainer").Show(active);
		}
		
		Container c;
		if( active )
		{

			SetDefaultFocus();
			if( m_OpenedContainers.Count() )
			{				
				c = Container.Cast(m_OpenedContainers.Get( m_ActiveIndex ));
				if (c)
				{
					c.SetActive(active);
				}
			}
		}
		else
		{
			c = GetFocusedContainer();
			if (c)
			{
				GetFocusedContainer().SetActive(false);
			}
			Unfocus();
			m_ActiveIndex = 0;
			m_FocusedColumn = 0;
		}
	}

	void UnfocusAll()
	{
		for ( int i = 0; i < Count(); i++ )
		{
			for ( int j = 0; j < ITEMS_IN_ROW; j++ )
			{
				if( Get( i ) && Get( i ).GetMainWidget() && Get( i ).GetMainWidget().FindAnyWidget( "Cursor" + j ) )
					Get( i ).GetMainWidget().FindAnyWidget( "Cursor" + j ).Show( false );
			}
		}
	}

	void UnfocusGrid()
	{
		if( GetFocusedContainer() )
		{
			m_FocusedColumn = 0;
			GetFocusedContainer().UnfocusAll();
		}
	}

	bool IsLastIndex()
	{
		return m_ActiveIndex == ( m_OpenedContainers.Count() - 1 );
	}
	
	bool IsFirstIndex()
	{
		return m_ActiveIndex == 0; 
	}
	
	bool IsFirstContainerFocused()
	{
		return m_ActiveIndex == 0;
	}
	
	bool IsLastContainerFocused()
	{
		return m_ActiveIndex >= ( m_OpenedContainers.Count() - 1 );
	}
	
	void ResetFocusedContainer()
	{
		if ( GetFocusedContainer() )
		{
			GetFocusedContainer().ResetFocusedContainer();
		}
		
		m_ActiveIndex == 0;
	}

	void SetNextActive()
	{
		HideOwnedTooltip();

		Container active;
		if (m_OpenedContainers.Count())
		{
			active = Container.Cast(m_OpenedContainers[m_ActiveIndex]);
		}		
		
		if (active && active.IsActive())
		{
			active.SetNextActive();
		}
		if (!active || !active.IsActive())
		{
			Container next;
			if (!IsLastContainerFocused())
			{
				m_ActiveIndex++;
				
				next = Container.Cast(m_OpenedContainers[m_ActiveIndex]);
				next.SetActive(true);
			} 
			else if (Container.Cast( GetParent() ))
			{
				SetActive(false);
			}
			else
			{
				SetActive(false);
				SetFirstActive();
			}
		}
	}

	void SetPreviousActive(bool force = false)
	{
		HideOwnedTooltip();
		Container active;
		if (m_OpenedContainers.Count())
		{
			active = Container.Cast(m_OpenedContainers[m_ActiveIndex]);
		}
		
		if (active && active.IsActive())
		{
			active.SetPreviousActive();
		}
		
		if (!active || !active.IsActive())
		{
			Container prev;
			if (!IsFirstContainerFocused())
			{
				m_ActiveIndex--;
				
				prev = Container.Cast(m_OpenedContainers[m_ActiveIndex]);
				prev.SetLastActive();
			} 
			else if (Container.Cast( GetParent() ))
			{
				SetActive(false);
			}
			else
			{
				SetActive(false);
				SetLastActive();
			}			
		}
	}
	
	void SetNextRightActive()
	{
		Container active;
		if (m_OpenedContainers.Count())
		{
			active = Container.Cast(m_OpenedContainers[m_ActiveIndex]);
		}
		
		if (active)
		{
			active.SetNextRightActive();
		}
	}
	
	void SetNextLeftActive()
	{
		Container active;
		if (m_OpenedContainers.Count())
		{
			active = Container.Cast(m_OpenedContainers[m_ActiveIndex]);
		}
		
		if (active)
		{
			active.SetNextLeftActive();
		}
		
	}
	
	void RecomputeOpenedContainers()
	{
		m_OpenedContainers.Clear();
		int i;
		bool need_reset_focus = false;
		Container c;
		for (i = 0; i < m_Body.Count(); i++)
		{
			c = Container.Cast(m_Body.Get( i ));
			if ( c ) 
			{
				c.RecomputeOpenedContainers();
				if (c.IsDisplayable() && c.IsVisible())
				{
					m_OpenedContainers.Insert(c);
				}
				else if (c.IsActive())
				{
					c.SetActive(false);
					need_reset_focus = true;
				}
				
			}	
		}
		
		//In case of removing focused container or change order of containers
		if (IsActive())
		{
			if (!need_reset_focus && ( m_ActiveIndex >= m_OpenedContainers.Count() || !m_OpenedContainers[m_ActiveIndex].IsActive() ))
			{
				need_reset_focus = true;
				for (i = 0; i < m_OpenedContainers.Count(); i++)
				{
					if (m_OpenedContainers[i].IsActive())
					{
						need_reset_focus = false;
						m_ActiveIndex = i;
					}
				}
			}
			
			if (need_reset_focus)
			{
				SetFirstActive();
			}
		}
	}

	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.Container;
	}

	void Insert( LayoutHolder container, int pos = -1, bool immedUpdate = true )
	{
		if ( pos > -1 && pos < m_Body.Count() )
		{
			if ( pos <= m_ActiveIndex )
				m_ActiveIndex++;
			m_Body.InsertAt( container, pos );
		}
		else
			m_Body.Insert( container );
		
		if ( immedUpdate )
			Refresh();
	}

	void Remove( LayoutHolder container )
	{
		if( m_Body )
		{
			int index = m_Body.Find( container );
			if( index > -1 )
			{
				index = m_OpenedContainers.Find( container );
				if (index > -1)
				{
					if (index <= m_ActiveIndex)
					{
						if (GetFocusedContainer() == container)
						{
							SetPreviousActive( true );
						}
						else
						{
							m_ActiveIndex--;
						}
					}
					m_OpenedContainers.RemoveItem( container );
				}
				m_Body.RemoveItem( container );
			}
		}
		
		Refresh();
	}
	
	LayoutHolder Get( int x )
	{
		return m_Body.Get( x );
	}

	override void Refresh()
	{
		for ( int i = 0; i < m_Body.Count(); i++ )
		{
			if( m_Body.Get( i ) )
				m_Body.Get( i ).Refresh();
		}
	}

	void UpdateBodySpacers()
	{
		for ( int i = 0; i < m_Body.Count(); i++ )
		{
			Container c = Container.Cast( m_Body.Get( i ) );
			if( c && c.IsInherited( Container ) )
			{
				c.UpdateSpacer();
			}
		}
		
		UpdateSpacer();
	}

	void HideContent( bool force_hide = false )
	{
		if( !m_ForcedHide )
		{
			m_ForcedHide = force_hide;
		}
		for(int i = 0; i < m_Body.Count(); i++)
		{
			if( m_Body.Get( i ) )
				m_Body.Get( i ).OnHide();
		}
	}
	
	void ShowContent( bool force_show = false )
	{
		if( force_show )
			m_ForcedHide = false;
		
		if( !m_ForcedHide )
		{
			for(int i = 0; i < m_Body.Count(); i++)
			{
				if( m_Body.Get( i ) )
				m_Body.Get( i ).OnShow();
			}
		}
	}
	
	void SetForceShow(bool value)
	{
		m_ForcedShow = value;
	}
	
	override void UpdateSelectionIcons()
	{
		m_Parent.UpdateSelectionIcons();
	}
	
	void ExpandCollapseContainer(){}
}
