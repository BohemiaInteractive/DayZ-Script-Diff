class SlotsIcon: LayoutHolder
{
	protected static int 			m_NormalWidth;
	protected static int 			m_NormalHeight;
	
	protected bool					m_IsWeapon			= false;
	protected bool					m_IsMagazine		= false;
	protected bool					m_HasTemperature	= false;
	protected bool					m_HasQuantity		= false;
	protected bool					m_HasItemSize		= false;
	protected float					m_CurrQuantity		= -1;
	
	protected EntityAI				m_Obj;
	protected ItemBase				m_Item;
	protected EntityAI				m_SlotParent;
	protected Container				m_Container;
	protected int					m_SlotID;
	protected bool					m_IsDragged			= false;
	
	protected Widget				m_PanelWidget;
	
	protected Widget				m_CursorWidget;
	protected Widget				m_ColWidget;
	protected Widget				m_MountedWidget;
	protected Widget				m_OutOfReachWidget;
	protected Widget				m_ReservedWidget;
	
	protected ItemPreviewWidget		m_ItemPreview;
	protected ImageWidget			m_GhostSlot;
	
	protected Widget				m_ColorWidget;
	protected Widget				m_SelectedPanel;
	protected Widget				m_EmptySelectedPanel;
	protected Widget				m_MicromanagedPanel;
	
	protected Widget				m_QuantityPanel;
	protected TextWidget			m_QuantityItem;
	protected ProgressBarWidget		m_QuantityProgress;
	protected Widget				m_QuantityStack;
	
	protected string				m_SlotDisplayName;
	protected string				m_SlotDesc;
	
	protected Widget				m_ItemSizePanel;
	protected TextWidget			m_ItemSizeWidget;
	
	protected Widget				m_RadialIconPanel;
	protected Widget				m_RadialIconClosed;
	protected Widget				m_RadialIcon;

	protected bool					m_Reserved;

	protected ImageWidget				m_AmmoIcon;
	protected ref array<ImageWidget>	m_AmmoIcons;

	void SlotsIcon( LayoutHolder parent, Widget root, int index, EntityAI slot_parent )
	{
		m_MainWidget			= root;
		
		m_PanelWidget			= m_MainWidget.FindAnyWidget( "PanelWidget" + index );
		
		m_CursorWidget			= m_MainWidget.FindAnyWidget( "Cursor" + index );
		m_ColWidget				= m_MainWidget.FindAnyWidget( "Col" + index );
		m_MountedWidget			= m_MainWidget.FindAnyWidget( "Mounted" + index );
		m_OutOfReachWidget		= m_MainWidget.FindAnyWidget( "OutOfReach" + index );

		m_ItemPreview			= ItemPreviewWidget.Cast( m_MainWidget.FindAnyWidget( "Render" + index ) );
		m_ItemPreview.SetForceFlipEnable(true);
		m_ItemPreview.SetForceFlip(false);
		
		m_GhostSlot				= ImageWidget.Cast( m_MainWidget.FindAnyWidget( "GhostSlot" + index ) );
		
		m_ColorWidget			= m_MainWidget.FindAnyWidget( "Color" + index );
		m_SelectedPanel			= m_MainWidget.FindAnyWidget( "Selected" + index );
		m_EmptySelectedPanel    = m_MainWidget.FindAnyWidget( "EmptySelected" + index );
		m_MicromanagedPanel    = m_MainWidget.FindAnyWidget( "Micromanaged" + index );
		
		m_QuantityPanel			= m_MainWidget.FindAnyWidget( "QuantityPanel" + index );
		m_QuantityItem			= TextWidget.Cast( m_MainWidget.FindAnyWidget( "Quantity" + index ) );
		m_QuantityProgress		= ProgressBarWidget.Cast( m_MainWidget.FindAnyWidget( "QuantityBar" + index ) );
		m_QuantityStack			= m_MainWidget.FindAnyWidget( "QuantityStackPanel" + index );
		
		m_ItemSizePanel			= m_MainWidget.FindAnyWidget( "ItemSizePanel" + index );
		m_ItemSizeWidget		= TextWidget.Cast( m_MainWidget.FindAnyWidget( "ItemSize" + index ) );
		
		m_AmmoIcon				= ImageWidget.Cast(m_MainWidget.FindAnyWidget( "AmmoIcon" + index ));
		
		m_RadialIconPanel		= m_MainWidget.FindAnyWidget( "RadialIconPanel" + index );
		m_RadialIconClosed		= m_MainWidget.FindAnyWidget( "RadialIconClosed" + index );
		m_RadialIcon			= m_MainWidget.FindAnyWidget( "RadialIcon" + index );
		
		m_ReservedWidget = Widget.Cast( GetGame().GetWorkspace().CreateWidgets( "gui/layouts/inventory_new/reserved_icon.layout", m_MainWidget ) );
		m_ReservedWidget.Show(false);
		
		WidgetEventHandler.GetInstance().RegisterOnMouseEnter( m_PanelWidget,  this, "MouseEnter" );
		WidgetEventHandler.GetInstance().RegisterOnMouseLeave( m_PanelWidget,  this, "MouseLeave" );
		
		WidgetEventHandler.GetInstance().RegisterOnMouseEnter( m_GhostSlot,  this, "MouseEnterGhostSlot" );
		WidgetEventHandler.GetInstance().RegisterOnMouseLeave( m_GhostSlot,  this, "MouseLeaveGhostSlot" );
		
		WidgetEventHandler.GetInstance().RegisterOnMouseEnter( m_RadialIconPanel,  this, "MouseEnterCategory" );
		WidgetEventHandler.GetInstance().RegisterOnMouseLeave( m_RadialIconPanel,  this, "MouseLeaveCategory" );
		
		WidgetEventHandler.GetInstance().RegisterOnDrag( m_PanelWidget, this, "OnIconDrag" );
		WidgetEventHandler.GetInstance().RegisterOnDrop( m_PanelWidget, this, "OnIconDrop" );
		
		m_Reserved 				= false;
		m_SlotID 				= -1;
		m_Item					= null;
		m_Obj					= null;
		m_Container				= null;
		
		m_SlotParent			= slot_parent;
		
		m_PanelWidget.SetUserData(this);
		m_ItemPreview.SetUserData(this);
		m_GhostSlot.SetUserData(this);
		m_MainWidget.SetUserData(this);
		
		float w,h;
		root.GetSize(w,h);
		
		m_NormalWidth = w;
		m_NormalHeight = h;
		
		SetActive( false );
	}
	
	override bool IsVisible()
	{
		return m_MainWidget.IsVisible();
	}
	
	void SetSlotParent( EntityAI parent)
	{
		m_SlotParent = parent;
	}
	
	void SetContainer( Container container )
	{
		m_Container = container;
	}
	
	void SetSlotDisplayName( string text )
	{
		m_SlotDisplayName = text;
	}
	
	Container GetContainer()
	{
		return m_Container;
	}
	
	string GetSlotDisplayName( )
	{
		return m_SlotDisplayName;
	}
	
	void SetSlotDesc( string text )
	{
		m_SlotDesc = text;
	}
	
	string GetSlotDesc()
	{
		return m_SlotDesc;
	}
	
	void ~SlotsIcon()
	{
		if (m_IsDragged)
		{
			m_IsDragged = false;
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().SetIsDragging( false );
		}
		
		if (m_Obj)
		{
			m_Obj.GetOnItemFlipped().Remove( UpdateFlip );
			m_Obj.GetOnViewIndexChanged().Remove( SetItemPreview );
		}
	}
	
	EntityAI GetSlotParent()
	{
		return m_SlotParent;
	}
	
	int GetSlotID()
	{
		return m_SlotID;
	}
	
	void SetSlotID(int slot_ID)
	{
		m_SlotID = slot_ID;
	}
	
	bool IsReserved()
	{
		return m_Reserved;
	}
	
	Widget GetPanelWidget()
	{
		return m_PanelWidget;
	}
	
	Widget GetCursorWidget()
	{
		return m_CursorWidget;
	}
	
	Widget GetColWidget()
	{
		return m_ColWidget;
	}
	
	Widget GetReservedWidget()
	{
		return m_ReservedWidget;
	}
	
	Widget GetMountedWidget()
	{
		return m_MountedWidget;
	}
	
	ItemPreviewWidget GetRender()
	{
		return m_ItemPreview;
	}

	ImageWidget GetGhostSlot()
	{
		return m_GhostSlot;
	}

	Widget GetColorWidget()
	{
		return m_ColorWidget;
	}

	Widget GetSelectedPanel()
	{
		return m_SelectedPanel;
	}
	
	Widget GetEmptySelectedPanel()
	{
		return m_EmptySelectedPanel;
	}
	
	Widget GetMicromanagedPanel()
	{
		return m_MicromanagedPanel;
	}

	Widget GetQuantityPanel()
	{
		return m_QuantityPanel;
	}
	
	Widget GetOutOfReachWidget()
	{
		return m_OutOfReachWidget;
	}

	TextWidget GetQuantityItem()
	{
		return m_QuantityItem;
	}
	
	ProgressBarWidget GetQuantityProgress()
	{
		return m_QuantityProgress;
	}
	
	Widget GetQuantityStack()
	{
		return m_QuantityStack;
	}

	Widget GetItemSizePanel()
	{
		return m_ItemSizePanel;
	}
	
	TextWidget GetItemSizeWidget()
	{
		return m_ItemSizeWidget;
	}

	ImageWidget GetAmmoIcon()
	{
		return m_AmmoIcon;
	}

	Widget GetRadialIconPanel()
	{
		return m_RadialIconPanel;
	}

	Widget GetRadialIconClosed()
	{
		return m_RadialIconClosed;
	}

	Widget GetRadialIcon()
	{
		return m_RadialIcon;
	}
	
	bool IsFocused()
	{
		return GetCursorWidget().IsVisible();
	}

	override void SetActive( bool active )
	{
#ifdef PLATFORM_CONSOLE	
		super.SetActive( active );
		float x, y;
		if( active && GetObject() )
		{
			GetMainWidget().GetScreenPos( x, y );
			PrepareOwnedTooltip( EntityAI.Cast( GetObject() ), x, y );
		}
		else if (active)
		{
			GetMainWidget().GetScreenPos( x, y );
			PrepareOwnedSlotsTooltip( GetMainWidget(), m_SlotDisplayName, m_SlotDesc, x, y );
		}
		
		m_SelectedPanel.Show( active );
#endif
	}
	
	override void SetLayoutName()
	{
		m_LayoutName = "";
	}

	override void Refresh()
	{
		//if (m_HasTemperature)
			SetTemperature();
		if (m_IsWeapon)
			RefreshMuzzleIcon();
		if (m_HasQuantity)
			SetQuantity();
	}

	void SetTemperature()
	{
		ItemManager.GetInstance().SetTemperature(EntityAI.Cast( m_Obj ), m_ItemPreview);
	}

	Object GetObject()
	{
		return m_Obj;
	}
	
	EntityAI GetEntity()
	{
		return EntityAI.Cast( m_Obj );
	}
	
	ItemBase GetItem()
	{
		return m_Item;
	}
	
	void RefreshMuzzleIcon()
	{
		Weapon_Base wpn = Weapon_Base.Cast(GetObject());
		if (wpn)
		{
			int i;
			if (!wpn.IsShowingChamberedBullet())
			{
				for (i = 0; i < m_AmmoIcons.Count(); i++)
					m_AmmoIcon = m_AmmoIcons.Get(i);
			}
			else
			{
				for (i = 0; i < wpn.GetMuzzleCount(); i++)
				{
					if (i > m_AmmoIcons.Count())
						break;
					
					m_AmmoIcon = m_AmmoIcons.Get(i);
					if (wpn.IsChamberFull(i))
					{
						m_AmmoIcon.Show(true);
						if (wpn.IsJammed())
						{
							m_AmmoIcon.Show(true);
							m_AmmoIcon.SetImage(2);
						}
						else if (wpn.IsChamberFiredOut(i))
						{
							m_AmmoIcon.Show(true);
							m_AmmoIcon.SetImage(1);
						}
						else
						{
							m_AmmoIcon.Show(true);
							m_AmmoIcon.SetImage(0);
						}
					}
					else
					{
						m_AmmoIcon.Show(false);
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
					string q_text = QuantityConversions.GetItemQuantityText(m_Item, true);

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
					float quantity 		= m_CurrQuantity;
	
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
	
	void SetItemPreview()
	{
		m_ItemPreview.Show( true );
		m_ItemPreview.SetItem( EntityAI.Cast( m_Obj ) );
		m_ItemPreview.SetModelOrientation( "0 0 0" );
		m_ItemPreview.SetView( m_Obj.GetViewIndex() );
	}
	
	void SetItemSize()
	{
		if( m_HasItemSize )
		{
			int size_x, size_y;
			GetGame().GetInventoryItemSize( InventoryItem.Cast( m_Obj ), size_x, size_y );
			
			m_ItemSizePanel.Show( true );
			m_ItemSizeWidget.SetText( ( size_x * size_y ).ToString() );
		}
	}
	
	void UpdateFlip( bool flipped )
	{
		if( !m_Reserved )
		{
			float x_content, y_content;
			GetPanelWidget().GetScreenSize( x_content, y_content );
			GetPanelWidget().SetSize( y_content, x_content );
		}
	}

	void Init( EntityAI obj, bool reservation = false )
	{
		if( m_Obj != obj )
		{
			ClearRemainSelected();
			m_Obj	= obj;
			m_Item	= ItemBase.Cast( m_Obj );
			m_Obj.GetOnItemFlipped().Insert( UpdateFlip );
			m_Obj.GetOnViewIndexChanged().Insert( SetItemPreview );
			m_Reserved = reservation;
			m_Container	= null;
			
			if(reservation)
			{
				ItemManager.GetInstance().SetWidgetDraggable( m_PanelWidget, false );
				
				m_IsWeapon = false;
				m_IsMagazine = false;
				m_HasTemperature = false;
				m_HasQuantity = false;
				m_HasItemSize = false;
			}
			else
			{
				CheckIsWeapon();
				CheckIsMagazine();
				CheckHasTemperature();
				CheckHasQuantity();
				CheckHasItemSize();
			}

			m_ReservedWidget.Show(reservation);

			SetItemPreview();
			
			m_GhostSlot.Show( false );
			m_PanelWidget.Show( true );
			
			Refresh();
#ifdef PLATFORM_CONSOLE			
			if ( IsFocused() )
			{
				Inventory.GetInstance().UpdateConsoleToolbar();
			}
#endif
		}
	}
	
	void ClearRemainSelected()
	{
		if (m_IsDragged)
		{
			OnIconDrop(m_PanelWidget);
			Widget a = CancelWidgetDragging();
			ItemManager.GetInstance().SetWidgetDraggable( a, false );
		}
		if (m_Obj)
		{
			m_Obj.GetOnItemFlipped().Remove( UpdateFlip );
			m_Obj.GetOnViewIndexChanged().Remove( SetItemPreview );
			HideOwnedTooltip();
		}
			
		m_Obj = null;
		m_Item = null;
		m_Container	= null;
		
		m_ItemPreview.Show( false );
		m_ItemPreview.SetItem( null );
		
		m_CurrQuantity = -1;
		m_IsWeapon = false;
		m_IsMagazine = false;
		m_HasTemperature = false;
		m_HasQuantity = false;
		m_HasItemSize = false;
		
		m_GhostSlot.Show( true );
		m_AmmoIcon.Show( false );
		m_PanelWidget.Show( false );
		m_RadialIconPanel.Show( false );
		
		m_QuantityPanel.Show( false );
		if(GetSlotID() != -1)
		{
			int stack_max = InventorySlots.GetStackMaxForSlotId( GetSlotID() );
			if(stack_max > 1)
			{
				m_QuantityPanel.Show( true );
				m_QuantityItem.SetText( string.Format("0/%1", stack_max.ToString()) );
				m_QuantityStack.Show( true );
				m_QuantityProgress.Show( false );
				m_PanelWidget.Show( true );
				m_ItemPreview.Show( true );
			}
		}
		
		m_ColWidget.Show( false );
		m_MountedWidget.Show( false );
		m_OutOfReachWidget.Show( false );
		m_ReservedWidget.Show( false );
		
		if( m_ItemSizePanel )
			m_ItemSizePanel.Show( false );
		
		if( m_ColorWidget )
			m_ColorWidget.Show( false );
		
		Refresh();
	}
	
	void Clear()
	{
		#ifdef PLATFORM_CONSOLE	
		m_SelectedPanel.Show(false);
		#endif
		m_CursorWidget.Show(false);
		m_EmptySelectedPanel.Show(false);
		
		ClearRemainSelected();
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
					m_AmmoIcons[0].GetSize(width,height);
				}
				posX += width;
				
				m_AmmoIcon.SetPos(posX, 0.0, false);
				
				AmmoData data = Magazine.GetAmmoData(wpn.GetChamberAmmoTypeName(i));
				if (data)
				{
					CartridgeType cartridgeType = data.m_CartridgeType;
					switch (cartridgeType)
					{
						case CartridgeType.Pistol:
						{
							m_AmmoIcon.LoadImageFile(0, "set:dayz_gui image:cartridge_pistol");
							m_AmmoIcon.LoadImageFile(1, "set:dayz_gui image:shell_pistol");
							m_AmmoIcon.LoadImageFile(2, "set:dayz_gui image:jam_pistol");
							break;
						}
						case CartridgeType.Intermediate:
						{
							m_AmmoIcon.LoadImageFile(0, "set:dayz_gui image:cartridge_int");
							m_AmmoIcon.LoadImageFile(1, "set:dayz_gui image:shell_int");
							m_AmmoIcon.LoadImageFile(2, "set:dayz_gui image:jam_int");
							break;
						}
						case CartridgeType.FullPower:
						{
							m_AmmoIcon.LoadImageFile(0, "set:dayz_gui image:cartridge_fp");
							m_AmmoIcon.LoadImageFile(1, "set:dayz_gui image:shell_fp");
							m_AmmoIcon.LoadImageFile(2, "set:dayz_gui image:jam_fp");
							break;
						}
						case CartridgeType.Shell:
						{
							m_AmmoIcon.LoadImageFile(0, "set:dayz_gui image:cartridge_shell");
							m_AmmoIcon.LoadImageFile(1, "set:dayz_gui image:shell_shell");
							m_AmmoIcon.LoadImageFile(2, "set:dayz_gui image:jam_shell");
							break;
						}
						//@note: Crossbows cant jam so we dont add a icon for the jam state
						case CartridgeType.Arrow:
						{
							m_AmmoIcon.LoadImageFile(0, "set:dayz_gui image:arrow_int");
							m_AmmoIcon.LoadImageFile(1, "set:dayz_gui image:arrow_int");
							break;
						}
					}
				}
				m_AmmoIcons.Insert(m_AmmoIcon);
			}
		}
		else
		{
			m_IsWeapon = false;
		}
	}
	
	void CheckIsMagazine()
	{
		m_IsMagazine = ( Magazine.Cast( m_Obj ) != null );
	}
	
	void CheckHasTemperature()
	{
		if (m_Item)
		{
			m_HasTemperature = m_Item.CanHaveTemperature();
		}
	}
	
	void CheckHasQuantity()
	{
		if( m_Item )
		{
			m_HasQuantity = ( QuantityConversions.HasItemQuantity( m_Item ) != QUANTITY_HIDDEN );
			if( m_HasQuantity )
			{
				m_QuantityPanel.Show( true );
			}
		}
	}
	
	void CheckHasItemSize()
	{
		#ifdef PLATFORM_CONSOLE
		string config = "CfgVehicles " + m_Obj.GetType() + " GUIInventoryAttachmentsProps";
		m_HasItemSize = ( InventoryItem.Cast( m_Obj ) && !GetGame().ConfigIsExisting( config ) );
		#else
		m_HasItemSize = false;
		#endif
	}
	
	bool IsOutOfReach()
	{
		bool oot = ( m_OutOfReachWidget.IsVisible() || m_MountedWidget.IsVisible() );
		return oot;
	}
	
	bool MouseEnter(Widget w, int x, int y)
	{
		if( m_Reserved )
			return MouseEnterGhostSlot(w, x, y);
		
		PrepareOwnedTooltip( m_Item, x, y );
		if( GetDragWidget() != m_PanelWidget && !IsOutOfReach() )
		{
			m_CursorWidget.Show( true );
		}
		return true;
	}
	
	bool MouseEnterGhostSlot(Widget w, int x, int y)
	{
		float pos_x, pos_y;
		m_MainWidget.GetScreenPos(pos_x, pos_y);

		x = pos_x;
		y = pos_y;
		
		PrepareOwnedSlotsTooltip( m_MainWidget, m_SlotDisplayName, m_SlotDesc, x, y );
		if( GetDragWidget() != m_PanelWidget )
		{
			m_EmptySelectedPanel.Show( true );
		}
		
		
		return true;
	}

	bool MouseLeave( Widget w, Widget s, int x, int y	)
	{
		if( m_Reserved )
			return MouseLeaveGhostSlot(w, s, x, y);
		
		HideOwnedTooltip();
		if( GetDragWidget() != m_PanelWidget )
		{
			m_CursorWidget.Show( false );
		}
		return true;
	}
	
	bool MouseLeaveGhostSlot( Widget w, Widget s, int x, int y	)
	{
		HideOwnedSlotsTooltip();
		
		if( GetDragWidget() != m_PanelWidget )
		{
			m_EmptySelectedPanel.Show( false );
		}
		
		return true;
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
			if (m_HasItemSize)
				SetItemSize();
		}
	}
	
	void OnIconDrag( Widget w )
	{
		if(!m_Obj || !w)
		{
			return;
		}
		ItemManager.GetInstance().HideDropzones();
		if( m_Obj.GetHierarchyRootPlayer() == GetGame().GetPlayer() )
		{
			ItemManager.GetInstance().GetRightDropzone().SetAlpha( 1 );
		}
		else
		{
			ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
		}

		ItemManager.GetInstance().SetIsDragging( true );
		int icon_x, icon_y;
		float icon_size, y;
		int m_sizeX, m_sizeY;

		if( m_Item )
		{
			GetGame().GetInventoryItemSize( m_Item, icon_x, icon_y );
			
			CargoContainer c_parent = CargoContainer.Cast( m_Parent );
			HandsPreview h_parent = HandsPreview.Cast( m_Parent );

			if( GetRoot().m_MainWidget.FindAnyWidget( "HandsPanel" ) )
			{
				GetRoot().m_MainWidget.FindAnyWidget( "HandsPanel" ).GetScreenSize( icon_size, y );
			}
			
			icon_size = icon_size / 10;
			
			w.SetFlags( WidgetFlags.EXACTSIZE );
			m_ItemPreview.SetForceFlipEnable(false);
			
			if( !m_Item.GetInventory().GetFlipCargo() )
			{
				w.SetSize( icon_x * icon_size , icon_y * icon_size );
			}
			else
			{
				w.SetSize( icon_y * icon_size , icon_x * icon_size );
			}
		
			m_ColWidget.Show( true );
			m_CursorWidget.Show( true );
			
			ItemManager.GetInstance().SetDraggedItem( m_Item );
		}
		m_IsDragged = true;
	}
	
	void OnIconDrop( Widget w )
	{
		m_IsDragged = false;
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
		w.ClearFlags( WidgetFlags.EXACTSIZE );
		w.SetSize( 1, 1 );
		m_ColWidget.Show( false );
		m_CursorWidget.Show( false );
		m_EmptySelectedPanel.Show( false );
		m_CursorWidget.SetColor( ARGBF( 1, 1, 1, 1 ) );
		m_ItemPreview.SetForceFlipEnable(true);
	}
	
	static int GetNormalWidth()
	{
		return m_NormalWidth;
	}
	
	static int GetNormalHeight()
	{
		return m_NormalHeight;
	}
	
	/*override void HideOwnedSlotsTooltip()
	{
		if (m_TooltipOwner)
		{
			ItemManager.GetInstance().HideTooltipSlot();
			m_TooltipOwner = false;
		}
	}*/
}