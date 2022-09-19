
//used as UserID in the layout. Values assigned for convenience here
enum EConsoleButtonsControls
{
	INVALID = 0, //assumed unassigned value, ignored (dividers, formatting elements etc.)
	BUTTON_MENU = 1,
	BUTTON_VIEW = 2,
	BUTTON_A = 4,
	BUTTON_B = 8,
	BUTTON_X = 16,
	BUTTON_Y = 32,
	BUTTON_PAD_UP = 64,
	BUTTON_PAD_DOWN = 128,
	BUTTON_PAD_LEFT = 256,
	BUTTON_PAD_RIGHT = 512,
	BUTTON_SHOULDER_LEFT = 1024,
	BUTTON_SHOULDER_RIGHT = 2048,
	BUTTON_THUMB_LEFT = 4096,
	BUTTON_THUMB_RIGHT = 8192,
	BUTTON_TRIGGER_LEFT = 16384,
	BUTTON_TRIGGER_RIGHT = 32768,
	BUTTON_GROUP_RIGHT_SIDE_COMMON = 65536,
	BUTTON_GROUP_PAD_COMMON = 131072
}

typedef map<int,ref array<Widget>> TButtonPairingInfo; //<button_mask,<associated widgets on the respective side>>

class ControlsXboxNew extends UIScriptedMenu
{
	protected string 					m_BackButtonTextID;
	protected int 						m_CurrentTabIdx = -1;
	protected int 						m_CurrentPresetVariant = -1;
	
	protected ButtonWidget 				m_Back;
	protected ImageWidget 				m_ControlsLayoutImage;
	
	//-------------
	protected CanvasWidget				m_CanvasWidget;
	protected TabberUI					m_TabScript;
	protected Widget					m_TabberWidget;
	protected Widget 					m_ControlsImage;
	protected Widget 					m_PlatformHolder; //controls container for selected platform
	protected Widget 					m_VariantWidget;
	
	protected ref map<int,Widget> 					m_ImageMarkerStructure;
	protected ref map<int,Widget> 					m_CategoryStructure;
	protected ref map<int,ref TButtonPairingInfo> 	m_AreasLR; //left/right area holders
	
	protected const int 				AREA_LEFT = 1;
	protected const int 				AREA_RIGHT = 2;
	protected const int 				PLATFORM_ADJUST_X1 = 1000;
	protected const int 				PLATFORM_ADJUST_PS = 2000;
	
	//============================================
	// ControlsXboxNew
	//============================================
	void ~ControlsXboxNew()
	{
		PPERequesterBank.GetRequester(PPERequesterBank.REQ_MENUEFFECTS).Stop();
	}
	
	protected void OnInputPresetChanged()
	{
		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		#endif
	}
	
	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		switch (pInputDeviceType)
		{
		case EInputDeviceType.CONTROLLER:
			UpdateControlsElements();
			layoutRoot.FindAnyWidget("toolbar_bg").Show(true);
		break;

		default:
			if (GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
			{
				layoutRoot.FindAnyWidget("toolbar_bg").Show(false);
			}
		break;
		}
	}
	
	void Back()
	{
		GetGame().GetUIManager().Back();
	}
	
	void UpdateTabContent( int tab_index )
	{
		Widget w;
		//hide old
		if (m_CurrentTabIdx != -1)
		{
			m_VariantWidget.Show(false);
			while (m_VariantWidget.GetParent())
			{
				m_VariantWidget = m_VariantWidget.GetParent();
				m_VariantWidget.Show(false);
			}
		}
		//show new
		w = FindChildByID(m_CategoryStructure[tab_index],InputUtils.GetConsolePresetID());
		w.Show(true);
		m_VariantWidget = w;
		
		while (w.GetParent())
		{
			w = w.GetParent();
			w.Show(true);
		}
		
		DrawConnectingLines(tab_index);
		m_CurrentTabIdx = tab_index;
	}
	
