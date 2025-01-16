class ServerBrowserFilterContainer extends ScriptedWidgetEventHandler
{
	ref map<string, string>		m_Options = new map<string, string>;
	
	protected EditBoxWidget	m_SearchByName;
	protected EditBoxWidget	m_SearchByIP;
	protected ButtonWidget m_RefreshList;

	ref OptionSelectorMultistate m_CharacterAliveFilter;
	ref OptionSelectorMultistate m_SortingFilter;
	ref OptionSelectorMultistate m_RegionFilter;
	ref OptionSelectorMultistate m_PingFilter;
	ref OptionSelectorMultistate m_MapFilter;
	ref OptionSelector m_DLCFilter; // DEPRECATED
	ref OptionSelector m_FavoritedFilter;
	ref OptionSelector m_FriendsPlayingFilter;
	ref OptionSelector m_BattleyeFilter; // DEPRECATED
	ref OptionSelector m_PasswordFilter;
	ref OptionSelector m_WhitelistFilter;
	ref OptionSelector m_KeyboardFilter;
	ref OptionSelector m_PreviouslyPlayedFilter;
	ref OptionSelector m_VersionMatchFilter;
	ref OptionSelector m_FullServerFilter;
	ref OptionSelector m_ThirdPersonFilter;
	ref OptionSelector m_PublicFilter; // DEPRECATED
	ref OptionSelector m_AcceleratedTimeFilter; // DEPRECATED
	protected ServerBrowserTab m_Tab;
	
	void ServerBrowserFilterContainer(Widget root, ServerBrowserTab parent)
	{
		string player_name;
		GetGame().GetPlayerName(player_name);
		m_Tab = parent;
		
		array<string> character_name_options ={"#server_browser_disabled", player_name};
		array<string> region_options = {"#server_browser_all", "#server_browser_americas", "#server_browser_europe", "#server_browser_asia"};
		array<string> sort_options = {"#server_browser_column_host A-Z", "#server_browser_column_host Z-A", "#players_title >", "#players_title <"};
		array<string> ping_options = {"#server_browser_disabled", "<30", "<50", "<100", "<200", "<300", "<500"};
		array<string> two_options = {"#server_browser_disabled", "#server_browser_show"};
		array<string> map_options = LoadMapFilterOptions();

		m_SearchByName = EditBoxWidget.Cast(root.FindAnyWidget("search_name_setting_option"));
		m_RefreshList = ButtonWidget.Cast(root.GetParent().FindAnyWidget("refresh_list_button"));
		
		m_RegionFilter = new OptionSelectorMultistate(root.FindAnyWidget("region_setting_option"), 0, this, false, region_options);
		m_PingFilter = new OptionSelectorMultistate(root.FindAnyWidget("ping_setting_option"), 0, this, false, ping_options);
		#ifdef PLATFORM_CONSOLE
		m_FavoritedFilter = new OptionSelectorMultistate(root.FindAnyWidget("favorites_setting_option"), 0, this, false, two_options);
		#else
		m_FavoritedFilter = new OptionSelector(root.FindAnyWidget("favorites_setting_option"), 0, this, false);
		#endif

		m_MapFilter	= new OptionSelectorMultistate(root.FindAnyWidget("map_setting_option"), 0, this, false, map_options);
		
		m_FriendsPlayingFilter = new OptionSelector(root.FindAnyWidget("friends_setting_option"), 0, this, false);
		m_PreviouslyPlayedFilter = new OptionSelector(root.FindAnyWidget("prev_played_setting_option"), 0, this, false);
		m_FullServerFilter = new OptionSelector(root.FindAnyWidget("full_server_setting_option"), 0, this, false);
		m_ThirdPersonFilter = new OptionSelector(root.FindAnyWidget("tps_setting_option"), 0, this, false);
		m_PasswordFilter = new OptionSelector(root.FindAnyWidget("password_setting_option"), 0, this, false);
		m_WhitelistFilter = new OptionSelector(root.FindAnyWidget("whitelist_setting_option"), 0, this, false);

		m_RegionFilter.m_OptionChanged.Insert(OnFilterChanged);
		m_PingFilter.m_OptionChanged.Insert(OnFilterChanged);
		m_FavoritedFilter.m_OptionChanged.Insert(OnFilterChanged);
		m_MapFilter.m_OptionChanged.Insert(OnMapFilterChanged);
		m_FriendsPlayingFilter.m_OptionChanged.Insert(OnFilterChanged);
		m_PreviouslyPlayedFilter.m_OptionChanged.Insert(OnFilterChanged);
		m_FullServerFilter.m_OptionChanged.Insert(OnFilterChanged);
		m_ThirdPersonFilter.m_OptionChanged.Insert(OnFilterChanged);
		m_PasswordFilter.m_OptionChanged.Insert(OnFilterChanged);
		m_WhitelistFilter.m_OptionChanged.Insert(OnFilterChanged);
		
		#ifdef PLATFORM_CONSOLE
			m_SortingFilter	= new OptionSelectorMultistate(root.FindAnyWidget("sort_setting_option"), 0, this, false, sort_options);
			m_KeyboardFilter = new OptionSelector(root.FindAnyWidget("keyboard_setting_option"), 0, this, false);
		
			m_SortingFilter.m_OptionChanged.Insert(OnSortChanged);
			m_SortingFilter.m_OptionChanged.Insert(OnFilterChanged);
			m_KeyboardFilter.m_OptionChanged.Insert(OnFilterChanged);
		
			m_PingFilter.Disable();
			m_FriendsPlayingFilter.Disable();
		#endif
		
		#ifdef PLATFORM_WINDOWS
			#ifndef PLATFORM_CONSOLE	
				m_SearchByIP = EditBoxWidget.Cast(root.FindAnyWidget("search_ip_setting_option"));
		
				m_SearchByName.SetHandler(this);
				m_SearchByIP.SetHandler(this);
		
				m_CharacterAliveFilter = new OptionSelectorMultistate(root.FindAnyWidget("restrict_char_setting_option"), 0, this, false, character_name_options);
				m_VersionMatchFilter = new OptionSelector(root.FindAnyWidget("ver_match_setting_option"), 0, this, false);
		
				m_CharacterAliveFilter.m_OptionChanged.Insert(OnFilterChanged);
				m_VersionMatchFilter.m_OptionChanged.Insert(OnFilterChanged);
			#endif
		#endif
		LoadFilters();
	}
	
	protected array<string> LoadMapFilterOptions()
	{
		array<string> mapOptions = {"#server_browser_disabled"};
		for (int i = ServerBrowserHelperFunctions.GetInstance().INTERNAL_MAP_NAMES.Count() - 1; i >= 0; i--)
		{
			mapOptions.Insert(ServerBrowserHelperFunctions.GetInstance().INTERNAL_MAP_NAMES.GetElement(i));
		}
		
		return mapOptions;
	}
	
	void OnFilterChanged()
	{
		m_Tab.OnFilterChanged();
	}
	
	void OnMapFilterChanged()
	{
		m_Tab.OnFilterChanged();
	}
	
	void LoadFilters()
	{
		string data;
		if (m_Tab.GetTabType() == TabType.LAN || m_Tab.GetTabType() == TabType.FAVORITE)
			return;
		
		GetGame().GetProfileString("SB_Filter_" + m_Tab.GetTabType(), data);
		
		m_Options.Clear();
		
		if (data != "")
		{
			string errorMessage;
			if (!JsonFileLoader<ref map<string, string>>.LoadData(data, m_Options, errorMessage))
				ErrorEx(errorMessage);
		}
		
		if (m_Options && m_Options.Count() >= 6)
		{
			m_SearchByName.SetText(m_Options.Get("m_SearchByName"));
			m_RegionFilter.SetStringOption(m_Options.Get("m_RegionFilter"), false);
			m_PingFilter.SetStringOption(m_Options.Get("m_PingFilter"), false);
			m_FriendsPlayingFilter.SetStringOption(m_Options.Get("m_FriendsPlayingFilter"), false);
			m_PreviouslyPlayedFilter.SetStringOption(m_Options.Get("m_PreviouslyPlayedFilter"), false);
			m_FullServerFilter.SetStringOption(m_Options.Get("m_FullServerFilter"), false);
			m_ThirdPersonFilter.SetStringOption(m_Options.Get("m_ThirdPersonFilter"), false);
			m_PasswordFilter.SetStringOption(m_Options.Get("m_PasswordFilter"), false);
			m_WhitelistFilter.SetStringOption(m_Options.Get("m_WhitelistFilter"), false);
			m_MapFilter.SetStringOption(m_Options.Get("m_MapFilter"), false);
			
			#ifdef PLATFORM_CONSOLE
				m_SortingFilter.SetStringOption(m_Options.Get("m_SortingFilter"), false);
				m_KeyboardFilter.SetStringOption(m_Options.Get("m_KeyboardFilter"), false);
			#else
				m_FavoritedFilter.SetStringOption(m_Options.Get("m_FavoritedFilter"), false);
			#endif
			
			if (m_Options.Count() >= 12)
			{
				#ifdef PLATFORM_WINDOWS
					#ifndef PLATFORM_CONSOLE
						m_SearchByIP.SetText(m_Options.Get("m_SearchByIP"));
						m_CharacterAliveFilter.SetStringOption(m_Options.Get("m_CharacterAliveFilter"), false);
						m_VersionMatchFilter.SetStringOption(m_Options.Get("m_VersionMatchFilter"), false);
					#endif
				#endif
			}
		}
	}
	
	void SaveFilters()
	{
		m_Options.Clear();
		
		m_Options.Insert("m_SearchByName", m_SearchByName.GetText());
		m_Options.Insert("m_RegionFilter", m_RegionFilter.GetStringValue());
		m_Options.Insert("m_PingFilter", m_PingFilter.GetStringValue());
		m_Options.Insert("m_FriendsPlayingFilter", m_FriendsPlayingFilter.GetStringValue());
		m_Options.Insert("m_PreviouslyPlayedFilter", m_PreviouslyPlayedFilter.GetStringValue());
		m_Options.Insert("m_FullServerFilter", m_FullServerFilter.GetStringValue());
		m_Options.Insert("m_ThirdPersonFilter", m_ThirdPersonFilter.GetStringValue());
		m_Options.Insert("m_PasswordFilter", m_PasswordFilter.GetStringValue());
		m_Options.Insert("m_WhitelistFilter", m_WhitelistFilter.GetStringValue());
		m_Options.Insert("m_MapFilter", m_MapFilter.GetStringValue());
		
		#ifdef PLATFORM_CONSOLE
			m_Options.Insert("m_SortingFilter", m_SortingFilter.GetStringValue());
			m_Options.Insert("m_KeyboardFilter", m_KeyboardFilter.GetStringValue());
		#else
			m_Options.Insert("m_FavoritedFilter", m_FavoritedFilter.GetStringValue());
		#endif
		
		#ifdef PLATFORM_WINDOWS
			#ifndef PLATFORM_CONSOLE
				m_Options.Insert("m_SearchByIP", m_SearchByIP.GetText());
				m_Options.Insert("m_CharacterAliveFilter", m_CharacterAliveFilter.GetStringValue());
				m_Options.Insert("m_VersionMatchFilter", m_VersionMatchFilter.GetStringValue());
			#endif
		#endif

		string data, errorMessage;
		if (!JsonFileLoader<ref map<string, string>>.MakeData(m_Options, data, errorMessage))
			ErrorEx(errorMessage);

		GetGame().SetProfileString("SB_Filter_" + m_Tab.GetTabType(), data);
	}
	
	void ResetFilters()
	{
		m_SearchByName.SetText("");
		m_RegionFilter.Reset();
		m_PingFilter.Reset();
		m_FavoritedFilter.Reset();
		m_FriendsPlayingFilter.Reset();
		m_PreviouslyPlayedFilter.Reset();
		m_FullServerFilter.Reset();
		m_ThirdPersonFilter.Reset();
		m_PasswordFilter.Reset();
		m_WhitelistFilter.Reset();
		m_MapFilter.Reset();
		
		#ifdef PLATFORM_CONSOLE
			m_SortingFilter.Reset();
			m_KeyboardFilter.Reset();
		#endif
		
		#ifdef PLATFORM_WINDOWS
			#ifndef PLATFORM_CONSOLE
				m_SearchByIP.SetText("");
				m_CharacterAliveFilter.Reset();
				m_VersionMatchFilter.Reset();
			#endif
		#endif
		SaveFilters();
	}
	
	bool PingIsSet()
	{
		return (m_Options["m_PingFilter"] != m_PingFilter.GetOptions()[0]);
	}
	
	bool FavoriteIsSet()
	{
		return (m_Options["m_FavoritedFilter"] != m_FavoritedFilter.GetOptions()[0]);
	}
	
	bool PreviouslyIsSet()
	{
		return (m_Options["m_PreviouslyPlayedFilter"] != m_PreviouslyPlayedFilter.GetOptions()[0]);
	}
	
	//! Removes all characters from the given string whirh are NOT a number, '.' or ':'. Returns true if any change was done to the input text.
	bool ProcessIntoIPAddress(out string potential_ip)
	{
		bool was_text_changed = false;
		int length = potential_ip.Length();
		
		// Filter out incompatible characters
		for (int i = length - 1 ; i >= 0; --i)
		{
			string character = potential_ip.Get(i);
			
			if (character.ToInt() == 0  &&  character != "0"  &&  character != "."  &&  character != ":")
			{
				potential_ip = potential_ip.SubstringInverted(potential_ip, i, i + 1);
				was_text_changed = true;
			}
		}
		
		bool text_was_shortened = LimitTextBoxCharacterCount(potential_ip, 21);
		
		return (was_text_changed  ||  text_was_shortened);
	}
	
	//! Limits the given text to 'max_length' character count. Returns true if any change was done to the input.
	bool LimitTextBoxCharacterCount(out string text, int max_length)
	{
		int length = text.Length();
		bool was_text_changed = false;
		
		if (length > max_length)
		{
			text = text.Substring(0, max_length);
			was_text_changed = true;
		}
		
		return was_text_changed;
	}
	
	//! Attempts to generate a valid IP address & port from the given string in format like this: "192.168.0.2:2503". If the process fails, then an empty string is returned.
	string GenerateValidIP(string potential_ip)
	{
		string correct_ip_format;
		
		TStringArray array_ip_and_port = new TStringArray;
		potential_ip.Split(":", array_ip_and_port);
		
		string ip_entries;
		int port;
		
		if(array_ip_and_port.Count() > 0)
		{
			ip_entries = array_ip_and_port.Get(0);
			TStringArray array_ip = new TStringArray;
			ip_entries.Split(".", array_ip);
			
			string ip_string;
			int ip_entries_count = array_ip.Count();
			
			if (ip_entries_count <= 3  ||  ip_entries_count > 4)
				return "";
			
			for (int i = 0; i < 4; i++)
			{
				int ip_entry = array_ip.Get(i).ToInt();
				
				// Check if the entry is a valid number
				if (ip_entry > 255  ||  ip_entry < 0)
					ip_entry = 0;
				
				ip_string = ip_string + ip_entry.ToString();
				
				if (i < ip_entries_count - 1)
					ip_string = ip_string + ".";
				;
			}
			
			correct_ip_format = ip_string;
		}
		else
		{
			ip_entries = "";
			correct_ip_format = ip_entries;
		}

		if (correct_ip_format == "")
			return ""; // Input string was not recognized as a valid IP address. Thus return an empty string.
		
		// The input string was recognized as a valid IP address. Thus continue with port number.
		
		if(array_ip_and_port.Count() > 1)
		{
			port = array_ip_and_port.Get(1).ToInt();
			
			if (port > 65535  |  port < 0) // max port value is 65535
				port = 2302;
			
			
			if (port >= 0)
				correct_ip_format = correct_ip_format + ":" + port.ToString();
			
		}
		else
		{
			port = 2302;
			correct_ip_format = correct_ip_format + ":" + port.ToString();
		}
		
		return correct_ip_format;
	}
	
	//! Turns any given string into a more convenient filter (limits character count, forces lowercase, trims empty spaces).
	bool GenerateValidFilter(out string potential_filter)
	{
		return LimitTextBoxCharacterCount(potential_filter, 16);
	}
	
	void OnSortChanged(int value)
	{
		switch(value)
		{
			case 0:
			{
				m_Tab.SetSort(ESortType.HOST, ESortOrder.ASCENDING);
				break;
			}
			case 1:
			{
				m_Tab.SetSort(ESortType.HOST, ESortOrder.DESCENDING);
				break;
			}
			case 2:
			{
				m_Tab.SetSort(ESortType.POPULATION, ESortOrder.DESCENDING);
				break;
			}
			case 3:
			{
				m_Tab.SetSort(ESortType.POPULATION, ESortOrder.ASCENDING);
				break;
			}
		}
	}
	
	void Focus()
	{
	#ifdef PLATFORM_CONSOLE		
		switch (m_Tab.GetTabType())
		{
			case TabType.FAVORITE:
			{
				SetFocus(m_RefreshList);
				break;
			}
			case TabType.OFFICIAL:
			case TabType.COMMUNITY:
			{
				SetFocus(m_SearchByName);
				break;
			}
		}
	#endif
	#ifdef PLATFORM_WINDOWS
		SetFocus(m_SearchByName);
	#endif
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if(w)
		{
			if(w == m_SearchByName)
			{
				string input_name = m_SearchByName.GetText();
				
				if (GenerateValidFilter(input_name))
					m_SearchByName.SetText(input_name); // Warning! Using SetText() causes the cursor to move at the end of the text box!
				
				OnFilterChanged();
				SetFocus(m_SearchByName);
	
				return true;
			}
			else if(w == m_SearchByIP)
			{
				string input_ip = m_SearchByIP.GetText();
				
				if (ProcessIntoIPAddress(input_ip))
					m_SearchByIP.SetText(input_ip); // Warning! Using SetText() causes the cursor to move at the end of the text box!
				
				OnFilterChanged();
				SetFocus(m_SearchByIP);
				
				return true;
			}
		}
		return false;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{		
		m_Tab.OnFilterFocus(w);
		
		return false;
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		return m_Tab.OnMouseEnter(w, x, y);
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		return m_Tab.OnMouseLeave(w, enterW, x, y);
	}
	
	GetServersInput GetFilterOptionsPC()
	{
		GetServersInput input = new GetServersInput();
		
		input.m_RowsPerPage = SERVER_BROWSER_PAGE_SIZE;
		input.m_Platform = 1;
		
		if (m_SearchByName)
		{
			string name_text = m_SearchByName.GetText();
			if (name_text != "")
			{
				input.SetNameFilter(name_text);
			}
		}
		
		if (m_FullServerFilter.IsSet())
		{
			input.SetFullServer(m_FullServerFilter.IsEnabled());
		}
		
		if (m_ThirdPersonFilter.IsSet())
		{
			input.SetThirdPerson(m_ThirdPersonFilter.IsEnabled());
		}
		
		if (m_PasswordFilter.IsSet())
		{
			input.SetPassworded(m_PasswordFilter.IsEnabled());
		}
		
		if (m_WhitelistFilter.IsSet())
		{
			input.SetWhitelistEnabled(m_WhitelistFilter.IsEnabled());
		}
		
		if (m_MapFilter.IsSet())
		{
			string internalMapName = ServerBrowserHelperFunctions.GetInternalMapName(m_MapFilter.GetStringValue());
			if (internalMapName != "")
				input.SetMapNameToRun(internalMapName);
		}
		
		#ifndef PLATFORM_CONSOLE
			if (m_SearchByIP)
			{
				string ip_text = m_SearchByIP.GetText();
				if (ip_text != "")
				{				
					input.SetHostIp(ip_text);
				}
			}
			
			if (m_PingFilter.IsSet())
			{
				string str_ping = m_PingFilter.GetStringValue();
				int ping = str_ping.Substring(1, str_ping.Length() - 1).ToInt();
				input.SetPingFilter(ping);
			}
		
			if (m_FriendsPlayingFilter.IsSet())
			{
				input.SetFriendsPlaying(m_FriendsPlayingFilter.IsEnabled());
			}
		
			if (m_PreviouslyPlayedFilter.IsSet())
			{
				input.SetPreviouslyPlayed(m_PreviouslyPlayedFilter.IsEnabled());
			}
			
			if (m_VersionMatchFilter.IsSet())
			{
				input.SetProperVersionMatch(m_VersionMatchFilter.IsEnabled());
			}
		#endif
		
		return input;
	}
	
	GetServersInput GetFilterOptionsConsoles()
	{
		GetServersInput input = new GetServersInput();
		
		input.m_RowsPerPage = SERVER_BROWSER_PAGE_SIZE;

		#ifdef PLATFORM_XBOX
			input.m_Platform = 2;
		#endif
		#ifdef PLATFORM_PS4
			input.m_Platform = 3;
		#endif
		
		if (m_SearchByName)
		{
			string name_text = m_SearchByName.GetText();
			if(name_text != "")
			{
				input.SetNameFilter(name_text);
			}
		}
		
		if (m_RegionFilter.IsSet())
		{
			input.SetRegionIdFilter(m_RegionFilter.GetValue());
		}

		if (m_FullServerFilter.IsSet())
		{
			input.SetFullServer(m_FullServerFilter.IsEnabled());
			if (m_FullServerFilter.IsEnabled())
			{
				input.SetFreeSlotsMax(1);
			}
			else
			{
				input.SetFreeSlotsMin(1);
			}
		}
			
		if (m_ThirdPersonFilter.IsSet())
		{
			// modeId is used to specify whether third person is allowed on CONSOLE servers
			// ALLOW third person: modeId = 0
			// BLOCK third person: modeId = 1
			input.SetModeIdFilter(!m_ThirdPersonFilter.IsEnabled());	
		}

		if (m_PasswordFilter.IsSet())
		{
			input.SetIsPasswordProtectedFilter(m_PasswordFilter.IsEnabled());
		}

		if (m_WhitelistFilter.IsSet())
		{
			input.SetWhitelistEnabled(m_WhitelistFilter.IsEnabled());
		}
							
		if (m_KeyboardFilter.IsSet())
		{
			input.SetMouseAndKeyboardEnabled(m_KeyboardFilter.IsEnabled());
		}
		
		if (m_MapFilter.IsSet())
		{
			string internalMapName = ServerBrowserHelperFunctions.GetInternalMapName(m_MapFilter.GetStringValue());
			if (internalMapName != "")
				input.SetMapNameToRun(internalMapName);
		}
		
		if (m_PreviouslyPlayedFilter.IsSet())
		{
			input.SetPreviouslyPlayed(m_PreviouslyPlayedFilter.IsEnabled());
		}
		
		return input;
	}
}