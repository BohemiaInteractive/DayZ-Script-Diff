class ScriptConsoleVicinityTab : ScriptConsoleTabBase
{
	protected TextListboxWidget 	m_VicinityListbox;
	protected ButtonWidget 			m_WatchItem;	
	protected ref array<Object> 	m_VicinityItems = new array<Object>;
	
	void ScriptConsoleVicinityTab(Widget root, ScriptConsole console)
	{
		m_VicinityListbox = TextListboxWidget.Cast(root.FindAnyWidget("VicinityListbox"));
		m_WatchItem = ButtonWidget.Cast(root.FindAnyWidget("WatchItemButton"));
		
		array<Object> objects = new array<Object>();
		array<CargoBase> cargo = new array<CargoBase>();
		
		GetGame().GetObjectsAtPosition(GetGame().GetPlayer().GetPosition(), 30, objects,cargo);
		m_VicinityListbox.ClearItems();
		m_VicinityItems.Clear();
		
		foreach (Object o:objects)
		{
			if (o.Type().ToString() != "Object")
			{
				if (o.IsMan())
				{
					m_VicinityItems.InsertAt(o,0);	//insert at 0 index
					m_VicinityListbox.AddItem(o.GetType(),null,0,0);//insert at 0 index
				}
				else
				{
					m_VicinityItems.Insert(o);//insert at the end
					m_VicinityListbox.AddItem(o.GetType(),null,0);//insert at the end
				}
			}
		}
	}
		
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w,x,y,button);
		if (w == m_WatchItem)
		{
			int row = m_VicinityListbox.GetSelectedRow();
			
			if (m_VicinityItems.IsValidIndex(row))
			{
				Object entity = m_VicinityItems.Get(row);
				if (entity)
				{
					PluginDeveloper.SetDeveloperItemClientEx(entity);
					return true;
				}
			}
		}
		return false;
	}
}
