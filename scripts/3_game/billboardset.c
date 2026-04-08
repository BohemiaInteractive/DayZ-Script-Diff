class BillboardSetHandler
{
	protected bool 							m_BillboardSetIndex 	= -1;
	protected ref array<ref BillboardSet> 	m_BillboardSets;
	protected static const string 			ROOT_CLASS = "BillboardPresets";
	protected static int 					m_SetIndexCached = -1;//once we resolve the name into an index, we cache it(this disallows dynamic index swapping during mission's runtime)
	
	string GetTextureByType(string type)
	{
		if (m_BillboardSetIndex == -1)
			return "";
		BillboardSet bbset = m_BillboardSets.Get(m_BillboardSetIndex);
		return bbset.GetTextureByType(type);
	}

	void OnRPCIndex(int index)
	{
		if (!m_BillboardSets)
			LoadBillboardConfigs();
						
		if (m_BillboardSets && m_BillboardSets.IsValidIndex(index))
		{
			m_BillboardSetIndex = index;
		}
	}
	
	protected bool LoadBillboardConfigs()
	{
		m_BillboardSets = new array<ref BillboardSet>();
		
		int setCount = g_Game.ConfigGetChildrenCount(ROOT_CLASS);

		for (int setIndex = 0; setIndex < setCount; setIndex++)
		{
			string setName;
			g_Game.ConfigGetChildName(ROOT_CLASS, setIndex, setName);
			string path = ROOT_CLASS + " " + setName;
			m_BillboardSets.Insert(new BillboardSet(path));
		}
		
		return true; 
	}
	
	static bool ActivateBillboardSet(string setClassName, PlayerIdentity identity)
	{
		if (!g_Game)
			return false;
		
		if (m_SetIndexCached == -1)
		{
			int setCount = g_Game.ConfigGetChildrenCount(ROOT_CLASS);
			for (int setIndex = 0; setIndex < setCount; setIndex++)
			{
				string setName;
				g_Game.ConfigGetChildName(ROOT_CLASS, setIndex, setName);
				
				if (setName == setClassName)
				{
					m_SetIndexCached = setIndex;
					break
				}
			}
		}
		
		if (m_SetIndexCached != -1)
		{
			auto param = new Param1<int>(m_SetIndexCached);
			g_Game.RPCSingleParam( identity.GetPlayer(), ERPCs.RPC_SET_BILLBOARDS, param, true, identity );
			return true;
		}
		return false;
	}
}


class BillboardSet
{
	protected ref map<string, string> m_TypeTextureMapping = new map<string, string>();
	
	void BillboardSet(string path)
	{
		LoadConfig(path);
	}
	
	string GetTextureByType(string type)
	{
		return m_TypeTextureMapping.Get(type);
	}

	protected void LoadConfig(string path)
	{
		int count = g_Game.ConfigGetChildrenCount(path);
		for ( int i = 0; i < count; i++ )
		{
			string itemName;
			g_Game.ConfigGetChildName(path, i, itemName);
			
			string typesPath = path + " " + itemName + " types";
			string texturePath = path + " " + itemName + " texture";
			
			if (g_Game.ConfigIsExisting(typesPath) && g_Game.ConfigIsExisting(texturePath))
			{
				TStringArray types = new TStringArray();
				string texture;
				g_Game.ConfigGetText(texturePath, texture);
				g_Game.ConfigGetTextArray(typesPath, types);
				
				foreach (string s: types)
				{
					m_TypeTextureMapping.Insert(s,texture);
				}
			}
			else
			{
				ErrorEx("Billboard set incorrect configuration, type or texture param missing: " + path);
			}
			
		}
	}
}