const static float NOTIFICATION_FADE_TIME = 3.0; //! DEPRECATED (moved into NotificationSystem)

enum NotificationType
{
	FRIEND_CONNECTED,
	INVITE_FAIL_SAME_SERVER,
	JOIN_FAIL_GET_SESSION,
	CONNECT_FAIL_GENERIC,
	DISCONNECTED,
	GENERIC_ERROR,
	//Please add types before this item
	NOTIFICATIONS_END
}

class NotificationRuntimeData
{
	ref NotificationData	m_StaticData;
	
	float					m_NotificationTime;
	float					m_TimeRemaining;
	string					m_DetailText;
	
	void NotificationRuntimeData(float time, NotificationData data, string detail_text)
	{
		m_NotificationTime	= time;
		m_TimeRemaining		= time;
		m_StaticData		= data;

		if (detail_text != "")
			m_DetailText	= detail_text;
		else
			m_DetailText	= m_StaticData.m_DescriptionText;
	}

	float GetTime()
	{
		return m_NotificationTime;
	}
	
	float GetRemainingTime()
	{
		return m_TimeRemaining;
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
	
	void UpdateRemainingTime(float updateInterval)
	{
		m_TimeRemaining -= updateInterval;
	}
	
	//! DEPRECATED
	void SetTime(float time);
}

class NotificationSystem
{
	const int DEFAULT_TIME_DISPLAYED 	= 10;
	const float NOTIFICATION_FADE_TIME 	= 3.0;

	protected static const string JSON_FILE_PATH = "scripts/data/notifications.json";					
	protected static const int MAX_NOTIFICATIONS = 5;

	private static const float UPDATE_INTERVAL_THRESHOLD = 1.0;
	
	protected static ref NotificationSystem						m_Instance;
	
	protected ref array<ref NotificationRuntimeData>			m_TimeArray;
	protected ref array<ref NotificationRuntimeData>			m_DeferredArray;
	protected ref map<NotificationType, ref NotificationData>	m_DataArray;
	
	protected float m_TimeElapsed;
	
	ref ScriptInvoker m_OnNotificationAdded = new ScriptInvoker();
	ref ScriptInvoker m_OnNotificationRemoved = new ScriptInvoker();
	
	static void InitInstance()
	{
		if (!m_Instance)
		{
			m_Instance = new NotificationSystem();
			m_Instance.LoadNotificationData();
		}
	}
	
	static void CleanupInstance()
	{
		m_Instance = null;
	}
	
	static NotificationSystem GetInstance()
	{
		return m_Instance;
	}
	
	void NotificationSystem()
	{
		m_TimeElapsed 	= 0.0;

		m_TimeArray 	= new array<ref NotificationRuntimeData>();
		m_DeferredArray = new array<ref NotificationRuntimeData>();
	}
	
