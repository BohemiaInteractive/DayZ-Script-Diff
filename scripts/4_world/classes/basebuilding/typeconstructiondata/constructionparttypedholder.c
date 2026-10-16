//! holder object for entity type-level parts info
class ConstructionDataTypeHolder : ConstructionDataTypeHolderBasic
{
	protected ref map<string, ref ConstructionPartTypeData> m_ConstructionPartTypeMap;
	protected ref array<ConstructionPartTypeData> m_ConstructionPartTypes;
	protected ref map<string, ref array<ConstructionPartTypeData>> m_GuiRequiredPartTypesMap; //<GuiCategyoryName, <related part types>>
	
	
	//! Called manually after spawning from typename
	override void InitPartsTypeDataHolder(EntityAIType eType, string cfgPath)
	{
		super.InitPartsTypeDataHolder(eType, cfgPath);
		
		m_ConstructionPartTypeMap = new map<string, ref ConstructionPartTypeData>();
		m_ConstructionPartTypes = new array<ConstructionPartTypeData>();
		InitAttachmentGUIPartStructure(eType);
		
		//main parts
		for (int i = 0; i < g_Game.ConfigGetChildrenCount(cfgPath); ++i)
		{
			string mainPartName;
			g_Game.ConfigGetChildName(cfgPath, i, mainPartName);
			string mainPartPath = cfgPath + " " + mainPartName;
			
			//parts
			for (int j = 0; j < g_Game.ConfigGetChildrenCount(mainPartPath); ++j)
			{
				string partName = "";
				g_Game.ConfigGetChildName(mainPartPath, j, partName);
				string partPath = mainPartPath + " " + partName;
				ConstructionPartTypeData typeDataObject;
				
				//check creation of a custom part data type first
				string customTypeStr;
				string customPartPath = partPath + " custom_part_type";
				if (g_Game.ConfigIsExisting(customPartPath) && g_Game.ConfigGetText(customPartPath, customTypeStr))
				{
					Class.CastTo(typeDataObject, (customTypeStr + "TypeData").ToType().Spawn());
				}
				
				//default typeDataObject creation
				if (!typeDataObject)
					typeDataObject = new ConstructionPartTypeData();
				else
					typeDataObject.SetCustomInstanceType(customTypeStr);
				
				PartTypeInitData initData = new PartTypeInitData();
				initData.m_PartPath = partPath;
				initData.m_PartName = partName;
				initData.m_MainPartName = mainPartName;
				typeDataObject.InitPartTypeData(initData);
				
				//general behavior for the entire construction
				if (g_Game.ConfigIsExisting(cfgPath + " can_decay_general"))
					typeDataObject.SetCanDecayConfig(g_Game.ConfigGetInt(cfgPath + " can_decay_general"));
				
				m_ConstructionPartTypeMap.Insert(partName, typeDataObject);
				m_ConstructionPartTypes.Insert(typeDataObject);
				
				InitAttachmentGUIPartLinks(eType, typeDataObject);
			}
		}
	}
	
	void InitAttachmentGUIPartStructure(EntityAIType eType)
	{
		if (m_GuiRequiredPartTypesMap) //prevents double inits
			return;
		
		TypeAttachmentGroupsDataHolder holder = eType.GetTypeAttachmentGroupsDataHolder();
		if (!holder.IsValid())
			return;
		
		m_GuiRequiredPartTypesMap = new map<string, ref array<ConstructionPartTypeData>>();
		TAttachmentGroupMap groupMap = holder.GetAttachmentGroupMap();
		foreach (string name, ref TypeAttachmentGroupData data : groupMap)
		{
			m_GuiRequiredPartTypesMap.Set(name, new array<ConstructionPartTypeData>());
		}
	}
	
	protected void InitAttachmentGUIPartLinks(EntityAIType eType, ConstructionPartTypeData typeDataObject)
	{
		if (!typeDataObject.HasRelatedGUICategories()) //nothing defined, skipping
			return;
		
		if (!m_GuiRequiredPartTypesMap) //SOMETHING defined, but no valid GUIInventoryAttachmentsProps subclass of that name
		{
			ErrorEx("'related_GUI_categories' defined on part " + typeDataObject.GetPartName() + ", but no valid 'GUIInventoryAttachmentsProps' group defined on the entity!");
			return;
		}
		TypeAttachmentGroupsDataHolder holder = eType.GetTypeAttachmentGroupsDataHolder();
		array<string> namesArr = typeDataObject.GetRelatedGUICategoryNames();
		foreach (string name : namesArr)
		{
			array<ConstructionPartTypeData> tmp = m_GuiRequiredPartTypesMap.Get(name);
			if (!tmp)
			{
				string validCategories = "";
				TAttachmentGroupMap groupMap = holder.GetAttachmentGroupMap();
				foreach (string validName, ref TypeAttachmentGroupData groupData : groupMap)
					validCategories += "'" + validName + "' ";
				ErrorEx("Part '" + typeDataObject.GetPartName() + "' on type '" + eType.GetName() + "' declares related_GUI_category '" + name + "', but it does not match any GUIInventoryAttachmentsProps entry! Valid categories: [" + validCategories + "]");
				continue;
			}
			tmp.Insert(typeDataObject);
		}
	}
	
	map<string, ref ConstructionPartTypeData> GetConstructionPartTypesMap()
	{
		return m_ConstructionPartTypeMap;
	}
	
	array<ConstructionPartTypeData> GetConstructionPartTypeDataArray()
	{
		return m_ConstructionPartTypes;
	}
	
	map<string, ref array<ConstructionPartTypeData>> GetGuiRequiredPartTypesMap()
	{
		return m_GuiRequiredPartTypesMap;
	}
}