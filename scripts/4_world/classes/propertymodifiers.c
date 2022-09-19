class PropertyModifiers
{
	vector m_RecoilModifiers;
	vector m_SwayModifiers;
	vector m_SightMisalignment;
	
	float m_BarrelLength;
	//ref Timer test_timer;
	ItemBase m_OwnerItem;
	float m_Weapon
	
	void PropertyModifiers(ItemBase owner)
	{
		//test_timer = new Timer();
		//test_timer.Run(5, this, "Refresh", NULL, true);
		m_OwnerItem = owner;
		UpdateModifiers();
		m_BarrelLength = CalculateBarrelLength(owner);
	}
	
	//! Get Barrel Legth in mm
	float GetBarrelLength()
	{
		//return m_BarrelLength * 1000:
		return CalculateBarrelLength(m_OwnerItem) * 1000;
	}
	
	float CalculateBarrelLength(ItemBase owner)
	{
		vector usti_hlavne_position = owner.GetSelectionPositionLS( "usti hlavne" );//usti hlavne
		vector konec_hlavne_position = owner.GetSelectionPositionLS( "konec hlavne" );//konec hlavne
		usti_hlavne_position = owner.ModelToWorld(usti_hlavne_position);
		konec_hlavne_position = owner.ModelToWorld(konec_hlavne_position);
		return vector.Distance(usti_hlavne_position, konec_hlavne_position);
	}


	void UpdateModifiers()
	{
		m_RecoilModifiers = GetModifierRaw(m_OwnerItem, "recoilModifier");
		m_SwayModifiers = GetModifierRaw(m_OwnerItem, "swayModifier");
		m_SightMisalignment = GetModifierRaw(m_OwnerItem, "sightMisalignmentModifier");
		
		for (int i = 0; i < m_OwnerItem.GetInventory().AttachmentCount(); i++)
		{
			ItemBase attachment = ItemBase.Cast(m_OwnerItem.GetInventory().GetAttachmentFromIndex(i));
			
			vector temp_value = PropertyModifiers.GetModifierRaw(attachment, "recoilModifier");
			m_RecoilModifiers[0] = m_RecoilModifiers[0] * temp_value[0];
			m_RecoilModifiers[1] = m_RecoilModifiers[1] * temp_value[1];
			m_RecoilModifiers[2] = m_RecoilModifiers[2] * temp_value[2];
			
			temp_value = PropertyModifiers.GetModifierRaw(attachment, "swayModifier");
			m_SwayModifiers[0] = m_SwayModifiers[0] * temp_value[0];
			m_SwayModifiers[1] = m_SwayModifiers[1] * temp_value[1];
			m_SwayModifiers[2] = m_SwayModifiers[2] * temp_value[2];
			
			temp_value = PropertyModifiers.GetModifierRaw(attachment, "sightMisalignmentModifier");
			m_SightMisalignment[0] = m_SightMisalignment[0] * temp_value[0];
			m_SightMisalignment[1] = m_SightMisalignment[1] * temp_value[1];
			m_SightMisalignment[2] = m_SightMisalignment[2] * temp_value[2];
		}
	}

	static vector GetModifierRaw(ItemBase item, string modifier_name, vector value = "1 1 1")
	{
		if( item.ConfigIsExisting(modifier_name) )
		{
			value = item.ConfigGetVector(modifier_name);
		}
		return value;
	}
	
}