//! Deprecated
class KeybindingsGroup extends ScriptedWidgetEventHandler
{
	protected Widget									m_Root;
	protected KeybindingsMenu							m_Menu;
	
	protected ref map<int, ref KeybindingElement>		m_KeyWidgets;
	protected int										m_CurrentSettingKeyIndex = -1;
	protected int										m_CurrentSettingAlternateKeyIndex = -1;
	
	protected ref DropdownPrefab						m_KBDropdown;
	
	void KeybindingsGroup( int index, Input input, Widget parent, KeybindingsMenu menu )
	{
		m_KeyWidgets	= new map<int, ref KeybindingElement>;
		m_Menu			= menu;
		
		string group_name;
		input.GetActionGroupName( index, group_name );
		
		m_Root	= GetGame().GetWorkspace().CreateWidgets( GetLayoutName(), parent );
		Widget subgroup	= m_Root.FindAnyWidget( "group_content" );
		
//		for( int i = 0; i < 1; i++ )
//		{
			AddSubgroup( /*index, */subgroup, input );
//		}
		
		InitPresets( index, m_Root.FindAnyWidget( "group_header" ), input );
		
		subgroup.Update();
		
		m_Root.SetHandler( this );
	}
	
	string GetLayoutName()
	{
		return "gui/layouts/new_ui/options/keybindings_selectors/keybinding_group.layout";
	}
	
	void InitPresets( int index, Widget parent, Input input )
	{
		Widget kb_root = parent.FindAnyWidget( "keyboard_dropown" );
		
		string profile_text;
		input.GetProfileName( input.GetCurrentProfile(), profile_text );
		
		m_KBDropdown		= new DropdownPrefab( kb_root, profile_text );
		
		m_KBDropdown.m_OnSelectItem.Insert( OnSelectKBPreset );
		
		for( int i = 0; i < input.GetProfilesCount(); i++ )
		{
			input.GetProfileName( i, profile_text );
			m_KBDropdown.AddElement( profile_text );
		}
		
		kb_root.Update();
	}
	
	void OnSelectKBPreset( int index )
	{
		string profile_text;
		GetGame().GetInput().GetProfileName( index, profile_text );
		m_KBDropdown.SetText( profile_text );
		GetGame().GetInput().SetProfile( index );
		ReloadProfiles();
		m_KBDropdown.Close();
	}
	
	void OnSelectConsolePreset( int index )
	{
		string profile_text;
		GetGame().GetInput().GetProfileName( index, profile_text );
		GetGame().GetInput().SetProfile( index );
		ReloadProfiles();
	}
	
	void ReloadProfiles()
	{
		foreach( int index,  KeybindingElement element : m_KeyWidgets )
		{
			element.Reload();
		}
	}
	
	void AddSubgroup( /*int index, */Widget parent, Input input )
	{
		Widget subgroup				= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/options/keybindings_selectors/keybinding_subgroup.layout", parent );
		TextWidget subgroup_name	= TextWidget.Cast( subgroup.FindAnyWidget( "subgroup_text" ) );
		
		subgroup_name.SetText( "TestSubgroup" );
		Widget subgroup_content = subgroup.FindAnyWidget( "subgroup_content" );
		
		TIntArray actions = new TIntArray;
		GetUApi().GetActiveInputs(actions);
		for( int i = 0; i < actions.Count(); i++ )
		{
			AddAction( actions.Get( i ), subgroup_content, input );
		}
		
		subgroup_content.Update();
	}
	
	void AddAction( int index, Widget parent, Input input )
	{
		m_KeyWidgets.Insert( index, new KeybindingElement( index, parent, this ) );
	}
	
