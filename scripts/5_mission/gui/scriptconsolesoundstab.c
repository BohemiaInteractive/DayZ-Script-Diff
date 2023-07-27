class ScriptConsoleSoundsTab : ScriptConsoleTabBase
{
	protected ButtonWidget 		m_CopySoundset;
	protected ButtonWidget 		m_PlaySoundset;
	protected ButtonWidget 		m_PlaySoundsetLooped;
	protected ButtonWidget 		m_StopSoundset;

	protected EditBoxWidget 		m_SoundFilter;
	protected TextListboxWidget 	m_SoundsTextListbox;
	
	protected static EffectSound 	m_SoundSet;
	protected ref Timer 			m_RefreshFilterTimer = new Timer();
	
	
	void ScriptConsoleSoundsTab(Widget root, ScriptConsole console)
	{
		m_CopySoundset		= ButtonWidget.Cast(root.FindAnyWidget("SoundsetCopy"));
		m_PlaySoundset		= ButtonWidget.Cast(root.FindAnyWidget("PlaySoundset"));
		m_PlaySoundsetLooped = ButtonWidget.Cast(root.FindAnyWidget("PlaySoundsetLooped"));
		m_StopSoundset 		= ButtonWidget.Cast(root.FindAnyWidget("StopSoundset"));
		
		m_SoundFilter = EditBoxWidget.Cast(root.FindAnyWidget("SoundsFilter"));
		m_SoundsTextListbox = TextListboxWidget.Cast(root.FindAnyWidget("SoundsList"));
		
		m_SoundFilter.SetText(m_ConfigDebugProfile.GetSoundsetFilter());
		ChangeFilterSound();
	}
	
	void ~ScriptConsoleSoundsTab(Widget root)
	{
		SEffectManager.DestroyEffect(m_SoundSet);
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
				
				if (w == m_PlaySoundsetLooped)
				{
					m_SoundSet = SEffectManager.PlaySoundEnviroment(soundSetName, GetGame().GetPlayer().GetPosition(), 0, 0, true);
				}
				else
				{
					m_SoundSet = SEffectManager.PlaySoundEnviroment(soundSetName, GetGame().GetPlayer().GetPosition());
				}
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
