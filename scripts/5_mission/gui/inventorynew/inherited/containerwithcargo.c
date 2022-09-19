class ContainerWithCargo extends ClosableContainer
{
	protected ref CargoContainer	m_CargoGrid;
	protected int m_CargoIndex = -1;

	void ContainerWithCargo( Container parent, int sort = -1 )
	{
		m_LockCargo = false;
		m_Parent = parent;

		m_CargoGrid = new CargoContainer( this );
		Insert( m_CargoGrid );
		
		m_CargoGrid.GetRootWidget().SetSort( 1 );
		
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver( m_MainWidget,  this, "DraggingOverGrid" );
		RecomputeOpenedContainers();
	}

	override bool IsDisplayable()
	{
		if (m_Entity)
			return m_Entity.CanDisplayCargo();
		return false;
	}
	
	override bool IsEmpty()
	{
		return m_CargoGrid.IsEmpty();
	}
	
	override bool IsItemActive()
	{
		return m_CargoGrid.IsItemActive();
	}
	
	override bool CanEquip()
	{
		return m_CargoGrid.CanEquip();
	}
	
	override bool CanCombine()
	{
		return m_CargoGrid.CanCombine();
	}
	
	override bool CanCombineAmmo()
	{
		return m_CargoGrid.CanCombineAmmo();
	}
	
	override bool IsItemWithQuantityActive()
	{
		return m_CargoGrid.IsItemWithQuantityActive();
	}
	
	void LockCargo(bool value)
	{
		if( value != m_LockCargo )
		{
			if( value )
			{
				m_LockCargo = true;
				OnHide();
			}
			else
			{
				m_LockCargo = false;
				SetOpenState(!m_Closed);
			}
		}	
	}
	
	override void Open()
	{
		if( !m_LockCargo )
		{
			ItemManager.GetInstance().SetDefaultOpenState( m_Entity.GetType(), true );
			m_Closed = false;
			SetOpenForSlotIcon(true);
			OnShow();
			m_Parent.m_Parent.Refresh();
		}
		
		if ( m_SlotIcon )
		{
			m_SlotIcon.GetRadialIconPanel().Show( !m_LockCargo );
		}
	}
	
	override void Close()
	{
		if( !m_LockCargo )
		{
			ItemManager.GetInstance().SetDefaultOpenState( m_Entity.GetType(), false );
			m_Closed = true;
			SetOpenForSlotIcon(false);
			OnHide();
			m_Parent.m_Parent.Refresh();
		}
		
		if ( m_SlotIcon )
		{
			m_SlotIcon.GetRadialIconPanel().Show( !m_LockCargo );
		}
	}
	
	override bool IsOpened()
	{
		return !m_Closed && !m_LockCargo;
	}
	
	override void UpdateInterval()
	{
		if ( m_Entity )
		{
			if ( m_Entity.GetInventory().IsInventoryLockedForLockType( HIDE_INV_FROM_SCRIPT ) || !m_Entity.CanDisplayCargo()  )
			{
				LockCargo(true);
				if ( m_CargoGrid.IsVisible() )
				{
					RecomputeOpenedContainers();
				}
			}
			else
			{
				LockCargo(false);
				if ( !m_CargoGrid.IsVisible() )
				{
					RecomputeOpenedContainers();
				}
			}
		
			super.UpdateInterval();
			m_CargoGrid.UpdateInterval();
			
			bool hide = m_LockCargo || ItemManager.GetInstance().GetDraggedItem() == m_Entity;
			if (!hide)
			{
				SetOpenForSlotIcon(IsOpened());
			}
			
			if ( m_SlotIcon )
			{
				m_SlotIcon.GetRadialIconPanel().Show( !hide );
			}
		}
	}
	
	override bool IsFirstContainerFocused()
	{
		return m_CargoGrid.IsFirstContainerFocused();
	}
	
	override bool IsLastContainerFocused()
	{
		return m_CargoGrid.IsLastContainerFocused();
	}

	override void SetDefaultFocus( bool while_micromanagment_mode = false )
	{
		m_CargoGrid.SetDefaultFocus( while_micromanagment_mode );
	}
	
	override void UnfocusAll()
	{
		m_CargoGrid.Unfocus();
	}
	
	override bool SplitItem()
	{
		return m_CargoGrid.SplitItem();
	}
	
	override bool EquipItem()
	{
		return m_CargoGrid.EquipItem();
	}
	
	override bool TransferItem()
	{
		return m_CargoGrid.TransferItem();
	}
	
	override bool TransferItemToVicinity()
	{
		return m_CargoGrid.TransferItemToVicinity();
	}
	
	override bool InspectItem()
	{
		return m_CargoGrid.InspectItem();
	}

	void SetEntity( EntityAI entity, int cargo_index = 0, bool immedUpdate = true )
	{
		m_Entity = entity;
		m_CargoIndex = cargo_index;

		SetOpenState( true );

		m_CargoGrid.SetEntity( entity, immedUpdate );
		m_CargoGrid.UpdateHeaderText();
		m_ClosableHeader.SetItemPreview( entity );
		CheckHeaderDragability();
		( Container.Cast( m_Parent ) ).Insert( this, -1, false );
		
		if ( m_Entity.GetInventory().IsInventoryLockedForLockType( HIDE_INV_FROM_SCRIPT ) || !m_Entity.CanDisplayCargo() )
			LockCargo(true);
		else
			LockCargo(false);

		if ( immedUpdate )
		{
			Refresh();
			GetMainWidget().Update();
		}
	}

	EntityAI GetEntity()
	{
		return m_Entity;
	}
	
	override EntityAI GetFocusedContainerEntity()
	{
		return m_Entity;
	}

	EntityAI GetItemPreviewItem( Widget w )
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
		if( !ipw || !ipw.IsInherited( ItemPreviewWidget ) )
		{
			return null;
		}
		return ipw.GetItem();
	}

	bool DraggingOverGrid( Widget w, int x, int y, Widget reciever )
	{
		if( w == null )
		{
			return false;
		}
		
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if( !player.CanManipulateInventory() )
		{
			return false;
		}
		
		EntityAI item = GetItemPreviewItem( w );
		
		if( !item )
		{
			return false;
		}
		
		if( !item.GetInventory().CanRemoveEntity() )
		{
			return false;
		} 

		int color, c_x, c_y;
		
		#ifdef PLATFORM_CONSOLE
		int idx = -1;
		#else
		int idx = 0;
		#endif
		
		CargoBase cargo = m_Entity.GetInventory().GetCargoFromIndex(m_CargoIndex);
		if( cargo )
		{
			c_x = cargo.GetHeight();
			c_y = cargo.GetWidth();
		}
		
		InventoryLocation dst = new InventoryLocation;
		#ifdef PLATFORM_CONSOLE
		x = 0;
		y = cargo.GetItemCount();
		m_Entity.GetInventory().FindFreeLocationFor( item, FindInventoryLocationType.CARGO, dst );
		#else
		dst.SetCargoAuto(cargo, item, x, y, item.GetInventory().GetFlipCargo());
		#endif
		
		#ifdef PLATFORM_CONSOLE
		if( dst.IsValid() && m_Entity.GetInventory().LocationCanAddEntityEx(dst))
		#else
		if( m_Entity && c_x > x && c_y > y && m_Entity.GetInventory().LocationCanAddEntityEx(dst))
		#endif
		{
			ItemManager.GetInstance().HideDropzones();
			if( m_Entity.GetHierarchyRootPlayer() == GetGame().GetPlayer() )
			{
				ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
			}
			else
			{
				ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
			}
			color = ColorManager.GREEN_COLOR;
		}
		else
		{
			color = ColorManager.RED_COLOR;
			ItemManager.GetInstance().ShowSourceDropzone( item );
		}

		if( w.FindAnyWidget("Selected") )
		{
			w.FindAnyWidget("Selected").SetColor( color );
		}
		else
		{
			string name = w.GetName();
			name.Replace( "PanelWidget", "Selected" );
			if( w.FindAnyWidget( name ) )
			{
				w.FindAnyWidget( name ).SetColor( color );
			}
		}

		return true;
	}

	void DropReceived( Widget w, int x, int y )
	{
		float xx, yy;
		GetMainWidget().Update();
		GetMainWidget().GetScreenSize( xx, yy );
		if( GetMainWidget().FindAnyWidget("Background") )
		{
			GetMainWidget().FindAnyWidget("Background").Show( true );
			GetMainWidget().FindAnyWidget("Background").SetSize( xx, yy );
		}
		EntityAI item = GetItemPreviewItem( w );
		if( !item )
		{
			return;
		}
		
		#ifdef PLATFORM_CONSOLE
		if( m_CargoGrid.HasItem( item ) )
		{
			return;
		}
		#endif
		
		#ifdef PLATFORM_CONSOLE
		int idx = -1;
		#else
		int idx = 0;
		#endif
		int c_x, c_y;
		
		CargoBase cargo = m_Entity.GetInventory().GetCargoFromIndex(m_CargoIndex);
		
		if( cargo )
		{
			c_x = cargo.GetHeight();
			c_y = cargo.GetWidth();
		}
		
		InventoryLocation dst = new InventoryLocation;
		#ifdef PLATFORM_CONSOLE
		x = 0;
		y = cargo.GetItemCount();
		m_Entity.GetInventory().FindFreeLocationFor( item, FindInventoryLocationType.CARGO, dst );
		#else
		dst.SetCargoAuto(cargo, item, x, y, item.GetInventory().GetFlipCargo());
		#endif
		
		InventoryLocation src = new InventoryLocation;
		item.GetInventory().GetCurrentInventoryLocation(src);

		if(src.CompareLocationOnly(dst) && src.GetFlip() == dst.GetFlip())
			return;
		
		#ifdef PLATFORM_CONSOLE
		if(dst.IsValid() && m_Entity.GetInventory().LocationCanAddEntityEx(dst))
		#else
		if( c_x > x && c_y > y && m_Entity.GetInventory().LocationCanAddEntityEx(dst))
		#endif
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			
			SplitItemUtils.TakeOrSplitToInventoryLocation( player, dst );
			
			Icon icon = m_CargoGrid.GetIcon( item );
			
			if( icon && w && w.FindAnyWidget("Selected") )
			{
				w.FindAnyWidget("Selected").SetColor( ColorManager.BASE_COLOR );
				icon.Refresh();
				Refresh();
			}
		}
		
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
	}
	
	void TakeIntoHands( notnull PlayerBase player, notnull EntityAI item )
	{
		ItemBase item_base = ItemBase.Cast( item );
		
		if( !item.GetInventory().CanRemoveEntity() || !player.CanManipulateInventory() )
			return;
		
		float stackable = item_base.GetTargetQuantityMax();
		
		if( stackable == 0 || stackable >= item_base.GetQuantity() )
		{
			player.PredictiveTakeEntityToHands( item );
		}
		else if( stackable != 0 && stackable < item_base.GetQuantity() )
		{
			item_base.SplitIntoStackMaxHandsClient( player );
		}
	}

	override void DraggingOverHeader( Widget w, int x, int y, Widget receiver )
	{
		DraggingOver( w, x, y, receiver );
	}

	override void DraggingOver( Widget w, int x, int y, Widget receiver )
	{
		if( !w )
			return;
		EntityAI item = GetItemPreviewItem( w );
		if( !item )
			return;
		
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if( item.GetInventory().CanRemoveEntity() && player.CanManipulateInventory() && m_Entity.GetInventory().CanAddEntityInCargo( item, item.GetInventory().GetFlipCargo() ) && !m_Entity.GetInventory().HasEntityInCargo( item ) )
		{
			ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
			ItemManager.GetInstance().HideDropzones();
			if( m_Entity.GetHierarchyRootPlayer() == GetGame().GetPlayer() )
			{
				ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
			}
			else
			{
				ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
			}
		}
		else
		{
			ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
			ItemManager.GetInstance().ShowSourceDropzone( item );
		}
	}

	override void OnDropReceivedFromHeader( Widget w, int x, int y, Widget receiver )
	{
		EntityAI item = GetItemPreviewItem( w );
		if( !item )
		{
			return;
		}

		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		InventoryLocation src = new InventoryLocation();
		if( GetEntity() && item.GetInventory().GetCurrentInventoryLocation(src))
		{
			InventoryLocation dst = new InventoryLocation();
			if (m_Entity.GetInventory().FindFreeLocationFor(item, FindInventoryLocationType.ATTACHMENT, dst))
			{
				player.PredictiveTakeToDst(src, dst);
			}

			bool can_add = m_Entity.GetInventory().CanAddEntityInCargo( item, item.GetInventory().GetFlipCargo() );
			bool in_cargo = !player.GetInventory().HasEntityInInventory( item ) || !m_Entity.GetInventory().HasEntityInCargo( item );
			if( can_add && in_cargo )
			{
				SplitItemUtils.TakeOrSplitToInventory( player, m_Entity, item );
			}
		}
	}
}
