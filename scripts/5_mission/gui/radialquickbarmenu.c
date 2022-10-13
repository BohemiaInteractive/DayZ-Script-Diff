enum RadialQuickbarCategory
{
	DEFAULT,
	SPECIALIZED_LIGHTS
}

class RadialQuickbarItem
{
	protected bool 					m_IsLightSourceExtra;
	protected bool 					m_IsNVG;
	protected int 					m_Id;
	protected int 					m_Category;
	protected int 					m_CategorySwitchID;
	protected EntityAI 				m_Item;
	protected string 				m_ItemName;
	
	//radial menu
	protected Widget 			m_RadialMenuSelector;
	protected Widget			m_RadialMenuItemCard;
	
	void RadialQuickbarItem( int id, EntityAI item, string item_name, int category = RadialQuickbarCategory.DEFAULT, int category_switch = -1 )
	{
		m_Id				= id;
		m_Item 				= item;
		m_ItemName			= item_name;
		m_Category 			= category;
		m_CategorySwitchID 	= category_switch;
		
		//
		if (ItemBase.Cast(m_Item))
		{
			m_IsNVG = ItemBase.Cast(m_Item).IsNVG();
			m_IsLightSourceExtra = ItemBase.Cast(m_Item).IsLightSource();
		}
	}
	
	EntityAI GetItem()
	{
		return m_Item;
	}
	
	void SetItem( EntityAI item )
	{
		m_Item = item;
	}
	
	bool IsLightSourceExtra()
	{
		return m_IsLightSourceExtra;
	}
	
	bool IsNVGExtra()
	{
		return m_IsNVG;
	}
	
	int GetId()
	{
		return m_Id;
	}
	
	int GetItemCategory()
	{
		return m_Category;
	}
	
	int GetCategorySwitchID()
	{
		return m_CategorySwitchID;
	}
		
	Widget GetRadialItemCard()
	{
		return m_RadialMenuItemCard;
	}
	
	void SetRadialItemCard( Widget widget )
	{
		m_RadialMenuItemCard = widget;
	}
	
	string GetItemName()
	{
		return m_ItemName;
	}
}

class RadialQuickbarMenu extends UIScriptedMenu
{
	protected Widget 								m_ItemCardPanel;
	protected ref array<ref RadialQuickbarItem> 	m_Items;
	protected Widget 								m_ToolbarPanel;
	
	protected bool 									m_IsMenuClosing;
	protected int 									m_CurrentCategory;
	//
	const string 									TEXT_ITEM_NAME	= "ItemName";
	const string 									TEXT_ITEM_TITLE	= "ItemTitle";
	//selections
	protected Widget 								m_SelectedItem;
	static EntityAI									m_ItemToAssign;
	
	//instance
	static RadialQuickbarMenu						instance;
	
	//============================================
	// RadialQuickbarMenu
	//============================================
	void RadialQuickbarMenu()
	{
		m_Items = new ref array<ref RadialQuickbarItem>;
		m_CurrentCategory = RadialQuickbarCategory.DEFAULT;
		
		if ( !instance )
		{
			instance = this;
		}

		GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
	}
	
	void ~RadialQuickbarMenu()
	{
		if (GetGame() && GetGame().GetMission())
		{
			GetGame().GetMission().RemoveActiveInputExcludes({"radialmenu"},false);
		}
	}
	
	static void SetItemToAssign( EntityAI item )
	{
		m_ItemToAssign = item;
	}
	
	static EntityAI GetItemToAssign()
	{
		return m_ItemToAssign;
	}
	
	static RadialQuickbarMenu GetMenuInstance()
	{
		return instance;
	}
	
