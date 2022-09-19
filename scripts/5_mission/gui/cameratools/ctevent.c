class CTEvent extends ScriptedWidgetEventHandler
{
	protected int				m_Index;
	protected int				m_ActorIndex;
	protected int				m_EventType;
	protected float				m_EventTime;
	
	protected CameraToolsMenu	m_Menu;
	
	protected Widget			m_Root;
	protected TextWidget		m_IndexWidget;
	protected EditBoxWidget		m_EventActorWidget;
	protected EditBoxWidget		m_EventTimeWidget;
	protected EditBoxWidget		m_EventTypeWidget;
	protected CheckBoxWidget	m_EventWalkWidget;
	
	protected HumanCommandActionCallback	m_Callback;
	
	void CTEvent( int index, int actor, Widget root, bool walk, CameraToolsMenu parent )
	{
		m_Menu				= parent;
		m_Index				= index;
		m_ActorIndex		= actor;
		
		m_Root				= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/camera_tools/event_entry.layout", root );
		m_IndexWidget		= TextWidget.Cast( m_Root.FindAnyWidget( "event_id" ) );
		m_EventActorWidget	= EditBoxWidget.Cast( m_Root.FindAnyWidget( "event_actor_edit" ) );
		m_EventTimeWidget	= EditBoxWidget.Cast( m_Root.FindAnyWidget( "event_time_edit" ) );
		m_EventTypeWidget	= EditBoxWidget.Cast( m_Root.FindAnyWidget( "event_type_edit" ) );
		m_EventWalkWidget	= CheckBoxWidget.Cast( m_Root.FindAnyWidget( "auto_walk" ) );
		
		m_EventWalkWidget.SetChecked( walk );
		m_IndexWidget.SetText( m_Index.ToString() );
		m_EventActorWidget.SetText( m_ActorIndex.ToString() );
		m_Root.SetHandler( this );
	}
	
	void ~CTEvent()
	{
		delete m_Root;
	}
	
	void Play()
	{
		PlayerBase player = m_Menu.GetActor( GetEventActor() );
		if( player )
		{
			if( m_EventWalkWidget.IsChecked() )
			{
				player.GetInputController().OverrideMovementAngle( true, 1 );
				player.GetInputController().OverrideMovementSpeed( true, 1 );
			}
			else
			{
				if( player.GetCommand_Action() )
				{
					//player.GetCommand_Action().Cancel();
				}
				m_Callback = player.StartCommand_Action( GetEventType(), EmoteCB, DayZPlayerConstants.STANCEMASK_ALL );
				m_Callback.EnableCancelCondition(true);
			}
		}
	}
	
	void Stop()
	{
		if( m_Callback )
		{
			m_Callback.Cancel();
			m_Callback = null;
		}
		PlayerBase player = m_Menu.GetActor( GetEventActor() );
		if( player )
		{
			player.GetInputController().OverrideMovementSpeed( true, 0 );
		}
	}
	
	bool IsTime( float start_time, float end_time )
	{
		if( m_EventTime >= start_time )
		{
			if( m_EventTime <= end_time )
			{
				return true;
			}
		}
		return false;
	}
	
	void SetEventTime( float time )
	{
		m_EventTime = time;
		m_EventTimeWidget.SetText( time.ToString() );
	}
	
	void SetEventType( int type )
	{
		m_EventTypeWidget.SetText( type.ToString() );
	}
	
	float GetEventTime()
	{
		m_EventTime = m_EventTimeWidget.GetText().ToFloat();
		return m_EventTime;
	}
	
	int GetEventType()
	{
		return m_EventTypeWidget.GetText().ToInt();
	}
	
	int GetEventActor()
	{
		return m_EventActorWidget.GetText().ToInt();
	}
	
	bool GetEventWalk()
	{
		return m_EventWalkWidget.IsChecked();
	}
	
	void Select()
	{
		m_Root.FindAnyWidget( "spacer" ).SetAlpha( 1 );
		m_IndexWidget.SetColor( ARGBF( 1, 1, 0, 0 ) );
		m_EventTypeWidget.SetColor( ARGBF( 1, 1, 0, 0 ) );
		m_EventTypeWidget.SetColor( ARGBF( 1, 1, 0, 0 ) );
	}
	
	void Unselect()
	{
		m_Root.FindAnyWidget( "spacer" ).SetAlpha( 0.625 );
		m_IndexWidget.SetColor( ARGBF( 1, 1, 1, 1 ) );
		m_EventTypeWidget.SetColor( ARGBF( 1, 1, 1, 1 ) );
		m_EventTypeWidget.SetColor( ARGBF( 1, 1, 1, 1 ) );
	}
}