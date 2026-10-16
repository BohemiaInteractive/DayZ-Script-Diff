//! Single door info
class AdditionalDoorInfo
{
	protected string m_DoorPath;
	
	string m_DoorType;
	string m_InteractLimitingPositionPoint; //! memory point name for limiting open/close actions on the door/window
	string m_InteractLimitingDirPoint; //! memory point name for limiting open/close actions on the door/window
	string m_DoorConstructionPartName; //! Name of the construction part linked to the doors, if any (empty default)
	string m_DoorConstructionPhysicsSource; //! Name of the AnimationSource controlling door physics (required for rebuildable doors, empty default)
	int m_LockCompatibilityBitMask; //! Lock compatibility type, now in door config. Lockpick enabled by default if undefined.
	ref TStringArray m_RelatedInventorySlotNames; //! Inventory slots that have something to do with this door. Mostly external locks?
	
	void AdditionalDoorInfo(string doorName, string doorPath)
	{
		m_DoorType = doorName;
		m_DoorPath = doorPath;
	}
	
	//parse and load whatever here
	void Init()
	{
		if (g_Game.ConfigIsExisting(m_DoorPath + " relatedInventorySlots"))
		{
			m_RelatedInventorySlotNames = new TStringArray();
			g_Game.ConfigGetTextArray(m_DoorPath + " relatedInventorySlots", m_RelatedInventorySlotNames);
		}
		
		if (g_Game.ConfigIsExisting(m_DoorPath + " lockCompatibilityBitMask"))
			m_LockCompatibilityBitMask = g_Game.ConfigGetInt(m_DoorPath + " lockCompatibilityBitMask");
		else
			m_LockCompatibilityBitMask = 1 << EBuildingLockType.LOCKPICK; //default behavior if not configured
		
		if (g_Game.ConfigIsExisting(m_DoorPath + " interactPositionPoint"))
			m_InteractLimitingPositionPoint = g_Game.ConfigGetTextOut(m_DoorPath + " interactPositionPoint");
		
		if (g_Game.ConfigIsExisting(m_DoorPath + " interactDirPoint"))
			m_InteractLimitingDirPoint = g_Game.ConfigGetTextOut(m_DoorPath + " interactDirPoint");
		
		if (g_Game.ConfigIsExisting(m_DoorPath + " doorConstructionPart"))
			m_DoorConstructionPartName = g_Game.ConfigGetTextOut(m_DoorPath + " doorConstructionPart");
		
		if (g_Game.ConfigIsExisting(m_DoorPath + " doorConstructionPhysicsSource"))
			m_DoorConstructionPhysicsSource = g_Game.ConfigGetTextOut(m_DoorPath + " doorConstructionPhysicsSource");
	}
}

//! Holds infos for all doors on a single building type
class AdditionalDoorInfosHolder
{
	protected string m_ParentType;
	protected ref map<int, ref AdditionalDoorInfo> m_DoorInfosMap = new map<int, ref AdditionalDoorInfo>();
	
	void AdditionalDoorInfosHolder(string parentType)
	{
		m_ParentType = parentType;
	}
	
	void Init()
	{
		string path = "cfgVehicles " + m_ParentType + " Doors";
		string doorName;
		string doorPath;
		AdditionalDoorInfo info;
		int count = g_Game.ConfigGetChildrenCount(path);
		for (int doorIdx = 0; doorIdx < count; ++doorIdx)
		{
			g_Game.ConfigGetChildName(path, doorIdx, doorName);
			doorPath = path + " " + doorName;
			
			info = null;
			if (g_Game.ConfigIsExisting(doorPath))
			{
				info = new AdditionalDoorInfo(doorName, doorPath);
				info.Init();
			}
			
			m_DoorInfosMap.Set(doorIdx, info);
		}
	}
	
	AdditionalDoorInfo GetSingleDoorInfo(int doorIdx)
	{
		return m_DoorInfosMap.Get(doorIdx);
	}
	
	map<int, ref AdditionalDoorInfo> GetDoorInfoMap()
	{
		return m_DoorInfosMap;
	}
}