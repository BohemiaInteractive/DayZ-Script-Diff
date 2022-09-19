class InventoryQuickbar extends InventoryGridController
{
	
	protected ref TItemsMap	m_Items;
	protected InventoryGrid	m_Grid;
	protected int			m_DraggingIndex;
	
	void InventoryQuickbar(Widget quickbarGridWidget)
	{
		m_DraggingIndex = INDEX_NOT_FOUND;
		m_Items = new TItemsMap;
		UpdateItems( quickbarGridWidget );
	}

	void ~InventoryQuickbar()
	{
	}
	
	void UpdateItems( Widget quickbarGridWidget )
	{
		if( !quickbarGridWidget )
		{
			return;
		}
		
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( !player )
			return;
		
		int i;

		// create grid and align it to center
		if( !m_Grid )
		{
			quickbarGridWidget.GetScript( m_Grid );
			m_Grid.SetController( this );
			m_Grid.GenerateQuickBarBackgroundTiles( 10 );
		}
		
		m_Items.Clear();
		InventoryItem item;
		
		if( m_Grid )
		{
			if( m_Grid.GetGridSize() != player.GetQuickBarSize() )
			{
				m_Grid.SetGridSize( player.GetQuickBarSize() );
			}
		}
		
		for( i = 0; i < m_Grid.GetGridSize(); i++)
		{
			item = InventoryItem.Cast( player.GetQuickBarEntity(i) );
			if( item )
			{
				m_Items.Set( item, Vector(i, 1, 1) );
			}
		}
		
		if( m_Grid )
		{
			m_Grid.UpdateQuickbarItems( m_Items );
		}
	}

	void Remove(InventoryItem itemToRemove)
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );

		for ( int i = 0; i < m_Grid.GetGridSize(); i++ )
		{
			InventoryItem item = InventoryItem.Cast( player.GetQuickBarEntity(i) );
			if(item == itemToRemove) 
			{
				player.RemoveQuickBarEntityShortcut( item );
			}
		}
	}
	
	// InventoryGridController override
	override void OnItemEnter(InventoryGrid grid, Widget w, int row, int col)
	{
		Widget quickbar = grid.GetRoot();
		if( quickbar )
		{
			// quickbar.SetColor( 0xFFFFFFFF );
		}
	}

	override void OnItemLeave(InventoryGrid grid, Widget w)
	{
		Widget quickbar = grid.GetRoot();
		if( quickbar )
		{
			// quickbar.SetColor( 0x7FFFFFFF );
		}
	}

	override void OnItemDraggingOver(InventoryGrid grid, Widget w, int row, int col)
	{
		ItemPreviewWidget iw;
		InventoryItem dragged_item;
		iw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		if( !iw )
		{
			string name = w.GetName();
			name.Replace( "PanelWidget", "Render" );
			iw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		} 
		
		if( !iw || !iw.GetItem() )
		{
			return;
		}
		
		if(!dragged_item && iw)
			dragged_item = InventoryItem.Cast( iw.GetItem() );

		if ( dragged_item == NULL)
			return;
		
		if (dragged_item && dragged_item.GetInventory().CanRemoveEntity() == false)
		{
			int color;

			color = ARGB( 150, 255, 1, 1 );

			if( w.FindAnyWidget("Selected") )
			{
				w.FindAnyWidget("Selected").SetColor( color );
			}
			else
			{
				name = w.GetName();
				name.Replace( "PanelWidget", "Cursor" );
				if( w.FindAnyWidget( name ) )
				{
					w.FindAnyWidget( name ).SetColor( color );
				}
			}			
		}
		else
		{
			color = ARGB( 150, 1, 255, 1 );

			if( w.FindAnyWidget("Selected") )
			{
				w.FindAnyWidget("Selected").SetColor( color );
			}
			else
			{
				name = w.GetName();
				name.Replace( "PanelWidget", "Cursor" );
				if( w.FindAnyWidget( name ) )
				{
					w.FindAnyWidget( name ).SetColor( color );
				}
			}
		}
	}

	override void OnItemDropReceived(InventoryGrid grid, Widget w, int row, int col)
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		ItemBase dragged_item = ItemBase.Cast( ItemManager.GetInstance().GetDraggedItem() );
		
		ItemPreviewWidget iw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		if( !iw )
		{
			string name = w.GetName();
			name.Replace( "PanelWidget", "Render" );
			iw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		}
		
		if(!dragged_item && iw)
			dragged_item = ItemBase.Cast( iw.GetItem() );
		
		if(dragged_item && dragged_item.GetHierarchyRootPlayer()!= GetGame().GetPlayer())
			return;
		
		ItemBase quickbar_item = ItemBase.Cast( player.GetQuickBarEntity(col) );
		
		if (m_DraggingIndex != INDEX_NOT_FOUND)
		{
			dragged_item = ItemBase.Cast( player.GetQuickBarEntity(m_DraggingIndex) );
		}
			
		if (dragged_item && dragged_item.GetInventory().CanRemoveEntity())
		{
			player.SetQuickBarEntityShortcut(dragged_item, col);
			
			if (quickbar_item && quickbar_item != dragged_item && m_DraggingIndex != INDEX_NOT_FOUND)
			{
				player.SetQuickBarEntityShortcut(quickbar_item, m_DraggingIndex);
			}
		}
		m_DraggingIndex = INDEX_NOT_FOUND;
		InventoryMenu menu = InventoryMenu.Cast( GetGame().GetUIManager().FindMenu(MENU_INVENTORY) );
		if ( menu )
		{
			menu.RefreshQuickbar();
		}
	}
	
	override void OnItemDrop(InventoryGrid grid, Widget w, int row, int col)
	{
		if (m_DraggingIndex != INDEX_NOT_FOUND)
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			InventoryItem dragged_item = InventoryItem.Cast( player.GetQuickBarEntity(m_DraggingIndex) );
			Remove(dragged_item);
			m_DraggingIndex = INDEX_NOT_FOUND;
		}
		
		InventoryMenu menu = InventoryMenu.Cast( GetGame().GetUIManager().FindMenu(MENU_INVENTORY) );
		if ( menu )
		{
			menu.RefreshQuickbar();
		}
		ItemManager.GetInstance().HideDropzones();
	}

	override void OnItemDrag(InventoryGrid grid, Widget w, int row, int col)
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );		
		InventoryItem item = InventoryItem.Cast( player.GetQuickBarEntity(col) );
		if (item)
		{	
			m_DraggingIndex = col;
			m_Grid.SetItemColor(item, InventoryGrid.ITEM_COLOR_DRAG);
		}
	}
	
	override int GetQuickbarItemColor( InventoryGrid grid, InventoryItem item )
	{
		int color = 0x0AFFFFFF;
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		EntityAI itemInHand = player.GetHumanInventory().GetEntityInHands();
		if ( player && itemInHand == item )
		{
			if ( player.GetHumanInventory().FindUserReservedLocationIndex(item) != -1 || grid.CanAddItemInHandToInventory() )
				color = InventoryGrid.ITEM_COLOR_QUICKBAR_H_GOOD;
			else
				color = InventoryGrid.ITEM_COLOR_QUICKBAR_H_BAD;
		}
		else if ( itemInHand )
		{
			if ( !grid.CanAddItemInHandToInventory() && !GameInventory.CanSwapEntitiesEx(item, itemInHand) )	
				color = InventoryGrid.ITEM_COLOR_QUICKBAR_I_BAD;
		}
		return color;
	}

	
	override int HasItemQuantity( InventoryItem item )
	{
		return QuantityConversions.HasItemQuantity( item );
	}

	override string GetItemQuantityText( InventoryItem item )
	{
		return QuantityConversions.GetItemQuantityText( item );
	}

	override float GetItemQuantity( InventoryItem item )
	{
		return QuantityConversions.GetItemQuantity( item );
	}
	
	override int GetItemQuantityMax( InventoryItem item )
	{
		ItemBase ib = ItemBase.Cast(item);
		return ib.m_VarQuantityMax;
	}
	
	override int GetItemCount( InventoryItem item )
	{
		ItemBase ib = ItemBase.Cast(item);
		return ib.m_Count;
	}
	
	override bool CanAddItemInHandToInventory()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		EntityAI itemInHand = player.GetHumanInventory().GetEntityInHands();
		return itemInHand && player.GetInventory().CanAddEntityToInventory(itemInHand);
	}
}
