class ClosableContainer extends Container
{
	protected ref ClosableHeader	m_ClosableHeader;
	protected bool					m_LockCargo;

	void ClosableContainer( LayoutHolder parent, int sort = -1 )
	{
		m_Body				= new array<ref LayoutHolder>;
		m_ClosableHeader	= new ClosableHeader( this, "CloseButtonOnMouseButtonDown" );
		
		if( sort > -1 )
			m_RootWidget.SetSort( sort + 2 );
		
		m_MainWidget = m_MainWidget.FindWidget( "body" );
	}
	
	override bool IsDisplayable()
	{
		for(int i = 0; i < m_Body.Count(); i++)
		{
			LayoutHolder c = m_Body[i];
			if( c && c.IsDisplayable())
				return true;
		}
		return false;
	}
	
	override void UpdateRadialIcon()
	{
		if ( m_SlotIcon )
		{
			bool show_radial_icon;
			show_radial_icon = IsOpened();
			show_radial_icon = show_radial_icon && ( ( m_Entity.GetInventory().GetCargo() && m_Entity.CanDisplayCargo()) || m_Entity.GetSlotsCountCorrect() > 0 );
			show_radial_icon = show_radial_icon && !m_Entity.GetInventory().IsInventoryLockedForLockType( HIDE_INV_FROM_SCRIPT );
			if ( IsDisplayable() )
			{
				m_SlotIcon.GetRadialIconPanel().Show( true );
				SetOpenForSlotIcon(show_radial_icon);
			}
			else
			{
				m_SlotIcon.GetRadialIconPanel().Show( false );
			}
		}
	}
	
	void SetOpenState( bool state )
	{
		ItemManager.GetInstance().SetDefaultOpenState( m_Entity.GetType(), state );
		m_Closed = !state;
		if( !m_Closed )
		{
			OnShow();
		}
		else
		{
			OnHide();
		}
	}
	
	override Header GetHeader()
	{
		return m_ClosableHeader;
	}
	
	override void Open()
	{
		if( IsDisplayable() )
		{
			super.Open();
			ItemManager.GetInstance().SetDefaultOpenState( m_Entity.GetType(), true );
			SetOpenForSlotIcon(true);
			OnShow();
			m_Parent.m_Parent.Refresh();
		}
	}

	override void Close()
	{
		ItemManager.GetInstance().SetDefaultOpenState( m_Entity.GetType(), false );
		super.Close();
		SetOpenForSlotIcon(false);
		OnHide();
		m_Parent.m_Parent.Refresh(); //TODO: ???
	}
	
	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.ClosableContainer;
	}

	override void OnShow()
	{
		if( IsOpened() )
		{
			super.OnShow();
		}
	}

	override LayoutHolder Get( int x )
	{
		if( m_Body && x < m_Body.Count() && x >= 0 )
			return m_Body.Get( x );
		return null;
	}

	override void Refresh()
	{
		if( !m_Closed )
		{
			super.Refresh();
		}
	}

	void CloseButtonOnMouseButtonDown()
	{
		Close();
	}
	
	override float GetFocusedContainerHeight( bool contents = false )
	{
		float x, y;
		if( contents && GetFocusedContainer() )
			y = GetFocusedContainer().GetFocusedContainerHeight( contents );
		else if( GetRootWidget() )
			GetRootWidget().GetScreenSize( x, y );
		return y;
	}
	
	override float GetFocusedContainerYPos( bool contents = false )
	{
		float x, y;
		if( contents && GetFocusedContainer() )
			y = GetFocusedContainer().GetFocusedContainerYPos( contents );
		else if( GetRootWidget() )
			GetRootWidget().GetPos( x, y );
		return y;
	}
	
	override float GetFocusedContainerYScreenPos( bool contents = false )
	{
		float x, y;
		if( contents && GetFocusedContainer() )
			y = GetFocusedContainer().GetFocusedContainerYScreenPos( contents );
		else if( GetRootWidget() )
			GetRootWidget().GetScreenPos( x, y );
		return y;
	}
	
	void MoveContainerUp( Widget cont )
	{
		if( m_Entity )
		{
			InventoryLocation loc = new InventoryLocation;
			m_Entity.GetInventory().GetCurrentInventoryLocation( loc );
			if( loc.IsValid() )
			{
				int slot = loc.GetSlot();
				Inventory.MoveAttachmentUp( slot );
				UpdateSelectionIcons();
			}
		}
	}
	
	void MoveContainerDown( Widget cont )
	{
		if( m_Entity )
		{
			InventoryLocation loc = new InventoryLocation;
			m_Entity.GetInventory().GetCurrentInventoryLocation( loc );
			if( loc.IsValid() )
			{
				int slot = loc.GetSlot();
				Inventory.MoveAttachmentDown( slot );
				UpdateSelectionIcons();
			}
		}
	}
	
	override void CheckHeaderDragability()
	{
		super.CheckHeaderDragability();
		
		if (m_ClosableHeader && m_Entity) //TODO: do the entity check here?
		{
			int flag = m_ClosableHeader.GetMainWidget().GetFlags();
			bool old = flag & WidgetFlags.DRAGGABLE;
			bool current = ItemBase.Cast(m_Entity) && m_Entity.IsTakeable();
			//bool changed = false;
			if (old && !current)
			{
				m_ClosableHeader.GetMainWidget().ClearFlags( WidgetFlags.DRAGGABLE );
				Widget drag = GetDragWidget();
				if (drag && drag == m_ClosableHeader.GetMainWidget())
				{
					CancelWidgetDragging();
					m_ClosableHeader.OnDropHeader(null);
				}
				
				//changed = true;
			}
			else if (!old && current)
			{
				m_ClosableHeader.GetMainWidget().SetFlags( WidgetFlags.DRAGGABLE );
				//changed = true;
			}
			/*if (old != current)
			{
				flag &= ~WidgetFlags.DRAGGABLE;
				m_ClosableHeader.GetMainWidget().SetFlags( flag );
			}*/
		}
	}
}
