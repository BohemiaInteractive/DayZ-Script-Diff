const static float NOTIFICATION_FADE_TIME = 3.0;

enum NotificationType
{
	FRIEND_CONNECTED,
	INVITE_FAIL_SAME_SERVER,
	JOIN_FAIL_GET_SESSION,
	CONNECT_FAIL_GENERIC,
	//Please add types before this item
	NOTIFICATIONS_END
}

class NotificationRuntimeData
{
	ref NotificationData	m_StaticData;
	
	float					m_NotificationTime;
	string					m_DetailText;
	
	void NotificationRuntimeData( float time, NotificationData data, string detail_text )
	{
		m_NotificationTime	= time + NOTIFICATION_FADE_TIME;
		m_StaticData		= data;
		if ( detail_text != "" )
			m_DetailText	= detail_text;
		else
			m_DetailText	= m_StaticData.m_DescriptionText;
	}
	
	void SetTime( float time )
	{
		m_NotificationTime = time;
	}
	
	float GetTime()
	{
		return m_NotificationTime;
	}
	
	string GetIcon()
	{
		return m_StaticData.m_Icon;
	}
	
	string GetTitleText()
	{
		return m_StaticData.m_TitleText;
	}
	
	string GetDetailText()
	{
		return m_DetailText;
	}
}

class NotificationSystem
{
	protected static const string								JSON_FILE_PATH = "Scripts/Data/notifications.json";					
	protected static const int									MAX_NOTIFICATIONS = 5;					
	
	protected static ref NotificationSystem						m_Instance;
				
	protected ref array<ref NotificationRuntimeData>			m_TimeArray;
	protected ref array<ref NotificationRuntimeData>			m_DeferredArray;
	protected ref map<NotificationType, ref NotificationData>	m_DataArray;
	
	protected ref ScriptInvoker									m_OnNotificationAdded = new ScriptInvoker;
	protected ref ScriptInvoker									m_OnNotificationRemoved = new ScriptInvoker;
	
	void NotificationSystem()
	{
		m_Instance = this;
		#ifndef WORKBENCH
		m_TimeArray = new array<ref NotificationRuntimeData>;
		m_DeferredArray = new array<ref NotificationRuntimeData>;
		
		LoadNotificationData();
		#endif
	}
	
	/**
	\brief Send custom notification to player from server
	@param player the target player to send notification to
	@param show_time amount of time this notification is displayed
	@param title_text the title text that is displayed in the notification
	@param detail_text additional text that can be added to the notification under the title - will not display additional text if not set
	@param icon the icon that is displayed in the notification - will use default icon if not set
	*/
	static void SendNotificationToPlayerExtended( Man player, float show_time, string title_text, string detail_text = "", string icon = "" )
	{
		if ( player )
		{
			SendNotificationToPlayerIdentityExtended( player.GetIdentity(), show_time, title_text, detail_text, icon );
		}
	}
	
	/**
	\brief Send custom notification to player identity from server
	@param player the target player to send notification to - if null, will send to all players
	@param show_time amount of time this notification is displayed
	@param title_text the title text that is displayed in the notification
	@param detail_text additional text that can be added to the notification under the title - will not display additional text if not set
	@param icon the icon that is displayed in the notification - will use default icon if not set
	*/
	static void SendNotificationToPlayerIdentityExtended( PlayerIdentity player, float show_time, string title_text, string detail_text = "", string icon = "" )
	{
		ScriptRPC rpc = new ScriptRPC();
		
		rpc.Write(show_time);
		rpc.Write(title_text);
		rpc.Write(detail_text);
		rpc.Write(icon);

		rpc.Send(null, ERPCs.RPC_SEND_NOTIFICATION_EXTENDED, true, player);
	}
	
	/**
	\brief Send notification from default types to player from server
	@param player the target player to send notification to
	@param type the type of notification to send - these can be viewed in /Scripts/Data/Notifications.json
	@param show_time amount of time this notification is displayed
	@param detail_text additional text that can be added to the notification under the title - will not display additional text if not set
	*/
	static void SendNotificationToPlayer( Man player, NotificationType type, float show_time, string detail_text = "" )
	{
		if ( player )
		{
			SendNotificationToPlayerIdentity( player.GetIdentity(), type, show_time, detail_text );
		}
	}
	
	/**
	\brief Send notification from default types to player identity from server
	@param player the target player to send notification to - if null, will send to all players
	@param type the type of notification to send - these can be viewed in /Scripts/Data/Notifications.json
	@param show_time amount of time this notification is displayed
	@param detail_text additional text that can be added to the notification under the title - will not display additional text if not set
	*/
	static void SendNotificationToPlayerIdentity( PlayerIdentity player, NotificationType type, float show_time, string detail_text = "" )
	{
		ScriptRPC rpc = new ScriptRPC();
		
		rpc.Write(type);
		rpc.Write(show_time);
		rpc.Write(detail_text);

		rpc.Send(null, ERPCs.RPC_SEND_NOTIFICATION, true, player);
	}
	
