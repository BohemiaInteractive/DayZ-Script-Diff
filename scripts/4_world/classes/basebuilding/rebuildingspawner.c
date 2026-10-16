#ifdef DIAG_DEVELOPER
class RebuildingSpawner
{
	static PlayerBase m_Player;
	static EntityAI m_Rebuidable;
	static ref array<string> m_AllRebuildableTypes;
	static ref array<string> m_RebuildableTypesUsed = {};
	static float m_Delay = 3000.0; //ms
	static float m_Dist = 10.0; //m
	static int m_LastIdx = -1;
	static bool m_IsSpawned;
	static bool m_Loop;
	static vector m_SpawnPos;
	
	static int m_DbgLockCounter;
	
	static void StartRebuildablesSpawning(bool loop = false, array<string> overrides = null)
	{
		//in case you also doubleclick by accident
		StopRebuildablesSpawning();
		
		if (!overrides)
		{
			if (!m_AllRebuildableTypes)
				InitAllRebuildableTypes();
			
			m_RebuildableTypesUsed.Copy(m_AllRebuildableTypes);
		}
		else
		{
			m_RebuildableTypesUsed.Copy(overrides);
		}
		
		if (g_Game.IsDedicatedServer())
			m_Player = PlayerBase.Cast(g_Game.GetPlayerByIndex());
		else
			m_Player = PlayerBase.Cast(g_Game.GetPlayer());
		m_SpawnPos = m_Player.GetPosition() + m_Player.GetDirection() * m_Dist;
		
		m_LastIdx = -1;
		m_Loop = loop;
		if (m_Rebuidable)
			m_Rebuidable.DeleteSafe();
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ProcessRebuildablesSpawning, m_Delay);
	}
	
	static void StopRebuildablesSpawning()
	{
		if (m_Rebuidable)
			m_Rebuidable.DeleteSafe();
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ProcessRebuildablesSpawning);
	}
	
	///////////////////////////////////
	
	protected static void InitAllRebuildableTypes()
	{
		string basePath = "cfgVehicles";
		string typeName;
		m_AllRebuildableTypes = new array<string>();
		int count = g_Game.ConfigGetChildrenCount(basePath);
		for (int q = 0; q < count; ++q)
		{
			g_Game.ConfigGetChildName(basePath, q, typeName);
			string cfgPath = "cfgVehicles " + typeName + " Construction";
			if (g_Game.ConfigIsExisting(cfgPath) && !g_Game.IsKindOf(typeName, "BaseBuildingBase"))
				m_AllRebuildableTypes.Insert(typeName);
		}
	}
	
	protected static void ProcessRebuildablesSpawning()
	{
		if (m_Rebuidable)
		{
			m_Rebuidable.DeleteSafe();
			
		if (m_LastIdx >= m_RebuildableTypesUsed.Count() - 1)
			{
				if (m_Loop)
					m_LastIdx = -1;
				else
					return;
			}
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ProcessRebuildablesSpawning, m_Delay);
		}
		else
		{
			++m_LastIdx;
			if (m_LastIdx >= m_RebuildableTypesUsed.Count())
				return;
			
			SpawnAndRebuildAllParts(m_RebuildableTypesUsed[m_LastIdx]);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ProcessRebuildablesSpawning, m_Delay);
		}
	}
	
	protected static void SpawnAndRebuildAllParts(string type)
	{
		m_Rebuidable = EntityAI.Cast(g_Game.CreateObjectEx(type, m_SpawnPos, ECE_PLACE_ON_SURFACE));
		ConstructionBase con = ConstructionBase.Cast(m_Rebuidable.GetConstructionBasic());
		map<string, ref ConstructionPart> partsMap = con.GetConstructionParts(); //simple array under define, too lazy to solve now
		foreach (string name, ref ConstructionPart part : partsMap) 
		{
			con.BuildPartServerEx(null, name, -1);
		}
	}
	
	static void RebuildAllWoodenPartsFiltered(ConstructionBase con, EntityAI ent)
	{
		TStringArray wallsDoored = new TStringArray();
		TStringArray wallsWindowed = new TStringArray();
		TStringArray wallsSimple = new TStringArray();
		TStringArray doors = new TStringArray();
		TStringArray windows = new TStringArray();
		
		string tmp;
		GetCLIParam("RBLDRebuildEverythingWithLocks", tmp);
		int maxTotalLockCount = tmp.ToInt();
		GameInventory inv = ent.GetInventory();
		
		//Print("DbgLocks | Entity: " + ent + " | lock count: " + (RebuildingSpawner.m_DbgLockCounter - 1));
		//go through all parts first, directly build the simple ones
		array<ConstructionPart> constructionPartArray = con.GetConstructionPartArray();
		foreach (ConstructionPart part : constructionPartArray)
		{
			if (part.IsUtilityPart())
				continue;
			
			string partName = part.GetPartName();
			if (partName.Contains("rubble") || partName.Contains("roof"))
			{
				con.BuildPartServerEx(null, partName, -1);
				continue;
			}
			
			if (!partName.Contains("_wood"))
				continue;
			
			if (partName.Contains("wall"))
			{
				if (partName.Contains("_window_"))
					wallsWindowed.Insert(partName);
				if (partName.Contains("_door_"))
					wallsDoored.Insert(partName);
				else
					wallsSimple.Insert(partName);
			}
			else if (partName.Contains("door"))
			{
				doors.Insert(partName);
			}
			else if (partName.Contains("window"))
			{
				windows.Insert(partName);
			}
		}
		
		//1. walls with doors
		foreach (string wallWithDoors : wallsDoored)
		{
			if (!con.HasConflictPart(wallWithDoors))
				con.BuildPartServerEx(null, wallWithDoors, -1);
		}
		
		//2. walls with windows
		foreach (string wallWithWindows : wallsWindowed)
		{
			if (!con.HasConflictPart(wallWithWindows))
				con.BuildPartServerEx(null, wallWithWindows, -1);
		}
		
		//3. simple walls
		foreach (string wallSimple : wallsSimple)
		{
			if (!con.HasConflictPart(wallSimple))
				con.BuildPartServerEx(null, wallSimple, -1);
		}
		
		//4. doors
		foreach (string door : doors)
		{
			if (!con.HasConflictPart(door) && con.HasRequiredPart(door)) //later parts need to check requirements too
			{
				con.BuildPartServerEx(null, door, -1);
				
				if (m_DbgLockCounter > maxTotalLockCount)
					continue;
				
				int idx1 = door.IndexOf("door") + 4;
				int idx2 = door.IndexOf("_wood");
				string attSlotName = door.Substring(idx1, idx2 - idx1); //slot number
				attSlotName = string.Format("Att_CodeLock%1_Wood", attSlotName);
				int slotID = InventorySlots.GetSlotIdFromString(attSlotName);
				DigitalCodeLock lock;
				
				if (Class.CastTo(lock, inv.FindAttachment(slotID))) //found old
				{
					++m_DbgLockCounter;
				}
				else if (!lock && Class.CastTo(lock, inv.CreateAttachmentEx("DigitalCodeLock", slotID))) //or spawned new
				{
					++m_DbgLockCounter;
					lock.OnDebugSpawn();
				}
			}
		}
		
		//5. windows
		foreach (string window : windows)
		{
			if (!con.HasConflictPart(door) && con.HasRequiredPart(window)) //later parts need to check requirements too
				con.BuildPartServerEx(null, window, -1);
		}
	}
}
#endif