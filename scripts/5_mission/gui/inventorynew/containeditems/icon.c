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
	protected Widget				m_MicromanagedPanel;
	protected Widget				m_CursorWidget;
	
	protected Widget				m_QuantityPanel;
	protected TextWidget			m_QuantityItem;
	protected ProgressBarWidget		m_QuantityProgress;
	protected Widget				m_QuantityStack;
	
	protected Widget				m_ItemSizePanel;
	protected TextWidget			m_ItemSizeWidget;
	
	protected ref array<ImageWidget>	m_AmmoIcons;
	protected ImageWidget				m_AmmoTypeIcon;

	void Icon( LayoutHolder parent, bool hands_icon = false )
	{
		m_HandsIcon = hands_icon;
		ItemManager.GetInstance().SetSelectedItemEx(null, null, null);
		
		m_ItemPreview		= ItemPreviewWidget.Cast( GetMainWidget().FindAnyWidget( "Render" ));
		
		m_ColorWidget		= GetMainWidget().FindAnyWidget( "Color" );
		m_SelectedPanel		= GetMainWidget().FindAnyWidget( "Selected" );
		m_MicromanagedPanel	= GetMainWidget().FindAnyWidget( "Micromanaged" );
		m_CursorWidget		= GetMainWidget().FindAnyWidget( "Cursor" );
		
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
		if (m_Obj)
		{
			m_Obj.GetOnItemFlipped().Remove(UpdateFlip);
			m_Obj.GetOnViewIndexChanged().Remove(SetItemPreview);
		}
		
		if (m_IsDragged)
		{
			RevertToOriginalFlip();
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().SetIsDragging(false);
			m_IsDragged = false;
		}
	}
	
	Widget GetSelectedWidget()
	{
		return m_SelectedPanel;
	}
	
	Widget GetCursorWidget()
	{
		return m_CursorWidget;
	}
	
	Widget GetMicromanagedPanel()
	{
		return m_MicromanagedPanel;
	}

	bool IsDragged()
	{
		return m_IsDragged;
	}

	override void SetActive( bool active )
	{
		super.SetActive(active);
		if (active && GetObject())
		{
			float x, y;
			GetMainWidget().GetScreenPos(x, y);
			PrepareOwnedTooltip(EntityAI.Cast( GetObject() ), -1, y);
		}
		else
		{
			HideOwnedTooltip();
		}
		
		m_SelectedPanel.Show(active);
	}
	
	override void SetParentWidget()
	{
		#ifndef PLATFORM_CONSOLE
		if (m_Parent.IsInherited(HandsPreview))
		{ 
			super.SetParentWidget();
		}
		else
		{
			if (m_Parent != null)
			{
				CargoContainer gridContainer = CargoContainer.Cast(m_Parent);
				if (gridContainer)
				{
					m_ParentWidget = gridContainer.GetMainWidget();
				}
			}
		}
		#else
		super.SetParentWidget();
		#endif
	}
	
	int GetRelevantInventoryAction(int relevantActions)
	{
		return 0;
	
	}

	void RefreshQuickbar()
	{
		InventoryMenu menu = InventoryMenu.Cast(g_Game.GetUIManager().FindMenu(MENU_INVENTORY));
		HideOwnedTooltip();
		if (menu)
		{
			menu.RefreshQuickbar();
		}
	}

	void DoubleClick(Widget w, int x, int y, int button)
	{
		if (button == MouseState.LEFT && !g_Game.IsLeftCtrlDown())
		{
			PlayerBase controlledPlayer = PlayerBase.Cast(g_Game.GetPlayer());
			if (controlledPlayer.GetInventory().HasInventoryReservation(m_Obj, null) || controlledPlayer.GetInventory().IsInventoryLocked() || controlledPlayer.IsItemsToDelete())
				return;
			
			ItemPreviewWidget targetIpw = ItemPreviewWidget.Cast(w.FindAnyWidget("Render"));
			if (!targetIpw)
			{
				string name = w.GetName();
				name.Replace("PanelWidget", "Render");
				targetIpw = ItemPreviewWidget.Cast(w.FindAnyWidget(name));
			}
			
			if (!targetIpw)
				targetIpw = ItemPreviewWidget.Cast(w);
	
			EntityAI targetEntity = targetIpw.GetItem();
			if (targetIpw)
			{
				if (!targetEntity.GetInventory().CanRemoveEntity())
					return;
				
				if (m_HandsIcon)
				{
					if (controlledPlayer.GetHumanInventory().CanRemoveEntityInHands())
						controlledPlayer.PredictiveMoveItemFromHandsToInventory();
	
					RefreshQuickbar();
	
					return;
				}
		
				EntityAI entityInHands = controlledPlayer.GetEntityInHands();
				EntityAI entityRootParent = targetEntity.GetHierarchyRoot();
				
				if (controlledPlayer.GetInventory().HasEntityInInventory(targetEntity) && controlledPlayer.GetHumanInventory().CanAddEntityInHands(targetEntity))
				{
					controlledPlayer.PredictiveTakeEntityToHands(targetEntity);
				}
				else if (entityInHands && entityRootParent == controlledPlayer)
				{
					InventoryLocation targetItemLocation = new InventoryLocation();
					InventoryLocation handsItemLocation = new InventoryLocation();
					entityInHands.GetInventory().GetCurrentInventoryLocation(handsItemLocation);
					int index = controlledPlayer.GetHumanInventory().FindUserReservedLocationIndex(entityInHands);
					if (index >= 0)
						controlledPlayer.GetHumanInventory().GetUserReservedLocation(index, targetItemLocation);
					
					if (controlledPlayer.GetInventory().CanForceSwapEntitiesEx(targetEntity, targetItemLocation, entityInHands, handsItemLocation))
					{
						controlledPlayer.PredictiveForceSwapEntities(targetEntity, entityInHands, targetItemLocation);
					}
					else if (controlledPlayer.GetInventory().CanSwapEntitiesEx(targetEntity, entityInHands ))
					{
						controlledPlayer.PredictiveSwapEntities(targetEntity, entityInHands);
					}
					else
					{
						controlledPlayer.GetInventory().FindFreeLocationFor(targetEntity, FindInventoryLocationType.ANY, targetItemLocation);
						if (targetItemLocation.IsValid() && controlledPlayer.GetInventory().LocationCanAddEntity(targetItemLocation))
							SplitItemUtils.TakeOrSplitToInventoryLocation(controlledPlayer, targetItemLocation);
					}
				}
				else
				{
					bool found = false;
					if (targetEntity.GetInventory().CanRemoveEntity())
					{
						InventoryLocation i2 = new InventoryLocation();
						found = controlledPlayer.GetInventory().FindFreeLocationFor(targetEntity, FindInventoryLocationType.ANY, i2);
						if (found)
						{
							if (i2.GetType() == FindInventoryLocationType.ATTACHMENT)
							{
								if (i2.GetParent() != controlledPlayer)
									found = false;
							}
						}
					}
	
					if (found)
					{
						if (controlledPlayer.GetHumanInventory().CanAddEntityToInventory(targetEntity))
							controlledPlayer.PredictiveTakeEntityToInventory(FindInventoryLocationType.ANY, targetEntity);
					}
					else
					{
						if (controlledPlayer.GetHumanInventory().CanAddEntityInHands(targetEntity))
							controlledPlayer.PredictiveTakeEntityToHands(targetEntity);
					}
				}
				
				RefreshQuickbar();
			}
		}
	}

	void DraggingOverSwap( Widget w, int x, int y, Widget receiver )
	{
		if (w == null)
		{
			return;
		}
		
		string name = w.GetName();
		name.Replace("PanelWidget", "Render");

		ItemPreviewWidget targetIpw = ItemPreviewWidget.Cast(receiver.FindAnyWidget( "Render" ));
		if (m_HandsIcon)
		{
			targetIpw = ItemPreviewWidget.Cast(receiver.GetParent().FindAnyWidget( "Render" ));
		}

		ItemPreviewWidget selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget(name));
		if (selectedIpw == null)
		{
			selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget("Render"));
		}
		
		if (selectedIpw == null)
		{
			return;
		}

		InventoryItem targetEntity = InventoryItem.Cast(targetIpw.GetItem());
		InventoryItem selectedEntity = InventoryItem.Cast(selectedIpw.GetItem());
		if (!selectedEntity || !targetEntity)
		{
			return;
		}
		
		InventoryLocation il = new InventoryLocation();
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		int index = player.GetHumanInventory().FindUserReservedLocationIndex(m_Item);
		
		if (index >= 0)
		{	
			player.GetHumanInventory().GetUserReservedLocation(index, il);
			
			if (GameInventory.CanForceSwapEntitiesEx(selectedEntity, null, targetEntity, il))
			{
				ColorManager.GetInstance().SetColor(w, ColorManager.FSWAP_COLOR);
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().ShowSourceDropzone(il.GetParent());
				return;
			}
		}
		static int testedFlags = InventoryCombinationFlags.SWAP | InventoryCombinationFlags.FSWAP;
		
		int chosenInventoryAction = ItemManager.GetChosenCombinationFlag(selectedEntity, targetEntity, testedFlags, il);
		UpdateFrameColor(selectedEntity, targetEntity, chosenInventoryAction, w, il);
	}

	void DraggingOverCombine(Widget w, int x, int y, Widget receiver)
	{
		if (w == null)
		{
			return;
		}
		
		ItemPreviewWidget selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget("Render"));
		if (!selectedIpw)
		{
			string name = w.GetName();
			name.Replace("PanelWidget", "Render");
			selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget(name));
		}
		
		if (!selectedIpw)
		{
			selectedIpw = ItemPreviewWidget.Cast(w);
		}
		
		if (!selectedIpw.GetItem())
		{
			return;
		}
		EntityAI selectedEntity = selectedIpw.GetItem();
		EntityAI targetEntity = g_Game.GetPlayer().GetEntityInHands();

		static int testedFlags = InventoryCombinationFlags.SET_ACTION | InventoryCombinationFlags.PERFORM_ACTION | InventoryCombinationFlags.COMBINE_QUANTITY2 | InventoryCombinationFlags.ADD_AS_CARGO | InventoryCombinationFlags.ADD_AS_ATTACHMENT;
		
		int chosenInventoryAction = ItemManager.GetChosenCombinationFlag(selectedEntity, targetEntity, testedFlags);
		UpdateFrameColor(selectedEntity, targetEntity, chosenInventoryAction, w);
	}

	bool MouseEnter(Widget w, int x, int y)
	{
		if (!m_IsDragged)
		{
			PrepareOwnedTooltip(m_Obj, x, y);
			m_CursorWidget.Show(true);
		}
		
		return true;
	}

	bool MouseLeave( Widget w, Widget s, int x, int y)
	{
		HideOwnedTooltip();
		if (!m_IsDragged)
		{
			m_CursorWidget.Show(false);
		}
		return true;
	}

	void DraggingOver( Widget w, int x, int y, Widget receiver )
	{
		ItemManager.GetInstance().HideDropzones();
		
		if (w == null)
		{
			return;
		}
		string name = w.GetName();
		name.Replace("PanelWidget", "Render");

		ItemPreviewWidget targetIpw = ItemPreviewWidget.Cast(receiver.FindAnyWidget( "Render" ));
		if (m_HandsIcon)
		{
			targetIpw = ItemPreviewWidget.Cast(receiver.GetParent().FindAnyWidget( "Render" ));
		}

		ItemPreviewWidget selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget( name ));
		if (selectedIpw == null)
		{
			selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget( "Render" ));
		}
		if (selectedIpw == null)
		{
			return;
		}

		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		InventoryItem targetEntity = InventoryItem.Cast(targetIpw.GetItem());
		InventoryItem selectedEntity = InventoryItem.Cast(selectedIpw.GetItem());
		InventoryLocation ilDst;
		if (!targetEntity || !selectedEntity)
		{
			return;
		}
		
		if (m_Lock)
		{
			if (targetEntity == selectedEntity)
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
				ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
			}
			else
			{
				CargoContainer parentContainer = CargoContainer.Cast(m_Parent);
			
				float parentX;
				float parentY;
			
				parentContainer.GetRootWidget().GetScreenPos(parentX, parentY);
			
				float iconSize = parentContainer.GetIconSize();
				float spaceSize = parentContainer.GetSpaceSize();
			
				int PosX = (x - parentX) / (iconSize + spaceSize);
				int PosY = (y - parentY) / (iconSize + spaceSize);
			
				EntityAI parent = m_Lock;
				CargoBase targetCargo 	= parent.GetInventory().GetCargo();
			
				ilDst = new InventoryLocation();			
				ilDst.SetCargoAuto(targetCargo, selectedEntity, PosY, PosX, selectedEntity.GetInventory().GetFlipCargo());
		
				if (parent.GetInventory().LocationCanAddEntity(ilDst))
				{
					ColorManager.GetInstance().SetColor(w, ColorManager.GREEN_COLOR);
					ItemManager.GetInstance().GetRightDropzone().SetAlpha(1);
				}
				else
				{
					ColorManager.GetInstance().SetColor(w, ColorManager.RED_COLOR);
					ItemManager.GetInstance().ShowSourceDropzone(selectedEntity);
				}
			}
		}
		else 
		{
			static int testedFlags = InventoryCombinationFlags.COMBINE_QUANTITY2 | InventoryCombinationFlags.ADD_AS_CARGO | InventoryCombinationFlags.ADD_AS_ATTACHMENT | InventoryCombinationFlags.SWAP | InventoryCombinationFlags.FSWAP | InventoryCombinationFlags.SWAP_MAGAZINE;
		
			int chosenInventoryAction = ItemManager.GetChosenCombinationFlag(selectedEntity, targetEntity, testedFlags, ilDst);
			UpdateFrameColor(selectedEntity, targetEntity, chosenInventoryAction, w, ilDst);
		}
	}

	void OnPerformCombination( int combinationFlags )
	{
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		if (m_am_entity1 == null || m_am_entity2 == null) return;

		if (combinationFlags == InventoryCombinationFlags.NONE) return;

		Weapon_Base wpn;
		Magazine mag;

		if (combinationFlags & InventoryCombinationFlags.PERFORM_ACTION)
		{
			ActionManagerClient amc;
			Class.CastTo(amc, player.GetActionManager());

			if (m_am_entity1 == player.GetEntityInHands())
			{
				amc.PerformActionFromInventory(ItemBase.Cast( m_am_entity1 ),ItemBase.Cast( m_am_entity2 ));
			}
			else
			{
				amc.PerformActionFromInventory(ItemBase.Cast( m_am_entity2 ),ItemBase.Cast( m_am_entity1 ));
			}
		}
		else if (combinationFlags & InventoryCombinationFlags.SET_ACTION)
		{
			ActionManagerClient amc2;
			Class.CastTo(amc2, player.GetActionManager());

			if (m_am_entity1 == player.GetEntityInHands())
			{
				amc2.SetActionFromInventory(ItemBase.Cast( m_am_entity1 ), ItemBase.Cast( m_am_entity2 ));
			}
			else
			{
				amc2.SetActionFromInventory(ItemBase.Cast( m_am_entity2 ), ItemBase.Cast( m_am_entity1 ));
			}
		}
		else if (combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY2)
		{
			ItemBase entity = ItemBase.Cast(m_am_entity1);
			entity.CombineItemsClient(ItemBase.Cast( m_am_entity2 ));
		}
		else if (combinationFlags & InventoryCombinationFlags.ADD_AS_ATTACHMENT)
		{
			float stackable = m_am_entity2.GetTargetQuantityMax(-1);
		
			if (stackable == 0 || stackable >= m_am_entity2.GetQuantity())
			{
				player.PredictiveTakeEntityToTargetAttachment(m_am_entity1, m_am_entity2);
			}
			else
			{
				InventoryLocation il = new InventoryLocation();
				m_am_entity1.GetInventory().FindFreeLocationFor(m_am_entity2, FindInventoryLocationType.ATTACHMENT, il);
				ItemBase.Cast(m_am_entity2).SplitIntoStackMaxToInventoryLocationClient(il);
			}
		}
		else if (combinationFlags & InventoryCombinationFlags.ADD_AS_CARGO)
		{
			SplitItemUtils.TakeOrSplitToInventory(player, m_am_entity1, m_am_entity2);
		}
	}
	
	bool PerformCombination(EntityAI selectedEntity, EntityAI targetEntity, int combinationFlag, InventoryLocation ilSwapDst = null)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		ActionManagerClient amc;
		
		switch (combinationFlag)
		{
			case InventoryCombinationFlags.ADD_AS_ATTACHMENT:
				return player.PredictiveTakeEntityToTargetAttachment(targetEntity, selectedEntity);
			case InventoryCombinationFlags.ADD_AS_CARGO:
				SplitItemUtils.TakeOrSplitToInventory(player, targetEntity, selectedEntity);
				return true;
			case InventoryCombinationFlags.COMBINE_QUANTITY2:
				targetEntity.CombineItemsClient(selectedEntity);
				return true;
			case InventoryCombinationFlags.SET_ACTION:
				Class.CastTo(amc, player.GetActionManager());
				if (targetEntity == player.GetEntityInHands())
				{
					amc.SetActionFromInventory(ItemBase.Cast(targetEntity), ItemBase.Cast(selectedEntity));
				}
				else
				{
					amc.SetActionFromInventory(ItemBase.Cast(selectedEntity), ItemBase.Cast(targetEntity));
				}
				return true;
			case InventoryCombinationFlags.PERFORM_ACTION:
				Class.CastTo(amc, player.GetActionManager());
				if (targetEntity == player.GetEntityInHands())
				{
					amc.PerformActionFromInventory(ItemBase.Cast(targetEntity), ItemBase.Cast(selectedEntity));
				}
				else
				{
					amc.PerformActionFromInventory(ItemBase.Cast(selectedEntity), ItemBase.Cast(targetEntity));
				}
				return true;
			case InventoryCombinationFlags.SWAP_MAGAZINE:
				return player.GetWeaponManager().SwapMagazine(Magazine.Cast(selectedEntity));
			case InventoryCombinationFlags.SWAP:
				return player.PredictiveSwapEntities(targetEntity, selectedEntity);
			case InventoryCombinationFlags.FSWAP:
				return player.PredictiveForceSwapEntities(selectedEntity, targetEntity, ilSwapDst);
			default:
				return false;
		}

		return false;
	}

	void UpdateFrameColor(EntityAI selectedEntity, EntityAI targetEntity, int combinationFlag, Widget w, InventoryLocation il = null)
	{
		int color;
		Widget targetDropzone;
		
		ItemManager.GetInstance().HideDropzones();
		switch (combinationFlag)
		{
			case InventoryCombinationFlags.ADD_AS_ATTACHMENT:
			case InventoryCombinationFlags.ADD_AS_CARGO:
				color = ColorManager.GREEN_COLOR;
				ItemManager.GetInstance().ShowSourceDropzone(targetEntity);
				break;
			case InventoryCombinationFlags.COMBINE_QUANTITY2:
			case InventoryCombinationFlags.SET_ACTION:
			case InventoryCombinationFlags.PERFORM_ACTION:
				color = ColorManager.COMBINE_COLOR;
				ItemManager.GetInstance().ShowSourceDropzone(targetEntity);
				break;
			case InventoryCombinationFlags.SWAP_MAGAZINE:
				color = ColorManager.SWAP_COLOR;
				ItemManager.GetInstance().ShowSourceDropzone(selectedEntity);
				break;
			case InventoryCombinationFlags.SWAP:
				color = ColorManager.SWAP_COLOR;
				ItemManager.GetInstance().ShowSourceDropzone(selectedEntity);
				break;
			case InventoryCombinationFlags.FSWAP:
				color = ColorManager.FSWAP_COLOR;
				if (il)
				{
					ItemManager.GetInstance().ShowSourceDropzone(il.GetParent());
				}
				else
				{
					ItemManager.GetInstance().ShowSourceDropzone(selectedEntity);
				}
				break;
			default:
				color = ColorManager.RED_COLOR;
				ItemManager.GetInstance().ShowSourceDropzone(selectedEntity);
		}

		ColorManager.GetInstance().SetColor( w, color );
	}

	

	void ShowActionMenuCombine( EntityAI entity1, EntityAI entity2, int combinationFlags, Widget w , bool color_test )
	{
		int lastFlag = 0;
		ContextMenu cmenu = ContextMenu.Cast(g_Game.GetUIManager().GetMenu().GetContextMenu());
		m_am_entity1 = entity1;
		m_am_entity2 = entity2;
		cmenu.Hide();
		cmenu.Clear();
		int id = -1;

		if ( entity1 == null || entity2 == null ) return;

		if ( combinationFlags == InventoryCombinationFlags.NONE )
		{
			if ( color_test )
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
				ItemManager.GetInstance().ShowSourceDropzone( entity2 );
			}
			return;
		}

		if (combinationFlags & InventoryCombinationFlags.ADD_AS_CARGO)
		{
			lastFlag = InventoryCombinationFlags.ADD_AS_CARGO;
			cmenu.Add( "#inv_context_add_as_cargo", this, "OnPerformCombination", new Param1<int>( lastFlag ) );
		}
		if (combinationFlags & InventoryCombinationFlags.ADD_AS_ATTACHMENT)
		{
			lastFlag = InventoryCombinationFlags.ADD_AS_ATTACHMENT;
			cmenu.Add( "#inv_context_add_as_attachment", this, "OnPerformCombination", new Param1<int>( lastFlag ));
		}
		
		if (combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY2)
		{
			lastFlag = InventoryCombinationFlags.COMBINE_QUANTITY2;
			cmenu.Add("#inv_context_combine_quantity", this, "OnPerformCombination", new Param1<int>( lastFlag ));
		}

		if (combinationFlags & InventoryCombinationFlags.SET_ACTION)
		{
			lastFlag = InventoryCombinationFlags.SET_ACTION;
			cmenu.Add("#inv_context_attach_magazine", this, "OnPerformCombination", new Param1<int>( lastFlag ));
		}
		
		if (combinationFlags & InventoryCombinationFlags.PERFORM_ACTION)
		{
			lastFlag = InventoryCombinationFlags.PERFORM_ACTION;
			cmenu.Add("Perform action", this, "OnPerformCombination", new Param1<int>( lastFlag ));
		}

		if (color_test)
		{
			if (lastFlag == 0)
			{
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
			}
			else if (lastFlag == InventoryCombinationFlags.ADD_AS_ATTACHMENT || lastFlag == InventoryCombinationFlags.ADD_AS_CARGO )
			{
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
				ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
			}
			else
			{
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetCenterDropzone().SetAlpha( 1 );
				ColorManager.GetInstance().SetColor( w, ColorManager.COMBINE_COLOR );
			}
		}
		else if (cmenu.Count() >= 1)
		{
			OnPerformCombination(combinationFlags);
			return;
		}	
	}

	void OnPerformRecipe(int id)
	{
		if ( m_am_entity1 == null || m_am_entity2 == null ) return;

		Debug.Log("OnPerformRecipe called for id:"+id.ToString(),"recipes");
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		player.GetCraftingManager().SetInventoryCraft( id, ItemBase.Cast(m_am_entity1), ItemBase.Cast(m_am_entity2));
	}

	void Combine( Widget w, int x, int y, Widget receiver )
	{
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging(false);
		ItemPreviewWidget selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget("Render"));
		if (!selectedIpw)
		{
			string name = w.GetName();
			name.Replace("PanelWidget", "Render");
			selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget( name ));
		}
		if (!selectedIpw)
		{
			selectedIpw = ItemPreviewWidget.Cast(w);
		}
		if (!selectedIpw.GetItem())
		{
			return;
		}
		EntityAI selectedEntity = selectedIpw.GetItem();
		EntityAI targetEntity = g_Game.GetPlayer().GetEntityInHands();
		static int testedFlags = InventoryCombinationFlags.SET_ACTION | InventoryCombinationFlags.PERFORM_ACTION | InventoryCombinationFlags.COMBINE_QUANTITY2 | InventoryCombinationFlags.ADD_AS_CARGO | InventoryCombinationFlags.ADD_AS_ATTACHMENT;
		
		int chosenInventoryAction = ItemManager.GetChosenCombinationFlag(selectedEntity, targetEntity, testedFlags);
		PerformCombination(selectedEntity, targetEntity, chosenInventoryAction);
	}
	
	bool CombineItems( EntityAI entity1, EntityAI entity2 )
	{
		int flags = ItemManager.GetCombinationFlags( entity1, entity2 );
		return FlagAction( entity1, entity2, flags );
	}
	
	bool FlagAction( EntityAI entity1, EntityAI entity2, int combinationFlags )
	{
		int current_flag;
		ContextMenu cmenu = ContextMenu.Cast(g_Game.GetUIManager().GetMenu().GetContextMenu());
		m_am_entity1 = entity1;
		m_am_entity2 = entity2;
		cmenu.Hide();
		cmenu.Clear();
		int id = -1;

		if (combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY2 )
		{
			ItemBase entity = ItemBase.Cast( entity1 );
			entity.CombineItemsClient( ItemBase.Cast( entity2 ) );
			return false;
		}

		if (entity1 == null || entity2 == null || combinationFlags == InventoryCombinationFlags.NONE )
			return true;

		if (combinationFlags & InventoryCombinationFlags.ADD_AS_ATTACHMENT )
		{
			current_flag = InventoryCombinationFlags.ADD_AS_ATTACHMENT;
			cmenu.Add( "#inv_context_add_as_attachment", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}
		/*if (combinationFlags & InventoryCombinationFlags.LOAD_CHAMBER )
		{
			current_flag = InventoryCombinationFlags.LOAD_CHAMBER;
			cmenu.Add( "#inv_context_load_chamber", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}*/
		if(combinationFlags & InventoryCombinationFlags.ATTACH_MAGAZINE)
		{
			current_flag = InventoryCombinationFlags.ATTACH_MAGAZINE;
			cmenu.Add("#inv_context_attach_magazine", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		if (combinationFlags & InventoryCombinationFlags.ADD_AS_CARGO )
		{
			current_flag = InventoryCombinationFlags.ADD_AS_CARGO;
			cmenu.Add( "#inv_context_add_as_cargo", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		if (combinationFlags & InventoryCombinationFlags.SWAP )
		{
			current_flag = InventoryCombinationFlags.SWAP;
			cmenu.Add( "#inv_context_swap", this, "OnPerformCombination", new Param1<int>( current_flag ) );
		}

		if (combinationFlags & InventoryCombinationFlags.COMBINE_QUANTITY2 )
		{
			current_flag = InventoryCombinationFlags.COMBINE_QUANTITY2;
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

		MissionGameplay mission = MissionGameplay.Cast( g_Game.GetMission() );
		/*if (combinationFlags & InventoryCombinationFlags.RECIPE_HANDS ||  combinationFlags & InventoryCombinationFlags.RECIPE_ANYWHERE )
		{
			OnPerformRecipe( id );
			return true;
		}
		else /*if (cmenu.Count() == 1 )*/
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

	void MouseClick(Widget w, int x, int y, int button)
	{
		if (button == MouseState.RIGHT)
		{
			if (m_Lock)
			{
				g_Game.GetPlayer().GetHumanInventory().ClearUserReservedLocationSynced(m_Item);
			}
			else if (m_Item)
			{
				#ifdef DIAG_DEVELOPER
				if (GetDayZGame().IsLeftCtrlDown())
					ShowActionMenu(m_Item);
				else
				#endif
				{
					m_Item.OnRightClick();
					
					if (m_HasQuantity)
						SetQuantity();
				}
			}
				
		}
		else if (!m_Lock)
		{
			switch (button)
			{
				case MouseState.MIDDLE:
					InspectItem(m_Item);
					break;
				
				case MouseState.LEFT:
					PlayerBase controlledPlayer = PlayerBase.Cast(g_Game.GetPlayer());
					if (g_Game.IsLeftCtrlDown())
					{
						if (controlledPlayer.CanDropEntity(m_Item))
						{
							if (m_Item.GetTargetQuantityMax() < m_Item.GetQuantity())
								m_Item.SplitIntoStackMaxClient(null, -1);
							else
								controlledPlayer.PhysicalPredictiveDropItem(m_Item);
	
							ItemManager.GetInstance().SetWidgetDraggable(w, false);
						}
					}
					else
					{
						InventoryLocation il = new InventoryLocation();
						m_Obj.GetInventory().GetCurrentInventoryLocation(il);
		
						bool draggable = !controlledPlayer.GetInventory().HasInventoryReservation(m_Obj, null) && !controlledPlayer.GetInventory().IsInventoryLocked() && !controlledPlayer.IsItemsToDelete();
						draggable = draggable && (m_Obj.GetHierarchyRoot() && m_Obj.GetInventory().CanRemoveEntity() || !m_Obj.GetHierarchyRoot() && AttachmentsOutOfReach.IsAttachmentReachable(m_Obj, "", il.GetSlot()));
				
						ItemManager.GetInstance().SetWidgetDraggable(w, draggable);				
					}
					break;
			}
		}
	}

	void DropReceivedFromMain( Widget w, int x, int y, Widget receiver )
	{
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging(false);
		string name = w.GetName();
		name.Replace("PanelWidget", "Render");
		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		
		ItemPreviewWidget targetIpw = ItemPreviewWidget.Cast(receiver.FindAnyWidget( "Render" ));
		if (m_HandsIcon)
		{
			targetIpw = ItemPreviewWidget.Cast(receiver.GetParent().FindAnyWidget( "Render" ));
		}

		ItemPreviewWidget selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget( name ));
		if (selectedIpw == null)
		{
			selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget( "Render" ));
		}
		if (selectedIpw == null)
		{
			return;
		}

		InventoryItem targetEntity = InventoryItem.Cast(targetIpw.GetItem());
		InventoryItem selectedEntity = InventoryItem.Cast(selectedIpw.GetItem());
		if (!selectedEntity)
		{
			return;
		}
		int index;
		InventoryLocation ilSrc;
		InventoryLocation ilDst;
		if (m_Lock && targetEntity == selectedEntity)
		{
			index = player.GetHumanInventory().FindUserReservedLocationIndex(m_Item);
			ilSrc = new InventoryLocation();
			ilDst = new InventoryLocation();
			
			m_Item.GetInventory().GetCurrentInventoryLocation(ilSrc);
			player.GetHumanInventory().GetUserReservedLocation(index, ilDst);
			
			if (ilDst.GetParent().GetInventory().LocationCanAddEntity(ilDst))
			{
			
				player.GetHumanInventory().ClearUserReservedLocation(m_Item);
				player.PredictiveTakeToDst(ilSrc, ilDst);
				m_Item.GetOnReleaseLock().Invoke(m_Item);
			}
		}
		else if (m_Lock)
		{
			CargoContainer parentCargo = CargoContainer.Cast(m_Parent);
			
			float parentX;
			float parentY;
			
			parentCargo.GetRootWidget().GetScreenPos(parentX, parentY);
			
			float iconSize = parentCargo.GetIconSize();
			float spaceSize = parentCargo.GetSpaceSize();
			
			int PosX = (x-parentX) / (iconSize + spaceSize);
			int PosY = (y-parentY) / (iconSize + spaceSize);
			
			EntityAI parent = m_Lock;
			CargoBase targetCargo = parent.GetInventory().GetCargo();
			
			ilDst = new InventoryLocation();			
			ilDst.SetCargoAuto(targetCargo, selectedEntity, PosY, PosX, selectedEntity.GetInventory().GetFlipCargo());
			
			if (parent.GetInventory().LocationCanAddEntity(ilDst))
			{
				player.GetHumanInventory().ClearUserReservedLocation(m_Item);
				m_Item.GetOnReleaseLock().Invoke(m_Item);
				
				SplitItemUtils.TakeOrSplitToInventoryLocation(player, ilDst);
			}
		}
		else
		{
			ilDst = new InventoryLocation();
			static int testedFlags = InventoryCombinationFlags.COMBINE_QUANTITY2 | InventoryCombinationFlags.ADD_AS_CARGO | InventoryCombinationFlags.ADD_AS_ATTACHMENT | InventoryCombinationFlags.SWAP | InventoryCombinationFlags.FSWAP | InventoryCombinationFlags.SWAP_MAGAZINE;
		
			int chosenInventoryAction = ItemManager.GetChosenCombinationFlag(selectedEntity, targetEntity, testedFlags, ilDst);
			PerformCombination(selectedEntity, targetEntity, chosenInventoryAction, ilDst);
		}		
	}

	void Swap( Widget w, int x, int y, Widget receiver )
	{
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging(false);
		string name = w.GetName();
		name.Replace("PanelWidget", "Render");

		ItemPreviewWidget targetIpw = ItemPreviewWidget.Cast(receiver.FindAnyWidget( "Render" ));
		if (m_HandsIcon)
		{
			targetIpw = ItemPreviewWidget.Cast(receiver.GetParent().FindAnyWidget( "Render" ));
		}

		ItemPreviewWidget selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget( name ));
		if (selectedIpw == null)
		{
			selectedIpw = ItemPreviewWidget.Cast(w.FindAnyWidget( "Render" ));
		}
		if (selectedIpw == null)
		{
			return;
		}

		InventoryItem targetEntity = InventoryItem.Cast(targetIpw.GetItem());
		InventoryItem selectedEntity = InventoryItem.Cast(selectedIpw.GetItem());
		if (!selectedEntity || !targetEntity)
		{
			return;
		}

		PlayerBase player = PlayerBase.Cast( g_Game.GetPlayer() );
		EntityAI itemInHands = player.GetEntityInHands();

		InventoryLocation ilDst = new InventoryLocation();
		InventoryLocation ilSrc = new InventoryLocation();
		targetEntity.GetInventory().GetCurrentInventoryLocation(ilSrc);
		Icon icon = ItemManager.GetInstance().GetDraggedIcon();
		
		int index = player.GetHumanInventory().FindUserReservedLocationIndex(targetEntity);
		if (index>=0)
		{
			player.GetHumanInventory().GetUserReservedLocation(index, ilDst);

			if (GameInventory.CanForceSwapEntitiesEx( selectedEntity, ilSrc, targetEntity, ilDst ))
			{
				if (m_HandsIcon && !player.GetInventory().HasInventoryReservation(itemInHands, null) && !player.IsItemsToDelete())
				{
					g_Game.GetPlayer().PredictiveForceSwapEntities(selectedEntity, targetEntity, ilDst);
					return;
				}
			}
		}
		static int testedFlags = InventoryCombinationFlags.SWAP | InventoryCombinationFlags.FSWAP;
		
		int chosenInventoryAction = ItemManager.GetChosenCombinationFlag(selectedEntity, targetEntity, testedFlags, ilDst);
		PerformCombination(selectedEntity, targetEntity, chosenInventoryAction, ilDst);
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
			switch (InventoryMenu.GetWidthType())
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

	void SetCargoPos(int x)
	{
		GetMainWidget().SetSort( x );
		m_CargoPos = x;
	}

	void DestroyWhiteBackground()
	{
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
		m_IsDragged = false;
		
		RevertToOriginalFlip();
		
		if (m_HandsIcon)
		{
			m_ItemPreview.SetForceFlipEnable(true);
			m_ItemPreview.SetForceFlip(false);
			Refresh();
			FullScreen();
		}
		
		m_ColorWidget.Show(false);
		
		m_CursorWidget.SetColor( ARGBF( 1, 1, 1, 1 ) );
		m_CursorWidget.Show( false );

		InventoryMenu menu = InventoryMenu.Cast(g_Game.GetUIManager().FindMenu( MENU_INVENTORY ));
		
		if (menu)
			menu.RefreshQuickbar();
	}

	void CreateWhiteBackground()
	{
		m_PreviousFlipOrientation = m_Item.GetInventory().GetFlipCargo();
		ItemManager.GetInstance().SetDraggedIcon(this);
		ItemManager.GetInstance().SetDraggedItem(m_Item);
		m_IsDragged = true;
		ItemManager.GetInstance().SetIsDragging(true);
		int ww, hh;
		g_Game.GetInventoryItemSize(m_Item, ww, hh);
		if (m_Item.GetInventory().GetFlipCargo())
			SetSize(hh, ww);
		else
			SetSize(ww, hh);
		
		SetSize();
		
		if (!m_HandsIcon)
		{
			Refresh();
		}
		else
		{
			m_ItemPreview.SetForceFlipEnable(false);
			m_ColorWidget.SetColor(ColorManager.BASE_COLOR);
		}
		m_ColorWidget.Show(m_HandsIcon);
		
		m_CursorWidget.Show(true);
	}

	void OnDraggingOverBackground( Widget w, int x, int y, Widget reciever  )
	{
		ItemManager.GetInstance().HideDropzones();
		EntityAI entity = EntityAI.Cast( m_Obj );
		EntityAI parent = entity.GetHierarchyParent();
		if (parent && parent.GetHierarchyRootPlayer() == g_Game.GetPlayer())
		{
			ItemManager.GetInstance().GetRightDropzone().SetAlpha(1);
		}
		else if (!m_HandsIcon)
		{
			ItemManager.GetInstance().GetLeftDropzone().SetAlpha(1);
		}

		if (w == null || reciever == null)
		{
			return;
		}
		Pos pa;
		reciever.GetUserData(pa);
		if (m_Parent)
		{
			ContainerWithCargo item = ContainerWithCargo.Cast(m_Parent.m_Parent);
			if (item)
			{
				item.DraggingOverGrid(w, m_PosY + pa.y, m_PosX + pa.x, reciever);
			}
			HandsContainer hands_item = HandsContainer.Cast(m_Parent.m_Parent);
			if (hands_item)
			{
				hands_item.DraggingOverGrid(w, m_PosY + pa.y, m_PosX + pa.x, reciever, null);
			}
		}
	}

	void DropReceivedFromBackground( Widget w, int x, int y, Widget reciever  )
	{
		Pos pa;
		if (reciever)
		{
			reciever.GetUserData(pa);
			if (m_Parent)
			{
				ContainerWithCargo item = ContainerWithCargo.Cast(m_Parent.m_Parent);
				if (item)
				{
					item.DropReceived(w, m_PosY + pa.y, m_PosX + pa.x);
				}
			}
		}
	}
	
	void RevertToOriginalFlip()
	{
		if (m_Item)
		{
			m_Item.GetInventory().SetFlipCargo(m_PreviousFlipOrientation);
			int ww, hh;
			g_Game.GetInventoryItemSize(m_Item, ww, hh);
			
			if (m_PreviousFlipOrientation)
				SetSize(hh, ww);
			else
				SetSize(ww, hh);

			SetSize();
		}
	}

	override void Refresh()
	{
		super.Refresh();

		if (!m_HandsIcon)
			SetPos();
		
		if (m_HasTemperature)
			SetTemperature();
		
		if (m_IsWeapon)
			RefreshMuzzleIcon();
		
		if (m_HasQuantity)
			SetQuantity();
	}

	void SetTemperature()
	{
		ItemManager.GetInstance().SetIconTemperature(EntityAI.Cast( m_Obj ), m_RootWidget);
	}

	void RefreshIconPos()
	{
		Refresh();
		GetMainWidget().Update();
	}

	void FullScreen()
	{
		if (m_IsDragged)
		{
			return;
		}
		GetRootWidget().ClearFlags(WidgetFlags.HEXACTSIZE + WidgetFlags.VEXACTSIZE);
		GetRootWidget().SetSize(1, 1);
		m_ColorWidget.SetColor(ARGB( 0, 0, 0, 0 ));
	}

	void RefreshPos( int row, int column )
	{
		if (row != m_PosY || column != m_PosX)
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
		Weapon_Base wpn = Weapon_Base.Cast(GetObject());
		if (wpn)
		{
			int i;
			ImageWidget ammoIcon;
			if (!wpn.IsShowingChamberedBullet())
			{
				for (i = 0; i < m_AmmoIcons.Count(); i++)
				{
					ammoIcon = m_AmmoIcons.Get(i);
					ammoIcon.Show(false);
				}
			}
			else
			{
				for (i = 0; i < wpn.GetMuzzleCount(); i++)
				{
					if (i > m_AmmoIcons.Count())
						break;
					
					ammoIcon = m_AmmoIcons.Get(i);
					
					if (wpn.IsChamberFull(i))
					{
						if (wpn.IsJammed())
						{
							ammoIcon.Show(true);
							ammoIcon.SetImage(2);
						}
						else if (wpn.IsChamberFiredOut(i))
						{
							ammoIcon.Show(true);
							ammoIcon.SetImage(1);
						}
						else
						{
							ammoIcon.Show(true);
							ammoIcon.SetImage(0);
						}
					}
					else
					{
						ammoIcon.Show(false);
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
					string quantityText = QuantityConversions.GetItemQuantityText(m_Item);

					if (QuantityConversions.GetItemQuantityMax(m_Item) == 1 || quantityText == "")
					{
						m_QuantityStack.Show(false);
					}
					else
					{
						m_QuantityItem.SetText(quantityText);
						m_QuantityStack.Show(true);
					}

					m_QuantityProgress.Show(false);
				}
				else if (quantityType == QUANTITY_PROGRESS)
				{	
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
	
	void SetSort(int index)
	{
		GetMainWidget().SetSort(index);
		GetMainWidget().Update();
	}
	
	int GetSort()
	{
		return GetMainWidget().GetSort();
	}
	
	void SetItemPreviewEx(bool refresh = true)
	{
		m_ItemPreview.Show(true, refresh);
		m_ItemPreview.SetItem(EntityAI.Cast(m_Obj));
		m_ItemPreview.SetModelOrientation("0 0 0");
		m_ItemPreview.SetView(m_Obj.GetViewIndex());
	}
	
	void SetItemPreview()
	{
		SetItemPreviewEx();
	}
	
	void SetItemSizeEx(bool refresh = true)
	{
		#ifdef PLATFORM_CONSOLE
		m_ItemSizePanel.Show(true, refresh);
		m_ItemSizeWidget.Show(true, refresh);
		
		int sizeX, sizeY;
		g_Game.GetInventoryItemSize(m_Item, sizeX, sizeY);
		int capacity = sizeX * sizeY;
		m_ItemSizeWidget.SetText(capacity.ToString());
		#endif
	}
	
	void SetItemSize()
	{
		SetItemSizeEx();
	}
	
	void UpdateFlip(bool flipped)
	{
		int sizeX, sizeY;
		g_Game.GetInventoryItemSize(m_Item, sizeX, sizeY);
		
		if (flipped)
			SetSize(sizeY, sizeX);
		else
			SetSize(sizeX, sizeY);

		SetSize();
	}
	
	void InitLock( EntityAI parent, EntityAI obj, int x_pos, int y_pos, bool flip)
	{
		m_Lock	= parent;
		m_Obj	= obj;
		m_Item	= ItemBase.Cast(m_Obj);
		
		SetPos(x_pos, y_pos);
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
		if (obj != m_Obj)
		{
			if (m_Obj)
			{
				m_Obj.GetOnItemFlipped().Remove(UpdateFlip );
				m_Obj.GetOnViewIndexChanged().Remove(SetItemPreview);
			}
			if (obj)
			{
				obj.GetOnItemFlipped().Insert(UpdateFlip);
				obj.GetOnViewIndexChanged().Insert(SetItemPreview);
			}
		}
		
		if (m_HandsIcon)
		{
			m_ItemPreview.SetForceFlipEnable(true);
			m_ItemPreview.SetForceFlip(false);
		}
		
		m_Obj	= obj;
		m_Item	= ItemBase.Cast(m_Obj);
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
		m_RootWidget.FindAnyWidget("Reserved").Show(false, refresh);
		
		m_ColorWidget.Show(false);
		if (refresh)
			Refresh();
	}
	
	void Init(EntityAI obj)
	{
		InitEx(obj);
	}
	
	void CheckIsWeapon()
	{
		Weapon_Base wpn = Weapon_Base.Cast(m_Obj);
		if (wpn)
		{
			m_AmmoIcons = new array<ImageWidget>();
			m_IsWeapon = true;
			float posX = 0.0;
			float width = 0.0, height = 0.0;
			for (int i = 0; i < wpn.GetMuzzleCount(); i++)
			{
				if (i == 1)
				{
					m_AmmoIcons[0].GetSize(width, height);
				}
				posX += width;
				
				Widget ammoIcon = Widget.Cast(g_Game.GetWorkspace().CreateWidgets("gui/layouts/inventory_new/ammo_icon.layout", GetMainWidget()));
				ammoIcon.SetPos(posX, 0.0, false);
				
				ImageWidget ammoIconImg = ImageWidget.Cast(ammoIcon.GetChildren());				
				
				AmmoData data = Magazine.GetAmmoData(wpn.GetChamberAmmoTypeName(i));
				if (data)
				{
					CartridgeType cartridgeType = data.m_CartridgeType;
					switch (cartridgeType)
					{
						case CartridgeType.Pistol:
						{
							ammoIconImg.LoadImageFile(0, "set:dayz_gui image:cartridge_pistol");
							ammoIconImg.LoadImageFile(1, "set:dayz_gui image:shell_pistol");
							ammoIconImg.LoadImageFile(2, "set:dayz_gui image:jam_pistol");
							break;
						}
						case CartridgeType.Intermediate:
						{
							ammoIconImg.LoadImageFile(0, "set:dayz_gui image:cartridge_int");
							ammoIconImg.LoadImageFile(1, "set:dayz_gui image:shell_int");
							ammoIconImg.LoadImageFile(2, "set:dayz_gui image:jam_int");
							break;
						}
						case CartridgeType.FullPower:
						{
							ammoIconImg.LoadImageFile(0, "set:dayz_gui image:cartridge_fp");
							ammoIconImg.LoadImageFile(1, "set:dayz_gui image:shell_fp");
							ammoIconImg.LoadImageFile(2, "set:dayz_gui image:jam_fp");
							break;
						}
						case CartridgeType.Shell:
						{
							ammoIconImg.LoadImageFile(0, "set:dayz_gui image:cartridge_shell");
							ammoIconImg.LoadImageFile(1, "set:dayz_gui image:shell_shell");
							ammoIconImg.LoadImageFile(2, "set:dayz_gui image:jam_shell");
							break;
						}
						//@note: Crossbows cant jam so we dont add a icon for the jam state
						case CartridgeType.Arrow:
						{
							ammoIconImg.LoadImageFile(0, "set:dayz_gui image:arrow_int");
							ammoIconImg.LoadImageFile(1, "set:dayz_gui image:arrow_int");
							break;
						}
					}
				}
				m_AmmoIcons.Insert(ammoIconImg);
			}
		}
		else
		{
			m_IsWeapon = false;
		}			
	}
	
	void CheckIsMagazineEx( bool refresh = true )
	{
		Magazine mag = Magazine.Cast(m_Obj);
		if (mag)
		{
			m_IsMagazine = true;
			AmmoData data = Magazine.GetAmmoData(mag.ClassName());
			if (data)
			{
				ProjectileType p_type = data.m_ProjectileType;
				switch (p_type)
				{
					case ProjectileType.None:
					{
						m_AmmoTypeIcon.Show(false, refresh);
						break;
					}
					case ProjectileType.Tracer:
					{
						m_AmmoTypeIcon.LoadImageFile(0, "set:dayz_gui image:tracer");
						m_AmmoTypeIcon.Show(true, refresh);
						break;
					}
					case ProjectileType.AP:
					{
						m_AmmoTypeIcon.LoadImageFile(0, "set:dayz_gui image:armor_piercing");
						m_AmmoTypeIcon.Show(true, refresh);
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
		if (m_Item)
		{
			m_HasTemperature = m_Item.CanHaveTemperature();
		}
	}
	
	void CheckHasQuantityEx(bool refresh = true)
	{
		if (m_Item)
		{
			m_HasQuantity = (QuantityConversions.HasItemQuantity( m_Item ) != QUANTITY_HIDDEN);
			
			if (m_HasQuantity)
				m_QuantityPanel.Show(true, refresh);
		}
	}
	
	void CheckHasQuantity()
	{
		CheckHasQuantityEx();
	}
	
	void SetPosX(int x)
	{
		m_PosX = x;
	}
	
	void SetPosY(int y)
	{
		m_PosY = y;
	}
	
	void SetSizeX(int x)
	{
		m_SizeX = x;
	}
	
	void SetSizeY(int y)
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

	void SetPosEx(bool refresh = true)
	{
		CargoContainer parentContainer = CargoContainer.Cast(m_Parent);
		HandsPreview parentHPrevContainer = HandsPreview.Cast(m_Parent);
		Widget rootWidget = GetRootWidget();
		float iconSize, spaceSize;
		if (parentContainer)
		{
			iconSize = parentContainer.GetIconSize();
			spaceSize = parentContainer.GetSpaceSize();
		}
		else if (parentHPrevContainer)
		{
			iconSize = parentHPrevContainer.GetIconSize();
			if (rootWidget)
			{
				rootWidget.SetFlags(WidgetFlags.EXACTSIZE, refresh);
			}
		}
		
		if (rootWidget)
		{
			#ifndef PLATFORM_CONSOLE
				rootWidget.SetPos(iconSize * GetPosX() + ( GetPosX() + 1 ) * spaceSize, iconSize * GetPosY() + ( GetPosY() + 1 ) * spaceSize, refresh);
				rootWidget.SetSize(iconSize * m_SizeX + ( m_SizeX ) * spaceSize, iconSize * m_SizeY + ( m_SizeY ) * spaceSize, refresh);
			#else
				int row = m_CargoPos / 5;
				int column = m_CargoPos % 5;
				rootWidget.SetPos(iconSize * column, iconSize * row, refresh);
				rootWidget.SetSize(iconSize, iconSize, refresh);
			#endif
		}
	}
	
	void SetPos()
	{
		SetPosEx();
	}
	
	void SetSize()
	{
		CargoContainer parentContainer = CargoContainer.Cast(m_Parent);
		HandsPreview parentHPrevContainer = HandsPreview.Cast(m_Parent);
		float iconSize, spaceSize;
		Widget rootWidget = GetRootWidget();
		if (parentContainer)
		{
			iconSize = parentContainer.GetIconSize();
			spaceSize = parentContainer.GetSpaceSize();
		}
		else if (parentHPrevContainer)
		{
			iconSize = parentHPrevContainer.GetIconSize();
			if (rootWidget)
			{
				GetRootWidget().SetFlags(WidgetFlags.EXACTSIZE);
			}
		}
	
		if (rootWidget)
		{
		#ifndef PLATFORM_CONSOLE
			GetRootWidget().SetSize(iconSize * m_SizeX + ( m_SizeX ) * spaceSize, iconSize * m_SizeY + ( m_SizeY ) * spaceSize);
		#else
			GetRootWidget().SetSize(iconSize, iconSize);
		#endif
		}
	}
	
	override void UpdateInterval()
	{
		if (m_Item)
		{
			if (m_HasTemperature)
				SetTemperature();
			
			if (m_IsWeapon)
				RefreshMuzzleIcon();
			
			if (m_HasQuantity)
				SetQuantity();
		}
	}
}

class Pos
{
	int x, y;

	void Pos(int _x, int _y)
	{
		x = _x;
		y = _y;
	}
}
