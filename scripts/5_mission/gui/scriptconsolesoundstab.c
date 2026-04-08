class ScriptConsoleSoundsTab : ScriptConsoleTabBase
{
	protected bool 				m_PlayerPosRefreshBlocked;
	protected static float 		DEBUG_MAP_ZOOM = 1;
	protected vector 			m_MapPos;
	protected static int 		m_SelectedID;
	protected static string 	m_SelectedSoundEventType;
	protected static string 	m_SelectedAttachments;
	
	protected ButtonWidget 		m_SetETSoundButton;
	protected ButtonWidget 		m_SetETSoundWeaponButton;
	protected ButtonWidget 		m_SetETSoundAttachmentButton;
	protected ButtonWidget 		m_SetETSoundVoiceButton;
	protected ButtonWidget 		m_PlaySoundEventButton;
	protected ButtonWidget 		m_CopySoundset;
	protected ButtonWidget 		m_PlaySoundset;
	protected ButtonWidget 		m_PlaySoundsetLooped;
	protected ButtonWidget 		m_StopSoundset;

	protected EditBoxWidget 		m_SoundEventIDBox;
	protected EditBoxWidget 		m_SoundFilter;
	protected TextListboxWidget 	m_SoundsTextListbox;
	
	protected static EffectSound 	m_SoundSet;
	protected ref Timer 			m_RefreshFilterTimer = new Timer();
	
	protected MapWidget 			m_DebugMapWidget;
	protected TextWidget 			m_MapDistWidget;
	protected TextWidget	 		m_MouseCurPos;
	protected TextWidget	 		m_MapHeadingWidget;
	
	protected ImageWidget			m_BBackgroundSound;
	protected ImageWidget			m_BBackgroundSoundWeapon;
	protected ImageWidget			m_BBackgroundSoundAttachment;
	protected ImageWidget			m_BBackgroundSoundVoice;
	protected ImageWidget			m_SelectedBackground;
	
	protected CheckBoxWidget		m_CheckBoxShoulderAtt;
	protected CheckBoxWidget		m_CheckBoxBodyAtt;
	protected CheckBoxWidget		m_CheckBoxBackAtt;
	
	void ScriptConsoleSoundsTab(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
	{
		m_MouseCurPos		= TextWidget.Cast(root.FindAnyWidget("MapSoundsPos"));
		m_MapDistWidget		= TextWidget.Cast(root.FindAnyWidget("MapSoundsDistance"));
		m_MapHeadingWidget	= TextWidget.Cast(root.FindAnyWidget("MapHeadingSounds"));
		
		m_SetETSoundButton				= ButtonWidget.Cast(root.FindAnyWidget("ETSoundButton"));
		m_SetETSoundWeaponButton		= ButtonWidget.Cast(root.FindAnyWidget("ETSoundWeaponButton"));
		m_SetETSoundAttachmentButton	= ButtonWidget.Cast(root.FindAnyWidget("ETSoundAttachmentButton"));
		m_SetETSoundVoiceButton			= ButtonWidget.Cast(root.FindAnyWidget("ETSoundVoiceButton"));
		m_PlaySoundEventButton			= ButtonWidget.Cast(root.FindAnyWidget("ETSoundPlayButton"));
		m_SoundEventIDBox 				= EditBoxWidget.Cast(root.FindAnyWidget("SoundIDEditBox"));
		m_CheckBoxShoulderAtt			= CheckBoxWidget.Cast(root.FindAnyWidget("CheckBoxShoulderAtt"));
		m_CheckBoxBodyAtt				= CheckBoxWidget.Cast(root.FindAnyWidget("CheckBoxBodyAtt"));
		m_CheckBoxBackAtt				= CheckBoxWidget.Cast(root.FindAnyWidget("CheckBoxBackAtt"));
		m_BBackgroundSound				= ImageWidget.Cast(root.FindAnyWidget("SoundEventType"));
		m_BBackgroundSoundWeapon		= ImageWidget.Cast(root.FindAnyWidget("SoundEventType0"));
		m_BBackgroundSoundAttachment	= ImageWidget.Cast(root.FindAnyWidget("SoundEventType1"));
		m_BBackgroundSoundVoice			= ImageWidget.Cast(root.FindAnyWidget("SoundEventType2"));
		
		m_CopySoundset		= ButtonWidget.Cast(root.FindAnyWidget("SoundsetCopy"));
		m_PlaySoundset		= ButtonWidget.Cast(root.FindAnyWidget("PlaySoundset"));
		m_PlaySoundsetLooped = ButtonWidget.Cast(root.FindAnyWidget("PlaySoundsetLooped"));
		m_StopSoundset 		= ButtonWidget.Cast(root.FindAnyWidget("StopSoundset"));
		m_DebugMapWidget = MapWidget.Cast(root.FindAnyWidget("MapSounds"));

		m_SoundFilter = EditBoxWidget.Cast(root.FindAnyWidget("SoundsFilter"));
		m_SoundsTextListbox = TextListboxWidget.Cast(root.FindAnyWidget("SoundsList"));
		
		m_SoundFilter.SetText(m_ConfigDebugProfile.GetSoundsetFilter());
		
		
		ChangeFilterSound();
		
		if (g_Game.GetPlayer())
		{
			m_DebugMapWidget.SetScale(DEBUG_MAP_ZOOM);
			m_DebugMapWidget.SetMapPos(g_Game.GetPlayer().GetWorldPosition());
			SetMapPos(g_Game.GetPlayer().GetWorldPosition());
		}
		
		// reopen update
		m_SoundEventIDBox.SetText(m_SelectedID.ToString());
		
		if (m_SelectedSoundEventType == "Sound")
			OnClick(m_SetETSoundButton,0,0,0);
		else if (m_SelectedSoundEventType == "SoundWeapon")
			OnClick(m_SetETSoundWeaponButton,0,0,0);
		else if (m_SelectedSoundEventType == "SoundAttachment")
			OnClick(m_SetETSoundAttachmentButton,0,0,0);
		else if (m_SelectedSoundEventType == "SoundVoice")
			OnClick(m_SetETSoundVoiceButton,0,0,0);
		
		string lastSelection = m_SelectedAttachments;
		if (lastSelection.Contains("shoulder"))
			m_CheckBoxShoulderAtt.SetChecked(true);
		if (lastSelection.Contains("body"))
			m_CheckBoxBodyAtt.SetChecked(true);
		if (lastSelection.Contains("back"))
			m_CheckBoxBackAtt.SetChecked(true);
		
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
		if(!g_Game.GetPlayer())
			return;
		int x,y;
		GetMousePos(x,y);
		vector mousePos, worldPos;
		mousePos[0] = x;
		mousePos[1] = y;
		worldPos = m_DebugMapWidget.ScreenToMap(mousePos);
		vector playerPos = g_Game.GetPlayer().GetWorldPosition();
		if (GetMapPos() != playerPos)
			worldPos = GetMapPos();
		worldPos[1] = g_Game.SurfaceY(worldPos[0], worldPos[2]);
		
		if (m_MouseCurPos)
		{
			m_MouseCurPos.SetText("Mouse: "+ MiscGameplayFunctions.TruncateToS(worldPos[0]) +", "+ MiscGameplayFunctions.TruncateToS(worldPos[1]) +", "+ MiscGameplayFunctions.TruncateToS(worldPos[2]));
		}
		if (m_MapDistWidget && g_Game.GetPlayer())
		{
			float dst = (worldPos - playerPos).Length();

			m_MapDistWidget.SetText("Distance: " + MiscGameplayFunctions.TruncateToS(dst));
		}
		if (m_MapHeadingWidget)
		{
			vector playerCamDir = g_Game.GetCurrentCameraDirection();
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

			int objects_count = g_Game.ConfigGetChildrenCount(config_path);
			for (int j = 0; j < objects_count; j++)
			{
				string child_name;
				bool add = false;
				g_Game.ConfigGetChildName(config_path, j, child_name);

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
		
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		
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
	
	protected void PlaySoundEvent()
	{
		m_SelectedID = m_SoundEventIDBox.GetText().ToInt();
		if (m_SelectedID == 0)
			return;
		
		DayZPlayerImplement player = DayZPlayerImplement.Cast(g_Game.GetPlayer());
		
		if (m_SelectedSoundEventType == "SoundAttachment")
			player.OnSoundEvent(m_SelectedSoundEventType, m_SelectedAttachments, m_SelectedID);
		else 
			player.OnSoundEvent(m_SelectedSoundEventType, string.Empty, m_SoundEventIDBox.GetText().ToInt());
	}
	
	protected void UpdateAttachmentSelection()
	{
		string attachments = string.Empty;
		if (m_CheckBoxShoulderAtt.IsChecked())
			attachments += "shoulder,";
		if (m_CheckBoxBodyAtt.IsChecked())
			attachments += "body,";
		if (m_CheckBoxBackAtt.IsChecked())
			attachments += "back";
		
		m_SelectedAttachments = attachments;
	}
	
	protected void UpdateSelectedColor(ImageWidget buttonBackground)
	{
		if (m_SelectedBackground)
			m_SelectedBackground.SetColor(0xFF141414);
		
		buttonBackground.SetColor(COLOR_RED);
		
		m_SelectedBackground = buttonBackground;
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
				worldPos[1] = g_Game.SurfaceY(worldPos[0], worldPos[2]);
				
				SetMapPos(worldPos);
			}
			else if (button == 1 && g_Game.GetPlayer())
			{
				SetMapPos(g_Game.GetPlayer().GetWorldPosition());
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
		else if (w == m_SetETSoundButton)
		{
			m_SelectedSoundEventType = "Sound";
			UpdateSelectedColor(m_BBackgroundSound);
			return true;
		}
		else if (w == m_SetETSoundWeaponButton)
		{
			m_SelectedSoundEventType = "SoundWeapon";
			UpdateSelectedColor(m_BBackgroundSoundWeapon);
			return true;
		}
		else if (w == m_SetETSoundAttachmentButton)
		{
			m_SelectedSoundEventType = "SoundAttachment";
			UpdateSelectedColor(m_BBackgroundSoundAttachment);
			return true;
		}
		else if (w == m_SetETSoundVoiceButton)
		{
			m_SelectedSoundEventType = "SoundVoice";
			UpdateSelectedColor(m_BBackgroundSoundVoice);
			return true;
		}
		else if (w == m_PlaySoundEventButton)
		{
			PlaySoundEvent();
			return true;
		}
		else if (w == m_CheckBoxShoulderAtt || w == m_CheckBoxBodyAtt || w == m_CheckBoxBackAtt)
		{
			UpdateAttachmentSelection();
			return true;
		}

		return false;
	}
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		super.OnDoubleClick(w, x, y, button);
				
		if (w == m_SoundEventIDBox || w == m_SoundFilter)
		{
			EditBoxWidget.Cast(w).SetText("");
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
