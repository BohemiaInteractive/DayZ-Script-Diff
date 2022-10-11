class AttachmentCategoriesContainer: CollapsibleContainer
{
	protected ref map<string, int>							m_CategorySlotIndex;
	protected int											m_SlotsCount;
	protected SlotsIcon										m_CargoSlotsIcon;

	void AttachmentCategoriesContainer( LayoutHolder parent, int sort = -1 )
	{
		m_CategorySlotIndex = new map<string, int>;
	}
	
	void SetEntity( EntityAI entity )
	{
		m_Entity = entity;
		InitIconsContainers();
		
		m_MainWidget = m_RootWidget.FindAnyWidget( "body" );
		WidgetEventHandler.GetInstance().RegisterOnChildAdd( m_MainWidget, this, "OnChildAdd" );
		WidgetEventHandler.GetInstance().RegisterOnChildRemove( m_MainWidget, this, "OnChildRemove" );
		
		InitGhostSlots();
		
		( Container.Cast( m_Parent ) ).m_Body.Insert( this );
		m_Parent.Refresh();
		SetHeaderName();
		RecomputeOpenedContainers();
	}

	void SetHeaderName()
	{
		m_CollapsibleHeader.SetName( m_Entity.GetDisplayName() );
	}
	
	override bool IsDisplayable()
	{
		return !IsEmpty() || CanDisplayAnyCategory();
	}

	override void UpdateInterval()
	{
		if( m_Entity )
		{
			if( m_Entity.GetInventory().IsInventoryLockedForLockType( HIDE_INV_FROM_SCRIPT ) || m_Hidden )
			{
				if( m_Body.Count() > 0 && !m_Hidden )
					m_CollapsibleHeader.OnHide();
				HideContent();
				GetMainWidget().Show( false );
			}
			else
			{
				if( m_Body.Count() > 0 && !m_Hidden )
				{
					if( m_Body.Count() > 0 && !m_Hidden )
						m_CollapsibleHeader.OnShow();
					int i;
					for (i = m_SlotsCount; i < m_Body.Count(); i++)
					{
						AttachmentCategoriesRow row = AttachmentCategoriesRow.Cast( m_Body[i] );
						if (row)
						{
							if (m_Entity.CanDisplayAttachmentCategory( row.GetCategoryIdentifier() ))
							{
								if ( row.IsDisplayable() )
								{
									ShowInSlots( row.GetCategoryIdentifier(), true );
									if (!row.IsVisible())
									{
										row.OnShow();
									}
								}
								else
								{
									ShowInSlots( row.GetCategoryIdentifier(), false );
									if (row.IsVisible())
									{
										row.OnHide();
									}
								}
							}
							else
							{
								ShowInSlots( row.GetCategoryIdentifier(), false );
								if (row.IsVisible())
								{
									row.OnHide();
								}
							}
							row.UpdateInterval();
						}
						else
						{
							ContainerWithCargo cargo = ContainerWithCargo.Cast(m_Body[i]);
							if (cargo)
							{
								if (m_Entity.CanDisplayCargo())
								{
									m_CargoSlotsIcon.m_MainWidget.Show(true);
									if (!cargo.IsVisible())
									{
										cargo.OnShow();
									}
								}
								else
								{
									m_CargoSlotsIcon.m_MainWidget.Show(false);
									if (cargo.IsVisible())
									{
										cargo.OnHide();
									}
								}
								cargo.UpdateInterval();
							}
							RecomputeOpenedContainers();
							
						}
						
					}
					for ( i = 0; i < m_SlotsCount; i++)
					{
						AttachmentCategoriesSlotsContainer slots = AttachmentCategoriesSlotsContainer.Cast( m_Body[i] );
						{
							if (slots.IsDisplayable())
							{
								slots.OnShow();
							}
							else
							{
								slots.OnHide();
							}	
						}
					}
				}
				GetMainWidget().Show( true );
			}
		}
	}
	
	void LoadAttachmentCategoriesIcon( SlotsContainer items_cont, string icon_name, int slot_number )
	{
		SlotsIcon icon				= items_cont.GetSlotIcon( slot_number );
		ImageWidget image_widget	= icon.GetGhostSlot();
		image_widget.Show( true );
		image_widget.LoadImageFile( 0, StaticGUIUtils.VerifyIconImageString(StaticGUIUtils.IMAGESETGROUP_INVENTORY,icon_name) ); //icon_name must be in format "set:<setname> image:<imagename>"
	}

	int GetAttachmentCategoriesCount( string config_path )
	{
		return GetGame().ConfigGetChildrenCount( config_path );
	}

	SlotsContainer GetSlotsContainer( int icons_row )
	{
		AttachmentCategoriesSlotsContainer items_cont = AttachmentCategoriesSlotsContainer.Cast(m_Body.Get(icons_row));
		return items_cont.GetSlotsContainer();
	}
	
	SlotsIcon GetCargoSlotsIcon()
	{
		return m_CargoSlotsIcon;
	}
	
	void ShowInSlots( string category, bool show )
	{
		int index						= m_CategorySlotIndex.Get( category );
		int slot_number					= index % ITEMS_IN_ROW;
		
		SlotsContainer items_cont		= GetSlotsContainer( index / ITEMS_IN_ROW );
		if( items_cont )
		{
			Widget icon_widget			= items_cont.GetSlotIcon( slot_number ).GetMainWidget();
			if( icon_widget )
				icon_widget.Show( show );
			icon_widget.GetParent().Update();
			icon_widget.GetParent().GetParent().Update();
			icon_widget.GetParent().GetParent().GetParent().Update();
		}
	}
	
	//oof
	override bool CanDisplayAnyCategory()
	{
		int count = m_Body.Count();
		AttachmentCategoriesRow row;
		for (int i = m_SlotsCount; i < count; i++)
		{
			if (Class.CastTo(row,m_Body[i]) && m_Entity.CanDisplayAttachmentCategory( row.GetCategoryIdentifier() ))
			{
				return true;
			}
		}
		return super.CanDisplayAnyCategory();
	}
	
	override void UpdateRadialIcon()
	{
		if ( m_SlotIcon )
		{
			if (m_Entity.CanDisplayCargo() || CanDisplayAnyCategory())
			{
				m_SlotIcon.GetRadialIconPanel().Show( true );
				SetOpenForSlotIcon(!m_Hidden);
			}
			else
			{
				m_SlotIcon.GetRadialIconPanel().Show( false );
			}
		}
	}
	
	string GetAttachmentCategory( string config_path_attachment_categories, int i )
	{
		string attachment_category;
		GetGame().ConfigGetChildName(config_path_attachment_categories, i, attachment_category);
		return attachment_category;
	}

	string GetIconName( string config_path_attachment_categories, string attachment_category )
	{
		string icon_path = config_path_attachment_categories+ " " + attachment_category + " icon";
		string icon_name;
		GetGame().ConfigGetText(icon_path, icon_name);
		return icon_name;
	}
	
	int GetViewIndex( string config_path_attachment_categories, string attachment_category )
	{
		string preview_path = config_path_attachment_categories+ " " + attachment_category + " view_index";
		return GetGame().ConfigGetInt( preview_path );;
	}

	void MouseClick( Widget w )
	{
		SlotsIcon slots_icon;
		w.GetUserData(slots_icon);
		
		ClosableContainer c = ClosableContainer.Cast( slots_icon.GetContainer() );
		if( c )
		{
			if( c.IsOpened() )
			{
				c.Close();
			}
			else
			{
				c.Open();
			}
		}
	}
	
	override void ExpandCollapseContainer()
	{
		if (m_OpenedContainers.Count() > m_ActiveIndex)
		{
			//c - container where selected icon is part of
			Container c = Container.Cast(m_OpenedContainers.Get( m_ActiveIndex ));
			//cc - container connected to selected icon (this container will be close/open)
			ClosableContainer cc;
			//icon - selected icon
			SlotsIcon icon = c.GetFocusedSlotsIcon();
			
			if (icon)
			{
				cc = ClosableContainer.Cast(icon.GetContainer());
			}
			
			if (cc)
			{
				
				cc.Toggle();
				RecomputeOpenedContainers();
			}
		}
	}
	
	bool IsHeaderActive()
	{
		return m_CollapsibleHeader.IsActive();
	}
	
	void InitIconsContainers()
	{
		m_SlotsCount = 0;
		
		string type = m_Entity.GetType();
		string config_path_attachment_categories = "CfgVehicles " + type + " GUIInventoryAttachmentsProps";
		int attachments_categories_count = GetAttachmentCategoriesCount( config_path_attachment_categories );
		int categories_count = attachments_categories_count;
		if (m_Entity.GetInventory().GetCargo())
		{
			categories_count++;
		}
		int row_count = categories_count / ITEMS_IN_ROW;
		if (categories_count % ITEMS_IN_ROW != 0)
		{
			row_count++;
		}
		for ( int i = 0; i < row_count; i++ )
		{
			ref AttachmentCategoriesSlotsContainer items_cont = new AttachmentCategoriesSlotsContainer( this, i );
			m_Body.Insert( items_cont );
			m_OpenedContainers.Insert( items_cont );
			
			//TODO MW find better way
			if (i == ( row_count - 1 ) && categories_count % ITEMS_IN_ROW != 0)
				items_cont.GetSlotsContainer().SetColumnCount( categories_count % ITEMS_IN_ROW );
			else
				items_cont.GetSlotsContainer().SetColumnCount( ITEMS_IN_ROW );
		}
		m_SlotsCount = row_count;
	}
	
	void InitGhostSlots()
	{
		string type = m_Entity.GetType();
		string config_path_attachment_categories = "CfgVehicles " + type + " GUIInventoryAttachmentsProps";

		int attachments_categories_count = GetAttachmentCategoriesCount( config_path_attachment_categories );

		SlotsContainer items_cont;
		SlotsIcon icon;
		string attachment_category;
		string icon_name;
		
		for (int i = 0; i < attachments_categories_count; i++)
		{
			items_cont = GetSlotsContainer( i / ITEMS_IN_ROW );
			attachment_category = GetAttachmentCategory( config_path_attachment_categories, i );
			icon_name = GetIconName( config_path_attachment_categories, attachment_category );

			if ( items_cont )
			{
				int slot_number = i % ITEMS_IN_ROW;
				m_CategorySlotIndex.Insert( attachment_category, i );
				icon = items_cont.GetSlotIcon( slot_number );
				icon.GetGhostSlot().SetFlags( WidgetFlags.IGNOREPOINTER );
				
				LoadAttachmentCategoriesIcon( items_cont, icon_name, slot_number );
				
				string config = config_path_attachment_categories + " " + attachment_category + " name";
				string name;
		
				GetGame().ConfigGetText(config,name);
				icon.SetSlotDisplayName(name);
				
				AttachmentCategoriesRow ar;

				ar = new AttachmentCategoriesRow( this, -1 );
				ar.Init(attachments_categories_count, i, attachment_category, config_path_attachment_categories, m_Entity, m_Body.Count() );

				//Insert(ar);
				ar.SetSlotIcon(icon);
				//icon.SetContainer(ar);
				
				icon.GetRadialIconPanel().Show( true );
				ar.Open();		
				icon.SetContainer(ar);
				Insert(ar);
			}
		}
		
		if ( m_Entity.GetInventory().GetCargo() )
		{
			items_cont = GetSlotsContainer( m_SlotsCount - 1 );
			if ( items_cont )
			{
				icon = items_cont.GetSlotIcon( attachments_categories_count );
				icon.GetGhostSlot().Show( true );
				icon.GetGhostSlot().LoadImageFile( 0, StaticGUIUtils.VerifyIconImageString(StaticGUIUtils.IMAGESETGROUP_INVENTORY,m_Entity.ConfigGetString("GUIInventoryCargoIcon")) );
				icon.SetSlotDisplayName(m_Entity.ConfigGetString("GUIInventoryCargoName"));
				icon.GetGhostSlot().SetFlags( WidgetFlags.IGNOREPOINTER );
				
				icon.GetRadialIconPanel().Show( true );
				icon.GetMainWidget().Show( true );
				
				ContainerWithCargo iwc = new ContainerWithCargo( this, -1 );
				iwc.Get( 0 ).GetRootWidget().ClearFlags( WidgetFlags.DRAGGABLE );
				iwc.SetEntity( m_Entity, 0, false );
				iwc.SetSlotIcon( icon );
				iwc.Open();
				
				icon.SetContainer(iwc);
				m_CargoSlotsIcon = icon;
			}
		}

		RecomputeOpenedContainers();
	}
	

	override void OnDropReceivedFromHeader( Widget w, int x, int y, Widget receiver )
	{
		ItemPreviewWidget iw = ItemPreviewWidget.Cast( w.FindAnyWidget("Render") );
		if(!iw)
		{
			string name = w.GetName();
			name.Replace("PanelWidget", "Render");
			iw = ItemPreviewWidget.Cast( w.FindAnyWidget(name) );
		}
		if(!iw)
		{
			iw = ItemPreviewWidget.Cast( w );
		}
		if( !iw.GetItem() )
			return;
		if( m_Entity.GetInventory().CanAddAttachment( iw.GetItem() ) && iw.GetItem().GetInventory().CanRemoveEntity() )
		{
			GetGame().GetPlayer().PredictiveTakeEntityToTargetAttachment( m_Entity, iw.GetItem() );
		}
		else if( m_Entity.GetInventory().CanAddEntityToInventory( iw.GetItem() ) && iw.GetItem().GetInventory().CanRemoveEntity() )
		{
			GetGame().GetPlayer().PredictiveTakeEntityToTargetInventory( m_Entity, FindInventoryLocationType.ANY, iw.GetItem() );
		}
	}
	
	override void DraggingOverHeader( Widget w, int x, int y, Widget receiver )
	{
		if( w == null )
		{
			return;
		}
		ItemPreviewWidget iw = ItemPreviewWidget.Cast( w.FindAnyWidget("Render") );
		if(!iw)
		{
			string name = w.GetName();
			name.Replace("PanelWidget", "Render");
			iw = ItemPreviewWidget.Cast( w.FindAnyWidget(name) );
		}
		if(!iw)
		{
			iw = ItemPreviewWidget.Cast( w );
		}
		if( !iw || !iw.GetItem() )
		{
			return;
		}
		
		ItemManager.GetInstance().ShowSourceDropzone( iw.GetItem() );
		ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
	}
	
	/*override void CollapseButtonOnMouseButtonDown(Widget w)
	{
		super.CollapseButtonOnMouseButtonDown(w);
		RecomputeOpenedContainers();
	}*/
	override void CollapseButtonOnMouseButtonDown( Widget w )
	{
		if( !m_Hidden )
		{
			for (int i = 1; i < m_Body.Count(); i++)
			{
				m_Body.Get( i ).OnHide();
				Container c = Container.Cast(m_Body.Get(i));
				if (c)
				{
					c.Close();
				}
			}

			//m_Hidden = true;
			OnHide();
		}
		else
		{
			//m_Hidden = false;
			OnShow();
		}
		m_Closed = m_Hidden;
		
		SetCollapsibleHeaderArrowState(m_Hidden);
		if (m_CollapsibleHeader)
		{
			m_CollapsibleHeader.SetHeaderVisible(true);
		}

		UpdateCollapseButtons();
		RecomputeOpenedContainers();
	}
	
	override void Open()
	{
		if( IsDisplayable() )
		{
			super.Open();
			//ItemManager.GetInstance().SetDefaultOpenState( m_Entity.GetType(), true );
			SetOpenForSlotIcon(true);
			OnShow();
			//m_Parent.m_Parent.Refresh();
		}
	}

	override void Close()
	{
		//ItemManager.GetInstance().SetDefaultOpenState( m_Entity.GetType(), false );
		super.Close();
		
		SlotsIcon icon = null;
		if (m_CargoSlotsIcon && m_CargoSlotsIcon.GetObject())
		{
			icon = m_CargoSlotsIcon;
		}
		else if (m_SlotIcon && m_SlotIcon.GetObject())
		{
			icon = m_SlotIcon;
		}
		/*else
		{
			Print("Dbg | no object in icon here!");
		}
		*/
		
		SetOpenForSlotIcon(false,icon);
		OnHide();
	}
	
	override void OnHide()
	{
		if (m_CollapsibleHeader)
		{
			bool b1 = !m_SlotIcon && !m_CargoSlotsIcon;
			bool b2 = (!m_SlotIcon || !m_SlotIcon.IsVisible()) && m_CargoSlotsIcon && m_CargoSlotsIcon.IsVisible();
			m_CollapsibleHeader.SetHeaderVisible(b1 || b2);
		}
		super.OnHide();
	}
	
	/*override void Refresh()
	{
		super.Refresh();
		
		if (m_CollapsibleHeader)
		{
			bool show = false;
			Container cont;
			int count = m_Body.Count();
			for ( int i = 0; i < count; i++ )
			{
				cont = Container.Cast(m_Body.Get( i ));
				if (cont && cont.IsOpened())
				{
					show = true;
					break;
				}
			}
			
			m_CollapsibleHeader.SetArrowButtonOpened(!show);
		}
	}*/
}
