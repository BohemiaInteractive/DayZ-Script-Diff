class MainMenuData
{
	protected static ref JsonDataNewsList m_NewsData;
	protected static ref array<ref ModInfo> m_AllMods;
	protected static ref map<string, ref ModInfo> m_AllDlcsMap;
	
	static JsonDataNewsList GetNewsData()
	{
		if (!m_NewsData)
			m_NewsData = new JsonDataNewsList();
		
		return m_NewsData;
	}
		
	static JsonDataNewsArticle GetNewsArticle(int index)
	{
		if (!m_NewsData)
			return null;
		
		if (index > m_NewsData.News.Count() - 1)
		{
			ErrorEx(string.Format("Requested news article for index %1 but article count is %2. Index out of bounds!", index, m_NewsData.News.Count()));
			return null;
		}
		
		return m_NewsData.News[index];
	}
	
	static void LoadMods()
	{
		if (!m_NewsData)
			GetNewsData();
		
		if (m_AllMods != null)
			return;
		
		m_AllMods = new array<ref ModInfo>;

		g_Game.GetModInfos(m_AllMods);
		if (m_AllMods.Count() > 0)
		{
			m_AllMods.Remove(m_AllMods.Count() - 1);
			m_AllMods.Invert();
		}
		
		FilterDLCs(m_AllMods);
		
		CreateDLCArticles();
	}
	
	static void FilterDLCs(inout array<ref ModInfo> modArray)
	{
		if (!m_AllDlcsMap)
			m_AllDlcsMap = new map<string,ref ModInfo>;
		else
			m_AllDlcsMap.Clear();
		
		ModInfo info;
		int count = modArray.Count();
		for (int i = count - 1; i >= 0; i--)
		{
			info = modArray[i];	
			if (info.GetIsDLC())
			{
				string dlcName = info.GetName();
				dlcName.ToLower();
				dlcName.Replace("dayz ", "");
				m_AllDlcsMap.Insert(dlcName, info);
			}
		}
	}
	
	static void CreateDLCArticles()
	{
		if (!m_AllDlcsMap)
		{		
			ErrorEx("Failed to get DLC map data!", ErrorExSeverity.WARNING);
			return;
		}
		
		if (!m_NewsData)
		{		
			ErrorEx("Failed to get news data!", ErrorExSeverity.WARNING);
			return;
		}
		
		for (int i = 0; i < m_AllDlcsMap.Count(); i++)
		{
			ModInfo info = m_AllDlcsMap.GetElement(i);
			JsonDataNewsArticle dlcArticle = new JsonDataNewsArticle;
			
			dlcArticle.categoryID = NewsCategory.DLCS;
			dlcArticle.excerpt = info.GetOverview();
			dlcArticle.title = info.GetName();
			dlcArticle.fullUrl = info.GetAction();
			string dlcName = info.GetName();
			dlcName.ToLower();
			dlcName.Replace("dayz ", "");
			dlcArticle.dlcName = dlcName;
			dlcArticle.coverImage = ModInfo.GetDLCImage(dlcName);
			m_NewsData.News.Insert(dlcArticle);
		}
		
		//! Temporary DLC entry for Badlands for the carousel promo on PC only. REMOVE AFTER MOD INFO HAS BEEN ADDED	
		JsonDataNewsArticle tempDLCArticle = new JsonDataNewsArticle;
		tempDLCArticle.categoryID = NewsCategory.DLC_PROMO;
		tempDLCArticle.title = "#dlc_news_badlands";
		#ifdef PLATFORM_WINDOWS
		tempDLCArticle.fullUrl = "https://store.steampowered.com/app/3816030";
		#else
		tempDLCArticle.fullUrl = "";
		#endif
		tempDLCArticle.dlcName = "badlands";
		tempDLCArticle.coverImage = ModInfo.GetDLCImage(tempDLCArticle.dlcName);
		m_NewsData.News.Insert(tempDLCArticle);
	}
	
	static void ClearAllDLCs()
	{
		m_AllMods = null;
	}
	
	//! DLCs only
	static ModInfo GetDLCModInfoByName(string dlcName)
	{
		ModInfo modInfo;
		m_AllDlcsMap.Find(dlcName, modInfo);
		return modInfo;
	}
	
	//! All mods including DLCs
	static array<ref ModInfo> GetAllMods()
	{
		return m_AllMods;
	}
}