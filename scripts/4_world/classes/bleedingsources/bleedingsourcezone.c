class BleedingSourceZone
{
	string 		m_FireSelectionName;
	int			m_Bit;
	vector		m_EmitterOffset;
	vector		m_EmitterOrientation;
	string		m_Bone;
	float		m_FlowModifier;
	int 		m_MaxTime;
	string		m_ParticleName;
	int			m_InventorySlotLocation;
	
	void BleedingSourceZone(string selection_name, int bit, vector offset, vector orientation, string bone, int max_time, float flow_modifier, string particle_name)
	{
		m_FireSelectionName = selection_name;
		m_Bit = bit;
		m_EmitterOffset = offset;
		m_EmitterOrientation = orientation;
		m_Bone = bone;
		m_FlowModifier = flow_modifier;
		m_MaxTime = max_time;
		m_ParticleName = particle_name;
	}

	//which inv location is associated(is covering) with this bleeding source ?
	void SetInvLocation(int inv_slot)
	{
		m_InventorySlotLocation = inv_slot;
	} 
	
	int GetInvLocation()
	{
		return m_InventorySlotLocation;
	}

	void ~BleedingSourceZone()
	{
	
	}
	
	string GetSelectionName()
	{
		return m_FireSelectionName;
	}
	
	int GetMaxTime()
	{
		return m_MaxTime;
	}
	
	string GetParticleName()
	{
		return m_ParticleName;
	}
	
	int GetBit()
	{
		return m_Bit;
	}
	
	float GetFlowModifier()
	{
		return m_FlowModifier;
	}
	
	vector GetOrientation()
	{
		return m_EmitterOrientation;
	}
		
	vector GetOffset()
	{
		return m_EmitterOffset;
	}
	
	string GetBoneName()
	{
		return m_Bone;
	}
	
	
}