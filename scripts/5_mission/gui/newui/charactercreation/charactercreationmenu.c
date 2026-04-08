class CharacterCreationMenu extends UIScriptedMenu
{
	#ifdef PLATFORM_CONSOLE
	DayZIntroSceneXbox										m_Scene;
	protected bool 											m_CharacterSaved;
	#else
	DayZIntroScenePC										m_Scene;
	#endif
	
	const int 												TOOLTIP_ID_SAVE = 1;
	const int 												TOOLTIP_ID_APPLY = 2;
	
	protected Widget										m_CharacterRotationFrame;
	protected Widget										m_Apply;
	protected Widget										m_Save;
	protected Widget										m_RandomizeCharacter;
	protected Widget										m_BackButton;
	protected Widget										m_PlayedCharacterInfo;
	protected TextWidget									m_Version;
	protected Widget										m_DetailsRoot; //tooltips
	protected TextWidget									m_DetailsLabel;
	protected RichTextWidget								m_DetailsText;
	protected TextWidget 									m_CharacterHeaderText;
	
	protected ref OptionSelectorEditbox						m_NameSelector;
	protected ref OptionSelectorMultistateCharacterMenu		m_GenderSelector;
	protected ref OptionSelectorMultistateCharacterMenu		m_SkinSelector;
	protected ref OptionSelectorMultistateCharacterMenu		m_TopSelector;
	protected ref OptionSelectorMultistateCharacterMenu		m_BottomSelector;
	protected ref OptionSelectorMultistateCharacterMenu		m_ShoesSelector;
	
	protected ref map<Widget, OptionSelectorMultistateCharacterMenu> m_MultiOptionSelectors;
	
	int m_OriginalCharacterID;
	
	void CharacterCreationMenu()
	{
		MissionMainMenu mission = MissionMainMenu.Cast(g_Game.GetMission());
		
		#ifdef PLATFORM_CONSOLE
		m_Scene = mission.GetIntroSceneXbox();
		#else
		m_Scene = mission.GetIntroScenePC();
		#endif
		
		m_Scene.ResetIntroCamera();
	}
	
	PlayerBase GetPlayerObj()
	{
		return m_Scene.GetIntroCharacter().GetCharacterObj();
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		#ifdef PLATFORM_CONSOLE
		switch (pInputDeviceType)
		{
		case EInputDeviceType.CONTROLLER:
			if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
			{
				g_Game.GetUIManager().ShowUICursor(false);
			}
			CheckNewOptions(); //TODO - pick only the 'focus' bit
		break;

		default:
			if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
			{
				g_Game.GetUIManager().ShowUICursor(true);
			}
		break;
		}
		
		UpdateControlsElementVisibility();
		#endif
	}
	
	override Widget Init()
	{
		#ifdef PLATFORM_CONSOLE
			layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/character_creation/xbox/character_creation.layout");
			m_CharacterSaved 				= false;
		#else
			layoutRoot = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/character_creation/pc/character_creation.layout");
		#endif
		
		m_CharacterRotationFrame			= layoutRoot.FindAnyWidget("character_rotation_frame");
		m_Apply								= layoutRoot.FindAnyWidget("apply");
		m_Save 								= layoutRoot.FindAnyWidget("save");
		m_RandomizeCharacter				= layoutRoot.FindAnyWidget("randomize_character");
		m_BackButton						= layoutRoot.FindAnyWidget("back");
		m_Version							= TextWidget.Cast(layoutRoot.FindAnyWidget("version"));
		m_DetailsRoot 						= layoutRoot.FindAnyWidget("menu_details_tooltip");
		m_DetailsLabel						= TextWidget.Cast(m_DetailsRoot.FindAnyWidget("menu_details_label"));
		m_DetailsText						= RichTextWidget.Cast(m_DetailsRoot.FindAnyWidget("menu_details_tooltip_content"));
		m_CharacterHeaderText 				= TextWidget.Cast(layoutRoot.FindAnyWidget("char_header_text"));
		m_PlayedCharacterInfo 				= layoutRoot.FindAnyWidget("played_char_info");
		
		string version;
		g_Game.GetVersion(version);
		#ifdef PLATFORM_CONSOLE
			version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
		#else
			version = "#main_menu_version" + " " + version;
		#endif
		m_Version.SetText(version);
		
		if (m_Scene && m_Scene.GetIntroCharacter())
		{
			m_OriginalCharacterID = m_Scene.GetIntroCharacter().GetCharacterID();
		}
		
		m_NameSelector		= new OptionSelectorEditbox(layoutRoot.FindAnyWidget("character_name_setting_option"), m_Scene.GetIntroCharacter().GetCharacterName(), null, false);
		m_GenderSelector	= new OptionSelectorMultistateCharacterMenu(layoutRoot.FindAnyWidget("character_gender_setting_option"), 0, null, false, m_Scene.GetIntroCharacter().GetCharGenderList());
		if (m_Scene.GetIntroCharacter().IsCharacterFemale())
		{
			m_GenderSelector.SetValue("Female");
			m_SkinSelector	= new OptionSelectorMultistateCharacterMenu(layoutRoot.FindAnyWidget("character_head_setting_option"), 0, null, false, m_Scene.GetIntroCharacter().GetCharList(ECharGender.Female));
		}
		else
		{
			m_GenderSelector.SetValue("Male");
			m_SkinSelector	= new OptionSelectorMultistateCharacterMenu(layoutRoot.FindAnyWidget("character_head_setting_option"), 0, null, false, m_Scene.GetIntroCharacter().GetCharList(ECharGender.Male));
		}
		
		m_TopSelector		= new OptionSelectorMultistateCharacterMenu(layoutRoot.FindAnyWidget("character_top_setting_option"), 0, null, false, DefaultCharacterCreationMethods.GetConfigAttachmentTypes(InventorySlots.BODY));
		m_BottomSelector	= new OptionSelectorMultistateCharacterMenu(layoutRoot.FindAnyWidget("character_bottom_setting_option"), 0, null, false, DefaultCharacterCreationMethods.GetConfigAttachmentTypes(InventorySlots.LEGS));
		m_ShoesSelector		= new OptionSelectorMultistateCharacterMenu(layoutRoot.FindAnyWidget("character_shoes_setting_option"), 0, null, false, DefaultCharacterCreationMethods.GetConfigAttachmentTypes(InventorySlots.FEET));
		
		m_GenderSelector.ShowNavigationButtons(true);
		m_SkinSelector.ShowNavigationButtons(true);
		m_TopSelector.ShowNavigationButtons(true);
		m_BottomSelector.ShowNavigationButtons(true);
		m_ShoesSelector.ShowNavigationButtons(true);
		
		if (!m_MultiOptionSelectors)
		{
			m_MultiOptionSelectors = new map<Widget, OptionSelectorMultistateCharacterMenu>;
			m_MultiOptionSelectors.Insert(layoutRoot.FindAnyWidget("character_gender_button"), m_GenderSelector);
			m_MultiOptionSelectors.Insert(layoutRoot.FindAnyWidget("character_head_button"), m_SkinSelector);
			m_MultiOptionSelectors.Insert(layoutRoot.FindAnyWidget("character_top_button"), m_TopSelector);
			m_MultiOptionSelectors.Insert(layoutRoot.FindAnyWidget("character_bottom_button"), m_BottomSelector);
			m_MultiOptionSelectors.Insert(layoutRoot.FindAnyWidget("character_shoes_button"), m_ShoesSelector);
		}
		
		PlayerBase scene_char = GetPlayerObj();
		if (scene_char)
		{
			Object obj = scene_char.GetInventory().FindAttachment(InventorySlots.BODY);
			if (obj)
				m_TopSelector.SetValue(obj.GetType(), false);
			
			obj = scene_char.GetInventory().FindAttachment(InventorySlots.LEGS);
			if (obj)
				m_BottomSelector.SetValue(obj.GetType(), false);
			
			obj = scene_char.GetInventory().FindAttachment(InventorySlots.FEET);
			if (obj)
				m_ShoesSelector.SetValue(obj.GetType(), false);
			
			m_SkinSelector.SetValue(scene_char.GetType());
		}
		
		m_GenderSelector.m_OptionChanged.Insert(GenderChanged);
		m_SkinSelector.m_OptionChanged.Insert(SkinChanged);
		m_TopSelector.m_OptionChanged.Insert(TopChanged);
		m_BottomSelector.m_OptionChanged.Insert(BottomChanged);
		m_ShoesSelector.m_OptionChanged.Insert(ShoesChanged);
		
		Refresh();
		SetCharacter();
		CheckNewOptions();
		
		g_Game.GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);

		return layoutRoot;
	}
	
	void ~CharacterCreationMenu()
	{
		m_GenderSelector.m_OptionChanged.Remove(GenderChanged);
		m_SkinSelector.m_OptionChanged.Remove(SkinChanged);
		m_TopSelector.m_OptionChanged.Remove(TopChanged);
		m_BottomSelector.m_OptionChanged.Remove(BottomChanged);
		m_ShoesSelector.m_OptionChanged.Remove(ShoesChanged);
	}
	
	//Button Events
	//! renames character
	void Apply()
	{
		if (!m_Scene.GetIntroCharacter().IsDefaultCharacter())
		{
			string name = m_NameSelector.GetValue();
			if (name == "")
				name = GameConstants.DEFAULT_CHARACTER_NAME;
			
			m_Scene.GetIntroCharacter().SaveCharName(name);
		}
		
		MainMenu menu_main = MainMenu.Cast(g_Game.GetUIManager().FindMenu(MENU_MAIN));
		if (menu_main)
		{
			menu_main.OnChangeCharacter(false);
		}
		g_Game.GetUIManager().Back();
	}
	
	//! saves default character
	void Save()
	{
		if (m_Scene.GetIntroCharacter().IsDefaultCharacter())
		{
			string name = m_NameSelector.GetValue();
			if (name == "")
				name = GameConstants.DEFAULT_CHARACTER_NAME;
			
			m_Scene.GetIntroCharacter().SaveCharName(name);
			m_Scene.GetIntroCharacter().SaveDefaultCharacter();
			m_Scene.GetIntroCharacter().SetToDefaultCharacter();
			SetCharacterSaved(true);
		}
	}
	
	void Back()
	{
		//bring back DefaultCharacter, if it exists (it should), or a previously played one.
		g_Game.GetMenuData().RequestGetDefaultCharacterData();
		#ifdef PLATFORM_CONSOLE
			if (m_OriginalCharacterID != GameConstants.DEFAULT_CHARACTER_MENU_ID && m_CharacterSaved)
			{
				m_OriginalCharacterID = GameConstants.DEFAULT_CHARACTER_MENU_ID;
			}
		#endif
		m_Scene.GetIntroCharacter().SetCharacterID(m_OriginalCharacterID);
		m_Scene.GetIntroCharacter().CreateNewCharacterById(m_Scene.GetIntroCharacter().GetCharacterID());
		g_Game.GetUIManager().Back();
	}
	
	void SetCharacter()
	{
		if (m_Scene.GetIntroCharacter().IsDefaultCharacter())
		{
			g_Game.GetMenuDefaultCharacterData().EquipDefaultCharacter(m_Scene.GetIntroCharacter().GetCharacterObj());
		}
	}
	
	void RandomizeCharacter()
	{
		m_Scene.GetIntroCharacter().SetToDefaultCharacter();
		
		// make random selection
		m_Scene.GetIntroCharacter().SetCharacterGender(Math.RandomInt(0, 2));
		
		if (m_Scene.GetIntroCharacter().IsCharacterFemale())
		{
			m_GenderSelector.SetValue("Female");
			m_SkinSelector.LoadNewValues(m_Scene.GetIntroCharacter().GetCharList(ECharGender.Female), 0);
			m_SkinSelector.SetRandomValue();
		}
		else
		{
			m_GenderSelector.SetValue("Male");
			m_SkinSelector.LoadNewValues(m_Scene.GetIntroCharacter().GetCharList(ECharGender.Male), 0);
			m_SkinSelector.SetRandomValue();
		}
		
		g_Game.GetMenuDefaultCharacterData().GenerateRandomEquip();
		
		m_TopSelector.SetValue(g_Game.GetMenuDefaultCharacterData().GetAttachmentMap().Get(InventorySlots.BODY),false);
		m_BottomSelector.SetValue(g_Game.GetMenuDefaultCharacterData().GetAttachmentMap().Get(InventorySlots.LEGS),false);
		m_ShoesSelector.SetValue(g_Game.GetMenuDefaultCharacterData().GetAttachmentMap().Get(InventorySlots.FEET),false);
		
		Refresh();
		SetCharacter();
		
		CheckNewOptions();
	}
	
	//Selector Events
	void GenderChanged()
	{
		ECharGender gender = ECharGender.Male;
		
		if (m_GenderSelector.GetStringValue() == "Female")
		{
			gender = ECharGender.Female;
		}
		
		m_Scene.GetIntroCharacter().SetCharacterGender(gender);
		
		m_SkinSelector.LoadNewValues(m_Scene.GetIntroCharacter().GetCharList(gender) , 0);
		m_SkinSelector.SetRandomValue();
		SetCharacterSaved(false);
	}
	
	void SkinChanged()
	{
		m_Scene.GetIntroCharacter().CreateNewCharacterByName(m_SkinSelector.GetStringValue(), false);
		SetCharacterSaved(false);
	}
	
	void TopChanged()
	{
		g_Game.GetMenuDefaultCharacterData().SetDefaultAttachment(InventorySlots.BODY,m_TopSelector.GetStringValue());
		g_Game.GetMenuDefaultCharacterData().EquipDefaultCharacter(m_Scene.GetIntroCharacter().GetCharacterObj());
		SetCharacterSaved(false);
	}
	
	void BottomChanged()
	{
		g_Game.GetMenuDefaultCharacterData().SetDefaultAttachment(InventorySlots.LEGS,m_BottomSelector.GetStringValue());
		g_Game.GetMenuDefaultCharacterData().EquipDefaultCharacter(m_Scene.GetIntroCharacter().GetCharacterObj());
		SetCharacterSaved(false);
	}
	
	void ShoesChanged()
	{
		g_Game.GetMenuDefaultCharacterData().SetDefaultAttachment(InventorySlots.FEET,m_ShoesSelector.GetStringValue());
		g_Game.GetMenuDefaultCharacterData().EquipDefaultCharacter(m_Scene.GetIntroCharacter().GetCharacterObj());
		SetCharacterSaved(false);
	}
	
	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		super.OnKeyPress(w, x, y, key);
		return false;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_Apply)
		{
			Apply();
			return true;
		}
		else if (w == m_Save)
		{
			Save();
			g_Game.GetUIManager().Back();
			return true;
		}
		else if (w == m_RandomizeCharacter)
		{
			RandomizeCharacter();
			return true;
		}
		else if (w == m_BackButton)
		{
			Back();
			return true;
		}
		return false;
	}
	
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		#ifndef PLATFORM_CONSOLE
		if (w == m_CharacterRotationFrame)
		{
			if (m_Scene)
				DayZIntroScenePC.Cast(m_Scene).CharacterRotationStart();
			return true;
		}
		#endif
		return false;
	}
	
	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		#ifndef PLATFORM_CONSOLE
		if (m_Scene)
			DayZIntroScenePC.Cast(m_Scene).CharacterRotationStop();
		#endif
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		string tooltip_header = "";
		string tooltip_text = "";
		ColorHighlight(w);
		switch (w.GetUserID())
		{
			case TOOLTIP_ID_APPLY:
				tooltip_header = "#layout_main_menu_rename";
				tooltip_text = "#layout_character_creation_apply_tooltip";
			break;
			
			case TOOLTIP_ID_SAVE:
				tooltip_header = "#layout_character_creation_save_character_alt";
				tooltip_text = "#layout_character_creation_save_tooltip";
			break;
		}
		SetTooltipTexts(w, tooltip_header, tooltip_text);
		return true;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		ColorNormal(w);
		SetFocus(null);
		
		return true;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		string tooltip_header = "";
		string tooltip_text = "";
		if (IsFocusable(w))
		{
			ColorHighlight(w);
			switch (w.GetUserID())
			{
				case TOOLTIP_ID_APPLY:
					tooltip_header = "#layout_main_menu_rename";
					tooltip_text = "#layout_character_creation_apply_tooltip";
				break;
				
				case TOOLTIP_ID_SAVE:
					tooltip_header = "#layout_character_creation_save_character_alt";
					tooltip_text = "#layout_character_creation_save_tooltip";
				break;
			}
			SetTooltipTexts(w, tooltip_header, tooltip_text);
			return true;
		}
		SetTooltipTexts(w);
		return false;
	}
	
	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (IsFocusable(w))
		{
			ColorNormal(w);
			return true;
		}
		return false;
	}
	
	bool IsFocusable(Widget w)
	{
		if (w)
		{
			return (w == m_Apply || w == m_Save || w == m_RandomizeCharacter || w == m_BackButton);
		}
		return false;
	}
	
	void SetTooltipTexts(Widget w, string header = "", string desc = "")
	{
		#ifndef PLATFORM_CONSOLE
			bool show = header != "" && desc != "";
			m_DetailsRoot.Show(show);
			m_DetailsLabel.SetText(header);
			m_DetailsText.SetText(desc);
			
			//moves the widget to parent, setting is relative to widget alignment, case-specific implementation (getter missing!!!)
			if (show)
			{
				float parent_pos_x, parent_pos_y;
				float parent_size_x, parent_size_y;
				float layout_size_x, layout_size_y;
				
				w.GetScreenPos(parent_pos_x,parent_pos_y);
				w.GetScreenSize(parent_size_x,parent_size_y);
				layoutRoot.GetScreenSize(layout_size_x,layout_size_y);
				
				float set_x = layout_size_x - parent_pos_x;
				float set_y = layout_size_y - parent_pos_y - parent_size_y;
				m_DetailsRoot.SetPos(set_x,set_y,true);
			}
			
			m_DetailsText.Update();
			m_DetailsLabel.Update();
			m_DetailsRoot.Update();
		#endif
	}
	
	void CheckNewOptions()
	{
		bool show_widgets = m_Scene.GetIntroCharacter().IsDefaultCharacter();
		layoutRoot.FindAnyWidget("character_gender_button").Show(show_widgets);
		layoutRoot.FindAnyWidget("character_head_button").Show(show_widgets);
		layoutRoot.FindAnyWidget("character_top_button").Show(show_widgets);
		layoutRoot.FindAnyWidget("character_bottom_button").Show(show_widgets);
		layoutRoot.FindAnyWidget("character_shoes_button").Show(show_widgets);
		
		if (show_widgets)
		{
			m_GenderSelector.Focus();
		}
		else
		{
			SetFocus(m_RandomizeCharacter);
		}
	}
	
	override void OnShow()
	{
#ifdef PLATFORM_CONSOLE
		m_GenderSelector.Focus();
		OnInputDeviceChanged(g_Game.GetInput().GetCurrentInputDevice());
#endif
		CheckNewOptions();
	}
	
	override void Refresh()
	{
		string name;
		#ifdef PLATFORM_CONSOLE
			if (g_Game.GetUserManager() && g_Game.GetUserManager().GetSelectedUser())
			{
				name = g_Game.GetUserManager().GetSelectedUser().GetName();
				if (name.LengthUtf8() > 16)
				{
					name = name.SubstringUtf8(0, 16);
					name += "...";
				}
			}
		#else
			name = m_Scene.GetIntroCharacter().GetCharacterName();
			if (name == "")
				name = GameConstants.DEFAULT_CHARACTER_NAME;
		#endif
		
		m_NameSelector.SetValue(name);
		
		string version;
		g_Game.GetVersion(version);
		#ifdef PLATFORM_CONSOLE
			version = "#main_menu_version" + " " + version + " (" + g_Game.GetDatabaseID() + ")";
			
			m_Apply.Show(m_CharacterSaved || !m_Scene.GetIntroCharacter().IsDefaultCharacter());
			m_Save.Show(!m_CharacterSaved && m_Scene.GetIntroCharacter().IsDefaultCharacter());
		#else
			version = "#main_menu_version" + " " + version;
			m_Apply.Show(!m_Scene.GetIntroCharacter().IsDefaultCharacter());
			m_Save.Show(m_Scene.GetIntroCharacter().IsDefaultCharacter());
		#endif
		m_PlayedCharacterInfo.Show(!m_Scene.GetIntroCharacter().IsDefaultCharacter());
		
		if (m_Scene.GetIntroCharacter().IsDefaultCharacter())
		{
			m_CharacterHeaderText.SetText("#character_menu_header");
		}
		else
		{
			m_CharacterHeaderText.SetText("#server_browser_prev_play_filter");
		}
		
		m_Version.SetText(version);
		
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		UpdateControlsElementVisibility();
		#endif
	}
	
	override void Update(float timeslice)
	{
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			Back();
		}
		
		if (GetUApi().GetInputByID(UAUICtrlX).LocalPress())
		{
			RandomizeCharacter();
		}
		
		if (GetUApi().GetInputByID(UAUIRight).LocalPress())
		{
			NextOption();
		}
		
		if (GetUApi().GetInputByID(UAUILeft).LocalPress())
		{
			PreviousOption();
		}
		
		#ifdef PLATFORM_CONSOLE
		if (GetUApi().GetInputByID(UAUICtrlY).LocalPress())
		{
			if (m_Scene.GetIntroCharacter().IsDefaultCharacter() && !m_CharacterSaved)
			{
				Save();
			}
		}
		#endif
	}
	
	protected void NextOption()
	{
		Widget w = GetFocus();
		if (!w)
			return;

		OptionSelectorMultistateCharacterMenu optionSelector = m_MultiOptionSelectors[w];
		if (optionSelector)
			optionSelector.SetNextOption();
	}
	
	protected void PreviousOption()
	{
		Widget w = GetFocus();
		if (!w)
			return;

		OptionSelectorMultistateCharacterMenu optionSelector = m_MultiOptionSelectors[w];
		if (optionSelector)
			optionSelector.SetPrevOption();
	}
	
	override void OnHide()
	{
	}
	
	//Coloring functions (Until WidgetStyles are useful)
	void ColorHighlight(Widget w)
	{
		if (w.IsInherited(ButtonWidget))
		{
			ButtonWidget button = ButtonWidget.Cast(w);
			button.SetTextColor(ARGB(255, 200, 0, 0));
		}
		
		w.SetColor(ARGB(255, 0, 0, 0));
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text"));
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text_1"));
		ImageWidget image	= ImageWidget.Cast(w.FindAnyWidget(w.GetName() + "_image"));
		Widget option		= Widget.Cast(w.FindAnyWidget(w.GetName() + "_option_wrapper"));
		Widget option_label = w.FindAnyWidget("option_label");
		
		if (text1)
		{
			text1.SetColor(ARGB(255, 255, 0, 0));
		}
		
		if (text2)
		{
			text2.SetColor(ARGB(255, 255, 0, 0));
		}
		
		if (text3)
		{
			text3.SetColor(ARGB(255, 255, 0, 0));
			w.SetAlpha(1);
		}
		
		if (image)
		{
			image.SetColor(ARGB(255, 200, 0, 0));
		}
		
		if (option)
		{
			option.SetColor(ARGB(255, 255, 0, 0));
		}
		
		#ifndef PLATFORM_CONSOLE
		if (option_label)
		{
			option_label.SetColor(ARGB(255, 255, 0, 0));
		}
		#endif
	}
	
	void ColorNormal(Widget w)
	{
		if (w.IsInherited(ButtonWidget))
		{
			ButtonWidget button = ButtonWidget.Cast(w);
			button.SetTextColor(ColorManager.COLOR_NORMAL_TEXT);
		}
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text"));
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text_1"));
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		ImageWidget image	= ImageWidget.Cast(w.FindAnyWidget(w.GetName() + "_image"));
		Widget option		= w.FindAnyWidget(w.GetName() + "_option_wrapper");
		Widget option_label = w.FindAnyWidget("option_label");
		
		if (text1)
		{
			text1.SetColor(ColorManager.COLOR_NORMAL_TEXT);
		}
		
		if (text2)
		{
			text2.SetColor(ColorManager.COLOR_NORMAL_TEXT);
		}
		
		if (text3)
		{
			text3.SetColor(ColorManager.COLOR_NORMAL_TEXT);
			w.SetAlpha(0);
		}
		
		if (image)
		{
			image.SetColor(ColorManager.COLOR_NORMAL_TEXT);
		}
		
		if (option)
		{
			option.SetColor(ARGB(150, 255, 255, 255));
		}
		
		#ifndef PLATFORM_CONSOLE
		if (option_label)
		{
			option_label.SetColor(ColorManager.COLOR_NORMAL_TEXT);
		}
		#endif
	}
	
	void ColorDisable(Widget w)
	{
		#ifndef PLATFORM_CONSOLE
		SetFocus(null);
		#endif
		
		if (w)
		{
			ButtonWidget button = ButtonWidget.Cast(w);
			if (button)
			{
				button.SetTextColor(ColorManager.COLOR_DISABLED_TEXT);
			}
		}
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text"));
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text_1"));
		ImageWidget image	= ImageWidget.Cast(w.FindAnyWidget(w.GetName() + "_image"));
		Widget option		= Widget.Cast(w.FindAnyWidget(w.GetName() + "_option_wrapper"));
		Widget option_label = w.FindAnyWidget("option_label");
		
		if (text1)
		{
			text1.SetColor(ColorManager.COLOR_DISABLED_TEXT);
		}
		
		if (text2)
		{
			text2.SetColor(ColorManager.COLOR_DISABLED_TEXT);
		}
		
		if (text3)
		{
			text3.SetColor(ColorManager.COLOR_DISABLED_TEXT);
			w.SetAlpha(1);
		}
		
		if (image)
		{
			image.SetColor(ColorManager.COLOR_DISABLED_TEXT);
		}
		
		if (option)
		{
			option.SetColor(ColorManager.COLOR_DISABLED_TEXT);
		}
		
		#ifndef PLATFORM_CONSOLE
		if (option_label)
		{
			option_label.SetColor(ColorManager.COLOR_DISABLED_TEXT);
		}
		#endif
	}
	
	void SetCharacterSaved(bool state)
	{
		#ifdef PLATFORM_CONSOLE
			m_CharacterSaved = state;
			Refresh();
		#endif
	}

	protected void UpdateControlsElements()
	{
		#ifdef PLATFORM_CONSOLE
		RichTextWidget toolbar_text = RichTextWidget.Cast(layoutRoot.FindAnyWidget("ContextToolbarText"));
		string text = "";
		if (!m_CharacterSaved && m_Scene.GetIntroCharacter().IsDefaultCharacter()) //can be saved..
		{
			text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlY", "#layout_character_creation_save_character_alt", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		}
		text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUICtrlX", "#layout_character_creation_toolbar_randomize", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		if (m_Scene.GetIntroCharacter().IsDefaultCharacter()) //edit options available
		{
			#ifdef PLATFORM_PS4
			string result = string.Format("<image set=\"%1\" name=\"%2\" scale=\"%3\" />", "playstation_buttons", "DPAD_left_short", 1.92);
			string result2 = string.Format("<image set=\"%1\" name=\"%2\" scale=\"%3\" />", "playstation_buttons", "DPAD_right_short", 1.92);
			#else
			string result = string.Format("<image set=\"%1\" name=\"%2\" scale=\"%3\" />", "xbox_buttons", "DPAD_left_short", 1.92);
			string result2 = string.Format("<image set=\"%1\" name=\"%2\" scale=\"%3\" />", "xbox_buttons", "DPAD_right_short", 1.92);
			#endif
			text += string.Format(" %1%2 %3", result, result2, "#layout_character_creation_toolbar_select");
		}
		text += string.Format(" %1",InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "#layout_character_creation_toolbar_back", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		toolbar_text.SetText(text);
		#endif
	}
	
	protected void UpdateControlsElementVisibility()
	{
		#ifdef PLATFORM_CONSOLE
		bool toolbarShow = !g_Game.GetInput().IsEnabledMouseAndKeyboard() || g_Game.GetInput().GetCurrentInputDevice() == EInputDeviceType.CONTROLLER;
		layoutRoot.FindAnyWidget("toolbar_bg").Show(toolbarShow);
		layoutRoot.FindAnyWidget("play_panel_root").Show(!toolbarShow);
		#endif
	}
}