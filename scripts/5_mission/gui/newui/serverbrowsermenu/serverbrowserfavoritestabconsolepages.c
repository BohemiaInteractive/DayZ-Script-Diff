class ServerBrowserFavoritesTabConsolePages extends ServerBrowserTabConsolePages
{
	protected bool m_IsFavoriteChanged;

	protected override void Construct(Widget parent, ServerBrowserMenuNew menu, TabType type)
	{
		super.Construct(parent, menu, type);
		
		// disabling filter section
		m_ResetFilters.Show(false);
		m_Root.FindAnyWidget("filters_content").Show(false);
		m_Root.FindAnyWidget("show_details_button").Show(false);
		m_Root.FindAnyWidget("spacer").Show(false);
		m_Root.FindAnyWidget("spacer1").Show(false);
		m_Root.FindAnyWidget("spacer5").Show(false);
		m_Root.FindAnyWidget("server_list_root_nav_wrapper").Show(false);
		m_RefreshList.Show(m_MouseKeyboardControlled);
		
		SwitchToDetails(false);
					
		m_Menu.ShowYButton(false);
	}
	
	override void OnLoadServersAsyncFinished()
	{
		// m_TotalLoadedServers for FAVORITES tab is determined by total number of favorited servers
		TStringArray favIds = m_Menu.GetFavoritedServerIds();
		m_TotalLoadedServers = favIds.Count();
		super.OnLoadServersAsyncFinished();
		SetFocusServers();
	}
	
	protected override void LoadEntries(int cur_page_index , GetServersResultRowArray page_entries)
	{
		if (cur_page_index == 1)
		{
			m_OnlineFavServers.Clear();
		}
		
		super.LoadEntries(cur_page_index, page_entries);
	}
	
	protected override void LoadExtraEntries(int index)
	{
		if ( !m_Menu || m_Menu.GetServersLoadingTab() != m_TabType )
		{
			return;
		}
		
		// m_PagesCount for FAVORITES tab is determined by total number of favorited servers
		TStringArray favIds = m_Menu.GetFavoritedServerIds();
		m_PagesCount = Math.Ceil((float)favIds.Count() / SERVER_BROWSER_PAGE_SIZE);
		
		// offlineFavIds will always have same order, even across pages,
		// to ensure we display only fav servers that HAVEN'T been displayed yet
		TStringArray offlineFavIds = new TStringArray();
		offlineFavIds.Reserve(favIds.Count() - m_OnlineFavServers.Count());
		foreach (string ipPort : favIds)
		{
			if (m_OnlineFavServers.Find(ipPort) == -1)
			{
				offlineFavIds.Insert(ipPort);
			}
		}
		
		// appending offline servers to server list
		int totalServersAlreadyShown = (GetCurrentPage() - 1) * SERVER_BROWSER_PAGE_SIZE + index;
		int startingIndex = totalServersAlreadyShown - m_OnlineFavServers.Count();
		for (int i = startingIndex; i < offlineFavIds.Count(); ++i)
		{
			string favServerId = offlineFavIds[i];
			
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
			
			// ensure server id has correct format
			if (parts.Count() != 2)
			{
				continue;
			}
			
			GetServersResultRow offlineRow = new GetServersResultRow();
			offlineRow.m_Name = favServerId;
			offlineRow.m_Id = favServerId;
			offlineRow.m_HostIp = parts[0];
			offlineRow.m_HostPort = parts[1].ToInt();
			offlineRow.m_SteamQueryPort = offlineRow.m_HostPort;
			offlineRow.m_Favorite = true;
			
			ServerBrowserEntry entry = GetServerEntryByIndex( index );
			entry.SetIsOnline(false);
			entry.FillInfo(offlineRow);
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
	
	override void RefreshServerList()
	{
		super.RefreshServerList();

		if (m_IsFavoriteChanged)
		{
			SetCurrentPage(1);
			m_IsFavoriteChanged = false;
		}
	}
	
	void SetFavoriteChanged(bool state)
	{
		m_IsFavoriteChanged = state;
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

	override void PressThumbRight()
	{
		switch (m_SelectedPanel)
		{
			// filters are disabled for console FAVORITES tab, so do nothing
			case SelectedPanel.FILTERS:
			{
				break;
			}
			default:
			{
				super.PressThumbRight();
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

		UpdatePageButtons();
	}
}