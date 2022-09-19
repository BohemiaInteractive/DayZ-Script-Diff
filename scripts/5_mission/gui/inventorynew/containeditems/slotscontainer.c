class SlotsContainer: Container
{
	protected ref array<ref SlotsIcon>	m_Icons;
	int m_VisibleColumnCount;
	
	void SlotsContainer( LayoutHolder parent, EntityAI slot_parent ) 
	{
		m_Icons = new array<ref SlotsIcon>;
		for ( int i = 0; i < ITEMS_IN_ROW; i++ )
		{
			if ( GetRootWidget().FindAnyWidget( "Icon" + i ) )
				m_Icons.Insert( new SlotsIcon( this, GetRootWidget().FindAnyWidget( "Icon" + i ), i , slot_parent) );
			else
			{
				Widget child = GetRootWidget().GetChildren();
				while ( child )
				{
					child = child.GetSibling();
				}
			}
		}
	}
	
	override void SetDefaultFocus( bool while_micromanagment_mode = false )
	{			
		m_FocusedColumn = 0;
		for (int i = 0; i < GetColumnCount(); i++)
		{
			SlotsIcon icon = m_Icons[i];
			if (icon.IsVisible())
			{
				m_FocusedColumn = i;
				break;
			}
		}
					
		SetFocus( m_FocusedColumn );
	}
	
	override void SetLastFocus()
	{
		SetDefaultFocus();
	}
	
	void SetFocus( int index )
	{
		if ( index >= 0 && index < m_Icons.Count() )
		{
			m_Icons.Get( index ).GetCursorWidget().Show( true );
		}
		UpdateIcon();
	}
	
	void SetVisibleFocus( int index )
	{
		Unfocus();
		int visible_icons_count = 0;
		
		m_FocusedColumn = 0;
		for (int i = 0; i < GetColumnCount(); i++)
		{
			SlotsIcon icon = m_Icons[i];
			if (icon.IsVisible())
			{
				m_FocusedColumn = i;
				if (visible_icons_count == index)
					break;
				visible_icons_count++;
			}
		}
					
		SetFocus( m_FocusedColumn );
	}
	
	int GetVisibleFocus()
	{
		int visible_icons_count = 0;
		for (int i = 0; i < GetColumnCount(); i++)
		{
			if (m_FocusedColumn == i)
			{
				return visible_icons_count;
			}
			SlotsIcon icon = m_Icons[i];
			if (icon.IsVisible())
			{
				visible_icons_count++;
			}
		}
		return visible_icons_count - 1;
	}
	
	override void SetNextRightActive()
	{
		Unfocus();
		
		int index = m_FocusedColumn + 1;
		for (int i = 0; i < GetColumnCount(); i++ )
		{
			if (index >= GetColumnCount())
			{
				index = 0;
			}
			SlotsIcon icon = m_Icons[index];
			if (icon.IsVisible())
			{
				m_FocusedColumn = index;
				break;
			}
			index++;
		}
		
		UpdateIcon();
	}
	
	override void SetNextLeftActive()
	{
		Unfocus();
		
		int index = m_FocusedColumn - 1;
		for (int i = 0; i < GetColumnCount(); i++ )
		{
			if (index < 0)
			{
				index = GetColumnCount() - 1;
			}
			SlotsIcon icon = m_Icons[index];
			if (icon.IsVisible())
			{
				m_FocusedColumn = index;
				break;
			}
			index--;
		}
		
		UpdateIcon();
	}
	
	void UpdateIcon()
	{	
		float x, y;
		SlotsIcon icon = GetSlotIcon( m_FocusedColumn );
			
		icon.GetCursorWidget().Show( true );
		icon.GetCursorWidget().GetScreenPos( x, y );
	
		EntityAI focused_item = GetFocusedItem();
		if ( focused_item )
		{
			PrepareOwnedTooltip( focused_item, -1, y ); //custom positioning for the controller
		}
		else
		{
			PrepareOwnedSlotsTooltip( icon.GetCursorWidget(), icon.GetSlotDisplayName(), icon.GetSlotDesc(), x, y );
		}
	}
	
	override void UnfocusAll()
	{
		for (int i = 0; i < m_Icons.Count(); i++)
		{
			m_Icons.Get( i ).GetCursorWidget().Show( false );
		}
	}
	
	override void Unfocus()
	{
		Widget w = m_Icons.Get( m_FocusedColumn ).GetCursorWidget();
		if ( w )
		{
			w.Show( false );
		}
	}
	
	override bool SelectItem()
	{
		ItemBase item = ItemBase.Cast( GetFocusedItem() );
		SlotsIcon icon = GetFocusedSlotsIcon();
		
		if( item && !item.IsLockedInSlot() && !icon.IsOutOfReach() && !icon.IsReserved() && item.IsTakeable())
		{
			ItemManager.GetInstance().SetSelectedItem( item, null, icon.GetCursorWidget(), icon );
			return true;
		}
		return false;
	}
	
	override bool IsEmpty()
	{
		return false;
	}
	
	override bool IsDisplayable()
	{
		if (m_ForcedShow)
			return true;
		for (int i = 0; i < m_ColumnCount; i++)
		{
			if ( GetSlotIcon(i).IsVisible() )
			{
				return true;
			}
		}
		return false;
	}
	
	SlotsIcon GetSlotIcon( int index )
	{
		if ( m_Icons.IsValidIndex( index ) )
		{
			return m_Icons.Get( index );
		}
		
		return null;
	}
	
	override SlotsIcon GetFocusedSlotsIcon()
	{
		if (m_Icons.Count())
		{
			return m_Icons.Get(m_FocusedColumn);
		}
		return null;
	}
	
	override EntityAI GetFocusedItem()
	{
		SlotsIcon focus_icon = GetFocusedSlotsIcon();

		if(focus_icon)
		{
			return EntityAI.Cast(focus_icon.GetObject());
		}

		return null;
	}

	override void SetLayoutName()
	{
		#ifdef PLATFORM_CONSOLE
			m_LayoutName = WidgetLayoutName.InventorySlotsContainerXbox;
		#else
			switch ( InventoryMenu.GetWidthType() )
			{
				case ScreenWidthType.NARROW:
				{
					m_LayoutName = WidgetLayoutName.InventorySlotsContainerNarrow;
					break;
				}
				case ScreenWidthType.MEDIUM:
				{
					m_LayoutName = WidgetLayoutName.InventorySlotsContainerMedium;
					break;
				}
				case ScreenWidthType.WIDE:
				{
					m_LayoutName = WidgetLayoutName.InventorySlotsContainerWide;
					break;
				}
			}
		#endif
			
	}
	
	//! Returns 'true' if radial icon is VISIBLE and also OPEN (individual icon or any icon, use parameter..)
	bool VerifySlotsIconVisibility(int idx = -1)
	{
		if (idx != -1)
		{
			return m_Icons[idx].IsVisible() && m_Icons[idx].GetRadialIconPanel().IsVisible() && m_Icons[idx].GetRadialIcon().IsVisible();
		}
		else
		{
			int count = m_Icons.Count();
			for (int i = 0; i < count; i++)
			{
				if (m_Icons[i].IsVisible() && m_Icons[i].GetRadialIconPanel().IsVisible() && m_Icons[i].GetRadialIcon().IsVisible())
				{
					return true;
				}
			}
		}
		return false;
	}
}