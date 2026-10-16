class EntityAIType : EntityType
{
	protected ref TypeAttachmentGroupsDataHolder m_TypeAttachmentGroupsHolder; //! Cached shtuff from 'GUIInventoryAttachmentsProps'
	protected ref ConstructionDataTypeHolderBasic m_TypedConstructionDataHolder;
	
	float m_VarTemperatureInit;
	float m_VarTemperatureMin;
	float m_VarTemperatureMax;
	
	float m_VarTemperatureFreezeThreshold;
	float m_VarTemperatureThawThreshold;
	float m_VarTemperatureFreezeTime;
	float m_VarTemperatureThawTime;
	float m_VarTemperatureOverheatTime;

	float m_VarHeatPermeabilityCoef;
	
	float m_ConfigWeight;
	bool m_CanDisplayWeight;

 	protected void EntityAIType()
 	{
		m_VarTemperatureInit = ConfigGetFloat("varTemperatureInit");
		m_VarTemperatureMin = ConfigGetFloat("varTemperatureMin");
		m_VarTemperatureMax = ConfigGetFloat("varTemperatureMax");
		
		if (ConfigIsExisting("varTemperatureFreezePoint"))
			m_VarTemperatureFreezeThreshold = ConfigGetFloat("varTemperatureFreezePoint");
		else
			m_VarTemperatureFreezeThreshold = float.LOWEST;
		
		if (ConfigIsExisting("varTemperatureThawPoint"))
			m_VarTemperatureThawThreshold = ConfigGetFloat("varTemperatureThawPoint");
		else
			m_VarTemperatureThawThreshold = float.LOWEST;
		
		m_VarTemperatureFreezeTime = Math.Clamp(ConfigGetFloat("varTemperatureFreezeTime"),1,float.MAX);
		m_VarTemperatureThawTime = Math.Clamp(ConfigGetFloat("varTemperatureThawTime"),1,float.MAX);
		if (ConfigIsExisting("varTemperatureOverheatTime"))
			m_VarTemperatureOverheatTime = ConfigGetFloat("varTemperatureOverheatTime");
		else
			m_VarTemperatureOverheatTime = -1;

		if (ConfigIsExisting("varHeatPermeabilityCoef"))
			m_VarHeatPermeabilityCoef = ConfigGetFloat("varHeatPermeabilityCoef");
		else
			m_VarHeatPermeabilityCoef = 1;
		
		m_ConfigWeight = ConfigGetInt("weight");
		m_CanDisplayWeight = ConfigGetBool("displayWeight");
		
		string name = GetName();
		InitTypeAttachmentGroupsData(name);
		InitConstructionDataTypeHolder(name);
	}

	protected void ~EntityAIType()
	{
	}

	protected void InitTypeAttachmentGroupsData(string ownerTypeName)
	{
		m_TypeAttachmentGroupsHolder = new TypeAttachmentGroupsDataHolder(ownerTypeName);
	}
	
	protected void InitConstructionDataTypeHolder(string ownerTypeName)
	{
		string path = "cfgVehicles " + ownerTypeName + " Construction";
		if (g_Game.ConfigIsExisting(path))
		{
			m_TypedConstructionDataHolder = ConstructionDataTypeHolderBasic.Cast("ConstructionDataTypeHolder".ToType().Spawn());
			m_TypedConstructionDataHolder.InitPartsTypeDataHolder(this, path);
		}
	}
	
	TypeAttachmentGroupsDataHolder GetTypeAttachmentGroupsDataHolder()
	{
		return m_TypeAttachmentGroupsHolder;
	}
	
	//! Cast to 'ConstructionDataTypeHolder' before using in higher modules
	ConstructionDataTypeHolderBasic GetConstructionDataHolderBasic()
	{
		return m_TypedConstructionDataHolder;
	}
};
