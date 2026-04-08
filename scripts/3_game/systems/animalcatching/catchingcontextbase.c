class BaitData
{
	float m_BaseProbability;
	EntityAI m_Owner;
	
	void BaitData(float value, EntityAI owner)
	{
		m_BaseProbability = value;
		m_Owner = owner;
	}
}

class CatchingContextBase
{
	protected EntityAI m_MainItem;
	protected YieldsMap m_YieldsMapAll;
	protected bool m_IsValid;
	protected int m_MethodMask;
	protected int m_EnviroMask;
	protected float m_QualityBaseMod; //base quality
	protected float m_QualityDispersionMinMod; //extra added quality, min
	protected float m_QualityDispersionMaxMod; //extra added quality, max
	protected ref map<int,ref BaitData> m_BaitCompatibilityMap;
	protected ref array<YieldItemBase> m_CompatibleItems; //list of all useful yields (initial evaluation based on masks)
	protected ref array<YieldItemBase> m_ExcludedItems;
	protected ref array<int> m_ProbabilityArray; //actual probability list
	protected ref CatchingResultBasic m_Result;
	
	void CatchingContextBase(Param par)
	{
		Init(par);
	}
	
	protected void Init(Param par)
	{
		m_IsValid = true;
		m_YieldsMapAll = g_Game.GetMission().GetWorldData().GetCatchYieldBank().GetYieldsMap();
		DeserializeData(par);
		CreateResultDataStructure();
		InitCatchingItemData();
		InitCatchMasks();
		SetupInitialTypes();
		SetupProbabilityArray();
	}
	
	protected void DeserializeData(Param par);
	
	protected void InitBaitCompatibilityMap()
	{
		if (!m_BaitCompatibilityMap)
			m_BaitCompatibilityMap = new map<int,ref BaitData>();
		else
			m_BaitCompatibilityMap.Clear();
	}
	
	protected void SetupInitialTypes()
	{
		if (!m_CompatibleItems)
			m_CompatibleItems = new array<YieldItemBase>();
		else
			m_CompatibleItems.Clear();
		
		if (!m_ExcludedItems)
			m_ExcludedItems = new array<YieldItemBase>();
		else
			m_ExcludedItems.Clear();
		
		
		ParseYieldItemsForCompatibility({ECatchingBaitCategories.BAIT_TYPE_EMPTY});
		if (m_CompatibleItems.Count() == 0)
			ParseYieldItemsForCompatibility();
		
		if (m_CompatibleItems.Count() == 0)
		{
			ErrorEx("Empty m_CompatibleItems for item: " + m_MainItem,ErrorExSeverity.INFO);
			m_IsValid = false;
		}
	}
	
	protected void ParseYieldItemsForCompatibility(array<ECatchingBaitCategories> excludedTypes = null)
	{
		YieldItemBase yItem;
		int count = m_YieldsMapAll.Count();
		for (int i = 0; i < count; i++)
		{
			yItem = m_YieldsMapAll.GetElement(i);
			if (m_ExcludedItems.Find(yItem) != -1) //skip already excluded yitems
				continue;
			
			if (CheckMaskCompatibility(yItem) && CheckBaitCompatibility(yItem,excludedTypes))
			{
				m_CompatibleItems.Insert(yItem);
			}
		}
	}
	
	protected void SetupProbabilityArray()
	{
		if (!m_ProbabilityArray)
		m_ProbabilityArray = new array<int>();
		m_ProbabilityArray.Clear();
		
		int count = m_CompatibleItems.Count();
		YieldItemBase yItem;
		
		for (int i = 0; i < count; i++)
		{
			yItem = m_CompatibleItems[i];
			string type = yItem.GetType();
			int weight = yItem.GetYieldWeight(this);
			
			for (int j = 0; j < weight; j++)
			{
				m_ProbabilityArray.Insert(type.Hash());
			}
		}
		
		if (m_ProbabilityArray.Count() == 0)
		{
			ErrorEx("Empty m_ProbabilityArray for item: " + m_MainItem,ErrorExSeverity.INFO);
			m_IsValid = false;
		}
	}
	
	YieldsMap GetYieldsMap()
	{
		return m_YieldsMapAll;
	}
	
	//! override if using another result
	protected void CreateResultDataStructure()
	{
		//m_Result = new CatchingResultSomethingSomethingCustom(someItem);
		
		if (!m_Result)
		{
			ErrorEx("Failed to create 'm_Result' structure for item: " + m_MainItem,ErrorExSeverity.INFO);
			m_IsValid = false;
		}
	}
	
	protected void InitCatchMasks()
	{
		InitCatchMethodMask();
		InitCatchEnviroMask();
	}
	
	protected void InitCatchMethodMask()
	{
		m_MethodMask = 0;
	}
	
