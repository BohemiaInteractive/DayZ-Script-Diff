class ItemManager
{
	private ref static ItemManager		m_Instance;
	protected bool						m_IsDragging;
	protected EntityAI					m_HoveredItem;
	protected bool						m_SlotInfoShown;
	protected EntityAI					m_DraggedItem;
	protected Icon						m_DraggedIcon;
	protected ref Widget				m_TooltipWidget;
	protected ref Widget				m_TooltipSlotWidget;
	protected ref Widget				m_TooltipCategoryWidget;
	protected ItemPreviewWidget			m_ItemPreviewWidget;
	protected Widget					m_RootWidget;
	protected ref map<string, bool>		m_DefautOpenStates;
	protected ref map<string, bool>		m_DefautHeaderOpenStates;
 	protected int						m_HandsDefaultOpenState;
	protected ref Timer					m_ToolTipTimer;
	protected ref Timer					m_TooltipSlotTimer;

	protected EntityAI					m_SelectedItem;
	protected Container					m_SelectedContainer;
	protected Widget					m_SelectedWidget;
	protected SlotsIcon					m_SelectedIcon;
	
	protected HandsPreview				m_HandsPreview;
	
	protected bool						m_ItemMicromanagmentMode;
	
	protected Widget					m_LeftDropzone;
	protected Widget					m_CenterDropzone;
	protected Widget					m_RightDropzone;
	
	protected int 						m_TooltipPosX;
	protected int	 					m_TooltipPosY;
	protected Widget 					m_TooltipSourceWidget; //stored here for tooltip position updates
	
	#ifndef PLATFORM_CONSOLE
	protected const float TOOLTIP_DELAY = 0.25; // in seconds
	#else
	protected const float TOOLTIP_DELAY = 1.5; // in seconds
	#endif
	
	void ItemManager( Widget root )
	{
		m_Instance					= this;
		m_RootWidget				= root;
		m_DefautOpenStates			= new map<string, bool>;
		m_DefautHeaderOpenStates	= new map<string, bool>;
		m_SlotInfoShown				= false;
		
		#ifdef PLATFORM_CONSOLE
			m_TooltipWidget			= GetGame().GetWorkspace().CreateWidgets("gui/layouts/inventory_new/day_z_inventory_new_tooltip_xbox.layout", root );
			m_TooltipSlotWidget		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/inventory_new/day_z_inventory_new_tooltip_slot_xbox.layout", root );
		#else
			m_TooltipWidget			= GetGame().GetWorkspace().CreateWidgets("gui/layouts/inventory_new/day_z_inventory_new_tooltip.layout", root );
			m_TooltipSlotWidget		= GetGame().GetWorkspace().CreateWidgets("gui/layouts/inventory_new/day_z_inventory_new_tooltip_slot.layout", root );
		#endif
		m_TooltipWidget.Show( false );
		m_TooltipSlotWidget.Show( false );
	}
	
	void SetItemMicromanagmentMode( bool item_micromanagment_mode )
	{
		m_ItemMicromanagmentMode = item_micromanagment_mode;
	}
	
	bool IsMicromanagmentMode()
	{
		return m_ItemMicromanagmentMode;
	}
	
	HandsPreview GetHandsPreview()
	{
		return m_HandsPreview;
	}
	
	void SetHandsPreview( HandsPreview hansd_preview )
	{
		m_HandsPreview = hansd_preview;
	}

	EntityAI GetSelectedItem()
	{
		return m_SelectedItem;
	}

	Container GetSelectedContainer()
	{
		return m_SelectedContainer;	
	}
	
	Widget GetSelectedWidget()
	{
		return m_SelectedWidget;
	}
	
	SlotsIcon GetSelectedIcon()
	{
		return m_SelectedIcon;
	}

	void SetSelectedItem( EntityAI selected_item, Container selected_container, Widget selected_widget, SlotsIcon selected_icon )
	{
		m_SelectedItem		= selected_item;
		m_SelectedContainer	= selected_container;
		m_SelectedWidget	= selected_widget;
		m_SelectedIcon		= selected_icon;
	}

	void ClearDefaultOpenStates()
	{
		m_DefautOpenStates.Clear();
	}

	void ClearDefaultHeaderOpenStates()
	{
		m_DefautHeaderOpenStates.Clear();
	}

	void SetDefaultOpenState( string type, bool is_opened )
	{
		m_DefautOpenStates.Set( type, is_opened );
	}

	void SetDefaultHeaderOpenState( string type, bool is_opened )
	{
		m_DefautHeaderOpenStates.Set( type, is_opened );
	}

	void SetDefaultOpenStateHands( bool is_opened )
	{
		m_HandsDefaultOpenState = is_opened;
		int hands_default_open_state = m_HandsDefaultOpenState; //from bool to int for easier parsing
		GetGame().SetProfileString( "defaultOpenStateHands", hands_default_open_state.ToString() );
		GetGame().SaveProfile();
	}

	bool GetDefaultOpenStateHands( )
	{
		string value;
		GetGame().GetProfileString( "defaultOpenStateHands", value );
		m_HandsDefaultOpenState = value.ToInt();
		return m_HandsDefaultOpenState;
	}

	void SerializeDefaultHeaderOpenStates()
	{
		TStringArray serialized_types = new TStringArray;

		for ( int i = 0; i < m_DefautHeaderOpenStates.Count(); i++ )
		{
			int is_opened = m_DefautHeaderOpenStates.GetElement( i ); //from bool to int for easier parsing
			serialized_types.Insert( m_DefautHeaderOpenStates.GetKey( i ) + "," + is_opened );
		}

		if( serialized_types.Count() > 0 )
		{
			GetGame().SetProfileStringList( "defaultHeaderOpenStates", serialized_types );
		}
		GetGame().SaveProfile();
	}

	void DeserializeDefaultHeaderOpenStates()
	{
		ClearDefaultHeaderOpenStates();

		TStringArray serialized_types = new TStringArray;
		GetGame().GetProfileStringList( "defaultHeaderOpenStates", serialized_types );

		for( int i = 0; i < serialized_types.Count(); i++ )
		{
			TStringArray strs = new TStringArray;
			serialized_types.Get( i ).Split( ",", strs );
			bool is_opened = strs.Get( 1 ).ToInt();
			SetDefaultHeaderOpenState( strs.Get( 0 ), is_opened );
		}
	}

	void SerializeDefaultOpenStates()
	{
		TStringArray serialized_types = new TStringArray;

		for ( int i = 0; i < m_DefautOpenStates.Count(); i++ )
		{
			int is_opened = m_DefautOpenStates.GetElement( i ); //from bool to int for easier parsing
			serialized_types.Insert( m_DefautOpenStates.GetKey( i ) + "," + is_opened );
		}

		if( serialized_types.Count() > 0 )
		{
			GetGame().SetProfileStringList( "defaultOpenStates", serialized_types );
		}
		GetGame().SaveProfile();
	}

	void DeserializeDefaultOpenStates()
	{
		ClearDefaultOpenStates();

		TStringArray serialized_types = new TStringArray;
		GetGame().GetProfileStringList( "defaultOpenStates", serialized_types );

		for( int i = 0; i < serialized_types.Count(); i++ )
		{
			TStringArray strs = new TStringArray;
			serialized_types.Get( i ).Split( ",", strs );
			bool is_opened = strs.Get( 1 ).ToInt();
			SetDefaultOpenState( strs.Get( 0 ), is_opened );
		}
	}

	bool GetDefaultOpenState( string type )
	{
		if( m_DefautOpenStates.Contains( type ) )
		{
			return m_DefautOpenStates.Get( type );
		}
		else
		{
			return true;
		}
	}

	bool GetDefaultHeaderOpenState( string type )
	{
		if( m_DefautHeaderOpenStates.Contains( type ) )
		{
			return m_DefautHeaderOpenStates.Get( type );
		}
		else
		{
			return true;
		}
	}

	static ItemManager GetInstance()
	{
		return m_Instance;
	}

	void HideDropzones()
	{
		GetRightDropzone().SetAlpha( 0 );
		GetLeftDropzone().SetAlpha( 0 );
		GetCenterDropzone().SetAlpha( 0 );
	}

	void ShowSourceDropzone( EntityAI item )
	{
		EntityAI owner = item.GetHierarchyParent();
		if( owner && owner != GetGame().GetPlayer() )
		{
			ShowSourceDropzone( owner );
		}
		else
		{
			InventoryLocation inv_loc_src = new InventoryLocation;
			item.GetInventory().GetCurrentInventoryLocation( inv_loc_src );
			int loc_type = inv_loc_src.GetType();
			HideDropzones();
			if( loc_type == InventoryLocationType.GROUND )
			{
				GetLeftDropzone().SetAlpha( 1 );
			}
			else if( loc_type == InventoryLocationType.HANDS )
			{
				GetCenterDropzone().SetAlpha( 1 );
			}
			else
			{
				GetRightDropzone().SetAlpha( 1 );
			}
		}
	}

	Widget GetLeftDropzone()
	{
		if( !m_LeftDropzone )
			m_LeftDropzone	= m_RootWidget.FindAnyWidget( "LeftPanel" ).FindAnyWidget( "DropzoneX" );
		
		return m_LeftDropzone;
	}
	
	Widget GetRightDropzone()
	{
		if( !m_RightDropzone )
			m_RightDropzone	= m_RootWidget.FindAnyWidget( "RightPanel" ).FindAnyWidget( "DropzoneX" );
		return m_RightDropzone;
	}
	
	Widget GetCenterDropzone()
	{
		if( !m_CenterDropzone )
			m_CenterDropzone = m_RootWidget.FindAnyWidget( "HandsPanel" ).FindAnyWidget( "DropzoneX" );
		return m_CenterDropzone;
	}
	
	EntityAI GetHoveredItem()
	{
		return m_HoveredItem;
	}

	void SetDraggedItem( EntityAI dragged_item )
	{
		m_DraggedItem = dragged_item;
	}

	EntityAI GetDraggedItem()
	{
		return m_DraggedItem;
	}

	void SetDraggedIcon( Icon dragged_icon )
	{
		m_DraggedIcon = dragged_icon;
	}

	Icon GetDraggedIcon()
	{
		return m_DraggedIcon;
	}

	void SetIsDragging( bool is_dragging )
	{
		m_IsDragging = is_dragging;
		if( !is_dragging )
		{
			SetDraggedItem( null );
			SetDraggedIcon( null );
		}
	}

	bool IsDragging()
	{
		return m_IsDragging;
	}

	void HideTooltip()
	{
		m_TooltipWidget.Show( false );
		m_HoveredItem = null;
		delete m_ToolTipTimer;
		
		HideTooltipSlot();
	}
	
	void HideTooltipSlot()
	{
		if ( m_SlotInfoShown )
		{
			m_TooltipSlotWidget.Show( false );
			m_SlotInfoShown = false;
			delete m_TooltipSlotTimer;
		}
	}

	static int GetItemHealthColor(int pHealthLevel)
	{
		switch (pHealthLevel)
		{
		case -1:
		break;
		case GameConstants.STATE_PRISTINE:
			return Colors.COLOR_PRISTINE;
		case GameConstants.STATE_WORN:
			return Colors.COLOR_WORN;
		case GameConstants.STATE_DAMAGED:
			return Colors.COLOR_DAMAGED;
		case GameConstants.STATE_BADLY_DAMAGED:
			return Colors.COLOR_BADLY_DAMAGED;
		case GameConstants.STATE_RUINED:
			return Colors.COLOR_RUINED;	
		}
			
		return 0x00FFFFFF;
	}
	
	static int GetItemHealthColor(EntityAI item, string zone = "")
	{
		if (item)
		{
			switch (item.GetHealthLevel(zone))
			{
			case -1:
			break;
			case GameConstants.STATE_PRISTINE:
				return Colors.COLOR_PRISTINE;
			case GameConstants.STATE_WORN:
				return Colors.COLOR_WORN;
			case GameConstants.STATE_DAMAGED:
				return Colors.COLOR_DAMAGED;
			case GameConstants.STATE_BADLY_DAMAGED:
				return Colors.COLOR_BADLY_DAMAGED;
			case GameConstants.STATE_RUINED:
				return Colors.COLOR_RUINED;	
			}
		}
			
		return 0x00FFFFFF;
	}
	
	static int ColorFromFloat( float fraction )
	{
		if( fraction > 1 || fraction < 0 )
			return 0x00FFFFFF;
		else if( fraction > 0.80 )
			return Colors.COLOR_PRISTINE;
		else if( fraction > 0.6 )
			return Colors.COLOR_WORN;
		else if( fraction > 0.4 )
			return Colors.COLOR_DAMAGED;
		else if( fraction > 0.2 )
			return Colors.COLOR_BADLY_DAMAGED;
		else
			return Colors.COLOR_RUINED;	
	}

	void SetTemperature(EntityAI item, Widget item_w)
	{
		if ( item_w )
		{
			if ( true/*show_temperature */ )
			{
				if ( item && item.IsInherited( ItemBase ) )
				{
					int color = ColorManager.GetInstance().GetItemColor( ItemBase.Cast( item ) ); // !!!!!
					if ( color )
					{
						string name = item_w.GetName();
						name.Replace("Render", "Temperature");
						Widget temperature_widget = item_w.GetParent().FindAnyWidget( name );
						if ( color != -1 )
						{
							temperature_widget.Show( true );
							temperature_widget.SetColor( color );
						}
						else
						{
							temperature_widget.Show( false );
						}

						temperature_widget.SetAlpha( 0.3 );
					}
				}
			}
		}
	}

	void SetIconTemperature(EntityAI item, Widget item_w)
	{
		if ( item_w )
		{
			if ( item && item.IsInherited( ItemBase ) )
			{
				int color = ColorManager.GetInstance().GetItemColor( ItemBase.Cast( item ) );

				if ( color )
				{
					Widget color_widget = item_w.FindAnyWidget( "Color" );
					if( color != -1 )
					{
						color_widget.SetColor( color );
					}
					else
					{
						color_widget.SetColor( ColorManager.BASE_COLOR );
					}
				}
			}
		}
	}

	void PrepareTooltip(EntityAI item, int x = 0, int y = 0)
	{
		if( IsDragging() || !item )
		{
			return;
		}
		
		if ( item.IsInherited( InventoryItem ) )
		{
			HideTooltip();
			
			m_HoveredItem = item;
			InspectMenuNew.UpdateItemInfo( m_TooltipWidget, item );
			
			int screen_w, screen_h;
			float w, h;
			GetScreenSize(screen_w, screen_h);
			m_TooltipWidget.GetScreenSize(w,h);
			
			if (x == -1)//set by icon focusing
			{
				x = screen_w/2 - w/2;
				float x1,y1;
				m_RootWidget.FindAnyWidget("InventoryFrameWidget").GetScreenPos(x1,y1); //allign to the same height
				y = y1;
			}
			else if (x == 0 && y == 0 && GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
			{
				GetMousePos(x,y);
			}
			
			//minimal edge distance adjustments..
			screen_w -= 10;
			screen_h -= 10;
			
			int rightEdge = x + w;
			if (rightEdge > screen_w)
			{
				x = screen_w - w;
			}
			
			int bottomEdge = y + h;
			if (bottomEdge > screen_h)
			{
				y = screen_h - h;
			}
			
			m_TooltipWidget.SetPos(x, y);
			
			m_ToolTipTimer = new Timer();
			m_ToolTipTimer.Run( TOOLTIP_DELAY, this, "ShowTooltip" );
			
			Widget preview_frame = m_TooltipWidget.FindAnyWidget("ItemFrameWidget");
			if (preview_frame)
			{
				m_ItemPreviewWidget = ItemPreviewWidget.Cast( preview_frame );
				m_ItemPreviewWidget.SetItem(item);
				m_ItemPreviewWidget.SetView( item.GetViewIndex() );
			}
		}
	}
	
	//! position is currentlycalculated from the owning 'm_TooltipSourceWidget' directly
	void PrepareSlotsTooltip(string name, string desc, int x = 0, int y = 0)
	{
		InspectMenuNew.UpdateSlotInfo( m_TooltipSlotWidget, name, desc );
		
		HideTooltip();
		
		if (name != "") 
		{
			m_SlotInfoShown = true;
			//CalculateTooltipSlotPosition(x,y);
			m_TooltipSlotTimer = new Timer();
			m_TooltipSlotTimer.Run( TOOLTIP_DELAY, this, "ShowTooltipSlot" );
		}
	}
	
	void CalculateTooltipSlotPosition(int x = 0, int y = 0)
	{
		int screen_w, screen_h;
		float w, h;
		GetScreenSize(screen_w, screen_h);
		//m_TooltipSlotWidget.GetScreenSize(w,h);
		m_TooltipSlotWidget.GetSize(w,h);
		int slot_normal_w = SlotsIcon.GetNormalWidth();
		int slot_normal_h = SlotsIcon.GetNormalHeight();
		//minimal edge distance adjustments..
		screen_w -= 10;
		screen_h -= 10;
		
#ifndef PLATFORM_CONSOLE
		x += 5;
		y += slot_normal_h + 5;
#else
		x += 5;
		y += 15;
#endif
		Widget scrollerWidget = m_TooltipSourceWidget.GetParent();
		while (scrollerWidget)
		{
			if (ScrollWidget.Cast(scrollerWidget))
			{
				break;
			}
			else
			{
				scrollerWidget = scrollerWidget.GetParent();
			}
		}

		int rightEdge = x + w;
		if (rightEdge > screen_w)
		{
			x = screen_w - w;
		}

		int bottomEdge = y + h;
		if (scrollerWidget)
		{
			float scrollerX, scrollerY, scrollerSizeX, scrollerSizeY;
			scrollerWidget.GetScreenPos(scrollerX,scrollerY);
			scrollerWidget.GetScreenSize(scrollerSizeX,scrollerSizeY);
			
			int scroller_bottom = scrollerY + scrollerSizeY;
			if (bottomEdge > scroller_bottom)
			{
				y = scroller_bottom - slot_normal_h;
				bottomEdge = y + h;
			}
		}
		/*else
		{
			ErrorEx("No scroller widget! | m_TooltipSourceWidget: " + m_TooltipSourceWidget);
		}*/
		
		if (bottomEdge > screen_h) //should mostly never proc now
		{
			y = screen_h - h;
		}
		
		m_TooltipPosX = x;
		m_TooltipPosY = y;
	}
	
	void UpdateTooltipSlotPosition()
	{
		float x, y;
		m_TooltipSourceWidget.GetScreenPos(x,y);
		CalculateTooltipSlotPosition(x,y);
	}
	
	void SetTooltipWidget(Widget w)
	{
		m_TooltipSourceWidget = w;
	}
	
	bool EvaluateContainerDragabilityDefault(EntityAI entity)
	{
		if ( !ItemBase.Cast( entity ) )
			return false;
		
		bool draggable;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		draggable = !player.GetInventory().HasInventoryReservation( entity, null ) && !player.IsItemsToDelete();
		draggable = draggable && entity.CanPutIntoHands( GetGame().GetPlayer() );
		draggable = draggable && entity.GetInventory().CanRemoveEntity();
		
		return draggable;
	}
	
	void SetWidgetDraggable( Widget w, bool draggable )
	{
		if (w)
		{
			if ( draggable )
				w.SetFlags( WidgetFlags.DRAGGABLE );
			else
				w.ClearFlags( WidgetFlags.DRAGGABLE );
		}
	}
	
	void ShowTooltip()
	{	
		if( m_ItemPreviewWidget )
		{
			m_TooltipWidget.Show( true );
		}
	}
	
	void ShowTooltipSlot()
	{	
		UpdateTooltipSlotPosition();
		m_TooltipSlotWidget.SetPos(m_TooltipPosX, m_TooltipPosY);
		m_TooltipSlotWidget.Show( true );
	}
	
	static int GetCombinationFlags( EntityAI entity1, EntityAI entity2 )
	{
		int flags = 0;
		PlayerBase m_player = PlayerBase.Cast( GetGame().GetPlayer() );

		if( !entity1 || !entity2 )
			return flags;
		
		if( entity1.IsInherited( ItemBase ) && entity2.IsInherited( ItemBase ) )
		{
			ItemBase ent1 = ItemBase.Cast( entity1 );
			if( ent1.CanBeCombined( ItemBase.Cast( entity2 ) ) ) flags = flags | InventoryCombinationFlags.COMBINE_QUANTITY2;
		}

		Weapon_Base wpn;
		Magazine mag;
		if( Class.CastTo(wpn,  entity1 ) && Class.CastTo(mag,  entity2 ) )
		{
			
		}
		else if( entity1.GetInventory().CanAddAttachment( entity2 ) )
		{
			if( !entity1.IsInherited( ZombieBase ) && !entity1.IsInherited( Car ) && !entity2.IsInherited( ZombieBase ) && !entity2.IsInherited( Car ) )
			{
				flags = flags | InventoryCombinationFlags.ADD_AS_ATTACHMENT;
			}
		}
		if( entity1.GetInventory().CanAddEntityInCargo( entity2, entity2.GetInventory().GetFlipCargo() ) ) flags = flags | InventoryCombinationFlags.ADD_AS_CARGO;

		if( entity1 == m_player.GetHumanInventory().GetEntityInHands() || entity2 == m_player.GetHumanInventory().GetEntityInHands())
		{
			ActionManagerClient amc;
			Class.CastTo(amc, m_player.GetActionManager());
			if( entity1 == m_player.GetHumanInventory().GetEntityInHands() )
			{
				if( amc.CanPerformActionFromInventory( ItemBase.Cast( entity1 ), ItemBase.Cast( entity2 ) ) )
				{
					flags = flags | InventoryCombinationFlags.PERFORM_ACTION;
				}
				else if( amc.CanSetActionFromInventory( ItemBase.Cast( entity1 ), ItemBase.Cast( entity2 ) ) )
				{
					flags = flags | InventoryCombinationFlags.SET_ACTION;
				}
			}
			else
			{
				if( amc.CanPerformActionFromInventory( ItemBase.Cast( entity2 ), ItemBase.Cast( entity1 ) ) )
				{
					flags = flags | InventoryCombinationFlags.PERFORM_ACTION;
				}
				else if( amc.CanSetActionFromInventory( ItemBase.Cast( entity2 ), ItemBase.Cast( entity1 ) ) )
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
	
	static int GetRecipeCount( bool recipe_anywhere, EntityAI entity1, EntityAI entity2 )
	{
		PluginRecipesManager plugin_recipes_manager = PluginRecipesManager.Cast( GetPlugin( PluginRecipesManager ) );
		return plugin_recipes_manager.GetValidRecipes( ItemBase.Cast( entity1 ), ItemBase.Cast( entity2 ), NULL, PlayerBase.Cast( GetGame().GetPlayer() ) );
	}
}
