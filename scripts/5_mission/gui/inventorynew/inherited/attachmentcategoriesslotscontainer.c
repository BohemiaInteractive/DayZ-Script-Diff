class AttachmentCategoriesSlotsContainer: Container
{
	ref SlotsContainer m_ItemsCont;
	
	void AttachmentCategoriesSlotsContainer( LayoutHolder parent, int index )
	{
		m_ItemsCont = new SlotsContainer(this, null);
		m_Body.Insert( m_ItemsCont );
		m_ItemsCont.GetMainWidget().SetUserID( index );
		
		for ( int k = 0; k < ITEMS_IN_ROW; k++ )
		{
			//WidgetEventHandler.GetInstance().RegisterOnDropReceived( m_ItemsCont.GetSlotIcon( k ).GetMainWidget(),  m_Parent, "OnDropReceivedFromHeader" );
			//WidgetEventHandler.GetInstance().RegisterOnDropReceived(  m_ItemsCont.GetSlotIcon( k ).GetPanelWidget(),  m_Parent, "OnDropReceivedFromHeader" );
			      
			WidgetEventHandler.GetInstance().RegisterOnDraggingOver( m_ItemsCont.GetSlotIcon( k ).GetMainWidget(),  m_Parent, "DraggingOverHeader" );
			WidgetEventHandler.GetInstance().RegisterOnDraggingOver( m_ItemsCont.GetSlotIcon( k ).GetPanelWidget(),  m_Parent, "DraggingOverHeader" );
			
			WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown( m_ItemsCont.GetSlotIcon( k ).GetMainWidget(),  m_Parent, "MouseClick" );
			WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown( m_ItemsCont.GetSlotIcon( k ).GetPanelWidget(),  m_Parent, "MouseClick" );
			
			WidgetEventHandler.GetInstance().RegisterOnMouseEnter( m_ItemsCont.GetSlotIcon( k ).GetMainWidget(), m_ItemsCont.GetSlotIcon( k ), "MouseEnterGhostSlot" );
			WidgetEventHandler.GetInstance().RegisterOnMouseLeave( m_ItemsCont.GetSlotIcon( k ).GetMainWidget(), m_ItemsCont.GetSlotIcon( k ), "MouseLeaveGhostSlot" );
			
			m_ItemsCont.GetSlotIcon( k ).GetMainWidget().SetUserID( k );
			m_ItemsCont.GetSlotIcon( k ).GetPanelWidget().SetUserID( k );
		}
	}
	
	override bool IsDisplayable()
	{
		for(int i = 0; i < m_Body.Count(); i++)
		{
			LayoutHolder c = m_Body[i];
			if( c && c.IsDisplayable())
				return true;
		}
		return false;
	}
  
	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.Container;
	}
  
	SlotsContainer GetSlotsContainer()
	{
		return m_ItemsCont;
	}
	
	override EntityAI GetFocusedItem()
	{
		return m_ItemsCont.GetFocusedSlotsIcon().GetRender().GetItem();
	}
	
	int GetParentID()
	{
		return m_ItemsCont.GetMainWidget().GetUserID();
	}
	
	int GetFocusedID()
	{
		return m_ItemsCont.GetFocusedSlotsIcon().GetMainWidget().GetUserID();
	}
	
	void ExpandCollapseContainer( bool expand )
	{
		m_ItemsCont.GetFocusedSlotsIcon().GetRadialIcon().Show( expand );
		m_ItemsCont.GetFocusedSlotsIcon().GetRadialIconClosed().Show( !expand );
	}
	
	override bool SelectItem()
	{
		return false;
	}
	
	override bool Select()
	{
		return false;
	}
	
	override bool Combine()
	{
		return false;
	}
	
	override bool TransferItemToVicinity()
	{
		return false;
	}
	
	override bool TransferItem()
	{
		return false;
	}
	
	override bool SplitItem()
	{
		return false;
	}
	
	override bool EquipItem()
	{
		return false;
	}
	
	override bool CanEquip()
	{
		return false;
	}
	
	override bool CanCombine()
	{
		return false;
	}
	
	override bool CanCombineAmmo()
	{
		return false;
	}
	
	override bool IsEmpty()
	{
		return false;
	}
	
	override bool IsItemActive()
	{
		return false;
	}

	override bool IsItemWithQuantityActive()
	{
		return false;
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
	
	/*override void Refresh()
	{
		super.Refresh();
		Print("DbgTest | Refresh");
		
		CollapsibleContainer cont = CollapsibleContainer.Cast(m_Parent);
		if (cont)
		{
			bool b1 = m_ItemsCont.VerifySlotsIconVisibility();
			Print("DbgTest | Refresh | b1: " + b1);
			cont.SetCollapsibleHeaderArrowState(!b1);
		}
	}*/
}