	protected void InitCatchEnviroMask()
	{
		m_EnviroMask = 0;
	}
	
	//! override using 'ECatchMethodMask'
	protected int GetCatchMethodMask()
	{
		return m_MethodMask;
	}
	
	protected int GetCatchEnviroMask()
	{
		return m_EnviroMask;
	}
	
	protected bool CheckMaskCompatibility(YieldItemBase yItem)
	{
		return (yItem.GetMethodMask() & GetCatchMethodMask()) && (yItem.GetEnviroMask() & GetCatchEnviroMask());
	}
	
	protected bool CheckBaitCompatibility(YieldItemBase yItem, array<ECatchingBaitCategories> excludedTypes = null)
	{
		float highestSensitivity = 0.0;
		float sensitivity = 0.0;
		int count = m_BaitCompatibilityMap.Count();
		ECatchingBaitCategories baitCategoryType;
		
		for (int i = 0; i < count; i++)
		{
			baitCategoryType = m_BaitCompatibilityMap.GetKey(i);
			if (excludedTypes && excludedTypes.Find(baitCategoryType) != -1) //skip ignored bait types in the first run
				continue;
			
			sensitivity = yItem.GetBaitTypeSensitivity(baitCategoryType);
			if (sensitivity < 0)
			{
				if (m_ExcludedItems.Find(yItem) != -1)
					m_ExcludedItems.Insert(yItem);
				return false;
			}
			
			highestSensitivity = Math.Max(highestSensitivity,sensitivity);
		}
		
		#ifdef DEVELOPER
		if (IsCLIParam("catchingLogs"))
		{
			if (count == 0)
				Print("dbgTrapz | " + yItem + " compatible due to unspecified bait compatibility!");
			else if (highestSensitivity > 0.0)
				Print("dbgTrapz | " + yItem + " compatible with highestSensitivity: " + highestSensitivity);
		}
		#endif
		
		return count == 0 || highestSensitivity > 0.0;
	}
	
	void UpdateCatchingItemData()
	{
		InitCatchingItemData();
		RecalculateProcessingData();
	}
	
	protected void InitCatchingItemData()
	{
		if (!m_MainItem)
		{
			ErrorEx("No item defined for 'InitCatchingItemData'. Skipping.");
			m_IsValid = false;
			return;
		}
		
		ClearCatchingItemData();
		InitBaitCompatibilityMap();
		
		//bruteforce, but should be done sparingly enough..
		array<EntityAI> children = new array<EntityAI>();
		m_MainItem.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER,children);
		foreach (EntityAI child: children)
		{
			InitItemValues(child);
		}
	}
	
	//! override to init context-specific values
	protected void InitItemValues(EntityAI item);
	//! called before actual item data initialization
	protected void ClearCatchingItemData()
	{
		m_QualityBaseMod = 0.0;
		m_QualityDispersionMinMod = 0.0;
		m_QualityDispersionMaxMod = 0.0;
	}
	
	//! Calculates and sets all data used in ongoing calculations
	protected void RecalculateProcessingData();
	
	bool IsValid()
	{
		return m_IsValid;
	}
	
	//result-related methods
	void GenerateResult()
	{
		YieldItemBase yItem;
		if (m_IsValid)
		{
			Class.CastTo(yItem,m_YieldsMapAll.Get(m_ProbabilityArray.GetRandomElement()));
			m_Result.SetYieldItem(yItem);
			UpdateBaseProbability(yItem);
		}
	}
	
	//! updates base probability when catching the specific item (some context subclasses only)
	void UpdateBaseProbability(YieldItemBase yItem);
	
	bool ModifySignalProbability(inout float probability)
	{
		return false;
	}
	
	float GetQualityModifier()
	{
		float ret = m_QualityBaseMod + Math.RandomFloatInclusive(m_QualityDispersionMinMod,m_QualityDispersionMaxMod);//server only, this is fine
		return ret;
	}
	
	void SetResultChanceOverride(bool ovrd, float val = 0.0)
	{
		m_Result.SetCatchChanceOverride(ovrd,val);
	}
	
	void SetResultQualityOverride(bool ovrd, float val = 0.0)
	{
		m_Result.SetQualityOverride(ovrd,val);
	}
	
	//done independently to help guide fishing action flow
	bool RollCatch()
	{
		m_Result.UpdateCatchChance(this);
		return m_Result.RollChance();
	}
	
	//done independently to help guide fishing action flow
	EntityAI SpawnAndSetupCatch(out int yItemIdx, vector v = vector.Zero)
	{
		if (!m_IsValid)
		{
			ErrorEx("Could not spawn catch for item: " + m_MainItem,ErrorExSeverity.INFO);
			return null;
		}
		m_Result.UpdateCatchQuality(this);
		
		return m_Result.SpawnAndSetup(yItemIdx,v);
	}
};