	protected void DrawConnectingLines(int index)
	{
		//disconnected for now, to be finished
		return;
		
		m_CanvasWidget.Clear();
		
		//TODO drawing over nyah
		m_VariantWidget;
		m_AreasLR = new map<int,ref TButtonPairingInfo>;
		
		Widget wid_side; //left or right area
		Widget wid_spacer; //item in the L/R areas
		wid_side = m_VariantWidget.GetChildren();
		typename t = EConsoleButtonsControls;
		int side_idx;
		int enum_value;
		array<Widget> items_raw;
		array<Widget> items_filtered;
		
		while (wid_side)
		{
			TButtonPairingInfo button_mapping = new TButtonPairingInfo;
			
			side_idx = wid_side.GetUserID();
			wid_spacer = wid_side.GetChildren(); //dig into the side first..
			
			for (int i = 1; i < EnumTools.GetEnumSize(EConsoleButtonsControls); i++)
			{
				items_raw = new array<Widget>;
				items_filtered = new array<Widget>;
				t.GetVariableValue(null, i, enum_value); //TODO
				
				FindAllChildrenByID(wid_spacer,enum_value,items_raw);
				if (FilterByVisible(items_raw,items_filtered) > 0) //if there are any button-relevant items..
				{
					button_mapping.Insert(enum_value,items_filtered);
					
					//dump it!
/*
					for (int dump = 0; dump < items_filtered.Count(); dump++)
					{
						Print("key: " + i + " | name: " + items_filtered[dump].GetName());
					}
					Print("-");
*/
				}
			}
			m_AreasLR.Insert(side_idx,button_mapping);
			
			wid_side = wid_side.GetSibling();
		}
		
		//dump it 2: the dumpening!
/*
		int count_2 = m_AreasLR.Count();
		for (int z = 0; z < m_AreasLR.Count(); z++)
		{
			Print("---------");
			Print("Area: " + z);
			TButtonPairingInfo tmp_info = m_AreasLR.GetElement(z);
			int count_1 = tmp_info.Count();
			for (int zz = 0; zz < tmp_info.Count(); zz++)
			{
				for (int zzz = 0; zzz < tmp_info.GetElement(zz).Count(); zzz++)
				{
					Print("key: " + tmp_info.GetKey(zz) + " | name: " + tmp_info.GetElement(zz).Get(zzz).GetName());
				}
			}
		}
		
		Print("------------------------");
*/
		/*
		map<string, ref array<int>> button_marker_groups_unflitred = new map<string, ref array<int>>;
		map<string, ref array<int>> button_marker_groups = new map<string, ref array<int>>;
		
		float text_widget_pos_x, text_widget_pos_y;
		float text_widget_width, text_widget_height;
		float dot_pos_x, dot_pos_y;
		float dot_width, dot_height;
		float draw_pos_x, draw_pos_y;
		
		m_CanvasWidget.Clear();
		
		for (int i = 0; i < m_TabScript.GetTabCount(); i++)
		{
			tab_array.Insert(new array<ref JsonControlMappingInfo>);
			for (int j = 0; j < 20; j++)
			{
				tab_array[i].Insert(null);
			}
		}
		
		// insert json info to array by index, so it is sorted
		for (i = 0; i < control_mapping_info.Count(); i++)
		{
			JsonControlMappingInfo info = control_mapping_info.Get(i);
			tab_array[info.m_TabID][info.m_TextWidgetID] = info;
		}
		
		// create group of buttons which are connected together with line
		for (int l = 0; l < control_mapping_info.Count(); l++)
		{
			JsonControlMappingInfo info1 = control_mapping_info[l];
			string button_name = info1.m_ButtonName;
			int text_widget_id = info1.m_TextWidgetID;
			if (info1.m_TabID != index)
			{
				continue;
			}
			if (!button_marker_groups_unflitred.Contains(button_name))
			{
				button_marker_groups_unflitred.Insert(button_name, new ref array<int>);
				button_marker_groups_unflitred.Get(button_name).Insert(text_widget_id);
			}
			else
			{
				button_marker_groups_unflitred.Get(button_name).Insert(text_widget_id);
			}
		}
		
		// we want groups which are bigger than 1
		for (l = 0; l < button_marker_groups_unflitred.Count(); l++)
		{
			if (button_marker_groups_unflitred.GetElement(l).Count() > 1)
			{
				string key = button_marker_groups_unflitred.GetKey(l);
				button_marker_groups.Insert(button_marker_groups_unflitred.GetKey(l), button_marker_groups_unflitred.Get(key));
			}
		}

		Widget panel_widget;
		Widget button_marker_widget;
		
		for (l = 0; l < tab_array[index].Count(); l++)
		{
			panel_widget = layoutRoot.FindAnyWidget("PanelWidget" + l);
			if (tab_array[index][l] != null)
			{
				TextWidget text_widget = TextWidget.Cast(panel_widget.FindAnyWidget("TextWidget" + l));

				string key_prefix;
				#ifdef PLATFORM_XBOX
				key_prefix = "xb_button_marker_";
				#else
				#ifdef PLATFORM_PS4
				key_prefix = "ps_button_marker_";
				#endif
				#endif

				button_marker_widget = layoutRoot.FindAnyWidget(key_prefix + tab_array[index][l].m_ButtonName);
				text_widget.SetText(tab_array[index][l].m_InfoText);
				panel_widget.Show(true);
				button_marker_widget.Show(true);
				panel_widget.Update();
				if (!button_marker_groups.Contains(tab_array[index][l].m_ButtonName))
				{
					panel_widget.GetScreenPos(text_widget_pos_x, text_widget_pos_y);
					panel_widget.GetScreenSize(text_widget_width,text_widget_height);
					
					button_marker_widget.GetScreenPos(dot_pos_x, dot_pos_y);
					button_marker_widget.GetScreenSize(dot_width, dot_height);
					
					draw_pos_y = text_widget_pos_y + text_widget_height / 2;
					if (l < 10)
					{
						draw_pos_x = text_widget_pos_x + text_widget_width - 1;
					}
					else
					{
						draw_pos_x = text_widget_pos_x;
					}
					m_CanvasWidget.DrawLine(draw_pos_x, draw_pos_y, dot_pos_x+dot_width/2, draw_pos_y, 2, ARGBF(0.6, 1, 1, 1));
					m_CanvasWidget.DrawLine(dot_pos_x+dot_width/2, draw_pos_y, dot_pos_x+dot_width/2, dot_pos_y+dot_height/2, 2, ARGBF(0.6, 1, 1, 1));	
				}
			}
			else
			{
				panel_widget.Show(false);
			}
			panel_widget.Update();
		}
		
		// draw connecting lines 
		for (l = 0; l <  button_marker_groups.Count(); l++)
		{
			text_widget_pos_x = 0;
			text_widget_pos_y = 0;
			text_widget_width = 0;
			text_widget_height = 0;
			float group_point_x = 0, group_point_y = 0;
			float first_x = 0, first_y = 0;
			
			ref array<int> element = button_marker_groups.GetElement(l);
			string key_name = button_marker_groups.GetKey(l);
			#ifdef PLATFORM_XBOX
			key_prefix = "xb_button_marker_";
			#else
			#ifdef PLATFORM_PS4
			key_prefix = "ps_button_marker_";
			#endif
			#endif
			button_marker_widget = layoutRoot.FindAnyWidget(key_prefix + key_name);
			
			for (int g = 0; g < element.Count(); g++)
			{
				panel_widget = layoutRoot.FindAnyWidget("PanelWidget" + element[g]);
				
				panel_widget.GetScreenPos(text_widget_pos_x, text_widget_pos_y);
				panel_widget.GetScreenSize(text_widget_width, text_widget_height);
				
				if (g == 0)
				{
					if (element[0] < 10)
					{
						first_x = text_widget_pos_x + text_widget_width +50;
					}
					else
					{
						first_x = text_widget_pos_x - 50;
					}

					first_y = text_widget_pos_y + text_widget_height/2;
				}

				group_point_x += text_widget_pos_x;
				group_point_y += text_widget_pos_y;
				if (element[0] < 10)
				{
					m_CanvasWidget.DrawLine(text_widget_pos_x + text_widget_width - 1, text_widget_pos_y + text_widget_height/2, text_widget_pos_x + text_widget_width +50, text_widget_pos_y + text_widget_height/2, 2, ARGBF(0.6, 1, 1, 1));
				}
				else
				{
					m_CanvasWidget.DrawLine(text_widget_pos_x, text_widget_pos_y + text_widget_height/2, text_widget_pos_x - 50, text_widget_pos_y + text_widget_height/2, 2, ARGBF(0.6, 1, 1, 1));
				}
			}
			if (element[0] < 10)
			{
				group_point_x = group_point_x/element.Count() + text_widget_width + 50;
			}
			else
			{
				group_point_x = group_point_x/element.Count() - 50;
			}

			if (element.Count() % 2 == 0)
			{
				group_point_y = ( ( text_widget_pos_y + text_widget_height/2 ) - first_y ) / 2 + first_y;
			}
			else
			{
				float text_widget_pos_x_center, text_widget_pos_y_center;
				float text_widget_width_center, text_widget_height_center;
				
				panel_widget = layoutRoot.FindAnyWidget("PanelWidget" + element[1]);
				
				panel_widget.GetScreenPos(text_widget_pos_x_center, text_widget_pos_y_center);
				panel_widget.GetScreenSize(text_widget_width_center, text_widget_height_center);
				
				group_point_y = text_widget_pos_y_center + text_widget_height_center / 2;
			}
			
			button_marker_widget.GetScreenPos(dot_pos_x, dot_pos_y);
			button_marker_widget.GetScreenSize(dot_width, dot_height);
			
			m_CanvasWidget.DrawLine(group_point_x, group_point_y, dot_pos_x+dot_width/2, group_point_y, 2, ARGBF(0.6, 1, 1, 1));
			m_CanvasWidget.DrawLine(dot_pos_x+dot_width/2, group_point_y, dot_pos_x+dot_width/2, dot_pos_y, 2, ARGBF(0.6, 1, 1, 1));
			
			if (element[0] < 10)
			{
				m_CanvasWidget.DrawLine(first_x, first_y, text_widget_pos_x + text_widget_width +50, text_widget_pos_y + text_widget_height/2, 2, ARGBF(0.6, 1, 1, 1));
			}
			else
			{
				m_CanvasWidget.DrawLine(first_x, first_y, text_widget_pos_x - 50, text_widget_pos_y + text_widget_height/2, 2, ARGBF(0.6, 1, 1, 1));
			}
		}
		*/
	}
	
