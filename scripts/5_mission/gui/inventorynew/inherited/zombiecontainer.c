class ZombieContainer: CollapsibleContainer
{
	protected ref AttachmentsGroupContainer		m_Container;
	protected ref ContainerWithCargo			m_CargoGrid;
	protected ref map<int, SlotsIcon>			m_InventorySlots;
	protected ref map<EntityAI, ref Container>	m_ShowedItems = new ref map<EntityAI, ref Container>;
	protected EntityAI							m_ZombieEntity;
	
	void ZombieContainer( LayoutHolder parent, int sort = -1 )
	{
		m_InventorySlots = new ref map<int, SlotsIcon>;
		m_Container = new AttachmentsGroupContainer(this);
		
		m_Container.SetHeader(GetHeader());
		SetHeaderName();
		SetHeader(null);
		m_Body.Insert( m_Container );
		
		m_MainWidget = m_RootWidget.FindAnyWidget( "body" );
		
		WidgetEventHandler.GetInstance().RegisterOnChildAdd( m_MainWidget, this, "OnChildAdd" );
		WidgetEventHandler.GetInstance().RegisterOnChildRemove( m_MainWidget, this, "OnChildRemove" );
		
		
		RecomputeOpenedContainers();
	}
	
	void ~ZombieContainer()
	{
		if( m_ZombieEntity )
		{
			m_ZombieEntity.GetOnItemAttached().Remove(ItemAttached);
			m_ZombieEntity.GetOnItemDetached().Remove(ItemDetached);
		}
	}
	
	void SetEntity( EntityAI zombie_entity )
	{
		m_ZombieEntity = zombie_entity;
		m_ZombieEntity.GetOnItemAttached().Insert(ItemAttached);
		m_ZombieEntity.GetOnItemDetached().Insert(ItemDetached);
		InitGhostSlots();
		m_Parent.Refresh();
	}
	
	override void UpdateRadialIcon()
	{
		if ( m_SlotIcon )
		{
			bool show_radial_icon;
			show_radial_icon = IsHidden();
			Widget rip = m_SlotIcon.GetRadialIconPanel();
			Widget icon_open = m_SlotIcon.GetRadialIcon();
			Widget icon_closed = m_SlotIcon.GetRadialIconClosed();
			rip.Show( !m_ZombieEntity.GetInventory().IsInventoryLockedForLockType( HIDE_INV_FROM_SCRIPT ) && m_ZombieEntity.CanDisplayCargo());
			icon_open.Show( show_radial_icon );
			icon_closed.Show( !show_radial_icon );
		}
	}
	
	override bool IsItemWithQuantityActive()
	{
		EntityAI ent = GetFocusedItem();
		return ent && QuantityConversions.HasItemQuantity( ent ) && ent.CanBeSplit();
	}
	
	bool IsContainerWithCargoActive()
	{
		return GetFocusedContainer().IsInherited( ContainerWithCargo );
	}
	
	bool IsItemWithAttachmentsActive()
	{
		return GetFocusedContainer().IsInherited( ContainerWithCargoAndAttachments );
	}
	
	bool IsZombieEquipmentActive()
	{
		return GetFocusedContainer() == m_Container;
	}
	
	bool IsItemWithContainerActive()
	{
		EntityAI ent = GetFocusedItem();
		return ent && ( ent.GetInventory().GetCargo() || ent.GetSlotsCountCorrect() > 0 );
	}
	
	bool IsEmptyItemActive()
	{
		return !GetFocusedItem();
	}
	
	override void ExpandCollapseContainer()
	{
		if( IsZombieEquipmentActive() )
		{
			ItemPreviewWidget item_preview = GetFocusedSlotsIcon().GetRender();
			ToggleContainer( item_preview.GetParent() );
		}
	}
	
	override void OnDropReceivedFromHeader( Widget w, int x, int y, Widget receiver )
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
		
		if( !ipw.IsInherited( ItemPreviewWidget ) )
		{
			return;	
		}
		
		EntityAI item = ipw.GetItem();
		
		if( !item.GetInventory().CanRemoveEntity() )
			return;
		
		if (m_ZombieEntity.GetInventory().CanAddEntityInCargo( item, item.GetInventory().GetFlipCargo() ))
			GetGame().GetPlayer().PredictiveTakeEntityToTargetCargo( m_ZombieEntity, item );
		else if( m_ZombieEntity.GetInventory().CanAddEntityToInventory( item ) )
		{
			GetGame().GetPlayer().PredictiveTakeEntityToTargetInventory( m_ZombieEntity, FindInventoryLocationType.CARGO, item );
		}
	}
	
	override void DraggingOverHeader(Widget w, int x, int y, Widget receiver)
	{
		ItemPreviewWidget ipw = ItemPreviewWidget.Cast(w.FindAnyWidget( "Render" ));
		string name = receiver.GetName();
		name.Replace("PanelWidget", "Render");
		ItemPreviewWidget receiver_iw = ItemPreviewWidget.Cast( receiver.FindAnyWidget( name ) );
		EntityAI receiver_item;
		if (receiver_iw)
		{
			receiver_item = receiver_iw.GetItem();
		}
		
		if(!ipw)
		{
			name = w.GetName();
			name.Replace("PanelWidget", "Render");
			ipw = ItemPreviewWidget.Cast(w.FindAnyWidget(name));
		}
		if(!ipw)
		{
		  ipw = ItemPreviewWidget.Cast(w);
		}
		
		if( !ipw.IsInherited(ItemPreviewWidget))
		{
			return;	
		}
		
		EntityAI item = ipw.GetItem();
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		
		if(!item.GetInventory().CanRemoveEntity())
			return;
	}
	
	void OnDropReceivedFromGhostArea(Widget w, int x, int y, Widget receiver)
	{
		ItemPreviewWidget ipw = ItemPreviewWidget.Cast( w.FindAnyWidget("Render"));
		string name = receiver.GetName();
		name.Replace("PanelWidget", "Render");
		ItemPreviewWidget receiver_iw = ItemPreviewWidget.Cast( receiver.FindAnyWidget(name));
		EntityAI receiver_item;
		if (receiver_iw)
		{
			receiver_item = receiver_iw.GetItem();
		}
		
		if( !ipw )
		{
			name = w.GetName();
			name.Replace( "PanelWidget", "Render" );
			ipw = ItemPreviewWidget.Cast( w.FindAnyWidget(name));
		}
		if(!ipw)
		{
		  ipw = ItemPreviewWidget.Cast(w);
		}
		
		if(!ipw.IsInherited(ItemPreviewWidget))
		{
			return;	
		}
		
		EntityAI item = ipw.GetItem();
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		
		if(!item.GetInventory().CanRemoveEntity())
		{
			return;
		}
		
		if(receiver_item)
		{
			if(receiver_item.GetInventory().CanAddAttachment(item))
			{
				player.PredictiveTakeEntityToTargetAttachment(receiver_item, item);
				return;
			}
			else if(receiver_item.GetInventory().CanAddEntityInCargo(item, item.GetInventory().GetFlipCargo()) && !receiver_item.GetInventory().HasEntityInCargo(item))
			{
				player.PredictiveTakeEntityToTargetInventory(receiver_item, FindInventoryLocationType.ANY, item);
				return;
			}
		}
		
		if(item)
		{
			if(m_ZombieEntity.GetInventory().CanAddAttachment(item))
			{
				player.PredictiveTakeEntityToTargetAttachment(m_ZombieEntity, item);
			}
			else if(m_ZombieEntity.GetInventory().CanAddEntityToInventory(item))
			{
				player.PredictiveTakeEntityToTargetInventory(m_ZombieEntity, FindInventoryLocationType.ANY, item);
			}
		}
		
		if( item )
		{
			if( m_ZombieEntity.GetInventory().CanAddEntityInto(item, FindInventoryLocationType.CARGO ))
			{
				player.PredictiveTakeEntityToTargetInventory(m_ZombieEntity, FindInventoryLocationType.CARGO, item);
			}
		}
	}

	void SetHeaderName()
	{
		m_CollapsibleHeader.SetName( "#container_inventory" );
	}
	
	void ItemAttached(EntityAI item, string slot_name)
	{
		int slot = InventorySlots.GetSlotIdFromString( slot_name );
		SlotsIcon icon = m_InventorySlots.Get( slot );
		if( icon )
		{
			icon.Init( item );

			if( m_ShowedItems.Contains( item ) == false )
			{
				string name;
				
				if( item.GetSlotsCountCorrect() > 0 )
				{
					ContainerWithCargoAndAttachments iwca = new ContainerWithCargoAndAttachments( this, false );
					iwca.SetEntity( item );
					WidgetEventHandler.GetInstance().RegisterOnMouseButtonUp( icon.GetPanelWidget(),  this, "ToggleContainer" );

					icon.GetRadialIconPanel().Show( true );
					m_ShowedItems.Insert( item, iwca );
					
					Refresh();
				}
				else if( item.GetInventory().GetCargo() )
				{
					ContainerWithCargo iwc = new ContainerWithCargo( this, false );
					iwc.SetEntity( item );
					iwc.UpdateInterval();
					WidgetEventHandler.GetInstance().RegisterOnMouseButtonUp( icon.GetPanelWidget(),  this, "ToggleContainer" );
					m_ShowedItems.Insert( item, iwc );
					icon.GetRadialIconPanel().Show(true);
					
					Refresh();
				}
			}
			
			ClosableContainer conta = ClosableContainer.Cast( m_ShowedItems.Get( item ) );
			string config = "CfgVehicles " + item.GetType() + " GUIInventoryAttachmentsProps";
			if( conta && conta.IsInherited( ClosableContainer ) )
			{
				bool show_radial_icon = ( item.GetInventory().GetCargo() || item.GetSlotsCountCorrect() > 0 ) && !GetGame().ConfigIsExisting( config );
				Widget rip = icon.GetRadialIconPanel();
				Widget icon_open = icon.GetRadialIcon();
				Widget icon_closed = icon.GetRadialIconClosed();				
				rip.Show( show_radial_icon );
				icon_open.Show( conta.IsOpened() );
				icon_closed.Show( !conta.IsOpened() );
			}
			RecomputeOpenedContainers();
		}
	}
	
	void ItemDetached(EntityAI item, string slot_name)
	{
		int slot = InventorySlots.GetSlotIdFromString( slot_name );
		SlotsIcon icon = m_InventorySlots.Get( slot );
		
		if( icon )
		{
			Container con = m_ShowedItems.Get( item );
			if( con )
			{
				( Container.Cast( con.m_Parent ) ).Remove( con );
				RecomputeOpenedContainers();
				GetMainWidget().Update();
				Refresh();
				m_ShowedItems.Remove( item );
			}
			
			icon.Clear();
			Inventory in = Inventory.Cast( GetRoot() );
			if( in )
				in.UpdateConsoleToolbar();
		}
	}

	// mouse button UP or call from ExpandCollapseContainer
	void ToggleContainer( Widget w )
	{
		EntityAI item;
		SlotsIcon slots_icon; 
		ClosableContainer c;
		w.GetUserData(slots_icon);
		if (slots_icon)
			item = slots_icon.GetEntity();
		if (item)
		{
			c = ClosableContainer.Cast( m_ShowedItems.Get( item ) );
		}
		else
		{
			c = ClosableContainer.Cast(slots_icon.GetContainer());
		}
		
		
		if (c)
		{
			c.Toggle();
		}
		RecomputeOpenedContainers();
	}
	
	SlotsIcon GetSlotsIcon( int row, int column )
	{
		return SlotsContainer.Cast( m_Container.Get( row ) ).GetSlotIcon( column );
	}

	void InitGhostSlots()
	{
		int row, column;
		SlotsIcon icon;
		bool has_cargo = m_ZombieEntity.GetInventory().GetCargo() != null;
		
		string config_path_ghosts_slots = "CfgVehicles ZombieBase InventoryEquipment playerSlots";
		ref array<string> player_ghosts_slots = new array<string>;
		GetGame().ConfigGetTextArray( config_path_ghosts_slots, player_ghosts_slots );
		
		for ( int i = 0; i < player_ghosts_slots.Count(); i++ )
		{
			string slot_name = player_ghosts_slots.Get ( i );
			string path = "CfgSlots" + " " + slot_name;

			if ( GetGame().ConfigIsExisting( path ) )
			{
				string icon_name; //icon_name must be in format "set:<setname> image:<imagename>"
				GetGame().ConfigGetText( path + " ghostIcon", icon_name );
				column = i % ITEMS_IN_ROW;

				//START - GetWidgetSlot
				row = i / ITEMS_IN_ROW;
				if( row >= m_Container.Count() )
				{
					if ( has_cargo )
					{
						if( row < ( (player_ghosts_slots.Count() + 1 )/ ITEMS_IN_ROW ) )
							AddSlotsContainer( ITEMS_IN_ROW );
						else
							AddSlotsContainer( ( player_ghosts_slots.Count() + 1 ) % ITEMS_IN_ROW );
					
					}
					else
					{
						if ( row < ( player_ghosts_slots.Count() / ITEMS_IN_ROW ) )
							AddSlotsContainer( ITEMS_IN_ROW );
						else
							AddSlotsContainer( player_ghosts_slots.Count() % ITEMS_IN_ROW );
					}

				}
		
				icon = GetSlotsIcon( row, column );
				
				icon.GetMainWidget().Show( true );
				icon.Clear();
				
				WidgetEventHandler.GetInstance().RegisterOnDoubleClick( icon.GetPanelWidget(),  this, "DoubleClick" );
				
				//END - GetWidgetSlot
				WidgetEventHandler.GetInstance().RegisterOnDropReceived( icon.GetPanelWidget(),  this, "OnDropReceivedFromGhostArea" );
				WidgetEventHandler.GetInstance().RegisterOnDropReceived( icon.GetGhostSlot(),  this, "OnDropReceivedFromGhostArea" );
				WidgetEventHandler.GetInstance().RegisterOnDraggingOver( icon.GetPanelWidget(),  this, "DraggingOver" );
				WidgetEventHandler.GetInstance().RegisterOnDraggingOver( icon.GetGhostSlot(),  this, "DraggingOver" );
				WidgetEventHandler.GetInstance().RegisterOnDraggingOver( icon.GetPanelWidget(),  this, "DraggingOver" );
				WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown( icon.GetPanelWidget(),  this, "MouseClick" );
				
				icon.GetGhostSlot().LoadImageFile( 0, StaticGUIUtils.VerifyIconImageString(StaticGUIUtils.IMAGESETGROUP_INVENTORY,icon_name) );
				//END - LoadIconIntoWidgetSlot

				GetGame().ConfigGetText( path + " name", slot_name );
				int slot_id = InventorySlots.GetSlotIdFromString( slot_name );
				icon.SetSlotID( slot_id );
				icon.SetSlotDisplayName(InventorySlots.GetSlotDisplayName(slot_id));
				m_InventorySlots.Set( slot_id, icon );
				
				int slot = InventorySlots.GetSlotIdFromString( slot_name );
				ItemBase item = ItemBase.Cast( m_ZombieEntity.GetInventory().FindAttachment( slot ) );
				
				if( item )
					ItemAttached( item, slot_name );
			}
		}
		
		if( m_ZombieEntity.GetInventory().GetCargo() )
		{
			row = player_ghosts_slots.Count() / ITEMS_IN_ROW;
			column = player_ghosts_slots.Count() % ITEMS_IN_ROW;
			if( row >= m_Container.Count() )
			{
				if( row < ( player_ghosts_slots.Count() + 1 / ITEMS_IN_ROW ) )
					AddSlotsContainer( ITEMS_IN_ROW );
				else
					AddSlotsContainer( ( player_ghosts_slots.Count() + 1 ) % ITEMS_IN_ROW );
			}
			
			icon = GetSlotsIcon( row, column );
			icon.GetMainWidget().Show( true );
			icon.Clear();
			icon.GetGhostSlot().LoadImageFile( 0, StaticGUIUtils.VerifyIconImageString(StaticGUIUtils.IMAGESETGROUP_INVENTORY,m_ZombieEntity.ConfigGetString("GUIInventoryCargoIcon")) );
			icon.SetSlotDisplayName( m_ZombieEntity.ConfigGetString("GUIInventoryCargoName") );
			icon.GetRadialIconPanel().Show( true );
			icon.GetRadialIcon().Show( true );
			
			WidgetEventHandler.GetInstance().RegisterOnMouseButtonUp( icon.GetGhostSlot(), this, "ToggleContainer" );
			m_CargoGrid = new ContainerWithCargo( this, false );
			m_CargoGrid.SetEntity( m_ZombieEntity );
			m_CargoGrid.GetHeader().GetMainWidget().ClearFlags( WidgetFlags.DRAGGABLE );
			m_ShowedItems.Insert( m_ZombieEntity, m_CargoGrid );
			icon.SetContainer(m_CargoGrid);
		}
		
		RecomputeOpenedContainers();
	}

	void AddSlotsContainer( int row_count )
	{
		ref SlotsContainer s_cont = new SlotsContainer( m_Container, m_ZombieEntity );
		s_cont.SetColumnCount( row_count );
		m_Container.Insert( s_cont );
	}

	Widget GetWidgetSlot( int slot_number )
	{
		int row = slot_number / ITEMS_IN_ROW;
		int column = slot_number % ITEMS_IN_ROW;

		return m_Container.Get( row ).GetMainWidget().FindAnyWidget( "Render" + column );
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
			
			ItemBase item = ItemBase.Cast( iw.GetItem() );
			if( !item )
			{
				return;
			}
			
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			
			if( player.GetInventory().HasInventoryReservation( item, null ) || player.IsItemsToDelete() )
			{
				return;
			}
			
			if( !item.GetInventory().CanRemoveEntity() )
				return;
					
			if ( player.GetInventory().HasEntityInInventory( item ) && GetGame().GetPlayer().GetHumanInventory().CanAddEntityInHands( item ) )
			{
				player.PredictiveTakeEntityToHands( item );
			}
			else
			{
				InventoryLocation il = new InventoryLocation;
				if( player.GetInventory().FindFreeLocationFor( item, FindInventoryLocationType.ANY, il) )
				{
					if( item.GetTargetQuantityMax(il.GetSlot()) < item.GetQuantity() )
						item.SplitIntoStackMaxToInventoryLocationClient( il );
					else
						player.PredictiveTakeEntityToInventory( FindInventoryLocationType.ANY, item );
				}
				else if( GetGame().GetPlayer().GetHumanInventory().CanAddEntityInHands( item ) )
				{
					if( item.GetTargetQuantityMax() < item.GetQuantity() )
						item.SplitIntoStackMaxHandsClient( player );
					else
						player.PredictiveTakeEntityToHands( item );
				}
			}
			
			HideOwnedTooltip();
			InventoryMenu menu = InventoryMenu.Cast( GetGame().GetUIManager().FindMenu( MENU_INVENTORY ) );
			if( menu )
			{
				menu.RefreshQuickbar();
			}
		}
	}

	override void CollapseButtonOnMouseButtonDown(Widget w)
	{
		super.CollapseButtonOnMouseButtonDown(w);
	}
	
	override bool SplitItem()
	{
		if( GetFocusedContainer().IsInherited( ContainerWithCargo ) || GetFocusedContainer().IsInherited( ContainerWithCargoAndAttachments ) )
		{
			return GetFocusedContainer().SplitItem();
		}
		
		return false;
	}
	
	override bool EquipItem()
	{
		if( GetFocusedContainer().IsInherited( ContainerWithCargo ) || GetFocusedContainer().IsInherited( ContainerWithCargoAndAttachments ) )
		{
			return GetFocusedContainer().EquipItem();
		}
		else
		{
			EntityAI item = GetFocusedItem();
			if( item )
			{
				GetGame().GetPlayer().PredictiveTakeEntityToInventory( FindInventoryLocationType.ATTACHMENT, item );
				return true;
			}
		}
		return false;
	}
	
	override bool TransferItem()
	{
		LeftArea left_area = LeftArea.Cast( GetParent() );
		EntityAI item;
		if( left_area )
		{
			if( GetFocusedContainer().IsInherited( ContainerWithCargo ) || GetFocusedContainer().IsInherited( ContainerWithCargoAndAttachments ) )
			{
				return GetFocusedContainer().TransferItem();
			}
			else
			{
				item = GetFocusedItem();
				if( item )
				{
					GetGame().GetPlayer().PredictiveTakeEntityToInventory( FindInventoryLocationType.CARGO, item );
					return true;
				}
			}
		}
		else
		{
			if( !GetFocusedContainer().IsInherited( ContainerWithCargo ) && !GetFocusedContainer().IsInherited( ContainerWithCargoAndAttachments ) )
			{
				item = GetFocusedItem();
				if( item )
				{
					GetGame().GetPlayer().PredictiveTakeEntityToInventory( FindInventoryLocationType.CARGO, item );
					return true;
				}
			}
		}
		return false;
	}
	
	override bool Select()
	{
		if( GetFocusedContainer().IsInherited( ContainerWithCargo ) || GetFocusedContainer().IsInherited( ContainerWithCargoAndAttachments ) )
		{
			return GetFocusedContainer().Select();
		}
		else
		{
			SlotsIcon selected_slot = ItemManager.GetInstance().GetSelectedIcon();
			EntityAI focused_item = GetFocusedItem();
			EntityAI selected_item = ItemManager.GetInstance().GetSelectedItem();
			if( !(selected_slot && selected_slot.IsOutOfReach() ) )
			{
				if( selected_item )
				{
					if( selected_item != focused_item)
					{
						if( selected_item.GetInventory().CanRemoveEntity() )
						{
							GetGame().GetPlayer().PredictiveTakeEntityToTargetInventory( m_ZombieEntity, FindInventoryLocationType.ANY, selected_item );
							ItemManager.GetInstance().SetSelectedItem( null, null, null, null );
							return true;
						}
					}
				}
				else
				{
					if( focused_item && focused_item.GetInventory().CanRemoveEntity() )
					{
						EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
						if( item_in_hands && item_in_hands.GetInventory().CanRemoveEntity() )
						{
							if( GameInventory.CanSwapEntitiesEx( item_in_hands, focused_item ) )
							{
								GetGame().GetPlayer().PredictiveSwapEntities( item_in_hands, focused_item );
								return true;
							}
							else
							{
								InventoryLocation il_hands_dst = new InventoryLocation;
								if( GetGame().GetPlayer().GetHumanInventory().FindFreeLocationFor( item_in_hands, FindInventoryLocationType.ANY, il_hands_dst ) )
								{
									InventoryMode invMode = InventoryMode.PREDICTIVE;
						
									if ( GetGame().GetPlayer().NeedInventoryJunctureFromServer(item_in_hands, item_in_hands.GetHierarchyParent(), il_hands_dst.GetParent()) || GetGame().GetPlayer().NeedInventoryJunctureFromServer(focused_item, focused_item.GetHierarchyParent(), GetGame().GetPlayer()) )
										invMode = InventoryMode.JUNCTURE;
									
									GetGame().GetPlayer().GetHumanInventory().ForceSwapEntities( InventoryMode.JUNCTURE, focused_item, item_in_hands, il_hands_dst );
									return true;
								}
							}
						}
						else
						{
							if( GetGame().GetPlayer().GetHumanInventory().CanAddEntityInHands( focused_item ) )
							{
								GetGame().GetPlayer().PredictiveTakeEntityToHands( focused_item );
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}
	
	override bool TransferItemToVicinity()
	{
		if( GetFocusedContainer().IsInherited( ContainerWithCargo ) || GetFocusedContainer().IsInherited( ContainerWithCargoAndAttachments ) )
		{
			return GetFocusedContainer().TransferItemToVicinity();
		}
		else
		{
			Man player = GetGame().GetPlayer();
			ItemBase item = ItemBase.Cast( GetFocusedItem() );
			if( item && player.CanDropEntity( item ) )
			{
				if( item.GetTargetQuantityMax() < item.GetQuantity() )
					item.SplitIntoStackMaxClient( null, -1 );
				else
					player.PhysicalPredictiveDropItem( item );
				return true;
			}
		}
		return false;
	}
}
