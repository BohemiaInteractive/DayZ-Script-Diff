class OptionSelectorBase extends ScriptedWidgetEventHandler
{
	protected int							m_SelectorType = 0;
	protected Widget						m_Parent;
	protected Widget						m_Root;
	
	protected bool							m_Enabled;
	
	protected ScriptedWidgetEventHandler	m_ParentClass;
	
	ref ScriptInvoker						m_OptionFocused = new ScriptInvoker;
	ref ScriptInvoker						m_OptionUnfocused = new ScriptInvoker;
	ref ScriptInvoker						m_AttemptOptionChange = new ScriptInvoker;
	ref ScriptInvoker						m_OptionChanged = new ScriptInvoker;
	
	void ~OptionSelectorBase()
	{
		delete m_Root;
	}
	
	Widget GetParent()
	{
		return m_Parent;
	}
	
	bool IsFocusable( Widget w )
	{
		if( w )
		{
			return w == m_Parent;
		}
		return false;
	}
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if( !IsFocusable( w ) )
			return true;
		
		if( m_ParentClass )
		{
			m_ParentClass.OnFocus( m_Root.GetParent(), -1, m_SelectorType );
			#ifdef PLATFORM_WINDOWS
			m_ParentClass.OnMouseEnter( m_Root.GetParent().GetParent(), x, y );
			#endif
		}
		
		UIScriptedMenu menu = GetGame().GetUIManager().GetMenu();
		
		if ( menu && menu.IsInherited( CharacterCreationMenu ) )
		{
			menu.OnFocus( m_Root.GetParent(), -1, m_SelectorType );
			menu.OnMouseEnter( m_Root.GetParent().GetParent(), x, y );
		}
		
		#ifdef PLATFORM_WINDOWS
		ColorHighlight( w );
		#else
		ColorHighlightConsole( w );
		if( m_ParentClass )
		{
			m_ParentClass.OnFocus( m_Root.GetParent(), -1, m_SelectorType );
		}
		#endif
		
		return true;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		#ifdef PLATFORM_CONSOLE
		if( IsFocusable( enterW ) )
			return true;
		#endif
		
		if( m_ParentClass )
		{
			m_ParentClass.OnFocus( null, x, y );
			#ifdef PLATFORM_WINDOWS
			m_ParentClass.OnMouseLeave( m_Root.GetParent().GetParent(), enterW, x, y );
			#endif
		}
		
		UIScriptedMenu menu = GetGame().GetUIManager().GetMenu();
		
		if ( menu && menu.IsInherited( CharacterCreationMenu ) )
		{
			menu.OnFocus( null, x, y );
			menu.OnMouseLeave( m_Root.GetParent().GetParent(), enterW, x, y );
		}
		
		#ifdef PLATFORM_WINDOWS
		ColorNormal( w );
		#else
		ColorNormalConsole( w );
		if( m_ParentClass )
		{
			m_ParentClass.OnFocusLost( w, x, y );
		}
		#endif
	
		return true;
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorHighlightConsole( w );
			if( m_ParentClass )
			{
				m_ParentClass.OnFocus( m_Root.GetParent(), -1, m_SelectorType );
			}
			return true;
		}
		return false;
	}
	
	override bool OnFocusLost( Widget w, int x, int y )
	{
		ColorNormalConsole( w );
		if( m_ParentClass )
		{
			m_ParentClass.OnFocusLost( w, x, y );
		}
		return true;
	}
	
	void Focus()
	{
		#ifdef PLATFORM_CONSOLE
			SetFocus( m_Parent );
		#else
		#ifdef PLATFORM_WINDOWS
			SetFocus( m_Root );
		#endif
		#endif
	}
	
	void Enable()
	{
		m_Enabled = true;
		
		m_Parent.ClearFlags( WidgetFlags.IGNOREPOINTER );
		
		#ifdef PLATFORM_CONSOLE
			ColorNormalConsole(m_Parent);
		#else
			ColorNormal(m_Parent);
		#endif
	}
	
	void Disable()
	{
		m_Enabled = false;
		
		m_Parent.SetFlags( WidgetFlags.IGNOREPOINTER );
		
		#ifdef PLATFORM_CONSOLE
			ColorDisabledConsole(m_Parent);
		#else
			ColorDisabled(m_Parent);
		#endif
	}
	
	void ColorHighlight( Widget w )
	{
		if( !w )
			return;
				
		ButtonSetColor(w, ARGB(255, 255, 0, 0));
	}
	
	void ColorNormal( Widget w )
	{
		if( !w )
			return;
		
		int color_pnl = ARGB(255, 255, 255, 255);
		int color_lbl = ARGB(255, 255, 255, 255);
		
		ButtonSetColor(w, color_pnl);
		
		Widget title_label = w.FindAnyWidget( w.GetName() + "_label" );
		Widget option_label = w.FindAnyWidget( "option_label" );
		
		if ( title_label )
		{
			title_label.SetColor( color_lbl );
		}
		
		if ( option_label )
		{
			option_label.SetColor( color_lbl );
		}
	}
	
	void ColorDisabled( Widget w )
	{
		if( !w )
			return;
		
		int color_pnl = ARGB(0, 0, 0, 0);
		int color_lbl = ARGB(120, 255, 255, 255);
		
		ButtonSetColor(w, color_pnl);
		
		Widget title_label = w.FindAnyWidget( w.GetName() + "_label" );
		Widget option_label = w.FindAnyWidget( "option_label" );
		
		if ( title_label )
		{
			title_label.SetColor( color_lbl );
		}
		
		if ( option_label )
		{
			option_label.SetColor( color_lbl );
		}		
	}
	
	void ButtonSetColor( Widget w, int color )
	{
		Widget option = w.FindAnyWidget( w.GetName() + "_image" );
		
		if ( option )
		{
			option.SetColor( color );
		}
	}
	
	void ColorHighlightConsole( Widget w )
	{
		if( !w )
			return;
		
		int color_pnl = ARGB(255, 200, 0, 0);
		int color_lbl = ARGB(255, 255, 255, 255);
		
		ButtonSetColorConsole(w, color_pnl);
		ButtonSetAlphaAnimConsole( null );
		ButtonSetTextColorConsole(w, color_lbl);
	}
	
	void ColorNormalConsole( Widget w )
	{
		if( !w )
			return;
		
		int color_pnl = ARGB(0, 0, 0, 0);
		int color_lbl = ARGB(255, 255, 255, 255);
		
		ButtonSetColorConsole(w, color_pnl);
		ButtonSetAlphaAnimConsole( null );
		ButtonSetTextColorConsole(w, color_lbl);
	}
	
	void ColorDisabledConsole( Widget w )
	{
		if (!w)
			return;
		
		int color_pnl = ARGB(0, 0, 0, 0);
		int color_lbl = ARGB(120, 255, 255, 255);
		
		ButtonSetColorConsole(w, color_pnl);
		ButtonSetAlphaAnimConsole( null );
		ButtonSetTextColorConsole(w, color_lbl);
	}
	
	void ButtonSetColorConsole( Widget w, int color )
	{
		w.SetColor( color );
	}
	
	void ButtonSetAlphaAnimConsole( Widget w )
	{
		if( !w )
			return;
		
		Widget panel = w.FindAnyWidget( w.GetName() + "_panel" );
		
		if( panel )
		{
			//m_Root.SetWidgetAnimAlpha( panel );
		}
	}
	
	void ButtonSetTextColorConsole( Widget w, int color )
	{
		if( !w )
			return;

		TextWidget label	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_label" ) );
		TextWidget text		= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text_1" ) );
		
		if( label )
		{
			label.SetColor( color );
		}
		
		if( text )
		{
			text.SetColor( color );
		}
		
		if( text2 )
		{
			text2.SetColor( color );
		}
	}
}