	//============================================
	// Init
	//============================================
	override Widget Init()
	{
		m_CategoryStructure = new map<int,Widget>;
		m_ImageMarkerStructure = new map<int,Widget>;
		
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/xbox/Controls_Screen.layout");
		#ifdef PLATFORM_XBOX
			m_ControlsImage = layoutRoot.FindAnyWidget("XboxControlsImage");
		#else
		#ifdef PLATFORM_PS4
			m_ControlsImage = layoutRoot.FindAnyWidget("PSControlsImage");
		#endif
		#endif
		m_ControlsImage.Show(true);
		m_TabberWidget = layoutRoot.FindAnyWidget("Tabber");
		m_TabberWidget.GetScript(m_TabScript);
		m_TabScript.m_OnTabSwitch.Insert(UpdateTabContent);
		m_CanvasWidget = CanvasWidget.Cast( layoutRoot.FindAnyWidget("CanvasUniversal") );
		m_Back = ButtonWidget.Cast(layoutRoot.FindAnyWidget("back"));
		
		UpdateControlsElements();
		
		PPERequester_MenuEffects requester;
		Class.CastTo(requester,PPERequesterBank.GetRequester(PPERequesterBank.REQ_MENUEFFECTS));
		requester.SetVignetteIntensity(0.6);
		
		ComposeData();
		UpdateTabContent(0);
		
		GetGame().GetMission().GetOnInputPresetChanged().Insert(OnInputPresetChanged);
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);
		
		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());

		
		return layoutRoot;
	}
	
	override void OnShow()
	{
		super.OnShow();
		
		UpdateToolbarText();
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button == MouseState.LEFT)
		{
			if (w == m_Back)
			{
				Back();
				return true;
			}
		}
		return false;
	}
	
	override void Update(float timeslice)
	{
		if (GetGame().GetInput().LocalPress("UAUITabLeft", false))
		{
			m_TabScript.PreviousTab();
		}
		
		if (GetGame().GetInput().LocalPress("UAUITabRight", false))
		{
			m_TabScript.NextTab();
		}
		
		if (GetGame().GetInput().LocalPress("UAUIBack", false))
		{
			Back();
		}
		
		if (GetGame().GetInput().LocalPress("UASwitchPreset",false))
		{
			PerformSwitchPreset();
			UpdateTabContent(m_CurrentTabIdx);
			m_TabScript.RefreshTab();
		}
	}
	
	//! Inits data structure
	protected void ComposeData()
	{
		if (m_CategoryStructure)
		{
			m_CategoryStructure = null;
			m_CategoryStructure = new map<int,Widget>;
		}
		
		if (m_ImageMarkerStructure)
		{
			m_ImageMarkerStructure = null;
			m_ImageMarkerStructure = new map<int,Widget>;
		}
		
		Widget w = m_ControlsImage.GetChildren();
		m_ImageMarkerStructure.Set(w.GetUserID(),w);
		
		while (w.GetSibling())
		{
			w = w.GetSibling();
			m_ImageMarkerStructure.Set(w.GetUserID(),w); //inits button markers with their IDs
		}
		
		InputUtils.UpdateConsolePresetID();
		
		#ifdef PLATFORM_XBOX
			m_PlatformHolder = FindChildByID(layoutRoot,PLATFORM_ADJUST_X1);
		#else
			m_PlatformHolder = FindChildByID(layoutRoot,PLATFORM_ADJUST_PS);
		#endif
		
		//categories
		Widget category_widget = m_PlatformHolder.GetChildren();
		m_CategoryStructure.Set(category_widget.GetUserID(),category_widget);
		
		while (category_widget.GetSibling())
		{
			category_widget = category_widget.GetSibling();
			m_CategoryStructure.Set(category_widget.GetUserID(),category_widget);
		}
	}
	
	protected void PerformSwitchPreset()
	{
		int index;
		string preset_text;
		UAInputAPI inputAPI = GetUApi();
		
		index = inputAPI.PresetCurrent() + 1;
		if (index >= inputAPI.PresetCount())
		{
			index = 0;
		}
		
		inputAPI.PresetSelect(index);
		UpdateToolbarText();
		
		GetUApi().Export();
		GetUApi().SaveInputPresetMiscData();
		
		GetGame().GetMission().GetOnInputPresetChanged().Invoke();
		
		InputUtils.UpdateConsolePresetID();
	}
	
	protected void UpdateToolbarText()
	{
		//changer text
		string changer_text;
		UAInputAPI inputAPI = GetUApi();
		TextWidget changer_widget;
		if ( Class.CastTo(changer_widget,layoutRoot.FindAnyWidget("ChangePresetText")) )
		{
			int target_idx = inputAPI.PresetCurrent() + 1;
			int count = inputAPI.PresetCount();
			if (target_idx >= inputAPI.PresetCount())
			{
				target_idx = 0;
			}
			
			changer_text = inputAPI.PresetName(target_idx);
			if (changer_text == InputUtils.PRESET_OLD)
			{
				changer_text = "#STR_UAPRESET_ChangeTo_0";
			}
			else if (changer_text == InputUtils.PRESET_NEW)
			{
				changer_text = "#STR_UAPRESET_ChangeTo_1";
			}
			else
			{
				changer_text = "Invalid console preset name: " + changer_text;
			}
			
			changer_widget.SetText(changer_text);
		}
	}
	
	//!Finds immediate child widget with a corresponding userID
	protected Widget FindChildByID(Widget wid, int user_id)
	{
		Widget ret = wid.GetChildren();
		while (ret)
		{
			if (ret.GetUserID() == user_id)
			{
				return ret;
			}
			ret = ret.GetSibling();
		}
		return ret;
	}
	
	//!Finds all immediate children widgets with corresponding userIDs
	protected bool FindAllChildrenByID(Widget wid, int user_id, out array<Widget> results)
	{
		Widget child = wid.GetChildren();
		while (child)
		{
			if (child.GetUserID() == user_id)
			{
				results.Insert(child);
			}
			child = child.GetSibling();
		}
		return (results && results.Count() > 0);
	}
	
	//! returns count
	protected int FilterByVisible(array<Widget> input, array<Widget> filtered)
	{
		for (int i = 0; i < input.Count(); i++)
		{
			if (input[i].IsVisible())
			{
				filtered.Insert(input[i]);
			}
		}
		
		return filtered.Count();
	}

	protected void UpdateControlsElements()
	{
		RichTextWidget toolbar_switch	= RichTextWidget.Cast(layoutRoot.FindAnyWidget("ChangePresetIcon"));
		RichTextWidget toolbar_back		= RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		toolbar_switch.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UASwitchPreset", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		toolbar_back.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
	}
}
