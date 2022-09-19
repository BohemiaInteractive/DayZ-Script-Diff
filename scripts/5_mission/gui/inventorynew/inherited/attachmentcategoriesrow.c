class AttachmentCategoriesRow: ClosableContainer
{
	protected string								m_CategoryIdentifier;
	protected string								m_RowConfigPath;
	protected bool									m_Refreshing;
	
	protected ref AttachmentsGroupContainer			m_AttachmentsContainer;
	protected ref map<int, ref ContainerWithCargo>	m_AttachmentCargos;
	
	void AttachmentCategoriesRow( LayoutHolder parent, int sort = -1 )
	{
		m_Refreshing = false;
		m_AttachmentCargos		= new map<int, ref ContainerWithCargo>;
		m_AttachmentsContainer	= new AttachmentsGroupContainer(this);
		
		m_Body.Insert(m_AttachmentsContainer);
		
		m_ClosableHeader.GetMainWidget().ClearFlags( WidgetFlags.DRAGGABLE );
		
		RecomputeOpenedContainers();
	}
	
	string GetCategoryIdentifier()
	{
		return m_CategoryIdentifier;
	}
	
	int GetFocusedSlot()
	{
		SlotsIcon icon = GetFocusedSlotsIcon();
		if (icon)
		{
			return icon.GetSlotID();
		}
		return -1;
	}
	
	override bool Select()
	{
		Man player = GetGame().GetPlayer();
		SlotsIcon focused_icon = GetFocusedSlotsIcon();
		EntityAI focused_item = GetFocusedItem();
		Container focused_cont = GetFocusedContainer();
		ItemBase selected_item = ItemBase.Cast( ItemManager.GetInstance().GetSelectedItem() ); // == dragged item 
		
		if( selected_item && focused_item != selected_item)//dropping from micromanagement
		{
			if( selected_item.GetInventory().CanRemoveEntity() )
			{
				int stack_max;
				
				if (!focused_icon)
				{
					if (focused_cont)
					{
						focused_cont.Select();
					}
				}
				else if( !focused_item && m_Entity && m_Entity.GetInventory().CanAddAttachmentEx( selected_item, focused_icon.GetSlotID() ) )
				{
					stack_max = InventorySlots.GetStackMaxForSlotId( focused_icon.GetSlotID() );
					float quantity = selected_item.GetQuantity();
					if( stack_max == 0 || stack_max >= quantity || !selected_item.CanBeSplit() )
					{
						player.PredictiveTakeEntityToTargetAttachmentEx( m_Entity, selected_item, focused_icon.GetSlotID() );
						return true;
					}
					else
					{
						selected_item.SplitIntoStackMaxClient( m_Entity, focused_icon.GetSlotID() );
						return true;
					}
				}
				else if( focused_icon.GetSlotID() != -1 )
				{
					if( focused_item && focused_item.GetInventory().CanRemoveEntity() )
					{
						InventoryLocation inv_loc = new InventoryLocation;
						focused_item.GetInventory().GetCurrentInventoryLocation( inv_loc );
						stack_max = InventorySlots.GetStackMaxForSlotId( inv_loc.GetSlot() );
						quantity = focused_item.GetQuantity();
						if( focused_item.CanBeCombined( ItemBase.Cast( selected_item ) ) )
						{
							focused_item.CombineItemsClient( selected_item, true );
							return true;
						}
						else if( stack_max == 0 && GameInventory.CanSwapEntitiesEx( focused_item, selected_item ) )
						{
							player.PredictiveSwapEntities( selected_item, focused_item );
							return true;
						}
						else if( m_AttachmentCargos.Contains( inv_loc.GetSlot() ) )
						{
							if( focused_item.GetInventory().CanAddEntityInCargo( selected_item, selected_item.GetInventory().GetFlipCargo() ) )
							{
								SplitItemUtils.TakeOrSplitToInventory( PlayerBase.Cast( player ), focused_item, selected_item );
								return true;
							}
						}
					}
				}
			}
		}
		else //clicking
		{
			if( focused_item && focused_item.GetInventory().CanRemoveEntity() && (!focused_icon || !focused_icon.IsOutOfReach()) )
			{
				EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
				if( item_in_hands && item_in_hands.GetInventory().CanRemoveEntity() )
				{
					if( GameInventory.CanSwapEntitiesEx( item_in_hands, focused_item ) )
					{
						player.PredictiveSwapEntities( item_in_hands, focused_item );
						return true;
					}
				}
				else
				{
					if( player.GetHumanInventory().CanAddEntityInHands( focused_item ) )
					{
						player.PredictiveTakeEntityToHands( focused_item );
						return true;
					}
				}
			}
		}
		
		return false;
	}
	
	override bool TransferItem()
	{
		EntityAI entity = GetFocusedItem();
		SlotsIcon focused_icon = GetFocusedSlotsIcon();
		if( entity && !entity.IsLockedInSlot() && (!focused_icon || !focused_icon.IsOutOfReach()) )
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			GetGame().GetPlayer().PredictiveTakeEntityToInventory( FindInventoryLocationType.CARGO, entity );
			return true;
		}
		return false;
	}
	
	override bool TransferItemToVicinity()
	{
		ItemBase item = ItemBase.Cast(GetFocusedItem());
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		SlotsIcon focused_icon = GetFocusedSlotsIcon();
		
		if( item && !item.IsLockedInSlot() && (focused_icon && !focused_icon.IsOutOfReach()) )
		{
			if( item.GetTargetQuantityMax() < item.GetQuantity() )
				item.SplitIntoStackMaxClient( null, -1 );
			else
				player.PhysicalPredictiveDropItem( item );
			return true;
		}
		return false;
	}
	
	override bool Combine()
	{
		Man player = GetGame().GetPlayer();
		ItemBase prev_item = ItemBase.Cast( GetFocusedItem() );
		ItemBase selected_item = ItemBase.Cast( player.GetHumanInventory().GetEntityInHands() );
		SlotsIcon focused_icon = GetFocusedSlotsIcon();
		
		if( selected_item )
		{
			int selected_slot = GetFocusedSlot();
			int stack_max;
			if( selected_item && selected_item.GetInventory().CanRemoveEntity() && (focused_icon && !focused_icon.IsOutOfReach()) )
			{
				if( !prev_item && m_Entity.GetInventory().CanAddAttachmentEx( selected_item, selected_slot ) )
				{
					stack_max = InventorySlots.GetStackMaxForSlotId( selected_slot );
					float quantity = selected_item.GetQuantity();
					if( stack_max == 0 || stack_max >= quantity || !selected_item.CanBeSplit() )
					{
						player.PredictiveTakeEntityToTargetAttachmentEx( m_Entity, selected_item, selected_slot );
						return false;
					}
					else
					{
						selected_item.SplitIntoStackMaxClient( m_Entity, selected_slot );
						return false;
					}
				}
				else if( selected_slot != -1 )
				{
					if( prev_item )
					{
						InventoryLocation inv_loc = new InventoryLocation;
						prev_item.GetInventory().GetCurrentInventoryLocation( inv_loc );
						stack_max = InventorySlots.GetStackMaxForSlotId( inv_loc.GetSlot() );
						quantity = prev_item.GetQuantity();
						if( prev_item.CanBeCombined( ItemBase.Cast( selected_item ) ) )
						{
							prev_item.CombineItemsClient( selected_item, true );
							return false;
						}
						else if( stack_max == 0 && GameInventory.CanSwapEntitiesEx( prev_item, selected_item ) )
						{
							player.PredictiveSwapEntities( selected_item, prev_item );
							return false;
						}
						else if( m_AttachmentCargos.Contains( inv_loc.GetSlot() ) )
						{
							if( prev_item.GetInventory().CanAddEntityInCargo( selected_item, selected_item.GetInventory().GetFlipCargo() ) )
							{
								SplitItemUtils.TakeOrSplitToInventory( PlayerBase.Cast( player ), prev_item, selected_item );
								return false;
							}
						}
					}
				}
			}
		}
		return true;
	}
	
	override bool SplitItem()
	{
		return false;
	}
	
	override bool EquipItem() //TODO: may be possible, but the hint doesn't reflect it anyway
	{
		return false;
	}
	
	override bool CanEquip()
	{
		return false;
	}
	
	override bool CanCombine()
	{
		Man player = GetGame().GetPlayer();
		ItemBase prev_item = ItemBase.Cast( GetFocusedItem() );
		ItemBase selected_item = ItemBase.Cast( player.GetHumanInventory().GetEntityInHands() );
		SlotsIcon focused_icon = GetFocusedSlotsIcon();
		
		if( selected_item )
		{
			int selected_slot = GetFocusedSlot();
			int stack_max;
			if( selected_item && selected_item.GetInventory().CanRemoveEntity() && (focused_icon && !focused_icon.IsOutOfReach()) )
			{
				if( m_Entity.GetInventory().CanAddAttachmentEx( selected_item, selected_slot ) )
				{
					stack_max = InventorySlots.GetStackMaxForSlotId( selected_slot );
					float quantity = selected_item.GetQuantity();
					if( stack_max == 0 || stack_max >= quantity || !selected_item.CanBeSplit() )
					{
						return true;
					}
					else
					{
						return true;
					}
				}
				else if( selected_slot != -1 )
				{
					if( prev_item )
					{
						InventoryLocation inv_loc = new InventoryLocation;
						prev_item.GetInventory().GetCurrentInventoryLocation( inv_loc );
						stack_max = InventorySlots.GetStackMaxForSlotId( inv_loc.GetSlot() );
						quantity = prev_item.GetQuantity();
						if( prev_item.CanBeCombined( ItemBase.Cast( selected_item ) ) )
						{
							return true;
						}
					}
				}
			}
		}
		return false;
	}
	
	override bool CanCombineAmmo()
	{
		return false;
	}
	
	override bool IsItemActive()
	{
		SlotsIcon icon = GetFocusedSlotsIcon();
		return GetFocusedItem() != null && (!icon || !icon.IsOutOfReach());
	}

	override bool IsItemWithQuantityActive()
	{
		ItemBase item = ItemBase.Cast(GetFocusedItem());
		SlotsIcon icon = GetFocusedSlotsIcon();
		return item && ( !icon || !icon.IsOutOfReach()) && item.CanBeSplit();
	}
	
	override void OnDropReceivedFromHeader( Widget w, int x, int y, Widget receiver )
	{
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
		SlotsIcon slots_icon;
		EntityAI receiver_item;
		int slot_id = -1;
		bool is_reserved = false;
		EntityAI attached_entity;
		receiver.GetUserData(slots_icon);
		float stackable = 0.0;
		InventoryLocation il = new InventoryLocation;
		
		if( slots_icon )
		{
			receiver_item	= slots_icon.GetEntity();
			slot_id			= slots_icon.GetSlotID();
			attached_entity = slots_icon.GetSlotParent();
			is_reserved		= slots_icon.IsReserved();
		}
		
		EntityAI item = GetItemPreviewItem( w );
		if( !item )
		{
			return;
		}
		ItemBase item_base	= ItemBase.Cast( item );
		
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if( !item.GetInventory().CanRemoveEntity() || !player.CanManipulateInventory() )
			return;
		
		EntityAI target_att_entity = m_Entity;
		Weapon_Base wpn;
		Magazine mag;
		if( Class.CastTo(wpn,  m_Entity ) && Class.CastTo(mag,  item ) )
		{
			if( player.GetWeaponManager().CanAttachMagazine( wpn, mag ) )
			{
				player.GetWeaponManager().AttachMagazine( mag );
			}
		}
		else if( receiver_item && !is_reserved )
		{
			if( ( ItemBase.Cast( receiver_item ) ).CanBeCombined( ItemBase.Cast( item ) ) )
			{
				( ItemBase.Cast( receiver_item ) ).CombineItemsClient( ItemBase.Cast( item ) );
			}
			else if( GameInventory.CanSwapEntitiesEx( receiver_item, item ) )
			{
				if( !receiver_item.GetInventory().CanRemoveEntity() )
					return;
				GetGame().GetPlayer().PredictiveSwapEntities( receiver_item, item );
			}
			else if( receiver_item.GetInventory().CanAddAttachment( item ) )
			{
				player.PredictiveTakeEntityToTargetAttachment(receiver_item, item);
			}
		}
		else if( attached_entity && slot_id != -1 &&  attached_entity.GetInventory().CanAddAttachmentEx( item, slot_id ) )
		{
			item_base	= ItemBase.Cast( item );
			stackable	= item_base.GetTargetQuantityMax( slot_id );
			
			if( stackable == 0 || stackable >= item_base.GetQuantity() )
			{
				player.PredictiveTakeEntityToTargetAttachmentEx(attached_entity, item, slot_id);
			}
			else if( stackable != 0 && stackable < item_base.GetQuantity() )
			{
				item_base.SplitIntoStackMaxClient( m_Entity, slot_id );
			}
		}
		else if(attached_entity && slot_id == -1 && attached_entity.GetInventory().FindFreeLocationFor(item,FindInventoryLocationType.ATTACHMENT,il))
		{
			item_base	= ItemBase.Cast( item );
			stackable	= item_base.GetTargetQuantityMax( il.GetSlot() );
			
			if( stackable == 0 || stackable >= item_base.GetQuantity() )
			{
				player.PredictiveTakeEntityToTargetAttachmentEx(attached_entity, item, il.GetSlot());
			}
			else if( stackable != 0 && stackable < item_base.GetQuantity() )
			{
				item_base.SplitIntoStackMaxClient( m_Entity, il.GetSlot() );
			}
		}
		/*else if( ( m_Entity.GetInventory().CanAddEntityInCargo( item, item.GetInventory().GetFlipCargo() ) && ( !player.GetInventory().HasEntityInInventory( item ) || !m_Entity.GetInventory().HasEntityInCargo( item )) ) )
		{
			SplitItemUtils.TakeOrSplitToInventory( PlayerBase.Cast( GetGame().GetPlayer() ), m_Entity, item );
		}*/
	}

	override void UnfocusAll()
	{
		int i;
		for ( i = 1; i < Count(); i++ )
		{
			for ( int j = 0; j < ITEMS_IN_ROW; j++ )
			{
				Widget w = Get( i ).GetMainWidget().FindAnyWidget( "Cursor" + j );
				if( w )
					w.Show( false );
			}
		}
		for ( i = 0; i < m_AttachmentCargos.Count(); i++ )
		{
			m_AttachmentCargos.GetElement( i ).Unfocus();
			m_AttachmentCargos.GetElement( i ).SetActive( false );
		}
	}

	override void DraggingOverHeader( Widget w, int x, int y, Widget receiver )
	{
		if( w == null )
		{
			return;
		}
		ItemPreviewWidget iw = ItemPreviewWidget.Cast( w.FindAnyWidget("Render") );
		if(!iw)
		{
		  string name = w.GetName();
		  name.Replace("PanelWidget", "Render");
		  iw = ItemPreviewWidget.Cast( w.FindAnyWidget(name) );
		}
		if(!iw)
		{
		  iw = ItemPreviewWidget.Cast( w );
		}
		
		if( !iw || !iw.GetItem() || (iw.GetItem() == m_Entity) )
		{
			return;
		}
		
		ItemBase item;
		ItemBase receiver_item;
		name = receiver.GetName();
		name.Replace("PanelWidget", "Render");
		ItemPreviewWidget receiver_iw = ItemPreviewWidget.Cast( receiver.FindAnyWidget(name) );
		if(receiver_iw)
			receiver_item = ItemBase.Cast( receiver_iw.GetItem() );
		
		SlotsIcon slots_icon;
		receiver.GetUserData(slots_icon);
		
		if( receiver_item )
		{
			int stack_max = InventorySlots.GetStackMaxForSlotId( receiver_item.GetInventory().GetSlotId(0) );
			//int quantity = receiver_item.GetQuantity();
			//bool combinable = ( quantity < stack_max ) && ( ItemBase.Cast( receiver_item ).CanBeCombined( ItemBase.Cast( iw.GetItem() ) ) );
			if( receiver_item.CanBeCombined( ItemBase.Cast( iw.GetItem() ) ) )
			{
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
				ColorManager.GetInstance().SetColor( w, ColorManager.COMBINE_COLOR );
			}
			else if( stack_max == 0 && GameInventory.CanSwapEntitiesEx( receiver_item, iw.GetItem() ) )
			{
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
				ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
			}
			else
			{
				ItemManager.GetInstance().ShowSourceDropzone( iw.GetItem() );
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
			}
		}
		else if( slots_icon && slots_icon.GetSlotID() != -1 )
		{
			item = ItemBase.Cast( iw.GetItem() );
			
			if( item && m_Entity.GetInventory().CanAddAttachmentEx( item, slots_icon.GetSlotID() ) )
			{
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
				ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
			}
			else
			{
				ItemManager.GetInstance().ShowSourceDropzone( iw.GetItem() );
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
			}
		}
		else
		{
			ItemManager.GetInstance().ShowSourceDropzone( iw.GetItem() );
			ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
		}
	}
	
	void DoubleClick(Widget w, int x, int y, int button)
	{
		if( button == MouseState.LEFT )
		{
			if( w == null )
			{
				return;
			}
			ItemPreviewWidget iw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
			if( !iw )
			{
			  string name = w.GetName();
			  name.Replace( "PanelWidget", "Render" );
			  iw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
			}
			if( !iw )
			{
			  iw = ItemPreviewWidget.Cast( w );
			}
			if( !iw.GetItem() )
			{
				return;
			}
			
			SlotsIcon slots_icon;
			w.GetUserData(slots_icon);
			
			if( slots_icon && m_Entity.GetInventory().GetSlotLock( iw.GetUserID() ) )
			{
				return;
			}
			
			ItemBase item = ItemBase.Cast( iw.GetItem() );
			
			if( !item.GetInventory().CanRemoveEntity() )
				return;
			
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			
			if( player.GetInventory().HasEntityInInventory( item ) && player.GetHumanInventory().CanAddEntityInHands( item ) )
			{
				player.PredictiveTakeEntityToHands( item );
			}
			else
			{
				if(player.GetInventory().CanAddEntityToInventory( item ) && item.GetInventory().CanRemoveEntity())
				{
					if( item.GetTargetQuantityMax() < item.GetQuantity() )
					{
						item.SplitIntoStackMaxClient( player, -1 );
					}
					else
					{
						player.PredictiveTakeEntityToInventory( FindInventoryLocationType.ANY, item );
					}
				}
				else
				{
					if( player.GetHumanInventory().CanAddEntityInHands( item ) )
					{
						if( item.GetTargetQuantityMax() < item.GetQuantity() )
						{
							item.SplitIntoStackMaxHandsClient( player );
						}
						else
						{
							player.PredictiveTakeEntityToHands( item );
						}
					}
				}
			}
			
			HideOwnedTooltip();
			
			name = w.GetName();
			name.Replace( "PanelWidget", "Temperature" );
			w.FindAnyWidget( name ).Show( false );
		}
	}
	
	bool DraggingOverGrid( Widget w,  int x, int y, Widget reciever, CargoContainer cargo )
	{
		if( w == null )
		{
			return false;
		}
		
		EntityAI item = GetItemPreviewItem( w );
		
		if( !item )
		{
			return false;
		}

		int color;
		int idx = 0;
		int c_x, c_y;
		
		EntityAI target_entity;
		CargoBase target_cargo;
		
		target_entity		=  cargo.GetEntity() ;
		if( target_entity )
		{
			target_cargo 	= target_entity.GetInventory().GetCargo();
		}
		else
			return false;
		
		if( target_cargo && target_entity )
		{
			c_x = target_cargo.GetHeight();
			c_y = target_cargo.GetWidth();
		}
		else
			return false;
		
		if( c_x > x && c_y > y && target_entity.GetInventory().CanAddEntityInCargoEx( item, idx, x, y, item.GetInventory().GetFlipCargo() ) )
		{
			color = ColorManager.GREEN_COLOR;
		}
		else
		{
			color = ColorManager.RED_COLOR;
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

	void DropReceived( Widget w, int x, int y, CargoContainer cargo )
	{
		EntityAI item = GetItemPreviewItem( w );
		if( !item )
		{
			return;
		}

		int idx = 0;
		int c_x, c_y;
		
		EntityAI target_entity;
		CargoBase target_cargo;
		
		target_entity	= cargo.GetEntity();
		if( target_entity )
		{
			target_cargo 	= target_entity.GetInventory().GetCargo();
		}
		else
			return;
		
		if( target_cargo && target_entity )
		{
			c_x = target_cargo.GetHeight();
			c_y = target_cargo.GetWidth();
		}
		else
			return;
		
		InventoryLocation dst = new InventoryLocation;		
		dst.SetCargoAuto(target_cargo, item, x, y, item.GetInventory().GetFlipCargo());
			
		if( c_x > x && c_y > y && target_entity.GetInventory().LocationCanAddEntity(dst))
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );

			SplitItemUtils.TakeOrSplitToInventoryLocation( player, dst);

			Icon icon = cargo.GetIcon( item );
			
			if( icon )
			{
				icon.GetRootWidget().FindAnyWidget("Selected").SetColor( ColorManager.BASE_COLOR );
				icon.RefreshPos( x, y );
				icon.Refresh();
				Refresh();
			}
		}
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
	
	override void Refresh()
	{
		if (!m_Refreshing)
		{
			if( !m_Closed )
			{
				super.Refresh();
				RefreshAllSlots();
			}
		}
	}
	
	void RefreshAllSlots()
	{
		m_Refreshing = true;
		int row_count = m_AttachmentsContainer.GetRowCount();
		int row;
		for (row = 0; row < row_count; row++)
		{
			int col_count = m_AttachmentsContainer.GetColumnCountForRow(row);
			int col;
			for (col = 0; col < col_count; col++)
			{
				RefreshSlot(row, col, -1, "");
			}
		}
		m_Refreshing = false;
	}
	
	//slot_id and slot_name parametr is not used
	void RefreshSlot( int row, int column, int slot_id, string slot_name )
	{
		SlotsIcon icon = m_AttachmentsContainer.GetSlotsIcon(row, column);
		int slot_id_ = icon.GetSlotID();
		EntityAI item = m_Entity.GetInventory().FindAttachment( slot_id_ );
		ContainerWithCargo cont;
		
		if( !m_Entity.CanDisplayAttachmentSlot( slot_id_ ) )
		{
			icon.Clear();
			icon.GetMainWidget().Show( false );
		}
		else if( !item )
		{
			icon.Clear();
			icon.GetMainWidget().Show( true );
			cont = m_AttachmentCargos.Get(slot_id_);
			if( cont )
			{
				m_AttachmentCargos.Remove(slot_id_);
				this.Remove(cont);
				( Container.Cast( cont.m_Parent ) ).Remove( cont );
			}
		}
		else
		{
			icon.GetMainWidget().Show( true );
			if (icon.GetEntity() != item)
			{
				icon.Init( item );
			}
			else
			{
				icon.Refresh();
			}
			//icon.Refresh();
			
			cont = m_AttachmentCargos.Get(slot_id_);
			if( cont && cont.GetEntity() != item)
			{
				m_AttachmentCargos.Remove(slot_id_);
				( Container.Cast( cont.m_Parent ) ).Remove( cont );
				this.Remove(cont);
				cont = null;
			}
			
			if( !cont )
			{
				if( item.GetInventory().GetCargo() && m_AttachmentCargos )
				{
					cont = new ContainerWithCargo( this, true );
					cont.GetRootWidget().SetSort( m_AttachmentCargos.Count() + 10 );
					cont.SetEntity( item );
					cont.SetSlotIcon( icon );
					
					m_AttachmentCargos.Insert( slot_id_, cont );
					icon.SetContainer(cont);
					
					//icon.GetRadialIconPanel().Show( true );
					//icon.GetRadialIconPanel().Show( cont.IsDisplayable() );
					
					SetOpenForSlotIcon(cont.IsOpened(),icon);
				}
			}
			
			if (cont)
			{
				icon.GetRadialIconPanel().Show( cont.IsDisplayable() );
			}
				
			string slot_name_ 			= InventorySlots.GetSlotName(slot_id_);
			bool draggable				= true;
			bool in_hands_condition		= m_Entity.GetHierarchyRoot() == GetGame().GetPlayer() && item.GetInventory().CanRemoveEntity();
			bool in_vicinity_condition	= AttachmentsOutOfReach.IsAttachmentReachable( m_Entity, slot_name_ );
			if( m_Entity.GetInventory().GetSlotLock( slot_id_ ) && ItemManager.GetInstance().GetDraggedItem() != item )
			{
				icon.GetMountedWidget().Show( true );
				draggable = false;
			}
			else
			{
				icon.GetMountedWidget().Show( false );
			}
			
			if( !m_Entity.CanReleaseAttachment( item ) )
			{
				draggable = false;
			}
			
			if( in_hands_condition || in_vicinity_condition )
			{
				icon.GetOutOfReachWidget().Show( false );
			}
			else
			{
				icon.GetOutOfReachWidget().Show( true );
				draggable = false;
			}
			
			if( draggable )
			{
				icon.GetRender().GetParent().SetFlags( WidgetFlags.DRAGGABLE );
			}
			else
			{
				icon.GetRender().GetParent().ClearFlags( WidgetFlags.DRAGGABLE );
			}
		}
	}
	
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		super.OnMouseButtonDown(w, x, y, button);
		
		SlotsIcon slots_icon;
		w.GetUserData(slots_icon);
		
		Container c = Container.Cast( slots_icon.GetContainer() );
		if( c )
		{
			if( c.IsOpened() )
			{
				c.Close();
			}
			else
			{
				c.Open();
			}
		}
		
		#ifdef DEVELOPER
		
		if( GetDayZGame().IsLeftCtrlDown() && button == 1 )
		{
			SlotsIcon user_data;
			w.GetUserData(user_data);
			if(user_data)
			{
				ShowActionMenu( user_data.GetItem() );
				return true;
			}
		}
		#endif
		return false;
	}
	
	void Init(int attachments_categories_count, int i, string attachment_category, string config_path_attachment_categories, EntityAI entity, int parent_m_Body_count )
	{
		m_Entity = entity;
		Header he = GetHeader();
		
		m_CategoryIdentifier = attachment_category;
		
		ref array<string> player_ghosts_slots2 = new array<string>;
		string category_name;
		
		m_RowConfigPath = config_path_attachment_categories + " " + attachment_category + " attachmentSlots";
		string config_path_category_name = config_path_attachment_categories + " " + attachment_category + " name";
		GetGame().ConfigGetTextArray( m_RowConfigPath, player_ghosts_slots2 );
		GetGame().ConfigGetText( config_path_category_name, category_name );
		he.SetName( category_name );
		
		m_AttachmentsContainer.SetHeader(he);
		SetHeader(null);
		
		int count = player_ghosts_slots2.Count();
		int last_row = count / ITEMS_IN_ROW;
		SlotsContainer ic;
		
		for ( int j = 0; j < count; j++ )
		{
			string slot_name = player_ghosts_slots2.Get(j);
			string icon_name; //must be in format "set:<setname> image:<imagename>"
			string path = "CfgSlots" + " Slot_" + slot_name;
			
			int slot_id	= InventorySlots.GetSlotIdFromString( slot_name );
				
			GetGame().ConfigGetText( path + " ghostIcon", icon_name );
			GetGame().ConfigGetText( path + " name", slot_name );
				
			int row = j / ITEMS_IN_ROW;
			int column = j % ITEMS_IN_ROW;
			if (column == 0)
			{
				ic = new SlotsContainer(m_AttachmentsContainer, m_Entity);
				ic.GetRootWidget().SetAlpha(0.7);
				if (row == last_row)
				{
					ic.SetColumnCount( count % ITEMS_IN_ROW );
				}
				else
				{
					ic.SetColumnCount( ITEMS_IN_ROW );
				}
				m_AttachmentsContainer.Insert(ic);
				ic.GetRootWidget().SetSort( row );
			} 
				
			SlotsIcon icon = ic.GetSlotIcon( column );
			WidgetEventHandler.GetInstance().RegisterOnDropReceived( icon.GetMainWidget(),  this, "OnDropReceivedFromHeader" );
			WidgetEventHandler.GetInstance().RegisterOnDropReceived( icon.GetGhostSlot(),  this, "OnDropReceivedFromHeader" );
			WidgetEventHandler.GetInstance().RegisterOnDropReceived( icon.GetPanelWidget(),  this, "OnDropReceivedFromHeader" );
					
			WidgetEventHandler.GetInstance().RegisterOnDraggingOver( icon.GetMainWidget(),  this, "DraggingOverHeader" );
			WidgetEventHandler.GetInstance().RegisterOnDraggingOver( icon.GetGhostSlot(),  this, "DraggingOverHeader" );
			WidgetEventHandler.GetInstance().RegisterOnDraggingOver( icon.GetPanelWidget(),  this, "DraggingOverHeader" );
					
			WidgetEventHandler.GetInstance().RegisterOnDrop( icon.GetMainWidget(),  this, "OnIconDrop" );
			WidgetEventHandler.GetInstance().RegisterOnDrop( icon.GetPanelWidget(),  this, "OnIconDrop" );
					
			WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown( icon.GetPanelWidget(),  this, "OnMouseButtonDown" );
			
			icon.GetGhostSlot().LoadImageFile( 0,StaticGUIUtils.VerifyIconImageString(StaticGUIUtils.IMAGESETGROUP_INVENTORY,icon_name) );
			
			icon.SetSlotID(slot_id);
			icon.SetSlotDisplayName(InventorySlots.GetSlotDisplayName(slot_id));
			
			//RefreshSlot( row, column, slot_id, slot_name );
		}
		RefreshAllSlots();
	}
	
	override void UpdateInterval()
	{
		super.UpdateInterval();
		RefreshAllSlots();
	}
}