	void ReloadAction( int index )
	{
		if( m_KeyWidgets.Contains( index ) )
		{
			m_KeyWidgets.Get( index ).Reload();
		}
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
		if( m_CurrentSettingKeyIndex != -1 )
		{
			m_KeyWidgets.Get( m_CurrentSettingKeyIndex ).CancelEnteringKeybind();
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
		if( m_CurrentSettingAlternateKeyIndex != -1 )
		{
			m_KeyWidgets.Get( m_CurrentSettingAlternateKeyIndex ).CancelEnteringAlternateKeybind();
			m_CurrentSettingAlternateKeyIndex = -1;
		}
	}
	
	bool IsChanged()
	{
		foreach( int index, KeybindingElement element : m_KeyWidgets )
		{
			if( element.IsChanged() || element.IsAlternateChanged() )
			{
				return true;
			}
		}
		return false;
	}
	
	void Apply()
	{
		UAInputAPI ua_api	= GetUApi();
		foreach( int index, KeybindingElement element : m_KeyWidgets )
		{
			UAInput input = ua_api.GetInputByID( index );
			int i;
			if( element.IsChanged() )
			{
				array<int> new_keys		= element.GetChangedBinds();
				
/*				if( input.AlternativeCount() == 0 )
				{
					input.AddAlternative();
				}
				else*/
				{
					input.ClearDeviceBind(EUAINPUT_DEVICE_KEYBOARDMOUSE);
//					input.SelectAlternative( 0 );
//					input.ClearAlternative( 0 );
				}
				
				if( new_keys.Count() > 0 )
				{
					input.BindComboByHash( new_keys.Get( 0 ) );
					for( i = 1; i < new_keys.Count(); i++ )
					{
						input.BindComboByHash( new_keys.Get( i ) );
					}
				}
			}
			
			if( element.IsAlternateChanged() )
			{
				array<int> new_alt_keys = element.GetChangedAlternateBinds();
				
				if( input.AlternativeCount() == 0 )
				{
					input.AddAlternative();				
				}
				
/*				if( input.AlternativeCount() < 2 )
				{
					input.AddAlternative();
				}
				else*/
				{
					input.ClearDeviceBind(EUAINPUT_DEVICE_CONTROLLER);
//					input.SelectAlternative( 1 );
//					input.ClearAlternative( 1 );
				}
				
				if( new_alt_keys.Count() > 0 )
				{
					input.BindComboByHash( new_alt_keys.Get( 0 ) );
					for( i = 1; i < new_alt_keys.Count(); i++ )
					{
						input.BindComboByHash( new_alt_keys.Get( i ) );
					}
				}
			}
			element.Reload();
		}
	}
	
	//DEPRECATED
	void Reset()
	{
		ResetEx();
	}
	
	void ResetEx(bool forced = false)
	{
		foreach( int index, KeybindingElement element : m_KeyWidgets )
		{
			if( element.IsChanged() || element.IsAlternateChanged() || forced )
			{
				element.Reload();
			}
		}
	}
	
	void Update( float timeslice )
	{
		if( m_CurrentSettingKeyIndex != -1 || m_CurrentSettingAlternateKeyIndex != -1 )
		{
			UAInputAPI ua_api = GetUApi();
			if( ua_api.DeterminePressedButton() != 0 )
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
				
				if( m_CurrentSettingKeyIndex != -1 )
				{
					m_Menu.ConfirmKeybindEntry( new_keybinds );
					m_KeyWidgets.Get( m_CurrentSettingKeyIndex ).Reload( new_keybinds, false );
					m_CurrentSettingKeyIndex = -1;
				}
				else if( m_CurrentSettingAlternateKeyIndex != -1 )
				{
					m_Menu.ConfirmAlternateKeybindEntry( new_keybinds );
					m_KeyWidgets.Get( m_CurrentSettingAlternateKeyIndex ).Reload( new_keybinds, true );
					m_CurrentSettingAlternateKeyIndex = -1;
				}
			}
		}
	}
	
	override bool OnMouseButtonDown( Widget w, int x, int y, int button )
	{
		if( !ButtonWidget.Cast( w ) )
		{
			m_KBDropdown.Close();
		}
		return false;
	}
}