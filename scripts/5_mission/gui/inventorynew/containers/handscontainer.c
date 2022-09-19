class HandsContainer: Container
{
	protected bool											m_Hidden;
	protected ref HandsHeader								m_CollapsibleHeader;
	protected ref HandsPreview								m_HandsPreview;
	
	protected ref Attachments								m_Atts;
	protected ref CargoContainer							m_CargoGrid;
	
	protected ref map<EntityAI, ref CargoContainer>			m_AttachmentCargos;
	protected ref map<EntityAI, AttachmentsWrapper>			m_AttachmentAttachmentsContainers;
	protected ref map<EntityAI, ref Attachments>			m_AttachmentAttachments;
	protected ref array<int>								m_AttachmentSlotsSorted;
	
	protected int											m_StaticAttCount = 0;
	protected int											m_StaticCargoCount = 0;
	
	protected ScrollWidget									m_ScrollWidget;

	void HandsContainer( LayoutHolder parent )
	{
		m_AttachmentCargos					= new map<EntityAI, ref CargoContainer>;
		m_AttachmentAttachmentsContainers	= new map<EntityAI, AttachmentsWrapper>;
		m_AttachmentAttachments				= new map<EntityAI, ref Attachments>;
		
		m_CollapsibleHeader = new HandsHeader( this, "CollapseButtonOnMouseButtonDown" );
		GetMainWidget().SetFlags( WidgetFlags.IGNOREPOINTER );
		m_MainWidget = m_MainWidget.FindWidget( "body" );
		GetMainWidget().SetFlags( WidgetFlags.IGNOREPOINTER );
		m_Body = new array<ref LayoutHolder>;
		m_HandsPreview = new HandsPreview( this );
		ItemManager.GetInstance().SetHandsPreview( m_HandsPreview );
		m_Body.Insert( m_HandsPreview );
		
		ItemBase hands_item = ItemBase.Cast( GetGame().GetPlayer().GetHumanInventory().GetEntityInHands() );
		if ( hands_item )
			m_HandsPreview.CreateNewIcon( hands_item );
		
		m_ScrollWidget						= ScrollWidget.Cast( m_RootWidget.GetParent().GetParent() );
		RecomputeOpenedContainers();
	}
	
	void ~HandsContainer()
	{
		if ( m_Entity )
		{
			m_Entity.GetOnItemAttached().Remove( AttachmentAdded );
			m_Entity.GetOnItemDetached().Remove( AttachmentRemoved );
		}
		
		m_AttachmentCargos.Clear();
		m_AttachmentAttachmentsContainers.Clear();
		m_AttachmentAttachments.Clear();
	}
	
	void ResetContainer()
	{
		m_Body.Clear();
		m_Body.Insert( m_HandsPreview );
	}
	
	override Header GetHeader()
	{
		return m_CollapsibleHeader;
	}
	
	override bool IsLastIndex()
	{
		return m_ActiveIndex == m_OpenedContainers.Count() - 1;
	}
	
	void SetFocusToIndex()
	{
		int index = m_ActiveIndex - 1;
		int attachment_start_index = -1;
		int cargo_start_index = -1;
		int attachment_end_index = -1;
		int cargo_end_index = -1;
		
		if ( m_Atts || m_AttachmentAttachments.Count() > 0 )
		{
			attachment_start_index = 0;
			if ( m_Atts )
				attachment_end_index++;
			attachment_end_index += m_AttachmentAttachments.Count();
		}
		
		if ( m_CargoGrid || m_AttachmentCargos.Count() > 0 )
		{
			cargo_start_index = attachment_end_index + 1;
			if ( m_CargoGrid )
				cargo_end_index++;
			cargo_end_index += cargo_start_index + m_AttachmentCargos.Count();
		}
		
		if ( m_ActiveIndex > m_OpenedContainers.Count() )
		{
			m_ActiveIndex = m_OpenedContainers.Count();
		}
		
		if ( index == -1 )
		{
			#ifdef PLATFORM_CONSOLE
			if ( m_MainWidget.FindAnyWidget("Selected") )
				m_MainWidget.FindAnyWidget("Selected").Show( true );
			#endif
			m_ScrollWidget.VScrollToPos01( 0 );
		}
		else
		{
			if( index.InRange( 0, attachment_end_index ) )
			{
				if( m_Atts )
				{
					if( index == 0 )
					{
						m_Atts.SetDefaultFocus();
						SetFocusedContainer( m_Atts.GetWrapper() );
					}
					else
					{
						m_AttachmentAttachments.GetElement( index - 1 ).SetDefaultFocus();
						SetFocusedContainer( m_AttachmentAttachments.GetElement( index - 1 ).GetWrapper() );
					}
				}
				else
				{
					m_AttachmentAttachments.GetElement( index ).SetDefaultFocus();
					SetFocusedContainer( m_AttachmentAttachments.GetElement( index ).GetWrapper() );
				}
			}
			else if( index.InRange( cargo_start_index, cargo_end_index ) )
			{
				if( m_CargoGrid )
				{
					if( index == cargo_start_index )
					{
						m_CargoGrid.SetDefaultFocus();
						SetFocusedContainer( m_CargoGrid );
					}
					else
					{
						m_AttachmentCargos.GetElement( index - 1 - cargo_start_index ).SetDefaultFocus();
						SetFocusedContainer( m_AttachmentCargos.GetElement( index - 1 - cargo_start_index ) );
					}
				}
				else
				{
					m_AttachmentCargos.GetElement( index - cargo_start_index ).SetDefaultFocus();
					SetFocusedContainer( m_AttachmentCargos.GetElement( index - cargo_start_index ) );
				}
			}
			
			if( m_MainWidget.FindAnyWidget("Selected") )
				m_MainWidget.FindAnyWidget("Selected").Show( false );
			ScrollToActiveContainer( GetFocusedContainer() );
		}
	}
	
	void ScrollToActiveContainer( Container active_container )
	{
		if( !active_container )
			return;
		float x, y, y_s;
		m_ScrollWidget.GetScreenPos( x, y );
		m_ScrollWidget.GetScreenSize( x, y_s );
		float amount;
		float next_pos	= active_container.GetFocusedContainerYScreenPos( true ) + active_container.GetFocusedContainerHeight( true );
			
		if( next_pos > ( y + y_s ) )
		{
			amount	= y + active_container.GetFocusedContainerYScreenPos( true );
			m_ScrollWidget.VScrollToPos( m_ScrollWidget.GetVScrollPos() + active_container.GetFocusedContainerHeight( true ) + 2 );
		}
		else if( active_container.GetFocusedContainerYScreenPos( true ) < y )
		{
			amount = active_container.GetFocusedContainerYScreenPos( true ) - y;
			m_ScrollWidget.VScrollToPos( m_ScrollWidget.GetVScrollPos() + amount - 2 );
		}
	}
	
	override void Insert( LayoutHolder container, int pos = -1, bool immedUpdate = true )
	{
		super.Insert( container, pos, immedUpdate );
	}

	void MouseClick2( Widget w, int x, int y, int button )
	{
		string name = w.GetName();
		name.Replace( "PanelWidget", "Render" );
		ItemPreviewWidget item_preview = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		
		if( !item_preview )
		{
			return;
		}
		
		EntityAI item = item_preview.GetItem();
		InventoryItem itemAtPos = InventoryItem.Cast( item );
		
		if( item )
		{
			if ( button == MouseState.MIDDLE )
			{
				InspectItem( itemAtPos );
			}
		}
	}
		
	void DraggingOverHeader2(Widget w, int x, int y, Widget receiver )
	{
		DraggingOverHeader(w, x, y, receiver );
	}
	
	override bool TransferItem()
	{
		if(m_ActiveIndex == 0)
		{
			EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
			if( item_in_hands && GetGame().GetPlayer().GetHumanInventory().CanRemoveEntityInHands() && !GetGame().GetPlayer().GetInventory().HasInventoryReservation(item_in_hands, null)  )
			{
				if( GetGame().GetPlayer().PredictiveTakeEntityToInventory( FindInventoryLocationType.CARGO, item_in_hands ) )
				{
					m_MainWidget.FindAnyWidget("Selected").Show( false );
					m_MainWidget.FindAnyWidget("hands_preview_root").SetAlpha( 0.7 );
					return true;
				}
			}
		}
		else if( GetFocusedContainer() )
		{
			return GetFocusedContainer().TransferItem();
		}
		return false;
	}
	
	override bool CanEquip()
	{
		if (m_ActiveIndex == 0)
		{
			EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
			if ( item_in_hands.IsInherited( Magazine ) || GetGame().GetPlayer().GetInventory().HasInventoryReservation(item_in_hands, null)  )
			{
				return false;
			}
			InventoryLocation il = new InventoryLocation;
			bool found = GetGame().GetPlayer().GetInventory().FindFreeLocationFor(item_in_hands,FindInventoryLocationType.ATTACHMENT,il);
			if (found)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if( GetFocusedContainer() )
		{
			return GetFocusedContainer().CanEquip();
		}
		return false;
	}
	
	override bool SplitItem()
	{
		if (m_ActiveIndex == 0)
		{
			ItemBase item_in_hands = ItemBase.Cast( GetGame().GetPlayer().GetHumanInventory().GetEntityInHands() );
			if ( item_in_hands )
			{
				if ( item_in_hands.CanBeSplit() )
				{
					item_in_hands.OnRightClick();
				}
			}
		}
		else if( GetFocusedContainer() )
		{
			return GetFocusedContainer().SplitItem();
		}
		return false;
	}
	
	override bool EquipItem()
	{
		if (m_ActiveIndex == 0)
		{
			ItemBase item_in_hands = ItemBase.Cast( GetGame().GetPlayer().GetHumanInventory().GetEntityInHands() );
			if ( item_in_hands )
			{
				if ( GetGame().GetPlayer().GetHumanInventory().CanRemoveEntityInHands() && !GetGame().GetPlayer().GetInventory().HasInventoryReservation(item_in_hands, null)  )
				{
					if ( GetGame().GetPlayer().PredictiveTakeEntityToInventory( FindInventoryLocationType.ATTACHMENT, item_in_hands ) )
					{
						m_MainWidget.FindAnyWidget("Selected").Show( false );
						m_MainWidget.FindAnyWidget("hands_preview_root").SetAlpha( 0.7 );
						return true;
					}
				}
			}
		}
		else if( GetFocusedContainer() )
		{
			return GetFocusedContainer().EquipItem();
		}
		return false;
	}
	
	override void SetActive( bool active )
	{
		super.SetActive(active);
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();

		if( item_in_hands )
		{
			if( m_MainWidget.FindAnyWidget( "Selected" ) )
			{
				m_MainWidget.FindAnyWidget( "Selected" ).Show( active );
			}
			
			if( active )
			{
				float x, y;
				GetMainWidget().GetScreenPos( x, y );
				PrepareOwnedTooltip( item_in_hands, -1, y );
			}
		}
		else
		{
			if( active )
				m_MainWidget.FindAnyWidget( "hands_preview_root" ).SetAlpha( 0.85 );
			else
				m_MainWidget.FindAnyWidget( "hands_preview_root" ).SetAlpha( 0.7 );
		}
	}
	
	override void SetLastActive()
	{
		super.SetLastActive();
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();

		if( item_in_hands )
		{
			if ( m_ActiveIndex == 0)
			{
				if( m_MainWidget.FindAnyWidget( "Selected" ) )
				{
					m_MainWidget.FindAnyWidget( "Selected" ).Show( true );
				}
			
				float x, y;
				GetMainWidget().GetScreenPos( x, y );
				PrepareOwnedTooltip( item_in_hands, x, y );
			}
		}
		else
		{
			m_MainWidget.FindAnyWidget( "hands_preview_root" ).SetAlpha( 0.85 );
		}
	}
	
	override void Refresh()
	{
		super.Refresh();
		SetActive( m_IsActive );
	}
	
	override bool IsItemActive()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		ItemBase item = ItemBase.Cast( player.GetHumanInventory().GetEntityInHands() );
		if( item == null )
		{
			return false;
		}
		return !IsEmpty() && ( !QuantityConversions.HasItemQuantity( item )  || ( QuantityConversions.HasItemQuantity( item ) && !item.CanBeSplit() ) );
	}
	
	override bool IsItemWithQuantityActive()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		ItemBase item = ItemBase.Cast( player.GetHumanInventory().GetEntityInHands() );
		if( item == null )
		{
			return false;
		}
		return !IsEmpty() && QuantityConversions.HasItemQuantity( item ) && item.CanBeSplit();
	}
	
	override bool IsEmpty()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		return player.GetHumanInventory().GetEntityInHands() == null;
	}
	
	override bool TransferItemToVicinity()
	{
		if (m_ActiveIndex == 0)
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			ItemBase item_in_hands = ItemBase.Cast(player.GetHumanInventory().GetEntityInHands());
			if ( item_in_hands && player.CanDropEntity( item_in_hands ) && GetGame().GetPlayer().GetHumanInventory().CanRemoveEntityInHands() && !player.GetInventory().HasInventoryReservation(item_in_hands, null) )
			{
				if ( item_in_hands.GetTargetQuantityMax() < item_in_hands.GetQuantity() )
					item_in_hands.SplitIntoStackMaxClient( null, -1 );
				else
					player.PhysicalPredictiveDropItem( item_in_hands );
				m_MainWidget.FindAnyWidget("Selected").Show( false );
				m_MainWidget.FindAnyWidget("hands_preview_root").SetAlpha( 0.7 );
				return true;
			}
		}
		else if ( GetFocusedContainer() )
		{
				return GetFocusedContainer().TransferItemToVicinity();
		}
		return false;
	}
	
	override bool InspectItem()
	{
		if(m_ActiveIndex == 0)
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();
			if( item_in_hands )
			{
				InspectItem( item_in_hands );
				return true;
			}
		}
		else if( GetFocusedContainer() )
		{
			return GetFocusedContainer().InspectItem();
		}
		return false;
	}
	
	override bool OnSelectButton()
	{
		if(m_ActiveIndex == 0)
		{
			if( ItemManager.GetInstance().IsMicromanagmentMode() )
			{
				EntityAI selectedItem = ItemManager.GetInstance().GetSelectedItem();
				if( selectedItem == GetFocusedItem() )
				{
					Inventory.GetInstance().DisableMicromanagement();
					return true;
				}
				else
				{
					return Select();
				}
			}
			else
			{
				Inventory.GetInstance().EnableMicromanagement();
				return true;
			}
		}
		else
		{
			if( GetFocusedContainer() )
			{
				return GetFocusedContainer().OnSelectButton();
			}
		}
		return false;
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
		
		if( !ipw )
		{
			return null;
		}
		
		return ipw.GetItem();
	}

	override void DraggingOverHeader( Widget w, int x, int y, Widget receiver )
	{
		if( w == null )
		{
			return;
		}
		EntityAI receiver_item;
		EntityAI slot_owner;
		int slot_id = -1;
		
		string name = w.GetName();
		name.Replace( "PanelWidget", "Render" );

		ItemPreviewWidget w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		if( w_ipw == null )
		{
			w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		}
		if( w_ipw == null )
		{
			return;
		}
		
		SlotsIcon slots_icon;
		receiver.GetUserData(slots_icon);
		
		if(slots_icon)
		{
			receiver_item	= slots_icon.GetEntity();
			slot_id			= slots_icon.GetSlotID();
			slot_owner		= slots_icon.GetSlotParent();
		}
		

		PlayerBase p = PlayerBase.Cast( GetGame().GetPlayer() );
		InventoryItem receiver_entity = InventoryItem.Cast( p.GetHumanInventory().GetEntityInHands() );
		InventoryItem w_entity = InventoryItem.Cast( w_ipw.GetItem() );
		if( !w_entity )
		{
			return;
		}

		if( receiver_entity )
		{
			if( receiver_item )
			{
				ItemBase receiver_itemIB	= ItemBase.Cast( receiver_item );
				ItemBase itemIB				= ItemBase.Cast( receiver_item );
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
				else 
				{
					PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
					Magazine mag = Magazine.Cast(receiver_item);
					Weapon_Base wpn = Weapon_Base.Cast(receiver_item.GetHierarchyParent());
				
					if( wpn && mag )
					{
						if( player.GetWeaponManager().CanSwapMagazine( wpn,  Magazine.Cast(w_entity) ) )
						{
							ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
							if( w_entity.GetHierarchyRootPlayer() == player )
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
							ItemManager.GetInstance().ShowSourceDropzone( w_entity );
						}
					}
					else
					{
						if( GameInventory.CanSwapEntitiesEx( receiver_item, w_entity ) )
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
						else if( receiver_itemIB.GetInventory().CanAddAttachment( w_entity ) )
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
				}
			}
			else if( slot_owner && slot_owner.GetInventory().CanAddAttachment( w_entity ) )
			{
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
				ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
			}
			else if( receiver_entity.GetInventory().CanAddAttachment( w_entity ) )
			{
				ItemManager.GetInstance().HideDropzones();
				if( receiver_entity.GetHierarchyRootPlayer() == GetGame().GetPlayer() )
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
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
				ItemManager.GetInstance().ShowSourceDropzone( w_entity );
			}
		}
		else
		{
			/*if( m_HandsIcon )
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
			}
			
			else
			{*/
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
				ItemManager.GetInstance().ShowSourceDropzone( w_entity );
			//}
		}
	}

	void OnDropReceivedFromHeader2( Widget w, int x, int y, Widget receiver )
	{
		TakeAsAttachment( w, receiver );
	}
	
	void OnPerformCombination(int combinationFlags)
	{
		PlayerBase m_player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( m_am_entity1 == null || m_am_entity2 == null ) return;

		if ( combinationFlags == InventoryCombinationFlags.NONE ) return;

		if ( combinationFlags & InventoryCombinationFlags.LOAD_CHAMBER )
		{
			Magazine mag;
			if ( Class.CastTo(mag,  m_am_entity2 ) )
			{
				m_player.GetWeaponManager().LoadBullet( mag );
				return;
			}
		}

		if ( combinationFlags & InventoryCombinationFlags.ADD_AS_ATTACHMENT )
		{
			if( m_Atts )
			{
				m_Body.RemoveItem( m_Atts.GetWrapper() );
				delete m_Atts;
			}
			if( m_am_entity2.GetSlotsCountCorrect() > 0 )
			{
				m_Atts = new Attachments( this, m_am_entity2 );
				m_Atts.InitAttachmentGrid( 1 );

			}

			Refresh();
			m_Parent.Refresh();

			Weapon_Base wpn1;
			Magazine mag1;
			if ( Class.CastTo(wpn1,  m_am_entity1 ) && Class.CastTo(mag1,  m_am_entity2 ) )
			{
				if( m_player.GetWeaponManager().CanAttachMagazine(wpn1, mag1) )
				{
					m_player.GetWeaponManager().AttachMagazine(mag1);
					return;
				}
			}
			
			SplitItemUtils.TakeOrSplitToInventory(m_player, m_am_entity1, m_am_entity2);
			//m_player.PredictiveTakeEntityToTargetAttachment( m_am_entity1, m_am_entity2 );
		}

		if ( combinationFlags & InventoryCombinationFlags.ADD_AS_CARGO )
		{
			m_player.PredictiveTakeEntityToTargetCargo( m_am_entity1, m_am_entity2 );
		}

		if ( combinationFlags & InventoryCombinationFlags.SWAP )
		{
			Magazine swapmag1;
			Magazine swapmag2;
			if (Class.CastTo(swapmag1,  m_am_entity1 ) && Class.CastTo(swapmag2,  m_am_entity2 ) )
			{
				if( m_player.GetWeaponManager().CanSwapMagazine( Weapon_Base.Cast( swapmag1.GetParent() ), Magazine.Cast( swapmag2 ) ) )
				{
					m_player.GetWeaponManager().SwapMagazine(swapmag2);
					return;
				}

				if( m_player.GetWeaponManager().CanSwapMagazine(Weapon_Base.Cast( swapmag2.GetParent() ), Magazine.Cast( swapmag1 )) )
				{
					m_player.GetWeaponManager().SwapMagazine(swapmag1);
					return;
				}
			}
			if ( !m_player.PredictiveSwapEntities( m_am_entity1, m_am_entity2 ) && m_player.GetHumanInventory().CanAddEntityInHands( m_am_entity1 ) )
			{
				m_player.PredictiveTakeEntityToHands( m_am_entity1 );
			}
		}

		if ( combinationFlags & InventoryCombinationFlags.TAKE_TO_HANDS )
		{
			if ( m_Atts )
			{
				m_Body.RemoveItem( m_Atts.GetWrapper() );
				delete m_Atts;
			}

			if ( m_am_entity2.GetSlotsCountCorrect() > 0 )
			{
				m_Atts = new Attachments( this, m_am_entity2 );
				m_Atts.InitAttachmentGrid( 1 );

			}
			Refresh();
			m_Parent.Refresh();
			
			if ( m_player.GetHumanInventory().CanAddEntityInHands( m_am_entity2 ) )
			{
				m_player.PredictiveTakeEntityToHands( m_am_entity2 );
			}
		}
	}

	void ShowActionMenuCombine( EntityAI entity1, EntityAI entity2, int combinationFlags )
	{
		if ( entity1 && entity2 ) PrintString( "Showing action menu for " + entity1.GetDisplayName() + " and " + entity2.GetDisplayName() );

		ContextMenu cmenu = GetGame().GetUIManager().GetMenu().GetContextMenu();
		m_am_entity1 = entity1;
		m_am_entity2 = entity2;
		cmenu.Hide();
		cmenu.Clear();

		if( combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY2 )
		{
			ItemBase entity = ItemBase.Cast( entity1 );
			entity.CombineItemsClient( ItemBase.Cast( entity2 ) );
			return;
		}

		if ( entity1 == null || entity2 == null ) return;

		if ( combinationFlags == InventoryCombinationFlags.NONE ) return;

		if ( combinationFlags & InventoryCombinationFlags.ADD_AS_ATTACHMENT )
		{
			cmenu.Add( "#inv_context_add_as_attachment", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.ADD_AS_ATTACHMENT ) );
		}
		if ( combinationFlags & InventoryCombinationFlags.LOAD_CHAMBER )
		{
			cmenu.Add( "#inv_context_load_chamber", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.LOAD_CHAMBER ) );
		}

		if ( combinationFlags & InventoryCombinationFlags.ADD_AS_CARGO )
		{
			cmenu.Add( "#inv_context_add_as_cargo", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.ADD_AS_CARGO ) );
		}

		if ( combinationFlags & InventoryCombinationFlags.SWAP )
		{
			cmenu.Add( "#inv_context_swap", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.SWAP ) );
		}

		if ( combinationFlags & InventoryCombinationFlags.TAKE_TO_HANDS )
		{
			cmenu.Add( "#inv_context_take_to_hands", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.TAKE_TO_HANDS ) );
		}

		if ( combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY )
		{
			cmenu.Add( "#inv_context_combine", this, "OnPerformCombination", new Param1<int>( InventoryCombinationFlags.COMBINE_QUANTITY ) );
		}

		int m_am_pos_x,  m_am_pos_y;
		GetMousePos( m_am_pos_x, m_am_pos_y );
		m_am_pos_x -= 5;
		m_am_pos_y -= 5;

		cmenu.Show( m_am_pos_x, m_am_pos_y );
	}


	override void OnDropReceivedFromHeader( Widget w, int x, int y, Widget receiver )
	{
		TakeAsAttachment( w, receiver );
	}

	int GetCombinationFlags( EntityAI entity1, EntityAI entity2 )
	{
		int flags = 0;
		PlayerBase m_player = PlayerBase.Cast( GetGame().GetPlayer() );
		EntityAI entity_in_hands = m_player.GetHumanInventory().GetEntityInHands();

		if ( !entity1 || !entity2 ) return flags;

		Magazine swapmag1;
		Magazine swapmag2;
		bool skipSwap = false;

		if( Class.CastTo(swapmag1, entity1) && Class.CastTo(swapmag2, entity2) )
		{
			Weapon_Base parentWpn;

			if(	Class.CastTo(parentWpn, swapmag1.GetHierarchyParent())	)
			{
				skipSwap = true;
				if( m_player.GetWeaponManager().CanSwapMagazine(parentWpn,swapmag2) )
					flags = flags | InventoryCombinationFlags.SWAP;
			}

			if(	Class.CastTo(parentWpn, swapmag2.GetHierarchyParent())	)
			{
				skipSwap = true;
				if( m_player.GetWeaponManager().CanSwapMagazine(parentWpn,swapmag1) )
					flags = flags | InventoryCombinationFlags.SWAP;

			}
		}

		if ( !skipSwap )
		{
			if ( entity1 == m_player.GetHumanInventory().GetEntityInHands() ) flags = flags | InventoryCombinationFlags.TAKE_TO_HANDS;
			else if ( GameInventory.CanSwapEntitiesEx( entity1, entity2 ) )
			{
				if ( !entity1.IsInherited( ZombieBase ) && !entity1.IsInherited( Car ) )
				{
					flags = flags | InventoryCombinationFlags.SWAP;
				}
			}
		}

		if ( entity1.IsInherited( ItemBase) && entity2.IsInherited( ItemBase ) )
		{
			ItemBase ent1 = ItemBase.Cast( entity1 );
			if ( ent1.CanBeCombined( ItemBase.Cast( entity2 ) ) ) flags = flags | InventoryCombinationFlags.COMBINE_QUANTITY2;
		}

		else if ( entity1.GetInventory().CanAddAttachment( entity2 ) )
		{
			if ( !entity1.IsInherited( ZombieBase ) && !entity1.IsInherited( Car ) && !entity2.IsInherited( ZombieBase ) && !entity2.IsInherited( Car ) )
			{
				flags = flags | InventoryCombinationFlags.ADD_AS_ATTACHMENT;
			}
		}
		if ( entity1.GetInventory().CanAddEntityInCargo( entity2, entity2.GetInventory().GetFlipCargo() ) ) flags = flags | InventoryCombinationFlags.ADD_AS_CARGO;

		if( entity1 == m_player.GetHumanInventory().GetEntityInHands() || entity2 == m_player.GetHumanInventory().GetEntityInHands() )
		{
			ActionManagerClient amc;
			Class.CastTo(amc, m_player.GetActionManager());
			if( entity1 == m_player.GetHumanInventory().GetEntityInHands() )
			{
				if( amc.CanSetActionFromInventory( ItemBase.Cast( entity1 ), ItemBase.Cast( entity2 ) ) )
				{
					flags = flags | InventoryCombinationFlags.SET_ACTION;
				}
			}
			else
			{
				if( amc.CanSetActionFromInventory( ItemBase.Cast( entity2 ), ItemBase.Cast( entity1 ) ) )
				{
					flags = flags | InventoryCombinationFlags.SET_ACTION;
				}
			}
		}

		if( GetRecipeCount( true, entity1, entity2 ) > 0 )
		{
			flags = flags | InventoryCombinationFlags.RECIPE_ANYWHERE;
		}
		return flags;
	}

	int GetRecipeCount( bool recipe_anywhere, EntityAI entity1, EntityAI entity2 )
	{
		PluginRecipesManager plugin_recipes_manager = PluginRecipesManager.Cast( GetPlugin( PluginRecipesManager ) );
		return plugin_recipes_manager.GetValidRecipes( ItemBase.Cast( entity1 ), ItemBase.Cast( entity2 ), null, PlayerBase.Cast( GetGame().GetPlayer() ) );
	}
	
	void RemoveItem()
	{
		m_Entity = null;
		m_CollapsibleHeader.ShowCollapseButtons( false, true );
	}
	
	void DestroyAtt()
	{
		if( m_Atts )
		{
			m_Body.RemoveItem( m_Atts.GetWrapper() );
			delete m_Atts;
			
			if( m_Entity )
			{
				m_Entity.GetOnItemAttached().Remove( AttachmentAdded );
				m_Entity.GetOnItemDetached().Remove( AttachmentRemoved );
			}
		}
		
		if( m_AttachmentAttachmentsContainers )
		{
			foreach( EntityAI ent, AttachmentsWrapper att : m_AttachmentAttachmentsContainers )
			{
				m_Body.RemoveItem( att );
				m_AttachmentAttachments.Remove( ent );
				delete att;
			}
		}
		
		m_AttachmentAttachmentsContainers.Clear();
		Refresh();
		m_Parent.Refresh();
		RecomputeOpenedContainers();
	}
	
	override void UnfocusGrid()
	{
		if( m_Atts )
		{
			m_Atts.UnfocusAll();
		}
			
		if( m_CargoGrid )
		{
			m_CargoGrid.UnfocusAll();
		}
		
		if( m_AttachmentCargos )
		{
			foreach( EntityAI e1, CargoContainer cargo : m_AttachmentCargos )
			{
				cargo.UnfocusAll();
					
			}
		}
		
		if( m_AttachmentCargos )
		{
			foreach( EntityAI e2, Attachments att : m_AttachmentAttachments )
			{
				att.UnfocusAll();
			}
		}
	}
	
	override void UnfocusAll()
	{
		UnfocusGrid();
	}
	
	void DestroyCargo()
	{
		if( m_CargoGrid )
		{
			m_Body.RemoveItem( m_CargoGrid );
			delete m_CargoGrid;
		}
		
		if( m_AttachmentCargos )
		{
			foreach( CargoContainer cargo : m_AttachmentCargos )
			{
				m_Body.RemoveItem( cargo );
				delete cargo;
			}
		}

		m_AttachmentCargos.Clear();
		Refresh();
		m_Parent.Refresh();
		RecomputeOpenedContainers();
	}
	
	void DropReceived( Widget w,  int x, int y, CargoContainer cargo )
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();
		
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
	
	bool DraggingOverGrid( Widget w,  int x, int y, Widget reciever, CargoContainer cargo )
	{
		EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
		if( w == null )
		{
			return false;
		}
		EntityAI item = GetItemPreviewItem( w );
		if( !item || !item_in_hands )
		{
			return false;
		}

		int color;
		int idx = 0;
		int c_x, c_y;
		
		if( item_in_hands.GetInventory().GetCargo() )
		{
			c_x = item_in_hands.GetInventory().GetCargo().GetHeight();
			c_y = item_in_hands.GetInventory().GetCargo().GetWidth();
		}
		
		if( c_x > x && c_y > y && item_in_hands.GetInventory().CanAddEntityInCargoEx( item, 0, x, y, item.GetInventory().GetFlipCargo() ) )
		{
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
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
			w.FindAnyWidget( name ).SetColor( color );
		}

		return true;
	}
	
	void TakeAsAttachment( Widget w, Widget receiver )
	{
		EntityAI receiver_item;
		EntityAI slot_owner;
		int slot_id = -1;

		SlotsIcon slots_icon; 
		receiver.GetUserData(slots_icon);
		if( slots_icon )
		{
			receiver_item	= slots_icon.GetEntity();
			slot_id			= slots_icon.GetSlotID();
			slot_owner		= slots_icon.GetSlotParent();
		}
		
		EntityAI item = GetItemPreviewItem( w );
		if( !item )
		{
			return;
		}
		
		ItemBase item_base	= ItemBase.Cast( item );
		float stack_max = item_base.GetTargetQuantityMax(slot_id);
		
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
			else if(player.GetWeaponManager().CanSwapMagazine( wpn, mag ))
			{
				player.GetWeaponManager().SwapMagazine( mag );
			}
		}
		else if( receiver_item )
		{
			if( receiver_item != item )
			{
				if( ( ItemBase.Cast( receiver_item ) ).CanBeCombined( ItemBase.Cast( item ) ) )
				{
					( ItemBase.Cast( receiver_item ) ).CombineItemsClient( ItemBase.Cast( item ) );
				}
				else if( GameInventory.CanSwapEntitiesEx( receiver_item, item ) )
				{
					if( !receiver_item.GetInventory().CanRemoveEntity() )
						return;
					GetGame().GetPlayer().PredictiveSwapEntities( item, receiver_item );
				}
			}
		}
		else if( slot_owner && slot_owner.GetInventory().CanAddAttachmentEx( item, slot_id ) )
		{
			if( stack_max == 0 || stack_max >= item_base.GetQuantity() )
			{
				player.PredictiveTakeEntityToTargetAttachmentEx(slot_owner, item, slot_id);
			}
			else if( stack_max != 0 && stack_max < item_base.GetQuantity() )
			{
				item_base.SplitIntoStackMaxClient( m_Entity, slot_id );
			}
		}
		else if( slot_owner && slot_owner.GetInventory().CanAddAttachment( item ) )
		{
			if( stack_max == 0 || stack_max >= item_base.GetQuantity() )
			{
				player.PredictiveTakeEntityToTargetAttachment(slot_owner, item);
			}
			else if( stack_max != 0 && stack_max < item_base.GetQuantity() )
			{
				item_base.SplitIntoStackMaxClient( m_Entity, -1 );
			}
		}
		else if( m_Entity.GetInventory().CanAddAttachment( item ) )
		{
			if( stack_max == 0 || stack_max >= item_base.GetQuantity() )
			{
				player.PredictiveTakeEntityToTargetAttachment(m_Entity, item);
			}
			else if( stack_max != 0 && stack_max < item_base.GetQuantity() )
			{
				item_base.SplitIntoStackMaxClient( m_Entity, -1 );
			}
		}
		else if( ( m_Entity.GetInventory().CanAddEntityInCargo( item, item.GetInventory().GetFlipCargo() ) && ( !player.GetInventory().HasEntityInInventory( item ) || !m_Entity.GetInventory().HasEntityInCargo( item )) ) || player.GetHumanInventory().HasEntityInHands( item ) )
		{
			SplitItemUtils.TakeOrSplitToInventory( PlayerBase.Cast( GetGame().GetPlayer() ), m_Entity, item );
		}
		/*else if( !player.GetInventory().HasEntityInInventory( item ) || !m_Entity.GetInventory().HasEntityInCargo( item ) )
		{
			SplitItemUtils.TakeOrSplitToInventory( PlayerBase.Cast( GetGame().GetPlayer() ), m_Entity, item );
		}*/
		
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
	}

	void ShowAtt( EntityAI entity )
	{
		ResetContainer();
		m_StaticCargoCount = 0;
		m_Entity = entity;
		m_ActiveIndex = 0;
		if( entity.GetSlotsCountCorrect() > 0 )
		{
			m_Atts = new Attachments( this, entity );
			m_Atts.InitAttachmentGrid( 1 );
			m_AttachmentSlotsSorted = m_Atts.GetSlotsSorted();
			m_StaticAttCount = m_Atts.GetAttachmentHeight();
			m_Entity.GetOnItemAttached().Insert( AttachmentAdded );
			m_Entity.GetOnItemDetached().Insert( AttachmentRemoved );
		}
		else
		{
			m_StaticAttCount = 0;
		}
		
		if( entity.GetInventory().GetCargo() )
		{
			m_CargoGrid = new CargoContainer( this, false );
			m_CargoGrid.SetEntity( entity );
			m_CargoGrid.GetRootWidget().SetSort( 1 );
			Insert( m_CargoGrid );
			m_StaticCargoCount++;
		}
		
		if (m_AttachmentSlotsSorted)
		{
			foreach( int slot_id : m_AttachmentSlotsSorted )
			{
				EntityAI item = m_Entity.GetInventory().FindAttachment( slot_id );
				if( item )
					AttachmentAdded( item, InventorySlots.GetSlotName( slot_id ), m_Entity );
			}
		}

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
		
		m_CollapsibleHeader.ShowCollapseButtons( m_Atts || m_CargoGrid, m_Atts || m_CargoGrid );
		RecomputeOpenedContainers();
		Refresh();
		m_Parent.Refresh();
	}
	
	void RefreshHands()
	{
		for (int i = 0; i < m_Body.Count(); i++)
		{
			m_Body.Get( i ).OnShow();
		}
		Refresh();
		m_Parent.Refresh();
	}
	
	void AttachmentAdded(EntityAI item, string slot, EntityAI parent)
	{
		int slot_id = InventorySlots.GetSlotIdFromString( slot );
		int sort = -1;
		Attachments att_cont = null;
		CargoContainer cont = null;
		
		if( item.GetInventory().GetAttachmentSlotsCount() > 0  )
		{
			att_cont = new Attachments( this, item );
			sort = (m_AttachmentSlotsSorted.Find( slot_id ) * 2) + SORT_ATTACHMENTS_NEXT_OFFSET;
			att_cont.InitAttachmentGrid( sort );
			
			m_AttachmentAttachments.Insert( item, att_cont );
			m_AttachmentAttachmentsContainers.Insert( item, att_cont.GetWrapper() );
			
			att_cont.UpdateInterval();
			if( m_Parent )
				m_Parent.Refresh();
			Inventory.GetInstance().UpdateConsoleToolbar();
		}
		
		if( item.GetInventory().GetCargo() )
		{
			cont = new CargoContainer( this, true );
			sort = (m_AttachmentSlotsSorted.Find( slot_id ) * 2) + SORT_CARGO_NEXT_OFFSET; //m_AttachmentSlotsSorted.Find( slot_id ) + 50
			cont.GetRootWidget().SetSort( sort );
			cont.SetEntity( item );
			
			Insert(cont, -1);
			
			m_AttachmentCargos.Insert( item, cont );
			if( m_Parent )
				m_Parent.Refresh();
			Inventory.GetInstance().UpdateConsoleToolbar();
		}
		
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
		
		RecomputeOpenedContainers();
	}
	
	void AttachmentRemoved(EntityAI item, string slot, EntityAI parent)
	{
		int slot_id = InventorySlots.GetSlotIdFromString( slot );
		
		CargoContainer old_cont = m_AttachmentCargos.Get( item );
		if( old_cont )
		{
			m_Body.RemoveItem( old_cont );
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
			m_Body.RemoveItem( old_att_cont );
			delete old_att_cont;
			if( m_Parent )
				m_Parent.Refresh();
			Inventory.GetInstance().UpdateConsoleToolbar();
		}
		RecomputeOpenedContainers();
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();
		
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
			
			if ( m_Atts )
			{
				m_Atts.UpdateInterval();
			}
			
			for ( int i = 0; i < m_AttachmentCargos.Count(); i++ )
			{
				m_AttachmentCargos.GetElement( i ).UpdateInterval();
			}
		}
		
		ElectricityIcon();
		m_CollapsibleHeader.UpdateInterval();
	}

	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.ClosableContainer;
	}

	override void OnShow()
	{
		super.OnShow();

		for ( int i = 0; i < m_Body.Count(); i++ )
		{
			m_Body.Get( i ).OnShow();
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
	
			ItemBase item = ItemBase.Cast( iw.GetItem() );
			
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();
			Weapon_Base wpn;
			Magazine mag;
			if( Class.CastTo(wpn, item_in_hands ) )
			{
				return;
			}
	
			if( !item.GetInventory().CanRemoveEntity() )
				return;
			
			if( player.GetInventory().HasEntityInInventory( item ) && player.GetHumanInventory().CanAddEntityInHands( item ) )
			{
				player.PredictiveTakeEntityToHands( item );
			}
			else
			{
				if( player.GetInventory().CanAddEntityToInventory( item ) && player.GetHumanInventory().CanRemoveEntityInHands() )
				{
					player.PredictiveTakeEntityToInventory( FindInventoryLocationType.ANY, item );
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
	
	void ElectricityIcon()
	{
		EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
		if( item_in_hands && item_in_hands.GetCompEM() )
		{
			if( GetRootWidget().FindAnyWidget( "electricity" ) )
			{
				bool show_electricity_icon = item_in_hands.GetCompEM().HasElectricityIcon();
				GetRootWidget().FindAnyWidget( "electricity" ).Show( show_electricity_icon );
			}
			
			if( GetRootWidget().FindAnyWidget( "electricity_can_work" ) )
			{
				bool show_electricity_can_work_icon = item_in_hands.GetCompEM().CanWork() && !item_in_hands.IsRuined();
				GetRootWidget().FindAnyWidget( "electricity_can_work" ).Show( show_electricity_can_work_icon );
			}
		}
		else
		{
			GetRootWidget().FindAnyWidget( "electricity" ).Show( false );
		}
	}

	void CollapseButtonOnMouseButtonDown( Widget w )
	{
		int i;
		if( !m_Hidden )
		{
			for (i = 0; i < m_Body.Count(); i++)
			{
				m_Body.Get( i ).OnHide();
			}
			m_Hidden = true;
		}
		else
		{
			for (i = 0; i < m_Body.Count(); i++)
			{
				m_Body.Get( i ).OnShow();
			}
			m_Hidden = false;
		}

		w.FindAnyWidget("opened").Show(m_Hidden);
		w.FindAnyWidget("closed").Show(!m_Hidden);
		
		m_Parent.Refresh();
	}
	
	void HideCargo()
	{
		if( m_CargoGrid )
		{
			m_CargoGrid.OnHide();
			RecomputeOpenedContainers();
		}	
	}
	
	void ShowCargo()
	{
		if( m_CargoGrid )
		{
			m_CargoGrid.OnShow();
			RecomputeOpenedContainers();
		}
	}
}
