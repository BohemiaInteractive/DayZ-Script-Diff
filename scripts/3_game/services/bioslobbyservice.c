// ip, name, connection port, queryPort
typedef Param4<string, string, int, int> CachedServerInfo;

// Script File

enum ESortType
{
	HOST,		
	TIME,
	POPULATION,
	SLOTS,
	PING
	FAVORITE,
	PASSWORDED
};

enum ESortOrder
{
	ASCENDING,		
	DESCENDING,
};

class GetServerModListResult
{
	string m_Id;	// server id (IP:Port)
	array<string> m_Mods;
}

//! GetServersResultRow the output structure of the GetServers operation that represents one game server
class GetServersResultRow
{
	string	m_Id;					// PC is IP:Port
	int		m_Priority;				// PC something is working -> 667223046
	string	m_Name;					// PC is name of server
	string	m_Description; 			// PC not work
	string	m_HostIp; 				// PC not work
	int		m_HostPort; 			// PC is works
	bool	m_Invisible;
	bool	m_Official;
	string	m_MapNameToRun; 		// PC not work alway 0
	bool	m_Modded; 				// PC not work alway 0
	int		m_ModeId; 				// PC not work alway 0
	bool	m_AntiCheat;
	int		m_RegionId; 			// PC not work alway 0
	int		m_MinPlayers;
	int		m_MaxPlayers; 			// PC - max players
	int		m_FreeSlots; 			// PC - max players
	int		m_CurrentNumberPlayers;
	string	m_GameVersion; 			// PC not work alway ""
	bool	m_IsPasswordProtected;	// PC work
	string	m_CreatedAt;
	string	m_UpdatedAt;
	bool	m_MouseAndKeyboardEnabled;
	bool	m_WhitelistEnabled;
	bool	m_IsDLC;
	
	//Scripted
	bool		m_IsExpanded;
	int 		m_SortName;	
	int			m_SortTime;
	bool		m_IsSelected;

	//characters alive
	string m_CharactersAlive;
	//steam friends - list of names separated by comma
	string m_SteamFriends;
	
	int m_Ping;	
  	string m_TimeOfDay;
  	//! disable3rdPerson servers for mode regular/hardcore
  	int m_Disable3rdPerson;  //1 for disabled, ie. hardcore
	//! time multiplier of environment
	float m_EnvironmentTimeMul;
	float m_EnvironmentNightTimeMul;
	bool		m_AllowedFilePatching;
	string m_ShardId;
	int m_SteamQueryPort;
	
	bool m_Favorite;

	string GetIpPort()
	{
#ifdef PLATFORM_WINDOWS
		return m_Id;
#else
		return GetIP() + ":" + m_HostPort;
#endif
	}
	
	string GetIP()
	{
#ifdef PLATFORM_WINDOWS
		// Hack - In new Serverborwser on PC has bad m_HostIp but ID contains correct IP
		array<string> parts = new array<string>;
		m_Id.Split(":", parts);
		return parts[0];
#else
		return m_HostIp;
#endif
	}
	
	bool IsSelected()
	{
		return m_IsSelected;
	}
	
	string GetValueStr(ESortType sort_type)
	{
		switch( sort_type )
		{
			case ESortType.HOST:
			{
				return m_Name;
			}
		}
		
		return "";
	}	
	
	int GetValueInt(ESortType sort_type)
	{
		switch( sort_type )
		{
			case ESortType.TIME:
			{
				return m_SortTime;
			}
			case ESortType.POPULATION:
			{
				return m_CurrentNumberPlayers;
			}
			case ESortType.SLOTS:
			{
				return m_MaxPlayers;
			}
			case ESortType.PING:
			{
				return m_Ping;
			}
			case ESortType.FAVORITE:
			{
			
			}
			case ESortType.PASSWORDED:
			{
			
			}
		}
		
		return 0;
	}
};


typedef array<ref GetServersResultRow> GetServersResultRowArray;

class GetFirstServerWithEmptySlotResult
{
	GetServersResult m_Result;
};

//! GetServersInput the input structure of the GetServers operation
class GetFirstServerWithEmptySlotInput
{
	bool		m_Official;
	string		m_GameVersion;
	int			m_RegionId;

	bool		m_UseOfficial;
	bool		m_UseGameVersion;
	bool		m_UseRegionId;
		
	void SetOfficial( bool Official )
	{
		m_Official					= Official;
		m_UseOfficial				= true;
	}
	
	void SetGameVersion( string GameVersion )
	{
		m_GameVersion				= GameVersion;
		m_UseGameVersion			= true;
	}
	
	void SetRegionId( int RegionId )
	{
		m_RegionId					= RegionId;
		m_UseRegionId				= true;
	}
};

