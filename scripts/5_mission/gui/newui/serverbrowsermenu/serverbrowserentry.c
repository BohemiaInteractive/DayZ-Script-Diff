class ServerBrowserEntry extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected Widget m_Favorite;

	//Basic info
	protected TextWidget m_ServerName;
	protected TextWidget m_ServerPopulation;
	protected TextWidget m_ServerSlots;
	protected TextWidget m_ServerPing;
	protected ImageWidget m_ServerTime; // not currently displayed
	protected ImageWidget m_ServerLock;
	protected ImageWidget m_ServerModIcon;
	protected ImageWidget m_ServerMaKIcon;

	//Detailed info
	protected TextWidget m_ServerShard;
	protected TextWidget m_ServerCharacterAlive;
	protected TextWidget m_ServerFriends;
	protected TextWidget m_ServerMode;
	protected TextWidget m_ServerBattleye;
	protected TextWidget m_ServerIP;
	protected TextWidget m_ServerAcceleration;
	protected TextWidget m_ServerMap;
	protected TextWidget m_ServerMods;
	protected ButtonWidget m_ServerModsExpand;
	protected ref array<string> m_Mods;
	
	protected Widget m_DetailedInfo;
	protected bool m_IsExpanded;
	protected bool m_IsFavorited;
	protected bool m_IsOnline;

	protected ref GetServersResultRow m_ServerData;
	protected int m_Index;
	protected ServerBrowserTab m_Tab;
	protected bool m_Selected;
	protected bool m_FirstExpand = true;

	void ServerBrowserEntry(Widget parent, int index, ServerBrowserTab tab)
	{
	#ifdef PLATFORM_CONSOLE
		m_Root = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/server_browser/xbox/server_browser_list_entry.layout", parent);
	#else
		m_Root = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/server_browser/pc/server_browser_list_entry_pages.layout", parent);
	#endif
		
		m_Root.Enable(true);
		m_Favorite = m_Root.FindAnyWidget("favorite_button");
		m_ServerName = TextWidget.Cast(m_Root.FindAnyWidget("server_name"));
		m_ServerPopulation = TextWidget.Cast( m_Root.FindAnyWidget("server_population"));
		m_ServerSlots = TextWidget.Cast(m_Root.FindAnyWidget("server_slots"));
		m_ServerPing = TextWidget.Cast(m_Root.FindAnyWidget("server_ping"));
		m_ServerTime = ImageWidget.Cast(m_Root.FindAnyWidget("server_time"));
		m_ServerLock = ImageWidget.Cast(m_Root.FindAnyWidget("lock_icon"));
		m_ServerModIcon = ImageWidget.Cast(m_Root.FindAnyWidget("modded_icon"));
		m_ServerMaKIcon = ImageWidget.Cast(m_Root.FindAnyWidget("mandk_icon"));
		
		m_ServerShard = TextWidget.Cast(m_Root.FindAnyWidget("shard_text"));
		m_ServerCharacterAlive = TextWidget.Cast(m_Root.FindAnyWidget("character_alive_text"));
		m_ServerFriends = TextWidget.Cast(m_Root.FindAnyWidget("steam_friends_text"));
		m_ServerMode = TextWidget.Cast(m_Root.FindAnyWidget("mode_text"));
		m_ServerBattleye = TextWidget.Cast(m_Root.FindAnyWidget("battlleye_text"));
		m_ServerIP = TextWidget.Cast(m_Root.FindAnyWidget("ip_text"));
		m_ServerAcceleration = TextWidget.Cast(m_Root.FindAnyWidget("server_acceleration_text"));
		m_ServerMap = TextWidget.Cast(m_Root.FindAnyWidget("server_map"));
		m_ServerMods = TextWidget.Cast(m_Root.FindAnyWidget("mods_text"));
		m_ServerModsExpand = ButtonWidget.Cast(m_Root.FindAnyWidget("mods_expand"));

		m_DetailedInfo = m_Root.FindAnyWidget("detailed_info");
		
		m_Root.FindAnyWidget("basic_info").Show(true);
		m_Root.FindAnyWidget("favorite_image").Update();
		m_Root.FindAnyWidget("unfavorite_image").Update();
		
		m_Index	= index;
		m_Tab = tab;
		m_IsOnline = true;
		
		m_ServerTime.LoadImageFile(0, "set:dayz_gui image:icon_sun");
		m_ServerTime.LoadImageFile(1, "set:dayz_gui image:icon_sun_accel");
		m_ServerTime.LoadImageFile(2, "set:dayz_gui image:icon_moon");
		m_ServerTime.LoadImageFile(3, "set:dayz_gui image:icon_moon_accel");
		
		m_Root.SetHandler(this);
	}

	void ~ServerBrowserEntry()
	{
		delete m_Root;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	void Show(bool show)
	{
		m_Root.Show(show);
	}
	
	// Method never gets called on console!
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!IsOnline() || w == null)
		{
			return false;
		}

		switch (w)
		{
		#ifdef PLATFORM_CONSOLE
			case m_Root:
			{
				m_Tab.Connect(this);
				return true;
			}
		#endif
			case m_ServerModsExpand:
			{
				string mods_text;
				if (m_Mods && m_Mods.Count() > 0)
				{
					mods_text = m_Mods[0];
					for (int i = 1; i < m_Mods.Count(); i++)
					{
						mods_text += "\n" + m_Mods[i];
					}
				}
	
				g_Game.GetUIManager().ShowDialog("#main_menu_mods", mods_text, 0, 0, 0, 0, g_Game.GetUIManager().GetMenu());
				return true;
			}
		}

		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (!IsOnline())
		{
			return false;
		}
		
		if (button == MouseState.LEFT && w == m_Root)
		{
			m_Tab.Connect(this);
			return true;
		}

		return false;
	}

	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if (button == MouseState.LEFT)
		{
			switch (w)
			{
				case m_Favorite:
				{
					ToggleFavorite();
					return true;
				}
				case m_Root:
				{
					OnSelect();
					return true;
				}
			}
		}
		return false;
	}

	void OnSelect()
	{
		Darken(m_Root, 0, 0);
		Select();
		SetFocus(m_Root);
		m_Tab.SetServerDetails(m_ServerData, m_IsOnline);
	}
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (IsFocusable(w))
		{
			Preview(w, x, y);
			return true;
		}

		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (IsFocusable(w) && !IsFocusable(enterW))
		{
			Lighten(w, enterW, x, y);
			return true;
		}

		return false;
	}

	void Focus()
	{
		SetFocus(m_Root);
	}

	void ServerListFocus(bool focus)
	{
		m_Tab.ServerListFocus(focus, m_IsFavorited);
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (!m_Selected)
		{
			if (IsFocusable(w))
				Darken(w, x, y);

		#ifdef PLATFORM_CONSOLE
			if (w == m_Root)
			{
				Select();
				ServerListFocus(true);
				m_Tab.SetServerDetails(m_ServerData, m_IsOnline);
			}
		#endif

			return true;
		}
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
	#ifdef PLATFORM_CONSOLE
		if (w == m_Root)
		{
			Deselect();
			ServerListFocus(false);
		}
	#endif
		
		if (IsFocusable(w))
		{
			Lighten(w, null, x, y);
		}
		
		return true;
		return false;
	}

	bool IsOnline()
	{
		return m_IsOnline;
	}

	bool IsFocusable(Widget w)
	{
		if (w)
			return (w == m_Root || w == m_Favorite || w == m_ServerModsExpand);

		return false;
	}

	void FillInfo(GetServersResultRow server_info)
	{
		m_ServerData = server_info;
		m_FirstExpand = true;
		
	#ifndef PLATFORM_CONSOLE
		m_DetailedInfo.Show(server_info.m_IsExpanded);
	#endif
		
		SetName(server_info.m_Name);
		SetPasswordLocked(server_info.m_IsPasswordProtected);
		SetPopulationEx(server_info);
		SetSlots(server_info.m_MaxPlayers);
		SetPing(server_info.m_Ping);
		SetFavorite(server_info.m_Favorite);
		SetModded(server_info.m_Modded);
		SetServerMapName();
		RefreshDLCIcon();
		
	#ifdef PLATFORM_CONSOLE
		SetMouseAndKeyboard(server_info.m_MouseAndKeyboardEnabled);
	#endif
		
#ifdef PLATFORM_WINDOWS
	#ifndef PLATFORM_CONSOLE
		SetExpand(server_info.m_IsExpanded);
		
		int pp = 0; // private
		if (server_info.m_ShardId.Length() == 3 && server_info.m_ShardId.ToInt() < 200)
		{
			pp = 1;	// official
		}
		
		SetShard(pp);
		SetCharacterAlive(server_info.m_CharactersAlive);
		SetFriends(server_info.m_SteamFriends);
		SetMode(server_info.m_Disable3rdPerson);
		SetBattleye(server_info.m_AntiCheat);
		SetIP(server_info.m_Id);
		SetAcceleration(server_info.m_EnvironmentTimeMul);
	#endif
#endif
	}

	void UpdateEntry()
	{
		if (m_ServerData.m_IsSelected)
		{
			Darken(m_Root, 0, 0);
			Select();
			SetFocus(m_Root);
		}
		else
		{
			Lighten(m_Root, null, 0, 0);
			Deselect();
		}
	}

	void SetName(string name)
	{
		m_ServerName.SetText(name);
	}

	void SetPasswordLocked(bool locked)
	{
		m_ServerLock.Show(locked);
	}

	private void SetPopulationEx(GetServersResultRow serverInfo)
	{
		string popText = "";
		int population = serverInfo.m_CurrentNumberPlayers;
		int maxPlayers = serverInfo.m_MaxPlayers;
		int playersInQueue = serverInfo.m_PlayersInQueue;
		
		if (IsOnline())
		{
			// sometimes servers report a queue size even though server isn't full,
			// in which case we ignore queue size
			if (playersInQueue > 0 && population == maxPlayers)
			{
				popText = population.ToString() + "+" + playersInQueue.ToString() + "/" + maxPlayers.ToString();
			}
			else
			{
				popText = population.ToString() + "/" + maxPlayers.ToString();
			}
		}
		
		else
		{
			popText = "-";
		}

		m_ServerPopulation.SetText(popText);
	}

	void SetSlots(int slots)
	{
		if (IsOnline())
		{
			m_ServerSlots.SetText(slots.ToString());
		}
		else
		{
			m_ServerSlots.SetText("-");
		}
	}

	void SetPing(int ping)
	{
		int color;
		string displayValue;
		
		if (ping < 50)
			color = ARGBF(1, 0, 1, 0);
		else if(ping < 100)
			color = ARGBF(1, 0.8, 0.8, 0);
		else if( ping < 200 )
			color = ARGBF(1, 1, 0.5, 0);
		else
			color = ARGBF(1, 1, 0, 0);
		
		if (IsOnline())
		{
			displayValue = ping.ToString();
		}
		
		else
		{
			displayValue = "-";
		}
		
		m_ServerPing.SetColor(color);
		m_ServerPing.SetText(displayValue);
	}

	void SetTime(string time, float multiplier)
	{
		if (time != "")
		{
			TStringArray arr = new TStringArray;
			
			time.Split(":", arr);
			
			if (arr.Count() == 2)
			{
				int hour = arr.Get(0).ToInt();
				int minute = arr.Get(1).ToInt();
				
				if (hour >= 19 || hour <= 5)	//Night
				{
					if (multiplier > 1)
						m_ServerTime.SetImage(3);
					else
						m_ServerTime.SetImage(2);
				}
				else							//Day
				{
					if (multiplier > 1)
						m_ServerTime.SetImage(1);
					else
						m_ServerTime.SetImage(0);
				}
			}
		}
	}

	void SetShard(int shard)
	{
		string text;
		switch (shard)
		{
			case 0:
			{
				text = "#server_browser_entry_private";
				break;
			}
			case 1:
			{
				text = "#server_browser_entry_official";
				break;
			}
		}
		m_ServerShard.SetText(text);
	}

	void RefreshDLCIcon()
	{
		if (m_ServerData.m_IsDLC)
		{
			bool own = g_Game.VerifyWorldOwnership(GetMapToRun());
			m_ServerModIcon.Show(true);
			m_ServerModIcon.FindWidget("Owned").Show(own);
			m_ServerModIcon.FindWidget("Unowned").Show(!own);
		}
		else
		{
			m_ServerModIcon.FindWidget("Owned").Show(false);
			m_ServerModIcon.FindWidget("Unowned").Show(false);
		}
	}

	void SetCharacterAlive(string char_alive)
	{
		if (char_alive == "")
			m_ServerCharacterAlive.SetText("#STR_server_browser_char_not_alive");
		else
			m_ServerCharacterAlive.SetText(char_alive);
	}

	void SetFriends(string friends_text)
	{
		m_ServerFriends.SetText(friends_text);
	}

	void SetMode(int mode)
	{
		string text;
		switch (mode)
		{
			case 0:
			{
				text = "#server_browser_entry_person_both";
				break;
			}
			case 1:
			{
				text = "#server_browser_entry_person_first";
				break;
			}
		}
		m_ServerMode.SetText(text);
	}

	void SetBattleye(bool battleye)
	{
		if (battleye)
		{
			m_ServerBattleye.SetText("#server_browser_entry_enabled");
			m_ServerBattleye.SetColor(ARGBF(1, 0, 1, 0));
		}
		else
		{
			m_ServerBattleye.SetText("#server_browser_entry_disabled");
			m_ServerBattleye.SetColor(ARGBF(1, 1, 0, 0));
		}
	}

	void SetIP(string ip)
	{
		m_ServerIP.SetText(ip);
	}

	string GetIP()
	{
		return m_ServerData.GetIP();
	}

	int GetPort()
	{
		return m_ServerData.m_HostPort;
	}

	int GetSteamQueryPort()
	{
		return m_ServerData.m_SteamQueryPort;
	}

	string GetServerID()
	{
		return m_ServerData.m_Id;
	}

	string GetMapToRun()
	{
		return m_ServerData.m_MapNameToRun;
	}

	void SetFavorite(bool favorite)
	{
		m_IsFavorited = favorite;
		m_Root.FindAnyWidget("favorite_image").Show(favorite);
		m_Root.FindAnyWidget("unfavorite_image").Show(!favorite);		
	}

	void SetAcceleration(float mult)
	{
		if (mult > 1)
		{
			m_ServerAcceleration.Show(true);
			m_ServerAcceleration.SetText(mult.ToString() + "x");
		}
		else
		{
			m_ServerAcceleration.Show(false);
		}
	}

	void SetModded(bool is_modded)
	{
		m_ServerModIcon.Show(is_modded);
	}

	void SetServerMapName()
	{
		string displayValue = "-";

		if (IsOnline())
		{
			displayValue = ServerBrowserHelperFunctions.GetMapDisplayName(m_ServerData.m_MapNameToRun);
		}
		
		m_ServerMap.SetText(displayValue);
	}

	void SetMods(array<string> mods)
	{
		m_Mods = mods;

		if (mods && mods.Count() > 0)
		{
			string mods_text = mods[0];
			for (int i = 1; i < mods.Count(); i++)
				mods_text += ", " + mods[i];
	
			m_ServerMods.SetText(mods_text);
		}

	#ifdef PLATFORM_WINDOWS
		m_ServerModsExpand.Show((mods && mods.Count() > 0));
	#endif
	}
	
	void SetMouseAndKeyboard(bool is_mkenabled)
	{
		m_ServerMaKIcon.Show(is_mkenabled);
	}
	
	void SetIsOnline(bool isOnline)
	{
		m_IsOnline = isOnline;
	}
	
	bool ToggleFavorite()
	{
		m_IsFavorited = !m_IsFavorited;
		string ip = m_ServerData.GetIP();
#ifdef PLATFORM_WINDOWS	
		//Save Data PC
		m_Tab.GetRootMenu().AddFavorite(ip, m_ServerData.m_HostPort, m_IsFavorited);
	
	#ifdef PLATFORM_CONSOLE
		OnlineServices.SetServerFavorited(ip, 0, m_ServerData.m_SteamQueryPort, m_IsFavorited);
	#else
		OnlineServices.SetServerFavorited(ip, m_ServerData.m_HostPort, m_ServerData.m_SteamQueryPort, m_IsFavorited);
	#endif
#else
		//Save Data Console		
		m_IsFavorited = m_Tab.GetRootMenu().SetFavoriteConsoles(ip, m_ServerData.m_HostPort, m_IsFavorited);
#endif
		
		m_Root.FindAnyWidget("unfavorite_image").Show(!m_IsFavorited);
		m_Root.FindAnyWidget("favorite_image").Show(m_IsFavorited);
		
		return m_IsFavorited;
	}
	
	bool ToggleExpand()
	{
		return SetExpand(!m_IsExpanded);
	}
	
	bool SetExpand(bool expand)
	{
		m_IsExpanded = expand;
		m_Root.FindAnyWidget("collapse_image").Show(m_IsExpanded);
		m_Root.FindAnyWidget("expand_image").Show(!m_IsExpanded);
		m_DetailedInfo.Show(m_IsExpanded);
		
		if (m_ServerData)
		{
			m_ServerData.m_IsExpanded = m_IsExpanded;
		}
		
		if (expand && m_FirstExpand)
		{
			if (m_ServerData.m_Modded)
			{
				OnlineServices.GetServerModList(m_ServerData.m_Id);
			}

			m_FirstExpand = false;
		}
		
		return m_IsExpanded;
	}
	
	void Select(bool notify = true)
	{
		if (!m_Selected)
		{
			if (notify)
			{
				m_Tab.SelectServer(this);
			}
			
			m_ServerData.m_IsSelected = true;
			m_Selected = true;
		}
	}
	
	void Deselect()
	{
		if (m_Selected)
		{
			m_ServerData.m_IsSelected = false;		
			m_Selected = false;
			
			Lighten(m_Root, null, 0, 0);
		}
	}
	
	void UpdateColors()
	{
		float alpha = 1;
		int maxPlayers = m_ServerData.m_MaxPlayers;
		int whiteColor = ARGBF(1, 1, 1, 1);
		int populationColor = whiteColor;
		int populationOutline = 1;
		
		if (IsOnline())
		{	
			if (maxPlayers > 0)
			{
				int population = m_ServerData.m_CurrentNumberPlayers;
				float pop_percentage = population / maxPlayers;
	
				if (pop_percentage >= 1)
				{
					populationColor	= ARGBF(1, 1, 0, 0);
				}
				else if (pop_percentage >= 0.8)
				{
					populationColor	= ARGBF(1, 1, 0.5, 0);
				}
			}
		}
		else
		{
			alpha = 0.5;
			populationOutline = 0;
						
			m_ServerPing.SetColor(whiteColor);
		}
		
		m_ServerTime.Show(IsOnline());
		m_ServerName.SetColor(whiteColor);
		m_ServerName.SetAlpha(alpha);
		m_ServerPopulation.SetBold(IsOnline());
		m_ServerPopulation.SetColor(populationColor);
		m_ServerPopulation.SetOutline(populationOutline);
		m_ServerPopulation.SetAlpha(alpha);
		m_ServerSlots.SetAlpha(alpha);
		m_ServerPing.SetAlpha(alpha);
		m_Root.SetAlpha(alpha);
	}
	
	//Coloring functions (Until WidgetStyles are useful)
	void Preview(Widget w, int x, int y)
	{
		if (m_Selected)
			return;
		
		switch (w)
		{
			case m_Root:
			case m_Favorite:
			{
				m_Root.SetColor(ARGB(255, 0, 0, 0));
				m_Root.SetAlpha(1);
				UpdateColors();
				
				m_ServerName.SetColor(ARGB(255, 255, 0, 0));
				if (!IsOnline())
				{
					m_ServerName.SetAlpha(0.5);
				}
				break;
			}
		}
	}
	
	//Coloring functions (Until WidgetStyles are useful)
	void Darken(Widget w, int x, int y)
	{
		if (m_Selected)
			return;
		
		switch (w)
		{
			case m_Root:
			case m_Favorite:
			{
				m_Root.SetColor(ARGB(255, 200, 0, 0));
				SetPopulationEx(m_ServerData);
				UpdateColors();
				break;
			}
		}
	}
	
	void Lighten(Widget w, Widget enterW, int x, int y)
	{
		float alpha = 0.3;
		
		if (GetFocus() == w || m_Selected)
		{
			return;
		}
		
		if (w == m_Root && (m_Favorite && enterW == m_Favorite))
		{
			return;
		}

		m_Root.SetColor(ARGB(255, 0, 0, 0));			
		SetPopulationEx(m_ServerData);
		UpdateColors();
		
		if (m_Index % 2)
		{
			alpha = 0;
		}
		
		m_Root.SetAlpha(alpha);
	}
	
	GetServersResultRow GetServerData()
	{
		return m_ServerData;
	}

	// DEPRECATED
	string GetMapName();
	
	// DEPRECATED
	void SetMapName(string mapName);
	
	// DEPRECATED
	void SetPopulation(int population, int slots);
}
