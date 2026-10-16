
class ActionInfoPanelBase: LayoutHolder
{}

class TargetActionInfoPanelBase: ActionInfoPanelBase
{
	protected int m_VisibleGridCount;
	protected ActionInfoDataBase m_ActionInfo;
	protected ActionTarget m_Target;
	protected ref array<ref ActionInfoGridBase> m_GridsArray;
	
	void Init()
	{
		InitGridStructure();
	}
	
	protected void InitGridStructure()
	{
		m_GridsArray = new array<ref ActionInfoGridBase>();
	}
	
	//! set to null when not in use
	void SetActionInfo(ActionInfoDataBase info)
	{
		m_ActionInfo = info;
	}
	
	//! generic use not handled yet, if needed, just do something like hiding all children of 'MainGrid' widget and filling your stuff into it directly
	override void SetLayoutName()
	{
		m_LayoutName = "Gui/layouts/new_ui/hud/action_info_spacer.layout";
	}
	
	void SetParentWidget(Widget w)
	{
		m_ParentWidget = w;
	}
	
	void UpdateTargetActionInfoPanel(ActionInfoDataBase info, ActionTarget target)
	{
		m_ActionInfo = info;
		m_Target = target;
		UpdateInterval();
	}
	
	override void UpdateInterval()
	{
		m_VisibleGridCount = 0;
		foreach (ActionInfoGridBase grid : m_GridsArray)
		{
			grid.SetInfoData(m_ActionInfo, m_Target);
			grid.UpdateInterval();
			m_VisibleGridCount += grid.IsVisible();
		}
		
		bool show = m_VisibleGridCount > 0;
		if (!m_RootWidget.IsVisibleHierarchy() && show)
			OnShow();
		if (m_RootWidget.IsVisibleHierarchy() && !show)
			PerformPanelCleanup();
		
		//Print(this);
		//Print(m_VisibleGridCount);
	}
	
	//! also hides!
	void PerformPanelCleanup()
	{
		Cleanup();
		OnHide();
	}
	
	protected void Cleanup()
	{
		foreach (ActionInfoGridBase grid : m_GridsArray)
		{
			grid.Cleanup();
		}
	}
}

class ConstructionActionInfoPanel: TargetActionInfoPanelBase
{
	protected Widget m_Separator;
	
	void ConstructionActionInfoPanel(LayoutHolder parent)
	{
	}
	
	override protected void InitGridStructure()
	{
		super.InitGridStructure();
		
		m_GridsArray.Insert(new ConstructionActionInfoToolsGrid(this, m_RootWidget.FindAnyWidget("SecondaryGrid0")));
		m_GridsArray.Insert(new ConstructionActionInfoMaterialsGrid(this, m_RootWidget.FindAnyWidget("SecondaryGrid1")));
		
		m_Separator = m_RootWidget.FindAnyWidget("Separator");
	}
	
	override void UpdateInterval()
	{
		super.UpdateInterval();
		
		//now handle the grid separator visibility
		bool separatorVisible = m_Separator.IsVisible();
		if ((m_VisibleGridCount > 1) != separatorVisible)
			m_Separator.Show(!separatorVisible);
	}
}