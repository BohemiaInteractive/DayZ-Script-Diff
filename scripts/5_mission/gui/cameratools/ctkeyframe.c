class CTKeyframe extends ScriptedWidgetEventHandler
{
	protected int				m_Index;
	protected float				m_InterpTime;
	protected float				m_TotalTimeBefore;
	
	protected vector			m_Position;
	protected vector			m_Orientation;
	protected CameraToolsMenu	m_Menu;
	
	protected Widget			m_Root;
	protected TextWidget		m_IndexWidget;
	protected EditBoxWidget		m_InterpTimeWidget;
	protected EditBoxWidget		m_FOVWidget;
	protected EditBoxWidget		m_DOFWidget;
	protected EditBoxWidget		m_PinWidget;
	protected TextWidget		m_TotalTimeWidget;
	
	void CTKeyframe( int index, vector pos, vector orient, float int_value, float fov, float dof, int pin, float time_before, Widget root, CameraToolsMenu parent )
	{
		m_Menu				= parent;
		
		m_Root				= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/camera_tools/keyframe_entry.layout", root );
		
		m_IndexWidget		= TextWidget.Cast( m_Root.FindAnyWidget( "keyframe_id" ) );
		m_InterpTimeWidget	= EditBoxWidget.Cast( m_Root.FindAnyWidget( "keyframe_time_edit" ) );
		m_FOVWidget			= EditBoxWidget.Cast( m_Root.FindAnyWidget( "keyframe_fov_edit" ) );
		m_DOFWidget			= EditBoxWidget.Cast( m_Root.FindAnyWidget( "keyframe_dof_edit" ) );
		m_PinWidget			= EditBoxWidget.Cast( m_Root.FindAnyWidget( "keyframe_pin_edit" ) );
		m_TotalTimeWidget	= TextWidget.Cast( m_Root.FindAnyWidget( "keyframe_time" ) );
		
		m_Index				= index;
		m_TotalTimeBefore	= time_before;
		m_Position			= pos;
		m_Orientation		= orient;
		
		SetInterpTime( int_value );
		SetFOV( fov );
		SetDOF( dof );
		SetPin( pin );
		m_IndexWidget.SetText( m_Index.ToString() );
		m_Root.SetHandler( this );
	}
	
	void ~CTKeyframe()
	{
		delete m_Root;
	}
	
	float GetInterpTime()
	{
		string time_text = m_InterpTimeWidget.GetText();
		m_InterpTime = time_text.ToFloat();
		return m_InterpTime;
	}
	
	void SetPin( int pin )
	{
		m_PinWidget.SetText( pin.ToString() );
	}
	
	int GetPin()
	{
		return m_PinWidget.GetText().ToInt();
	}
	
	void SetFOV( float fov )
	{
		m_FOVWidget.SetText( fov.ToString() );
	}
	
	float GetFOV()
	{
		return m_FOVWidget.GetText().ToFloat();
	}
	
	void SetDOF( float dof )
	{
		m_DOFWidget.SetText( dof.ToString() );
	}
	
	float GetDOF()
	{
		return m_DOFWidget.GetText().ToFloat();
	}
	
	void SetPosition( vector pos )
	{
		m_Position = pos;
	}
	
	void SetOrientation( vector orient )
	{
		m_Orientation = orient;
	}
	
	vector GetPosition()
	{
		return m_Position;
	}
	
	vector GetOrientation()
	{
		return m_Orientation;
	}
	
	void SetTimeBefore( float time )
	{
		m_TotalTimeBefore = time;
		m_TotalTimeWidget.SetText( ( m_TotalTimeBefore + m_InterpTime ).ToString() );
	}
	
	void SetInterpTime( float time )
	{
		m_InterpTime = time;
		m_InterpTimeWidget.SetText( m_InterpTime.ToString() );
		m_TotalTimeWidget.SetText( ( m_TotalTimeBefore + m_InterpTime ).ToString() );
	}
	
	void Select()
	{
		m_Root.FindAnyWidget( "spacer" ).SetAlpha( 1 );
		m_IndexWidget.SetColor( ARGBF( 1, 1, 0, 0 ) );
		m_InterpTimeWidget.SetColor( ARGBF( 1, 1, 0, 0 ) );
		m_TotalTimeWidget.SetColor( ARGBF( 1, 1, 0, 0 ) );
	}
	
	void Unselect()
	{
		m_Root.FindAnyWidget( "spacer" ).SetAlpha( 0.625 );
		m_IndexWidget.SetColor( ARGBF( 1, 1, 1, 1 ) );
		m_InterpTimeWidget.SetColor( ARGBF( 1, 1, 1, 1 ) );
		m_TotalTimeWidget.SetColor( ARGBF( 1, 1, 1, 1 ) );
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		if( w == m_Root )
		{
			m_Menu.SelectKeyframe( this );
			return true;
		}
		return false;
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			m_Menu.SelectKeyframe( this );
			return true;
		}
		return false;
	}
	
	bool IsFocusable( Widget w )
	{
		if( w )
		{
			return ( w == m_InterpTimeWidget || w == m_TotalTimeWidget || w == m_FOVWidget || w == m_DOFWidget );
		}
		return false;
	}
}