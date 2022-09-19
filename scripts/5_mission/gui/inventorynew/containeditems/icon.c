class Icon: LayoutHolder
{
	protected int					m_SizeX;
	protected int					m_SizeY;
	protected int					m_PosX;
	protected int					m_PosY;
	
	protected EntityAI				m_Lock;
	protected bool					m_IsWeapon			= false;
	protected bool					m_IsMagazine		= false;
	protected bool					m_HasTemperature	= false;
	protected bool					m_HasQuantity		= false;
	protected float					m_CurrQuantity		= -1;
	
	protected EntityAI				m_Obj;
	protected ItemBase				m_Item;
	protected bool					m_HandsIcon;
	protected int					m_CargoPos;
	protected bool					m_IsDragged;
	protected bool					m_PreviousFlipOrientation;
	
	const int NUMBER_OF_TIMERS = 2;
	
	protected ItemPreviewWidget		m_ItemPreview;
	
	protected Widget				m_ColorWidget;
	protected Widget				m_SelectedPanel;
	
	protected Widget				m_QuantityPanel;
	protected TextWidget			m_QuantityItem;
	protected ProgressBarWidget		m_QuantityProgress;
	protected Widget				m_QuantityStack;
	
	protected Widget				m_ItemSizePanel;
	protected TextWidget			m_ItemSizeWidget;
	
	protected ref array<ImageWidget>m_AmmoIcons;
	protected ImageWidget			m_AmmoTypeIcon;

	void Icon( LayoutHolder parent, bool hands_icon = false )
	{
		m_HandsIcon = hands_icon;
		ItemManager.GetInstance().SetSelectedItem( null, null, null, null );
		
		m_ItemPreview		= ItemPreviewWidget.Cast( GetMainWidget().FindAnyWidget( "Render" ) );
		
		m_ColorWidget		= GetMainWidget().FindAnyWidget( "Color" );
		m_SelectedPanel		= GetMainWidget().FindAnyWidget( "Selected" );
		
		m_QuantityPanel		= GetMainWidget().FindAnyWidget( "QuantityPanel" );
		m_QuantityItem		= TextWidget.Cast( GetMainWidget().FindAnyWidget( "Quantity" ) );
		m_QuantityProgress	= ProgressBarWidget.Cast( GetMainWidget().FindAnyWidget( "QuantityBar" ) );
		m_QuantityStack		= GetMainWidget().FindAnyWidget( "QuantityStackPanel" );
		
		m_ItemSizePanel		= GetMainWidget().FindAnyWidget( "ItemSizePanel" );
		m_ItemSizeWidget	= TextWidget.Cast( GetMainWidget().FindAnyWidget( "ItemSize" ) );
		
		m_AmmoTypeIcon		= ImageWidget.Cast( GetMainWidget().FindAnyWidget( "AmmoTypeIcon" ) );
		SetActive( false );
	}
	
	void ~Icon()
	{
		if( m_Obj )
		{
			m_Obj.GetOnItemFlipped().Remove( UpdateFlip );
			m_Obj.GetOnViewIndexChanged().Remove( SetItemPreview );
		}
		
		if( m_IsDragged)
		{
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().SetIsDragging( false );
			m_IsDragged = false;
		}
	}
	
	Widget GetSelectedWidget()
	{
		return m_SelectedPanel;
	}

	bool IsDragged()
	{
		return m_IsDragged;
	}

	override void SetActive( bool active )
	{
		super.SetActive( active );
		if( active && GetObject() )
		{
			float x, y;
			GetMainWidget().GetScreenPos( x, y );
			PrepareOwnedTooltip( EntityAI.Cast( GetObject() ), -1, y );
		}
		
		m_SelectedPanel.Show( active );
	}
	
	override void SetParentWidget()
	{
		#ifndef PLATFORM_CONSOLE
		if( m_Parent.IsInherited( HandsPreview ) )
		{ 
			super.SetParentWidget();
		}
		else
		{
			if( m_Parent != null )
			{
				CargoContainer grid_container = CargoContainer.Cast( m_Parent );
				if( grid_container )
				{
					m_ParentWidget = grid_container.GetMainWidget();
				}
			}
		}
		#else
		super.SetParentWidget();
		#endif
	}

	void RefreshQuickbar()
	{
		InventoryMenu menu = InventoryMenu.Cast( GetGame().GetUIManager().FindMenu( MENU_INVENTORY ) );
		HideOwnedTooltip();
		if( menu )
		{
			menu.RefreshQuickbar();
		}
	}

	void DoubleClick(Widget w, int x, int y, int button)
	{
		if( button == MouseState.LEFT )
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			if( player.GetInventory().HasInventoryReservation( m_Obj, null ) || player.GetInventory().IsInventoryLocked() || player.IsItemsToDelete() )
			{
				return;
			}
			
			InventoryMenu menu = InventoryMenu.Cast( GetGame().GetUIManager().FindMenu( MENU_INVENTORY ) );
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
	
			EntityAI item = iw.GetItem();
			
			if( !item.GetInventory().CanRemoveEntity() )
				return;
			
			if( m_HandsIcon )
			{
				if( player.GetHumanInventory().CanRemoveEntityInHands() )
				{
					player.PredictiveMoveItemFromHandsToInventory();
				}
				HideOwnedTooltip();
				if( menu )
				{
					menu.RefreshQuickbar();
				}
				return;
			}
	
			InventoryLocation i1 = new InventoryLocation;
			EntityAI hands_item = player.GetHumanInventory().GetEntityInHands();
			EntityAI item_root_owner = item.GetHierarchyRoot();
			
			
			if( player.GetInventory().HasEntityInInventory( item ) && player.GetHumanInventory().CanAddEntityInHands( item ) )
			{
				player.PredictiveTakeEntityToHands( item );
			}
			else if( hands_item && item_root_owner == GetGame().GetPlayer() )
			{
				int index = player.GetHumanInventory().FindUserReservedLocationIndex(hands_item);
				if(index>=0)
				{
					player.GetHumanInventory().GetUserReservedLocation( index, i1);
				}
				
				if( player.GetInventory().CanForceSwapEntitiesEx( item, null, hands_item, i1 ) )
				{
					player.PredictiveForceSwapEntities( item, hands_item, i1);
				}
				else if( player.GetInventory().CanSwapEntitiesEx( item, hands_item ) )
				{
					player.PredictiveSwapEntities( item, hands_item );
				}
			}
			else
			{
				bool found = false;
				if( item.GetInventory().CanRemoveEntity())
				{
					InventoryLocation i2 = new InventoryLocation;
					found = player.GetInventory().FindFreeLocationFor( item, FindInventoryLocationType.ANY, i2 );
					if( found )
					{
						if( i2.GetType() == FindInventoryLocationType.ATTACHMENT )
						{
							if( i2.GetParent() != player )
								found = false;
						}
					}
				}
				if( found )
				{
					if(player.GetHumanInventory().CanAddEntityToInventory(item))
						player.PredictiveTakeEntityToInventory(FindInventoryLocationType.ANY, item);
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
			if( menu )
			{
				menu.RefreshQuickbar();
			}
		}
	}

	void DraggingOverSwap( Widget w, int x, int y, Widget receiver )
	{
		if( w == null )
		{
			return;
		}
		
		string name = w.GetName();
		name.Replace( "PanelWidget", "Render" );

		ItemPreviewWidget receiver_ipw = ItemPreviewWidget.Cast( receiver.FindAnyWidget( "Render" ) );
		if( m_HandsIcon )
		{
			receiver_ipw = ItemPreviewWidget.Cast( receiver.GetParent().FindAnyWidget( "Render" ) );
		}

		ItemPreviewWidget w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		if( w_ipw == null )
		{
			w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		}
		
		if( w_ipw == null )
		{
			return;
		}

		InventoryItem receiver_entity = InventoryItem.Cast( receiver_ipw.GetItem() );
		InventoryItem w_entity = InventoryItem.Cast( w_ipw.GetItem() );
		if( !w_entity )
		{
			return;
		}
		
		InventoryLocation il = new InventoryLocation;
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		int index = player.GetHumanInventory().FindUserReservedLocationIndex( m_Item );
		
		if(index >= 0)
		{	
			player.GetHumanInventory().GetUserReservedLocation( index, il );
		}
		
		if( index < 0 && GameInventory.CanSwapEntitiesEx( receiver_entity, w_entity ) )
		{
			ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
		}
		else if( GameInventory.CanForceSwapEntitiesEx( w_entity, null, receiver_entity, il ) )
		{
			ColorManager.GetInstance().SetColor( w, ColorManager.FSWAP_COLOR );
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
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

	void DraggingOverCombine( Widget w, int x, int y, Widget receiver )
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

		int flags = ItemManager.GetCombinationFlags( GetGame().GetPlayer().GetHumanInventory().GetEntityInHands(), iw.GetItem() );
		ShowActionMenuCombine( GetGame().GetPlayer().GetHumanInventory().GetEntityInHands(), iw.GetItem(), flags , w , true);
	}

	bool MouseEnter(Widget w, int x, int y)
	{
		if( !m_IsDragged )
		{
			PrepareOwnedTooltip( m_Obj, x, y );
			m_SelectedPanel.Show( true );
		}
		
		return true;
	}

	bool MouseLeave( Widget w, Widget s, int x, int y	)
	{
		HideOwnedTooltip();
		if( !m_IsDragged )
		{
			m_SelectedPanel.Show( false );
		}
		return true;
	}

	void DraggingOver( Widget w, int x, int y, Widget receiver )
	{
		ItemManager.GetInstance().HideDropzones();
		
		if( w == null )
		{
			return;
		}
		string name = w.GetName();
		name.Replace( "PanelWidget", "Render" );

		ItemPreviewWidget receiver_ipw = ItemPreviewWidget.Cast( receiver.FindAnyWidget( "Render" ) );
		if( m_HandsIcon )
		{
			receiver_ipw = ItemPreviewWidget.Cast( receiver.GetParent().FindAnyWidget( "Render" ) );
		}

		ItemPreviewWidget w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		if( w_ipw == null )
		{
			w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		}
		if( w_ipw == null )
		{
			return;
		}

		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		InventoryItem receiver_entity = InventoryItem.Cast( receiver_ipw.GetItem() );
		InventoryItem w_entity = InventoryItem.Cast( w_ipw.GetItem() );
		InventoryLocation loc_dst;
		if( !receiver_entity || !w_entity )
		{
			return;
		}
		
		if( m_Lock )
		{
			if( receiver_entity == w_entity )
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
				ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
			}
			else
			{
				CargoContainer c_parent = CargoContainer.Cast( m_Parent );
			
				float parent_x;
				float parent_y;
			
			
				c_parent.GetRootWidget().GetScreenPos( parent_x, parent_y );
			
				float icon_size = c_parent.GetIconSize();
				float space_size = c_parent.GetSpaceSize();
			
				int PosX = (x-parent_x) / (icon_size + space_size);
				int PosY = (y-parent_y) / (icon_size + space_size);
			
				//Print("X: " + GetPosX() + " or " + PosX  + " ;Y: " + GetPosY() + " or " + PosY);
			
				EntityAI parent = m_Lock;
				CargoBase target_cargo 	= parent.GetInventory().GetCargo();
			
				loc_dst = new InventoryLocation();			
				loc_dst.SetCargoAuto(target_cargo, w_entity, PosY, PosX, w_entity.GetInventory().GetFlipCargo());
		
				if( parent.GetInventory().LocationCanAddEntity(loc_dst))
				{
					ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
					ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
				}
				else
				{
					ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
					ItemManager.GetInstance().ShowSourceDropzone( w_entity );
				}
			}
		}
		else if( ( ItemBase.Cast( receiver_entity ) ).CanBeCombined( ItemBase.Cast( w_entity ) ) )
		{
			ColorManager.GetInstance().SetColor( w, ColorManager.COMBINE_COLOR );
			if( m_Obj.GetHierarchyRootPlayer() == player )
			{
				ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
			}
			else if( !m_HandsIcon )
			{
				ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
			}
		}
		else
		{
			Magazine mag = Magazine.Cast(w_entity);
			Weapon_Base wpn = Weapon_Base.Cast(w_entity.GetHierarchyParent());
			EntityAI w_hierarchyParent = w_entity.GetHierarchyParent();
			EntityAI receiver_hierarchyParent = w_entity.GetHierarchyParent();
			if ( wpn && mag )
			{
				if( player.GetWeaponManager().CanSwapMagazine( wpn,  Magazine.Cast(receiver_entity) ) )
				{
					ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
					if( m_Obj.GetHierarchyRootPlayer() == player )
					{
						ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
					}
					else if( !m_HandsIcon )
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
			else if ((w_hierarchyParent && (w_hierarchyParent.IsInherited(FenceKit) || w_hierarchyParent.IsInherited(WatchtowerKit))) || (receiver_hierarchyParent && (receiver_hierarchyParent.IsInherited(FenceKit) || receiver_hierarchyParent.IsInherited(WatchtowerKit))))
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
				ItemManager.GetInstance().ShowSourceDropzone( w_entity );
			}
			else
			{
				InventoryLocation il = new InventoryLocation;
				if( GameInventory.CanSwapEntitiesEx( receiver_entity, w_entity ) )
				{
					ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
					if( m_Obj.GetHierarchyRootPlayer() == player )
					{
						ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
					}
					else if( !m_HandsIcon )
					{
						ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
					}
				}
				else if( GameInventory.CanForceSwapEntitiesEx( w_entity, null, receiver_entity, il ) )
					{
						ColorManager.GetInstance().SetColor( w, ColorManager.FSWAP_COLOR );
						if( m_Obj.GetHierarchyRootPlayer() == player )
						{
							ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
						}
						else if( !m_HandsIcon )
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
			}
		}

	void OnPerformCombination( int combinationFlags )
	{
		PlayerBase m_player = PlayerBase.Cast( GetGame().GetPlayer() );
		if( m_am_entity1 == null || m_am_entity2 == null ) return;

		if( combinationFlags == InventoryCombinationFlags.NONE ) return;

		Weapon_Base wpn;
		Magazine mag;

		if( combinationFlags & InventoryCombinationFlags.LOAD_CHAMBER )
		{
			if( Class.CastTo(wpn,  m_am_entity1 ) && Class.CastTo(mag,  m_am_entity2 ) )
			{
				if( m_player.GetWeaponManager().CanLoadBullet(wpn, mag) )
				{
					m_player.GetWeaponManager().LoadBullet(mag);
					return;
				}
			}
		}
		if( combinationFlags & InventoryCombinationFlags.ADD_AS_ATTACHMENT )
		{
			float stackable = m_am_entity2.GetTargetQuantityMax(-1);
		
			if ( stackable == 0 || stackable >= m_am_entity2.GetQuantity() )
			{
				m_player.PredictiveTakeEntityToTargetAttachment(m_am_entity1, m_am_entity2);
			}
			else
			{
				InventoryLocation il = new InventoryLocation;
				m_am_entity1.GetInventory().FindFreeLocationFor( m_am_entity2, FindInventoryLocationType.ATTACHMENT, il );
				ItemBase.Cast(m_am_entity2).SplitIntoStackMaxToInventoryLocationClient( il );
			}
		}
		if( combinationFlags & InventoryCombinationFlags.ADD_AS_CARGO )
		{
			SplitItemUtils.TakeOrSplitToInventory(m_player, m_am_entity1, m_am_entity2);
		}
		if( combinationFlags & InventoryCombinationFlags.SWAP )
		{
			if( !m_player.PredictiveSwapEntities( m_am_entity1, m_am_entity2 ) && m_player.GetHumanInventory().CanAddEntityInHands( m_am_entity2 ) )
			{
				m_player.PredictiveTakeEntityToHands( m_am_entity2 );
			}
		}
		if( combinationFlags & InventoryCombinationFlags.TAKE_TO_HANDS )
		{
			if( m_player.GetHumanInventory().CanAddEntityInHands( m_am_entity2 ) )
			{
				m_player.PredictiveTakeEntityToHands( m_am_entity2 );
			}
		}
		if( combinationFlags & InventoryCombinationFlags.PERFORM_ACTION )
		{
			ActionManagerClient amc;
			Class.CastTo(amc, m_player.GetActionManager());

			if( m_am_entity1 == m_player.GetHumanInventory().GetEntityInHands() )
			{
				amc.PerformActionFromInventory(ItemBase.Cast( m_am_entity1 ),ItemBase.Cast( m_am_entity2 ));
			}
			else
			{
				amc.PerformActionFromInventory(ItemBase.Cast( m_am_entity2 ),ItemBase.Cast( m_am_entity1 ));
			}
		}
		if( combinationFlags & InventoryCombinationFlags.SET_ACTION )
		{
			ActionManagerClient amc2;
			Class.CastTo(amc2, m_player.GetActionManager());

			if( m_am_entity1 == m_player.GetHumanInventory().GetEntityInHands() )
			{
				amc2.SetActionFromInventory(ItemBase.Cast( m_am_entity1 ),ItemBase.Cast( m_am_entity2 ));
			}
			else
			{
				amc2.SetActionFromInventory(ItemBase.Cast( m_am_entity2 ),ItemBase.Cast( m_am_entity1 ));
			}
		}
	}

	

	

	void ShowActionMenuCombine( EntityAI entity1, EntityAI entity2, int combinationFlags, Widget w , bool color_test )
	{
		int current_flag;
		ContextMenu cmenu = GetGame().GetUIManager().GetMenu().GetContextMenu();
		m_am_entity1 = entity1;
		m_am_entity2 = entity2;
		cmenu.Hide();
		cmenu.Clear();
		int id = -1;

		if( entity1 == null || entity2 == null ) return;

		if( combinationFlags == InventoryCombinationFlags.NONE )
		{
			if( color_test )
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
				ItemManager.GetInstance().ShowSourceDropzone( entity2 );
			}
			return;
		}

		if( combinationFlags & InventoryCombinationFlags.ADD_AS_ATTACHMENT )
		{
			current_flag = InventoryCombinationFlags.ADD_AS_ATTACHMENT;
			cmenu.Add( "#inv_context_add_as_attachment", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}
		if( combinationFlags & InventoryCombinationFlags.LOAD_CHAMBER )
		{
			current_flag = InventoryCombinationFlags.LOAD_CHAMBER;
			cmenu.Add( "#inv_context_load_chamber", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}
		if(combinationFlags & InventoryCombinationFlags.ATTACH_MAGAZINE)
		{
			current_flag = InventoryCombinationFlags.ATTACH_MAGAZINE;
			cmenu.Add("#inv_context_attach_magazine", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		if( combinationFlags & InventoryCombinationFlags.ADD_AS_CARGO && !(combinationFlags & InventoryCombinationFlags.SET_ACTION) )
		{
			current_flag = InventoryCombinationFlags.ADD_AS_CARGO;
			cmenu.Add( "#inv_context_add_as_cargo", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		if( combinationFlags & InventoryCombinationFlags.SWAP )
		{
			current_flag = InventoryCombinationFlags.SWAP;
			cmenu.Add( "#inv_context_swap", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		if( combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY )
		{
			current_flag = InventoryCombinationFlags.COMBINE_QUANTITY;
			cmenu.Add( "#inv_context_combine", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		if(combinationFlags & InventoryCombinationFlags.SET_ACTION)
		{
			current_flag = InventoryCombinationFlags.SET_ACTION;
			cmenu.Add("#inv_context_attach_magazine", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}
		
		if(combinationFlags & InventoryCombinationFlags.PERFORM_ACTION)
		{
			current_flag = InventoryCombinationFlags.PERFORM_ACTION;
			cmenu.Add("Perform action", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		int m_am_Pos_x,  m_am_Pos_y;
		GetMousePos( m_am_Pos_x, m_am_Pos_y );
		m_am_Pos_x -= 5;
		m_am_Pos_y -= 5;

		if( color_test )
		{
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
			ColorManager.GetInstance().SetColor( w, ColorManager.COMBINE_COLOR );
		}
		/*if( combinationFlags & InventoryCombinationFlags.RECIPE_HANDS ||  combinationFlags & InventoryCombinationFlags.RECIPE_ANYWHERE )
		{
			if( !color_test )
			{
				OnPerformRecipe( id );
				return;
			}
		}
		else*/ if( cmenu.Count() >= 1 )
		{
			if( !color_test )
			{
				OnPerformCombination( current_flag );
				return;
			}
		}
		else if( combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY2 )
		{
			if( color_test )
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.COMBINE_COLOR );
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
			}
			else
			{
				ItemBase entity = ItemBase.Cast( entity1 );
				entity.CombineItemsClient( ItemBase.Cast( entity2 ) );
			}
			return;
		}
		else
		{
			if( color_test )
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
				ItemManager.GetInstance().ShowSourceDropzone( entity2 );
			}
		/*	else
			{
				cmenu.Show( m_am_Pos_x, m_am_Pos_y );
			}*/
		}
	}

	void OnPerformRecipe(int id)
	{
		if( m_am_entity1 == null || m_am_entity2 == null ) return;

		Debug.Log("OnPerformRecipe called for id:"+id.ToString(),"recipes");
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		player.GetCraftingManager().SetInventoryCraft( id, ItemBase.Cast( m_am_entity1 ), ItemBase.Cast( m_am_entity2 ) );
	}

	void Combine( Widget w, int x, int y, Widget receiver )
	{
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
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

		int flags = ItemManager.GetCombinationFlags( GetGame().GetPlayer().GetHumanInventory().GetEntityInHands(), iw.GetItem() );
		ShowActionMenuCombine( GetGame().GetPlayer().GetHumanInventory().GetEntityInHands(), iw.GetItem(), flags , w, false );
		
	}
	
	bool CombineItems( EntityAI entity1, EntityAI entity2 )
	{
		int flags = ItemManager.GetCombinationFlags( entity1, entity2 );
		return FlagAction( entity1, entity2, flags );
	}
	
	bool FlagAction( EntityAI entity1, EntityAI entity2, int combinationFlags )
	{
		int current_flag;
		ContextMenu cmenu = GetGame().GetUIManager().GetMenu().GetContextMenu();
		m_am_entity1 = entity1;
		m_am_entity2 = entity2;
		cmenu.Hide();
		cmenu.Clear();
		int id = -1;

		if( combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY2 )
		{
			ItemBase entity = ItemBase.Cast( entity1 );
			entity.CombineItemsClient( ItemBase.Cast( entity2 ) );
			return false;
		}

		if( entity1 == null || entity2 == null || combinationFlags == InventoryCombinationFlags.NONE )
			return true;

		if( combinationFlags & InventoryCombinationFlags.ADD_AS_ATTACHMENT )
		{
			current_flag = InventoryCombinationFlags.ADD_AS_ATTACHMENT;
			cmenu.Add( "#inv_context_add_as_attachment", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}
		if( combinationFlags & InventoryCombinationFlags.LOAD_CHAMBER )
		{
			current_flag = InventoryCombinationFlags.LOAD_CHAMBER;
			cmenu.Add( "#inv_context_load_chamber", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}
		if(combinationFlags & InventoryCombinationFlags.ATTACH_MAGAZINE)
		{
			current_flag = InventoryCombinationFlags.ATTACH_MAGAZINE;
			cmenu.Add("#inv_context_attach_magazine", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		if( combinationFlags & InventoryCombinationFlags.ADD_AS_CARGO )
		{
			current_flag = InventoryCombinationFlags.ADD_AS_CARGO;
			cmenu.Add( "#inv_context_add_as_cargo", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		if( combinationFlags & InventoryCombinationFlags.SWAP )
		{
			current_flag = InventoryCombinationFlags.SWAP;
			cmenu.Add( "#inv_context_swap", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		if( combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY )
		{
			current_flag = InventoryCombinationFlags.COMBINE_QUANTITY;
			cmenu.Add( "#inv_context_combine", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		if(combinationFlags & InventoryCombinationFlags.SET_ACTION)
		{
			current_flag = InventoryCombinationFlags.SET_ACTION;
			cmenu.Add("#inv_context_attach_magazine", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}
		
		if(combinationFlags & InventoryCombinationFlags.PERFORM_ACTION)
		{
			current_flag = InventoryCombinationFlags.PERFORM_ACTION;
			cmenu.Add("Perform Action2", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		int m_am_Pos_x,  m_am_Pos_y;
		GetMousePos( m_am_Pos_x, m_am_Pos_y );
		m_am_Pos_x -= 5;
		m_am_Pos_y -= 5;

		MissionGameplay mission = MissionGameplay.Cast( GetGame().GetMission() );
		/*if( combinationFlags & InventoryCombinationFlags.RECIPE_HANDS ||  combinationFlags & InventoryCombinationFlags.RECIPE_ANYWHERE )
		{
			OnPerformRecipe( id );
			return true;
		}
		else /*if( cmenu.Count() == 1 )*/
		//{
			OnPerformCombination( current_flag );
			return true;
		//}
		/*else
		{
			cmenu.Show( m_am_Pos_x, m_am_Pos_y );
			return true;
		}*/
	}

	void MouseClick( Widget w, int x, int y, int button )
	{
		if( button == MouseState.RIGHT )
		{
			if( m_Lock )
			{
				GetGame().GetPlayer().GetHumanInventory().ClearUserReservedLocationSynced( m_Item );
			}
			else
			{
				if( m_Item && m_Item.IsItemBase() )
				{
					m_Item.OnRightClick();
					
					if( m_HasQuantity )
						SetQuantity();
					#ifdef DEVELOPER
					if( GetDayZGame().IsLeftCtrlDown() )
						ShowActionMenu( m_Item );
					#endif
				}
			}
				
		}
		else if (!m_Lock)
		{
			if ( button == MouseState.MIDDLE )
			{
				InspectItem( m_Item );
			}
			else if ( button == MouseState.LEFT )
			{
				PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
				if (g_Game.IsLeftCtrlDown())
				{
					if ( player && player.CanDropEntity( m_Item ) )
					{
						if ( m_Item.GetTargetQuantityMax() < m_Item.GetQuantity() )
							m_Item.SplitIntoStackMaxClient( null, -1 );
						else
						{
							player.PhysicalPredictiveDropItem( m_Item );
						}
						ItemManager.GetInstance().SetWidgetDraggable( w, false );	
					}
				}
				else
				{
					InventoryLocation il = new InventoryLocation;
					m_Obj.GetInventory().GetCurrentInventoryLocation( il );
	
					bool draggable = !player.GetInventory().HasInventoryReservation( m_Obj, null ) && !player.GetInventory().IsInventoryLocked() && !player.IsItemsToDelete();
					draggable = draggable && ( m_Obj.GetHierarchyRoot() && m_Obj.GetInventory().CanRemoveEntity() || !m_Obj.GetHierarchyRoot() && AttachmentsOutOfReach.IsAttachmentReachable( m_Obj, "", il.GetSlot() ) );
			
					ItemManager.GetInstance().SetWidgetDraggable( w, draggable );				
				}
			}
		}
	}

	void DropReceivedFromMain( Widget w, int x, int y, Widget receiver )
	{
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
		string name = w.GetName();
		name.Replace( "PanelWidget", "Render" );
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		ItemPreviewWidget receiver_ipw = ItemPreviewWidget.Cast( receiver.FindAnyWidget( "Render" ) );
		if( m_HandsIcon )
		{
			receiver_ipw = ItemPreviewWidget.Cast( receiver.GetParent().FindAnyWidget( "Render" ) );
		}

		ItemPreviewWidget w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		if( w_ipw == null )
		{
			w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		}
		if( w_ipw == null )
		{
			return;
		}

		InventoryItem receiver_entity = InventoryItem.Cast( receiver_ipw.GetItem() );
		InventoryItem w_entity = InventoryItem.Cast( w_ipw.GetItem() );
		if( !w_entity )
		{
			return;
		}
		int index;
		InventoryLocation loc_src;
		InventoryLocation loc_dst;
		if( m_Lock && receiver_entity == w_entity )
		{
			index = player.GetHumanInventory().FindUserReservedLocationIndex( m_Item );
			loc_src = new InventoryLocation;
			loc_dst = new InventoryLocation;
			
			m_Item.GetInventory().GetCurrentInventoryLocation( loc_src );
			player.GetHumanInventory().GetUserReservedLocation( index, loc_dst );
			
			if(loc_dst.GetParent().GetInventory().LocationCanAddEntity(loc_dst))
			{
			
				player.GetHumanInventory().ClearUserReservedLocation( m_Item );
				player.PredictiveTakeToDst( loc_src, loc_dst );
				m_Item.GetOnReleaseLock().Invoke(m_Item);
			}
		}
		else if( m_Lock )
		{
			CargoContainer c_parent = CargoContainer.Cast( m_Parent );
			
			float parent_x;
			float parent_y;
			
			
			c_parent.GetRootWidget().GetScreenPos( parent_x, parent_y );
			
			float icon_size = c_parent.GetIconSize();
			float space_size = c_parent.GetSpaceSize();
			
			int PosX = (x-parent_x) / (icon_size + space_size);
			int PosY = (y-parent_y) / (icon_size + space_size);
			
			//Print("X: " + GetPosX() + " or " + PosX  + " ;Y: " + GetPosY() + " or " + PosY);
			
			EntityAI parent = m_Lock;
			CargoBase target_cargo 	= parent.GetInventory().GetCargo();
			
			loc_dst = new InventoryLocation();			
			loc_dst.SetCargoAuto(target_cargo, w_entity, PosY, PosX, w_entity.GetInventory().GetFlipCargo());
			
			
			if( parent.GetInventory().LocationCanAddEntity(loc_dst))
			{
				player.GetHumanInventory().ClearUserReservedLocation( m_Item );
				m_Item.GetOnReleaseLock().Invoke(m_Item);
				
				SplitItemUtils.TakeOrSplitToInventoryLocation( player, loc_dst);
			}
			
			//player.PredictiveTakeEntityToCargoEx( w_entity, loc_dst.GetIdx(), loc_dst.GetRow(), loc_dst.GetCol() );
		}
		else
		{
			if( ( ItemBase.Cast( receiver_entity ) ).CanBeCombined( ItemBase.Cast( w_entity ) ) )
			{
				( ItemBase.Cast( receiver_entity ) ).CombineItemsClient( ItemBase.Cast( w_entity ) );
			}
			else
			{
				Magazine mag = Magazine.Cast(w_entity);
				Weapon_Base wpn = Weapon_Base.Cast(w_entity.GetHierarchyParent());
				
				if( wpn && mag )
				{
					if( player.GetWeaponManager().CanSwapMagazine( wpn,  Magazine.Cast(receiver_entity) ) )
						player.GetWeaponManager().SwapMagazine( Magazine.Cast(receiver_entity) );
							//return;
				}
				else
				{
					if( GameInventory.CanSwapEntitiesEx( w_entity, receiver_entity ) )
					{
						player.PredictiveSwapEntities( w_entity, receiver_entity );
		
						Icon icon = ItemManager.GetInstance().GetDraggedIcon();
						if( m_Parent && m_Parent.IsInherited( IconsContainer ) )
						{
							ToRefresh( this, icon );
						}
					}
					else
					{	
						loc_src = new InventoryLocation;
						loc_dst = new InventoryLocation;
						
						InventoryLocation loc_temp = new InventoryLocation;
							
						w_entity.GetInventory().GetCurrentInventoryLocation(loc_src);
						w_entity.GetInventory().GetCurrentInventoryLocation(loc_temp);
						receiver_entity.GetInventory().GetCurrentInventoryLocation(loc_dst);
						
						loc_src.CopyLocationFrom(loc_dst,false);
						loc_src.SetFlip(w_entity.GetInventory().GetFlipCargo());
					
						loc_dst.CopyLocationFrom(loc_temp,false);
						
						if (w_entity.IsInherited(Rope) && loc_dst.GetType() == InventoryLocationType.ATTACHMENT)
							return;
				
				 		if( GameInventory.CanForceSwapEntitiesEx( w_entity, loc_src, receiver_entity, loc_dst ) )
						{
							player.PredictiveForceSwapEntities( w_entity, receiver_entity, loc_dst );
						}
					}
				}
			}
		}
			
	}

	void Swap( Widget w, int x, int y, Widget receiver )
	{
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
		string name = w.GetName();
		name.Replace( "PanelWidget", "Render" );

		ItemPreviewWidget receiver_ipw = ItemPreviewWidget.Cast( receiver.FindAnyWidget( "Render" ) );
		if( m_HandsIcon )
		{
			receiver_ipw = ItemPreviewWidget.Cast( receiver.GetParent().FindAnyWidget( "Render" ) );
		}

		ItemPreviewWidget w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		if( w_ipw == null )
		{
			w_ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		}
		if( w_ipw == null )
		{
			return;
		}

		InventoryItem receiver_entity = InventoryItem.Cast( receiver_ipw.GetItem() );
		InventoryItem w_entity = InventoryItem.Cast( w_ipw.GetItem() );
		if( !w_entity )
		{
			return;
		}

		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		EntityAI item_in_hands = player.GetHumanInventory().GetEntityInHands();
		Weapon_Base wpn;
		Magazine mag;

		if( Class.CastTo(wpn,  item_in_hands ) && Class.CastTo(mag,  w_entity ) && Class.CastTo(wpn,  mag.GetHierarchyParent() ) )
		{
			return;
		}

		InventoryLocation il_fswap = new InventoryLocation;
		Icon icon = ItemManager.GetInstance().GetDraggedIcon();
		
		int index = player.GetHumanInventory().FindUserReservedLocationIndex(receiver_entity);
		if(index>=0)
		{
			player.GetHumanInventory().GetUserReservedLocation( index, il_fswap);
		
			if( GameInventory.CanForceSwapEntitiesEx( w_entity, null, receiver_entity, il_fswap ))
			{
				if( m_HandsIcon && !player.GetInventory().HasInventoryReservation( item_in_hands, null ) && !player.IsItemsToDelete() )
				{
					GetGame().GetPlayer().PredictiveForceSwapEntities( w_entity, receiver_entity, il_fswap );
				}
			}
			else
			{
				if( GameInventory.CanSwapEntitiesEx( w_entity, receiver_entity  ) )
				{
					if( !player.GetInventory().HasInventoryReservation( item_in_hands, null ) && !player.IsItemsToDelete() )
					{
						GetGame().GetPlayer().PredictiveSwapEntities( w_entity, receiver_entity );
				
						if( m_Parent.IsInherited( IconsContainer ) )
						{
							ToRefresh( this, icon );
						}
					}
				}
			}
		}
		else
		{
			if( GameInventory.CanSwapEntitiesEx( w_entity, receiver_entity  ) )
			{
				if( !player.GetInventory().HasInventoryReservation( item_in_hands, null ) && !player.IsItemsToDelete() )
				{
					GetGame().GetPlayer().PredictiveSwapEntities( w_entity, receiver_entity );
				
					if( m_Parent.IsInherited( IconsContainer ) )
					{
						ToRefresh( this, icon );
					}
				}
			}
			else if( GameInventory.CanForceSwapEntitiesEx( w_entity, null, receiver_entity, il_fswap ))
			{
				if( m_HandsIcon && !player.GetInventory().HasInventoryReservation( item_in_hands, null ) && !player.IsItemsToDelete() )
				{
					GetGame().GetPlayer().PredictiveForceSwapEntities( w_entity, receiver_entity, il_fswap );
				}
			}
		}
	}

	void ToRefresh( Icon icon, Icon icon2 )
	{
		( IconsContainer.Cast( m_Parent ) ).RemoveItem( icon );
		( IconsContainer.Cast( m_Parent ) ).RemoveItem( icon2 );
	}

	override void SetLayoutName()
	{
		#ifdef PLATFORM_CONSOLE
			m_LayoutName = WidgetLayoutName.IconXbox;
		#else
			switch( InventoryMenu.GetWidthType() )
			{
				case ScreenWidthType.NARROW:
				{
					m_LayoutName = WidgetLayoutName.IconNarrow;
					break;
				}
				case ScreenWidthType.MEDIUM:
				{
					m_LayoutName = WidgetLayoutName.IconMedium;
					break;
				}
				case ScreenWidthType.WIDE:
				{
					m_LayoutName = WidgetLayoutName.IconWide;
					break;
				}
			}
		#endif
			
	}

	void SetSize( int x, int y )
	{
		m_SizeX = x;
		m_SizeY = y;
	}
	
	void SetPos( int x, int y )
	{
		m_PosX = x;
		m_PosY = y;
	}

	int GetCargoPos()
	{
		return m_CargoPos;
	}

	void SetCargoPos( int x )
	{
		GetMainWidget().SetSort( x );
		m_CargoPos = x;
	}

	void DestroyWhiteBackground()
	{
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
		m_IsDragged = false;
		
		if ( m_Item )
		{
			m_Item.GetInventory().SetFlipCargo( m_PreviousFlipOrientation );
			int ww, hh;
			GetGame().GetInventoryItemSize( m_Item, ww, hh );
			
			if ( m_PreviousFlipOrientation )
				SetSize( hh, ww );
			else
				SetSize( ww, hh );

			SetSize();
		}
		
		if ( m_HandsIcon )
		{
			m_ItemPreview.SetForceFlipEnable(true);
			m_ItemPreview.SetForceFlip(false);
			Refresh();
			FullScreen();
		}
		
		m_SelectedPanel.SetColor( ARGBF( 1, 1, 1, 1 ) );
		m_SelectedPanel.Show( false );

		InventoryMenu menu = InventoryMenu.Cast( GetGame().GetUIManager().FindMenu( MENU_INVENTORY ) );
		
		if ( menu )
			menu.RefreshQuickbar();
	}

	void CreateWhiteBackground()
	{
		m_PreviousFlipOrientation = m_Item.GetInventory().GetFlipCargo();
		ItemManager.GetInstance().SetDraggedIcon(this);
		ItemManager.GetInstance().SetDraggedItem(m_Item);
		m_IsDragged = true;
		ItemManager.GetInstance().SetIsDragging( true );
		int ww, hh;
		GetGame().GetInventoryItemSize( m_Item, ww, hh );
		if ( m_Item.GetInventory().GetFlipCargo() )
			SetSize( hh, ww );
		else
			SetSize( ww, hh );
		
		SetSize();

		if ( !m_HandsIcon )
		{
			Refresh();
		}
		else
		{
			m_ItemPreview.SetForceFlipEnable(false);
			m_ColorWidget.SetColor( ColorManager.BASE_COLOR );
			m_ColorWidget.SetAlpha( 0.1 );
		}
		
		m_SelectedPanel.Show( true );
	}

	void OnDraggingOverBackground( Widget w, int x, int y, Widget reciever  )
	{
		ItemManager.GetInstance().HideDropzones();
		EntityAI entity = EntityAI.Cast( m_Obj );
		EntityAI owner = entity.GetHierarchyParent();
		if( owner && owner.GetHierarchyRootPlayer() == GetGame().GetPlayer() )
		{
			ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
		}
		else if( !m_HandsIcon )
		{
			ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
		}

		if( w == null || reciever == null )
		{
			return;
		}
		Pos pa;
		reciever.GetUserData( pa );
		if( m_Parent )
		{
			ContainerWithCargo item = ContainerWithCargo.Cast( m_Parent.m_Parent );
			if( item )
			{
				item.DraggingOverGrid( w, m_PosY + pa.y, m_PosX + pa.x, reciever );
			}
			HandsContainer hands_item = HandsContainer.Cast( m_Parent.m_Parent );
			if( hands_item )
			{
				hands_item.DraggingOverGrid( w, m_PosY + pa.y, m_PosX + pa.x, reciever, null );
			}
		}
	}

	void DropReceivedFromBackground( Widget w, int x, int y, Widget reciever  )
	{
		Pos pa;
		if( reciever )
		{
			reciever.GetUserData( pa );
			if( m_Parent )
			{
				ContainerWithCargo item = ContainerWithCargo.Cast( m_Parent.m_Parent );
				if( item )
				{
					item.DropReceived(w, m_PosY + pa.y, m_PosX + pa.x );
				}
			}
		}
	}

	override void Refresh()
	{
		super.Refresh();

		if ( !m_HandsIcon )
			SetPos();
		
		if ( m_HasTemperature )
			SetTemperature();
		
		if ( m_IsWeapon )
			RefreshMuzzleIcon();
		
		if ( m_HasQuantity )
			SetQuantity();
	}

	void SetTemperature()
	{
		ItemManager.GetInstance().SetIconTemperature( EntityAI.Cast( m_Obj ), m_RootWidget );
	}

	void RefreshIconPos()
	{
		Refresh();
		GetMainWidget().Update();
	}

	void FullScreen()
	{
		if( m_IsDragged )
		{
			return;
		}
		GetRootWidget().ClearFlags( WidgetFlags.HEXACTSIZE + WidgetFlags.VEXACTSIZE );
		GetRootWidget().SetSize( 1, 1 );
		m_ColorWidget.SetColor( ARGB( 0, 0, 0, 0 ) );
	}

	void RefreshPos( int row, int column )
	{
		if( row != m_PosY || column != m_PosX )
		{
			m_PosX = column;
			m_PosY = row;
			SetPos();
			RefreshIconPos();
		}
	}

	Object GetObject()
	{
		return m_Obj;
	}
	
	void RefreshMuzzleIcon()
	{
		Weapon_Base wpn = Weapon_Base.Cast( GetObject() );
		if( wpn )
		{
			int i;
			ImageWidget ammo_icon;
			if (!wpn.IsShowingChamberedBullet())
			{
				for (i = 0; i < m_AmmoIcons.Count(); i++)
				{
					ammo_icon = m_AmmoIcons.Get(i);
					ammo_icon.Show(false);
				}
			/*	int bullet_count = 0;
				int fireout_count = 0;
				
				for ( i = 0; i < wpn.GetMuzzleCount(); i++ )
				{
					if (wpn.IsChamberFull(i))
					{
						if (wpn.IsChamberFiredOut(i))
						{
							fireout_count++;
						}
						else
						{
							bullet_count++;
						}
					}
				}
				
				int j;
				i = 0;
				for ( j= 0; j < bullet_count; j++ )
				{
					if ( i > m_AmmoIcons.Count() )
					{
						//add plus ammo
						break;
					}
					
					ammo_icon = m_AmmoIcons.Get(i);
					ammo_icon.Show( true );
					ammo_icon.SetImage( 0 );
					i++;
				}
				
				for ( j= 0; j < fireout_count; j++ )
				{
					if ( i > m_AmmoIcons.Count() )
					{
						//add plus ammo
						break;
					}
					
					ammo_icon = m_AmmoIcons.Get(i);
					ammo_icon.Show( true );
					ammo_icon.SetImage( 1 );
					i++;
				}
				
				for (j = i; j < m_AmmoIcons.Count(); j++)
				{
					ammo_icon = m_AmmoIcons.Get(j);
					ammo_icon.Show(false);
				}*/
			}
			else
			{
				//TODO MW - add more conplex logic
				for ( i = 0; i < wpn.GetMuzzleCount(); i++ )
				{
					if ( i > m_AmmoIcons.Count() )
					{
						//add plus ammo
						break;
					}
					
					ammo_icon = m_AmmoIcons.Get(i);
					
					if( wpn.IsChamberFull( i ) )
					{
						if( wpn.IsJammed() )
						{
							ammo_icon.Show( true );
							ammo_icon.SetImage( 2 );
						}
						else if( wpn.IsChamberFiredOut( i ) )
						{
							ammo_icon.Show( true );
							ammo_icon.SetImage( 1 );
						}
						else
						{
							ammo_icon.Show( true );
							ammo_icon.SetImage( 0 );
						}
					}
					else
					{
						ammo_icon.Show( false );
					}
				}
			}
		}
	}
	
	void SetQuantity()
	{
		if (m_Item)
		{
			int quantityType = QuantityConversions.HasItemQuantity(m_Item);
			if (quantityType != QUANTITY_HIDDEN && m_CurrQuantity != QuantityConversions.GetItemQuantity(m_Item))
			{
				m_CurrQuantity = QuantityConversions.GetItemQuantity(m_Item);
			
				if (quantityType == QUANTITY_COUNT)
				{
					string q_text = QuantityConversions.GetItemQuantityText(m_Item);

					if (QuantityConversions.GetItemQuantityMax(m_Item) == 1 || q_text == "")
					{
						m_QuantityStack.Show(false);
					}
					else
					{
						m_QuantityItem.SetText(q_text);
						m_QuantityStack.Show(true);
					}

					m_QuantityProgress.Show(false);
				}
				else if (quantityType == QUANTITY_PROGRESS)
				{	
					float progress_max	= m_QuantityProgress.GetMax();
					int max 			= m_Item.GetQuantityMax();
					int count 			= m_Item.ConfigGetInt("count");
					float quantity		= m_CurrQuantity;
	
					if (count > 0)
					{
						max = count;
					}
					
					if (max > 0)
					{
						float value = Math.Round((quantity / max) * 100);
						m_QuantityProgress.SetCurrent(value);
					}

					m_QuantityStack.Show(false);
					m_QuantityProgress.Show(true);
				}
			}
		}
	}
	
	void SetSort( int index )
	{
		GetMainWidget().SetSort( index );
		GetMainWidget().Update();
	}
	
	int GetSort()
	{
		return GetMainWidget().GetSort( );
	}
	
	void SetItemPreviewEx(bool refresh = true)
	{
		m_ItemPreview.Show( true, refresh );
		m_ItemPreview.SetItem( EntityAI.Cast( m_Obj ) );
		m_ItemPreview.SetModelOrientation( "0 0 0" );
		m_ItemPreview.SetView( m_Obj.GetViewIndex() );
	}
	
	void SetItemPreview()
	{
		SetItemPreviewEx();
	}
	
	void SetItemSizeEx(bool refresh = true)
	{
		#ifdef PLATFORM_CONSOLE
		m_ItemSizePanel.Show( true, refresh );
		m_ItemSizeWidget.Show( true, refresh );
		
		int size_x, size_y;
		GetGame().GetInventoryItemSize( m_Item, size_x, size_y );
		int capacity = size_x * size_y;
		m_ItemSizeWidget.SetText( capacity.ToString() );
		#endif
	}
	
	void SetItemSize()
	{
		SetItemSizeEx();
	}
	
	void UpdateFlip( bool flipped )
	{
		int size_x, size_y;
		GetGame().GetInventoryItemSize( m_Item, size_x, size_y );
		
		if ( flipped )
			SetSize( size_y, size_x );
		else
			SetSize( size_x, size_y );

		SetSize();
	}
	
	void InitLock( EntityAI parent, EntityAI obj, int x_pos, int y_pos, bool flip)
	{
		m_Lock	= parent;
		m_Obj	= obj;
		m_Item	= ItemBase.Cast( m_Obj );
		
		SetPos( x_pos, y_pos );
		UpdateFlip( flip );
		
		ItemManager.GetInstance().SetWidgetDraggable( GetMainWidget(), false );
		WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown( GetMainWidget(), this, "MouseClick" );
		WidgetEventHandler.GetInstance().RegisterOnDropReceived( GetMainWidget(),  this, "DropReceivedFromMain" );
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver( GetMainWidget(),  this, "DraggingOver" );
		
		m_RootWidget.FindAnyWidget( "Reserved" ).Show( true );
		m_ItemPreview.SetForceFlipEnable(true);
		m_ItemPreview.SetForceFlip(flip);
		SetItemPreview();
		Refresh();
	}

	void InitEx( EntityAI obj, bool refresh = true )
	{
		if ( obj != m_Obj )
		{
			if ( m_Obj )
			{
				m_Obj.GetOnItemFlipped().Remove( UpdateFlip );
				m_Obj.GetOnViewIndexChanged().Remove( SetItemPreview );
			}
			if ( obj )
			{
				obj.GetOnItemFlipped().Insert( UpdateFlip );
				obj.GetOnViewIndexChanged().Insert( SetItemPreview );
			}
		}
		
		if (m_HandsIcon)
		{
			m_ItemPreview.SetForceFlipEnable(true);
			m_ItemPreview.SetForceFlip(false);
		}
		
		m_Obj	= obj;
		m_Item	= ItemBase.Cast( m_Obj );
		m_Lock = null;
		
		SetItemPreviewEx(refresh);
		
		WidgetEventHandler.GetInstance().RegisterOnDrag( GetMainWidget(),  this, "CreateWhiteBackground" );
		WidgetEventHandler.GetInstance().RegisterOnDrop( GetMainWidget(),  this, "DestroyWhiteBackground" );
		WidgetEventHandler.GetInstance().RegisterOnDropReceived( GetMainWidget(),  this, "DropReceivedFromMain" );
		WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown( GetMainWidget(),  this, "MouseClick" );
		WidgetEventHandler.GetInstance().RegisterOnDropReceived( GetMainWidget().FindAnyWidget( "Swap" ),  this, "Swap" );
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver( GetMainWidget().FindAnyWidget( "Swap" ),  this, "DraggingOverSwap" );
		WidgetEventHandler.GetInstance().RegisterOnDropReceived( GetMainWidget().FindAnyWidget( "Combine" ),  this, "Combine" );
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver( GetMainWidget().FindAnyWidget( "Combine" ),  this, "DraggingOverCombine" );
		WidgetEventHandler.GetInstance().RegisterOnDraggingOver( GetMainWidget(),  this, "DraggingOver" );
		WidgetEventHandler.GetInstance().RegisterOnMouseEnter( GetMainWidget(),  this, "MouseEnter" );
		WidgetEventHandler.GetInstance().RegisterOnMouseLeave( GetMainWidget(),  this, "MouseLeave" );
		WidgetEventHandler.GetInstance().RegisterOnDoubleClick( GetMainWidget(),  this, "DoubleClick" );
		
		SetItemSizeEx(refresh);
		CheckIsWeapon();
		CheckIsMagazineEx(refresh);
		CheckHasTemperature();
		CheckHasQuantityEx(refresh);
		m_RootWidget.FindAnyWidget( "Reserved" ).Show( false, refresh );
		
		if (refresh)
			Refresh();
	}
	
	void Init( EntityAI obj )
	{
		InitEx( obj );
	}
	
	void CheckIsWeapon()
	{
		Weapon_Base wpn = Weapon_Base.Cast( m_Obj );
		if ( wpn )
		{
			m_AmmoIcons = new array<ImageWidget>;
			m_IsWeapon = true;
			float x_pos = 0.0;
			float widht = 0.0, height = 0.0; 
			for (int i = 0; i < wpn.GetMuzzleCount(); i++)
			{
				if (i == 1)
				{
					m_AmmoIcons[0].GetSize(widht,height);
				}
				x_pos += widht;
				
				Widget ammo_icon = Widget.Cast( GetGame().GetWorkspace().CreateWidgets( "gui/layouts/inventory_new/ammo_icon.layout", GetMainWidget() ) );
				ammo_icon.SetPos(x_pos, 0.0, false);
				
				ImageWidget ammo_icon_img = ImageWidget.Cast(ammo_icon.GetChildren());				
				
				AmmoData data = Magazine.GetAmmoData( wpn.GetChamberAmmoTypeName( i ) );
				if ( data )
				{
					CartridgeType c_type = data.m_CartridgeType;
					switch ( c_type )
					{
						case CartridgeType.Pistol:
						{
							ammo_icon_img.LoadImageFile( 0, "set:dayz_gui image:cartridge_pistol" );
							ammo_icon_img.LoadImageFile( 1, "set:dayz_gui image:shell_pistol" );
							ammo_icon_img.LoadImageFile( 2, "set:dayz_gui image:jam_pistol" );
							break;
						}
						case CartridgeType.Intermediate:
						{
							ammo_icon_img.LoadImageFile( 0, "set:dayz_gui image:cartridge_int" );
							ammo_icon_img.LoadImageFile( 1, "set:dayz_gui image:shell_int" );
							ammo_icon_img.LoadImageFile( 2, "set:dayz_gui image:jam_int" );
							break;
						}
						case CartridgeType.FullPower:
						{
							ammo_icon_img.LoadImageFile( 0, "set:dayz_gui image:cartridge_fp" );
							ammo_icon_img.LoadImageFile( 1, "set:dayz_gui image:shell_fp" );
							ammo_icon_img.LoadImageFile( 2, "set:dayz_gui image:jam_fp" );
							break;
						}
						case CartridgeType.Shell:
						{
							ammo_icon_img.LoadImageFile( 0, "set:dayz_gui image:cartridge_shell" );
							ammo_icon_img.LoadImageFile( 1, "set:dayz_gui image:shell_shell" );
							ammo_icon_img.LoadImageFile( 2, "set:dayz_gui image:jam_shell" );
							break;
						}
					}
				}
				m_AmmoIcons.Insert(ammo_icon_img);
			}
		}
		else
		{
			m_IsWeapon = false;
		}			
	}
	
	void CheckIsMagazineEx( bool refresh = true )
	{
		Magazine mag = Magazine.Cast( m_Obj );
		if ( mag )
		{
			m_IsMagazine = true;
			AmmoData data = Magazine.GetAmmoData( mag.ClassName() );
			if ( data )
			{
				ProjectileType p_type = data.m_ProjectileType;
				switch (p_type)
				{
					case ProjectileType.None:
					{
						m_AmmoTypeIcon.Show( false, refresh );
						break;
					}
					case ProjectileType.Tracer:
					{
						m_AmmoTypeIcon.LoadImageFile( 0, "set:dayz_gui image:tracer" );
						m_AmmoTypeIcon.Show( true, refresh );
						break;
					}
					case ProjectileType.AP:
					{
						m_AmmoTypeIcon.LoadImageFile( 0, "set:dayz_gui image:armor_piercing" );
						m_AmmoTypeIcon.Show( true, refresh );
						break;
					}
				}
			}
		}
		else
		{
			m_IsMagazine = false;
		}
	}
	
	void CheckIsMagazine()
	{
		CheckIsMagazineEx();
	}
	
	void CheckHasTemperature()
	{
		if ( m_Item )
		{
			m_HasTemperature = ( m_Item.GetTemperatureMax() != 0 && m_Item.GetTemperatureMin() != 0 );
		}
	}
	
	void CheckHasQuantityEx(bool refresh = true)
	{
		if ( m_Item )
		{
			m_HasQuantity = ( QuantityConversions.HasItemQuantity( m_Item ) != QUANTITY_HIDDEN );
			
			if ( m_HasQuantity )
				m_QuantityPanel.Show( true, refresh );
		}
	}
	
	void CheckHasQuantity()
	{
		CheckHasQuantityEx();
	}
	
	void SetPosX( int x )
	{
		m_PosX = x;
	}
	
	void SetPosY( int y )
	{
		m_PosY = y;
	}
	
	void SetSizeX( int x )
	{
		m_SizeX = x;
	}
	
	void SetSizeY( int y )
	{
		m_SizeY = y;
	}
	
	int GetPosX()
	{
		return m_PosX;
	}
	
	int GetPosY()
	{
		return m_PosY;
	}
	
	int GetSizeX()
	{
		return m_SizeX;
	}
	
	int GetSizeY()
	{
		return m_SizeY;
	}

	void SetPosEx( bool refresh = true )
	{
		CargoContainer c_parent = CargoContainer.Cast( m_Parent );
		HandsPreview h_parent = HandsPreview.Cast( m_Parent );
		float icon_size, space_size;
		if ( c_parent )
		{
			icon_size = c_parent.GetIconSize();
			space_size = c_parent.GetSpaceSize();
		}
		else if ( h_parent )
		{
			icon_size = h_parent.GetIconSize();
			GetRootWidget().SetFlags( WidgetFlags.EXACTSIZE, refresh );
		}
		
		#ifndef PLATFORM_CONSOLE
			GetRootWidget().SetPos( icon_size * GetPosX() + ( GetPosX() + 1 ) * space_size, icon_size * GetPosY() + ( GetPosY() + 1 ) * space_size, refresh );
			GetRootWidget().SetSize( icon_size * m_SizeX + ( m_SizeX ) * space_size, icon_size * m_SizeY + ( m_SizeY ) * space_size, refresh );
		#else
			int row = m_CargoPos / 5;
			int column = m_CargoPos % 5;
			GetRootWidget().SetPos( icon_size * column, icon_size * row, refresh );
			GetRootWidget().SetSize( icon_size, icon_size, refresh );
		#endif
	}
	
	void SetPos()
	{
		SetPosEx();
	}
	
	void SetSize()
	{
		CargoContainer c_parent = CargoContainer.Cast( m_Parent );
		HandsPreview h_parent = HandsPreview.Cast( m_Parent );
		float icon_size, space_size;
		if( c_parent )
		{
			icon_size = c_parent.GetIconSize();
			space_size = c_parent.GetSpaceSize();
		}
		else if( h_parent )
		{
			icon_size = h_parent.GetIconSize();
			GetRootWidget().SetFlags( WidgetFlags.EXACTSIZE );
		}
		
		#ifndef PLATFORM_CONSOLE
			GetRootWidget().SetSize( icon_size * m_SizeX + ( m_SizeX ) * space_size, icon_size * m_SizeY + ( m_SizeY ) * space_size );
		#else
			GetRootWidget().SetSize( icon_size, icon_size );
		#endif
	}
	
	override void UpdateInterval()
	{
		if ( m_Item )
		{
			if ( m_HasTemperature )
				SetTemperature();
			
			if ( m_IsWeapon )
				RefreshMuzzleIcon();
			
			if ( m_HasQuantity )
				SetQuantity();
		}
	}
}

class Pos
{
	int x, y;

	void Pos( int _x, int _y )
	{
		x = _x;
		y = _y;
	}
}
