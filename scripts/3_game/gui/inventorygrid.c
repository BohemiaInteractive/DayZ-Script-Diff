// -----------------------------------------------------------
class InventoryGridController extends ScriptedWidgetEventHandler
{
	void OnItemEnter(InventoryGrid grid, Widget w, int row, int col) {}
	void OnItemLeave(InventoryGrid grid, Widget w) {}
	void OnItemDrag(InventoryGrid grid, Widget w, int row, int col) {}
	void OnItemDraggingOver(InventoryGrid grid, Widget w, int row, int col) {}
	void OnItemDrop(InventoryGrid grid, Widget w, int row, int col) {}
	void OnItemDropReceived(InventoryGrid grid, Widget w, int row, int col) {}
	void OnItemClick(InventoryGrid grid, Widget w, int row, int col) {}
	void OnItemLeftClick(InventoryGrid grid, Widget w, int row, int col) {}
	void OnItemRightClick(InventoryGrid grid, Widget w, int row, int col) {}
	void OnItemDoubleClick(InventoryGrid grid, Widget w, int row, int col) {}
	// float GetItemQuantity(InventoryGrid grid, InventoryItem item) {}
	int GetItemColor(ScriptedWidgetEventHandler grid, InventoryItem item)
	{ 
		return 0;
	}
	int GetQuickbarItemColor(InventoryGrid grid, InventoryItem item) {}
	vector GetItemSize(InventoryGrid grid, InventoryItem item)
	{
		return Vector(0, 1, 1);
	}
	string GetItemQuantityText( InventoryItem item ) {}
	int HasItemQuantity( InventoryItem item ) {}
	float GetItemQuantity( InventoryItem item ) {}
	int GetItemQuantityMax( InventoryItem item ) {}
	int GetItemCount( InventoryItem item ) {}
	bool CanAddItemInHandToInventory() {}
};

// -----------------------------------------------------------
//! map: item x vector(index, width, height)
typedef map<InventoryItem, vector> TItemsMap

class InventoryGrid extends ScriptedWidgetEventHandler
{
	// AARRGGBB
	static int ITEM_COLOR_QUICKBAR_NORMAL	= 0x0AFFFFFF;
	static int ITEM_COLOR_QUICKBAR_H_GOOD	= 0x2A6e980d;
	static int ITEM_COLOR_QUICKBAR_H_BAD	= 0x2A980d0d;
	static int ITEM_COLOR_QUICKBAR_I_BAD	= 0x2A986e0d;
	static int ITEM_COLOR_NORMAL			= 0x0AFFFFFF;
	static int ITEM_COLOR_DRAG				= 0x0AFFFFFF;
	
	reference bool						m_IsDebugOutput;

	protected ref map<int, Widget>		m_BackgroundWidgets;
	protected ref map<int, Widget>		m_ItemWidgets;
	protected ref TItemsMap				m_Items;
	protected InventoryGridController	m_Controller;

	
	protected Widget					m_Root;
	protected int						m_GridSize;
	protected bool						m_IsMouseLeftDown;
	protected bool						m_CanAddItemInHandToInventory;

	void InventoryGrid()
	{
		m_Items				= new TItemsMap;	
		m_BackgroundWidgets	= new map<int, Widget>;
		m_ItemWidgets		= new map<int, Widget>;
	}

	protected void OnWidgetScriptInit(Widget w)
	{
		m_Root = w;
		m_Root.SetHandler(this);
	}
	
	// NOTE: This is a cached value, depending on when it is called it might not be 100% accurate
	bool CanAddItemInHandToInventory()
	{
		return m_CanAddItemInHandToInventory;
	}
	
	// ScriptedWidgetEventHandler override
	override bool OnUpdate(Widget w)
	{
		return false;
	}
	
	// -----------------------------------------------------------
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int col = GetCol( w );
		
		if ( !IsValidPos( col ) )
			return false;	
		
		if (m_IsDebugOutput)
		{
			PrintString (m_Root.GetName() + "::OnMouseEnter(" + col.ToString() + ")");
		}
		
		if (m_Controller) m_Controller.OnItemEnter(this, w, 0, col);
	