//! GetServersResult the output structure of the GetServers operation
class GetServersResult
{
	int								m_Page;
	int								m_Pages;
	ref GetServersResultRowArray	m_Results;

	int m_NumServers;
};

//! GetServersInput the input structure of the GetServers operation
class GetServersInput
{
	bool		m_AntiCheat;
	int			m_RowsPerPage;
	string		m_SortBy; //na of property
	ESortOrder	m_SortOrder;
	string		m_Name;
	string		m_GameType;
	int			m_Platform; // 1 - PC, 2 - xbox, 3 - PSN
	int			m_ModeId;
	int			m_Page;
	string		m_GameVersion;
	bool		m_Official;
	bool		m_Joinable;
	string		m_MapNameToRun;
	bool		m_IsModded;
	int			m_MinPlayers; // minimum 1
	int			m_MaxPlayers; // maximum 99
	bool		m_IsPasswordProtected;
	int			m_RegionId; // 1 - Americas, 2 - Europe, 3 - Asia
	int			m_Priority;
	int 		m_FreeSlotsMin;
	int 		m_FreeSlotsMax;
	string		m_HostIp;
	int			m_HostPort;
	string		m_FavoriteServers;
	bool		m_MouseAndKeyboardEnabled;
	bool		m_WhitelistEnabled;
	bool		m_IsDLC;

	bool m_UseAntiCheat;
	bool m_UseName;
	bool m_UseGameType;
	bool m_UseModeId;
	bool m_UseGameVersion;
	bool m_UseOfficial;
	bool m_UseJoinable;
	bool m_UseMapNameToRun;
	bool m_UseIsModded;
	bool m_UseMinPlayers; 
	bool m_UseMaxPlayers; 
	bool m_UseIsPasswordProtected;
	bool m_UseRegionId; 
	bool m_UsePriority;
	bool m_UseFreeSlotsMin;
	bool m_UseFreeSlotsMax;
	bool m_UseHostIp;
	bool m_UseHostPort;
	bool m_UseHostIps;
	bool m_UseMouseAndKeyboardEnabled;
	bool m_UseWhitelistEnabled;
	bool m_UseIsDLC;
	
	void SetAntiCheatFilter( bool anti_cheat )
	{
		m_AntiCheat					= anti_cheat;
		m_UseAntiCheat				= true;
	}
	
	void SetNameFilter( string name )
	{
		m_Name						= name;
		m_UseName					= true;
	}
	
	void SetGameTypeFilter( string game_type )
	{
		m_GameType					= game_type;
		m_UseGameType				= true;
	}
	
	void SetModeIdFilter( int mode_id )
	{
		m_ModeId					= mode_id;
		m_UseModeId					= true;
	}
	
	void SetGameVersionFilter( string game_version )
	{
		m_GameVersion				= game_version;
		m_UseGameVersion			= true;
	}
	
	void SetOfficialFilter( bool official )
	{
		m_Official					= official;
		m_UseOfficial				= true;
	}
	
	void SetJoinableFilter( bool joinable )
	{
		m_Joinable					= joinable;
		m_UseJoinable				= true;
	}
	
	void SetMapNameToRun( string mapNameToRun )
	{
		m_MapNameToRun = mapNameToRun;
		m_UseMapNameToRun = true;
	}
	
	void SetIsModdedFilter( bool is_modded )
	{
		m_IsModded					= is_modded;
		m_UseIsModded				= true;
	}
	
	void SetMinPlayersFilter( int min_players )
	{
		m_MinPlayers				= min_players;
		m_UseMinPlayers				= true;
	}
	
	void SetMaxPlayersFilter( int max_players )
	{
		m_MaxPlayers				= max_players;
		m_UseMaxPlayers				= true;
	}
	
	void SetIsPasswordProtectedFilter( bool password_protected )
	{
		m_IsPasswordProtected		= password_protected;
		m_UseIsPasswordProtected	= true;
	}
	
	void SetRegionIdFilter( int region )
	{
		m_RegionId					= region;
		m_UseRegionId				= true;
	}
	
	void SetPriorityFilter( int priority )
	{
		m_Priority					= priority;
		m_UsePriority				= true;
	}
	
	void SetFreeSlotsMin( int freeSlotsMin )
	{
		m_FreeSlotsMin = freeSlotsMin;
		m_UseFreeSlotsMin = true;
	}
	
	void SetFreeSlotsMax( int freeSlotsMax )
	{
		m_FreeSlotsMax = freeSlotsMax;
		m_UseFreeSlotsMax = true;
	}
	
	void SetHostIp( string hostIp )
	{
		m_HostIp = hostIp;
		m_UseHostIp = true;
	}
	
	void SetHostPort( int hostPort )
	{
		m_HostPort = hostPort;
		m_UseHostPort = true;
	}
	
