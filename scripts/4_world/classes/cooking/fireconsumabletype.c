class FireConsumableType
{
	typename 	m_ItemType;
	float 		m_Energy;
	bool 		m_IsKindling;
	string 		m_AttSlot;
	
	void FireConsumableType ( typename item_type, float energy, bool is_kindling, string att_slot )
	{
		m_ItemType = item_type;
		m_Energy = energy;
		m_IsKindling = is_kindling;
		m_AttSlot = att_slot;
	}
	
	//Item typename
	typename GetItemType()
	{
		return m_ItemType;
	}
	
	//Energy
	float GetEnergy()
	{
		return m_Energy;
	}
	
	//Is Kindling
	bool IsKindling()
	{
		return m_IsKindling;
	}
	
	//Attachment slot
	string GetAttSlot()
	{
		return m_AttSlot;
	}
}
