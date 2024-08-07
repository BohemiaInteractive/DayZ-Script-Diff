class TemperatureAccessTypeBase
{
	protected ETemperatureAccessTypes m_ID = ETemperatureAccessTypes.ACCESS_UNKNOWN;
	protected int m_Priority;
	
	void TemperatureAccessTypeBase()
	{
		Init();
	}
	
	protected void Init()
	{
		m_ID = ETemperatureAccessTypes.ACCESS_UNKNOWN;
		m_Priority = ETemperatureAccessPriority.PRIO_MAX;
	}
	
	ETemperatureAccessTypes GetAccessTypeID()
	{
		return m_ID;
	}
	
	int GetPriority()
	{
		return m_Priority;
	}
}

class TemperatureAccessTypeUnknown : TemperatureAccessTypeBase {};

class TemperatureAccessTypeWorld : TemperatureAccessTypeBase
{
	override protected void Init()
	{
		m_ID = ETemperatureAccessTypes.ACCESS_WORLD;
		m_Priority = ETemperatureAccessPriority.PRIO_0;
	}
}

class TemperatureAccessTypeInventory : TemperatureAccessTypeBase
{
	override protected void Init()
	{
		m_ID = ETemperatureAccessTypes.ACCESS_INVENTORY;
		m_Priority = ETemperatureAccessPriority.PRIO_2;
	}
}

class TemperatureAccessTypeUTS : TemperatureAccessTypeBase
{
	override protected void Init()
	{
		m_ID = ETemperatureAccessTypes.ACCESS_UTS;
		m_Priority = ETemperatureAccessPriority.PRIO_1;
	}
}

class TemperatureAccessTypeFireplace : TemperatureAccessTypeBase
{
	override protected void Init()
	{
		m_ID = ETemperatureAccessTypes.ACCESS_FIREPLACE;
		m_Priority = ETemperatureAccessPriority.PRIO_2;
	}
}

class TemperatureAccessTypeCooking : TemperatureAccessTypeBase
{
	override protected void Init()
	{
		m_ID = ETemperatureAccessTypes.ACCESS_COOKING;
		m_Priority = ETemperatureAccessPriority.PRIO_2;
	}
}