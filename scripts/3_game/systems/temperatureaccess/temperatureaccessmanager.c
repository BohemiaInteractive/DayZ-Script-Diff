class TemperatureAccessManager
{
	static ref map<ETemperatureAccessTypes,ref TemperatureAccessTypeBase> m_AccessTypesMap;
	
	void TemperatureAccessManager()
	{
		m_AccessTypesMap = new map<ETemperatureAccessTypes,ref TemperatureAccessTypeBase>;
		
		Init();
	}
	
	protected void Init()
	{
		RegisterTemperatureAccesType(TemperatureAccessTypeUnknown);
		RegisterTemperatureAccesType(TemperatureAccessTypeWorld);
		RegisterTemperatureAccesType(TemperatureAccessTypeInventory);
		RegisterTemperatureAccesType(TemperatureAccessTypeUTS);
		RegisterTemperatureAccesType(TemperatureAccessTypeFireplace);
		RegisterTemperatureAccesType(TemperatureAccessTypeCooking);
	}
	
	protected void RegisterTemperatureAccesType(typename type)
	{
		TemperatureAccessTypeBase data = TemperatureAccessTypeBase.Cast(type.Spawn());
		m_AccessTypesMap.Set(data.GetAccessTypeID(),data);
	}
	
	static TemperatureAccessTypeBase GetTAData(ETemperatureAccessTypes id)
	{
		return m_AccessTypesMap[id];
	}
}