	void SetMouseAndKeyboardEnabled(bool enabledMouseAndKeyboard)
	{
		m_MouseAndKeyboardEnabled = enabledMouseAndKeyboard;
		m_UseMouseAndKeyboardEnabled = true;
	}
	
	void SetFavorited( bool show )
	{
		m_SortBy += "F";
		AddShow( show );
		
	}
	
	void SetFriendsPlaying( bool show )
	{
		m_SortBy += "P";
		AddShow( show );
	}
	
	void SetPreviouslyPlayed( bool show )
	{
		m_SortBy += "R";
		AddShow( show );
	}
	
	void SetProperVersionMatch( bool show )
	{
		m_SortBy += "V";
		AddShow( show );
	}
	
	void SetFullServer( bool show )
	{
		m_SortBy += "S";
		AddShow( show );
	}
	
	void SetThirdPerson( bool show )
	{
		m_SortBy += "3";
		AddShow( show );
	}
	
	void SetPublic( bool show )
	{
		m_SortBy += "L";
		AddShow( show );
	}
	
	void SetAcceleratedTime( bool show )
	{
		m_SortBy += "A";
		AddShow( show );
	}

	void SetAllowedFilePatching( bool show )
	{
		m_SortBy += "7";
		AddShow( show );
	}
	
	void SetLAN()
	{
		m_SortBy += "N";
	}
	
	void SetPingFilter( int pingMaxValue )
	{
		m_SortBy += "M" + pingMaxValue + ";";
	}
	
	void SetBattleyeProtection(bool show)
	{
		m_SortBy += "B";
		AddShow(show);
	}
		
	void SetPassworded(bool show)
	{
		m_SortBy += "C";
		AddShow(show);
	}

	void AddShow( bool show )
	{
		if( show )
			m_SortBy += "+";
		else
			m_SortBy += "-";
	}
	
	void AddFavourite(string ip, int port)
	{
		m_FavoriteServers += ip + ";" + port + ";";
	}
	
	void SetWhitelistEnabled(bool whitelistEnabled)
	{
		m_WhitelistEnabled = whitelistEnabled;
		m_UseWhitelistEnabled = true;
		
		m_SortBy += "W";
		AddShow(whitelistEnabled);
	}
	
	void SetIsDLC(bool isDLC)
	{
		m_IsDLC = isDLC;
		m_UseIsDLC = true;
		
		m_SortBy += "D";
		AddShow(isDLC);
	}
};

class BiosLobbyService
{
	//! Query game servers
	/*!
		The async result is returned in the OnDoneAsync callback.
		Expected errors:
			BAD_PARAMETER -	if atleast one of the permissions is not supported on the current platform, 
							or atleast one of the input arrays is NULL.
			
		@param inputValues input structure
		
	*/
	proto native EBiosError GetServers(GetServersInput inputValues);

	//! First 1st free server
	/*!
		The async result is returned in the OnGetFirstServerWithEmptySlot callback.
				
	*/
	proto native EBiosError GetFirstServerWithEmptySlot(GetFirstServerWithEmptySlotInput inputValues);
	
	proto native void AddServerFavorite(string ipAddress, int port, int steamQueryPort);
	proto native void RemoveServerFavorite(string ipAddress, int port, int steamQueryPort);
	proto native void GetFavoriteServers(TStringArray favServers);
	
	//! Get cached info about favorited servers (ONLY ON WINDOWS)
	/*!
		@param favServersInfoCache gets populated with data with format: key = Query End Point, value.param1 = Server Name, value.param2 = Connection Port
	*/
	proto native void GetCachedFavoriteServerInfo(array<ref CachedServerInfo> favServersInfoCache);
	
	//! Async function to retrieve info about mods for specific server (only for PC)
	//! @param serverId have to be an id returned by callbacks issued by last call to GetServers
	//! if GetServers is used another time the id's must be upated
	proto native EBiosError GetServerModList(string serverId);
	
	//! Async callback for GetServers
	/*!
		@param result_list result object
		@param error error indicating success or fail of the async operation.
		@param response for debugging - this is the data returned by the server; or an empty string ;)
	*/
	
	void OnDoneAsync(GetServersResult result_list, EBiosError error, string response)
	{
		/*
		if (result_list.m_Results != null && result_list.m_Results.Count() > 0)
			Print(result_list.m_Results[0].m_TimeOfDay);
		*/
		OnlineServices.OnLoadServersAsync( result_list, error, response );
	}
	
	void OnGetFirstServerWithEmptySlot(GetFirstServerWithEmptySlotResult result_list, EBiosError error)
	{
		OnlineServices.OnAutoConnectToEmptyServer( result_list, error );
	}
	
	//! Async callback for GetServerModList
	void OnServerModList(GetServerModListResult result_list, EBiosError error)
	{
		OnlineServices.OnGetServerModList( result_list, error );
	}
};
