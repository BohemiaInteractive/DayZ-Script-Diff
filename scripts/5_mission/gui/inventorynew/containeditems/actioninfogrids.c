class ActionInfoGridBase: LayoutHolder
{
	protected bool m_UpdateTarget;
	protected ActionInfoDataBase m_ActionInfo;
	protected ActionTarget m_Target;
	protected ref array<ref GenericIconBase> m_Icons;
	protected int m_IconVisibleCount;
	
	void ActionInfoGridBase(LayoutHolder parent, Widget handledWidget)
	{
		InitInfoGrid(parent,handledWidget);
	}
	
	protected void InitInfoGrid(LayoutHolder parent, Widget handledWidget)
	{
		m_Icons = new array<ref GenericIconBase>();
		
		if (handledWidget)
		{
			m_MainWidget = handledWidget;
			m_RootWidget = handledWidget;
		}
		
		//1. hide default margin cards
		Widget child = m_MainWidget.GetChildren();
		while (child)
		{
			child.Show(false);
			child = child.GetSibling();
		}
	}
	
	//! does not create unique layout, just a handler for an existing grid
	override void SetLayoutName()
	{
		m_LayoutName = "";
	}
	
	//! We CAN add more icons during runtime if needed..
	protected void CreateNewIcon(int idx = -1)
	{
		m_Icons.Insert(new GenericIconBase(this,null,idx,null));
	}
	
	void SetInfoData(ActionInfoDataBase info, ActionTarget target)
	{
		m_ActionInfo = info;
		m_Target = target;
	}
	
	void Cleanup()
	{
		m_IconVisibleCount = 0;
		m_ActionInfo = null;
		m_Target = null;
		
		foreach (GenericIconBase icon : m_Icons)
		{
			//icon.SetSlotID(InventorySlots.INVALID);
			icon.ResetIconValues(); //reset extra values on full cleanup as appropriate
			icon.Clear();
		}
	}
	
	override void UpdateInterval()
	{
		bool visible = IsVisible();
		bool shouldBe = m_IconVisibleCount > 0;
		if (visible != shouldBe)
		{
			m_RootWidget.Show(shouldBe);
			
			if (!shouldBe)
				Cleanup();
		}
	}
	
	//! parent to the main grid
	override void SetParentWidget()
	{
		if (m_Parent)
			m_ParentWidget = m_Parent.GetRootWidget().FindAnyWidget("MainGrid");
	}
}

class ConstructionActionInfoGrid: ActionInfoGridBase
{
	ConstructionPart m_LastPart;
	
	override protected void CreateNewIcon(int idx = -1)
	{
		//m_Icons.Insert(new ConstructionInfoIcon(this,null,idx,null));
	}
	
	override void Cleanup()
	{
		super.Cleanup();
		
		m_LastPart = null;
	}
}

class ConstructionActionInfoToolsGrid: ConstructionActionInfoGrid
{
	protected int m_LastToolMask;
	protected int m_LastPartMask;
	
	override protected void InitInfoGrid(LayoutHolder parent, Widget handledWidget)
	{
		super.InitInfoGrid(parent,handledWidget);
		
		//2. create default number of icons
		for (int i = 1; i < EnumTools.GetEnumSize(EConstructionTools); ++i) //skipping the 'TOOL_NO_ACTION'
		{
			CreateNewIcon(1 << (i - 1)); //indexing by tool category bit value
		}
		
		m_LastToolMask = 0;
		m_LastPartMask = 0;
	}
	
	override protected void CreateNewIcon(int idx = -1)
	{
		m_Icons.Insert(new ConstructionToolInfoIcon(this,null,idx,null));
	}
	
