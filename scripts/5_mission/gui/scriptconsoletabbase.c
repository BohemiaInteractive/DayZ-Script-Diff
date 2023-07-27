class ScriptConsoleTabBase
{
	protected Widget m_Root;
	protected ScriptConsole m_ScriptConsole;
	protected PluginConfigDebugProfile m_ConfigDebugProfile;
	protected PluginConfigDebugProfileFixed m_ConfigDebugProfileFixed;
	
	void ScriptConsoleTabBase(Widget root, ScriptConsole console)
	{
		m_ScriptConsole = console;
		m_Root = root;
		m_ConfigDebugProfile = PluginConfigDebugProfile.Cast(GetPlugin(PluginConfigDebugProfile));
		m_ConfigDebugProfileFixed	= PluginConfigDebugProfileFixed.Cast(GetPlugin(PluginConfigDebugProfileFixed));
	}
	
	void Update(float timeslice);

	bool OnChange(Widget w, int x, int y, bool finished);
	bool OnClick(Widget w, int x, int y, int button);
	bool OnItemSelected(Widget w, int x, int y, int row, int  column,	int  oldRow, int  oldColumn);
	bool OnKeyDown(Widget w, int x, int y, int key);
	bool OnDoubleClick(Widget w, int x, int y, int button);
	void OnRPCEx(int rpc_type, ParamsReadContext ctx);
	bool OnMouseButtonDown(Widget w, int x, int y, int button);
	
	protected void AddItemToClipboard(TextListboxWidget text_listbox_widget)
	{
		int selected_row_index = text_listbox_widget.GetSelectedRow();
		if (selected_row_index != -1)
		{
			string item_name;
			text_listbox_widget.GetItemText(selected_row_index, 0, item_name);
			GetGame().CopyToClipboard(item_name);
		}
	}
}