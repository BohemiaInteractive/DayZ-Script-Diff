class CTActor extends CTObjectFollower
{
	protected int				m_Index;

	protected Widget			m_Root;
	protected TextWidget		m_IndexWidget;
	
	protected string			m_HandsItem;
	protected ref array<string>	m_Items;
	
	protected EntityAI			m_HandsItemObj;
	
	void CTActor( int index, vector pos, vector orient, string type, array<string> items, string hands_item, CameraToolsMenu parent )
	{
		m_FollowerRoot		= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/camera_tools/event_tracker.layout", null );
		m_FollowerButton	= m_FollowerRoot.FindAnyWidget( "IconPanel" );
		m_IndexWidget		= TextWidget.Cast( m_FollowerRoot.FindAnyWidget( "Text" ) );
		m_Index				= index;
		m_Position			= pos;
		m_Orientation		= orient;
		m_Menu				= parent;
		
		m_IndexWidget.SetText( m_Index.ToString() );
		m_FollowerRoot.SetHandler( this );
		CreateFollowedObject( type );
		SetHandsItem( hands_item );
		SetItems( items );
	}
	
	void ~CTActor()
	{
		delete m_FollowerRoot;
	}
	
	string GetActorType()
	{
		if( m_FollowedObject )
			return m_FollowedObject.GetType();
		return "";
	}
	
	void AddItem( string item )
	{
		PlayerBase p = PlayerBase.Cast( m_FollowedObject );
		if( p.GetInventory().CreateAttachment( item ) )
			m_Items.Insert( item );
	}
	
	void SetItems( array<string> items )
	{
		m_Items = items;
		PlayerBase p = PlayerBase.Cast( m_FollowedObject );
		if( p )
		{
			foreach( string item : items )
			{
				p.GetInventory().CreateAttachment( item );
			}
		}
	}
	
	array<string> GetItems()
	{
		return m_Items;
	}
	
	void SetHandsItem( string item )
	{
		m_HandsItem = item;
		if ( m_HandsItemObj && m_FollowedObject)
		{
			HumanInventory.Cast( m_FollowedObject.GetInventory() ).LocalDestroyEntity( m_HandsItemObj );
			GetGame().ObjectDelete( m_HandsItemObj );
		}
		if (item)
		{
			HumanInventory.Cast( m_FollowedObject.GetInventory() ).CreateInHands( item );
		}
	}
	
	string GetHandsItem()
	{
		return m_HandsItem;
	}
	
	void Reset()
	{
		string type = m_FollowedObject.GetType();
		DestroyFollowedObject();
		CreateFollowedObject( type );
		
		SetHandsItem( m_HandsItem );
		SetItems( GetItems() );
	}
}