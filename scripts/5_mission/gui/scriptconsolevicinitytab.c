class ScriptConsoleVicinityTab : ScriptConsoleTabBase
{
	protected static float m_Distance = 30;
	protected static Object m_Selected = null;
	
	protected TextListboxWidget 	m_VicinityListbox;
	protected ButtonWidget 			m_WatchItem;	
	protected EditBoxWidget			m_DistanceWidget;
	protected ref array<Object> 	m_VicinityItems = new array<Object>;

	protected ref ContextMenu		m_ContextMenu;
	
	void ScriptConsoleVicinityTab(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
	{
		m_VicinityListbox = TextListboxWidget.Cast(root.FindAnyWidget("VicinityListbox"));
		m_WatchItem = ButtonWidget.Cast(root.FindAnyWidget("WatchItemButton"));
		m_DistanceWidget = EditBoxWidget.Cast(root.FindAnyWidget("DistanceInput"));
		
		m_DistanceWidget.SetText(m_Distance.ToString());

		m_ContextMenu = new ContextMenu();
		m_ContextMenu.Init(root.FindAnyWidget("VicinityContextMenu"), true);

		m_ContextMenu.SetSize(1, 1);
		m_ContextMenu.Show(0, 0);
		
		RenderList();
	}
	
	override void OnSelected()
	{
		UpdateSelected();
	}
	
	void RenderList()
	{
		float radius = m_DistanceWidget.GetText().ToFloat();
		if (!radius)
			return;
		array<Object> objects = new array<Object>();
		array<CargoBase> cargo = new array<CargoBase>();
		
		vector playerPos = g_Game.GetCurrentCameraPosition();
		g_Game.GetObjectsAtPosition3D(playerPos, radius, objects,cargo);
		m_VicinityListbox.ClearItems();
		m_VicinityItems.Clear();

		if (m_Selected && objects.Find(m_Selected) == -1)
		{
			objects.Insert(m_Selected);
		}
		
		map<int, Object> objectsMap = new map<int, Object>();
		array<int> distances = new array<int>();
		
		foreach (int x, Object obj:objects)
		{
			int dist = vector.Distance(playerPos, obj.GetPosition()) * 1000;
			
			while (objectsMap.Contains(dist))//hacking for when multiple objects are at the same distance from the player, otherwise sorting using map will fail
				dist++;
			objectsMap.Insert(dist, obj);
			distances.Insert(dist);
		}
		
		distances.Sort();
		
		foreach (int dst:distances)
		{
			Object o = objectsMap.Get(dst);
			if (!o)
				continue;
			int i = 0;
			if (o.Type() != Object)
			{
				m_VicinityItems.Insert(o);//insert at the end
				int indx = m_VicinityListbox.AddItem(o.GetType(),null,0,-1);
				float distanceMeters = dst / 1000;
				m_VicinityListbox.SetItem(indx, distanceMeters.ToString(),null,1);
				i++;
				
				if (m_Selected == o)
				{
					m_VicinityListbox.SelectRow(indx);
				}
			}
		}
		
		UpdateSelected();
	}
	
	void UpdateSelected()
	{
		int row = m_VicinityListbox.GetSelectedRow();
		
		m_Selected = null;
		if (m_VicinityItems.IsValidIndex(row))
		{
			m_Selected = m_VicinityItems.Get(row);
		}
		
		EntityAI entity = EntityAI.Cast(m_Selected);
		if (entity)
		{
			m_ContextMenu.BuildContextMenu(entity, m_VicinityListbox, this);
	
			m_ContextMenu.SetSize(1, 1);
			m_ContextMenu.Show(0, 0);
		}
		else
		{
			m_ContextMenu.Hide();
		}
	}
		
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w,x,y,button);
		
		UpdateSelected();
		
		int row = m_VicinityListbox.GetSelectedRow();
		if (w == m_WatchItem)
		{
			if (m_VicinityItems.IsValidIndex(row))
			{
				if (m_Selected)
				{
					PluginDeveloper.SetDeveloperItemClientEx(m_Selected);
					return true;
				}
			}
		}
		
		return false;
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		
		UpdateSelected();
		
		if (w == m_DistanceWidget)
		{
			RenderList();
			m_Distance = m_DistanceWidget.GetText().ToFloat();
		}
		return false;
	}
	
	void OnSelectAction(EntityAI ent, int actionId)
	{
		#ifdef DIAG_DEVELOPER
		if (!ent || ent.IsSetForDeletion())
			return;
		
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (player)
		{
			player.GetActionManager().OnInstantAction(ActionDebug, new Param2<EntityAI, int>(ent, actionId));
		}
		else
		{
			ent.OnAction(actionId, null, null);
		}
		#endif
	}
}
