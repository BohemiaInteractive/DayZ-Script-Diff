class HandsPreview: Container
{
	protected ref Icon	m_Icon;
	protected EntityAI	m_AttachmentsInitialized;
	
	protected float		m_IconSize;

	protected ItemBase	m_Item

	void HandsPreview( LayoutHolder parent )
	{
		GetGame().GetPlayer().GetOnItemAddedToHands().Insert( CreateNewIcon );
		GetGame().GetPlayer().GetOnItemRemovedFromHands().Insert( DeleteIcon );
	}
	
	void ~HandsPreview()
	{
		GetGame().GetPlayer().GetOnItemAddedToHands().Remove( CreateNewIcon );
		GetGame().GetPlayer().GetOnItemRemovedFromHands().Remove( DeleteIcon );
	}

	void RefreshQuantity( EntityAI m_Item_to_refresh )
	{
		if ( m_Icon )
		{
			m_Icon.SetQuantity();
		}
	}
	
	override EntityAI GetFocusedItem()
	{
		return m_Item;
	}

	Icon GetIcon()
	{
		return m_Icon;
	}
	
	override void SetDefaultFocus(bool while_micromanagment_mode = false)
	{
		super.SetDefaultFocus(while_micromanagment_mode);
		if ( m_Icon )
			m_Icon.SetActive(true);
	}
	
	override void SetLastFocus()
	{
		super.SetLastFocus();
		if ( m_Icon )
			m_Icon.SetActive(true);
	}
	
	override void Unfocus()
	{
		super.Unfocus();
		if ( m_Icon )
			m_Icon.SetActive(false);
	}
	
	float GetIconSize()
	{
		return m_IconSize;
	}

	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.HandsPreview;
	}

	void RemoveItem()
	{
		delete m_Icon;
		m_Item = null;
	}

	override void UpdateInterval()
	{
		bool show_combine_swap = ItemManager.GetInstance().IsDragging();

		#ifdef PLATFORM_CONSOLE
		if ( m_Icon && !m_Icon.IsDragged() && GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer() )
		#else
		if ( m_Icon && !m_Icon.IsDragged() )
		#endif
		{
			m_Icon.GetMainWidget().FindAnyWidget( "Combine" ).Show(  show_combine_swap );
			m_Icon.GetMainWidget().FindAnyWidget( "Swap" ).Show( show_combine_swap );
		}
		
		if ( m_Icon )
		{
			m_Icon.UpdateInterval();
		}
	}

	void CreateNewIcon( ItemBase item )
	{
		float y;
		GetRootWidget().GetParent().GetParent().GetParent().GetParent().GetScreenSize( m_IconSize, y );
		m_IconSize = m_IconSize / 10;
		
		m_Icon = new Icon( this, true );
		m_Icon.Refresh();
		if ( m_Icon )
		{
			m_Item = item;
			m_Icon.Init( m_Item );
			m_Icon.FullScreen();
			( HandsContainer.Cast( m_Parent ) ).ShowAtt( m_Item );
			m_Icon.Refresh();
		}
		
		m_RootWidget.SetColor( ARGB( 180, 0, 0, 0 ) );
		m_Parent.GetParent().Refresh();
		Inventory.GetInstance().UpdateConsoleToolbar();
		
		HandsContainer parent = HandsContainer.Cast( m_Parent );
		HandsHeader header = HandsHeader.Cast( parent.GetHeader() );

		if( m_Item && !m_Item.GetInventory().GetCargo() )
		{
			string display_name = m_Item.GetDisplayName();
			display_name.ToUpper();
			header.SetName( display_name );
		}
	}
	
	void DeleteIcon()
	{
		RemoveItem();
		( HandsContainer.Cast( m_Parent ) ).DestroyAtt();
		( HandsContainer.Cast( m_Parent ) ).DestroyCargo();
		( HandsContainer.Cast( m_Parent ) ).RemoveItem();
		m_AttachmentsInitialized = null;
		m_RootWidget.SetColor( ARGB( 166, 80, 80, 80 ) );
		m_Parent.GetParent().Refresh();
		Inventory.GetInstance().UpdateConsoleToolbar();
	}
	
	override bool SelectItem()
	{
		if( m_Item )
		{
			ItemManager.GetInstance().SetSelectedItem( m_Item, null, null, null );
			return true;
		}
		return false;
	}
	
	override bool Select()
	{
		Man player = GetGame().GetPlayer();
		EntityAI item_in_hands = m_Item;
		if( ItemManager.GetInstance().IsMicromanagmentMode() )
		{
			EntityAI selected_item = ItemManager.GetInstance().GetSelectedItem();
			if( selected_item && selected_item.GetInventory().CanRemoveEntity() )
			{
				if(item_in_hands && item_in_hands.GetInventory().CanRemoveEntity())
				{
					if( GameInventory.CanSwapEntitiesEx( item_in_hands, selected_item ) )
					{
						player.PredictiveSwapEntities( item_in_hands, selected_item );
						return true;
					}
				}
				else
				{
					if ( player.GetHumanInventory().CanAddEntityInHands( selected_item ) && selected_item.GetInventory().CanRemoveEntity())
					{
			 			ItemBase item_base = ItemBase.Cast( selected_item );
						float stackable = item_base.GetTargetQuantityMax();
						if (stackable == 0 || item_base.GetQuantity() <= stackable)
						{
							GetGame().GetPlayer().PredictiveTakeEntityToHands( item_base );		
							return true;
						}
						else if( stackable != 0 && stackable <= item_base.GetQuantity() )
						{
							item_base.SplitIntoStackMaxHandsClient(PlayerBase.Cast( GetGame().GetPlayer() ));
							return true;
						}
					}
				}
			}
		}
		return false;
	}
}
