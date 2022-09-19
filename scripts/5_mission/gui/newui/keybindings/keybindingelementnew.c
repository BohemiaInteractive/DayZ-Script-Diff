class KeybindingElementNew extends ScriptedWidgetEventHandler
{
	protected KeybindingsContainer			m_Container;
	
	protected Widget						m_Root;
	protected TextWidget					m_ElementName;
	protected TextWidget					m_ElementModifier;
	protected ButtonWidget					m_PrimaryBindButton;
	protected ButtonWidget					m_AlternativeBindButton;
	protected Widget						m_PrimaryClear;
	protected Widget						m_AlternativeClear;
	
	protected int							m_ElementIndex;
	protected bool							m_IsEdited;
	protected bool							m_IsAlternateEdited;
	
	protected ref array<int>				m_CustomBind;
	protected ref array<int>				m_CustomAlternateBind;
	
	protected ref Timer						m_EntryTimer = new Timer( CALL_CATEGORY_GUI );
	
	void KeybindingElementNew( int key_index, Widget parent, KeybindingsContainer group )
	{
		m_Root					= GetGame().GetWorkspace().CreateWidgets( GetLayoutName(), parent );
		m_ElementName			= TextWidget.Cast( m_Root.FindAnyWidget( "setting_label" ) );
		m_ElementModifier		= TextWidget.Cast( m_Root.FindAnyWidget( "modifier_label" ) );
		m_PrimaryBindButton		= ButtonWidget.Cast( m_Root.FindAnyWidget( "primary_bind" ) );
		m_AlternativeBindButton	= ButtonWidget.Cast( m_Root.FindAnyWidget( "alternative_bind" ) );
		m_PrimaryClear			= m_Root.FindAnyWidget( "primary_clear" );
		m_AlternativeClear		= m_Root.FindAnyWidget( "alternative_clear" );
		
		m_Container					= group;
		m_ElementIndex			= key_index;
		
		Reload();
		m_Root.SetHandler( this );
	}
	
	string GetLayoutName()
	{
		return "gui/layouts/new_ui/options/keybindings_selectors/keybinding_option.layout";
	}
	
	bool IsChanged()
	{
		return m_IsEdited;
	}
	
	bool IsAlternateChanged()
	{
		return m_IsAlternateEdited;
	}
	
	array<int> GetChangedBinds()
	{
		return m_CustomBind;
	}
	
	array<int> GetChangedAlternateBinds()
	{
		return m_CustomAlternateBind;
	}
	
	// assembly all related binding at widget element
	void SetElementTitle( ButtonWidget btnWidget, UAInput pInput, int iDeviceFlags )
	{
		string output;
		int a, i, countbind = 0;
	
		for( a = 0; a < pInput.AlternativeCount(); a++ )
		{
			pInput.SelectAlternative(a);
			if( pInput.IsCombo() )
			{
				if( pInput.BindingCount() > 0 )
				{
					if( pInput.Binding(0) != 0 && pInput.CheckBindDevice(0,iDeviceFlags) )
					{
						if( countbind > 0 )
							output += ", ";
						
						output += GetUApi().GetButtonName( pInput.Binding(0) );
						countbind++;
						
						for( i = 1; i < pInput.BindingCount(); i++ )
						{
							if( pInput.Binding(i) != 0 )
							{
								output += " + " + GetUApi().GetButtonName( pInput.Binding(i) );
								countbind++;
							}
						}
						
					}
				}
			}
			else
			{
				if( pInput.BindingCount() > 0 )
				{
					if( pInput.Binding(0) != 0 && pInput.CheckBindDevice(0,iDeviceFlags) )
					{
						if( countbind > 0 )
							output += ", ";

						output += GetUApi().GetButtonName( pInput.Binding(0) );
						countbind++;
					}
				}
			}
		}

		// nothing may be available - we do not want "null" or "0" in string
		if( countbind > 0 )
			btnWidget.SetText(output);
		else
			btnWidget.SetText("");
	}
	
	void Reload()
	{
		UAInput input			= GetUApi().GetInputByID( m_ElementIndex );
		m_IsEdited				= false;
		m_IsAlternateEdited		= false;
		m_CustomBind			= null;
		m_CustomAlternateBind	= null;

		if( input.IsLimited() )
		{
			if( input.IsPressLimit() )
			{
				m_ElementModifier.SetText( "#keybind_press" );
			}
			if( input.IsReleaseLimit() )
			{
				m_ElementModifier.SetText( "#keybind_release" );
			}
			if( input.IsHoldLimit() )
			{
				m_ElementModifier.SetText( "#keybind_hold" );
			}
			if( input.IsHoldBeginLimit() )
			{
				m_ElementModifier.SetText( "#keybind_holdbegin" );
			}
			if( input.IsClickLimit() )
			{
				m_ElementModifier.SetText( "#keybind_click" );
			}
			if( input.IsDoubleClickLimit() )
			{
				m_ElementModifier.SetText( "#keybind_doubletap" );
			}
		}
		else
		{
			m_ElementModifier.SetText( "" );
		}
		
		string option_text;
		string name;
		
		GetGame().GetInput().GetActionDesc( m_ElementIndex, option_text );
		m_ElementName.SetText( option_text );
		
		// column #1 :: keyboard + mouse
		SetElementTitle(m_PrimaryBindButton, input, EUAINPUT_DEVICE_KEYBOARDMOUSE);

		// column #2 :: controller
		SetElementTitle(m_AlternativeBindButton, input, EUAINPUT_DEVICE_CONTROLLER);
	}
	
	void Reload( array<int> custom_binds, bool is_alternate )
	{
		string output;
		if( custom_binds.Count() > 1 )
		{
			if( custom_binds.Get( 0 ) != 0 )
				output = GetUApi().GetButtonName( custom_binds.Get( 0 ) );
			for( int i = 1; i < custom_binds.Count(); i++ )
			{
				if( custom_binds.Get( i ) != 0 )
					output += " + " + GetUApi().GetButtonName( custom_binds.Get( i ) );
			}
		}
		else if( custom_binds.Count() > 0 )
		{
			if( custom_binds.Get( 0 ) != 0 )
				output = GetUApi().GetButtonName( custom_binds.Get( 0 ) );
		}
		
		if( is_alternate )
		{
			m_CustomAlternateBind = custom_binds;
			m_IsAlternateEdited = true;
			m_AlternativeBindButton.SetText( output );
		}
		else
		{
			m_CustomBind = custom_binds;
			m_IsEdited = true;
			m_PrimaryBindButton.SetText( output );
		}
	}
	
	void StartEnteringKeybind()
	{
		m_Container.StartEnteringKeybind( m_ElementIndex );
		m_PrimaryBindButton.SetText( "#layout_keybinding_new_keybind" );
	}
	
	void CancelEnteringKeybind()
	{
		Reload();
	}
	
	void StartEnteringAlternateKeybind()
	{
		m_Container.StartEnteringAlternateKeybind( m_ElementIndex );
		m_AlternativeBindButton.SetText( "#layout_keybinding_new_keybind" );
	}
	
	void CancelEnteringAlternateKeybind()
	{
		Reload();
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		if( !m_Container.IsEnteringKeyBind() )
		{
			if( w == m_PrimaryBindButton )
			{
				m_EntryTimer.Run( 0.01, this, "StartEnteringKeybind" );
			}
			if( w == m_AlternativeBindButton )
			{
				m_EntryTimer.Run( 0.01, this, "StartEnteringAlternateKeybind" );
			}
		}
		return false;
	}
	
	override bool OnMouseButtonUp( Widget w, int x, int y, int button )
	{
		if( w == m_PrimaryClear )
		{
			m_IsEdited				= true;
			m_CustomBind			= new array<int>;
			m_PrimaryBindButton.SetText( "" );
			m_Container.ClearKeybind( m_ElementIndex );
		}
		if( w == m_AlternativeClear )
		{
			m_IsAlternateEdited		= true;
			m_CustomAlternateBind	= new array<int>;
			m_AlternativeBindButton.SetText( "" );
			m_Container.ClearAlternativeKeybind( m_ElementIndex );
		}
		return false;
	}
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if( w == m_PrimaryBindButton || w == m_PrimaryClear )
		{
			m_PrimaryBindButton.SetColor( ARGBF( 1, 1, 0, 0 ) );
			m_PrimaryClear.Show( true );
			m_PrimaryClear.Update();
			m_AlternativeClear.Show( false );
			return true;
		}
		else if( w == m_AlternativeBindButton || w == m_AlternativeClear )
		{
			m_AlternativeBindButton.SetColor( ARGBF( 1, 1, 0, 0 ) );
			m_PrimaryClear.Show( false );
			m_AlternativeClear.Show( true );
			m_AlternativeClear.Update();
			return true;
		}
		else
		{
			m_PrimaryBindButton.SetColor( ARGBF( 0, 0, 0, 0 ) );
			m_AlternativeBindButton.SetColor( ARGBF( 1, 0, 0, 0 ) );
			m_PrimaryClear.Show( false );
			m_AlternativeClear.Show( false );
		}
		return false;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		if( w == m_PrimaryClear || w == m_PrimaryBindButton )
		{
			if( enterW != m_PrimaryClear && enterW != m_PrimaryBindButton )
			{
				m_PrimaryClear.Show( false );
				m_PrimaryBindButton.SetColor( ARGBF( 1, 0, 0, 0 ) );
			}
		}
		if( w == m_AlternativeClear || w == m_AlternativeBindButton )
		{
			if( enterW != m_AlternativeClear && enterW != m_AlternativeBindButton )
			{
				m_AlternativeClear.Show( false );
				m_AlternativeBindButton.SetColor( ARGBF( 1, 0, 0, 0 ) );
			}
		}
		return false;
	}
}