//! instanced data holder for single construction material
class ConstructionMaterialData
{
	bool m_MaterialLockable;
	bool m_SkipOnRepair = false; //does not require the material to repair
	bool m_SkipOnDismantle = false; //does not return the material on dismantle
	int m_MaterialSlotID;
	float m_MaterialQuantity;
	string m_MaterialType;
	string m_MaterialSlotName;
	

	void ConstructionMaterialData(string cfgPath)
	{
		Init(cfgPath);
	}
	
	protected void Init(string cfgPath)
	{
		m_MaterialLockable = false;
		m_MaterialSlotID = InventorySlots.INVALID;
		m_MaterialQuantity = -1;
		m_MaterialType = "";
		m_MaterialSlotName = "";
		
		g_Game.ConfigGetText(cfgPath + " type", m_MaterialType);
		g_Game.ConfigGetText(cfgPath + " slot_name", m_MaterialSlotName);
		m_MaterialSlotID = InventorySlots.GetSlotIdFromString(m_MaterialSlotName);
		m_MaterialQuantity = g_Game.ConfigGetFloat(cfgPath + " quantity");
		m_MaterialLockable = g_Game.ConfigGetInt(cfgPath + " lockable");
		m_SkipOnRepair = g_Game.ConfigGetInt(cfgPath + " skipOnRepair");
		m_SkipOnDismantle = g_Game.ConfigGetInt(cfgPath + " skipOnDismantle");
	}
}