	/**
	\brief Send notification from default types to local player
	@param type the type of notification to send - these can be viewed in /Scripts/Data/Notifications.json
	@param show_time amount of time this notification is displayed
	@param detail_text additional text that can be added to the notification under the title - will not display additional text if not set
	*/
	static void AddNotification( NotificationType type, float show_time, string detail_text = "" )
	{
		if ( m_Instance.m_TimeArray.Count() < MAX_NOTIFICATIONS )
		{
			float time = GetGame().GetTickTime() + show_time;
			NotificationRuntimeData data = new NotificationRuntimeData(time, m_Instance.GetNotificationData( type ), detail_text);
			
			m_Instance.m_TimeArray.Insert( data );
			m_Instance.m_OnNotificationAdded.Invoke( data );
		}
		else
		{
			NotificationRuntimeData data_def = new NotificationRuntimeData(show_time, m_Instance.GetNotificationData( type ), detail_text);
			m_Instance.m_DeferredArray.Insert( data_def );
		}
	}
	
	/**
	\brief Send custom notification from to local player
	@param show_time amount of time this notification is displayed
	@param title_text the title text that is displayed in the notification
	@param detail_text additional text that can be added to the notification under the title - will not display additional text if not set
	@param icon the icon that is displayed in the notification - will use default icon if not set
	*/
	static void AddNotificationExtended( float show_time, string title_text, string detail_text = "", string icon = "" )
	{
		if( m_Instance.m_TimeArray.Count() < MAX_NOTIFICATIONS )
		{
			float time = GetGame().GetTickTime() + show_time;
			
			NotificationData temp_data = new NotificationData( icon, title_text );
			NotificationRuntimeData data = new NotificationRuntimeData(time, temp_data, detail_text);
			
			m_Instance.m_TimeArray.Insert( data );
			m_Instance.m_OnNotificationAdded.Invoke( data );
		}
		else
		{
			NotificationData temp_data_def = new NotificationData( icon, title_text );
			NotificationRuntimeData data_def = new NotificationRuntimeData(show_time, temp_data_def, detail_text);
			m_Instance.m_DeferredArray.Insert( data_def );
		}
	}
	
	static void Update(float timeslice)
	{
		if( m_Instance )
		{
			array<NotificationRuntimeData> to_remove = new array<NotificationRuntimeData>;
			foreach( NotificationRuntimeData data : m_Instance.m_TimeArray )
			{
				if( data.GetTime() < GetGame().GetTickTime() )
				{
					to_remove.Insert( data );
				} 
			}
			
			foreach( NotificationRuntimeData data2 : to_remove )
			{
				m_Instance.m_TimeArray.RemoveItem( data2 );
				m_Instance.m_OnNotificationRemoved.Invoke( data2 );
				
				if( m_Instance.m_DeferredArray.Count() > 0 )
				{
					NotificationRuntimeData data_def = m_Instance.m_DeferredArray.Get( m_Instance.m_DeferredArray.Count() - 1 );
					float time = GetGame().GetTickTime() + data_def.GetTime();
					data_def.SetTime( time );
					m_Instance.m_TimeArray.Insert( data_def );
					m_Instance.m_OnNotificationAdded.Invoke( data_def );
					m_Instance.m_DeferredArray.Remove( m_Instance.m_DeferredArray.Count() - 1 );
				}
			}
		}
	}
	
	protected NotificationData GetNotificationData( NotificationType type )
	{
		if( m_DataArray.Contains(type) )
		{
			return m_DataArray.Get( type );
		}
		
		return null;
	}
	
	static void LoadNotificationData()
	{
		map<NotificationType, NotificationData> data_array;
		m_Instance.m_DataArray = new map<NotificationType, ref NotificationData>;
		JsonFileLoader<map<NotificationType, NotificationData>>.JsonLoadFile( JSON_FILE_PATH, data_array );
		
		m_Instance.m_DataArray.Copy( data_array );
		
		array<int> types = new array<int>;
		NotificationType type_curr = 0;
		while ( type_curr != NotificationType.NOTIFICATIONS_END )
		{
			types.Insert( type_curr );
			type_curr++;
		}
		
		for ( int i = 0; i < m_Instance.m_DataArray.Count(); i++ )
		{
			types.RemoveItem( m_Instance.m_DataArray.GetKey( i ) );
		}
		
		if ( types.Count() > 0 )
		{
			foreach ( NotificationType type2 : types )
			{
				NotificationData data2 = new NotificationData( "please_add_an_icon", "please_add_a_title", "optional_description" );
				m_Instance.m_DataArray.Insert( type2, data2 );
			}
			JsonFileLoader<map<NotificationType, ref NotificationData>>.JsonSaveFile( JSON_FILE_PATH, m_Instance.m_DataArray );
		}
	}
	
	static void BindOnAdd( func fn )
	{
		m_Instance.m_OnNotificationAdded.Insert( fn );
	}
	
	static void UnbindOnAdd( func fn )
	{
		m_Instance.m_OnNotificationAdded.Remove( fn );
	}
	
	static void BindOnRemove( func fn )
	{
		m_Instance.m_OnNotificationRemoved.Insert( fn );
	}
	
	static void UnbindOnRemove( func fn )
	{
		m_Instance.m_OnNotificationRemoved.Remove( fn );
	}
}