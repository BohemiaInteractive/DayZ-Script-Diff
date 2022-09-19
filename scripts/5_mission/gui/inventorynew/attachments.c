const int ITEMS_IN_ROW = 8;

class Attachments
{
	protected Container							m_Parent;
	protected EntityAI							m_Entity;
	protected ref AttachmentsWrapper			m_AttachmentsContainer;
	protected ref array<string>					m_AttachmentSlotNames;
	protected ref array<string>					m_AttachmentSlotDisplayable;
	protected ref map<int, SlotsIcon>			m_AttachmentSlots;
	protected ref array<int>					m_AttachmentIDOrdered;
	
	protected int m_RowIndex;

	void Attachments( Container parent, EntityAI entity )
	{
		m_Parent					= parent;
		m_Entity					= entity;
		m_AttachmentSlots			= new map<int, SlotsIcon>;
		m_AttachmentIDOrdered		= new array<int>;
		m_AttachmentSlotNames		= GetItemSlots( entity );
		m_AttachmentSlotDisplayable	= new array<string>;
		m_Entity.GetOnItemAttached().Insert( AttachmentAdded );
		m_Entity.GetOnItemDetached().Insert( AttachmentRemoved );
		m_Entity.GetOnAttachmentSetLock().Insert( OnAttachmentReservationSet );
		m_Entity.GetOnAttachmentReleaseLock().Insert( OnAttachmentReservationRelease );
		
	}
	
	void ~Attachments()
	{
		if( m_Entity )
		{
			m_Entity.GetOnItemAttached().Remove( AttachmentAdded );
			m_Entity.GetOnItemDetached().Remove( AttachmentRemoved );
			m_Entity.GetOnAttachmentSetLock().Remove( OnAttachmentReservationSet );
			m_Entity.GetOnAttachmentReleaseLock().Remove( OnAttachmentReservationRelease );
		}
		
		delete m_AttachmentsContainer;
	}
	
	AttachmentsWrapper GetWrapper()
	{
		return m_AttachmentsContainer;
	}
	
	bool IsEmpty()
	{
		return m_AttachmentsContainer.IsEmpty();
	}
	
	bool IsItemActive()
	{
		ItemBase item = ItemBase.Cast( GetFocusedItem() );
		if( !item )
		{
			return false;
		}
		return !IsEmpty() && ( !QuantityConversions.HasItemQuantity( item )  || ( QuantityConversions.HasItemQuantity( item ) && !item.CanBeSplit() ) );
	}
	
	bool IsItemWithQuantityActive()
	{
		ItemBase item = ItemBase.Cast( GetFocusedItem() );
		if( !item )
		{
			return false;
		}
		return !IsEmpty() && QuantityConversions.HasItemQuantity( item ) && item.CanBeSplit();
	}

	void UnfocusAll()
	{
		m_AttachmentsContainer.UnfocusAll();
	}

	void SetDefaultFocus( bool while_micromanagment_mode = false )
	{
		m_AttachmentsContainer.SetDefaultFocus(while_micromanagment_mode);
	}
	
	void SetLastActive()
	{
		m_AttachmentsContainer.SetLastActive();
	}
	
	void SetActive( bool active )
	{
		m_AttachmentsContainer.SetActive(active);
	}
	
	void SelFirstActive()
	{
		m_AttachmentsContainer.SetFirstActive();
	}
	
	SlotsIcon GetFocusedSlotsIcon()
	{
		return m_AttachmentsContainer.GetFocusedSlotsIcon();
	}
	
	EntityAI GetFocusedItem()
	{
		return m_AttachmentsContainer.GetFocusedItem();
	}
	
	int GetFocusedSlot()
	{
		SlotsIcon icon = m_AttachmentsContainer.GetFocusedSlotsIcon();
		if (icon)
			return icon.GetSlotID();
		return -1;
	}
	
	bool SelectItem()
	{
		ItemBase item = ItemBase.Cast( GetFocusedItem() );
		SlotsIcon icon = GetFocusedSlotsIcon();
		
		if (icon && !icon.IsOutOfReach())
		{
			ItemManager.GetInstance().SetSelectedItem( item, null, icon.GetCursorWidget(), icon );
			return true;
		}
		return false;
	}
	
