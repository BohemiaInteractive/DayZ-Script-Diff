class MapHandler : ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	
	void MapHandler(Widget w)
	{
		m_Root = w;
		m_Root.SetHandler(this);
	}
	
	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (!super.OnKeyDown(w, x, y, key))
			return false;
		
		vector screen_to_map = MapWidget.Cast(w).ScreenToMap(Vector(x,y,0));
		Print(key);
		Print(screen_to_map);
		return true;
	}
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		super.OnDoubleClick(w, x, y, button);
		
		vector screen_to_map = MapWidget.Cast(w).ScreenToMap(Vector(x,y,0));
		MapMenu m = MapMenu.Cast(g_Game.GetUIManager().FindMenu(MENU_MAP));
		int rand = Math.RandomInt(0,eMapMarkerTypes.MARKERTYPE_MAX);
		//m.AddMarker(screen_to_map,ARGB(255,255,0,0),rand);
		
		//m.AddUserMark(screen_to_map, "marker", ARGB(255,0,0,255), "\\dz\\gear\\navigation\\data\\map_tree_ca.paa");
		
		return true;
	}
	
	/*void AddMarker(MapWidget w, vector pos, int color, int icon = -1)
	{
		 w.AddUserMark(pos, "", color, "\\dz\\gear\\navigation\\data\\map_tree_ca.paa");
	}*/
};