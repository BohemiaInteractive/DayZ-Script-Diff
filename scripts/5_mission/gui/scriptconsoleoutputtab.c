class ScriptConsoleOutputTab : ScriptConsoleTabBase
{
	protected TextListboxWidget m_ClientLogListbox;
	protected ButtonWidget 		m_ClientLogClearButton;
	protected CheckBoxWidget 	m_ClientLogScrollCheckbox;
	
	void ScriptConsoleOutputTab(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
	{
		m_ClientLogListbox = TextListboxWidget.Cast(root.FindAnyWidget("TextListbox"));
		m_ClientLogClearButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonClear"));
		m_ClientLogScrollCheckbox = CheckBoxWidget.Cast(root.FindAnyWidget("CheckBoxAutoScroll"));
		ReloadOutput();
	}
	
	void ~ScriptConsoleOutputTab()
	{
	}
		
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w,x,y,button);
		
		if (w == m_ClientLogClearButton)
		{
			Clear(true);
			return true;
		}
		return false;
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		return false;
	}

	
	protected void Clear(bool clearFile = false)
	{
		if(clearFile)
			Debug.ClearLogs();
		m_ClientLogListbox.ClearItems();
	}
	
	protected void Add(string message, bool isReload = false)
	{
		if (m_ClientLogListbox)
		{
			m_ClientLogListbox.AddItem(String(message), NULL, 0);

			if (m_ClientLogScrollCheckbox.IsChecked())
			{
				m_ClientLogListbox.EnsureVisible(m_ClientLogListbox.GetNumItems());
			}
		}
	}
	
	protected void ReloadOutput()
	{
		Clear();
		FileHandle file_index = OpenFile(Debug.GetFileName(), FileMode.READ);
		
		if ( file_index )
		{
			string line_content;
			while ( FGets( file_index,  line_content ) != -1 )
			{
				Add(line_content, true);
			}
			
			CloseFile(file_index);
		}
	}
	
}
