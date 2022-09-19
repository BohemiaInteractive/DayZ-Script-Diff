class ContainerWithCargoAndAttachments extends ClosableContainer
{
	protected ref Attachments								m_Atts;
	protected ref CargoContainer							m_CargoGrid;
	
	protected ref map<EntityAI, ref CargoContainer>			m_AttachmentCargos;
	protected ref map<EntityAI, ref AttachmentsWrapper>		m_AttachmentAttachmentsContainers;
	protected ref map<EntityAI, ref Attachments>			m_AttachmentAttachments;
	protected ref array<int>								m_AttachmentSlotsSorted;
	
	void ContainerWithCargoAndAttachments( LayoutHolder parent, int sort = -1 )
	{
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver( m_MainWidget,  this, "DraggingOverHeader2" );
	}

	void ~ContainerWithCargoAndAttachments()
	{
		if ( m_Entity )
		{
			m_Entity.GetOnItemAttached().Remove( AttachmentAdded );
			m_Entity.GetOnItemDetached().Remove( AttachmentRemoved );
		}
		
		if ( m_Atts )
			delete m_Atts;
		
		foreach ( EntityAI e, Attachments att : m_AttachmentAttachments )
		{
			delete att;
		}
		
		if ( m_AttachmentAttachments )
			m_AttachmentAttachments.Clear();
		
		if ( m_AttachmentAttachmentsContainers )
			m_AttachmentAttachmentsContainers.Clear();
		
		if ( m_AttachmentCargos )
			m_AttachmentCargos.Clear();
	}
	
	void RecomputeContainers()
	{
		m_Body.Clear();

		if(m_Atts)
		{
			m_Body.Insert(m_Atts.GetWrapper());
		}

		if(m_CargoGrid)
		{
			m_Body.Insert(m_CargoGrid);
		}
		
		GameInventory inv = m_Entity.GetInventory();
		for (int i = 0; i < inv.AttachmentCount(); i++)
		{
			EntityAI ent = inv.GetAttachmentFromIndex(i);
			if(ent)
			{
				AttachmentsWrapper att = m_AttachmentAttachmentsContainers.Get(ent);
				if(att)
				{
					m_Body.Insert(att);
				}
				
				CargoContainer cargo = m_AttachmentCargos.Get(ent);
				if(cargo)
				{
					m_Body.Insert(cargo);
				}
			}
		}
	}
	
	void AttachmentAddedEx(EntityAI item, string slot, EntityAI parent, bool immedUpdate = true)
	{
		int slot_id = InventorySlots.GetSlotIdFromString( slot );
		int sort = -1;
		bool updateNeeded = false;
		ref Attachments att_cont = null;
		ref CargoContainer cont = null;
		
		if ( item.GetInventory().GetAttachmentSlotsCount() > 0 && item.CanDisplayAnyAttachmentSlot() )
		{
			updateNeeded = true;
			
			att_cont = new Attachments( this, item );
			sort = (m_AttachmentSlotsSorted.Find( slot_id ) * 2) + SORT_ATTACHMENTS_NEXT_OFFSET;
			att_cont.InitAttachmentGrid( sort );
			
			m_AttachmentAttachments.Insert( item, att_cont );
			m_AttachmentAttachmentsContainers.Insert( item, att_cont.GetWrapper() );
			
			att_cont.UpdateInterval();
		}
		
		if ( item.GetInventory().GetCargo() )
		{
			updateNeeded = true;
			
			cont = new CargoContainer( this, true );
			sort = (m_AttachmentSlotsSorted.Find( slot_id ) * 2) + SORT_CARGO_NEXT_OFFSET;
			cont.GetRootWidget().SetSort( sort );
			cont.SetEntity( item, false );
			Insert( cont, m_Atts.GetAttachmentHeight() + m_AttachmentCargos.Count() + 1 );
			
			m_AttachmentCargos.Insert( item, cont );
		}
		
		if (updateNeeded)
		{
			if (att_cont)
			{
				att_cont.ShowFalseAttachmentsHeader(true);
				if (cont)
				{
					cont.ShowFalseCargoHeader(false);
					cont.UpdateSize();
					cont.SetAlternateFalseTextHeaderWidget(att_cont.GetFalseHeaderTextWidget());
				}
			}
			else if (cont)
			{
				cont.SetAlternateFalseTextHeaderWidget(null); //just to be safe..
			}
			
			RecomputeContainers();
			RecomputeOpenedContainers();
			
			Inventory.GetInstance().UpdateConsoleToolbar();
			
			if (m_Parent && immedUpdate)
				m_Parent.Refresh();
		}
	}
	
	void AttachmentAdded(EntityAI item, string slot, EntityAI parent)
	{
		AttachmentAddedEx(item, slot, parent);
	}
	
	void AttachmentRemoved(EntityAI item, string slot, EntityAI parent)
	{
		int slot_id = InventorySlots.GetSlotIdFromString( slot );
		
		CargoContainer old_cont = m_AttachmentCargos.Get( item );
		if( old_cont )
		{
			m_AttachmentCargos.Remove( item );
			delete old_cont;
			
			if( m_Parent )
				m_Parent.Refresh();
			Inventory.GetInstance().UpdateConsoleToolbar();
		}
				
		
		AttachmentsWrapper old_att_cont = m_AttachmentAttachmentsContainers.Get( item );
		if( old_att_cont )
		{
			m_AttachmentAttachmentsContainers.Remove( item );
			m_AttachmentAttachments.Remove( item );
			delete old_att_cont;
			
			if( m_Parent )
				m_Parent.Refresh();
			Inventory.GetInstance().UpdateConsoleToolbar();
		}
		
		RecomputeContainers();
		RecomputeOpenedContainers();
	}
	
	override void UpdateInterval()
	{
		if ( m_Entity )
		{
			if (m_CargoGrid)
			{
				bool hideCargo = m_Entity.GetInventory().IsInventoryLockedForLockType( HIDE_INV_FROM_SCRIPT ) || !m_Entity.CanDisplayCargo() || m_ForcedHide;
				if (m_CargoGrid.IsVisible() && hideCargo)
				{
					HideCargo();
				}
				else if (!m_CargoGrid.IsVisible() && !hideCargo)
				{
					ShowCargo();
				}
				
				m_CargoGrid.UpdateInterval();
			}
			
			if ( m_SlotIcon )
			{
				bool hide = m_LockCargo || ItemManager.GetInstance().GetDraggedItem() == m_Entity;
				if (!hide)
				{
					SetOpenForSlotIcon(IsOpened());
				}
				m_SlotIcon.GetRadialIconPanel().Show( !hide );
				
			}
			
			super.UpdateInterval();
		}
	}
	
	override EntityAI GetFocusedContainerEntity()
	{
		return m_Entity;
	}
	
	override void UnfocusAll()
	{
		if( m_Atts )
		{
			m_Atts.UnfocusAll();
		}
			
		if( m_CargoGrid )
		{
			m_CargoGrid.UnfocusAll();
		}
		
		foreach( EntityAI e1, CargoContainer cargo : m_AttachmentCargos )
		{
			cargo.UnfocusAll();
				
		}
		
		foreach( EntityAI e2, Attachments att : m_AttachmentAttachments )
		{
			att.UnfocusAll();
		}
	}
	
	override bool IsLastIndex()
	{
		return m_ActiveIndex == ( m_OpenedContainers.Count() - 1 );
	}
	
	override bool IsFirstContainerFocused()
	{
		return IsFirstIndex();
	}
	
	override bool IsLastContainerFocused()
	{
		return IsLastIndex();
	}

	override void MoveGridCursor( int direction )
	{
		Container c = GetFocusedContainer();
		if ( c )
		{
			c.MoveGridCursor( direction );
			Inventory.GetInstance().UpdateConsoleToolbar();
		}
	}
	
	void SetEntity( EntityAI entity, bool immedUpdate = true )
	{
		m_Entity = entity;
		
		m_Atts = new Attachments( this, m_Entity );
		m_Atts.InitAttachmentGrid( SORT_ATTACHMENTS_OWN );
		m_AttachmentSlotsSorted = m_Atts.GetSlotsSorted();
		
		m_Entity.GetOnItemAttached().Insert( AttachmentAdded );
		m_Entity.GetOnItemDetached().Insert( AttachmentRemoved );
		
		m_ClosableHeader.SetItemPreview( m_Entity );
		CheckHeaderDragability();
		
		if ( m_Entity.GetInventory().GetCargo() )
		{
			m_CargoGrid = new CargoContainer( this, false );
			m_CargoGrid.GetRootWidget().SetSort( SORT_CARGO_OWN );
			m_CargoGrid.SetEntity( m_Entity, 0, immedUpdate );
			m_CargoGrid.UpdateHeaderText(); // TODO: refresh?
			Insert( m_CargoGrid );
		}
		else
		{
			string name = m_Entity.GetDisplayName();
			name.ToUpper();
			m_ClosableHeader.SetName( name );
		}

		m_AttachmentCargos					= new map<EntityAI, ref CargoContainer>;
		m_AttachmentAttachmentsContainers	= new map<EntityAI, ref AttachmentsWrapper>;
		m_AttachmentAttachments				= new map<EntityAI, ref Attachments>;
		
		(Container.Cast( m_Parent )).Insert( this );
		
		foreach ( int slot_id : m_AttachmentSlotsSorted )
		{
			EntityAI item = m_Entity.GetInventory().FindAttachment( slot_id );
			if ( item )
				AttachmentAddedEx( item, InventorySlots.GetSlotName( slot_id ), m_Entity, false );
		}
		
		RecomputeContainers();
		
		if (m_CargoGrid)
		{
			bool hideCargo = m_Entity.GetInventory().IsInventoryLockedForLockType( HIDE_INV_FROM_SCRIPT ) || !m_Entity.CanDisplayCargo() || m_ForcedHide;
			if (m_CargoGrid.IsVisible() && hideCargo)
			{
				HideCargo();
			}
			else if (!m_CargoGrid.IsVisible() && !hideCargo)
			{
				ShowCargo();
			}
		}
		
		if( IsDisplayable() )
			SetOpenState( true );
		else
			SetOpenState( false );
		
		if (immedUpdate)
			m_Parent.m_Parent.Refresh();
		
		RecomputeOpenedContainers();
	}
	
	void HideCargo()
	{
		if( m_CargoGrid )
		{
			if(m_CargoGrid.IsVisible())
			{
				m_CargoGrid.OnHide();
				RecomputeOpenedContainers();
			}
		}
	}
	
	void ShowCargo()
	{
		if( m_CargoGrid )
		{
			if(!m_CargoGrid.IsVisible())
			{
				m_CargoGrid.OnShow();
				RecomputeOpenedContainers();
			}
		}
	}

	EntityAI GetEntity()
	{
		return m_Entity;
	}

	ItemPreviewWidget GetItemPreviewWidgetDragOrDrop( Widget w )
	{
		string name = w.GetName();
		name.Replace( "PanelWidget", "Render" );
		return ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
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
			return NULL;
		}
		return ipw.GetItem();
	}

	Widget GetItemPreviewWidget( Widget w )
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
		return ipw;
	}

	void MouseClick2( Widget w, int x, int y, int button )
	{
		SlotsIcon slots_icon;
		w.GetUserData(slots_icon);
		
		EntityAI item = null;
		if(slots_icon)
			item = slots_icon.GetEntity();
		
		ItemBase itemAtPos = ItemBase.Cast( item );
		
		if( item )
		{
			bool reserved = false;
		
			reserved = slots_icon.IsReserved();
		
			if ( button == MouseState.RIGHT )
			{
				#ifdef DEVELOPER
				if( GetDayZGame().IsLeftCtrlDown() )
					ShowActionMenu( InventoryItem.Cast(item) );
				#endif
				if( reserved )
				{
					EntityAI att_parent = slots_icon.GetSlotParent();
					GetGame().GetPlayer().GetHumanInventory().ClearUserReservedLocation( item );
					att_parent.GetOnAttachmentReleaseLock().Invoke(item, slots_icon.GetSlotID());
				}
			}
			else if ( button == MouseState.MIDDLE )
			{
				if( !reserved )
				{
					InspectItem( itemAtPos );
				}
			} 
			else if ( button == MouseState.LEFT )
			{
				if( !reserved )
				{
					PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
					if ( g_Game.IsLeftCtrlDown() )
					{
						if( itemAtPos && itemAtPos.GetInventory().CanRemoveEntity() )
						{
							if( itemAtPos.GetTargetQuantityMax() < itemAtPos.GetQuantity() )
								itemAtPos.SplitIntoStackMaxClient( player, -1 );
							else
								player.PredictiveTakeEntityToInventory( FindInventoryLocationType.ANY, itemAtPos );
						}
					}
					else
					{
						bool draggable = !player.GetInventory().HasInventoryReservation( item, null ) && !player.GetInventory().IsInventoryLocked() && item.GetInventory().CanRemoveEntity() && !player.IsItemsToDelete();
						ItemManager.GetInstance().SetWidgetDraggable( w, draggable );
					}
				}
			}
		}
	}
	
	/*void DraggingOverHeader2(Widget w, int x, int y, Widget receiver )
	{
		DraggingOverHeader(w, x, y, receiver );
	}*/

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
		
		if( cargo == m_CargoGrid )
		{
			target_entity	= m_Entity;
			target_cargo 	= m_Entity.GetInventory().GetCargo();
			#ifdef PLATFORM_CONSOLE
			if( m_CargoGrid.HasItem( item ) )
			{
				return;
			}
			#endif
		}
		else
		{
			target_entity	= m_AttachmentCargos.GetKeyByValue( cargo );
			if( target_entity )
			{
				target_cargo 	= target_entity.GetInventory().GetCargo();
				#ifdef PLATFORM_CONSOLE
				if( cargo.HasItem( item ) )
				{
					return;
				}
				#endif
			}
			else
				return;
		}
		
		if( target_cargo && target_entity )
		{
			c_x = target_cargo.GetHeight();
			c_y = target_cargo.GetWidth();
		}
		else
			return;
		
		InventoryLocation dst = new InventoryLocation;
		#ifdef PLATFORM_CONSOLE
		x = 0;
		y = target_cargo.GetItemCount();
		target_entity.GetInventory().FindFreeLocationFor( item, FindInventoryLocationType.CARGO, dst );
		#else
		dst.SetCargoAuto(target_cargo, item, x, y, item.GetInventory().GetFlipCargo());
		#endif
		
		InventoryLocation src = new InventoryLocation;
		item.GetInventory().GetCurrentInventoryLocation(src);
		if(src.CompareLocationOnly(dst) && src.GetFlip() == dst.GetFlip())
			return;
		
		#ifdef PLATFORM_CONSOLE
		if(dst.IsValid() && target_entity.GetInventory().LocationCanAddEntity(dst))
		#else
		if( c_x > x && c_y > y && target_entity.GetInventory().LocationCanAddEntity(dst))
		#endif
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );

			SplitItemUtils.TakeOrSplitToInventoryLocation( player, dst );
			
			Icon icon = cargo.GetIcon( item );
			
			if( icon )
			{
				if( w && w.FindAnyWidget("Selected") )
					w.FindAnyWidget("Selected").SetColor( ColorManager.BASE_COLOR );
				icon.Refresh();
				Refresh();
			}
		}
		
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
	}
	
	void TakeAsAttachment( Widget w, Widget receiver )
	{
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
		SlotsIcon slots_icon;
		EntityAI receiver_item;
		int slot_id = -1;
		bool is_reserved = false;
		EntityAI attached_entity;
		receiver.GetUserData(slots_icon);
		//string name = receiver.GetName();
		//name.Replace("PanelWidget", "Render");
		
		//ItemPreviewWidget receiver_iw = ItemPreviewWidget.Cast( receiver.FindAnyWidget(name) );
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
		InventoryLocation il = new InventoryLocation;
		float stackable;
		
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
		else if( attached_entity && attached_entity.GetInventory().CanAddAttachmentEx( item, slot_id ) )
		{
			stackable = item_base.GetTargetQuantityMax(slot_id);
			if( stackable == 0 || stackable >= item_base.GetQuantity() )
			{
				player.PredictiveTakeEntityToTargetAttachmentEx(attached_entity, item, slot_id);
			}
			else if( stackable != 0 && stackable < item_base.GetQuantity() )
			{
				item_base.SplitIntoStackMaxClient( attached_entity, slot_id );
			}
		}
		else if(attached_entity && attached_entity.GetInventory().CanAddAttachment(item))
		{
			attached_entity.GetInventory().FindFreeLocationFor(item,FindInventoryLocationType.ATTACHMENT,il);
			stackable = item_base.GetTargetQuantityMax(il.GetSlot());
			if( stackable == 0 || stackable >= item_base.GetQuantity() )
			{
				player.PredictiveTakeEntityToTargetAttachmentEx(attached_entity, item, il.GetSlot());
			}
			else if( stackable != 0 && stackable < item_base.GetQuantity() )
			{
				item_base.SplitIntoStackMaxClient( attached_entity, il.GetSlot() );
			}
		}
		else if( m_Entity.GetInventory().CanAddAttachment(item) )
		{
			m_Entity.GetInventory().FindFreeLocationFor(item,FindInventoryLocationType.ATTACHMENT,il);
			stackable = item_base.GetTargetQuantityMax(il.GetSlot());
			if( stackable == 0 || stackable >= item_base.GetQuantity() )
			{
				player.PredictiveTakeEntityToTargetAttachmentEx(m_Entity, item, il.GetSlot());
			}
			else if( stackable != 0 && stackable < item_base.GetQuantity() )
			{
				item_base.SplitIntoStackMaxClient( m_Entity, il.GetSlot() );
			}
		}
		else if( m_Entity.GetInventory().CanAddEntityInCargo( item, item.GetInventory().GetFlipCargo() ) && !m_Entity.GetInventory().HasEntityInCargo( item ) )
		{
			SplitItemUtils.TakeOrSplitToInventory( PlayerBase.Cast( GetGame().GetPlayer() ), m_Entity, item );
		}
		/*else if( !player.GetInventory().HasEntityInInventory( item ) || !m_Entity.GetInventory().HasEntityInCargo( item ) )
		{
			SplitItemUtils.TakeOrSplitToInventory( PlayerBase.Cast( GetGame().GetPlayer() ), m_Entity, item );
		}*/
	}

	override void OnDropReceivedFromHeader( Widget w, int x, int y, Widget receiver )
	{
		TakeAsAttachment( w, receiver );
	}

	void OnDropReceivedFromHeader2( Widget w, int x, int y, Widget receiver )
	{
		TakeAsAttachment( w, receiver );
	}

	void DoubleClick(Widget w, int x, int y, int button)
	{
		if( button == MouseState.LEFT )
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			if( w == NULL || player.GetInventory().IsInventoryLocked() )
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
			
			EntityAI item = iw.GetItem();
			
			if( !item )
			{
				return;
			}
			
			SlotsIcon icon;
			iw.GetUserData(icon);
		
			if(icon && icon.IsReserved())
			{
				return;
			}
			
			if( !item.GetInventory().CanRemoveEntity() || !player.CanManipulateInventory() )
				return;
			
			if( player.GetInventory().HasEntityInInventory( item ) && player.GetHumanInventory().CanAddEntityInHands( item ) )
			{
				player.PredictiveTakeEntityToHands( item );
			}
			else
			{
				if(player.GetInventory().CanAddEntityToInventory( item ) && item.GetInventory().CanRemoveEntity())
				{
					player.PredictiveTakeEntityToInventory( FindInventoryLocationType.ANY, InventoryItem.Cast( item ) );
				}
				else
				{
					if( player.GetHumanInventory().CanAddEntityInHands( item ) )
					{
						player.PredictiveTakeEntityToHands( item );
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
		if( w == NULL )
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
		
		if( cargo == m_CargoGrid )
		{
			target_entity	= m_Entity;
			target_cargo 	= m_Entity.GetInventory().GetCargo();
		}
		else
		{
			target_entity	= m_AttachmentCargos.GetKeyByValue( cargo );
			if( target_entity )
			{
				target_cargo 	= target_entity.GetInventory().GetCargo();
			}
			else
				return false;
		}
		
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
			if( target_entity.GetHierarchyRootPlayer() == GetGame().GetPlayer() )
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

	override void DraggingOver( Widget w, int x, int y, Widget receiver )
	{
		DraggingOverHeader( w, x, y, receiver );
	}

	override void DraggingOverHeader( Widget w, int x, int y, Widget receiver )
	{
		if( w == NULL )
		{
			return;
		}
		EntityAI item = GetItemPreviewItem( w );
		if( !item )
		{
			return;
		}
		
		SlotsIcon slots_icon;
		receiver.GetUserData(slots_icon);
	
		EntityAI attached_entity;
		EntityAI receiver_item;
		bool is_reserved = false;
		int slot_id = -1;
			
		if(slots_icon)
		{
			attached_entity	= slots_icon.GetSlotParent();
			slot_id			= slots_icon.GetSlotID();
			receiver_item 	= slots_icon.GetEntity();
			is_reserved 	= slots_icon.IsReserved();
		}


		Weapon_Base wpn;
		Magazine mag;
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if( m_Entity )
		{
			if( Class.CastTo(wpn,  m_Entity ) && Class.CastTo(mag,  item ) )
			{
				if( player.GetWeaponManager().CanAttachMagazine( wpn, mag ) )
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
					ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
				}
			}
			else if( receiver_item && !is_reserved )
			{
				ItemBase receiver_itemIB	= ItemBase.Cast( receiver_item );
				ItemBase itemIB				= ItemBase.Cast( item );
				if( receiver_itemIB && itemIB && receiver_itemIB.CanBeCombined( itemIB ) )
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
					ColorManager.GetInstance().SetColor( w, ColorManager.COMBINE_COLOR );
				}
				else if( GameInventory.CanSwapEntitiesEx( receiver_item, item ) )
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
					ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
				}
				else if( receiver_itemIB.GetInventory().CanAddAttachment( item ) )
				{
					ItemManager.GetInstance().HideDropzones();
					if( receiver_itemIB.GetHierarchyRootPlayer() == GetGame().GetPlayer() )
					{
						ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
					}
					else
					{
						ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
					}
					ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
				}
			}
			else if( attached_entity && attached_entity.GetInventory().CanAddAttachmentEx( item, slot_id ) )
			{
				ItemManager.GetInstance().HideDropzones();
				if( attached_entity.GetHierarchyRootPlayer() == GetGame().GetPlayer() )
				{
					ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
				}
				else
				{
					ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
				}
				ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
			}
			else if( m_Entity.GetInventory().CanAddAttachment( item ) )
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
				ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
			}
			else if( ( m_Entity.GetInventory().CanAddEntityInCargo( item, item.GetInventory().GetFlipCargo() ) && !m_Entity.GetInventory().HasEntityInCargo( item ) ) /*|| player.GetHumanInventory().HasEntityInHands( item )*/ )
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
				ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
			}
			else
			{
				ItemManager.GetInstance().ShowSourceDropzone( item );
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
			}
		}
	}
}
