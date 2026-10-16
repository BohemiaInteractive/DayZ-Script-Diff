typedef ItemBase Inventory_Base;
typedef ItemBaseType Inventory_BaseType;

class ItemBaseType : InventoryItemType
{
	bool m_CanShowQuantity;
	bool m_HasQuantityBar;
	bool m_CanBeDigged;
	bool m_HideSelectionsBySlot;
	bool m_CanBeSplit;

	//! Quantity config
	int	m_Count;
	int	m_VarQuantityInit;
	int	m_VarQuantityMin;
	int	m_VarQuantityMax;	
	
	//! Cleanness config
	int	m_CleannessInit;
	int	m_CleannessMin;
	int	m_CleannessMax;

	int m_LiquidContainerMask;

	int m_VarLiquidTypeInit;

	int m_QuickBarBonus;

	//! Attachment locking
	int	m_LockType;
	ref TIntArray m_CompatibleLocks;
	
	int m_ItemBehaviour; // -1 = not specified; 0 = heavy item; 1= onehanded item; 2 = twohanded item

	float m_VarStackMax;
	
	//! Wetness config
	float m_VarWetInit;
	float m_VarWetMin;
	float m_VarWetMax;

	float m_HeatIsolation;
	
	float m_ItemModelLength
	float m_ItemAttachOffset; // Offset length for when the item is attached e.g. to weapon
	
	float m_TemperaturePerQuantityWeight;
	
	string m_LockSoundSet;

	ref TIntFloatMap m_SoakingIncrements;
	ref TIntFloatMap m_DryingIncrements;
	ref TStringArray m_HeadHidingSelections;

	void ItemBaseType()
	{
		m_CanBeSplit = false;
		if (ConfigIsExisting("canBeSplit"))
			m_CanBeSplit = ConfigGetBool("canBeSplit");

		m_Count = ConfigGetInt("count");
	
		m_VarQuantityInit = ConfigGetInt("varQuantityInit");
		m_VarQuantityMin = ConfigGetInt("varQuantityMin");
		m_VarQuantityMax = ConfigGetInt("varQuantityMax");

		m_VarStackMax = ConfigGetFloat("varStackMax");
		
		m_VarWetInit = ConfigGetFloat("varWetInit");
		m_VarWetMin = ConfigGetFloat("varWetMin");
		m_VarWetMax = ConfigGetFloat("varWetMax");
		
		m_CanShowQuantity = ConfigGetBool("quantityShow");
		m_HasQuantityBar = ConfigGetBool("quantityBar");

		m_CleannessInit = ConfigGetInt("varCleannessInit");
		m_CleannessMin = ConfigGetInt("varCleannessMin");
		m_CleannessMax = ConfigGetInt("varCleannessMax");
		
		m_LiquidContainerMask = ConfigGetInt("liquidContainerType");			
	
		m_VarLiquidTypeInit = ConfigGetInt("varLiquidTypeInit");
		
		m_HeatIsolation = ConfigGetFloat("heatIsolation");
		
		m_ItemModelLength = 0.0;
		if (ConfigIsExisting("itemModelLength"))
			m_ItemModelLength = ConfigGetFloat("itemModelLength");
		
		m_ItemAttachOffset = 0.0;
		if (ConfigIsExisting("itemAttachOffset"))
			m_ItemAttachOffset = ConfigGetFloat("itemAttachOffset");
		
		m_CanBeDigged = ConfigGetBool("canBeDigged");

		m_LockType 			= ConfigGetInt("lockType");
		m_LockSoundSet 		= ConfigGetString("lockSoundSet");
		m_CompatibleLocks 	= new array<int>();
		ConfigGetIntArray("compatibleLocks", m_CompatibleLocks);
		
		m_ItemBehaviour = -1;
		if (ConfigIsExisting("itemBehaviour"))
			m_ItemBehaviour = ConfigGetInt("itemBehaviour");

		if (ConfigIsExisting("headSelectionsToHide"))
		{
			m_HeadHidingSelections = new TStringArray();
			ConfigGetTextArray("headSelectionsToHide", m_HeadHidingSelections);
		}
		
		m_HideSelectionsBySlot = false;
		if (ConfigIsExisting("hideSelectionsByinventorySlot"))
			m_HideSelectionsBySlot = ConfigGetBool("hideSelectionsByinventorySlot");
		
		m_QuickBarBonus = Math.Max(0, ConfigGetInt("quickBarBonus"));
		
		m_TemperaturePerQuantityWeight = 1.0;
		if (ConfigIsExisting("temperaturePerQuantityWeight"))
			m_TemperaturePerQuantityWeight = ConfigGetFloat("temperaturePerQuantityWeight");
		
		{
			string paramPath;
			m_SoakingIncrements = new TIntFloatMap();
			paramPath = string.Format("CfgVehicles %1 EnvironmentWetnessIncrements Soaking %2", GetName(), "parentWithLiquid");
			m_SoakingIncrements.Set(EEnvironmentSoakingCategories.PARENT_LIQUID, g_Game.ConfigGetFloat(paramPath));
			paramPath = string.Format("CfgVehicles %1 EnvironmentWetnessIncrements Soaking %2", GetName(), "wetParent");
			m_SoakingIncrements.Set(EEnvironmentSoakingCategories.PARENT_WET, g_Game.ConfigGetFloat(paramPath));

			m_DryingIncrements = new TIntFloatMap();
			paramPath = string.Format("CfgVehicles %1 EnvironmentWetnessIncrements Drying %2", GetName(), "player");
			m_DryingIncrements.Set(EEnvironmentDryingCategories.PLAYER, g_Game.ConfigGetFloat(paramPath));
			paramPath = string.Format("CfgVehicles %1 EnvironmentWetnessIncrements Drying %2", GetName(), "ground");
			m_DryingIncrements.Set(EEnvironmentDryingCategories.GROUND, g_Game.ConfigGetFloat(paramPath));
			paramPath = string.Format("CfgVehicles %1 EnvironmentWetnessIncrements Drying %2", GetName(), "playerHeatSource");
			m_DryingIncrements.Set(EEnvironmentDryingCategories.PLAYER_HEATSOURCE, g_Game.ConfigGetFloat(paramPath));
			paramPath = string.Format("CfgVehicles %1 EnvironmentWetnessIncrements Drying %2", GetName(), "groundHeatSource");
			m_DryingIncrements.Set(EEnvironmentDryingCategories.GROUND_HEATSOURCE, g_Game.ConfigGetFloat(paramPath));
		}
	}
};
