class ClientData
{
	static ref ScriptInvoker SyncEvent_OnPlayerListUpdate = new ScriptInvoker();
	static ref ScriptInvoker SyncEvent_OnEntityKilled = new ScriptInvoker();
	static ref ScriptInvoker SyncEvent_OnPlayerIgnitedFireplace = new ScriptInvoker();
	
	static ref array<Man>			m_PlayerBaseList = new array<Man>;
	static ref SyncPlayerList		m_PlayerList;
	static ref SyncPlayerList		m_LastNewPlayers;
	
	static ref OnlineServices		m_OnlineServices;
	
	static void ResetClientData()
	{
		if( m_PlayerBaseList )
			m_PlayerBaseList.Clear();
		if( m_PlayerList && m_PlayerList.m_PlayerList )
			m_PlayerList.m_PlayerList.Clear();
		if( m_LastNewPlayers && m_LastNewPlayers.m_PlayerList )
			m_LastNewPlayers.m_PlayerList.Clear();
	}
	
	static void AddPlayerBase( Man player )
	{
		if( m_PlayerBaseList && player != GetGame().GetPlayer() )
			m_PlayerBaseList.Insert( player );
	}
	
	static void RemovePlayerBase( Man player )
	{
		if( m_PlayerBaseList )
			m_PlayerBaseList.RemoveItem( player );
	}
	
	static void SyncEvent_OnRecievedPlayerList( SyncPlayerList player_list )
	{
		if (m_PlayerList && m_PlayerList.m_PlayerList)
		{
			//Print("SyncEvent_OnRecievedPlayerList | " + m_PlayerList.m_PlayerList.Count());
		}
		
		SyncPlayerList new_players = SyncPlayerList.Compare(m_PlayerList, player_list);
		
		if ( !m_LastNewPlayers )
		{
			m_LastNewPlayers = player_list;
		}
		else
		{
			m_LastNewPlayers = SyncPlayerList.Compare( m_LastNewPlayers, new_players );
		}
		
		m_PlayerList = SyncPlayerList.Compare(player_list, null);
		
		#ifdef PLATFORM_CONSOLE
			#ifndef PLATFORM_WINDOWS // if app is not on Windows with -XBOX parameter
				OnlineServices.LoadPermissions( GetSimplePlayerList( new_players ) );
				if( new_players.m_PlayerList.Count() > 0 )
					OnlineServices.LoadFriends();
				if( g_Game.GetGameState() == DayZGameState.IN_GAME )
					OnlineServices.SetMultiplayState(OnlineServices.GetMultiplayState());
			#endif
		#endif
		
		SyncEvent_OnPlayerListUpdate.Invoke( player_list );
	}
	static array<string> GetSimplePlayerList()
	{
		array<string> ids = new array<string>;
		if( m_PlayerList && m_PlayerList.m_PlayerList )
		{
			for( int i = 0; i < m_PlayerList.m_PlayerList.Count(); i++ )
			{
				SyncPlayer player = m_PlayerList.m_PlayerList.Get( i );
				ids.Insert( player.m_UID );
			}
		}
		
		return ids;
	}
	
	static array<string> GetSimplePlayerList( SyncPlayerList list )
	{
		array<string> ids = new array<string>;
		if( list )
		{
			for( int i = 0; i < list.m_PlayerList.Count(); i++ )
			{
				SyncPlayer player = list.m_PlayerList.Get( i );
				ids.Insert( player.m_UID );
			}
		}
		
		return ids;
	}
	
	static void SyncEvent_OnEntityKilled( SyncEntityKillInfo entity_killed_data )
	{
		SyncEvent_OnEntityKilled.Invoke(entity_killed_data.m_EntityVictim, entity_killed_data.m_EntityKiller, entity_killed_data.m_EntitySource, entity_killed_data.m_IsHeadShot );
	}
	
	static void SyncEvent_OnPlayerIgnitedFireplace( EFireIgniteType igante_type )
	{
		SyncEvent_OnPlayerIgnitedFireplace.Invoke(igante_type);
	}
}