	override void UpdateInterval()
	{
		if (m_ActionInfo.m_InfoContextMask & EConstructionInfoCategories.TOOL_PRESENCE)
		{
			ActionInfoDataConstructionBase info = ActionInfoDataConstructionBase.Cast(m_ActionInfo);
			ConstructionPart part = info.GetConstructionPart();
			//int partActionMask = part.GetBuildActionTypeMask();
			int partInfoMask = part.GetBuildIndicationTypeMask();
			int toolActionMask = EConstructionTools.TOOL_HANDS; //hands as default
			ItemBase tool;
			if (Class.CastTo(tool, info.GetTool()))
			{
				//
				string toolCfgPath = "cfgVehicles" + " " + tool.GetType() + " " + "build_action_type"; //TODO: cache later on tools (ItemBase sufficient?)
				if (g_Game.ConfigIsExisting(toolCfgPath))
				{
					toolActionMask = g_Game.ConfigGetInt(toolCfgPath);
				}
				else
				{
					toolActionMask = EConstructionTools.TOOL_NO_ACTION;
				}
				//
			}
			
			if (partInfoMask != m_LastPartMask || part != m_LastPart || toolActionMask != m_LastToolMask)
			{
				m_IconVisibleCount = 0;
				
				bool firstValidToolSet = false;
				int maskOverlap = toolActionMask & partInfoMask;
				//bool toolToDisplay = maskOverlap != EConstructionTools.TOOL_NO_ACTION;
				//toolToDisplay |= EConstructionTools.TOOL_HANDS & partInfoMask;
				int iconIdx;
				ConstructionToolInfoIcon toolIcon;
				
				//m_CrossedImage
				foreach (GenericIconBase icon : m_Icons)
				{
					Class.CastTo(toolIcon,icon);
					toolIcon.SetConstructionPart(part);
					iconIdx = toolIcon.GetIconIndex();
					bool validToolBool = iconIdx & maskOverlap;
					//use first icon and hide the rest
					if (maskOverlap)
					{
						if (validToolBool && !firstValidToolSet)
						{
							toolIcon.SetShowIcon(true);
							toolIcon.SetTool(tool);
							toolIcon.ToolCompatibilityWarning(false);
							firstValidToolSet = true;
						}
						else
						{
							toolIcon.SetShowIcon(false);
							toolIcon.SetTool(null);
						}
						
						toolIcon.UpdateInterval();
						m_IconVisibleCount += toolIcon.IsVisible();
					}
					else //show all tool icons by sanitized part mask value (indicates required tools)
					{
						if (iconIdx == EConstructionTools.TOOL_HANDS)
						{
							toolIcon.ToolCompatibilityWarning(tool != null);
							toolIcon.SetTool(tool);
						}
						else
						{
							toolIcon.SetTool(null);
						}
						
						toolIcon.SetShowIcon(iconIdx & partInfoMask);
						toolIcon.UpdateInterval();
						m_IconVisibleCount += toolIcon.IsVisible();
					}
				}
				
				m_LastPartMask = partInfoMask;
				m_LastToolMask = toolActionMask;
				m_LastPart = part;
			}
		}
		else
		{
			m_IconVisibleCount = 0;
		}
		
		super.UpdateInterval(); //handles grid widget visibility
	}
	
	override void Cleanup()
	{
		super.Cleanup();
		
		m_LastPartMask = 0;
		m_LastToolMask = 0;
	}
	
	//! parent to the first subgrid
	override void SetParentWidget()
	{
		if (m_Parent)
			m_ParentWidget = m_Parent.GetRootWidget().FindAnyWidget("SecondaryGrid0");
	}
}

class ConstructionActionInfoMaterialsGrid: ConstructionActionInfoGrid
{
	const int DEFAULT_MATERIAL_ICON_COUNT = 5; //TODO: remove and replace by max material count (store somewhere on materials init)
	
	override protected void InitInfoGrid(LayoutHolder parent, Widget handledWidget)
	{
		super.InitInfoGrid(parent,handledWidget);
		
		//2. create default number of icons
		for (int i = 0; i < DEFAULT_MATERIAL_ICON_COUNT; ++i)
		{
			CreateNewIcon(i); //indexing by part's material idx
		}
	}
	
	override protected void CreateNewIcon(int idx = -1)
	{
		m_Icons.Insert(new ConstructionMaterialInfoIcon(this,null,idx,null));
	}
	
	override void UpdateInterval()
	{
		if (m_ActionInfo.m_InfoContextMask & EConstructionInfoCategories.MATERIALS_PRESENCE)
		{
			ActionInfoDataConstructionBase info = ActionInfoDataConstructionBase.Cast(m_ActionInfo);
			ConstructionPart part = info.GetConstructionPart();
			
			m_IconVisibleCount = 0;
			ConstructionMaterialInfoIcon matIcon;
			foreach (GenericIconBase icon : m_Icons)
			{
				Class.CastTo(matIcon,icon);
				
				matIcon.SetConstructionPart(part);
				matIcon.SetTargetEntity(EntityAI.Cast(m_Target.GetObject()));
				//matIcon.InitMaterialInfo();
				matIcon.UpdateInterval();
				m_IconVisibleCount += matIcon.IsVisible();
			}
		}
		else
		{
			m_IconVisibleCount = 0;
		}
		
		super.UpdateInterval(); //handles grid widget visibility
	}
	
	//! parent to the second subgrid
	override void SetParentWidget()
	{
		if (m_Parent)
			m_ParentWidget = m_Parent.GetRootWidget().FindAnyWidget("SecondaryGrid1");
	}
}