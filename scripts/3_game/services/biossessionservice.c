class BiosSessionService
{
	protected int				m_GetSessionAttempts;
	string						m_CurrentHandle;
	
	//! Enter a gameplay session
	/*!
		The async result is returned in the OnEnterGameplaySession callback.
		Expected errors:
			LOGICAL - if the user is currently in an active gameplay session.
			
		@param session_address server IP address.
		@param session_port server port.	
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError	EnterGameplaySessionAsync(string session_address, int session_port);
	
	//! Leave a gameplay session
	/*!
		The async result is returned in the OnLeaveGameplaySession callback.
		If there is an unexpected error the state is cleared.
		Expected errors:
			ERR_NOT_FOUND - when attempting to leave a gameplay session the user is not part of.
			ERR_LOGICAL - when the user is not in a gameplay session.
			
		@param session_address server IP address.
		@param session_port server port.	
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError	LeaveGameplaySessionAsync(string session_address, int session_port);
	
	//! Gets a session from a join handle
	/*!
		The async result is returned in the OnGetGameplaySession, OnGetLobbySession or OnGetSessionError callback,
		dependinng on the type of session, or error.
		Expected errors:

		@param join_handle the parsed join handle.
		@return EBiosError indicating if the async operation is pending.
	*/
	void TryGetSession( string join_handle = "" )
	{
		if ( join_handle != "" )
		{
			m_GetSessionAttempts	= 0;
			m_CurrentHandle			= join_handle;
		}
		
		if ( m_GetSessionAttempts < 10 )
			GetSessionAsync( m_CurrentHandle );
		else
		{
			GetGame().GetUserManager().SelectUserEx(OnlineServices.GetBiosUser());
			if ( g_Game.GetGameState() != DayZGameState.IN_GAME )
			{
				if ( GetGame().GetMission() )
				{
					GetGame().GetUIManager().CloseAllSubmenus();
					GetGame().GetMission().AbortMission();
				
					g_Game.SetGameState( DayZGameState.MAIN_MENU );
					g_Game.SetLoadState( DayZLoadState.MAIN_MENU_START );
					g_Game.GamepadCheck();
				}
				else
				{
					g_Game.MainMenuLaunch();
				}
			}
			
			GetGame().GetUIManager().ShowDialog( "#str_xbox_join_fail_title", "#str_xbox_join_fail", 444, DBT_OK, DBB_NONE, DMT_INFO, GetGame().GetUIManager().GetMenu() );
		}
	}
	
	//! Gets a session from a join handle
	/*!
		The async result is returned in the OnGetGameplaySession, OnGetLobbySession or OnGetSessionError callback,
		dependinng on the type of session, or error.
		Expected errors:

		@param join_handle the parsed join handle.
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError	GetSessionAsync(string join_handle);
	
	//! Sets the activity to a gameplay session
	/*!
		The async result is returned in the OnSetActivity callback.
		Expected errors:
			ERR_NOT_FOUND - when attempting to set a gameplay session activity the user is not part of.
			
		@param session_address server IP address.
		@param session_port server port.		
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError	SetGameplayActivityAsync(string session_address, int session_port);
	
	//! not implemented
	//proto native EBiosError	SetLobbyActivityAsync(...);
	
	//! Clears the current activity
	/*!
		The async result is returned in the OnClearActivity callback.
		Expected errors:
					
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError	ClearActivityAsync();
	
	//! Show system UI to invite friends to current gameplay session
	/*! 
		The async result is returned in the OnShowInviteToGameplaySession callback. 
		On Xbox, if session with session_address and session_port does not exist, then xbox show 
		message "We could not send the invite".
		
		@param session_address server IP address.
		@param session_port server port.
		
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError ShowInviteToGameplaySessionAsync(string session_address, int session_port);

	//! Send invite to list of users
	/*! 
		The async result is returned in the OnInviteToGameplaySession callback. 
		The user must be inside the session. That means, OnEnterGameplaySession must be called with no errors.
		
		@param session_address server IP address.
		@param session_port server port.
		@param invitee_list list of users to invite

		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError InviteToGameplaySessionAsync(string session_address, int session_port, array<string> invitee_list);

	//! Notifiy about interactive multiplayer state
	proto native void SetMultiplayState(bool is_active);
	
	//! Callback function
	/*!
		@param error error indicating success or fail of the async operation.
	*/
	void OnSetActivity(EBiosError error)
	{
		OnlineServices.ErrorCaught( error );
	}
	
	//! Callback function
	/*!
		@param error error indicating success or fail of the async operation.
	*/
	void OnClearActivity(EBiosError error)
	{
	}
	
	//! Callback function
	/*!
		@param session_address server IP address.
		@param session_port server port.
	*/
	void OnGetGameplaySession(string session_address, int session_port)
	{	
		m_GetSessionAttempts = 0;
		if ( g_Game.GetGameState() == DayZGameState.IN_GAME )
		{
			string addr;
			int port;
			bool found = GetGame().GetHostAddress( addr, port );
			if ( found && !( addr == session_address && port == session_port ) )
			{
				OnlineServices.SetInviteServerInfo( session_address, session_port );
				g_Game.GetUIManager().EnterScriptedMenu( MENU_INVITE_TIMER, null );
			}
			else
			{
				NotificationSystem.AddNotification( NotificationType.INVITE_FAIL_SAME_SERVER, 6, "#ps4_already_in_session" );
			}
			
		}
		else
		{
			g_Game.ConnectFromJoin( session_address, session_port );
		}
	}
	
	//! //! Callback function, not implemented
	/*void OnGetLobbySession(...)
	{
	}*/
	
	//! Callback function
	/*!
		@param error error indicating fail of the async operation. Cannot be OK.
	*/
	void OnGetSessionError(EBiosError error)
	{
		OnlineServices.ErrorCaught( error );
		m_GetSessionAttempts++;
		
		#ifdef PLATFORM_XBOX
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( TryGetSession, 100, false, "" );
		#else
			#ifdef PLATFORM_PS4
				GetGame().GetUserManager().SelectUserEx(OnlineServices.GetBiosUser());
				if( g_Game.GetGameState() != DayZGameState.IN_GAME )
				{
					if( GetGame().GetMission() )
					{
						if( g_Game.GetGameState() != DayZGameState.MAIN_MENU )
						{
							GetGame().GetUIManager().CloseAllSubmenus();
							GetGame().GetMission().AbortMission();
							if (g_Game.GetGameState() == DayZGameState.JOIN)
								NotificationSystem.AddNotification(NotificationType.JOIN_FAIL_GET_SESSION, 6);
							g_Game.SetGameState( DayZGameState.MAIN_MENU );
							g_Game.SetLoadState( DayZLoadState.MAIN_MENU_START );
							g_Game.GamepadCheck();
							return;
						}
					}
					else
					{
						g_Game.MainMenuLaunch();
					}
				}
				NotificationSystem.AddNotification( NotificationType.JOIN_FAIL_GET_SESSION, 6 );
			#endif
		#endif
	}
	
	//! Callback function
	/*!
		@param session_address server IP address. Empty if failed.
		@param session_port server port. 0 if failed.
		@param error error indicating success or fail of the async operation.
	*/
	void OnEnterGameplaySession(string session_address, int session_port, EBiosError error)
	{
		if ( !OnlineServices.ErrorCaught( error ) )
		{
			SetGameplayActivityAsync( session_address, session_port );
			if ( OnlineServices.GetPendingInviteList() )
				InviteToGameplaySessionAsync( session_address, session_port, OnlineServices.GetPendingInviteList() );
			
			//OnlineServices.GetCurrentServerInfo(session_address, session_port);
		}
	}
	
	//! Callback function
	/*!
		@param error error indicating success or fail of the async operation.
	*/
	void OnLeaveGameplaySession(EBiosError error)
	{
	}
	
	//! Callback function
	/*!
		@param error indicating success or fail of the async operation.
	*/
	void OnShowInviteToGameplaySession(EBiosError error)
	{
		OnlineServices.ErrorCaught( error );
	}

	//! Callback function
	/*!
		@param error indicating success or fail of the async operation.
	*/
	void OnInviteToGameplaySession(EBiosError error)
	{

	}
	
};
