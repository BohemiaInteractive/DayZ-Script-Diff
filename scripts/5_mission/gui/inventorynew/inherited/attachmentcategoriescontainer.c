class AttachmentCategoriesContainer: CollapsibleContainer
{
	protected ref map<string, int>							m_CategorySlotIndex;
	protected int											m_SlotsCount;
	protected SlotsIcon										m_CargoSlotsIcon;
	
	void AttachmentCategoriesContainer(LayoutHolder parent, int sort = -1)
	{
		m_CategorySlotIndex = new map<string, int>;
	}
	
	void ~AttachmentCategoriesContainer()
	{
		#ifdef PLATFORM_CONSOLE
		if (IsActive() && GetParent())
		{
			LeftArea leftArea = LeftArea.Cast(GetParent());
			if (leftArea)
			{
				leftArea.SetFirstActive();
				leftArea.ScrollToActiveContainer();
			}
		}
		#endif
	}
	
	void SetEntity(EntityAI entity)
	{
		m_Entity = entity;
		InitIconsContainers();
		
		m_MainWidget = m_RootWidget.FindAnyWidget("body");
		WidgetEventHandler.GetInstance().RegisterOnChildAdd(m_MainWidget, this, "OnChildAdd");
		WidgetEventHandler.GetInstance().RegisterOnChildRemove(m_MainWidget, this, "OnChildRemove");
		
		InitGhostSlots();
		
		(Container.Cast(m_Parent)).m_Body.Insert(this);
		m_Parent.Refresh();
		UpdateNames();
		RecomputeOpenedContainers();
	}

	void SetHeaderName()
	{
		TypeAttachmentGroupsDataHolder holder = m_Entity.GetTypeAttachmentGroupInfoHolder();
		string text = "";
		if (holder)
			text = holder.GetHeaderDisplayName();
		
		if (text == "")
			text = m_Entity.GetDisplayName();
		
		m_CollapsibleHeader.SetName(text);
	}
	
	override bool IsDisplayable()
	{
		return !IsEmpty() || CanDisplayAnyCategory();
	}

	override void UpdateInterval()
	{
		if (m_Entity)
		{
			int childrenCount = m_Body.Count();
			int displayableChildrenCount = 0;
			
			if (childrenCount > 0)
			{
				int i;
				for (i = m_SlotsCount; i < childrenCount; ++i)
				{
					AttachmentCategoriesRow row = AttachmentCategoriesRow.Cast(m_Body[i]);
					if (row)
					{
						if (m_Entity.CanDisplayAttachmentCategory(row.GetCategoryIdentifier()))
						{
							if (row.IsDisplayable())
							{
								ShowInSlots(row.GetCategoryIdentifier(), true);
								if (!row.IsVisible())
									row.OnShow();
							}
							else
							{
								ShowInSlots(row.GetCategoryIdentifier(), false);
								if (row.IsVisible())
									row.OnHide();
							}
						}
						else
						{
							ShowInSlots(row.GetCategoryIdentifier(), false);
							if (row.IsVisible())
								row.OnHide();
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
									cargo.OnShow();
							}
							else
							{
								m_CargoSlotsIcon.m_MainWidget.Show(false);
								if (cargo.IsVisible())
									cargo.OnHide();
							}
							cargo.UpdateInterval();
						}
						RecomputeOpenedContainers();
					}
					
				}
				
				//finally update slot category icons
				for (i = 0; i < m_SlotsCount; ++i)
				{
					AttachmentCategoriesSlotsContainer slotsContainer = AttachmentCategoriesSlotsContainer.Cast(m_Body[i]);
					if (slotsContainer.IsDisplayable())
					{
						if (!m_Hidden)
							slotsContainer.OnShow();
						else
							slotsContainer.OnHide();
						
						displayableChildrenCount++; //slot category icons should always count as 'there', regardless of visibility due to closed state of the parent container
					}
					else
					{
						slotsContainer.OnHide();
					}
				}
			}
			
			bool show = displayableChildrenCount > 0 && !m_Entity.GetInventory().IsInventoryLockedForLockType(HIDE_INV_FROM_SCRIPT);
			if (!m_CollapsibleHeader.IsVisible() && show)
			{
				m_CollapsibleHeader.OnShow();
			}
			else if (m_CollapsibleHeader.IsVisible() && !show)
			{
				m_CollapsibleHeader.OnHide();
				HideContent();
			}
			
			GetMainWidget().Show(show);
		}
	}
	
	void LoadAttachmentCategoriesIcon(SlotsContainer items_cont, string icon_name, int slot_number)
	{
		SlotsIcon icon				= items_cont.GetSlotIcon(slot_number);
		ImageWidget image_widget	= icon.GetGhostSlot();
		image_widget.Show(true);
		image_widget.LoadImageFile(0, StaticGUIUtils.VerifyIconImageString(StaticGUIUtils.IMAGESETGROUP_INVENTORY,icon_name)); //icon_name must be in format "set:<setname> image:<imagename>"
	}

	int GetAttachmentCategoriesCount(string config_path)
	{
		return g_Game.ConfigGetChildrenCount( config_path );
	}

	SlotsContainer GetSlotsContainer(int icons_row)
	{
		AttachmentCategoriesSlotsContainer items_cont = AttachmentCategoriesSlotsContainer.Cast(m_Body.Get(icons_row));
		return items_cont.GetSlotsContainer();
	}
	
	SlotsIcon GetCargoSlotsIcon()
	{
		return m_CargoSlotsIcon;
	}
	
	void ShowInSlots(string category, bool show)
	{
		int index						= m_CategorySlotIndex.Get(category);
		int slot_number					= index % ITEMS_IN_ROW;
		
		SlotsContainer items_cont		= GetSlotsContainer(index / ITEMS_IN_ROW);
		if (items_cont)
		{
			Widget icon_widget			= items_cont.GetSlotIcon(slot_number).GetMainWidget();
			if (icon_widget)
				icon_widget.Show(show);
			icon_widget.GetParent().Update();
			icon_widget.GetParent().GetParent().Update();
			icon_widget.GetParent().GetParent().GetParent().Update();
		}
	}
	
	//oof
	override bool CanDisplayAnyCategory()
	{
		AttachmentCategoriesRow row;
		foreach (LayoutHolder holder : m_Body)
		{
			if (Class.CastTo(row, holder) && m_Entity.CanDisplayAttachmentCategory(row.GetCategoryIdentifier()))
				return true;
		}
		return super.CanDisplayAnyCategory();
	}
	
	override void UpdateRadialIcon()
	{
		if (m_SlotIcon)
		{
			if (m_Entity.CanDisplayCargo() || CanDisplayAnyCategory())
			{
				m_SlotIcon.GetRadialIconPanel().Show(true);
				SetOpenForSlotIcon(!m_Hidden);
			}
			else
			{
				m_SlotIcon.GetRadialIconPanel().Show(false);
			}
		}
	}
	
	string GetAttachmentCategory(string config_path_attachment_categories, int i)
	{
		string attachment_category;
		g_Game.ConfigGetChildName(config_path_attachment_categories, i, attachment_category);
		return attachment_category;
	}

	string GetIconName(string config_path_attachment_categories, string attachment_category)
	{
		string icon_path = config_path_attachment_categories+ " " + attachment_category + " icon";
		string icon_name;
		g_Game.ConfigGetText(icon_path, icon_name);
		return icon_name;
	}
	
	int GetViewIndex(string config_path_attachment_categories, string attachment_category)
	{
		string preview_path = config_path_attachment_categories+ " " + attachment_category + " view_index";
		return g_Game.ConfigGetInt( preview_path );
	}

	void MouseClick(Widget w)
	{
		SlotsIcon icon;
		w.GetUserData(icon);
		
		ClosableContainer c = ClosableContainer.Cast(icon.GetContainer());
		if (c)
			c.Toggle();
	}
	
	override void ExpandCollapseContainer()
	{
		if (m_OpenedContainers.Count() > m_ActiveIndex)
		{
			//c - container where selected icon is part of
			Container c = Container.Cast(m_OpenedContainers.Get(m_ActiveIndex));
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
		TypeAttachmentGroupsDataHolder holder = m_Entity.GetTypeAttachmentGroupInfoHolder();
		int categoriesCount = 0;
		if (holder)
			categoriesCount = holder.GetValidAttachmnetGroupCount();
		
		if (m_Entity.GetInventory().GetCargo())
		{
			categoriesCount++;
		}
		int rowCount = categoriesCount / ITEMS_IN_ROW;
		if (categoriesCount % ITEMS_IN_ROW != 0)
		{
			rowCount++;
		}
		for (int i = 0; i < rowCount; ++i)
		{
			ref AttachmentCategoriesSlotsContainer itemsCont = new AttachmentCategoriesSlotsContainer(this, i);
			m_Body.Insert(itemsCont);
			m_OpenedContainers.Insert(itemsCont);
			
			//TODO MW find better way
			if (i == (rowCount - 1) && categoriesCount % ITEMS_IN_ROW != 0)
				itemsCont.GetSlotsContainer().SetColumnCount(categoriesCount % ITEMS_IN_ROW);
			else
				itemsCont.GetSlotsContainer().SetColumnCount(ITEMS_IN_ROW);
		}
		m_SlotsCount = rowCount;
	}
	
	void InitGhostSlots()
	{
		TypeAttachmentGroupsDataHolder holder = m_Entity.GetTypeAttachmentGroupInfoHolder();
		TAttachmentGroupMap groupMap = holder.GetAttachmentGroupMap();
		int categoriesCount = holder.GetValidAttachmnetGroupCount();
		int childrenCount = m_Body.Count();

		SlotsContainer items_cont;
		SlotsIcon icon;
		string attCategoryClass;
		string icon_name2;
		TypeAttachmentGroupData groupData;
		
		for (int i = 0; i < categoriesCount; ++i)
		{
			items_cont = GetSlotsContainer(i / ITEMS_IN_ROW);
			attCategoryClass = groupMap.GetKey(i);
			groupData = groupMap.Get(attCategoryClass);

			if (items_cont)
			{
				int slot_number = i % ITEMS_IN_ROW;
				m_CategorySlotIndex.Insert(attCategoryClass, i);
				icon = items_cont.GetSlotIcon(slot_number);
				icon.GetGhostSlot().SetFlags(WidgetFlags.IGNOREPOINTER);
				LoadAttachmentCategoriesIcon(items_cont, groupData.m_IconPath, slot_number);
				icon.SetSlotDisplayName(groupData.m_Name);
				
				AttachmentCategoriesRow ar;
				ar = new AttachmentCategoriesRow(this, -1);
				ar.Init(categoriesCount, i, attCategoryClass, holder.GetPropsCfgPath(), m_Entity, childrenCount);
				
				ar.SetSlotIcon(icon);
				
				icon.GetRadialIconPanel().Show(true);
				ar.Open();		
				icon.SetContainer(ar);
				Insert(ar);
			}
		}
		
		if (m_Entity.GetInventory().GetCargo())
		{
			items_cont = GetSlotsContainer(m_SlotsCount - 1);
			if (items_cont)
			{
				icon = items_cont.GetSlotIcon(categoriesCount);
				icon.GetGhostSlot().Show(true);
				icon.GetGhostSlot().LoadImageFile(0, StaticGUIUtils.VerifyIconImageString(StaticGUIUtils.IMAGESETGROUP_INVENTORY,m_Entity.ConfigGetString("GUIInventoryCargoIcon")));
				icon.SetSlotDisplayName(m_Entity.ConfigGetString("GUIInventoryCargoName"));
				icon.GetGhostSlot().SetFlags(WidgetFlags.IGNOREPOINTER);
				
				icon.GetRadialIconPanel().Show(true);
				icon.GetMainWidget().Show(true);
				
				ContainerWithCargo iwc = new ContainerWithCargo(this, -1);
				iwc.Get(0).GetRootWidget().ClearFlags(WidgetFlags.DRAGGABLE);
				iwc.SetEntity(m_Entity, 0, false);
				iwc.SetSlotIcon(icon);
				iwc.Open();
				
				icon.SetContainer(iwc);
				m_CargoSlotsIcon = icon;
			}
		}

		RecomputeOpenedContainers();
	}
	

	override void OnDropReceivedFromHeader(Widget w, int x, int y, Widget receiver)
	{
		ItemPreviewWidget iw = ItemPreviewWidget.Cast(w.FindAnyWidget("Render"));
		if (!iw)
		{
			string name = w.GetName();
			name.Replace("PanelWidget", "Render");
			iw = ItemPreviewWidget.Cast(w.FindAnyWidget(name));
		}
		if (!iw)
		{
			iw = ItemPreviewWidget.Cast(w);
		}
		if (!iw.GetItem())
			return;
		if (m_Entity.GetInventory().CanAddAttachment(iw.GetItem()) && iw.GetItem().GetInventory().CanRemoveEntity())
		{
			g_Game.GetPlayer().PredictiveTakeEntityToTargetAttachment( m_Entity, iw.GetItem() );
		}
		else if (m_Entity.GetInventory().CanAddEntityToInventory(iw.GetItem()) && iw.GetItem().GetInventory().CanRemoveEntity())
		{
			g_Game.GetPlayer().PredictiveTakeEntityToTargetInventory( m_Entity, FindInventoryLocationType.ANY, iw.GetItem() );
		}
	}
	
	override void DraggingOverHeader(Widget w, int x, int y, Widget receiver)
	{
		if (w == null)
		{
			return;
		}
		ItemPreviewWidget iw = ItemPreviewWidget.Cast(w.FindAnyWidget("Render"));
		if (!iw)
		{
			string name = w.GetName();
			name.Replace("PanelWidget", "Render");
			iw = ItemPreviewWidget.Cast(w.FindAnyWidget(name));
		}
		if (!iw)
		{
			iw = ItemPreviewWidget.Cast(w);
		}
		if (!iw || !iw.GetItem())
		{
			return;
		}
		
		ItemManager.GetInstance().ShowSourceDropzone(iw.GetItem());
		ColorManager.GetInstance().SetColor(w, ColorManager.RED_COLOR);
	}
	
	override void CollapseButtonOnMouseButtonDown(Widget w)
	{
		if (!m_Hidden)
		{
			int childrenCount = m_Body.Count();
			for (int i = 1; i < childrenCount; ++i)
			{
				m_Body.Get(i).OnHide();
				Container c = Container.Cast(m_Body.Get(i));
				if (c)
				{
					c.Close();
				}
			}

			OnHide();
		}
		else
		{
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
		if (IsDisplayable())
		{
			super.Open();
			//ItemManager.GetInstance().SetDefaultOpenState(m_Entity.GetType(), true);
			SetOpenForSlotIcon(true);
			OnShow();
			//m_Parent.m_Parent.Refresh();
		}
	}

	override void Close()
	{
		//ItemManager.GetInstance().SetDefaultOpenState(m_Entity.GetType(), false);
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
	
	override bool CanOpenCloseContainerEx(EntityAI focusedEntity)
	{
		if (m_OpenedContainers.Count() > m_ActiveIndex)
		{
			ClosableContainer c;
			SlotsIcon icon = GetFocusedSlotsIcon();
			
			if (icon)
			{
				c = ClosableContainer.Cast(icon.GetContainer());
			}
			
			if (c && c.IsDisplayable())
			{
				return true;
			}
		}
		
		return false;
	}
	
	void UpdateNames()
	{
		TypeAttachmentGroupsDataHolder holder = m_Entity.GetTypeAttachmentGroupInfoHolder();
		if (holder)
			holder.RefreshLocalizedTexts();

		SetHeaderName();

		if (holder)
		{
			TAttachmentGroupMap groupMap = holder.GetAttachmentGroupMap();
			foreach (string attCategoryClass, int slotIndex : m_CategorySlotIndex)
			{
				TypeAttachmentGroupData groupData = groupMap.Get(attCategoryClass);
				SlotsContainer items_cont = GetSlotsContainer(slotIndex / ITEMS_IN_ROW);
				if (items_cont && groupData)
				{
					SlotsIcon icon = items_cont.GetSlotIcon(slotIndex % ITEMS_IN_ROW);
					if (icon)
						icon.SetSlotDisplayName(groupData.m_Name);
				}
			}
		}
	}
}
