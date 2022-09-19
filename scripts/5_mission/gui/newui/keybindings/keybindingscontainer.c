//container for various subgroups and their elements
class KeybindingsContainer extends ScriptedWidgetEventHandler
{
	protected const int  								NO_SORTING = -1;
	protected Widget									m_Root;
	protected KeybindingsMenu							m_Menu;
	protected ScrollWidget 								m_Scroll;
	
	protected ref map<int, ref ElementArray>			m_KeyWidgetElements; //<input_action_id,<KeybindingElementNew>>
	protected int										m_CurrentSettingKeyIndex = -1;
	protected int										m_CurrentSettingAlternateKeyIndex = -1;
	
	protected ref array<Widget> 						m_Subgroups;
	
	void KeybindingsContainer( int index, Input input, Widget parent, KeybindingsMenu menu )
	{
		m_KeyWidgetElements	= new map<int, ref ElementArray>;
		m_Menu			= menu;
		
		m_Root	= GetGame().GetWorkspace().CreateWidgets( GetLayoutName(), parent );
		m_Scroll = ScrollWidget.Cast(m_Root.FindAnyWidget("group_scroll"));
		Widget container	= m_Root.FindAnyWidget( "group_scroll" );
		
		CreateSubgroups(container,input);
		
		container.Update();
		
		m_Root.SetHandler( this );
	}
	
	string GetLayoutName()
	{
		return "gui/layouts/new_ui/options/keybindings_selectors/keybinding_container.layout";
	}
	
	void OnSelectKBPreset( int index )
	{
		GetUApi().PresetSelect( index );
		ReloadElements();
		GetUApi().Export();
		GetUApi().SaveInputPresetMiscData();
	}
	
	void ReloadElements()
	{
		foreach ( ElementArray elements : m_KeyWidgetElements )
		{
			foreach (KeybindingElementNew element : elements)
			{
				element.Reload();
			}
		}
	}
	
	void AddSubgroup( int sort_index, Widget parent, Input input )
	{
		Widget subgroup				= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/options/keybindings_selectors/keybinding_subgroup.layout", parent );
		Widget subgroup_content 	= subgroup.FindAnyWidget( "subgroup_content" );
		
		TIntArray input_actions;
		if (sort_index != NO_SORTING)
		{
			input_actions = InputUtils.GetInputActionSortingMap().Get(sort_index);
		}
		else
		{
			input_actions = InputUtils.GetUnsortedInputActions();
		}
		
		for (int i = 0; i < input_actions.Count(); i++)
		{
			AddElement( input_actions[i], subgroup_content, input );
		}
		
		m_Subgroups.Insert(subgroup);
		
		subgroup_content.Update();
	}
	
	void CreateSubgroups( Widget parent, Input input )
	{
		m_Subgroups = new array<Widget>;
		
		int sort_count = InputUtils.GetInputActionSortingMap().Count();
		for (int i = 0; i < sort_count; i++)
		{
			if (InputUtils.GetInputActionSortingMap().GetElement(i) && InputUtils.GetInputActionSortingMap().GetElement(i).Count() > 0)
			{
				AddSubgroup(InputUtils.GetInputActionSortingMap().GetKey(i),parent,input);
			}
		}
		
		//any unssorted inputs go here
		if (InputUtils.GetUnsortedInputActions() && InputUtils.GetUnsortedInputActions().Count() > 0)
		{
			AddSubgroup(NO_SORTING,parent,input);
		}
	}
	
	void AddElement( int index, Widget parent, Input input )
	{
		//create array if needed
		if (!m_KeyWidgetElements.Get(index))
		{
			ElementArray elements = new ElementArray;
			m_KeyWidgetElements.Insert( index, elements );
		}
		
		KeybindingElementNew element = new KeybindingElementNew( index, parent, this );
		m_KeyWidgetElements.Get(index).Insert(element);
	}
	
	bool IsEnteringKeyBind()
	{
		return ( m_CurrentSettingKeyIndex != -1 || m_CurrentSettingAlternateKeyIndex != -1 );
	}
	
	void ClearKeybind( int key_index )
	{
		m_Menu.ClearKeybind( key_index );
	}
	
	void ClearAlternativeKeybind( int key_index )
	{
		m_Menu.ClearAlternativeKeybind( key_index );
	}
	
	void StartEnteringKeybind( int key_index )
	{
		m_CurrentSettingAlternateKeyIndex	= -1;
		m_CurrentSettingKeyIndex			= key_index;
		m_Menu.StartEnteringKeybind( key_index );
	}
	
	void CancelEnteringKeybind()
	{
		if ( m_CurrentSettingKeyIndex != -1 )
		{
			foreach (KeybindingElementNew element : m_KeyWidgetElements.Get( m_CurrentSettingKeyIndex ))
			{
				element.CancelEnteringKeybind();
			}
			m_CurrentSettingKeyIndex = -1;
		}
	}
	
