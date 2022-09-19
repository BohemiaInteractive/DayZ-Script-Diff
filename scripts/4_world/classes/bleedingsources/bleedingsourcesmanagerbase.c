class BleedingSourcesManagerBase
{
	ref map<int, ref BleedingSource> m_BleedingSources = new map<int, ref BleedingSource>;
	ref map<string, ref BleedingSourceZone> m_BleedingSourceZone = new map<string, ref BleedingSourceZone>;
	ref map<int, int> m_BleedingSourcesByLocation = new map<int, int>;
	static ref map<int, int> m_BleedingSourcesZonesMaskByLocation = new map<int, int>;//for each inventory location, keep a bitmask where a bit is set to 1 for each bleeding source zone that belongs to that location
	static ref set<int> m_BleedingSourcesLocationsList = new set<int>;
	ItemBase m_Item;//item used to remove the bleeding source
	PlayerBase m_Player;
	//ref map<string, int> m_FireGeomToBit = new map<string, int>;
	ref map<int, string> m_BitToFireGeom = new map<int, string>;
	protected int m_Bit;
	int m_BitOffset = 0;
	
	void BleedingSourcesManagerBase( PlayerBase player )
	{
		m_Player = player;
		Init();
	}
	
	protected void Init()
	{
		//dmgZone_head
		RegisterBleedingZoneEx("Head",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 90 0" , "0 0 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_HIGH, "BleedingSourceEffect",InventorySlots.MASK);
		//dmgZone_torso
		RegisterBleedingZoneEx("Neck", PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL,"", "-180 0 0" , "0.02 -0.05 0.05", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_HIGH, "BleedingSourceEffect",InventorySlots.HEADGEAR);
		RegisterBleedingZoneEx("Pelvis",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 -135 0" , "0 0.12 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_HIGH, "BleedingSourceEffect",InventorySlots.BODY);
		RegisterBleedingZoneEx("Spine",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 55 0" , "0 -0.1 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_HIGH, "BleedingSourceEffect",InventorySlots.BODY);
		RegisterBleedingZoneEx("Spine1",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 155 0" , "0 -0.1 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_HIGH, "BleedingSourceEffect",InventorySlots.BODY);
		RegisterBleedingZoneEx("Spine2",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 55 0" , "0 -0.07 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_HIGH, "BleedingSourceEffect",InventorySlots.BODY);
		RegisterBleedingZoneEx("Spine3",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 155 0" , "0 -0.05 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_HIGH, "BleedingSourceEffect",InventorySlots.BODY);
		//dmgZone_leftArm
		RegisterBleedingZoneEx("LeftShoulder",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 -90 0" , "0 0.07 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect", InventorySlots.BODY);
		RegisterBleedingZoneEx("LeftArm", PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "","0 90 90" , "0 -0.05 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.BODY);
		RegisterBleedingZoneEx("LeftArmRoll",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 90 0" , "0 -0.05 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.BODY);
		RegisterBleedingZoneEx("LeftForeArm",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 90 0" , "0 -0.05 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.BODY);
		//dmgZone_rightArm
		RegisterBleedingZoneEx("RightShoulder",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 90 0" , "0 -0.07 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.BODY);
		RegisterBleedingZoneEx("RightArm", PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL,"", "0 -90 0" , "0 0.05 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.BODY);
		RegisterBleedingZoneEx("RightArmRoll",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 -90 0" , "0 0.05 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.BODY);
		RegisterBleedingZoneEx("RightForeArm",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 -90 0" , "0 0.05 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.BODY);
		//dmgZone_leftHand
		RegisterBleedingZoneEx("LeftForeArmRoll",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 90 0" , "0.1 0 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_LOW, "BleedingSourceEffectLight", InventorySlots.GLOVES);
		//dmgZone_rightHand
		RegisterBleedingZoneEx("RightForeArmRoll",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 90 0" , "-0.1 0 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_LOW, "BleedingSourceEffectLight", InventorySlots.GLOVES);
		//dmgZone_leftLeg
		RegisterBleedingZoneEx("LeftLeg",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 90 0" , "0 -0.07 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.LEGS);
		RegisterBleedingZoneEx("LeftLegRoll",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 90 0" , "0 -0.07 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.LEGS);
		RegisterBleedingZoneEx("LeftUpLeg",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 90 0" , "0 -0.12 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.LEGS);
		RegisterBleedingZoneEx("LeftUpLegRoll",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 90 0" , "0 -0.07 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM , "BleedingSourceEffect",InventorySlots.LEGS);
		//dmgZone_rightLeg
		RegisterBleedingZoneEx("RightLeg",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 -90 0" , "0 0.06 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.LEGS);
		RegisterBleedingZoneEx("RightLegRoll",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 -90 0" , "0 0 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.LEGS);
		RegisterBleedingZoneEx("RightUpLeg",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 -90 0" , "0 0.12 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.LEGS);
		RegisterBleedingZoneEx("RightUpLegRoll",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 -90 0" , "0 0.06 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_MEDIUM, "BleedingSourceEffect",InventorySlots.LEGS);
		//dmgZone_leftFoot
		RegisterBleedingZoneEx("LeftFoot",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 180 0" , "0 0 0.035", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_LOW, "BleedingSourceEffectLight", InventorySlots.LEGS);
		RegisterBleedingZoneEx("LeftToeBase",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 -90 0" , "0 0.07 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_LOW, "BleedingSourceEffect",InventorySlots.FEET);
		//dmgZone_rightFoot
		RegisterBleedingZoneEx("RightFoot",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 0 0" , "0 0 -0.035", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_LOW, "BleedingSourceEffectLight", InventorySlots.LEGS);
		RegisterBleedingZoneEx("RightToeBase",PlayerConstants.BLEEDING_SOURCE_DURATION_NORMAL, "", "0 90 0" , "0 -0.07 0", PlayerConstants.BLEEDING_SOURCE_FLOW_MODIFIER_LOW, "BleedingSourceEffect", InventorySlots.FEET);
	}
	
	protected void SetItem(ItemBase item)
	{
		m_Item = item;
	}
	
	
	set<int> GetBleedingSourcesLocations()
	{
		return m_BleedingSourcesLocationsList;
	}
	
	
	//for inventory location, get the active bits
	int GetBleedingSourceBitsByInvLocation(int inv_location)
	{
		if(m_BleedingSourcesByLocation.Contains(inv_location))
		{
			return m_BleedingSourcesByLocation.Get(inv_location);
		}
		return 0;
	}

	//for inventory location, get the active bits
	int GetFreeBleedingSourceBitsByInvLocation(int inv_location)
	{
		if(m_BleedingSourcesZonesMaskByLocation.Contains(inv_location))
		{
			return (~m_BleedingSourcesZonesMaskByLocation.Get(inv_location) & GetBleedingSourceBitsByInvLocation(inv_location)) | (m_BleedingSourcesZonesMaskByLocation.Get(inv_location) & ~GetBleedingSourceBitsByInvLocation(inv_location));//xor
		}
		return 0;
	}
	
	
	protected int GetBitFromSelectionID(int id)
	{
		CachedObjectsArrays.ARRAY_STRING.Clear();
		m_Player.GetActionComponentNameList(id, CachedObjectsArrays.ARRAY_STRING, "fire");
		
		for (int i = 0; i < CachedObjectsArrays.ARRAY_STRING.Count(); i++)
		{
			/*
			string name = CachedObjectsArrays.ARRAY_STRING.Get(i);
			PrintString(name);
			*/
			int bit = GetBitFromSelectionName(CachedObjectsArrays.ARRAY_STRING.Get(i));
			if (  bit !=0 )
			{
				return bit;
			}
		}
		return 0;
	}
	
	protected BleedingSourceZone GetBleedingSourceMeta(int bit)
	{
		return m_BleedingSourceZone.Get(GetSelectionNameFromBit(bit));
	}
	
	int GetRegisteredSourcesCount()
	{
		return m_BitOffset;
	}
	
	protected void RegisterBleedingZoneEx(string name, int max_time, string bone = "", vector orientation = "0 0 0", vector offset = "0 0 0", float flow_modifier = 1, string particle_name = "BleedingSourceEffect", int inv_location = 0)
	{
		if ( m_BitOffset == BIT_INT_SIZE)
		{
			Error("Too many bleeding sources, max is "+BIT_INT_SIZE.ToString());
		}
		else
		{
			name.ToLower();
			//PrintString(name);
			//int bit = Math.Pow(2, m_BitOffset);
			m_Bit = 1 << m_BitOffset;
			//PrintString(bit.ToString());
			string bone_name = bone;
			
			if (bone_name == "")
			{
				bone_name = name;
			}
			
			m_BleedingSourceZone.Insert(name, new BleedingSourceZone(name, m_Bit, offset, orientation, bone_name, max_time, flow_modifier, particle_name));
			m_BleedingSourceZone.Get(name).SetInvLocation(inv_location);
			m_BitToFireGeom.Insert(m_Bit, name);
			m_BleedingSourcesZonesMaskByLocation.Set( inv_location, m_BleedingSourcesZonesMaskByLocation.Get(inv_location) | m_Bit);//set a bit to 1 to already existing bitmask for that location
			m_BleedingSourcesLocationsList.Insert(inv_location);
			m_BitOffset++;
		}
	}
	protected void RegisterBleedingZone(string name, int max_time, string bone = "", vector orientation = "0 0 0", vector offset = "0 0 0", float flow_modifier = 1, string particle_name = "BleedingSourceEffect")
	{
		RegisterBleedingZoneEx(name, max_time, bone, orientation, offset, flow_modifier, particle_name);
	}
	
	void RemoveAllSources()
	{
		//int count = m_BleedingSources.Count();
		while (m_BleedingSources.Count() > 0 )
		{
			int bit = m_BleedingSources.GetKey(0);
			RemoveBleedingSource(bit);
		}
	}
	
	protected int GetBitFromSelectionName(string name)
	{
		if (!m_BleedingSourceZone.Get(name))
		{
			return 0;
		}
		return m_BleedingSourceZone.Get(name).GetBit();
	}	
	
	protected string GetSelectionNameFromBit(int bit)
	{
		return m_BitToFireGeom.Get(bit);
	}
	
	bool AttemptAddBleedingSource(int component)
	{
		int bit = GetBitFromSelectionID(component);
		
		if ( bit == 0 )
		{
			return false;
		}
		
		if ( CanAddBleedingSource(bit) )
		{
			AddBleedingSource(bit);
			return true;
		}
		
		return false;
	}
	
	bool AttemptAddBleedingSourceBySelection(string selection_name)
	{
		selection_name.ToLower();
		int bit = GetBitFromSelectionName(selection_name);
		
		if ( bit == 0 )
		{
			return false;
		}
		
		if ( CanAddBleedingSource(bit) )
		{
			AddBleedingSource(bit);
			return true;
		}
		return false;
	}
	
	
	bool AttemptAddBleedingSourceDirectly(int bit, eBleedingSourceType type = eBleedingSourceType.NORMAL, int context = 0)
	{
		if(CanAddBleedingSource(bit))
		{
			AddBleedingSourceEx(bit, type, context);
			return true;
		}  
		return false;
	}
	
	
	protected bool CanAddBleedingSource(int bit)
	{
		if (!GetBleedingSourceMeta(bit)) return false;
		return ((m_Player.GetBleedingBits() & bit) == 0 );
	}
	
	protected void AddBleedingSourceEx(int bit, eBleedingSourceType type = eBleedingSourceType.NORMAL, int context = 0)
	{
		AddBleedingSource(bit);
		m_BleedingSources.Get(bit).SetType(type);
	}
	
	protected void AddBleedingSource(int bit)
	{
		BleedingSourceZone bsz = GetBleedingSourceMeta(bit);
		
		vector orientation = bsz.GetOrientation();
		vector offset = bsz.GetOffset();
		string bone_name =  bsz.GetBoneName();
		float flow_modifier = bsz.GetFlowModifier();
		int max_time = bsz.GetMaxTime();
		string particle_name = bsz.GetParticleName();
		int inventory_loc = bsz.GetInvLocation();
		m_BleedingSourcesByLocation.Set(inventory_loc, (m_BleedingSourcesByLocation.Get(inventory_loc) | bit));
		m_BleedingSources.Insert(bit, new BleedingSource(m_Player, bit,bone_name, orientation, offset, max_time, flow_modifier, particle_name) );
		
		m_Player.OnBleedingSourceAdded();
	}
	
	int GetBleedingSourceActiveTime(int bit)
	{
		int time = -1;
		if (m_BleedingSources.Contains(bit))
		{
			time = m_BleedingSources.Get(bit).GetActiveTime();
		}
		return time;
	}
	
	void SetBleedingSourceActiveTime(int bit, int time)
	{
		if (m_BleedingSources.Contains(bit))
		{
			m_BleedingSources.Get(bit).SetActiveTime(time);
		}
	}
	
	void SetBleedingSourceType(int bit, eBleedingSourceType type)
	{
		if (m_BleedingSources.Contains(bit))
		{
			m_BleedingSources.Get(bit).SetType(type);
		}
	}	
	eBleedingSourceType GetBleedingSourceType(int bit)
	{
		if (m_BleedingSources.Contains(bit))
		{
			return m_BleedingSources.Get(bit).GetType();
		}
		return -1;
	}
	
	protected bool RemoveBleedingSource(int bit)
	{
		if (m_BleedingSources.Contains(bit))
		{
			BleedingSourceZone bsz = GetBleedingSourceMeta(bit);
			int inventory_loc = bsz.GetInvLocation();
			m_BleedingSourcesByLocation.Set(inventory_loc, (m_BleedingSourcesByLocation.Get(inventory_loc) & ~bit));//deactivate the bit
			m_BleedingSources.Remove(bit);
			m_Player.OnBleedingSourceRemovedEx(m_Item);
			return true;
		}
		return false;
	}

	int GetBleedingSourcesCount()
	{
		return m_BleedingSources.Count();
	}
	
	/*void ChangeBleedingIndicatorVisibility(bool visible)
	{
		int count = m_BleedingSources.Count();
		for (int i = 0; i < count; i++)
		{
			m_BleedingSources.GetElement(i).ToggleSourceBleedingIndication(visible);
		}
	}*/
}