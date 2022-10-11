class LayoutHolder extends ScriptedWidgetEventHandler
{
	protected Widget				m_MainWidget;
	protected Widget				m_RootWidget;
	protected Widget				m_ParentWidget;
	protected LayoutHolder			m_Parent;
	protected string				m_LayoutName;
	
	protected bool					m_IsActive;
	protected bool					m_ImmedUpdate;
	protected bool					m_TooltipOwner;
	
	protected EntityAI				m_am_entity1, m_am_entity2;

	void UpdateInterval();
	void SetLayoutName();
	
	// Override this and set m_ImmedUpdate to true if you need the widget to update on construction
	// Had to be done this way since adding it to the constructor parameters would break mods..
	void SetImmedUpdate()
	{
		m_ImmedUpdate = false;
	}
	
	void OnSelectAction(ItemBase item, int actionId)
	{
		PlayerBase m_player = PlayerBase.Cast( GetGame().GetPlayer() );
		m_player.GetActionManager().OnInstantAction(ActionDebug,new Param2<ItemBase,int>(item,actionId));
	}
	
	void ShowActionMenu(InventoryItem item)
	{
		PlayerBase m_player = PlayerBase.Cast( GetGame().GetPlayer() );
		HideOwnedTooltip();
		m_am_entity1 = item;
		m_am_entity2 = null;
		ContextMenu cmenu = GetGame().GetUIManager().GetMenu().GetContextMenu();

		cmenu.Hide();
		cmenu.Clear();

		if (m_am_entity1 == null)
			return;

		TSelectableActionInfoArray customActions = new TSelectableActionInfoArray;
		ItemBase itemBase = ItemBase.Cast( item );
		
		///itemBase.GetRecipesActions(m_player, customActions);
		itemBase.GetDebugActions(customActions);
		/*
		if( ItemBase.GetDebugActionsMask() & DebugActionType.GENERIC_ACTIONS )
		{
			
		}*/
		if ( ItemBase.GetDebugActionsMask() & DebugActionType.PLAYER_AGENTS )
		{
			m_player.GetDebugActions(customActions);
		}

		int actionsCount = customActions.Count();
		for ( int i = 0; i < customActions.Count(); i++ )
		{
			TSelectableActionInfo actionInfo = customActions.Get(i);
			if ( actionInfo )
			{
				int actionId = actionInfo.param2;
				string actionText = actionInfo.param3;

				cmenu.Add(actionText, this, "OnSelectAction", new Param2<ItemBase, int>(itemBase, actionId));
			}
		}

		int m_am_Pos_x,  m_am_Pos_y;
		GetMousePos( m_am_Pos_x, m_am_Pos_y );
				m_am_Pos_x -= 5;
		m_am_Pos_y -= 5;
		cmenu.Show(m_am_Pos_x, m_am_Pos_y);
	}
	
	
	void LayoutHolder( LayoutHolder parent )
	{		
		m_Parent = parent;
		
		SetLayoutName();
		SetParentWidget();
		SetImmedUpdate();
		
		m_TooltipOwner = false;
		
		if ( m_LayoutName != "" )
		{
			m_MainWidget = GetGame().GetWorkspace().CreateWidgets( m_LayoutName, null, false );
			m_MainWidget.Show( true, false );
		}
		
		m_RootWidget = m_MainWidget;
		
		if ( m_ParentWidget )
		{
			m_ParentWidget.Show( true, false );
			if ( m_MainWidget )
			{
				m_ParentWidget.AddChild( m_MainWidget, false );
			}
		}
		
		if ( m_ImmedUpdate )
		{
			if ( m_MainWidget )
				m_MainWidget.Update();
		}
	}
	
	void ~LayoutHolder()
	{
		HideOwnedTooltip();
		delete m_RootWidget;
	}	
		
	LayoutHolder GetRoot()
	{
		if ( m_Parent )
		{
			ref LayoutHolder container = m_Parent.GetRoot();
			return container;
		}
		else
		{
			return this;
		}
	}
	
	void SetParentWidget()
	{
		if ( m_Parent )
			m_ParentWidget = m_Parent.GetMainWidget();
	}
	
	LayoutHolder GetParent()
	{
		return m_Parent;
	}
	
	void SetActive(bool active)
	{
		m_IsActive = active;
	}
	
	void SetLastActive();
	void SetFirstActive();

	bool IsActive()
	{
		return m_IsActive;
	}

	Widget GetMainWidget()
	{
		return m_MainWidget;
	}
	
	Widget GetRootWidget()
	{
		return m_RootWidget;
	}
	
	bool IsDisplayable()
	{
		return true;
	}
	
	bool IsVisible()
	{
		if (m_RootWidget)
		{
			return m_RootWidget.IsVisible();
		}
		return false;
	}

	void OnShow()
	{
		m_RootWidget.Show( true );
		if ( m_ParentWidget )
			m_ParentWidget.Show( true );
	}

	void OnHide()
	{
		m_RootWidget.Show( false );
	}

	void Refresh()
	{
		m_RootWidget.Update();
	}
	
	void InspectItem( EntityAI item )
	{
		InventoryMenu menu = InventoryMenu.Cast( GetGame().GetUIManager().FindMenu(MENU_INVENTORY) );
		InspectMenuNew inspect_menu = InspectMenuNew.Cast( menu.EnterScriptedMenu(MENU_INSPECT) );
		if ( inspect_menu )
		{
			GetGame().GetMission().GetHud().ShowHudUI( false );
			GetGame().GetMission().GetHud().ShowQuickbarUI( false );
			inspect_menu.SetItem( item );
		}
	}
	
	void UpdateSelectionIcons()
	{}
	
	void PrepareOwnedTooltip(EntityAI item/*, Widget w*/, int x = 0, int y = 0)
	{
		ItemManager.GetInstance().PrepareTooltip(item,x,y);
		m_TooltipOwner = true;
	}
	
	void PrepareOwnedSlotsTooltip(Widget w, string name, string desc, int x = 0, int y = 0)
	{
		ItemManager.GetInstance().SetTooltipWidget(w);
		ItemManager.GetInstance().PrepareSlotsTooltip(name,desc,x,y);
		m_TooltipOwner = true;
	}
	
	void HideOwnedTooltip()
	{
		if (m_TooltipOwner)
		{
			ItemManager.GetInstance().HideTooltip();
			m_TooltipOwner = false;
		}
	}
	
	void HideOwnedSlotsTooltip()
	{
		if (m_TooltipOwner)
		{
			ItemManager.GetInstance().HideTooltipSlot();
			m_TooltipOwner = false;
		}
	}
}
