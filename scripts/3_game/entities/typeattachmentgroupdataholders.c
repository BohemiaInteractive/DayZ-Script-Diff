typedef map<string,ref TypeAttachmentGroupData> TAttachmentGroupMap;

//! Data holder for attachment group classes
class TypeAttachmentGroupsDataHolder
{
	protected int m_ValidSubclassCount;
	protected bool m_IsValid;
	protected string m_ParentType;
	protected string m_AttGroupHeaderDisplayName;
	protected string m_PropsPath; //! full path to 'GUIInventoryAttachmentsProps'
	protected ref TAttachmentGroupMap m_AttachmentGroupMap; // <GroupSubclassName, ref cachedInfo>
	protected ref map<string, ref array<TypeAttachmentGroupData>> m_SelectionPairingMap; // <selection name, <cachedInfos array>>
	
	void TypeAttachmentGroupsDataHolder(string parentType)
	{
		m_ParentType = parentType;
		Init();
	}
	
	protected void Init()
	{
		m_IsValid = false;
		
		//choose the proper path first
		string bases[3] = {CFG_WEAPONSPATH, CFG_MAGAZINESPATH, CFG_VEHICLESPATH};
		string pathBase;
		foreach (string base : bases)
		{
			pathBase = "" + base + " " + m_ParentType;
			if (g_Game.ConfigIsExisting(pathBase))
				break;
			
			pathBase = "";
		}
		
		if (pathBase == "")
			return;
		
		m_PropsPath = "" + pathBase + " GUIInventoryAttachmentsProps";
		if (!g_Game.ConfigIsExisting(m_PropsPath) || m_AttachmentGroupMap != null)
			return;
		
		int count = g_Game.ConfigGetChildrenCount(m_PropsPath);
		if (count == 0)
			return;
		
		m_AttachmentGroupMap = new TAttachmentGroupMap();
		m_SelectionPairingMap = new map<string, ref array<TypeAttachmentGroupData>>();
		string childName;
		for (int i = 0; i < count; ++i)
		{
			g_Game.ConfigGetChildName(m_PropsPath, i, childName);
			TypeAttachmentGroupData groupData = new TypeAttachmentGroupData(childName, m_PropsPath);
			
			if (groupData.IsValid())
			{
				m_AttachmentGroupMap.Set(childName, groupData);
				array<TypeAttachmentGroupData> groupDataArray = m_SelectionPairingMap.Get(groupData.m_SelectionName);
				if (!groupDataArray)
					groupDataArray = new array<TypeAttachmentGroupData>();
				groupDataArray.Insert(groupData);
				m_SelectionPairingMap.Set(groupData.m_SelectionName, groupDataArray);
			}
			else
			{
				groupData = null;
			}
		}
		
		m_ValidSubclassCount = m_AttachmentGroupMap.Count();
		m_IsValid = m_ValidSubclassCount > 0;
		g_Game.ConfigGetText("" + m_PropsPath + " headerDisplayName", m_AttGroupHeaderDisplayName);
	}
	
	//! Re-reads localized header/name strings without rebuilding slot config data
	void RefreshLocalizedTexts()
	{
		if (m_PropsPath == "")
			return;

		g_Game.ConfigGetText("" + m_PropsPath + " headerDisplayName", m_AttGroupHeaderDisplayName);

		if (m_AttachmentGroupMap)
		{
			for (int i = 0; i < m_AttachmentGroupMap.Count(); ++i)
			{
				TypeAttachmentGroupData data = m_AttachmentGroupMap.GetElement(i);
				if (data)
					data.RefreshLocalizedName();
			}
		}
	}
	
	TAttachmentGroupMap GetAttachmentGroupMap()
	{
		return m_AttachmentGroupMap;
	}
	
	int GetValidAttachmnetGroupCount()
	{
		return m_ValidSubclassCount;
	}
	
	string GetPropsCfgPath()
	{
		return m_PropsPath;
	}
	
	//! Returns single 'GUIInventoryAttachmentsProps' subclass data based on class name
	TypeAttachmentGroupData GetAttGroupDataByClassName(string groupName)
	{
		if (m_AttachmentGroupMap)
			return m_AttachmentGroupMap.Get(groupName);
		
		return null;
	}
	
	//! Returns an array of 'GUIInventoryAttachmentsProps' subclasses that use the same 'selection'
	array<TypeAttachmentGroupData> GetAttGroupsDataBySelectionParam(string selectionName)
	{
		if (m_SelectionPairingMap)
			return m_SelectionPairingMap.Get(selectionName);
		
		return null;
	}
	
	string GetHeaderDisplayName()
	{
		return m_AttGroupHeaderDisplayName;
	}
	
	bool IsValid()
	{
		return m_IsValid;
	}
}
