class SlotsIcon: SlotsIconBase
{
	override protected void InitIcon(LayoutHolder parent, Widget root, int index, EntityAI slot_parent)
	{
		super.InitIcon(parent,root,index,slot_parent);
	
		WidgetEventHandler.GetInstance().RegisterOnMouseEnter( m_PanelWidget,  this, "MouseEnter" );
		WidgetEventHandler.GetInstance().RegisterOnMouseLeave( m_PanelWidget,  this, "MouseLeave" );
		
		WidgetEventHandler.GetInstance().RegisterOnMouseEnter( m_GhostSlot,  this, "MouseEnterGhostSlot" );
		WidgetEventHandler.GetInstance().RegisterOnMouseLeave( m_GhostSlot,  this, "MouseLeaveGhostSlot" );
		
		WidgetEventHandler.GetInstance().RegisterOnDrag( m_PanelWidget, this, "OnIconDrag" );
		WidgetEventHandler.GetInstance().RegisterOnDrop( m_PanelWidget, this, "OnIconDrop" );
	}
	
	override protected void InitIconWidgets(LayoutHolder parent, Widget root, int index, EntityAI slot_parent)
	{
		super.InitIconWidgets(parent,root,index,slot_parent);
		
		m_PanelWidget			= m_MainWidget.FindAnyWidget( "PanelWidget" + index );
		
		m_CursorWidget			= m_MainWidget.FindAnyWidget( "Cursor" + index );
		m_ColWidget				= m_MainWidget.FindAnyWidget( "Col" + index );
		m_MountedWidget			= m_MainWidget.FindAnyWidget( "Mounted" + index );
		m_OutOfReachWidget		= m_MainWidget.FindAnyWidget( "OutOfReach" + index );

		m_ItemPreview			= ItemPreviewWidget.Cast( m_MainWidget.FindAnyWidget( "Render" + index ) );
		m_ItemPreview.SetForceFlipEnable(true);
		m_ItemPreview.SetForceFlip(false);
		
		m_GhostSlot				= ImageWidget.Cast( m_MainWidget.FindAnyWidget( "GhostSlot" + index ) );
		
		m_ColorWidget			= m_MainWidget.FindAnyWidget( "Color" + index );
		m_SelectedPanel			= m_MainWidget.FindAnyWidget( "Selected" + index );
		m_EmptySelectedPanel    = m_MainWidget.FindAnyWidget( "EmptySelected" + index );
		m_MicromanagedPanel    = m_MainWidget.FindAnyWidget( "Micromanaged" + index );
		
		m_QuantityPanel			= m_MainWidget.FindAnyWidget( "QuantityPanel" + index );
		m_QuantityItem			= TextWidget.Cast( m_MainWidget.FindAnyWidget( "Quantity" + index ) );
		m_QuantityProgress		= ProgressBarWidget.Cast( m_MainWidget.FindAnyWidget( "QuantityBar" + index ) );
		m_QuantityStack			= m_MainWidget.FindAnyWidget( "QuantityStackPanel" + index );
		
		m_ItemSizePanel			= m_MainWidget.FindAnyWidget( "ItemSizePanel" + index );
		m_ItemSizeWidget		= TextWidget.Cast( m_MainWidget.FindAnyWidget( "ItemSize" + index ) );
		
		m_AmmoIcon				= ImageWidget.Cast(m_MainWidget.FindAnyWidget( "AmmoIcon" + index ));
		
		m_RadialIconPanel		= m_MainWidget.FindAnyWidget( "RadialIconPanel" + index );
		m_RadialIconClosed		= m_MainWidget.FindAnyWidget( "RadialIconClosed" + index );
		m_RadialIcon			= m_MainWidget.FindAnyWidget( "RadialIcon" + index );
		
		m_ReservedWidget = Widget.Cast( g_Game.GetWorkspace().CreateWidgets( "gui/layouts/inventory_new/reserved_icon.layout", m_MainWidget ) );
		m_ReservedWidget.Show(false);
	}
	
	override void ClearRemainSelected()
	{
		if (m_IsDragged)
			OnIconDrop(m_PanelWidget);
		
		super.ClearRemainSelected();
	}
	
	override void Clear()
	{
		#ifdef PLATFORM_CONSOLE
		m_SelectedPanel.Show(false);
		#endif
		
		super.Clear();
	}
	
