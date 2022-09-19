class OptionSelector extends OptionSelectorBase
{
	protected Widget						m_PreviousOption;
	protected Widget						m_NextOption;
	protected TextWidget					m_SelectedOption;
	protected int							m_SelectedOptionIndex;
	protected ref array<string>				m_Options;
	
	void OptionSelector( Widget parent, int current_index, ScriptedWidgetEventHandler parent_c, bool disabled )
	{
		m_Options				= { "#server_browser_disabled", "#server_browser_show", "#server_browser_hide" };
		m_ParentClass			= parent_c;
		m_SelectorType			= 2;
		if( current_index < 0 || current_index >= m_Options.Count() )
		{
			m_SelectedOptionIndex = 0;
		}
		else
		{
			m_SelectedOptionIndex = current_index;
		}
		
		m_Root						= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/option_selector.layout", parent );
		#ifdef PLATFORM_CONSOLE
			m_Parent				= parent.GetParent().GetParent();
		#else
		#ifdef PLATFORM_WINDOWS
			m_Parent				= parent.GetParent();
		#endif
		#endif
		
		//m_DisablePanel				= m_Parent.GetParent().FindAnyWidget( m_Parent.GetName() + "_disable" );
		
		m_SelectedOption			= TextWidget.Cast( m_Root.FindAnyWidget( "option_label" ) );
		m_PreviousOption			= m_Root.FindAnyWidget( "prev_option" );
		m_NextOption				= m_Root.FindAnyWidget( "next_option" );
		
		#ifdef PLATFORM_CONSOLE
			m_NextOption.Show( false );
			m_PreviousOption.Show( false );
		#endif
		
		m_Enabled = !disabled;
		if( m_Enabled )
		{
			Enable();
		}
		else
		{
			Disable();
		}
		
		m_SelectedOption.SetText( m_Options.Get( m_SelectedOptionIndex ) );
		
		m_Parent.SetHandler( this );
	}
	
	void ~OptionSelector()
	{
		delete m_Root;
	}
	
	override bool OnMouseButtonUp( Widget w, int x, int y, int button )
	{
		if( button == MouseState.LEFT )
		{
			if( w == m_NextOption )
			{
				SetNextOption();
				return true;
			}
			else if ( w == m_PreviousOption )
			{
				SetPrevOption();
				return true;
			}
		}
		return false;
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		if( button == MouseState.LEFT )
		{
			if( w == m_Parent )
				SetNextOption();
		}
		return true;
	}
	
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		return super.OnMouseEnter( w, x, y );
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		return super.OnMouseLeave( w, enterW, x, y );
	}
	
	void Reset()
	{
		m_SelectedOptionIndex = 0;
		
		m_SelectedOption.SetText( m_Options.Get( m_SelectedOptionIndex ) );
		//ColorOption();
		
		m_OptionChanged.Invoke( m_SelectedOptionIndex );
	}
	
	void SetNextOption()
	{
		m_SelectedOptionIndex++;
		if( m_SelectedOptionIndex >= m_Options.Count() )
		{
			m_SelectedOptionIndex = 0;
		}
		
		m_SelectedOption.SetText( m_Options.Get( m_SelectedOptionIndex ) );
		//ColorOption();
		
		m_OptionChanged.Invoke( m_SelectedOptionIndex );
	}
	
	void SetPrevOption()
	{
		m_SelectedOptionIndex--;
		if( m_SelectedOptionIndex < 0 )
		{
			m_SelectedOptionIndex = m_Options.Count() - 1;
		}
		
		m_SelectedOption.SetText( m_Options.Get( m_SelectedOptionIndex ) );
		//ColorOption();
		
		m_OptionChanged.Invoke( m_SelectedOptionIndex );
	}
	
	array<string> GetOptions()
	{
		return m_Options;
	}
	
	
	bool IsSet()
	{
		return m_SelectedOptionIndex != 0;
	}
	
	bool IsEnabled()
	{
		return m_SelectedOptionIndex == 1;
	}
	
	/*
	int GetOptionIndex()
	{
		return m_SelectedOptionIndex;
	}
	
	void SetOptionIndex(int option_index)
	{
		m_SelectedOptionIndex = option_index;
	}
	*/
	
	string GetStringValue()
	{
		return m_Options.Get( m_SelectedOptionIndex );
	}
	
	void SetStringOption( string option, bool fire_event = true )
	{
		int index = m_Options.Find( option );
		if( index > -1 )
		{
			m_SelectedOptionIndex = index;
			m_SelectedOption.SetText( m_Options.Get( m_SelectedOptionIndex ) );
			//ColorOption();
			
			if( fire_event )
				m_OptionChanged.Invoke( m_SelectedOptionIndex );
		}
	}
	
	void ColorOption()
	{
		switch( m_SelectedOptionIndex )
		{
			case 0:
			{
				m_SelectedOption.SetColor( ARGB( 255, 255, 255, 255 ) );
				break;
			}
			case 1:
			{
				m_SelectedOption.SetColor( ARGB( 255, 0, 255, 0 ) );
				break;
			}
			case 2:
			{
				m_SelectedOption.SetColor( ARGB( 255, 255, 0, 0 ) );
				break;
			}
		}
	}
	
	override bool IsFocusable( Widget w )
	{
		if( w )
		{
			return ( w == m_Parent || w == m_NextOption || w == m_PreviousOption );
		}
		return false;
	}
	
	override void Enable()
	{
		super.Enable();
		#ifndef PLATFORM_CONSOLE
			m_NextOption.ClearFlags( WidgetFlags.IGNOREPOINTER );
			m_NextOption.Show( true );
			m_PreviousOption.ClearFlags( WidgetFlags.IGNOREPOINTER );
			m_PreviousOption.Show( true );
		#endif
	}
	
	override void Disable()
	{
		super.Disable();
		#ifndef PLATFORM_CONSOLE
			m_NextOption.SetFlags( WidgetFlags.IGNOREPOINTER );
			m_NextOption.Show( false );
			m_PreviousOption.SetFlags( WidgetFlags.IGNOREPOINTER );
			m_PreviousOption.Show( false );
		#endif
	}
}
