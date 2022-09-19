class ServerBrowserFavoritesTabPc extends ServerBrowserTabPc
{
	// value = ip, name, connection port, query port
	private ref array<ref CachedServerInfo> m_CachedFavoriteServerInfo;
	
	protected override void Construct(Widget parent, ServerBrowserMenuNew menu, TabType type)
	{
		m_CachedFavoriteServerInfo = new array<ref CachedServerInfo>();
		
		super.Construct(parent, menu, type);
		DisableFilters();
	}

	protected override void LoadExtraEntries(int index)
	{
		array<ref Param2<string, CachedServerInfo>> relevantCachedInfo = new array<ref Param2<string, CachedServerInfo>>();	
		foreach (CachedServerInfo cachedInfo : m_CachedFavoriteServerInfo)
		{
			string serverId = GetConnEndPoint(cachedInfo);
			
			// ensure the cached server is still favorited
			if (!m_Menu.IsFavorited(GetQueryEndPoint(cachedInfo)))
			{
				continue;
			}
			
			if (m_OnlineFavServers.Find(serverId) > -1)
			{
				continue;
			}
			
			relevantCachedInfo.Insert(new Param2<string, CachedServerInfo>(serverId, cachedInfo));
		}
		
		// adding FAVORITED, OFFLINE servers to favorites tab
		int totalServersAlreadyShown = m_PageIndex * SERVERS_VISIBLE_COUNT + index;
		int startingIndex = totalServersAlreadyShown - m_TotalLoadedServers;
		m_TotalLoadedServers += relevantCachedInfo.Count();
		for (int i = startingIndex; i < relevantCachedInfo.Count(); ++i)
		{
			if (index >= SERVERS_VISIBLE_COUNT)
			{
				break;
			}
			
			Param2<string, CachedServerInfo> relevantEntry = relevantCachedInfo[i];
			
			cachedInfo = relevantEntry.param2;			
			string ip = cachedInfo.param1;
			string cachedName = cachedInfo.param2;
			int connPort = cachedInfo.param3;
			int queryPort = cachedInfo.param4;

			// do NOT insert offlineRow into m_EntriesSorted[m_SortType]!!
			// we assume that rows already in m_EntriesSorted[m_SortType] are ONLINE,
			// if we add offline info to m_EntriesSorted[m_SortType] then we cannot differente
			// between online and offline servers			
			GetServersResultRow offlineRow = new GetServersResultRow();
			offlineRow.m_Id = relevantEntry.param1;
			offlineRow.m_Name = cachedName;
			offlineRow.m_HostIp = ip;
			offlineRow.m_SteamQueryPort = queryPort;
			offlineRow.m_HostPort = connPort;
			offlineRow.m_Favorite = true;
			
			ServerBrowserEntry entry = GetServerEntryByIndex(index, offlineRow.m_Id);
			entry.Show(true);
			entry.FillInfo(offlineRow);
			entry.SetIsOnline(false);
			entry.UpdateEntry();
			
			index++;
		}
	}
	
	override void RefreshList()
	{
		OnlineServices.GetCachedFavServerInfo(m_CachedFavoriteServerInfo);
		
		m_OnlineFavServers.Clear();
		super.RefreshList();
		
		m_CurrentFilterInput.SetFavorited( true );
		AddFavoritesToFilter(m_CurrentFilterInput);
		OnlineServices.LoadServers(m_CurrentFilterInput);
	}

	private string GetConnEndPoint(CachedServerInfo cachedInfo)
	{
		string ip = cachedInfo.param1;
		int connPort = cachedInfo.param3;
		return ip + ":" + connPort;
	}
	
	private string GetQueryEndPoint(CachedServerInfo cachedInfo)
	{
		string ip = cachedInfo.param1;
		int queryPort = cachedInfo.param4;
		return ip + ":" + queryPort;
	}
}