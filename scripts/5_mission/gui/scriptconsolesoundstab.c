class ScriptConsoleSoundsTab : ScriptConsoleTabBase
{
	protected bool 				m_PlayerPosRefreshBlocked;
	protected static float 		DEBUG_MAP_ZOOM = 1;
	protected vector 			m_MapPos;
	protected ButtonWidget 		m_CopySoundset;
	protected ButtonWidget 		m_PlaySoundset;
	protected ButtonWidget 		m_PlaySoundsetLooped;
	protected ButtonWidget 		m_StopSoundset;

	protected EditBoxWidget 		m_SoundFilter;
	protected TextListboxWidget 	m_SoundsTextListbox;
	
	protected static EffectSound 	m_SoundSet;
	protected ref Timer 			m_RefreshFilterTimer = new Timer();
	
	protected MapWidget 			m_DebugMapWidget;
	protected TextWidget 			m_MapDistWidget;
	protected TextWidget	 		m_MouseCurPos;
	protected TextWidget	 		m_MapHeadingWidget;
	
	void ScriptConsoleSoundsTab(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
	{
		m_MouseCurPos		= TextWidget.Cast(root.FindAnyWidget("MapSoundsPos"));
		m_MapDistWidget		= TextWidget.Cast(root.FindAnyWidget("MapSoundsDistance"));
		m_MapHeadingWidget	= TextWidget.Cast(root.FindAnyWidget("MapHeadingSounds"));
		
		m_CopySoundset		= ButtonWidget.Cast(root.FindAnyWidget("SoundsetCopy"));
		m_PlaySoundset		= ButtonWidget.Cast(root.FindAnyWidget("PlaySoundset"));
		m_PlaySoundsetLooped = ButtonWidget.Cast(root.FindAnyWidget("PlaySoundsetLooped"));
		m_StopSoundset 		= ButtonWidget.Cast(root.FindAnyWidget("StopSoundset"));
		m_DebugMapWidget = MapWidget.Cast(root.FindAnyWidget("MapSounds"));
		
		m_SoundFilter = EditBoxWidget.Cast(root.FindAnyWidget("SoundsFilter"));
		m_SoundsTextListbox = TextListboxWidget.Cast(root.FindAnyWidget("SoundsList"));
		
		m_SoundFilter.SetText(m_ConfigDebugProfile.GetSoundsetFilter());
		
		
		ChangeFilterSound();
		
		if (GetGame().GetPlayer())
		{
			m_DebugMapWidget.SetScale(DEBUG_MAP_ZOOM);
			m_DebugMapWidget.SetMapPos(GetGame().GetPlayer().GetWorldPosition());
			SetMapPos(GetGame().GetPlayer().GetWorldPosition());
		}
	}
	
	void ~ScriptConsoleSoundsTab(Widget root)
	{
		DEBUG_MAP_ZOOM = m_DebugMapWidget.GetScale();
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		
		if (w == m_SoundFilter)
		{
			m_RefreshFilterTimer.Run(0.85, this, "ChangeFilterSound", null, false);
			return true;
		}
		return false;
	}
	
	void UpdateMousePos()
	{
		if(!GetGame().GetPlayer())
			return;
		int x,y;
		GetMousePos(x,y);
		vector mousePos, worldPos;
		mousePos[0] = x;
		mousePos[1] = y;
		worldPos = m_DebugMapWidget.ScreenToMap(mousePos);
		vector playerPos = GetGame().GetPlayer().GetWorldPosition();
		if (GetMapPos() != playerPos)
			worldPos = GetMapPos();
		worldPos[1] = GetGame().SurfaceY(worldPos[0], worldPos[2]);
		
		if (m_MouseCurPos)
		{
			m_MouseCurPos.SetText("Mouse: "+ MiscGameplayFunctions.TruncateToS(worldPos[0]) +", "+ MiscGameplayFunctions.TruncateToS(worldPos[1]) +", "+ MiscGameplayFunctions.TruncateToS(worldPos[2]));
		}
		if (m_MapDistWidget && GetGame().GetPlayer())
		{
			float dst = (worldPos - playerPos).Length();

			m_MapDistWidget.SetText("Distance: " + MiscGameplayFunctions.TruncateToS(dst));
		}
		if (m_MapHeadingWidget)
		{
			vector playerCamDir = GetGame().GetCurrentCameraDirection();
			float heading = Math3D.AngleFromPosition(playerPos, playerCamDir, worldPos) * Math.RAD2DEG;
			m_MapHeadingWidget.SetText("Heading:" +heading.ToString());
		}
	}
	
	protected void PrepareFilters(string filter, out TStringArray filters)
	{
		filter.Trim();
		filter.ToLower();

		filters = new TStringArray;
		TStringArray rawFilters = new TStringArray;
		filter.Split(" ", rawFilters);
		
		foreach (int i, string f:rawFilters)
		{
			filters.Insert(f);

		}
	}
	
	// this is a 95% of the code of the function of the same name inside ScriptConsole.c
	protected void ChangeFilter(TStringArray classes, TextListboxWidget widget, EditBoxWidget filterWidget, int categoryMask = -1, bool ignoreScope = false)
	{
		widget.ClearItems();
		
		TStringArray filters;
		PrepareFilters(filterWidget.GetText(),filters);

		map<string,ref TStringArray> itemsByFilters = new map<string,ref TStringArray>();

		TStringArray itemsArray = TStringArray();
		
		for (int i = 0; i < classes.Count(); i++)
		{
			string config_path = classes.Get(i);

			int objects_count = GetGame().ConfigGetChildrenCount(config_path);
			for (int j = 0; j < objects_count; j++)
			{
				string child_name;
				bool add = false;
				GetGame().ConfigGetChildName(config_path, j, child_name);

				if (!filters.Count())
				{
					add = true;
				}
				else
				{
					foreach (int indx, string filter:filters)
					{
						string child_name_lower = child_name;
						child_name_lower.ToLower();
						
						if (child_name_lower.Contains(filter))
						{
							add = true;
							break;			
						}
					}
				}
				if (add)
					itemsArray.Insert(child_name);
			}
		}
	
		if (itemsArray)
		{
			itemsArray.Sort();
			foreach (string it:itemsArray)
			{
				widget.AddItem(it, NULL, 0);
			}
		}
		
		if (filters)
		{
			/*
			if (m_FilterOrderReversed)
				filters.Invert();
			*/
			foreach (string f:filters)
			{
				TStringArray arr2 = itemsByFilters.Get(f);
				if (arr2)
				{
					arr2.Sort();
					foreach (string itm: arr2)
					{
						int row = widget.AddItem(itm, NULL, 0);
					}
				}
			}
		}
		
		
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		HandleKeys();
		m_DebugMapWidget.ClearUserMarks();
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		
		if (player)
		{
			vector playerPos = player.GetWorldPosition();
			m_DebugMapWidget.AddUserMark(playerPos,"You", COLOR_RED,"\\dz\\gear\\navigation\\data\\map_tree_ca.paa");
			
			if (playerPos != GetMapPos())
				m_DebugMapWidget.AddUserMark(GetMapPos(),"Source", COLOR_GREEN,"\\dz\\gear\\navigation\\data\\map_tree_ca.paa");
		}
		UpdateMousePos();
	}
	
	protected void HandleKeys()
	{
		bool playRequested = KeyState(KeyCode.KC_P) != 0;
		if (playRequested)
		{
			int selected_row_index = m_SoundsTextListbox.GetSelectedRow();
			
			if (m_SoundSet)
				m_SoundSet.Stop();
			if (selected_row_index != -1)
			{
				string soundSetName;
				m_SoundsTextListbox.GetItemText(selected_row_index, 0, soundSetName);
				
				m_SoundSet = SEffectManager.PlaySoundEnviroment(soundSetName, GetMapPos());
				m_SoundSet.SetAutodestroy(true);
			}
		}
	}
	
	protected void SetMapPos(vector pos)
	{
		m_MapPos = pos;
	}
	
	protected vector GetMapPos()
	{
		return m_MapPos;
	}
	
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		super.OnMouseButtonDown(w,x,y,button);
		
		if (w ==  m_DebugMapWidget)
		{
			if (button == 0)
			{
				m_PlayerPosRefreshBlocked = true;
				int mouseX, mouseY;
				GetMousePos(mouseX,mouseY);
				vector mousePos, worldPos;
				mousePos[0] = mouseX;
				mousePos[1] = mouseY;
				worldPos = m_DebugMapWidget.ScreenToMap(mousePos);
				worldPos[1] = GetGame().SurfaceY(worldPos[0], worldPos[2]);
				
				SetMapPos(worldPos);
			}
			else if (button == 1 && GetGame().GetPlayer())
			{
				SetMapPos(GetGame().GetPlayer().GetWorldPosition());
			}
		}
		return true;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w,x,y,button);
		
		if (w == m_CopySoundset)
		{
			AddItemToClipboard(m_SoundsTextListbox);
			return true;
		}
		else if (w == m_PlaySoundset || w == m_PlaySoundsetLooped)
		{
			int selected_row_index = m_SoundsTextListbox.GetSelectedRow();
			string soundSetName;
			if (m_SoundSet)
				m_SoundSet.Stop();
			if (selected_row_index != -1)
			{
				m_SoundsTextListbox.GetItemText(selected_row_index, 0, soundSetName);
				
				bool looped = (w == m_PlaySoundsetLooped);
				m_SoundSet = SEffectManager.PlaySoundEnviroment(soundSetName, GetMapPos(), 0, 0, looped);

			}
			return true;
		}
		else if (w == m_StopSoundset)
		{
			if (m_SoundSet)
				m_SoundSet.Stop();
			return true;
		}
		else if (w == m_SoundFilter)
		{
			ChangeFilterSound();
			return true;
		}
		
		return false;
	}
	

	protected TStringArray GetSoundClasses()
	{
		return {CFG_SOUND_SETS};
	}
	
	protected void ChangeFilterSound()
	{
		m_ConfigDebugProfile.SetSoundsetFilter(m_SoundFilter.GetText());
		ChangeFilter(GetSoundClasses(), m_SoundsTextListbox, m_SoundFilter, 0,true);
	}
	
}
