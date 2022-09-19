class ServerBrowserTabConsole extends ServerBrowserTab
{
	private bool m_IsFilterChanged;
	private bool m_IsFilterFocused;
	
	private Widget m_WidgetNavFilters;
	private Widget m_WidgetNavServers;
	
	protected override void Construct( Widget parent, ServerBrowserMenuNew menu, TabType type )
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/server_browser/xbox/server_browser_tab.layout", parent );
		
		m_ServerListScroller	= ScrollWidget.Cast( m_Root.FindAnyWidget( "server_list_scroller" ) );
		m_ServerList			= SpacerBaseWidget.Cast( m_ServerListScroller.FindAnyWidget( "server_list_content" ) );
		m_ServerListScroller.VScrollToPos01( 0 );
		
		m_EntryWidgets			= new map<string, ref ServerBrowserEntry>;
		m_EntriesSorted			= new map<ESortType, ref array<ref GetServersResultRow>>;
		
		m_EntriesSorted[ESortType.HOST] = new array<ref GetServersResultRow>;
		m_EntriesSorted[ESortType.POPULATION] = new array<ref GetServersResultRow>;
		
		m_Menu					= menu;
		m_TabType				= type;
		
		m_ApplyFilter			= m_Root.FindAnyWidget( "apply_filter_button" );
		m_RefreshList			= m_Root.FindAnyWidget( "refresh_list_button" );
		m_FiltersChanged		= m_Root.FindAnyWidget( "unapplied_filters_notify" );
		m_HostSort				= m_Root.FindAnyWidget( "server_list_content_header_host" );
		m_TimeSort				= m_Root.FindAnyWidget( "server_list_content_header_time" );
		m_PopulationSort		= m_Root.FindAnyWidget( "server_list_content_header_population" );
		m_SlotsSort				= m_Root.FindAnyWidget( "server_list_content_header_slots" );
		m_PingSort				= m_Root.FindAnyWidget( "server_list_content_header_ping" );
		m_LoadingText			= TextWidget.Cast( m_Root.FindAnyWidget( "loading_servers_info" ) );
		m_WidgetNavFilters		= m_Root.FindAnyWidget( "filters_root_nav_wrapper" );
		m_WidgetNavServers		= m_Root.FindAnyWidget( "server_list_root_nav_wrapper" );
		
		ShowHideConsoleWidgets();		
		
		m_Filters				= new ServerBrowserFilterContainer( m_Root.FindAnyWidget( "filters_content" ), this );
		
		SetSort( ESortType.HOST, ESortOrder.ASCENDING );
		SetFocusFilters();
		
		m_Root.SetHandler( this );
	}
	
	void ShowHideConsoleWidgets()
	{		
#ifdef PLATFORM_PS4
		bool is_xbox = false;
#else
		bool is_xbox = true;
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
	
	override void OnLoadServersAsyncConsole( GetServersResult result_list, EBiosError error, string response )
	{
		//Print("m_NumServers: "+ result_list.m_NumServers +" m_Pages: "+ result_list.m_Pages + " m_Page: "+ result_list.m_Page);
		//string smg = "m_NumServers: "+ result_list.m_NumServers +" m_Pages: "+ result_list.m_Pages + " m_Page: "+ result_list.m_Page +" response: "+ response +" error: "+ error;
		//m_LoadingText.SetText( smg );
		
		if ( error != EBiosError.OK || !result_list )
		{
			m_LoadingText.SetText( string.Format("Error: %1", ErrorModuleHandler.GetClientMessage(ErrorCategory.BIOSError, error)) );
			return;
		}
		
		if ( result_list.m_Page == 1 )
		{
			m_TotalPages = result_list.m_Pages;
			m_TotalServers = result_list.m_NumServers;
					
			if ( m_Menu.GetServersLoadingTab() != m_TabType || !result_list || ( !result_list.m_Results || result_list.m_Results.Count() == 0 ) )
			{
				m_Menu.SetServersLoadingTab( TabType.NONE );				
				m_LoadingText.SetText("#server_browser_tab_unable_to_get_server #server_browser_tab_no_servers_with_filter");
				return;
			}
		}
		
		m_LastLoadedPage = result_list.m_Page;
		
		if ( m_TotalPages > 0 )
		{
			LoadEntries( result_list.m_Page, result_list.m_Results );
		}
		
		if ( m_TotalPages > m_LastLoadedPage )
		{
			GetNextPage();
			return;
		}
		
		if ( result_list.m_Page == result_list.m_Pages )
		{
			OnLoadServersAsyncFinished();
		}
	}
	
	void OnLoadServersAsyncFinished()
	{
		string msg = "#servers_found: "+ m_TotalLoadedServers;
		if ( m_TotalLoadedServers == 0 )
		{
			msg = "#server_browser_tab_unable_to_get_server";
		}
		m_LoadingText.SetText( msg );
		m_LoadingFinished = true;
	}
	
	void LoadEntries( int cur_page_index , GetServersResultRowArray page )
	{
		if( !m_Menu || m_Menu.GetServersLoadingTab() != m_TabType )
		{
			return;
		}
		
		int index = cur_page_index * SERVER_BROWSER_PAGE_SIZE;
		ref GetServersResultRowArray page_entries = page;
		if( page_entries )
		{
			foreach( GetServersResultRow result : page_entries )
			{
				m_TotalLoadedServers++;
				
				if( PassFilter( result ) )
				{
					ref ServerBrowserEntry entry = new ServerBrowserEntry( m_ServerList, index, this );
					string ipPort = result.GetIpPort();
					entry.FillInfo( result );
					entry.SetFavorite( m_Menu.IsFavorited(ipPort));
					entry.UpdateEntry();
					
					m_EntryWidgets.Insert(ipPort, entry);
					index++;					
					m_EntriesSorted[m_SortType].Insert( result );
					
					if ( m_EntryWidgets.Count() > 0 )
					{
						SetFocusServers();
					}
				}
				
				if( !m_Menu || m_Menu.GetServersLoadingTab() != m_TabType )
					return;
			}
			
			m_ServerList.Update();
		}
		/*
		if ( cur_page_index == 1 && m_EntryWidgets.Count() > 0 )
		{
			SetFocusServers();
		}
		else
		{
			m_LoadingText.SetText( "#server_browser_unable_with_filter" );
			SetFocusFilters();
		}
		*/
		
		if( m_EntriesSorted[m_SortType].Count() > 0 )
		{
			/*
			
			*/
		}
		else
		{
			//m_LoadingText.SetText( "#server_browser_unable_with_filter" );
			
		}
		
		if ( !m_Menu )
			return;
		
		if( m_LastLoadedPage == m_TotalPages )
		{
			//m_LoadingText.SetText( "#server_browser_tab_loaded" + " " + m_EntryWidgets.Count() + "/" + m_EntryWidgets.Count() + " " + "#server_browser_servers_desc" );
			m_Menu.SetServersLoadingTab( TabType.NONE );
		}
	}
	
	override void OnFilterFocus( Widget w )
	{
		super.OnFilterFocus( w );
		
		//ColorHighlight( w );
	}
	
	override void OnFilterChanged()
	{
		m_IsFilterChanged = true;
	}
	
	override void RefreshList()
	{
		for ( int i = 0; i < m_EntriesSorted.Count(); i++ )
		{
			m_EntriesSorted.GetElement(i).Clear();
		}
		
		m_Filters.SaveFilters();
		
		super.RefreshList();
		
		m_ServerListScroller.VScrollToPos01( 0 );
		
		m_LoadingText.SetText( "#dayz_game_loading" );
		
		SetFocusFilters();
		//UpdateStatusBar();
	}
	
	override void ResetFilters()
	{
		super.ResetFilters();
		
		m_IsFilterChanged = false;
	}
	
	override void ApplyFilters()
	{
		m_IsFilterChanged = false;
		m_CurrentFilterInput = m_Filters.GetFilterOptionsConsoles();
		RefreshList();
	}
	
	override void OnFilterFocusLost( Widget w )
	{
		super.OnFilterFocusLost( w );
	}
	
	override void PressA()
	{
		
	}
	
	override void PressX()
	{
		RefreshList();
	}
	
	override void PressY()
	{
		/*
		if( m_Menu.GetServersLoadingTab() != TabType.NONE )
		{
			return;
		}
		*/

		switch( m_SelectedPanel )
		{
			case SelectedPanel.BROWSER:
			{
				if( m_SelectedServer )
				{
					m_Menu.ServerListFocus( true, m_SelectedServer.ToggleFavorite() );
				}
				break;
			}
			case SelectedPanel.FILTERS:
			{
				if( m_Filters )
					m_Filters.ResetFilters();
				break;
			}
		}
	}
	
	override void PressSholderLeft()
	{
		if ( m_IsFilterFocused )
		{
			SetFocusServers();
		}
		else
		{
			SetFocusFilters();
		}
	}
	
	override void PressSholderRight()
	{
		if ( m_IsFilterFocused )
		{
			SetFocusServers();
		}
		else
		{
			SetFocusFilters();
		}
	}
	
	void SetFocusFilters()
	{
		SetEnableFilters( true );
		SetEnableServers( false );
		
		// if loaded servers is 0, then hide Top navigation menu <Left / Right>
		m_WidgetNavFilters.Show( (m_EntryWidgets.Count() > 0) );
		m_WidgetNavServers.Show( false );
		
		m_Filters.Focus();
		m_IsFilterFocused = true;
	}
	
	override void Focus()
	{
		if( m_EntryWidgets.Contains( m_CurrentSelectedServer ) )
		{
			m_EntryWidgets.Get( m_CurrentSelectedServer ).Focus();
			ScrollToEntry( m_EntryWidgets.Get( m_CurrentSelectedServer ) );
		}
		SetFocusServers();
	}
	
	void SetFocusServers()
	{
		SetEnableServers( true );
		SetEnableFilters( false );
		
		m_WidgetNavFilters.Show( false );
		m_WidgetNavServers.Show( true );
		
		array<ref GetServersResultRow> entries = m_EntriesSorted[m_SortType];
		if( entries.Count() > 0 )
		{
			m_EntryWidgets.Get(entries.Get(0).GetIpPort()).Focus();
			m_IsFilterFocused = false;
		}
		else
		{
			SetFocusFilters();
		}
	}
	
	void SetEnableFilters(bool enable)
	{
		Widget w_filters = m_Root.FindAnyWidget("filters_content");
		Widget w_content = m_Root.FindAnyWidget("server_list_scroller");
		w_filters.Enable( enable );
		w_content.Enable( !enable );
	}
	
	void SetEnableServers(bool enable)
	{
		Widget w_filters = m_Root.FindAnyWidget("filters_content");
		Widget w_content = m_Root.FindAnyWidget("server_list_scroller");
		w_filters.Enable( !enable );
		w_content.Enable( enable );
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
		if ( !w)
		{
			return
		}
		
		if ( (w.GetFlags() & WidgetFlags.IGNOREPOINTER) == WidgetFlags.IGNOREPOINTER )
		{
			return;
		}
		
		if( w.IsInherited( ButtonWidget ) )
		{
			ButtonWidget button = ButtonWidget.Cast( w );
			button.SetTextColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		TextWidget text1	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text_1" ) );
		TextWidget text3	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_label" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindAnyWidget( w.GetName() + "_image" ) );
		
		Widget option	= Widget.Cast( w.FindAnyWidget( w.GetName() + "_option_wrapper" ) );
		
		if( text1 )
		{
			text1.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if( text2 )
		{
			text2.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if( text3 )
		{
			text3.SetColor( ARGB( 255, 255, 255, 255 ) );
			w.SetAlpha(0);
		}
		
		if( image )
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
		if( button )
		{
			button.SetTextColor( ColorManager.COLOR_DISABLED_TEXT );
		}
	}
}