	void StartEnteringAlternateKeybind( int key_index )
	{
		m_CurrentSettingKeyIndex			= -1;
		m_CurrentSettingAlternateKeyIndex	= key_index;
		m_Menu.StartEnteringAlternateKeybind( key_index );
	}
	
	void CancelEnteringAlternateKeybind()
	{
		if ( m_CurrentSettingAlternateKeyIndex != -1 )
		{
			foreach (KeybindingElementNew element : m_KeyWidgetElements.Get( m_CurrentSettingAlternateKeyIndex ))
			{
				element.CancelEnteringKeybind();
			}
			m_CurrentSettingAlternateKeyIndex = -1;
		}
	}
	
	//! is anything changed?
	bool IsChanged()
	{
		foreach (ElementArray elements : m_KeyWidgetElements)
		{
			foreach (KeybindingElementNew element : elements)
			{
				if (element.IsChanged() || element.IsAlternateChanged())
				{
					return true;
				}
			}
		}
		return false;
	}
	
	void Apply()
	{
		UAInputAPI ua_api	= GetUApi();
		int input_index = 0;
		
		foreach (ElementArray elements : m_KeyWidgetElements)
		{
			foreach (int element_index, KeybindingElementNew element : elements)
			{
				if (element_index == 0)
				{
					UAInput input = ua_api.GetInputByID( m_KeyWidgetElements.GetKey(input_index) );
					int i;
					if ( element.IsChanged() )
					{
						array<int> new_keys		= element.GetChangedBinds();
						input.ClearDeviceBind(EUAINPUT_DEVICE_KEYBOARDMOUSE);
						if ( new_keys.Count() > 0 )
						{
							input.BindComboByHash( new_keys.Get( 0 ) );
							for( i = 1; i < new_keys.Count(); i++ )
							{
								input.BindComboByHash( new_keys.Get( i ) );
							}
						}
					}
					
					if ( element.IsAlternateChanged() )
					{
						array<int> new_alt_keys = element.GetChangedAlternateBinds();
						
						if ( input.AlternativeCount() == 0 )
						{
							input.AddAlternative();				
						}
						
						input.ClearDeviceBind(EUAINPUT_DEVICE_CONTROLLER);
						
						if ( new_alt_keys.Count() > 0 )
						{
							input.BindComboByHash( new_alt_keys.Get( 0 ) );
							for( i = 1; i < new_alt_keys.Count(); i++ )
							{
								input.BindComboByHash( new_alt_keys.Get( i ) );
							}
						}
					}
				}
				element.Reload();
			}
			input_index++;
		}
	}
	
	void Reset(bool forced = false)
	{
		foreach ( ElementArray elements : m_KeyWidgetElements )
		{
			foreach (KeybindingElementNew element : elements)
			{
				if ( element.IsChanged() || element.IsAlternateChanged() || forced )
				{
					element.Reload();
				}
			}
		}
	}
	
	void Update( float timeslice )
	{
		if ( m_CurrentSettingKeyIndex != -1 || m_CurrentSettingAlternateKeyIndex != -1 )
		{
			UAInputAPI ua_api = GetUApi();
			if ( ua_api.DeterminePressedButton() != 0 )
			{
				string name;
				string text;
				ref array<int> new_keybinds = new array<int>;
				
				// remove previous backlit
				GetDayZGame().GetBacklit().KeybindingClear();

				for( int i = 0; i < ua_api.DeterminedCount(); ++i )
				{
					int kb_id = ua_api.GetDetermined( i );
					new_keybinds.Insert( kb_id );
					
					// light up specific key
					GetDayZGame().GetBacklit().KeybindingShow(kb_id);
				}
				
				if ( m_CurrentSettingKeyIndex != -1 )
				{
					m_Menu.ConfirmKeybindEntry( new_keybinds );
					foreach (KeybindingElementNew element : m_KeyWidgetElements.Get( m_CurrentSettingKeyIndex ))
					{
						element.Reload( new_keybinds, false );
					}
					m_CurrentSettingKeyIndex = -1;
				}
				else if ( m_CurrentSettingAlternateKeyIndex != -1 )
				{
					m_Menu.ConfirmAlternateKeybindEntry( new_keybinds );
					foreach (KeybindingElementNew elementAlternate : m_KeyWidgetElements.Get( m_CurrentSettingAlternateKeyIndex ))
					{
						elementAlternate.Reload( new_keybinds, true );
					}
					m_CurrentSettingAlternateKeyIndex = -1;
				}
			}
		}
	}
	
	void SwitchSubgroup(int index)
	{
		Widget w;
		for (int i = 0; i < m_Subgroups.Count(); i++)
		{
			w = m_Subgroups[i];
			w.Show(index == i);
		}
		
		m_Scroll.VScrollToPos01(0);
	}
}

typedef array<ref KeybindingElementNew> ElementArray;