class GenericIconBase: SlotsIconBase
{
	protected int m_IconIndex;
	protected Widget m_GenericWarning;
	protected Widget m_ImageBackdrop;
	protected Widget m_CrossedImage;
	
	//! 'index' is used for indexing within various other systems
	override protected void InitIcon(LayoutHolder parent, Widget root, int index, EntityAI slot_parent)
	{
		super.InitIcon(parent,root,index,slot_parent);
		
		m_IconIndex = index;
	}
	
	override protected void InitIconWidgets(LayoutHolder parent, Widget root, int index, EntityAI slot_parent)
	{
		super.InitIconWidgets(parent,root,index,slot_parent);
		
		m_PanelWidget			= m_MainWidget.FindAnyWidget("PanelWidget");
		
		m_CursorWidget			= m_MainWidget.FindAnyWidget("Cursor");
		m_ColWidget				= m_MainWidget.FindAnyWidget("Col");
		m_MountedWidget			= m_MainWidget.FindAnyWidget("Mounted");
		m_OutOfReachWidget		= m_MainWidget.FindAnyWidget("OutOfReach");

		m_ItemPreview			= ItemPreviewWidget.Cast(m_MainWidget.FindAnyWidget("Render"));
		
		m_GhostSlot				= ImageWidget.Cast(m_MainWidget.FindAnyWidget("GhostSlot"));
		
		m_ColorWidget			= m_MainWidget.FindAnyWidget("Color");
		m_SelectedPanel			= m_MainWidget.FindAnyWidget("Selected");
		m_EmptySelectedPanel    = m_MainWidget.FindAnyWidget("EmptySelected");
		m_MicromanagedPanel    = m_MainWidget.FindAnyWidget("Micromanaged");
		
		m_QuantityPanel			= m_MainWidget.FindAnyWidget("QuantityPanel");
		m_QuantityItem			= RichTextWidget.Cast(m_MainWidget.FindAnyWidget("Quantity"));
		m_QuantityProgress		= ProgressBarWidget.Cast(m_MainWidget.FindAnyWidget("QuantityBar"));
		m_QuantityStack			= m_MainWidget.FindAnyWidget("QuantityStackPanel");
		
		m_ItemSizePanel			= m_MainWidget.FindAnyWidget("ItemSizePanel");
		m_ItemSizeWidget		= RichTextWidget.Cast(m_MainWidget.FindAnyWidget("ItemSize"));
		
		m_AmmoIcon				= ImageWidget.Cast(m_MainWidget.FindAnyWidget("AmmoIcon"));
		
		m_RadialIconPanel		= m_MainWidget.FindAnyWidget("RadialIconPanel");
		m_RadialIconClosed		= m_MainWidget.FindAnyWidget("RadialIconClosed");
		m_RadialIcon			= m_MainWidget.FindAnyWidget("RadialIcon");
		
		m_GenericWarning 		= m_MainWidget.FindAnyWidget("GenericWarning");
		m_GenericWarning.Show(false);
		
		m_ImageBackdrop 		= m_MainWidget.FindAnyWidget("ImageBackdrop");
		m_ImageBackdrop.Show(false);
		
		m_CrossedImage 		= m_MainWidget.FindAnyWidget("SomethingWrongIcon");
		m_CrossedImage.Show(false);
		
		m_ReservedWidget = Widget.Cast( GetGame().GetWorkspace().CreateWidgets( "gui/layouts/inventory_new/reserved_icon.layout", m_MainWidget ) );
		m_ReservedWidget.Show(false);
	}
	
	override void SetLayoutName()
	{
		m_LayoutName = "Gui/layouts/new_ui/hud/SimpleIconTemplate.layout";
	}
	
	//! in contrast to 'Clear' method, this just resets extra values from child classes
	void ResetIconValues()
	{
		m_SlotID = InventorySlots.INVALID;
	}
	
	int GetIconIndex()
	{
		return m_IconIndex;
	}
}
