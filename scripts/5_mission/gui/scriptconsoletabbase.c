class ScriptConsoleTabBase
{
	protected int m_Id;
	protected bool m_IsSelected;
	protected bool m_IsShiftDown;
	protected Widget m_ParentRoot;
	protected Widget m_Root;
	protected Widget m_Button;
	protected ScriptConsole m_ScriptConsole;
	protected ScriptConsoleTabBase m_Parent;//for subtabs
	protected PluginConfigDebugProfile m_ConfigDebugProfile;
	protected PluginConfigDebugProfileFixed m_ConfigDebugProfileFixed;
	
	void ScriptConsoleTabBase(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
	{
		m_Button = button;
		m_ScriptConsole = console;
		m_Root = root;
		if (parent)
			m_ParentRoot = parent.GetRoot();
		else
			m_ParentRoot = console.layoutRoot;
		m_ConfigDebugProfile = PluginConfigDebugProfile.Cast(GetPlugin(PluginConfigDebugProfile));
		m_ConfigDebugProfileFixed	= PluginConfigDebugProfileFixed.Cast(GetPlugin(PluginConfigDebugProfileFixed));
	}
	
	void Init(int id)
	{
		m_Id = id;
	}
	
	int GetID()
	{
		return m_Id;
	}
	
	Widget GetRoot()
	{
		return m_Root;
	}
	
	bool IsSelected()
	{
		return m_IsSelected;
	}
	
	void Select(bool select, ScriptConsoleTabBase selectedHandler)
	{
		m_IsSelected = select;
		if (m_Root)
		{
			Show(select, selectedHandler);
			ButtonWidget btn = ButtonWidget.Cast(m_Button);
			if (btn)
				btn.SetState(select);
		}
		if (select)
			OnSelected();
	}
	
	void OnSelected();
	
	void Show(bool show, ScriptConsoleTabBase selectedHandler)
	{
		m_Root.Show(show);
		m_Root.Enable(show);
	}
	
	Widget GetButton()
	{
		return m_Button;
	}
	
	void Update(float timeslice)
	{
		m_IsShiftDown = KeyState(KeyCode.KC_LSHIFT) || KeyState(KeyCode.KC_RSHIFT);
	}

	bool OnChange(Widget w, int x, int y, bool finished);
	bool OnClick(Widget w, int x, int y, int button);
	bool OnItemSelected(Widget w, int x, int y, int row, int  column,	int  oldRow, int  oldColumn);
	bool OnKeyDown(Widget w, int x, int y, int key);
	bool OnDoubleClick(Widget w, int x, int y, int button);
	void OnRPCEx(int rpc_type, ParamsReadContext ctx);
	bool OnMouseButtonDown(Widget w, int x, int y, int button);
	bool OnKeyPress(Widget w, int x, int y, int key);
	bool OnMouseEnter(Widget w, int x, int y);
	bool OnMouseLeave(Widget w, Widget enterW, int x, int y);
	
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