	/**
	\brief Send custom notification to player from server
	@param player the target player to send notification to
	@param show_time amount of time this notification is displayed
	@param title_text the title text that is displayed in the notification
	@param detail_text additional text that can be added to the notification under the title - will not display additional text if not set
	@param icon the icon that is displayed in the notification - will use default icon if not set
	*/
	static void SendNotificationToPlayerExtended(Man player, float show_time, string title_text, string detail_text = "", string icon = "")
	{
		if (player)
		{
			SendNotificationToPlayerIdentityExtended(player.GetIdentity(), show_time, title_text, detail_text, icon);
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
	static void SendNotificationToPlayerIdentityExtended(PlayerIdentity player, float show_time, string title_text, string detail_text = "", string icon = "")
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
	static void SendNotificationToPlayer(Man player, NotificationType type, float show_time, string detail_text = "")
	{
		if (player)
		{
			SendNotificationToPlayerIdentity(player.GetIdentity(), type, show_time, detail_text);
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
	static void AddNotification(NotificationType type, float show_time, string detail_text = "")
	{
		if (m_Instance.m_TimeArray.Count() < MAX_NOTIFICATIONS)
		{
			NotificationRuntimeData data = new NotificationRuntimeData(show_time, m_Instance.GetNotificationData(type), detail_text);
			
			m_Instance.m_TimeArray.Insert(data);
			m_Instance.m_OnNotificationAdded.Invoke(data);
		}
		else
		{
			NotificationRuntimeData dataDeferred = new NotificationRuntimeData(show_time, m_Instance.GetNotificationData(type), detail_text);
			m_Instance.m_DeferredArray.Insert(dataDeferred);
		}
	}
	
	/**
	\brief Send custom notification from to local player
	@param show_time amount of time this notification is displayed
	@param title_text the title text that is displayed in the notification
	@param detail_text additional text that can be added to the notification under the title - will not display additional text if not set
	@param icon the icon that is displayed in the notification - will use default icon if not set
	*/
	static void AddNotificationExtended(float show_time, string title_text, string detail_text = "", string icon = "")
	{
		if (m_Instance.m_TimeArray.Count() < MAX_NOTIFICATIONS)
		{
			NotificationData tempData = new NotificationData(icon, title_text);
			NotificationRuntimeData data = new NotificationRuntimeData(show_time, tempData, detail_text);
			
			m_Instance.m_TimeArray.Insert(data);
			m_Instance.m_OnNotificationAdded.Invoke(data);
		}
		else
		{
			NotificationData tempDataDeferred = new NotificationData(icon, title_text);
			NotificationRuntimeData dataDeferred = new NotificationRuntimeData(show_time, tempDataDeferred, detail_text);
			m_Instance.m_DeferredArray.Insert(dataDeferred);
		}
	}
	
	static void Update(float timeslice)
	{
		if (m_Instance)
		{
			if (g_Game.GetGameState() != DayZGameState.IN_GAME && g_Game.GetGameState() != DayZGameState.MAIN_MENU)
				return;
	
			m_Instance.m_TimeElapsed += timeslice;
			if (m_Instance.m_TimeElapsed >= UPDATE_INTERVAL_THRESHOLD)
			{
				array<NotificationRuntimeData> expiredNotifications = new array<NotificationRuntimeData>();
				foreach (NotificationRuntimeData visibleNotificationData : m_Instance.m_TimeArray)
				{
					if (visibleNotificationData.GetRemainingTime() <= 0.0)
						expiredNotifications.Insert(visibleNotificationData);
					else
						visibleNotificationData.UpdateRemainingTime(UPDATE_INTERVAL_THRESHOLD);
				}
				
				foreach (NotificationRuntimeData expiredNotificationData : expiredNotifications)
				{
					m_Instance.m_OnNotificationRemoved.Invoke(expiredNotificationData);
					m_Instance.m_TimeArray.RemoveItem(expiredNotificationData);
					
					if (m_Instance.m_DeferredArray.Count() > 0)
					{
						int count = m_Instance.m_DeferredArray.Count();
						NotificationRuntimeData deferredNotificationData = m_Instance.m_DeferredArray.Get(count - 1);
						m_Instance.m_TimeArray.Insert(deferredNotificationData);
						m_Instance.m_OnNotificationAdded.Invoke(deferredNotificationData);
						m_Instance.m_DeferredArray.Remove(count - 1);
					}
				}
	
				m_Instance.m_TimeElapsed = 0;
			}
		}
	}
	
	protected NotificationData GetNotificationData(NotificationType type)
	{
		if (m_DataArray.Contains(type))
			return m_DataArray.Get(type);
		
		return null;
	}
	
	static void LoadNotificationData()
	{
		map<NotificationType, NotificationData> dataArray;
		m_Instance.m_DataArray = new map<NotificationType, ref NotificationData>();
		string errorMessage;
		if (!JsonFileLoader<map<NotificationType, NotificationData>>.LoadFile(JSON_FILE_PATH, dataArray, errorMessage))
			ErrorEx(errorMessage);
		
		m_Instance.m_DataArray.Copy(dataArray);
		
		array<int> notificationTypes = new array<int>();
		NotificationType notificationType = 0;
		while (notificationType != NotificationType.NOTIFICATIONS_END)
		{
			notificationTypes.Insert(notificationType);
			notificationType++;
		}
		
		for (int i = 0; i < m_Instance.m_DataArray.Count(); ++i)
		{
			notificationTypes.RemoveItem(m_Instance.m_DataArray.GetKey(i));
		}
		
		if (notificationTypes.Count() > 0)
		{
			foreach (NotificationType notificationType2 : notificationTypes)
			{
				NotificationData notificationData = new NotificationData(
					"please_add_an_icon",
					"please_add_a_title",
					"optional_description",
				);

				m_Instance.m_DataArray.Insert(notificationType2, notificationData);
			}

			if (!JsonFileLoader<map<NotificationType, ref NotificationData>>.SaveFile(JSON_FILE_PATH, m_Instance.m_DataArray, errorMessage))
				ErrorEx(errorMessage);
		}
	}
}