		return true;
	}
	
	// -----------------------------------------------------------
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (m_Controller) m_Controller.OnItemLeave(this, w);
	
		return true;
	}
	
	// -----------------------------------------------------------
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (button == MouseState.RIGHT || button == MouseState.LEFT)
		{
			int col = GetCol( w );
		
			if ( !IsValidPos( col ) )
				return false;
			
			if (m_IsDebugOutput)
			{
				PrintString (m_Root.GetName() + "::OnMouseButtonDown(" + col.ToString() + ")");
			}
		
			if (m_Controller) 
			{
				if (button == MouseState.RIGHT )
				{
					m_Controller.OnItemRightClick(this, w, 0, col);
				}

				if (button == MouseState.LEFT )
				{
					m_IsMouseLeftDown = true;
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	// -----------------------------------------------------------
	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if ( button == MouseState.LEFT )
		{
			if( m_IsMouseLeftDown )
			{
				m_Controller.OnItemLeftClick(this, w, 0, m_col);
				m_IsMouseLeftDown = false;
			}
		}

		if (button == MouseState.RIGHT || button == MouseState.LEFT)
		{
			int col = GetCol( w );
		
			if ( !IsValidPos( col ) )
				return false;
			
			if (m_IsDebugOutput)
			{
				PrintString (m_Root.GetName() + "::OnMouseButtonUp(" + col.ToString() + ")");
			}
		
			return true;
		}
		else
		{
			return false;
		}
	}
	
	// -----------------------------------------------------------
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT) return false;
		
		int col = GetCol( w );
	
		if ( !IsValidPos( col ) )
				return false;
		
		if (m_IsDebugOutput)
		{
			PrintString (m_Root.GetName() + "::OnDoubleClick(" + col.ToString() + ")");
		}
	
		if (m_Controller) m_Controller.OnItemDoubleClick(this, w, 0, col);
	
		return true;
	}
	
	//--------------------------------------------------------------------------
	override bool OnDrop(Widget w, int x, int y, Widget reciever)
	{
		if (m_IsDebugOutput)
		{
			PrintString (m_Root.GetName() + "::OnDrop()");
		}
	
		if (m_Controller) 
		{
			m_Controller.OnItemDrop(this, w, 0, m_col);
		}
		
		if (w)
		{
			ItemPreviewWidget item_preview = ItemPreviewWidget.Cast( w.FindAnyWidget("Preview") );
			if (item_preview)
			{
				item_preview.SetView( item_preview.GetItem().GetViewIndex() );
			}
		}
		
		return true;
	}
	
	int GetCol( Widget w )
	{
		int index;
		
		if( m_ItemWidgets.GetKeyByValueChecked( w, index ) )
			return index;
		else if( m_BackgroundWidgets.GetKeyByValueChecked( w, index ) )
			return index;
		else
			return -1;
	}
	
	int GetColFromBg( Widget w )
	{
		return m_BackgroundWidgets.GetKeyByValue( w );
	}
	
	int GetGridSize()
	{
		return m_GridSize;
	}
	
	void SetGridSize( int size )
	{
		m_GridSize = size;
		for( int i = 0; i < m_BackgroundWidgets.Count(); i++ )
		{
			if( i < size )
			{
				m_BackgroundWidgets.Get( i ).Show( true );
			}
			else
			{
				m_BackgroundWidgets.Get( i ).Show( false );
			}
		}
	}
	
	bool IsValidPos( int index )
	{
		return ( m_GridSize > index && index > -1 );
	}
	
	private Widget m_on_drag_item;
	private int m_col;
	
	//--------------------------------------------------------------------------
	override bool OnDrag(Widget w, int x, int y)
	{
		w.SetPos(x, y);

		m_col = GetCol( w );
		
		if ( !IsValidPos( m_col ) )
			return false;
		
		if (m_IsDebugOutput)
		{
			PrintString (m_Root.GetName() + "::OnDrag(" + m_col.ToString() + ")");
		}
	
		if (m_Controller) m_Controller.OnItemDrag(this, w, 0, m_col);
	
		ItemPreviewWidget item_preview = ItemPreviewWidget.Cast( w.FindAnyWidget("Preview") );
		if (item_preview)
		{
			item_preview.SetView( item_preview.GetItem().GetViewIndex() );
		}
		return true;
	}
	
	//--------------------------------------------------------------------------
	override bool OnDraggingOver(Widget w, int x, int y, Widget reciever)
	{
		int col = GetCol( reciever );
		
		if ( !IsValidPos( col ) )
		{
			return false;
		}
		
		if (m_IsDebugOutput)
		{
			PrintString (m_Root.GetName() + "::OnDraggingOver(" + col.ToString() + ")");
		}
	
		if (m_Controller) m_Controller.OnItemDraggingOver(this, w, 0, col);
	
		
		return true;
	}
	
	//--------------------------------------------------------------------------
	override bool OnDropReceived(Widget w, int x, int y, Widget reciever)
	{
		int col = GetCol( reciever );
	
		if ( !IsValidPos( col ) )
			return false;
		
		if (m_IsDebugOutput)
		{
			PrintString (m_Root.GetName() + "::OnDropReceived(" + col.ToString() + ")");
		}
	
		if (m_Controller) m_Controller.OnItemDropReceived(this, w, 0, col);
	
		
		return true;
	}
	
	TItemsMap GetItems() {
		return m_Items;	
	}
	
	//--------------------------------------------------------------------------
	Widget GetItem(int index)
	{
		if( m_ItemWidgets.Contains( index ) )
			return m_ItemWidgets.Get( index );
		else
			return null;
	}
	
	//--------------------------------------------------------------------------
	Widget GetItemBackground(int index)
	{
		if( m_BackgroundWidgets.Contains( index ) )
			return m_BackgroundWidgets.Get( index );
		else
			return null;
	}

	Widget GetRoot()
	{
		return m_Root;
	}
	
	void SetItemColor(InventoryItem item, int color)
	{
		if (m_Items.Contains(item))
		{
			vector data = m_Items.Get(item);
			int index = Math.Round(data[0]);
			
			Widget w = GetItem(index);
			if (w)
				w.SetColor(color);
		}
	}
	
	void SetController(InventoryGridController controller) { 
		m_Controller = controller; 
	}

	InventoryGridController GetController() { 
		return m_Controller; 
	}
	
	//--------------------------------------------------------------------------
	void GenerateQuickBarBackgroundTiles(int count)
	{
		for (int i = 0; i < count; i++)
		{
			Widget root_widget = GetGame().GetWorkspace().CreateWidgets("gui/layouts/inventory/inventoryGridBackground.layout", m_Root);
			TextWidget label_widget = TextWidget.Cast( root_widget.FindAnyWidget( "Label1" ) );
			TextWidget label_widget2 = TextWidget.Cast( root_widget.FindAnyWidget( "Label2" ) );
			label_widget.SetText( (i+1).ToString() );
			label_widget2.SetText( (i+1).ToString() );
			m_BackgroundWidgets.Insert( i, root_widget );
		}
	}
	
	//--------------------------------------------------------------------------
	void UpdateQuickbarItems( TItemsMap items )
	{
		int i;
		int c;
		int index;
		int width;
		int height;
		int row;
		int col;
		InventoryItem item;
		vector data;
		Widget bck;
		Widget item_w;
		
		// remove not actual items
		c = m_Items.Count();
		for (i = 0; i < c; i++)
		{
			item = m_Items.GetKey(i);
			if( item != NULL )
			{
				bool remove_item = false;
				
				if (items.Contains(item) == false)
				{
					remove_item = true;
				}
				else
				{
					// check items position actual
					if ((m_Items.Get(item) - items.Get(item)).LengthSq() > 0.01)
					{
						// item has different position or size
						remove_item = true;
					}
				}
				
				if (remove_item)
				{
					RemoveItem(item);
					c--;
					i--;			
				}
			}
		}
		
		// add new items
		for (i = 0; i < items.Count(); i++)
		{
			item = items.GetKey(i);
			data = items.Get(item);
					
			if (m_Items.Contains(item) == false)
			{
				AddItem(item, data, Vector(0,0,0) );
			}
		}
		
		// cache if entity in hands can be added to inventory
		m_CanAddItemInHandToInventory = m_Controller.CanAddItemInHandToInventory();
	
		// refresh quickbar
		for (i = 0; i < items.Count(); i++)
		{
			item = items.GetKey(i);
			data = items.Get(item);
			RefreshQuickbarItemVariables( item, data );
		}
	}
	
	
	//--------------------------------------------------------------------------
	void UpdateItems(TItemsMap items, bool show_quantity, bool show_temperature )
	{
		int i;
		int c;
		int index;
		int width;
		int height;
		int row;
		int col;
		InventoryItem item;
		vector data;
		Widget bck;
		Widget item_w;
		
		// remove not actual items
		c = m_Items.Count();
		for (i = 0; i < c; i++)
		{
			item = m_Items.GetKey(i);
			
			bool remove_item = false;
			
			if (items.Contains(item) == false)
			{
				remove_item = true;
			}
			else
			{
				// check items position actual
				if ((m_Items.Get(item) - items.Get(item)).LengthSq() > 0.01)
				{
					// item has different position or size
					remove_item = true;
				}
			}
			
			if (remove_item)
			{
				RemoveItem(item);
				c--;
				i--;			
			}
		}
		
		// add new items
		for (i = 0; i < items.Count(); i++)
		{
			item = items.GetKey(i);
			data = items.Get(item);
					
			if (m_Items.Contains(item) == false)
			{
				AddItem(item, data, Vector(0,0,0) );
			}
		}
	
		// add new items
		for (i = 0; i < items.Count(); i++)
		{
			item = items.GetKey(i);
			data = items.Get(item);
			// refresh quantity
			RefreshItemVariables( item, data, show_quantity, show_temperature );
		}
	}
	
	//--------------------------------------------------------------------------
	void UpdateQuantityItems()
	{
		int i;
		int index;
		InventoryItem item;
		vector data;
		for (i = 0; i < m_Items.Count(); i++)
		{
			item = m_Items.GetKey(i);
			data = m_Items.Get(item); 
			index = Math.Round(data[0]);
			RefreshItemVariables( item, data, true, false );
		}
	}
	
	//--------------------------------------------------------------------------
	void UpdateTemperatureItems()
	{
		int i;
		int index;
		InventoryItem item;
		vector data;
		for (i = 0; i < m_Items.Count(); i++)
		{
			item = m_Items.GetKey(i);
			data = m_Items.Get(item); 
			index = Math.Round(data[0]);
			RefreshItemVariables( item, data, false, true );
		}
	}
	
	//--------------------------------------------------------------------------
	void RefreshQuickbarItemVariables(InventoryItem item, vector data)
	{
		int	index = Math.Round(data[0]);
		Widget bck = GetItemBackground(index);
		if ( bck )
		{
			Widget item_w = bck.FindAnyWidget("GridItem");
			if ( item_w )
			{
				int color = m_Controller.GetQuickbarItemColor( this, item ); // !!!!!
				item_w.SetColor( color );
			}
		}
		RefreshItemVariables( item, data, true, false );
	}
	
	//--------------------------------------------------------------------------
	void RefreshItemVariables(InventoryItem item, vector data, bool show_quantity, bool show_temperature )
	{
		int	index = Math.Round(data[0]);
		Widget bck = GetItemBackground(index);
		Widget item_w;
	
		if ( bck )
		{
			item_w = bck.FindAnyWidget("GridItem");
			if ( item_w )
			{
				int has_quantity = m_Controller.HasItemQuantity( item );
				Widget quantity_panel = bck.FindAnyWidget("QuantityPanel");
				TextWidget item_quantity = TextWidget.Cast( bck.FindAnyWidget("Quantity") );
				ProgressBarWidget quantity_progress = ProgressBarWidget.Cast( bck.FindAnyWidget("QuantityBar") );
				Widget quantity_stack = bck.FindAnyWidget("QuantityStackPanel");
				if ( has_quantity == QUANTITY_HIDDEN )
				{
					quantity_panel.Show( false );
				}
				else
				{
					quantity_panel.Show( true );
					if ( has_quantity == QUANTITY_COUNT )
					{
						item_quantity.SetText( m_Controller.GetItemQuantityText( item ) );
						quantity_stack.Show( true );
						quantity_progress.Show( false );
					}
					else if ( has_quantity == QUANTITY_PROGRESS )
					{
						float progress_max = quantity_progress.GetMax();
						int max = m_Controller.GetItemQuantityMax( item );
						int count = m_Controller.GetItemCount( item );
						float quantity = m_Controller.GetItemQuantity( item );
						if ( count > 0 )
						{
							max = count;
						}
						if ( max > 0 )
						{
	
							float value = Math.Round( ( quantity / max ) * 100 );
							quantity_progress.SetCurrent( value );
						}
						quantity_stack.Show( false );
						quantity_progress.Show( true );
					}
				}
				
				if ( show_temperature )
				{
					if ( item && item.IsInherited( InventoryItem ) )
					{
						int color = m_Controller.GetItemColor( this, item); // !!!!!
						if ( color )
						{
							item_w.SetColor( color );
						}
					}
				}
			}
		}
	}
	
	//--------------------------------------------------------------------------
	void AddItem(InventoryItem item, vector data, vector rotation)
	{
		m_Items.Set(item, data);			
				
		int index = Math.Round(data[0]);
		int width = Math.Round(data[1]);
		int height = Math.Round(data[2]);
				
		Widget bck = GetItemBackground(index);
		Widget item_w_bck = GetGame().GetWorkspace().CreateWidgets("gui/layouts/inventory/inventoryGridItem.layout", bck);
		Widget item_w = item_w_bck.FindAnyWidget("GridItem");
		
		bck.FindAnyWidget("LabelTR").Show( true );
		bck.FindAnyWidget("LabelCC").Show( false );
		
		m_ItemWidgets.Insert( index, item_w );
		
		ResetItemWidget(item_w, width, height);
		
		if ( item )
		{
			ItemPreviewWidget item_preview = ItemPreviewWidget.Cast( item_w.FindAnyWidget("Preview") );
			item_preview.SetItem(item);
			item_preview.SetModelOrientation( rotation );
			item_preview.SetView( item_preview.GetItem().GetViewIndex() );
			
			RefreshItemVariables( item, data, true, true );
		}
	}

	//--------------------------------------------------------------------------
	void SetItem(InventoryItem item, vector data, vector rotation)
	{
		m_Items.Set(item, data);
				
		int index = Math.Round(data[0]);
		int width = Math.Round(data[1]);
		int height = Math.Round(data[2]);
				
		Widget bck = GetItemBackground(index);
		Widget item_w_bck = bck.FindAnyWidget("GridItemBck");
		
		if(item_w_bck)
		{
			bck.FindAnyWidget("LabelTR").Show( true );
			bck.FindAnyWidget("LabelCC").Show( false );
			
			Widget item_w = item_w_bck.FindAnyWidget("GridItem");
			ResetItemWidget(item_w, width, height);
			
			if ( item )
			{
				ItemPreviewWidget item_preview = ItemPreviewWidget.Cast( item_w.FindAnyWidget("Preview") );
				item_preview.SetItem(item);
				item_preview.Show( true );
				item_preview.SetModelOrientation( rotation );
				item_preview.SetView( item_preview.GetItem().GetViewIndex() );
				
				RefreshItemVariables( item, data, true, true );
			}
			else
			{
				item_preview = ItemPreviewWidget.Cast( item_w.FindAnyWidget("Preview") );
				item_preview.Show( false );
			}
		}
	}
	
	//--------------------------------------------------------------------------
	protected void ResetItemWidget(Widget item_w, int width, int height)
	{
		if( item_w )
		{
			item_w.SetColor(ITEM_COLOR_NORMAL);
		}
	}
	
	//--------------------------------------------------------------------------
	void ResetItem(InventoryItem item)
	{
		if (m_Items.Contains(item))
		{
			vector data = m_Items.Get(item);
			int index = Math.Round(data[0]);
			int width = Math.Round(data[1]);
			int height = Math.Round(data[2]);
			Widget bck = GetItemBackground(index);
			Widget item_w = bck.FindAnyWidget("GridItem");
			ResetItemWidget(item_w, width, height);
			RefreshItemVariables( item, data, true, true );
		}
	}
	
	//--------------------------------------------------------------------------
	bool HasItem(InventoryItem item)
	{
		return m_Items.Contains(item);
	}
	
	//--------------------------------------------------------------------------
	void RemoveItem(InventoryItem item)
	{
		Widget bck;
		Widget itemW;
		if ( item && m_Items.Contains( item ) )
		{
			vector data = m_Items.Get(item);
			int index = Math.Round(data[0]);
			int width = Math.Round(data[1]);
			int height = Math.Round(data[2]);
			bck = GetItemBackground(index);
			if( bck )
			{
				itemW = bck.FindAnyWidget("GridCell");
				if( itemW )
					delete itemW;
				bck.FindAnyWidget("LabelTR").Show( false );
				bck.FindAnyWidget("LabelCC").Show( true );
			}
		
			m_Items.Remove( item );
			m_ItemWidgets.Remove( index );
		}
	}
};
