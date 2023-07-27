class ModsMenuSimple extends ScriptedWidgetEventHandler
{
	protected const int 									MOD_DISPLAY_COUNT_MAX = 3;
	
	protected Widget										m_Root;
	protected Widget										m_MoreButton;
	protected Widget										m_MoreHighlight;
	protected ref map<ModInfo, ref ModsMenuSimpleEntry>		m_Data;
	protected ModsMenuDetailed								m_DetailMenu;
	
	void ModsMenuSimple(array<ref ModInfo> data, Widget parent, ModsMenuDetailed detail_menu)
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/mods_menu/mods_menu_simple.layout", parent);
		m_MoreButton = m_Root.FindAnyWidget("ModMore");
		m_MoreHighlight = m_Root.FindAnyWidget("ModMoreOverlay");
		m_Data = new map<ModInfo, ref ModsMenuSimpleEntry>;
		m_DetailMenu = detail_menu;
		
		m_Root.SetHandler(this);
		LoadEntries(data);
	}
	
	void ~ModsMenuSimple()
	{
		delete m_Root;
	}

	void LoadEntries(array<ref ModInfo> data)
	{
		m_MoreButton.Show(data.Count() > MOD_DISPLAY_COUNT_MAX);
		int count = Math.Clamp(data.Count(),0,MOD_DISPLAY_COUNT_MAX);
		
		for (int i = 0; i < count; i++)
		{
			ref ModsMenuSimpleEntry entry = new ModsMenuSimpleEntry(data[i], i, m_Root, this);
			m_Data.Insert(data[i], entry);
		}
	}
	
	void Select(ModInfo mod)
	{
		m_DetailMenu.Open();
		m_DetailMenu.Highlight(mod);
	}
	
	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if (w == m_MoreButton)
		{
			if (m_DetailMenu.IsOpen())
				m_DetailMenu.Close();
			else
				m_DetailMenu.Open();
			return true;
		}
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w == m_MoreButton)
		{
			m_MoreHighlight.Show(true);
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (enterW != m_MoreButton)
		{
			m_MoreHighlight.Show(false);
			return true;
		}
		return false;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		if (w == m_MoreButton)
		{
			m_MoreHighlight.Show(true);
			return true;
		}
		return false;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (w == m_MoreButton)
		{
			m_MoreHighlight.Show(false);
			return true;
		}
		return false;
	}
}