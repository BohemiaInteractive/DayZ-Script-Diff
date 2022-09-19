
enum EBiosJoinType
{
	INVITE,
	ACTIVITY,
};

class BiosUser
{
	proto native owned string GetName();
	proto native owned string GetUid();
	proto native BiosClientServices GetClientServices();
	proto native bool IsOnline();
};

class BiosUserManager
{
	//! Gets the initiatior of the title
	/*!
		@return BiosUser the initiator. May be NULL.
	*/
	proto native BiosUser GetTitleInitiator();
	
	//! Gets the currently present list of users
	/*!
		Fills in the array.
		Expected errors:
			BAD_PARAMETER -	user_list is NULL, 
	
		@return EBiosError indicating if the operation is done.
	*/
	proto native EBiosError GetUserList(array<ref BiosUser> user_list);
	
	//! Display a system dependant ui for log-on
	proto native EBiosError LogOnUserAsync(BiosUser user);

	//! Display a system dependant account picket
	/*!
		Xbox: The async result is returned in the OnUserPicked callback.
	
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError PickUserAsync();
	
	
	//! Parse the join data from from command line parameters.
	/*!
		The async result is returned in the OnJoin callback.
		The OnJoin callback is called also during runtime when a join is accepted.
		Expected errors:
			BAD_PARAMETER -	join_data could not be parsed, 
	
		@param join_data the startup join data from command line parameters.
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError ParseJoinAsync(string join_data);

	//! Parse the party data from from command line parameters.
	/*!
		The async result is returned in the OnPartyHost callback.
		The OnPartyHost callback is called also during runtime when a player hosts a game for the party.
		Expected errors:
			BAD_PARAMETER -	join_data could not be parsed, 
	
		@param party_data the startup party data from command line parameters.
		@return EBiosError indicating if the async operation is pending.
	*/
	proto native EBiosError ParsePartyAsync(string party_data);
	
	//! Informs the engine about the current selected user.
	/*!
		Must be called to ensure proper authentication etc.
	
		@param user the user to select.
	*/
	proto native void SelectUser(BiosUser user);
	
	//! Returns the currently selected user.
	/*!
		@return BiosUser the selected user. May be NULL.
	*/
	proto native BiosUser GetSelectedUser();
	
	//! Call async function to get database ID
	/*!
		@return EBiosError indicating if the async operation is pending. If active user is not set, then return NOT_FOUND
	*/
	proto native EBiosError GetUserDatabaseIdAsync();
	
	void SelectUserEx(BiosUser user)
	{
		BiosUser selectedUser = GetSelectedUser();
		if (selectedUser && selectedUser != user && g_Game.GetGameState() != DayZGameState.MAIN_MENU)
		{
			SelectUser(user);
			
			if ( GetGame().GetMission() )
			{
				GetGame().GetMission().AbortMission();
						
				if ( g_Game.GetGameState() == DayZGameState.JOIN )
					NotificationSystem.AddNotification( NotificationType.JOIN_FAIL_GET_SESSION, 6 );
						
				g_Game.SetGameState( DayZGameState.MAIN_MENU );
				g_Game.SetLoadState( DayZLoadState.MAIN_MENU_CONTROLLER_SELECT );
					
				g_Game.GamepadCheck();
				
				g_Game.DeleteGamepadDisconnectMenu();
			}
			else
			{
 				g_Game.MainMenuLaunch();
			}
		}
		else
			SelectUser(user);
	}
	
	//! Callback function.
	/*!
		@param dbID user database ID. If something went wrong, then it is empty string.
		@param error indicating correct state.
	*/
	void OnUserDatabaseId(string dbID, EBiosError error)
	{
		if( !OnlineServices.ErrorCaught( error ) )
		{
			g_Game.SetDatabaseID( dbID );
		}
	}

	//! Callback function.
	/*!
		@param error error indicating success or fail of the async operation.
	*/
	void OnUserLoggedOn(EBiosError error)
	{
		if( OnlineServices.ErrorCaught( error ) )
		{
			if (GetGame().GetUIManager())
			{
				GetGame().GetUIManager().CloseAllSubmenus();
				
				if ( GetGame().GetUIManager().IsDialogVisible() )
				{
					GetGame().GetUIManager().CloseDialog();
				}
				
				g_Game.DeleteGamepadDisconnectMenu();
			}
			
			if ( GetGame().GetMission() != null )
			{
				GetGame().GetMission().AbortMission();
			}
			
			GetGame().GetInput().ResetActiveGamepad();
			
			g_Game.SetGameState( DayZGameState.MAIN_MENU );
			g_Game.SetLoadState( DayZLoadState.MAIN_MENU_START );
			g_Game.GamepadCheck();
		}
	}
	
	//! Callback function.
	/*!
		@param user picked user. NULL on fail.
		@param error error indicating success or fail of the async operation.
	*/
	void OnUserPicked(BiosUser user, EBiosError error)
	{
		if ( !user )
		{
			GetGame().GetInput().ResetActiveGamepad();
			g_Game.GamepadCheck();
		}
		else if ( !OnlineServices.ErrorCaught( error ) )
		{
			SelectUserEx( user );
			if ( GetGame().GetMission() )
				GetGame().GetMission().Reset();
			OnGameNameChanged( user );
			g_Game.SelectUser(GetGame().GetInput().GetUserGamepad(user));
		}
	}

	//! Callback function.
	void OnLoggedOn(BiosUser user)
	{
		if ( user && GetSelectedUser() == user )
			g_Game.SelectUser(GetGame().GetInput().GetUserGamepad(user));
	}

