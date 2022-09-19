class ServerBrowserFavoritesTabConsolePages extends ServerBrowserTabConsolePages
{	
	protected override void Construct(Widget parent, ServerBrowserMenuNew menu, TabType type)
	{
		super.Construct(parent, menu, type);
		
		// disabling filter section
		m_Root.FindAnyWidget("filters_content").Show(false);
		m_Root.FindAnyWidget("reset_filter_button").Show(false);
	}
	
	override void OnLoadServersAsyncFinished()
	{
		// m_TotalLoadedServers for FAVORITES tab is determined by total number of favorited servers
		TStringArray favIds = m_Menu.GetFavoritedServerIds();
		m_TotalLoadedServers = favIds.Count();
		super.OnLoadServersAsyncFinished();
	}
	
	protected override void LoadExtraEntries(int index)
	{
		if ( !m_Menu || m_Menu.GetServersLoadingTab() != m_TabType )
		{
			return;
		}
		
		// m_PagesCount for FAVORITES tab is determined by total number of favorited servers
		TStringArray favIds = m_Menu.GetFavoritedServerIds();
		m_PagesCount = Math.Ceil((float)favIds.Count() /  SERVER_BROWSER_PAGE_SIZE);
		
		// appending offline servers to server list
		int totalServersAlreadyShown = (GetCurrentPage() - 1) * SERVER_BROWSER_PAGE_SIZE + index;
		int startingIndex = totalServersAlreadyShown - m_OnlineFavServers.Count();
		
		for (int i = startingIndex; i < favIds.Count(); ++i)
		{
			string favServerId = favIds[i];
			
			// only append server if there is a free entry left on the page
			if (index >= SERVER_BROWSER_PAGE_SIZE)
			{
				break;
			}
			
			if (m_OnlineFavServers.Find(favServerId) > -1)
			{
				continue;
			}
			array<string> parts = new array<string>;
			favServerId.Split(":", parts);
			
			GetServersResultRow offlineRow = new GetServersResultRow();
			offlineRow.m_Name = favServerId;
			offlineRow.m_Id = favServerId;
			offlineRow.m_HostIp = parts[0];
			offlineRow.m_HostPort = parts[1].ToInt();
			offlineRow.m_SteamQueryPort = offlineRow.m_HostPort;
			offlineRow.m_Favorite = true;
			
			ServerBrowserEntry entry = GetServerEntryByIndex( index );
			entry.FillInfo(offlineRow);
			entry.SetIsOnline(false);
			entry.UpdateEntry();
			
			m_EntryWidgets.Insert(favServerId, entry);
			m_EntriesSorted[m_SortType].Insert(offlineRow);
			
			index++;
		}
	}
	
	override void RefreshList()
	{
		super.RefreshList();
#ifdef PLATFORM_WINDOWS
		m_CurrentFilterInput.SetFavorited( true );
#endif
		AddFavoritesToFilter(m_CurrentFilterInput);
		OnlineServices.LoadServers(m_CurrentFilterInput);
	}
	
	override bool PassFilter(GetServersResultRow result)
	{
		if (m_TabType == TabType.FAVORITE)
 		{
	 	 	if (!m_Menu.IsFavorited(result.GetIpPort()))
	 	 	{
	 	 		return false;
	 	 	}
 	 	}
		
		return super.PassFilter(result);
	}
	
	override void PressY()
	{
		switch ( m_SelectedPanel )
		{
			// filters are disabled for console FAVORITES tab, so do nothing
			case SelectedPanel.FILTERS:
			{	
				break;
			}
			default:
			{
				super.PressY();
				break;
			}
		}
	}
	
	override void PressX()
	{
		if ( (GetGame().GetTime() - m_TimeLastServerRefresh) > 1000 )
		{
			SetCurrentPage(1);
			super.PressX();
		}
	}
	
	override void SetFocusFilters()
	{
		super.SetFocusFilters();
			
		m_Menu.ShowYButton(false);
		m_Menu.ShowAButton(false);
		
		// focus on the back button instead of filter section
		m_Menu.BackButtonFocus();
		
		UpdatePageButtons();
	}
}