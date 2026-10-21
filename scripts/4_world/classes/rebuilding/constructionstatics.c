class ConstructionPartStaticsSupportData
{
	protected ref map<int, ref StaticsSupportCategoryValues> m_ValueMap;
	protected ref set<int> m_ConfiguredCategories;
	
	void Init(string partPath)
	{
		//prevents double initialization
		if (m_ValueMap)
			return;
		
		m_ConfiguredCategories = new set<int>();
		string pathBase = partPath + " StaticsSupportData";
		if (g_Game.ConfigIsExisting(pathBase))
		{
			string childName;
			string supportCategory;
			int supportStrength;
			int i;
			
			m_ValueMap = new map<int, ref StaticsSupportCategoryValues>();
			map<int, int> requirementMap = new map<int, int>();
			map<int, int> providerMap = new map<int, int>();
			
			//support provider data basic parse
			string path = pathBase + " SupportProviders";
			int count = g_Game.ConfigGetChildrenCount(path);
			if (count > 0)
			{
				for (i = 0; i < count; ++i)
				{
					childName = "";
					supportCategory = "";
					supportStrength = 0;
					
					g_Game.ConfigGetChildName(path, i, childName);
					g_Game.ConfigGetTextRaw(path + " " + childName + " supportCategory", supportCategory);
					supportStrength = g_Game.ConfigGetInt(path + " " + childName + " supportStrength");
					providerMap.Insert(supportCategory.Hash(), supportStrength);
					m_ConfiguredCategories.Insert(supportCategory.Hash());
				}
			}
			
			//support requirements data basic parse
			path = pathBase + " SupportRequirements";
			count = g_Game.ConfigGetChildrenCount(path);
			if (count > 0)
			{
				for (i = 0; i < count; ++i)
				{
					childName = "";
					supportCategory = "";
					supportStrength = 0;
					
					g_Game.ConfigGetChildName(path, i, childName);
					g_Game.ConfigGetTextRaw(path + " " + childName + " supportCategory", supportCategory);
					supportStrength = g_Game.ConfigGetInt(path + " " + childName + " supportStrength");
					requirementMap.Insert(supportCategory.Hash(), supportStrength);
					m_ConfiguredCategories.Insert(supportCategory.Hash());
				}
			}
			
			//now insert as 'StaticsSupportCategoryValues'
			foreach (int category : m_ConfiguredCategories)
			{
				//default invalid values
				int provided = -1;
				if (providerMap.Contains(category))
					provided = providerMap.Get(category);
				
				int required = -1;
				if (requirementMap.Contains(category))
					required = requirementMap.Get(category);
				
				m_ValueMap.Insert(category, new StaticsSupportCategoryValues(category, provided, required));
			}
		}
	}
	
	bool GetSupportProvided(int category, out int providedStrength)
	{
		if (!m_ValueMap || !m_ValueMap.Contains(category))
			return false;
		
		providedStrength = m_ValueMap.Get(category).m_ProvidedSupport;
		return providedStrength > -1;
	}
	
	bool GetSupportRequirement(int category, out int requiredStrength)
	{
		if (!m_ValueMap || !m_ValueMap.Contains(category))
			return false;
		
		requiredStrength = m_ValueMap.Get(category).m_RequiredSupport;
		return requiredStrength > -1;
	}
	
	set<int> GetConfiguredSupportCategories()
	{
		return m_ConfiguredCategories;
	}
}

class StaticsSupportCategoryValues
{
	int m_Category;
	int m_ProvidedSupport;
	int m_RequiredSupport;
	
	void StaticsSupportCategoryValues(int category, int provides, int requires)
	{
		m_Category = category;
		m_ProvidedSupport = provides;
		m_RequiredSupport = requires;
	}
}