	override protected void HandleConsoleToolbar()
	{
		#ifdef PLATFORM_CONSOLE
		if (IsFocused())
		{
			Inventory.GetInstance().UpdateConsoleToolbar();
		}
		#endif
	}
	
	override void CheckHasItemSize()
	{
		#ifdef PLATFORM_CONSOLE
		string config = "CfgVehicles " + m_Obj.GetType() + " GUIInventoryAttachmentsProps";
		m_HasItemSize = ( InventoryItem.Cast( m_Obj ) && !g_Game.ConfigIsExisting( config ) );
		#else
		m_HasItemSize = false;
		#endif
	}
	
	bool MouseEnter(Widget w, int x, int y)
	{
		if( m_Reserved )
			return MouseEnterGhostSlot(w, x, y);
		
		PrepareOwnedTooltip( m_Item, x, y );
		if( GetDragWidget() != m_PanelWidget && !IsOutOfReach() )
		{
			m_CursorWidget.Show( true );
		}
		return true;
	}
	
	bool MouseLeave( Widget w, Widget s, int x, int y	)
	{
		if( m_Reserved )
			return MouseLeaveGhostSlot(w, s, x, y);
		
		HideOwnedTooltip();
		if( GetDragWidget() != m_PanelWidget )
		{
			m_CursorWidget.Show( false );
		}
		return true;
	}
	
	bool MouseEnterGhostSlot(Widget w, int x, int y)
	{
		float pos_x, pos_y;
		m_MainWidget.GetScreenPos(pos_x, pos_y);

		x = pos_x;
		y = pos_y;
		
		PrepareOwnedSlotsTooltip( m_MainWidget, m_SlotDisplayName, m_SlotDesc, x, y );
		if( GetDragWidget() != m_PanelWidget )
		{
			m_EmptySelectedPanel.Show( true );
		}
		
		return true;
	}

	bool MouseLeaveGhostSlot( Widget w, Widget s, int x, int y	)
	{
		HideOwnedSlotsTooltip();
		
		if( GetDragWidget() != m_PanelWidget )
		{
			m_EmptySelectedPanel.Show( false );
		}
		
		return true;
	}
	
	void OnIconDrag( Widget w )
	{
		if(!m_Obj || !w)
		{
			return;
		}
		ItemManager.GetInstance().HideDropzones();
		if( m_Obj.GetHierarchyRootPlayer() == g_Game.GetPlayer() )
		{
			ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
		}
		else
		{
			ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
		}

		ItemManager.GetInstance().SetIsDragging( true );
		int icon_x, icon_y;
		float icon_size, y;
		int m_sizeX, m_sizeY;

		if( m_Item )
		{
			g_Game.GetInventoryItemSize( m_Item, icon_x, icon_y );
			
			CargoContainer c_parent = CargoContainer.Cast( m_Parent );
			HandsPreview h_parent = HandsPreview.Cast( m_Parent );

			if( GetRoot().m_MainWidget.FindAnyWidget( "HandsPanel" ) )
			{
				GetRoot().m_MainWidget.FindAnyWidget( "HandsPanel" ).GetScreenSize( icon_size, y );
			}
			
			icon_size = icon_size / 10;
			
			w.SetFlags( WidgetFlags.EXACTSIZE );
			m_ItemPreview.SetForceFlipEnable(false);
			
			if( !m_Item.GetInventory().GetFlipCargo() )
			{
				w.SetSize( icon_x * icon_size , icon_y * icon_size );
			}
			else
			{
				w.SetSize( icon_y * icon_size , icon_x * icon_size );
			}
		
			m_ColWidget.Show( true );
			m_CursorWidget.Show( true );
			
			ItemManager.GetInstance().SetDraggedItem( m_Item );
		}
		m_IsDragged = true;
	}
	
	void OnIconDrop( Widget w )
	{
		m_IsDragged = false;
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
		w.ClearFlags( WidgetFlags.EXACTSIZE );
		w.SetSize( 1, 1 );
		m_ColWidget.Show( false );
		m_CursorWidget.Show( false );
		m_EmptySelectedPanel.Show( false );
		m_CursorWidget.SetColor( ARGBF( 1, 1, 1, 1 ) );
		m_ItemPreview.SetForceFlipEnable(true);
	}
}