	//! Callback function.
	void OnLoggedOff(BiosUser user)
	{
		OnSignedOut( user );
	}
	
	//! Callback function.
	/*!
		Called when a new user signed in
	
		@param user the user that signed in. Cannot be NULL.
	*/
	void OnSignedIn(BiosUser user)
	{
		
	}
	
	//! Callback function.
	/*!
		Called when a new user signed out
	
		@param user the user that signed out. Cannot be NULL.
	*/
	void OnSignedOut(BiosUser user)
	{
		Print("OnSignedOut(): selectedUser: " + GetSelectedUser());
		bool isSelectedUser  = (GetSelectedUser() != null);
		
		if ( user == GetSelectedUser() )
		{
			SelectUserEx( null );
			
			if (GetGame().GetUIManager())
			{
				GetGame().GetUIManager().CloseAllSubmenus();
				
				if ( GetGame().GetUIManager().IsDialogVisible() )
				{
					GetGame().GetUIManager().CloseDialog();
				}
				
				g_Game.DeleteGamepadDisconnectMenu();
			}
			
			if ( GetGame().GetMission() != null )
			{
				Print("OnSignedOut(): AbortMission");
				GetGame().GetMission().AbortMission();
				GetGame().AbortMission();
			}
			
			GetGame().GetInput().ResetActiveGamepad();
			
			g_Game.SetGameState( DayZGameState.MAIN_MENU );
			g_Game.SetLoadState( DayZLoadState.MAIN_MENU_START );
			g_Game.GamepadCheck();
		}
		else if ( !isSelectedUser )
		{
			Print("OnSignedOut(): before TitleScreen");
		}
	}
	
	//! Callback function.
	/*!
		Called when a join is parsed or when a runtime join is accepted from the system UI.
	
		@param type the type of join. Undefined on error.
		@param joiner the user that is joining. NULL on error.
		@param handle join handle for a session. Empty on error.
		@param joinee uid of the user that is being joined. Empty on error.
		@param error indicating if parsing failed. OK if not a result of ParseJoinAsync.
	*/
	void OnJoin(EBiosJoinType type, BiosUser joiner, string handle, string joinee, EBiosError error) 
	{
		if ( !OnlineServices.ErrorCaught( error ) )
		{
			OnlineServices.SetBiosUser( joiner );
			
			SelectUserEx( joiner );
			OnlineServices.SetSessionHandle( handle );
			
			if ( g_Game.GetGameState() == DayZGameState.IN_GAME )
			{
				g_Game.SetLoadState( DayZLoadState.JOIN_START );
				OnlineServices.GetSession();
			}
			else
			{
				if ( GetGame().GetUIManager() && GetGame().GetInput().IsActiveGamepadSelected() )
				{
					GetGame().GetUIManager().CloseMenu( MENU_TITLE_SCREEN );
					GetGame().GetInput().IdentifyGamepad( GamepadButton.BUTTON_NONE );
				}
				g_Game.SetLoadState( DayZLoadState.JOIN_START );
				g_Game.GamepadCheck();
			}
		}
		else
		{
			if ( g_Game.GetGameState() != DayZGameState.IN_GAME )
			{
				if ( GetGame().GetMission() )
				{
					if ( g_Game.GetGameState() != DayZGameState.MAIN_MENU )
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
		}
	}
	
	//! Callback function.
	/*!
		Called when a party is parsed or when a party is hosted from the system UI.
	
		@param host the user that is hosting the party. NULL on error.
		@param invitee_list list of party members.
		@param error indicating if parsing failed. OK if not a result of ParsePartyAsync.
	*/
	void OnPartyHost(BiosUser host, array<string> invitee_list, EBiosError error)
	{
		#ifdef PLATFORM_PS4
		if( host && host.IsOnline() )
		#endif
		{
			SelectUserEx(host);
		}
		#ifdef PLATFORM_PS4
		else if( host )
		{
			SelectUserEx( host );
			LogOnUserAsync( host );
		}
		#endif

		if (GetGame().GetUIManager())
		{
			GetGame().GetUIManager().CloseMenu(MENU_TITLE_SCREEN);
		}
		
		OnlineServices.SetPendingInviteList( invitee_list );
		if(g_Game.GetGameState() != DayZGameState.IN_GAME && g_Game.GetGameState() != DayZGameState.CONNECTING)
		{
			if (!GetGame().GetUIManager().GetMenu() || GetGame().GetUIManager().GetMenu().GetID() != MENU_MAIN)
			{
				GetGame().GetUIManager().EnterScriptedMenu(MENU_MAIN, GetGame().GetUIManager().GetMenu());
			}
			g_Game.SetGameState( DayZGameState.PARTY );
			g_Game.SetLoadState( DayZLoadState.PARTY_START );
			g_Game.GamepadCheck();
		}
	}

	//! Callback function.
	/*!
		Called when display info of a signed in user changed.
	
		@param user the user affected by the change.
	*/
	void OnGameNameChanged(BiosUser user)
	{
		if( user == GetSelectedUser() )
		{
			g_Game.SetPlayerName( user.GetName() );
			#ifdef PLATFORM_CONSOLE
				g_Game.SetPlayerGameName( user.GetName() );
			#endif
			if( GetGame().GetUIManager().GetMenu() )
			{
				GetGame().GetUIManager().GetMenu().Refresh();
			}
		}
	}
	
	BiosUser GetUser( string user_id )
	{
		array<ref BiosUser> user_list = new array<ref BiosUser>;
		GetUserList( user_list );
		foreach( BiosUser user : user_list )
		{
			if( user.GetUid() == user_id )
			{
				return user;
			}
		}
		return null;
	}
};
