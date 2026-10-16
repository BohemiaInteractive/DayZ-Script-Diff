class ConstructionPartRebuildRubbleTypeData : ConstructionPartRebuildTypeData
{
	protected ref array<string> m_RelatedPartNames; //! Names of related rubble parts
	protected ref map<string, bool> m_CustomAnimationSourceBehaviors; //! "hide-type" animation sources to be animated when all related parts are built, and the direction
	protected ref array<string> m_MaterialTypes; //type and quantity counts must match!
	protected ref array<float> m_MaterialBaseQuantities; //type and quantity counts must match!
	
	override protected void InitCustomPartTypeProperties(PartTypeInitData initData)
	{
		string path = initData.m_PartPath + " CustomPartTypeProperties";
		if (!g_Game.ConfigIsExisting(path))
		{
			ErrorEx("class CustomPartTypeProperties undefined for " + initData.m_PartPath);
			return;
		}
		else
		{
			if (!g_Game.ConfigIsExisting(path + " related_parts"))
			{
				ErrorConfigUndefined("related_parts", path);
				return;
			}
			m_RelatedPartNames = new array<string>();
			g_Game.ConfigGetTextArray(path + " related_parts", m_RelatedPartNames);
			
			if (!g_Game.ConfigIsExisting(path + " animation_sources"))
			{
				ClearInitData();
				ErrorConfigUndefined("animation_sources", path);
				return;
			}
			array<string> animSourceNames = new array<string>();
			g_Game.ConfigGetTextArray(path + " animation_sources", animSourceNames);
			
			if (!g_Game.ConfigIsExisting(path + " animation_source_behaviors_invert"))
			{
				ClearInitData();
				ErrorConfigUndefined("animation_source_behaviors_invert", path);
				return;
			}
			array<int> animSourceBehaviors = new array<int>();
			g_Game.ConfigGetIntArray(path + " animation_source_behaviors_invert", animSourceBehaviors);
			
			int animSourcesCount = animSourceNames.Count();
			if (animSourcesCount == 0 || (animSourcesCount != animSourceBehaviors.Count()))
			{
				ClearInitData();
				ErrorEx("invalid count or count mismatch between 'animation_sources' and 'animation_source_behaviors_invert' in " + path);
				return;
			}
			
			m_CustomAnimationSourceBehaviors = new map<string, bool>();
			for (int i = 0; i < animSourcesCount; ++i)
			{
				m_CustomAnimationSourceBehaviors.Set(animSourceNames[i], animSourceBehaviors[i]);
			}
			
			InitMaterialsArray();
		}
	}
	
	protected void InitMaterialsArray()
	{
		string cfgPath = m_PartPath + " MaterialsYieldOnBuilt";
		if (g_Game.ConfigIsExisting(cfgPath))
		{
			int count = g_Game.ConfigGetChildrenCount(cfgPath);
			if (count < 1)
				return;
			
			m_MaterialTypes = new array<string>();
			m_MaterialBaseQuantities = new array<float>();
			string childClassName, itemType, tmpPath;
			float quantity;
			
			for (int i = 0; i < count; ++i)
			{
				g_Game.ConfigGetChildName(cfgPath, i, childClassName);
				
				tmpPath = cfgPath + " " + childClassName + " " + "type";
				g_Game.ConfigGetText(tmpPath, itemType);
				m_MaterialTypes.Insert(itemType);
				
				tmpPath = cfgPath + " " + childClassName + " " + "quantity";
				quantity = g_Game.ConfigGetFloat(tmpPath);
				m_MaterialBaseQuantities.Insert(quantity);
			}
		}
	}
	
	protected void ClearInitData()
	{
		m_RelatedPartNames = null;
		m_CustomAnimationSourceBehaviors = null;
	}
	
	///////////
	//Getters//
	///////////
	
	array<string> GetRelatedPartNames()
	{
		return m_RelatedPartNames;
	}
	
	map<string, bool> CustomAnimationSourceBehaviorMap()
	{
		return m_CustomAnimationSourceBehaviors;
	}
	
	array<string> GetRubbleMaterialTypes()
	{
		return m_MaterialTypes;
	}
	
	array<float> GetRubbleMaterialBaseQuantities()
	{
		return m_MaterialBaseQuantities;
	}
}