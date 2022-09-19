class OptionSelectorSliderSetup extends OptionSelectorBase
{
	protected SliderWidget			m_Slider;
	protected float					m_MinValue;
	protected float					m_MaxValue;
	
	void ~OptionSelectorSliderSetup()
	{
		delete m_Root;
	}
	
	override void Enable()
	{
		super.Enable();
		
		m_Slider.ClearFlags( WidgetFlags.IGNOREPOINTER );
	}
	
	override void Disable()
	{
		super.Disable();
		
		m_Slider.SetFlags( WidgetFlags.IGNOREPOINTER );
	}
	
	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if( m_ParentClass )
		{
			OnFocus( w, x, y );
			m_ParentClass.OnFocus( m_Root.GetParent(), -1, m_SelectorType );
			#ifdef PLATFORM_WINDOWS
			m_ParentClass.OnMouseEnter( m_Root.GetParent().GetParent(), x, y );
			ColorHighlight(w);
			#endif
		}
		
		return true;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		if( m_ParentClass )
		{
			#ifdef PLATFORM_WINDOWS
			m_ParentClass.OnMouseLeave( m_Root.GetParent().GetParent(), enterW, x, y );
			ColorNormal( w );
			#endif
			OnFocusLost( w, x, y );
			SetFocus( null );
		}
			
		return true;
	}
	
	override bool OnMouseButtonUp( Widget w, int x, int y, int button )
	{
		if( button == MouseState.LEFT && w == m_Slider )
		{
		}
		return false;
	}
	
	override bool OnChange( Widget w, int x, int y, bool finished )
	{
		if( w == m_Slider )
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
			return ( w == m_Parent || w == m_Slider );
		}
		return false;
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		#ifdef PLATFORM_CONSOLE
		if( GetFocus() != m_Slider )
		{
			SetFocus( m_Slider );
			m_Parent.Enable( false );
		}
		
		return super.OnFocus( m_Parent, x, y );
		
		#else
		return false;
		#endif
	}
	
	override bool OnFocusLost( Widget w, int x, int y )
	{
		if( w == m_Slider )
		{
			m_Parent.Enable( true );
			return super.OnFocusLost( m_Parent, x, y );
		}
		return false;
	}
	
	float NormalizeInput( float value )
	{
		float ret = ( value - m_MinValue) / ( m_MaxValue - m_MinValue );
		return ret;
	}
	
	void SetStep(float step)
	{
		m_Slider.SetStep(step);
	}
	
	void SetValue( float value, bool update = true )
	{
		m_Slider.SetCurrent( NormalizeInput( value ) );
		if( update )
			m_OptionChanged.Invoke( GetValue() );
	}
	
	float GetValue()
	{
		float ret = ( m_Slider.GetCurrent() * ( m_MaxValue - m_MinValue ) ) + m_MinValue;
		return ret;
	}
	
	override void ColorHighlight( Widget w )
	{
		if( !w )
			return;
		
		if ( m_Slider )
		{
			SetFocus( m_Slider );
			m_Slider.SetColor( ARGB(255, 200, 0, 0) );
		}
		
		super.ColorHighlight( w );
	}
	
	override void ColorNormal( Widget w )
	{
		if( !w )
			return;
		
		if ( m_Slider )
		{
			m_Slider.SetColor( ARGB(140, 255, 255, 255) );
		}
		
		super.ColorNormal( w );
	}
}