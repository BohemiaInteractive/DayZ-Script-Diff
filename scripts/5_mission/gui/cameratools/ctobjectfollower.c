class CTObjectFollower extends ScriptedWidgetEventHandler
{
	protected Widget			m_FollowerRoot;
	protected Widget			m_FollowerButton;
	
	protected vector			m_Position;
	protected vector			m_Orientation;
	protected EntityAI			m_FollowedObject;
	
	protected float				m_MaxFade;
	protected float				m_MinFade;
	
	protected CameraToolsMenu	m_Menu;
	
	void ~CTObjectFollower()
	{
		DestroyFollowedObject();
		delete m_FollowerRoot;
	}
	
	void CreateFollowedObject( string type )
	{
		m_FollowedObject = EntityAI.Cast( GetGame().CreateObject( type, m_Position, true ) );
		SetPosition( m_Position );
		SetRotation( m_Orientation );
	}
	
	void DestroyFollowedObject()
	{
		if( m_FollowedObject )
		{
			GetGame().ObjectDelete( m_FollowedObject );
		}
	}
	
	void Update( float timeslice )
	{
		UpdatePos();
	}
	
	EntityAI GetObj()
	{
		return m_FollowedObject;
	}
	
	void SetPosition( vector position )
	{
		m_Position = position;
		if( m_FollowedObject )
		{
			m_FollowedObject.SetPosition( position );
			m_Position = m_FollowedObject.GetPosition();
		}
		UpdatePos();
	}
	
	void SetRotation( vector dir )
	{
		m_Orientation = dir;
		PlayerBase player = PlayerBase.Cast( m_FollowedObject );
		if( player )
		{
			player.SetOrientation( m_Orientation );
		}
		UpdatePos();
	}
	
	vector GetPosition()
	{
		if( m_FollowedObject && m_FollowedObject.GetPosition() != m_Position )
		{
			SetPosition( m_FollowedObject.GetPosition() );
		}
		return m_Position;
	}
	
	vector GetRotation()
	{
		if( m_FollowedObject )
		{
			return m_FollowedObject.GetOrientation();
		}
		return "0 0 0";
	}
	
/*! \brief Function updating the position of the tracker widget.
 *	
 *	Currently tracks using GetScreenPos() on the position of the player.
 */
	void UpdatePos()
	{
		vector relativePos;
			
		relativePos		=	GetGame().GetScreenPosRelative( GetPosition() );
		
		if( relativePos[0] >= 1 || relativePos[0] == 0 || relativePos[1] >= 1 || relativePos[1] == 0 )
		{
			m_FollowerRoot.Show( false );
			return;
		}
		else if( relativePos[2] < 0 )
		{
			m_FollowerRoot.Show( false );
			return;
		}
		else
		{
			m_FollowerRoot.Show( true );
		}
		
		float x, y;
		m_FollowerRoot.GetSize( x, y );
		
		m_FollowerRoot.SetPos( relativePos[0], relativePos[1] );
	}
	
	void Show()
	{
		m_FollowerRoot.Show( true );
	}
	
	void Hide()
	{
		m_FollowerRoot.Show( false );
	}
	
	void Fade( bool fade )
	{
		if( fade )
		{
			m_FollowerRoot.SetAlpha( m_MinFade );
		}
		else
		{
			m_FollowerRoot.SetAlpha( m_MaxFade );
		}
	}
	
	override bool OnClick( Widget w, int x, int y, int button )
	{
		return false;
	}
	
	override bool OnDoubleClick( Widget w, int x, int y, int button )
	{
		return false;
	}
	
	override bool OnMouseButtonDown( Widget w, int x, int y, int button )
	{
		if( w == m_FollowerButton && button == MouseState.LEFT )
		{
			if( m_Menu )
			{
				m_Menu.SelectActor( CTActor.Cast( this ) );
			}
			return true;
		}
		return false;
	}
	
	override bool OnMouseButtonUp( Widget w, int x, int y, int button )
	{
		if( w == m_FollowerButton && button == MouseState.LEFT )
		{
			if( m_Menu )
			{
				m_Menu.SelectActor( null );
			}
			return true;
		}
		return false;
	}
}