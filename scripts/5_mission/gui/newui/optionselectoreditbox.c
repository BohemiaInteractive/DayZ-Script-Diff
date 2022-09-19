class OptionSelectorEditbox extends OptionSelectorBase
{
	protected EditBoxWidget			m_EditBox;
	
	void OptionSelectorEditbox( Widget parent, string value, ScriptedWidgetEventHandler parent_menu, bool disabled )
	{
		m_Root					= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/option_editbox.layout", parent );
		#ifdef PLATFORM_CONSOLE
			m_Parent				= parent.GetParent().GetParent();
		#else
		#ifdef PLATFORM_WINDOWS
			m_Parent				= parent.GetParent();
		#endif
		#endif
		
		m_SelectorType				= 1;
		m_ParentClass				= parent_menu;
		m_EditBox					= EditBoxWidget.Cast( m_Root.FindAnyWidget( "option_value" ) );
		
		SetValue( value );
		Enable();
		
		m_Parent.SetHandler( this );
	}
	
	void ~OptionSelectorEditbox()
	{
		delete m_Root;
	}
	
	override void Enable()
	{
		super.Enable();
		
		m_EditBox.ClearFlags( WidgetFlags.IGNOREPOINTER );
	}
	
	override void Disable()
	{
		super.Disable();
		
		m_EditBox.SetFlags( WidgetFlags.IGNOREPOINTER );
	}
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if( !IsFocusable( w ) )
			return true;
		
		if( m_ParentClass )
		{
			m_ParentClass.OnFocus( m_Root.GetParent(), -1, m_SelectorType );
			m_ParentClass.OnMouseEnter( m_Root.GetParent().GetParent(), x, y );
		}
		
		UIScriptedMenu menu = GetGame().GetUIManager().GetMenu();
		
		if ( menu && menu.IsInherited( CharacterCreationMenu ) )
		{
			menu.OnFocus( m_Root.GetParent(), -1, m_SelectorType );
			menu.OnMouseEnter( m_Root.GetParent().GetParent(), x, y );
		}
		
		ColorHighlight(w);
		
		return true;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		if( IsFocusable( enterW ) )
			return true;
		
		if( m_ParentClass )
		{
			m_ParentClass.OnFocus( null, x, y );
			m_ParentClass.OnMouseLeave( m_Root.GetParent().GetParent(), enterW, x, y );
		}
		
		UIScriptedMenu menu = GetGame().GetUIManager().GetMenu();
		
		if ( menu && menu.IsInherited( CharacterCreationMenu ) )
		{
			menu.OnFocus( null, x, y );
			menu.OnMouseLeave( m_Root.GetParent().GetParent(), enterW, x, y );
		}
		
		ColorNormal( w );
		
		return true;
	}
	
	override bool OnChange( Widget w, int x, int y, bool finished )
	{
		if( w == m_EditBox )
		{
			m_OptionChanged.Invoke( GetValue() );
			return true;
		}
		return false;
	}
	
	override bool IsFocusable( Widget w )
	{
		if( w )
		{
			return ( w == m_Parent || w == m_EditBox );
		}
		return false;
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		if( GetFocus() != m_EditBox )
		{
			SetFocus( m_EditBox );
			m_Parent.Enable( false );
		}
		
		return super.OnFocus( m_Parent, x, y );
	}
	
	override bool OnFocusLost( Widget w, int x, int y )
	{
		if( w == m_EditBox )
		{
			m_Parent.Enable( true );
			return super.OnFocusLost( m_Parent, x, y );
		}
		return false;
	}
	
	void SetValue( string value, bool update = true )
	{
		m_EditBox.SetText( value );
		if( update )
			m_OptionChanged.Invoke( GetValue() );
	}
	
	string GetValue()
	{
		return m_EditBox.GetText();
	}
	
	override void ColorHighlight( Widget w )
	{
		if( !w )
			return;
		
		if ( m_EditBox )
		{
			SetFocus( m_EditBox );
			m_EditBox.SetColor( ARGB(255, 200, 0, 0) );
		}
		
		super.ColorHighlight( w );
	}
	
	override void ColorNormal( Widget w )
	{
		if( !w )
			return;
		
		if ( m_EditBox )
		{
			m_EditBox.SetColor( ARGB(140, 255, 255, 255) );
		}
		
		super.ColorNormal( w );
	}
}