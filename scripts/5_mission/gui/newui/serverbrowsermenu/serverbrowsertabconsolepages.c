class ServerBrowserTabConsolePages extends ServerBrowserTab
{
	protected const int SERVERS_VISIBLE_COUNT = 22;
	
	private bool m_IsFilterChanged;
	private bool m_IsFilterFocused;
	protected bool m_MouseKeyboardControlled
	
	private Widget m_WidgetNavFilters;
	private Widget m_WidgetNavServers;
	
	protected Widget m_ButtonPageLeftImg;
	protected Widget m_ButtonPageRightImg;
	
	protected int m_PreviousPage;
	protected int m_TotalServersCount;
	protected int m_PageStartNum;
	protected int m_PageEndNum;
	protected int m_PagesCount;
	protected int m_ServersEstimateCount;
	protected int m_TimeLastServerRefresh;
	protected int m_TimeLastServerRefreshHoldButton;
	
	protected Widget m_PnlPagesPanel;
	protected TextWidget m_PnlPagesLoadingText;
	protected ref array<ref ServerBrowserEntry> m_ServerListEntiers;
	
	private bool m_IsDetailsFocused;
	
	protected override void Construct(Widget parent, ServerBrowserMenuNew menu, TabType type)
	{
		m_Root = g_Game.GetWorkspace().CreateWidgets("gui/layouts/new_ui/server_browser/xbox/server_browser_tab_console_pages.layout", parent);
		
		m_ServerListScroller = ScrollWidget.Cast( m_Root.FindAnyWidget("server_list_scroller"));
		m_ServerList = SpacerBaseWidget.Cast(m_ServerListScroller.FindAnyWidget("server_list_content"));
		m_ServerListScroller.VScrollToPos01(0);
		
		m_ServerListEntiers	= new array<ref ServerBrowserEntry>();
		m_EntryWidgets = new map<string, ref ServerBrowserEntry>();
		m_EntriesSorted	= new map<ESortType, ref array<ref GetServersResultRow>>();
		
		m_EntriesSorted[ESortType.HOST] = new array<ref GetServersResultRow>();
		m_EntriesSorted[ESortType.POPULATION] = new array<ref GetServersResultRow>();
		
		m_Menu = menu;
		m_TabType = type;

		m_FilterRoot = m_Root.FindAnyWidget("filters_root");
		m_FilterContent = m_Root.FindAnyWidget("filters_content");
		m_ApplyFilter = m_Root.FindAnyWidget("apply_filter_button");
		m_ResetFilters = m_Root.FindAnyWidget("reset_filter_button");
		m_RefreshList = m_Root.FindAnyWidget("refresh_list_button");
		m_FiltersChanged = m_Root.FindAnyWidget("unapplied_filters_notify");
		m_HostSort = m_Root.FindAnyWidget("server_list_content_header_host");
		m_TimeSort = m_Root.FindAnyWidget("server_list_content_header_time");
		m_PopulationSort = m_Root.FindAnyWidget("server_list_content_header_population");
		m_SlotsSort	= m_Root.FindAnyWidget("server_list_content_header_slots");
		m_PingSort = m_Root.FindAnyWidget("server_list_content_header_ping");
		m_MapSort = m_Root.FindAnyWidget("server_list_content_header_map");
		m_FilterSearchText = m_Root.FindAnyWidget("search_name_button");
		m_FilterSearchTextBox = m_Root.FindAnyWidget("search_name_setting_option");
		m_LoadingText = TextWidget.Cast( m_Root.FindAnyWidget("loading_servers_info"));
		m_WidgetNavFilters = m_Root.FindAnyWidget("filters_root_nav_wrapper");
		m_WidgetNavServers = m_Root.FindAnyWidget("server_list_root_nav_wrapper");
				
		#ifdef PLATFORM_MSSTORE
		// Find M&K filter and disable it
		Widget keyboard_button = m_Root.FindAnyWidget("keyboard_button");
		if (keyboard_button)
			keyboard_button.Show(false);
		#endif
		
		m_BtnPagePrev = ButtonWidget.Cast(m_Root.FindAnyWidget("servers_navigation_prev"));
		m_BtnPageNext = ButtonWidget.Cast(m_Root.FindAnyWidget("servers_navigation_next"));
		
		#ifdef PLATFORM_PS4
		m_ButtonPageLeftImg = m_Root.FindAnyWidget("servers_navigation_page_prev_icon_ps4");
		m_ButtonPageRightImg = m_Root.FindAnyWidget("servers_navigation_page_next_icon_ps4");
		m_Root.FindAnyWidget("servers_navigation_page_prev_icon_xbox").Show(false);
		m_Root.FindAnyWidget("servers_navigation_page_next_icon_xbox").Show(false);
		#else
		m_ButtonPageLeftImg = m_Root.FindAnyWidget("servers_navigation_page_prev_icon_xbox");
		m_ButtonPageRightImg = m_Root.FindAnyWidget("servers_navigation_page_next_icon_xbox");
		m_Root.FindAnyWidget("servers_navigation_page_prev_icon_ps4" ).Show(false);
		m_Root.FindAnyWidget("servers_navigation_page_next_icon_ps4" ).Show(false);
		#endif
		
		m_RightAreaHeaderText = TextWidget.Cast(m_Root.FindAnyWidget("right_area_header_text"));
		m_FilterRoot = m_Root.FindAnyWidget("filters_root");
		m_BtnShowDetails = ButtonWidget.Cast(m_Root.FindAnyWidget("show_details_button"));
		
		m_DetailsRoot = m_Root.FindAnyWidget("details_root");
		m_BtnShowFilters = ButtonWidget.Cast(m_Root.FindAnyWidget("show_filters_button"));
		
		ShowHideConsoleWidgets();		
		
		m_Filters = new ServerBrowserFilterContainer(m_Root.FindAnyWidget("filters_content"), this);
		
		m_PnlPagesPanel = m_Root.FindAnyWidget( "servers_navigation_spacer" );
		m_PreviousPage = 1;
		SetCurrentPage(1);
		m_PnlPagesPanel.Show(true);
		m_LoadingFinished = true;
		
		m_Filters.OnSortChanged(m_Filters.m_SortingFilter.GetValue());
		UpdatePageButtons();
		
		m_Root.SetHandler(this);
		m_FilterSearchTextBox.SetHandler(this);
		
		g_Game.GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);

		OnInputDeviceChanged(g_Game.GetInput().GetCurrentInputDevice());

		m_Details = new ServerBrowserDetailsContainer(m_Root.FindAnyWidget("details_content"), this);
		
	#ifdef DIAG_DEVELOPER
		InitDummyServers();
	#endif
		
	}
	
	void ShowHideConsoleWidgets()
	{
		bool is_xbox = true;
		
		#ifdef PLATFORM_PS4
		is_xbox = false;
		#endif
		
		m_Root.FindAnyWidget( "filters_root_nav_img_lb_xbox" ).Show( is_xbox );
		m_Root.FindAnyWidget( "filters_root_nav_img_rb_xbox" ).Show( is_xbox );
		m_Root.FindAnyWidget( "server_list_root_nav_img_lb_xbox" ).Show( is_xbox );
		m_Root.FindAnyWidget( "server_list_root_nav_img_rb_xbox" ).Show( is_xbox );
		
		m_Root.FindAnyWidget( "filters_root_nav_img_l1_ps4" ).Show( !is_xbox );
		m_Root.FindAnyWidget( "filters_root_nav_img_r1_ps4" ).Show( !is_xbox );
		m_Root.FindAnyWidget( "server_list_root_nav_img_l1_ps4" ).Show( !is_xbox );
		m_Root.FindAnyWidget( "server_list_root_nav_img_r1_ps4" ).Show( !is_xbox );
	}

	protected void OnInputDeviceChanged(EInputDeviceType pInputDeviceType)
	{
		switch (pInputDeviceType)
		{
			case EInputDeviceType.CONTROLLER:
			{
				m_MouseKeyboardControlled = false;
	
				ShowHideConsoleWidgets();
	
				m_WidgetNavFilters.Show(m_IsFilterFocused || m_IsDetailsFocused);
				if (m_TabType != TabType.FAVORITE)
				{
					m_WidgetNavServers.Show(!m_IsFilterFocused && !m_IsDetailsFocused);
				}
	
				m_BtnShowFilters.Show(false);
				m_BtnShowDetails.Show(false);
				m_RefreshList.Show(false);
				m_Root.FindAnyWidget("spacer2").Show(false);
				m_Root.FindAnyWidget("spacer5").Show(false);
				m_Menu.GetLayoutRoot().FindAnyWidget("play_panel_root").Show(false);
				m_Menu.GetLayoutRoot().FindAnyWidget("MouseAndKeyboardWarning").Show(false);
	
				UpdatePageButtons();
				break;
			}
	
			default:
			{
				if (g_Game.GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
				{
					bool isFavoriteTab = m_TabType == TabType.FAVORITE;
	
					m_MouseKeyboardControlled = true;
	
					m_WidgetNavFilters.Show(false);
					m_WidgetNavServers.Show(false);
					m_RefreshList.Show(true);
					m_BtnShowDetails.Show(!m_DetailsRoot.IsVisible());
					m_BtnShowFilters.Show(!isFavoriteTab);
					m_Root.FindAnyWidget("spacer").Show(!isFavoriteTab);
					m_Root.FindAnyWidget("spacer2").Show(true);
					m_Root.FindAnyWidget("spacer5").Show(!isFavoriteTab);
					m_Menu.GetLayoutRoot().FindAnyWidget("play_panel_root").Show(true);
					m_Menu.GetLayoutRoot().FindAnyWidget("MouseAndKeyboardWarning").Show(true);
	
					UpdatePageButtons();
				}
				break;
			}
		}
	}
	
	override void OnLoadServersAsyncConsole( GetServersResult result_list, EBiosError error, string response )
	{
	#ifdef DIAG_DEVELOPER
		if (m_DummyServersEnabled)
			return;
	#endif
		
		if ( error != EBiosError.OK )
		{
			m_LoadingText.SetText( string.Format("Error: %1", ErrorModuleHandler.GetClientMessage(ErrorCategory.BIOSError, error)) );
			m_Menu.SetServersLoadingTab( TabType.NONE );
			m_LoadingFinished = true;
			SetFocusFilters();
			return;
		}
				
		m_PagesCount = result_list.m_Pages;
		if (m_PagesCount < 1)
		    m_PagesCount = 1;
		
		m_TotalServersCount = result_list.m_NumServers;
	
	#ifdef DIAG_DEVELOPER
		if (m_DummyServersEnabled)
		{
			m_PagesCount = m_DummyServers.m_Pages;
			if (m_PagesCount < 1)
			    m_PagesCount = 1;
			
			m_TotalServersCount = m_DummyServers.m_NumServers;
		
			int startIndex = (GetCurrentPage() - 1) * SERVERS_VISIBLE_COUNT;
			int endIndex = startIndex + SERVERS_VISIBLE_COUNT;
		
			int resultCount = m_DummyServers.m_Results.Count();
			if (endIndex > resultCount)
			{
				endIndex = resultCount;
			}
		
			GetServersResultRowArray tempDummyServerResult = new GetServersResultRowArray();
			for (int i = startIndex; i < endIndex; ++i)
			{
				tempDummyServerResult.Insert(m_DummyServers.m_Results[i]);
			}
		
			LoadEntries(GetCurrentPage(), tempDummyServerResult);
		}
		else
		{
			LoadEntries(result_list.m_Page, result_list.m_Results);
		}
	#else
		LoadEntries(result_list.m_Page, result_list.m_Results);
	#endif
		
		OnLoadServersAsyncFinished();
		UpdatePageButtons();
	}
	
	void OnLoadServersAsyncFinished()
	{
		string msg = "#servers_found: " + m_TotalLoadedServers;
		if (m_TotalLoadedServers == 0)
		{
			
			msg = "#server_browser_tab_unable_to_get_server";
		}

		m_LoadingText.SetText(msg);
		m_LoadingFinished = true;
		m_Menu.SetServersLoadingTab(TabType.NONE);
		
		if (m_ServerListEntiers && m_ServerListEntiers.Count() && m_SelectedPanel == SelectedPanel.BROWSER)
		{
			ServerBrowserEntry entry = m_ServerListEntiers[0];
			if (entry)
			{
				entry.Darken(entry.GetRoot(), 0, 0);
				entry.Select();
				SetFocus(entry.GetRoot());
			}
		}
	}
	
 	protected void LoadEntries( int cur_page_index , GetServersResultRowArray page_entries )
	{
		int index = 0;
		ServerBrowserEntry entry;
		m_TotalLoadedServers = m_TotalServersCount;
		
		if ( !m_Menu || m_Menu.GetServersLoadingTab() != m_TabType )
		{
			return;
		}
		
		// only load servers if they correspond to current page
		if (cur_page_index != m_CurrentPageNum)
		{
			return;
		}
		
		if ( m_PagesCount && page_entries )
		{
			foreach ( GetServersResultRow result : page_entries )
			{
				if ( PassFilter( result ) )
				{
					string ipPort = result.GetIpPort();
					bool isFavorited = m_Menu.IsFavorited(ipPort);
					
					entry = GetServerEntryByIndex( index );
					entry.SetIsOnline(true);
					entry.FillInfo( result );
					entry.SetFavorite(isFavorited);
					entry.UpdateEntry();
					
					if (isFavorited && m_OnlineFavServers.Find(ipPort) == -1)
					{
						m_OnlineFavServers.Insert(ipPort);
					}
										
					m_EntryWidgets.Insert(ipPort, entry);
					m_EntriesSorted[m_SortType].Insert( result );
					
					index++;
				}
				
				if ( !m_Menu || m_Menu.GetServersLoadingTab() != m_TabType )
				{
					return;
				}
			}
		}
		
		LoadExtraEntries(index);
		m_ServerList.Update();
	}
	
	override bool PassFilter(GetServersResultRow result)
	{
		if (m_Filters.m_PreviouslyPlayedFilter.IsSet())
        {
            bool is_visited = g_Game.IsVisited(result.m_HostIp, result.m_HostPort);
            if (!is_visited && m_Filters.m_PreviouslyPlayedFilter.IsEnabled())
                return false;
			
            if (is_visited && !m_Filters.m_PreviouslyPlayedFilter.IsEnabled())
                return false;
        }
		
		return super.PassFilter(result);
	}
	
	protected ServerBrowserEntry GetServerEntryByIndex( int index )
	{
		ServerBrowserEntry entry;
		
		if ( index >= 0 )
		{
			if ( index < m_ServerListEntiers.Count() )
			{
				entry = m_ServerListEntiers[index];
			}
			else
			{
				entry = new ServerBrowserEntry( m_ServerList, index, this );				
				m_ServerListEntiers.Insert(entry);
			}
		}
		else
		{
			return null;
		}
		
		entry.Show( true );
		
		return entry;
	}
	
	override void OnFilterChanged()
	{
		m_IsFilterChanged = true;
	}
	
	bool CanRefreshServerList()
	{	
		return m_LoadingFinished;
	}
	
	override void RefreshList()
	{
	#ifdef DIAG_DEVELOPER
		if (m_DummyServersEnabled)
		{
			RefreshDummyList();
			return;
		}
	#endif
		
		for ( int i = 0; i < m_ServerListEntiers.Count(); i++ )
		{
			m_ServerListEntiers[i].Show(false);
		}
		
		for ( int j = 0; j < m_EntriesSorted.Count(); j++ )
		{
			array<ref GetServersResultRow> result_rows = m_EntriesSorted.GetElement(j);
			
			if ( result_rows )
			{
				result_rows.Clear();
			}
		}
		
		m_IsFilterChanged = false;
		m_Filters.SaveFilters();
		
		super.RefreshList();
		
		m_ServerListScroller.VScrollToPos01( 0 );
		
		m_LoadingText.SetText( "#dayz_game_loading" );
	}
	
	override void ResetFilters()
	{
		super.ResetFilters();
		
		m_IsFilterChanged = false;
	}
	
	override void ApplyFilters()
	{
		if ( m_IsFilterChanged )
		{
			SetCurrentPage(1);
		}
		
		m_IsFilterChanged = false;
		m_CurrentFilterInput = m_Filters.GetFilterOptionsConsoles();
		RefreshList();
	}
	
	override void PressA()
	{
		
	}
	
	override void PressX()
	{
	#ifdef DIAG_DEVELOPER
		RefreshServerList(!m_DummyServersEnabled);
	#else
		RefreshServerList();
	#endif
	}
	
#ifdef DIAG_DEVELOPER
	void RefreshServerList(bool forcePageChange = false)
#else
	void RefreshServerList()
#endif
	{
		int currentTime = g_Game.GetTime();
	#ifdef DIAG_DEVELOPER
		if (!forcePageChange && m_TabType != TabType.FAVORITE && (currentTime - m_TimeLastServerRefresh) < 1000)
	#else
		if (m_TabType != TabType.FAVORITE && (currentTime - m_TimeLastServerRefresh) < 1000)
	#endif
			return;
			
		m_TimeLastServerRefresh = currentTime;
	#ifdef DIAG_DEVELOPER
		if (m_IsFilterChanged || forcePageChange)
	#else
		if (m_IsFilterChanged)
	#endif
		{
			SetCurrentPage(1);
		}
		m_OnlineFavServers.Clear();
		RefreshList();
	}
	
	override void PressY()
	{
		SwitchRightAreaView();
	}
	
	override void PressThumbRight()
	{
		switch (m_SelectedPanel)
		{
			case SelectedPanel.BROWSER:
			{
				if (m_SelectedServer)
				{
					m_Menu.ServerListFocus(true, m_SelectedServer.ToggleFavorite());
				}
				break;
			}
			case SelectedPanel.FILTERS:
			{
				if (m_Filters)
				{
					m_Filters.ResetFilters();
				}
				break;
			}
		}
	}
	
	override void Left()
	{
		if (CanNavigatePages())
		{
			int currPage = GetCurrentPage();
			m_PreviousPage = currPage;
			if ( currPage > 1 )
			{
				SetCurrentPage( currPage - 1 );
				UpdatePageButtons();
			}
		}
	}
	
	override void LeftHold()
	{
		if (CanNavigatePages())
		{
			int currentTime = g_Game.GetTime();
			if ( (currentTime - m_TimeLastServerRefreshHoldButton) > 100 )
			{
				m_TimeLastServerRefreshHoldButton = currentTime;
				Left();
			}
		}		
	}
	
	override void LeftRelease()
	{
		if ( CanNavigatePages() && GetCurrentPage() != m_PreviousPage )
		{
			RefreshList();
		}
	}
	
	override void Right()
	{
		if (CanNavigatePages())
		{
			int currPage = GetCurrentPage();
			m_PreviousPage = currPage;
			if (currPage < m_PagesCount)
			{
				SetCurrentPage(currPage + 1);
				UpdatePageButtons();
			}
		}
	}
	
	override void RightHold()
	{
		if (CanNavigatePages())
		{
			int currentTime = g_Game.GetTime();
			if ( (currentTime - m_TimeLastServerRefreshHoldButton) > 100 )
			{				
				m_TimeLastServerRefreshHoldButton = currentTime;
				Right();
			}
		}
	}
	
	override void RightRelease()
	{
		if (CanNavigatePages() && GetCurrentPage() != m_PreviousPage)
		{
			RefreshList();
		}
	}
	
	override void PressSholderLeft()
	{
		OnPressShoulder();
	}
	
	override void PressSholderRight()
	{
		OnPressShoulder();
	}
	
	override void OnPressShoulder()
	{
		if (m_TabType == TabType.FAVORITE)
			return;

		switch (m_SelectedPanel)
		{
			case SelectedPanel.BROWSER:
			{
				if (m_TabType == TabType.FAVORITE)
				{
					SwitchToDetails();
				}
				else
				{
					SwitchToFilters();
				}
				break;
			}
			case SelectedPanel.DETAILS:
			case SelectedPanel.FILTERS:
			{
				SetFocusServers();
				break;
			}
		}
	}

	override void Focus()
	{
		if (m_TabType == TabType.FAVORITE)
		{
			SetFocusServers();
		}
		else
		{
			SwitchToFilters(true);
		}
	}
	
	override void ShowServerDetails()
	{
		super.ShowServerDetails();
		
		if (m_TabType != TabType.FAVORITE)
		{
			m_FilterRoot.Show(false);
			m_BtnShowFilters.Show(m_MouseKeyboardControlled);
		}
	}
	
	void SetFocusFilters()
	{
		SetEnableFilters(true);
		SetEnableServers(false);
		
		// if loaded servers is 0, then hide Top navigation menu <Left / Right>
		if (!m_MouseKeyboardControlled)
		{
			m_WidgetNavFilters.Show(true);
			m_WidgetNavServers.Show(false);
		}
		
		m_Filters.Focus();
		m_IsFilterFocused = true;
		m_IsDetailsFocused = false;
		m_Menu.ShowAButton(true);
		
		UpdatePageButtons();
		m_SelectedPanel = SelectedPanel.FILTERS;
	}

	void SetFocusDetails()
	{
		SetEnableFilters(false);
		SetEnableServers(false);
			
		if (!m_MouseKeyboardControlled)
		{
			m_WidgetNavFilters.Show(true);
			m_WidgetNavServers.Show(false);
		}
		
		m_Details.Focus();
		m_IsDetailsFocused = true;
		m_IsFilterFocused = false;		
	
		UpdatePageButtons();
	
		m_SelectedPanel = SelectedPanel.DETAILS;
	}
	
	void SetFocusServers()
	{
		SetEnableServers(true);
		SetEnableFilters(false);
		
		if (!m_MouseKeyboardControlled)
		{
			m_WidgetNavFilters.Show(false);
			if (m_TabType != TabType.FAVORITE)
			{
				m_WidgetNavServers.Show(true);
			}
		}

		array<ref GetServersResultRow> entries = m_EntriesSorted[m_SortType];
		if (entries && entries.Count() > 0)
		{
			string entryKey = entries.Get(0).GetIpPort();
			ServerBrowserEntry sbEntry = m_EntryWidgets.Get(entryKey);
			if (sbEntry)
			{
				sbEntry.Focus();
			}
			
			m_Menu.ShowAButton(true);
			m_Menu.UpdateAButtonLabel("#str_serverbrowserroot_toolbar_bg_consoletoolbar_connect_connecttext0");
			m_Menu.ShowThumbRButton(true);
		}
		else
		{
			SetFocus(null);
			m_Menu.ShowAButton(false);
			m_Menu.ShowThumbRButton(false);
		}

		m_IsFilterFocused = false;
		m_IsDetailsFocused = false;
		
		UpdatePageButtons();
		m_SelectedPanel = SelectedPanel.BROWSER;
	}
	
	void SetEnableFilters(bool enable)
	{
		Widget w_filters = m_Root.FindAnyWidget("filters_content");
		Widget w_content = m_Root.FindAnyWidget("server_list_scroller");
		w_content.Enable( !enable );
		
		if ( enable )
		{
			SetFocus( ButtonWidget.Cast(w_filters.FindWidget("sort_button")) );
		}
	}
	
	void SetEnableServers(bool enable)
	{
		Widget w_filters = m_Root.FindAnyWidget("filters_content");
		Widget w_content = m_Root.FindAnyWidget("server_list_scroller");
		w_content.Enable( enable );
	}
	
	void UpdatePageButtons()
	{
		TextWidget wgtPageStat = TextWidget.Cast(m_Root.FindAnyWidget("servers_navigation_page_status"));
	
		int pagesCount = GetPagesCount();
		int currentPage = GetCurrentPage();
	
		if (currentPage < 1)
			currentPage = 1;

		if (currentPage > pagesCount)
			currentPage = pagesCount;

		wgtPageStat.SetText(currentPage.ToString() + " / " + pagesCount.ToString());
	
		bool canLeft = currentPage > 1;
		bool canRight = currentPage < pagesCount;
	
		bool showMouseKeyboardPaging = m_MouseKeyboardControlled && (pagesCount > 1);
		bool showControllerPaging = !m_MouseKeyboardControlled && !m_IsFilterFocused && !m_IsDetailsFocused && (pagesCount > 1);
	
		m_BtnPagePrev.Show(showMouseKeyboardPaging && canLeft);
		m_BtnPageNext.Show(showMouseKeyboardPaging && canRight);
	
		m_ButtonPageLeftImg.Show(showControllerPaging && canLeft);
		m_ButtonPageRightImg.Show(showControllerPaging && canRight);
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		int oldPage;
		if (button == MouseState.LEFT)
		{
			if (w == m_ResetFilters)
			{
				ResetFilters();
			}
			else if (w == m_ApplyFilter)
			{
				ApplyFilters();
				return true;
			}
			else if (w == m_RefreshList)
			{
				if (m_Loading && !m_LoadingFinished)
				{
					PressX();
				}
				else
				{
					RefreshList();
				}
				
				return true;
			}
			else if (w == m_BtnPagePrev)
			{
				oldPage = GetCurrentPage();
				Left();
			
				if (GetCurrentPage() != oldPage)
					RefreshList();
				
				return true;
			}
			else if (w == m_BtnPageNext)
			{
				oldPage = GetCurrentPage();
				Right();
			
				if (GetCurrentPage() != oldPage)
					RefreshList();
				
				return true;
			}
			else if (w == m_BtnShowDetails)
			{
				SwitchToDetails();
				return true;
			}
			else if (w == m_BtnShowFilters)
			{
				SwitchToFilters();
				return true;
			}
		}
		return false;
	}
	
	//Coloring functions (Until WidgetStyles are useful)
	override void ColorHighlight( Widget w )
	{
		if ( !w )
		{
			return;
		}
		
		w.SetColor( ARGB( 255, 200, 0, 0) );
	}
	
	override void ColorNormal( Widget w )
	{
		if ( !w )
		{
			return
		}
		
		if ( (w.GetFlags() & WidgetFlags.IGNOREPOINTER) == WidgetFlags.IGNOREPOINTER )
		{
			return;
		}
		
		if ( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			button.SetTextColor( ARGB( 255, 255, 255, 255 ) );
		}
		else if ( !w.IsInherited( EditBoxWidget ) )
		{
			w.SetColor( ARGB( 0, 255, 255, 255 ) );
		}
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text_1" ) );
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_label" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindAnyWidget( w.GetName() + "_image" ) );
		
		Widget option	= Widget.Cast( w.FindAnyWidget( w.GetName() + "_option_wrapper" ) );
		
		if ( text1 )
		{
			text1.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if ( text2 )
		{
			text2.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if ( text3 )
		{
			text3.SetColor( ARGB( 255, 255, 255, 255 ) );
			w.SetAlpha(0);
		}
		
		if ( image )
		{
			image.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if ( option )
		{
			option.SetColor( ARGB( 150, 255, 255, 255 ) );
		}
	}
	
	override void ColorDisable( Widget w )
	{
		#ifdef PLATFORM_WINDOWS
		SetFocus( null );
		#endif
		
		ButtonWidget button = ButtonWidget.Cast( w );
		if ( button )
		{
			button.SetTextColor( ColorManager.COLOR_DISABLED_TEXT );
		}
	}
	
	override void SwitchToDetails(bool focus = true)
	{
		ShowServerDetails();
		
		if (focus)
			SetFocusDetails();

		m_Menu.UpdateYButtonLabel("#server_browser_tab_server #STR_server_browser_menu_server_filters");
		m_RightAreaHeaderText.SetText("#STR_server_browser_menu_server_details");
	}
	
	override void SwitchToFilters(bool focus = true)
	{
		if (m_TabType == TabType.FAVORITE)
			return;

		m_FilterRoot.Show(true);
		m_BtnShowDetails.Show(m_MouseKeyboardControlled);
		m_DetailsRoot.Show(false);
		
		if (focus)
			SetFocusFilters();

		m_Menu.UpdateYButtonLabel("#server_details_header");
		m_RightAreaHeaderText.SetText("#STR_server_browser_menu_server_filters");
	}
	
	protected bool CanNavigatePages()
	{
		if (m_MouseKeyboardControlled)
		{
			return true;
		}
	
		return !m_IsFilterFocused && !m_IsDetailsFocused;
	}
	
	protected int GetPagesCount()
	{
		if (m_PagesCount < 1)
	    {
	        return 1;
	    }
	
	    return m_PagesCount;
	}
	
#ifdef DIAG_DEVELOPER
	protected void RefreshDummyList()
	{
		for (int i = 0; i < m_ServerListEntiers.Count(); ++i)
		{
			m_ServerListEntiers[i].Show(false);
		}
	
		for (int j = 0; j < m_EntriesSorted.Count(); ++j)
		{
			array<ref GetServersResultRow> resultRows = m_EntriesSorted.GetElement(j);
			if (resultRows)
			{
				resultRows.Clear();
			}
		}
	
		m_EntryWidgets.Clear();
		m_OnlineFavServers.Clear();
	
		m_IsFilterChanged = false;
		m_Filters.SaveFilters();
	
		m_Menu.SetServersLoadingTab(m_TabType);
		m_LoadingFinished = false;
		m_Initialized = true;
		m_BegunLoading = false;
		m_LastLoadedPage = 0;
		m_TotalPages = -1;
		m_TotalServers = 0;
		m_TotalLoadedServers = 0;
		m_CurrentLoadedPage = 0;
		m_Loading = true;
	
		m_Menu.DeselectCurrentServer();
	
		m_CurrentFilterInput = m_Filters.GetFilterOptionsConsoles();
		m_CurrentFilterInput.m_Page = GetCurrentPage();
		m_CurrentFilterInput.m_SortBy = GetSortOption();
		m_CurrentFilterInput.m_SortOrder = m_SortOrder;
	
		m_ServerListScroller.VScrollToPos01(0);
		m_LoadingText.SetText("#dayz_game_loading");
	
		GetServersResultRowArray filteredResults = new GetServersResultRowArray();
	
		if (m_DummyServers && m_DummyServers.m_Results)
		{
			for (int k = 0; k < m_DummyServers.m_Results.Count(); ++k)
			{
				GetServersResultRow row = m_DummyServers.m_Results[k];
				if (PassLocalFilters(row))
				{
					filteredResults.Insert(row);
				}
			}
		}
	
		m_TotalServersCount = filteredResults.Count();
		m_PagesCount = (m_TotalServersCount + SERVERS_VISIBLE_COUNT - 1) / SERVERS_VISIBLE_COUNT;
		if (m_PagesCount < 1)
		{
			m_PagesCount = 1;
		}
	
		int safePage = GetCurrentPage();
		if (safePage < 1)
		{
			safePage = 1;
		}
		if (safePage > m_PagesCount)
		{
			safePage = m_PagesCount;
		}
		SetCurrentPage(safePage);
	
		int startIndex = (safePage - 1) * SERVERS_VISIBLE_COUNT;
		int endIndex = Math.Min(startIndex + SERVERS_VISIBLE_COUNT, filteredResults.Count());
	
		GetServersResultRowArray pageResults = new GetServersResultRowArray();
		for (int i2 = startIndex; i2 < endIndex; ++i2)
		{
			pageResults.Insert(filteredResults[i2]);
		}
	
		LoadEntries(safePage, pageResults);
		OnLoadServersAsyncFinished();
		UpdatePageButtons();
	}
	
	override void OnDummyServersEnabledChanged()
	{
		super.OnDummyServersEnabledChanged();
	
		SetCurrentPage(1);
		m_PreviousPage = 1;
		m_OnlineFavServers.Clear();
	
		RefreshServerList(!m_DummyServersEnabled);
	}
	
	override int GetServersVisibleCount()
	{
		return SERVERS_VISIBLE_COUNT;
	}
#endif
}