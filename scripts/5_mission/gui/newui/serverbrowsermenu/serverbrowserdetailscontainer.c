class ServerBrowserDetailsContainer extends ScriptedWidgetEventHandler
{	
	protected ServerBrowserTab m_Tab;

	protected Widget m_Root;
	protected TextWidget m_ServerNameText;
	protected TextWidget m_ServerTypeResult;
	protected TextWidget m_ServerModeResult;
	protected TextWidget m_LastCharacterResult;
	protected TextWidget m_ServerIP;
	
	#ifdef PLATFORM_WINDOWS
	#ifndef PLATFORM_CONSOLE
	protected GridSpacerWidget m_FriendsContainer;
	protected TextWidget m_FriendsResult;
	#endif
	#endif
	
	protected TextWidget m_TimeAccelerationResult;
	protected ImageWidget m_TimeIcon;

	protected TextWidget m_BattleyeResult;
	
	protected WrapSpacerWidget m_VersionContainer;
	protected TextWidget m_VersionResult;
	
	protected GridSpacerWidget m_ServerDecContainer;
	protected MultilineTextWidget m_ServerDesc;

	protected WrapSpacerWidget m_DetailsContainerRight;
	protected ImageWidget m_MapImage;

	protected ButtonWidget m_BtnShowFilters;
	protected ButtonWidget m_RefreshList;

	protected bool m_IsOnline;

	void ServerBrowserDetailsContainer(Widget root, ServerBrowserTab parent)
	{
		m_Root = root;
		m_Tab = parent;
		
		m_ServerNameText = TextWidget.Cast(root.FindAnyWidget("details_servername_label"));
		m_ServerTypeResult = TextWidget.Cast(root.FindAnyWidget("details_servertype_result_label"));
		m_ServerModeResult = TextWidget.Cast(root.FindAnyWidget("details_mode_result_label"));
		m_LastCharacterResult = TextWidget.Cast(root.FindAnyWidget("details_character_result_label"));
		m_ServerIP = TextWidget.Cast(root.FindAnyWidget("details_ip_result_label"));
		
		#ifdef PLATFORM_WINDOWS
		#ifndef PLATFORM_CONSOLE
		m_FriendsContainer = GridSpacerWidget.Cast(root.FindAnyWidget("details_friends_container"));
		m_FriendsResult = TextWidget.Cast(root.FindAnyWidget("details_friends_result_label"));
		#endif
		#endif
		
		m_TimeAccelerationResult = TextWidget.Cast(root.FindAnyWidget("details_time_result_label"));
		m_TimeIcon = ImageWidget.Cast(root.FindAnyWidget("detail_time_img"));
		
		m_BattleyeResult = TextWidget.Cast(root.FindAnyWidget("details_battleye_result_label"));
		
		m_VersionContainer = WrapSpacerWidget.Cast(root.FindAnyWidget("details_version_container"));
		m_VersionResult = TextWidget.Cast(root.FindAnyWidget("details_version_result_label"));
		
		m_ServerDecContainer = GridSpacerWidget.Cast(root.FindAnyWidget("details_description_container"));
		m_ServerDesc = MultilineTextWidget.Cast(root.FindAnyWidget("details_description_label"));
		
		m_DetailsContainerRight = WrapSpacerWidget.Cast(root.FindAnyWidget("details_container_right"));
		m_MapImage = ImageWidget.Cast(root.FindAnyWidget("map_image"));
		
		m_TimeIcon.LoadImageFile(0, "set:dayz_gui image:icon_sun");
		m_TimeIcon.LoadImageFile(1, "set:dayz_gui image:icon_sun_accel");
		m_TimeIcon.LoadImageFile(2, "set:dayz_gui image:icon_moon");
		m_TimeIcon.LoadImageFile(3, "set:dayz_gui image:icon_moon_accel");

		m_BtnShowFilters = ButtonWidget.Cast(parent.GetDetailsRoot().FindAnyWidget("show_filters_button"));
		m_RefreshList = ButtonWidget.Cast(parent.GetFilterRoot().FindAnyWidget("refresh_list_button"));
	}

	void SetDetails(GetServersResultRow serverInfo, bool online = false)
	{
		if (!online)
		{
			ClearDetails();
			return;
		}

		if (serverInfo)
		{
			SetName(serverInfo.m_Name);
		#ifdef PLATFORM_WINDOWS
			#ifndef PLATFORM_CONSOLE
			SetType(serverInfo.m_ShardId);
			SetMode(serverInfo.m_Disable3rdPerson);
			#endif
		#endif
			#ifdef PLATFORM_CONSOLE
			SetMode(serverInfo.m_ModeId);
			SetConsoleType(serverInfo.m_Official);
			#endif
			SetServerIP(serverInfo.GetIpPort());
			
			//! Server time acceleration text version is hidden for now
			/*
			SetTimeAcceleration(serverInfo.m_EnvironmentTimeMul, serverInfo.m_EnvironmentNightTimeMul);
			*/

			SetTimeIcon(serverInfo.m_TimeOfDay, serverInfo.m_EnvironmentTimeMul);
		#ifdef PLATFORM_WINDOWS
			#ifndef PLATFORM_CONSOLE
			SetCharacter(serverInfo.m_CharactersAlive);
			SetFriends(serverInfo.m_SteamFriends);
			#endif
		#endif
			
			//! Server anticheat text is hidden for now
			/*
			SetBattleyeState(serverInfo.m_AntiCheat);
			*/
			
			//! Server version text is hidden for now
			/*
			SetVersion(serverInfo.m_GameVersion);
			*/
			
			SetServerDesc(serverInfo.m_Description);
		}
		
		//! Map image container and handling is hidden/disabled for now
		m_DetailsContainerRight.Show(false);
		//SetServerMap();
	}
	
	void ClearDetails()
	{
		m_ServerNameText.SetText("#STR_server_browser_details_no_server_selected");
		m_ServerTypeResult.SetText("-");
		m_ServerModeResult.SetText("-");
		m_ServerIP.SetText("-");
		m_TimeIcon.Show(false);
		m_TimeAccelerationResult.Show(true);
		m_TimeAccelerationResult.SetText("-");
	}
	
	protected void SetName(string name)
	{
		m_ServerNameText.SetText(name);
		m_ServerNameText.Update();
	}
	
	protected void SetType(string shardId)
	{
		string result = "#server_browser_tab_community";
		if (shardId.Length() == 3 && shardId.ToInt() < 200)
			result = "#server_browser_entry_official";
		
		m_ServerTypeResult.SetText(result);
	}

	#ifdef PLATFORM_CONSOLE
	protected void SetConsoleType(bool offical)
	{
		string result = "#server_browser_tab_community";
		if (offical)
			result = "#server_browser_entry_official";
		
		m_ServerTypeResult.SetText(result);
	}
	#endif
	
	protected void SetMode(int mode)
	{
		string result = "#STR_server_browser_no_server_selected";		
		switch (mode)
		{
			case 0:
			{
				result = "#STR_server_browser_details_third_person";
				break;
			}
			case 1:
			{
				result = "#STR_server_browser_details_first_person";
				break;
			}
		}

		m_ServerModeResult.SetText(result);
	}
	
	protected void SetServerIP(string ip)
	{
		m_ServerIP.SetText(ip);
	}

#ifdef PLATFORM_WINDOWS
	#ifndef PLATFORM_CONSOLE
	protected void SetCharacter(string characterName)
	{
		string result = "#STR_server_browser_char_not_alive";
		if (characterName != "")
			result = characterName;
		
		m_LastCharacterResult.SetText(result);
	}
	
	protected void SetFriends(string steamFriends = "")
	{
		string result = "-";
		if (steamFriends != "")
			result = steamFriends;
		
		m_FriendsResult.SetText(result);
		m_FriendsResult.Update();
	}
	#endif
#endif
	
	protected void SetTimeAcceleration(float timeMul, float nightTimeMul)
	{
		m_TimeAccelerationResult.SetText(timeMul.ToString() + "/" + nightTimeMul.ToString());
	}
	
	void SetTimeIcon(string time, float multiplier)
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
						m_TimeIcon.SetImage(3);
					else
						m_TimeIcon.SetImage(2);
				}
				else							//Day
				{
					if (multiplier > 1)
						m_TimeIcon.SetImage(1);
					else
						m_TimeIcon.SetImage(0);
				}
				
				m_TimeIcon.Show(true);
				m_TimeAccelerationResult.Show(false);
			}
		}
		else
		{
			m_TimeIcon.Show(false);
			m_TimeAccelerationResult.Show(true);
			m_TimeAccelerationResult.SetText("-");
		}
	}
	
	protected void SetBattleyeState(int enabled = -1)
	{
		string result = "#STR_server_browser_no_server_selected";
		int color = ARGB(255, 255, 255, 255);
		
		switch (enabled)
		{
			case 0:
			{
				result = "#server_browser_entry_disabled";
				color = ARGB(255, 255, 0, 0);
				break;
			}
			case 1:
			{
				result = "#server_browser_entry_enabled";
				color = ARGB(255, 0, 255, 0);
				break;
			}
		}
		
		m_BattleyeResult.SetText(result);
		m_BattleyeResult.SetColor(color);
	}
	
	protected void SetVersion(string version)
	{
		m_VersionResult.SetText(version);
	}
	
	protected void SetServerDesc(string desc)
	{
		m_ServerDesc.SetText(desc);
		m_ServerDesc.Update();
	}
	
	protected void SetServerMap(GetServersResultRow serverInfo)
	{
		string mapImagePath = ServerBrowserHelperFunctions.GetServerMapImagePath(serverInfo.m_MapNameToRun);			
		m_MapImage.LoadImageFile(0, mapImagePath);
		m_DetailsContainerRight.Show(true);
	}
	
	void Focus()
	{
		#ifdef PLATFORM_CONSOLE
		if (m_Tab.GetTabType() == TabType.FAVORITE)
		{
			SetFocus(m_RefreshList);
		}
		else
		{
			SetFocus(m_BtnShowFilters);
		}
		#endif
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		m_Tab.OnDetailsFocus(w);
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
}