class ServerBrowserTabConsolePages extends ServerBrowserTab
{
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
		m_Root = GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/server_browser/xbox/server_browser_tab_console_pages.layout", parent);
		
		m_ServerListScroller = ScrollWidget.Cast( m_Root.FindAnyWidget("server_list_scroller"));
		m_ServerList = SpacerBaseWidget.Cast(m_ServerListScroller.FindAnyWidget("server_list_content"));
		m_ServerListScroller.VScrollToPos01(0);
		
		m_ServerListEntiers	= new array<ref ServerBrowserEntry>;
		m_EntryWidgets = new map<string, ref ServerBrowserEntry>;
		m_EntriesSorted	= new map<ESortType, ref array<ref GetServersResultRow>>;
		
		m_EntriesSorted[ESortType.HOST] = new array<ref GetServersResultRow>;
		m_EntriesSorted[ESortType.POPULATION] = new array<ref GetServersResultRow>;
		
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
		
		GetGame().GetMission().GetOnInputDeviceChanged().Insert(OnInputDeviceChanged);

		OnInputDeviceChanged(GetGame().GetInput().GetCurrentInputDevice());

		m_Details = new ServerBrowserDetailsContainer(m_Root.FindAnyWidget("details_content"), this);
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
			ShowHideConsoleWidgets();
			UpdatePageButtons();
			m_WidgetNavFilters.Show(m_IsFilterFocused || m_IsDetailsFocused);
			if (m_TabType != TabType.FAVORITE)
			{
				m_WidgetNavServers.Show((!m_IsFilterFocused && !m_IsDetailsFocused));
			}
			m_BtnShowFilters.Show(false);
			m_BtnShowDetails.Show(false);
			m_RefreshList.Show(false);
			m_Root.FindAnyWidget("spacer2").Show(false);
			m_Root.FindAnyWidget("spacer5").Show(false);
			m_Menu.GetLayoutRoot().FindAnyWidget("play_panel_root").Show(false);
			m_Menu.GetLayoutRoot().FindAnyWidget("MouseAndKeyboardWarning").Show(false);
			m_MouseKeyboardControlled = false;
		break;

		default:
			if (GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer())
			{
				bool isFavoriteTab = m_TabType == TabType.FAVORITE;	
				m_WidgetNavFilters.Show(false);
				m_WidgetNavServers.Show(false);
				m_ButtonPageLeftImg.Show(false);
				m_ButtonPageRightImg.Show(false);
				m_RefreshList.Show(true);
				m_BtnShowDetails.Show(!m_DetailsRoot.IsVisible());
				m_BtnShowFilters.Show(!isFavoriteTab);
				m_Root.FindAnyWidget("spacer").Show(!isFavoriteTab);
				m_Root.FindAnyWidget("spacer2").Show(true);
				m_Root.FindAnyWidget("spacer5").Show(!isFavoriteTab);
				m_Menu.GetLayoutRoot().FindAnyWidget("play_panel_root").Show(true);
				m_Menu.GetLayoutRoot().FindAnyWidget("MouseAndKeyboardWarning").Show(true);
				m_MouseKeyboardControlled = true;
			}
		break;
		}
	}
	
	override void OnLoadServersAsyncConsole( GetServersResult result_list, EBiosError error, string response )
	{
		if ( error != EBiosError.OK )
		{
			m_LoadingText.SetText( string.Format("Error: %1", ErrorModuleHandler.GetClientMessage(ErrorCategory.BIOSError, error)) );
			m_Menu.SetServersLoadingTab( TabType.NONE );
			m_LoadingFinished = true;
			SetFocusFilters();
			return;
		}
				
		m_PagesCount = result_list.m_Pages;
		m_TotalServersCount = result_list.m_NumServers;
		
		LoadEntries( result_list.m_Page, result_list.m_Results );		
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
		RefreshServerList();
	}
	
	void RefreshServerList()
	{
		if (m_TabType != TabType.FAVORITE && (GetGame().GetTime() - m_TimeLastServerRefresh) < 1000)
			return;
			
		m_TimeLastServerRefresh = GetGame().GetTime();
		if (m_IsFilterChanged)
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
		if (!m_IsFilterFocused && !m_IsDetailsFocused)
		{
			int curr_page = GetCurrentPage();
			m_PreviousPage = curr_page;
			if ( curr_page > 1 )
			{
				SetCurrentPage( curr_page - 1 );
				UpdatePageButtons();
			}
		}
	}
	
	override void LeftHold()
	{
		if (!m_IsFilterFocused && !m_IsDetailsFocused)
		{
			if ( (GetGame().GetTime() - m_TimeLastServerRefreshHoldButton) > 100 )
			{
				m_TimeLastServerRefreshHoldButton = GetGame().GetTime();
				Left();
			}
		}		
	}
	
	override void LeftRelease()
	{
		if ( !m_IsFilterFocused && GetCurrentPage() != m_PreviousPage )
		{
			RefreshList();
		}
	}
	
	override void Right()
	{
		if (!m_IsFilterFocused && !m_IsDetailsFocused)
		{
			int curr_page = GetCurrentPage();
			m_PreviousPage = curr_page;
			if ( curr_page < m_PagesCount )
			{
				SetCurrentPage( curr_page + 1 );
				UpdatePageButtons();
			}
		}
	}
	
	override void RightHold()
	{
		if (!m_IsFilterFocused && !m_IsDetailsFocused)
		{
			if ( (GetGame().GetTime() - m_TimeLastServerRefreshHoldButton) > 100 )
			{				
				m_TimeLastServerRefreshHoldButton = GetGame().GetTime();
				Right();
			}
		}
	}
	
	override void RightRelease()
	{
		if ((!m_IsFilterFocused && !m_IsDetailsFocused) && GetCurrentPage() != m_PreviousPage)
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
		TextWidget wgt_page_stat = TextWidget.Cast( m_Root.FindAnyWidget( "servers_navigation_page_status" ) );		
		
		wgt_page_stat.SetText( GetCurrentPage().ToString() +" / "+ m_PagesCount.ToString() );
				
		if ( (!m_IsFilterFocused && !m_IsDetailsFocused) && (m_PagesCount > 1) && !m_MouseKeyboardControlled )
		{
			bool can_left = (GetCurrentPage() > 1);
			m_ButtonPageLeftImg.Show( can_left );
			m_BtnPagePrev.Show( can_left );
			
			bool can_right = (GetCurrentPage() < m_PagesCount);
			m_ButtonPageRightImg.Show( can_right );
			m_BtnPageNext.Show( can_right );
		}
		else
		{
			m_ButtonPageLeftImg.Show( false );
			m_BtnPagePrev.Show( false );
			m_ButtonPageRightImg.Show( false );
			m_BtnPageNext.Show( false );
		}
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
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
				Left();
				return true;
			}
			else if (w == m_BtnPageNext)
			{
				Right();
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
}