	bool Select()
	{
		SlotsIcon selected_slot = ItemManager.GetInstance().GetSelectedIcon();
		EntityAI selected_item = ItemManager.GetInstance().GetSelectedItem();
		SlotsIcon focused_slot = GetFocusedSlotsIcon();
		EntityAI focused_item = GetFocusedItem();
		Man player = GetGame().GetPlayer();
		
		if( focused_slot.IsReserved() || focused_item != selected_item && !(selected_slot && selected_slot.IsOutOfReach() ) )
		{
			if( selected_item )
			{
				if( selected_item.GetInventory().CanRemoveEntity() )
				{
					if( m_Entity.GetInventory().CanAddAttachmentEx( selected_item, focused_slot.GetSlotID() ) )
					{
						player.PredictiveTakeEntityToTargetAttachmentEx( m_Entity, selected_item, focused_slot.GetSlotID() );
						ItemManager.GetInstance().SetSelectedItem( null, null, null, null );
						return true;
					
					}
					else if( m_Entity.GetInventory().CanAddAttachment( selected_item ) )
					{
						player.PredictiveTakeEntityToTargetAttachment(m_Entity, selected_item);
						ItemManager.GetInstance().SetSelectedItem( null, null, null, null );
						return true;
					}
				}
			}
			else
			{
				if ( focused_item && !focused_slot.IsOutOfReach() )
				{
					EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
					InventoryLocation il = new InventoryLocation;
					focused_item.GetInventory().GetCurrentInventoryLocation( il );
					bool reachable = AttachmentsOutOfReach.IsAttachmentReachable( m_Entity, "", il.GetSlot() );
					if( reachable && focused_item.GetInventory().CanRemoveEntity() )
					{
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
			}	
		}
		return false;
	}
	
	int GetRecipeCount( bool recipe_anywhere, EntityAI entity1, EntityAI entity2 )
	{
		PluginRecipesManager plugin_recipes_manager = PluginRecipesManager.Cast( GetPlugin( PluginRecipesManager ) );
		return plugin_recipes_manager.GetValidRecipes( ItemBase.Cast( entity1 ), ItemBase.Cast( entity2 ), null, PlayerBase.Cast( GetGame().GetPlayer() ) );
	}
	
	bool CanCombine()
	{
		ItemBase ent = ItemBase.Cast( GetFocusedItem() );
		ItemBase item_in_hands = ItemBase.Cast(	GetGame().GetPlayer().GetHumanInventory().GetEntityInHands() );
		
		return ( ItemManager.GetCombinationFlags( item_in_hands, ent ) != 0 );
	}
	
	bool CanCombineAmmo()
	{
		PlayerBase m_player = PlayerBase.Cast( GetGame().GetPlayer() );
		ItemBase ent = ItemBase.Cast(  GetFocusedItem() );
		ItemBase item_in_hands = ItemBase.Cast(	GetGame().GetPlayer().GetHumanInventory().GetEntityInHands() );
		ActionManagerClient amc;
		Class.CastTo(amc, m_player.GetActionManager());

		return ( amc.CanPerformActionFromInventory( item_in_hands, ent ) );
	}
	
	bool CanEquip()
	{
		EntityAI entity = ItemBase.Cast( GetFocusedItem() );
		InventoryLocation il = new InventoryLocation;
		if( !entity || entity.IsInherited( Magazine ) )
		{
			return false;
		}
		return GetGame().GetPlayer().GetInventory().FindFreeLocationFor( entity, FindInventoryLocationType.ATTACHMENT, il );
	}
	
	bool Combine()
	{
		ItemBase ent = ItemBase.Cast( GetFocusedItem() );
		ItemBase item_in_hands = ItemBase.Cast(	GetGame().GetPlayer().GetHumanInventory().GetEntityInHands() );
		Icon hands_icon = ItemManager.GetInstance().GetHandsPreview().GetIcon();
		
		if ( item_in_hands && ent && hands_icon )
		{
			return hands_icon.CombineItems( item_in_hands, ent );
		}
		
		return false;
	}
	
	bool SplitItem()
	{
		ItemBase entity = ItemBase.Cast( GetFocusedItem() );
		if( entity && !entity.IsInherited( Magazine ) && !GetFocusedSlotsIcon().IsOutOfReach() )
		{
			if( entity.HasQuantity() )
			{
				entity.OnRightClick();
				return true;
			}
		}
		return false;
	}
	
	bool EquipItem()
	{
		ItemBase entity = ItemBase.Cast( GetFocusedItem() );
		if( entity && !entity.IsInherited( Magazine ) && !GetFocusedSlotsIcon().IsOutOfReach() )
		{
			GetGame().GetPlayer().PredictiveTakeEntityToInventory( FindInventoryLocationType.ATTACHMENT, entity );
			return true;
		}
		return false;
	}
	
	bool TransferItem()
	{
		EntityAI entity = GetFocusedItem();
		if( entity && !GetFocusedSlotsIcon().IsOutOfReach() )
		{
			GetGame().GetPlayer().PredictiveTakeEntityToInventory( FindInventoryLocationType.CARGO, entity );
			return true;
		}
		return false;
	}
	
	bool InspectItem()
	{
		EntityAI entity = GetFocusedItem();
		if( entity )
		{
			m_Parent.InspectItem( entity );
			return true;
		}
		return false;
	}
	
	bool TransferItemToVicinity()
	{
		ItemBase item = ItemBase.Cast(GetFocusedItem());
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if( item && !GetFocusedSlotsIcon().IsOutOfReach() )
		{
			if( item.GetTargetQuantityMax() < item.GetQuantity() )
				item.SplitIntoStackMaxClient( null, -1 );
			else
				player.PhysicalPredictiveDropItem( item );
			return true;
		}
		return false;
	}
	
	bool IsActive()
	{
		return m_Parent.GetMainWidget().FindAnyWidget("AttachmentsWrapper").GetAlpha() > 0;
	}
	
	int GetAttachmentHeight()
	{
		return m_AttachmentsContainer.Count();
	}
	
	void UpdateInterval()
	{
		SlotsIcon icon;
		int slot_id;
		m_AttachmentSlotDisplayable.Clear();
		int i = 0;
		for (i = m_AttachmentSlotNames.Count() - 1; i >=0; --i)
		{
			slot_id = InventorySlots.GetSlotIdFromString( m_AttachmentSlotNames[i] );
			if (m_Entity.CanDisplayAttachmentSlot(slot_id))
			{
				m_AttachmentSlotDisplayable.Insert(m_AttachmentSlotNames[i]);
			}
			else
			{
				icon = m_AttachmentSlots.Get( slot_id );
				if (icon)
				{
					icon.GetMainWidget().Show( false );
					if( GetFocusedSlotsIcon() == icon )
					{
						SetDefaultFocus();
					}
					// radial icon (collabsable icon handling)
					icon.UpdateInterval();
				}
			}
			
		}
		
		if ( m_AttachmentSlotDisplayable.Count() == 0 )
		{
			if (m_Parent)
			{
				m_Parent.UpdateRadialIcon();
				//m_Parent.Close();
			}
		}
		
		for (i = 0; i < m_AttachmentSlotDisplayable.Count(); ++i)
		{
			slot_id = InventorySlots.GetSlotIdFromString( m_AttachmentSlotDisplayable[i] );
			icon = m_AttachmentSlots.Get( slot_id );
			EntityAI item = icon.GetEntity();
			icon.GetMainWidget().Show( true );
			icon.UpdateInterval();
			if ( item )
			{
				bool draggable = true;
				if(icon.IsReserved())
				{
					draggable = false;
				}
				
				if( m_Entity.GetInventory().GetSlotLock( slot_id ) && ItemManager.GetInstance().GetDraggedItem() != item )
				{
					icon.GetMountedWidget().Show( true );
					draggable = false;
				}
				else
				{
					icon.GetMountedWidget().Show( false );
				}
				
				PlayerBase p = PlayerBase.Cast( GetGame().GetPlayer() );
				bool in_hands_condition		= m_Entity.GetHierarchyRoot() && item.GetInventory().CanRemoveEntity();
				bool in_vicinity_condition	= !m_Entity.GetHierarchyRoot() && AttachmentsOutOfReach.IsAttachmentReachable( m_Entity, m_AttachmentSlotDisplayable[i] );
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
					icon.GetPanelWidget().SetFlags( WidgetFlags.DRAGGABLE );
				}
				else
				{
					icon.GetPanelWidget().ClearFlags( WidgetFlags.DRAGGABLE );
				}
			}
		}
		m_AttachmentsContainer.GetRootWidget().Update();
	}
	
	array<int> GetSlotsSorted()
	{
		return m_AttachmentIDOrdered;
	}
	
	void AttachmentAdded(EntityAI item, string slot, EntityAI parent)
	{
		int slot_id = InventorySlots.GetSlotIdFromString(slot);
		SlotsIcon icon = m_AttachmentSlots.Get(slot_id);
		if (icon)
		{
			icon.SetSlotID(slot_id);
			icon.SetSlotDisplayName(InventorySlots.GetSlotDisplayName(slot_id));
			if (item)
			{
				icon.Init(item);
			}
		}
	}
	
	void AttachmentRemoved(EntityAI item, string slot, EntityAI parent)
	{
		int slot_id = InventorySlots.GetSlotIdFromString(slot);
		SlotsIcon icon = m_AttachmentSlots.Get(slot_id);
		if (icon)
		{
			icon.Clear();
		}
	}
	
	void OnAttachmentReservationSet( EntityAI item, int slot_id )
	{
		SlotsIcon icon = m_AttachmentSlots.Get( slot_id );
		
		if (item)
		{
			icon.Init( item, true );
		}
	}
	
	void OnAttachmentReservationRelease( EntityAI item, int slot_id )
	{
		SlotsIcon icon = m_AttachmentSlots.Get( slot_id );
		icon.Clear();
	}
	

	void InitAttachmentGrid( int att_row_index )
	{
		SlotsIcon icon;
		int i;
		m_RowIndex = att_row_index;

		int number_of_rows = Math.Ceil( m_AttachmentSlotNames.Count() / ITEMS_IN_ROW );
		string name = m_Entity.GetDisplayName();
		name.ToUpper();
		
		m_AttachmentsContainer = new AttachmentsWrapper( m_Parent );
		m_AttachmentsContainer.SetParent( this );
		m_AttachmentsContainer.SetFalseAttachmentsHeaderText(name);
		m_AttachmentsContainer.GetRootWidget().SetSort( att_row_index );
		m_Parent.Insert( m_AttachmentsContainer, att_row_index );
		
		for ( i = 0; i < number_of_rows; i++ )
		{
			SlotsContainer ic = new SlotsContainer( m_AttachmentsContainer, m_Entity );
			m_AttachmentsContainer.Insert( ic );

			if( i == ( number_of_rows - 1 ) && m_AttachmentSlotNames.Count() % ITEMS_IN_ROW != 0 )
			{
				ic.SetColumnCount( m_AttachmentSlotNames.Count() % ITEMS_IN_ROW );
			}
			else
			{
				ic.SetColumnCount( ITEMS_IN_ROW );
			}

			for( int j = 0; j < ITEMS_IN_ROW; j++ )
			{
				icon = ic.GetSlotIcon( j );
				WidgetEventHandler.GetInstance().RegisterOnDropReceived( icon.GetMainWidget(), m_Parent, "OnDropReceivedFromHeader2" );
				WidgetEventHandler.GetInstance().RegisterOnDropReceived( icon.GetGhostSlot(), m_Parent, "OnDropReceivedFromHeader2" );
				WidgetEventHandler.GetInstance().RegisterOnDropReceived( icon.GetPanelWidget(), m_Parent, "OnDropReceivedFromHeader2" );

				WidgetEventHandler.GetInstance().RegisterOnDraggingOver( icon.GetMainWidget(), m_Parent, "DraggingOverHeader" );
				WidgetEventHandler.GetInstance().RegisterOnDraggingOver( icon.GetGhostSlot(), m_Parent, "DraggingOverHeader" );
				WidgetEventHandler.GetInstance().RegisterOnDraggingOver( icon.GetPanelWidget(), m_Parent, "DraggingOverHeader" );
				
				WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown( icon.GetMainWidget(), m_Parent, "MouseClick2" );
				WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown( icon.GetGhostSlot(), m_Parent, "MouseClick2" );
				WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown( icon.GetPanelWidget(), m_Parent, "MouseClick2" );
			}
		}

		for ( i = 0; i < m_AttachmentSlotNames.Count(); i++ )
		{
			icon = SlotsContainer.Cast( m_AttachmentsContainer.Get( ( i / ITEMS_IN_ROW ) ) ).GetSlotIcon( i % ITEMS_IN_ROW );
			WidgetEventHandler.GetInstance().RegisterOnDoubleClick( icon.GetPanelWidget(), m_Parent, "DoubleClick" );
			
			string path = "CfgSlots" + " Slot_" + m_AttachmentSlotNames[i];

			//Show different magazine icon for firearms and pistols
			if ( m_AttachmentSlotNames[i] == "magazine" )
			{
				if ( !m_Entity.IsInherited( Pistol_Base ) )
					path = "CfgSlots" + " Slot_" + "magazine2";
			}
			
			string icon_name = ""; //icon_name must be in format "set:<setname> image:<imagename>"
			if( GetGame().ConfigGetText( path + " ghostIcon", icon_name ) && icon_name != "" )
				icon.GetGhostSlot().LoadImageFile( 0, StaticGUIUtils.VerifyIconImageString(StaticGUIUtils.IMAGESETGROUP_INVENTORY,icon_name) );
			int slot_id = InventorySlots.GetSlotIdFromString( m_AttachmentSlotNames[i] );
			m_AttachmentSlots.Insert( slot_id, icon );
			m_AttachmentIDOrdered.Insert(slot_id);
			
			icon.SetSlotID(slot_id);
			icon.SetSlotDisplayName(InventorySlots.GetSlotDisplayName(slot_id));
			
			EntityAI item = m_Entity.GetInventory().FindAttachment( slot_id );
			if( item )
				AttachmentAdded( item, m_AttachmentSlotNames[i], m_Entity );
			else
				icon.Clear();
			
			if (m_Entity.CanDisplayAttachmentSlot(slot_id))
			{
				icon.GetMainWidget().Show( true );
			}
			else
			{
				icon.GetMainWidget().Show( false );
			}
		}
		
		if( m_AttachmentSlotNames.Count() > 0 )
		{
			int row_index = number_of_rows - 1;
			SlotsContainer row_last = SlotsContainer.Cast( m_AttachmentsContainer.Get( row_index ) );
			if( row_last )
			{
				for( int k = ((m_AttachmentSlotNames.Count() - 1) % ITEMS_IN_ROW) + 1; k < ITEMS_IN_ROW; k++ )
				{
					row_last.GetSlotIcon( k ).GetMainWidget().Show( false );
				}
				row_last.GetRootWidget().Update();
				row_last.GetRootWidget().GetParent().Update();
			}
		}
		
		m_AttachmentsContainer.RecomputeOpenedContainers();
	}

	array<string> GetItemSlots( EntityAI e )
	{
		TStringArray searching_in = new TStringArray;
		searching_in.Insert( CFG_VEHICLESPATH );
		searching_in.Insert( CFG_WEAPONSPATH );
		searching_in.Insert( CFG_MAGAZINESPATH );

		array<string> attachments_slots	= new array<string>;
		int i = 0;
		for ( int s = 0; s < searching_in.Count(); ++s )
		{
			string cfg_name = searching_in.Get( s );
			string path = cfg_name + " " + e.GetType();

			if ( GetGame().ConfigIsExisting( path ) )
			{
				GetGame().ConfigGetTextArray( path + " attachments", attachments_slots );
				if ( e.IsWeapon() && (!e.ConfigIsExisting("DisplayMagazine") || e.ConfigGetBool("DisplayMagazine")) )
				{
					attachments_slots.Insert( "magazine" );
				}

				return attachments_slots;
			}
		}
		if ( e.IsWeapon() && (!e.ConfigIsExisting("DisplayMagazine") || e.ConfigGetBool("DisplayMagazine")) )
		{
			attachments_slots.Insert( "magazine" );
		}

		return attachments_slots;
	}
	
	void ShowFalseAttachmentsHeader(bool show)
	{
		m_AttachmentsContainer.ShowFalseAttachmentsHeader(show);
	}
	
	void SetFalseAttachmentsHeaderText(string text)
	{
		m_AttachmentsContainer.SetFalseAttachmentsHeaderText(text);
	}
	
	TextWidget GetFalseHeaderTextWidget()
	{
		return m_AttachmentsContainer.GetFalseHeaderTextWidget();
	}
}
