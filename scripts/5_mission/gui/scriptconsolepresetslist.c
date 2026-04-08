class ScriptConsolePresetsList extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected Widget m_Parent;
	protected ScriptConsoleTabBase m_Tab;
	protected TextListboxWidget m_PresetsList;
	
	ref ScriptInvoker m_ItemSelected = new ScriptInvoker;
	
	void ScriptConsolePresetsList(Widget parent, ScriptConsoleTabBase tab)
	{
		m_Parent = parent;
		m_Tab = tab;

		m_Root = g_Game.GetWorkspace().CreateWidgets(GetLayoutFile(), parent);
		m_PresetsList = TextListboxWidget.Cast(m_Root);
		
		m_Root.SetHandler(this);
	}
	
	void AddListItem(string label, int column, int row, Class data = null)
	{
		m_PresetsList.AddItem(label, data, column, row);
		m_PresetsList.Update();
	}
	
	void ClearList()
	{
		m_PresetsList.ClearItems();
	}
	
	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		if (w == m_PresetsList)
		{
			m_ItemSelected.Invoke(row, column);
			return true;
		}
		
		return false;
	}
	
	string GetLayoutFile()
	{
		return "gui/layouts/script_console/script_console_presets_list.layout";
	}
	
	Widget GetParent()
	{
		return m_Parent;
	}
	
	ScriptConsoleTabBase GetTab()
	{
		return m_Tab;
	}
	
	TextListboxWidget GetPresetsList()
	{
		return m_PresetsList;	
	}
}