	override void OnHide()
	{
		super.OnHide();
		
		//reset item to assign
		RadialQuickbarMenu.SetItemToAssign( NULL );
	}
	
	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		#endif
	}

	//============================================
	// Menu Controls
	//============================================	
	static void OpenMenu( UIScriptedMenu parent = NULL )
	{
		GetGame().GetUIManager().EnterScriptedMenu( MENU_RADIAL_QUICKBAR, parent );
	}
	
	static void CloseMenu()
	{
		GetGame().GetUIManager().Back();
		//GetGame().GetMission().RemoveActiveInputExcludes({"radialmenu"},false);
	}
		
	//============================================
	// Init & Widget Events
	//============================================
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/radial_menu/radial_quickbar/radial_quickbar_menu.layout");
		m_ItemCardPanel = layoutRoot.FindAnyWidget(RadialMenu.RADIAL_ITEM_CARD_CONTAINER);
		
		//register gestures menu
		RadialMenu.GetInstance().RegisterClass(this);
		
		//delay updates until fully initialized
		RadialMenu.GetInstance().SetWidgetInitialized(false);
		
		//set radial menu properties
		RadialMenu.GetInstance().SetWidgetProperties("gui/layouts/radial_menu/radial_quickbar/radial_quickbar_delimiter.layout");

		//create content (widgets) for items
		RefreshQuickbar();
		
		//set controller toolbar icons
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		#endif
		
		m_ToolbarPanel = layoutRoot.FindAnyWidget( "toolbar_bg" );
		m_ToolbarPanel.Show( true );
				
		return layoutRoot;
	}
	
	override void OnShow()
	{
		super.OnShow();
		
		SetFocus( layoutRoot );
		m_IsMenuClosing = false;
	}	
	
	override bool OnController( Widget w, int control, int value )
	{
		super.OnController( w, control, value );
		
		RadialMenu.GetInstance().SetControlType( RadialMenuControlType.CONTROLLER );

		return false;
	}	
		
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		super.OnMouseEnter( w, x, y );
		
		RadialMenu.GetInstance().SetControlType( RadialMenuControlType.MOUSE );

		return false;
	}	

	override bool UseMouse()
	{
		return true;
	}
	
	override bool UseGamepad()
	{
		return true;
	}	
	
	//============================================
	// Content
	//============================================
	//reset_selection - if false, selected quick bar item will be remembered after content refresh
	protected void RefreshQuickbar( bool reset_selection = true )
	{
		int selected_item_id = -1;
		if ( !reset_selection )
		{
			RadialQuickbarItem quickbar_item;
			if ( instance.m_SelectedItem )
			{
				instance.m_SelectedItem.GetUserData( quickbar_item );
				selected_item_id = quickbar_item.GetId();
			}
		}
		
		GetItems( m_Items );
		//CheckForLightsAndNVG( m_Items );
		CreateContent( selected_item_id );
	}	
	
	//
	//	ITEMS
	//
	protected void GetItems( out ref array<ref RadialQuickbarItem> items )
	{
		items.Clear();
		
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		int size = player.GetQuickBarSize();
		EntityAI entity;
		
		for ( int i = 0; i < size; ++i )
		{
			entity = player.GetQuickBarEntity( i );
			
			items.Insert( new RadialQuickbarItem( i, entity, "" ) );
		}
		
		CheckForLightsAndNVG(m_Items,i);
	}
	
	protected void CheckForLightsAndNVG( out ref array<ref RadialQuickbarItem> items, int last_idx )
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		int count = 0;
		EntityAI entity;
		ItemBase headgear = ItemBase.Cast(player.FindAttachmentBySlotName("Headgear"));
		ItemBase eyewear = ItemBase.Cast(player.FindAttachmentBySlotName("Eyewear"));
		
		//nvg - headgear check
		if ( headgear )
		{
			entity = headgear.FindAttachmentBySlotName("NVG");
			if (entity)
			{
				items.Insert( new RadialQuickbarItem( count, entity, "", RadialQuickbarCategory.SPECIALIZED_LIGHTS ) );
				count++;
			}
		}
		//nvg/light - eyewear check
		if ( eyewear )
		{
			entity = eyewear.FindAttachmentBySlotName("NVG");
			if (entity)
			{
				items.Insert( new RadialQuickbarItem( count, entity, "", RadialQuickbarCategory.SPECIALIZED_LIGHTS ) );
				count++;
			}
			else if ( eyewear.IsLightSource() && eyewear.HasEnergyManager() && eyewear.GetCompEM().CanWork() )
			{
				entity = eyewear;
				items.Insert( new RadialQuickbarItem( count, entity, "", RadialQuickbarCategory.SPECIALIZED_LIGHTS ) );
				count++;
			}
		}
		//light
		if ( headgear )
		{
			if ( headgear.GetInventory().AttachmentCount() > 0 )
			{
				ItemBase attachment;
				for (int i = 0; i < headgear.GetInventory().AttachmentCount(); i++)
				{
					attachment = ItemBase.Cast(headgear.GetInventory().GetAttachmentFromIndex(i));
					if ( attachment && attachment.IsLightSource() && attachment.HasEnergyManager() && attachment.GetCompEM().CanWork() )
					{
						entity = attachment;
						items.Insert( new RadialQuickbarItem( count, entity, "", RadialQuickbarCategory.SPECIALIZED_LIGHTS ) );
						count++;
					}
				}
			}
		}
		
		//Add a category switchers
		if (m_CurrentCategory == RadialQuickbarCategory.DEFAULT && count > 0)
		{
			items.InsertAt( new RadialQuickbarItem(32,null,"#toggle_lights",RadialQuickbarCategory.DEFAULT,RadialQuickbarCategory.SPECIALIZED_LIGHTS),0 );
		}
		else if (m_CurrentCategory == RadialQuickbarCategory.SPECIALIZED_LIGHTS)
		{
			items.InsertAt( new RadialQuickbarItem(32,null,"#menu_back",RadialQuickbarCategory.SPECIALIZED_LIGHTS,RadialQuickbarCategory.DEFAULT),0 );
		}
	}
	
	protected void CreateContent( int selected_item_id = -1 )
	{
		//delete existing content
		DeleteItems();
		
		int category_item_count;
		
		for ( int i = 0; i < m_Items.Count(); ++i )
		{
			RadialQuickbarItem quickbar_item = m_Items.Get( i );
			
			if (quickbar_item.GetItemCategory() == m_CurrentCategory)
			{
				//create item card
				Widget item_card_widget = Widget.Cast( GetGame().GetWorkspace().CreateWidgets( "gui/layouts/radial_menu/radial_quickbar/radial_quickbar_item_card.layout", m_ItemCardPanel ) );
				quickbar_item.SetRadialItemCard( item_card_widget );
				
				//update item card widget
				UpdateQuickbarItemCard( quickbar_item );
				
				//set data
				item_card_widget.SetUserData( quickbar_item );
				
				//set selection
				if ( quickbar_item.GetId() == selected_item_id )
				{
					MarkSelected( quickbar_item.GetRadialItemCard() );
				}
				category_item_count++;
			}
		}	

		//adjust radial parameters for content
		if ( /*m_Items.Count()*/category_item_count > 0 ) 
		{
			RadialMenu radial_menu = RadialMenu.GetInstance();
			radial_menu.SetRadiusOffset( 0 );
			radial_menu.SetExecuteDistOffset( 0.5 );
			radial_menu.SetOffsetFromTop( 0 );
			radial_menu.SetItemCardRadiusOffset( 0.25 );
			radial_menu.ActivateControllerTimeout( false );
		}
		
		//refresh radial menu
		RadialMenu.GetInstance().Refresh( false );
	}
	
	protected void UpdateQuickbarItemCard( RadialQuickbarItem quickbar_item )
	{
		Widget item_card_widget = quickbar_item.GetRadialItemCard();

		//get content panels
		Widget item_details = item_card_widget.FindAnyWidget( "ItemDetails" );
		TextWidget item_title = TextWidget.Cast( item_card_widget.FindAnyWidget( "ItemTitle" ) );
		
		//set text
		TextWidget text_widget = TextWidget.Cast( item_card_widget.FindAnyWidget( TEXT_ITEM_NAME ) );
		EntityAI item = quickbar_item.GetItem();
		
		Widget quantity_panel = item_card_widget.FindAnyWidget( "QuantityPanel" );
		if ( item ) 
		{
			//item text
			text_widget.SetText( quickbar_item.GetItem().GetDisplayName() );
			
			//item preview
			ItemPreviewWidget item_preview = ItemPreviewWidget.Cast( item_card_widget.FindAnyWidget( "ItemPreview" ) );
			item_preview.SetItem( item );
			item_preview.SetView( item.GetViewIndex() );
			item_preview.SetModelOrientation( Vector( 0,0,0 ) );
			
			//item quantity
			Widget quantity_stack = quantity_panel.FindAnyWidget( "QuantityStackPanel" );
			ProgressBarWidget quantity_bar = ProgressBarWidget.Cast( quantity_panel.FindAnyWidget( "QuantityBar" ) );
			int has_quantity = QuantityConversions.HasItemQuantity( item );
			//calculate and set quantity
			if ( has_quantity == QUANTITY_HIDDEN )
			{
				quantity_panel.Show( false );
			}
			else if ( has_quantity == QUANTITY_COUNT )
			{
				//hide bar
				quantity_bar.Show( false );
				
				//show stack
				TextWidget quantity_text = TextWidget.Cast( quantity_stack.FindAnyWidget( "Quantity" ) );
				quantity_text.SetText( QuantityConversions.GetItemQuantityText( item ) );
				quantity_stack.Show( true );
			}
			else if ( has_quantity == QUANTITY_PROGRESS )
			{
				//hide stack
				quantity_stack.Show( false );
				
				//show bar
				float progress_max = quantity_bar.GetMax();
				int max = item.ConfigGetInt( "varQuantityMax" );
				int count = item.ConfigGetInt( "count" );
				float quantity = QuantityConversions.GetItemQuantity( ItemBase.Cast( item ) );
				
				if ( count > 0 )
				{
					max = count;
				}
				if ( max > 0 )
				{

					float value = Math.Round( ( quantity / max ) * 100 );
					quantity_bar.SetCurrent( value );
				}
				
				quantity_bar.Show( true );
			}
			
			//display content panels
			item_details.Show( true );
			item_title.Show( false );
		}
		else if ( quickbar_item.GetCategorySwitchID() != -1 )
		{
			item_title.SetText( quickbar_item.GetItemName() );
			
			item_details.Show( false );
			item_title.Show( true );	
		}
		else
		{
			item_title.SetText( "#container_empty" );
			
			//display content panels
			item_details.Show( false );
			item_title.Show( true );			
		}			
	}
	
	//Common
	protected void DeleteItems()
	{
		Widget child;
		Widget child_to_destroy;
		
		child = m_ItemCardPanel.GetChildren();
		while ( child )
		{
			child_to_destroy = child;
			child = child.GetSibling();
			
			delete child_to_destroy;
		}		
	}
	
	protected void ChangeCurrentCategory(int category)
	{
		m_CurrentCategory = category;
		RefreshQuickbar(false);
		UpdateControlsElements();
	}
	
	//============================================
	// Radial Menu Events
	//============================================
	//Common
	void OnControlsChanged( RadialMenuControlType type )
	{
	}
	
	//Mouse
	void OnMouseSelect( Widget w )
	{
		MarkSelected( w );
	}

	void OnMouseDeselect( Widget w )
	{
		UnmarkSelected( w );
	}

	void OnMouseExecute( Widget w )
	{
	}
	
	//! LMB
	void OnMousePressLeft( Widget w )
	{
		PrimaryAction( w );
	}
	
	//! RMB
	void OnMousePressRight( Widget w )
	{
		BackOneLevel();
	}
	
	//Controller
	void OnControllerSelect( Widget w )
	{
		MarkSelected( w );
	}

	void OnControllerDeselect( Widget w )
	{
		UnmarkSelected( w );
	}

	void OnControllerPressSelect( Widget w )
	{
		PrimaryAction( w );
	}
	
	void OnControllerPressBack( Widget w )
	{
		//SecondaryAction( w );
		BackOneLevel();
	}
	
	//Actions
	protected void MarkSelected( Widget w )
	{
		m_SelectedItem = w;
		
		if (w)
		{
			RadialQuickbarItem quickbar_item;
			w.GetUserData( quickbar_item );
			ItemBase item;
			
			if (quickbar_item && Class.CastTo(item,quickbar_item.GetItem()))
			{
				w.SetFlags(WidgetFlags.DISABLED);
			}
			else
			{
				w.ClearFlags(WidgetFlags.DISABLED);
			}
/*
			//is not category
			if ( quickbar_item )
			{
				if ( quickbar_item.GetItem() )
				{
					//alter item visual
					TextWidget text_widget = TextWidget.Cast( quickbar_item.GetRadialItemCard().FindAnyWidget( TEXT_ITEM_NAME ) );
					text_widget.SetColor( ARGB( 255, 66, 175, 95 ) );
				}
				else
				{
					//alter item visual
					TextWidget title_widget = TextWidget.Cast( quickbar_item.GetRadialItemCard().FindAnyWidget( TEXT_ITEM_TITLE ) );
					title_widget.SetColor( ARGB( 255, 66, 175, 95 ) );
				}
			}
*/
		}
	}
	
	protected void UnmarkSelected( Widget w )
	{
		m_SelectedItem = NULL;
		
		/*
		if ( w )
		{
			RadialQuickbarItem quickbar_item;
			w.GetUserData( quickbar_item );

			//is not category
			if ( quickbar_item )
			{			
				if ( quickbar_item.GetItem() )
				{
					//alter item visual
					TextWidget text_widget = TextWidget.Cast( quickbar_item.GetRadialItemCard().FindAnyWidget( TEXT_ITEM_NAME ) );
					text_widget.SetColor( ARGB( 255, 255, 255, 255 ) );
				}
				else
				{
					//alter item visual
					TextWidget title_widget = TextWidget.Cast( quickbar_item.GetRadialItemCard().FindAnyWidget( TEXT_ITEM_TITLE ) );
					title_widget.SetColor( ARGB( 255, 255, 255, 255 ) );					
				}
			}
		}
		*/
	}
	
	protected void PrimaryAction( Widget w )
	{
		if ( instance.m_SelectedItem )
		{
			if ( !GetGame().IsDedicatedServer() )
			{
				RadialQuickbarItem quickbar_item;
				instance.m_SelectedItem.GetUserData( quickbar_item );
			
				if ( quickbar_item ) 
				{
					PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
					
					//ASSIGN ACTION
					if ( GetItemToAssign() )
					{
						//assign item to slot
						if ( quickbar_item.GetItem() == GetItemToAssign() )
						{
							player.RemoveQuickBarEntityShortcut( GetItemToAssign() );
						}
						else
						{
							player.SetQuickBarEntityShortcut( GetItemToAssign(), quickbar_item.GetId() );
						}
					}
					//LIGHTS
					else if (m_CurrentCategory == RadialQuickbarCategory.SPECIALIZED_LIGHTS && quickbar_item.IsLightSourceExtra())
					{
						HandleLights(quickbar_item);
					}
					//NVG
					else if (m_CurrentCategory == RadialQuickbarCategory.SPECIALIZED_LIGHTS && quickbar_item.IsNVGExtra())
					{
						HandleNVG(quickbar_item);
					}
					//change quickbar category
					else if (quickbar_item.GetCategorySwitchID() != -1)
					{
						ChangeCurrentCategory(quickbar_item.GetCategorySwitchID());
						return;
					}
					//SWAP
					else
					{
						EntityAI item = quickbar_item.GetItem();
						
						if ( item )
						{
							//swap
							player.RadialQuickBarSingleUse( quickbar_item.GetId() + 1 );				//id must begin with 1 (simulating key press 1-9)
						}
					}
					
					RefreshQuickbar( false );
				}
			}
		}
	}
	
	protected void SecondaryAction( Widget w )
	{
		if ( instance.m_SelectedItem && m_CurrentCategory == RadialQuickbarCategory.DEFAULT )
		{
			if ( !GetGame().IsDedicatedServer() )
			{
				RadialQuickbarItem quickbar_item;
				instance.m_SelectedItem.GetUserData( quickbar_item );
			
				if ( quickbar_item ) 
				{
					PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
					EntityAI item = quickbar_item.GetItem();
					
					if ( item )
					{
						player.RadialQuickBarCombine( quickbar_item.GetId() + 1 );	//id must begin with 1 (simulating key press 1-9)
						RefreshQuickbar( false );
					}	
				}
			}
		}
	}
	
	// returns to default, missing hierarchy to properly traverse ATM
	protected void BackOneLevel()
	{
		if (m_CurrentCategory != RadialQuickbarCategory.DEFAULT)
		{
			ChangeCurrentCategory(RadialQuickbarCategory.DEFAULT);
		}
	}
	
	//-------------------------------------------
	//NVG/Light handling extension
	//-------------------------------------------
	void HandleLights(RadialQuickbarItem quickbar_item)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		ItemBase item = ItemBase.Cast(quickbar_item.GetItem());
		ActionManagerClient mngr_client = ActionManagerClient.Cast(player.GetActionManager());
		ActionTarget atrg;
		
		if ( Headtorch_ColorBase.Cast(item) )		
		{
			atrg = new ActionTarget(item,null,-1,vector.Zero,-1.0);
			if ( mngr_client.GetAction(ActionTurnOnHeadtorch).Can(player,atrg,null) )
			{
				mngr_client.PerformActionStart(player.GetActionManager().GetAction(ActionTurnOnHeadtorch),atrg,null);
			}
			else if ( mngr_client.GetAction(ActionTurnOffHeadtorch).Can(player,atrg,null) )
			{
				mngr_client.PerformActionStart(player.GetActionManager().GetAction(ActionTurnOffHeadtorch),atrg,null);
			}
		}
		else if ( Mich2001Helmet.Cast(item.GetHierarchyParent()) )
		{
			atrg = new ActionTarget(item.GetHierarchyParent(),null,-1,vector.Zero,-1.0);
			if ( mngr_client.GetAction(ActionTurnOnHelmetFlashlight).Can(player,atrg,null) )
			{
				mngr_client.PerformActionStart(player.GetActionManager().GetAction(ActionTurnOnHelmetFlashlight),atrg,null);
			}
			else if ( mngr_client.GetAction(ActionTurnOffHelmetFlashlight).Can(player,atrg,null) )
			{
				mngr_client.PerformActionStart(player.GetActionManager().GetAction(ActionTurnOffHelmetFlashlight),atrg,null);
			}
		}
	}
	
	void HandleNVG(RadialQuickbarItem quickbar_item)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		ActionManagerClient mngr_client = ActionManagerClient.Cast(player.GetActionManager());
		ActionTarget atrg;
		
		atrg = new ActionTarget(quickbar_item.GetItem().GetHierarchyParent(),null,-1,vector.Zero,-1.0);
		if ( mngr_client.GetAction(ActionToggleNVG).Can(player,atrg,null) )
		{
			mngr_client.PerformActionStart(player.GetActionManager().GetAction(ActionToggleNVG),atrg,null);
		}
	}
	
	bool IsMenuClosing()
	{
		return m_IsMenuClosing;
	}
	
	void SetMenuClosing(bool state)
	{
		m_IsMenuClosing = state;
	}

	protected void UpdateControlsElements()
	{
		Widget toolbarBackSpacer = layoutRoot.FindAnyWidget("BackSpacer");
		//Widget toolbarNavigateSpacer = layoutRoot.FindAnyWidget("Navigate");
		
		RichTextWidget toolbarSelectIcon = RichTextWidget.Cast(layoutRoot.FindAnyWidget("SelectIcon"));
		RichTextWidget toolbarBackIcon = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		
		string selectAction;
		string backAction;
		int controllerID;
		
		if (GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
		{
			selectAction = "UAMenuSelect";
			backAction = "UAMenuBack";
			controllerID = EUAINPUT_DEVICE_KEYBOARDMOUSE;
		}
		else
		{
			selectAction = "UAUISelect";
			backAction = "UAUIBack";
			controllerID = EUAINPUT_DEVICE_CONTROLLER;
		}
		
		toolbarSelectIcon.SetText(InputUtils.GetRichtextButtonIconFromInputAction(selectAction, "", controllerID, InputUtils.ICON_SCALE_TOOLBAR));
		toolbarBackIcon.SetText(InputUtils.GetRichtextButtonIconFromInputAction(backAction, "", controllerID, InputUtils.ICON_SCALE_TOOLBAR));
		toolbarBackSpacer.Show(m_CurrentCategory != RadialQuickbarCategory.DEFAULT);
	}
}
