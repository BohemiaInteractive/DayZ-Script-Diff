class Bunker : House
{
	protected const float DOOR_AUTOCLOSE_TIME = 12;
	private bool m_IsActive;

	protected ref map<int, ref Timer> m_AutoCloseTimers;

	protected DigitalCodeLock_Bunker m_CodeLock;
	
	void Bunker()
	{
		RegisterNetSyncVariableBool("m_IsActive");
	}

	// --------------------------------------------------------------------------------
	
	CodeLockComponent GetCodeLockComponent()
	{
		return m_CodeLock.GetCodeLockComponent();
	}
	
	// --------------------------------------------------------------------------------
	
	bool HasCodeLock()
	{
		return m_CodeLock != null;
	}
	
	// --------------------------------------------------------------------------------
	
	DigitalCodeLock_Bunker GetCodeLock()
	{
		return m_CodeLock;
	}

	// --------------------------------------------------------------------------------
	
	override void DeferredInit()
	{
		super.DeferredInit();
		
		g_Game.RegisterNetworkStaticObject(this);
	}

	// --------------------------------------------------------------------------------

	override bool CanDoorBeOpened(notnull DoorManipulationParams params)
	{
		if (IsMainDoor(params.m_DoorIndex))
		{
			AdditionalDoorInfo doorInfo = GetDoorInfo(params.m_DoorIndex);
			string memPointStartName = string.Format("%1_action", doorInfo.m_DoorType);
			string memPointEndName = string.Format("%1_inside", doorInfo.m_DoorType);
			
			bool insideAndInactive = MiscGameplayFunctions.MemPointDirectionalCheck(PlayerBase.Cast(params.m_Caller), this, memPointStartName, memPointEndName) && super.CanDoorBeOpened(params);
			bool outside = m_IsActive && !GetCodeLock().IsLocked() && super.CanDoorBeOpened(params);

			return insideAndInactive || outside;
		}
		
		return super.CanDoorBeOpened(params);
	}
	
	// --------------------------------------------------------------------------------
	
	override bool CanDoorBeLocked(int doorIndex)
	{
		return false;
	}
	
	// --------------------------------------------------------------------------------
	
	override void OnDoorOpenStart(DoorStartParams params)
	{
		if (!IsMainDoor(params.param1))
			return;

		if (!m_AutoCloseTimers)
			m_AutoCloseTimers = new map<int, ref Timer>();
		
		Timer doorTimer = new Timer();
		doorTimer.Run(DOOR_AUTOCLOSE_TIME, this, "AutoCloseDoor", new Param1<int>(params.param1));
		m_AutoCloseTimers.Set(params.param1, doorTimer);
	}
	
	override void OnDoorCloseStart(DoorStartParams params)
	{
		if (HasCodeLock())
			GetCodeLock().SetLockState(true);
	}
	
	// --------------------------------------------------------------------------------

	void SetActive(bool isActive)
	{
		m_IsActive = isActive;
		SetSynchDirty();
	}
	
	// --------------------------------------------------------------------------------
	
	//! adapt this to your lockable doors (model specific)
	int GetMainDoorIndex()
	{
		return 0;
	}
	
	// --------------------------------------------------------------------------------
	
	protected bool IsMainDoor(int doorIndex)
	{
		return (doorIndex == GetMainDoorIndex());
	}

	// --------------------------------------------------------------------------------
	
	protected void AutoCloseDoor(int doorIndex)
	{
		CloseDoor(doorIndex);
		RemoveDoorTimer(doorIndex);
	}

	// --------------------------------------------------------------------------------
	
	protected void RemoveDoorTimer(int doorIndex)
	{
		if (m_AutoCloseTimers)
			m_AutoCloseTimers.Remove(doorIndex);
	}

	// --------------------------------------------------------------------------------
	
	void OnBunkerActivated(bool loadedAsActive, string codeLockCode)
	{
		DigitalCodeLock_Bunker codeLock = GetCodeLock();
		codeLock.SetLockCode(codeLockCode);
		codeLock.SetLockState(true);
		SetActive(true);
	}

	// --------------------------------------------------------------------------------
	
	void OnBunkerDeactivated()
	{
		DigitalCodeLock_Bunker codeLock = GetCodeLock();
		codeLock.SetLockState(false);
		codeLock.Disable();
		SetActive(false);
	}

	// --------------------------------------------------------------------------------
	
	#ifdef DIAG_DEVELOPER
	override string GetDebugText()
	{
		string text = super.GetDebugText();
		
		if (m_CodeLock)
		{
			text += string.Format("Code: %1\n", m_CodeLock.GetLockPIN());
			text += string.Format("Active: %1\n", m_IsActive.ToString());
		}

		return text;
	}
	#endif

}

class BroadcastedBunker : Bunker
{
	protected static const int RESOLVE_CODELOCK_RETRY_DELAY = 200;	//! ms between client retries while waiting for the codelock attachment to replicate

	protected const string CODE_LOCK_ATTACHMENT_NAME		= "att_codelock";
	protected const string CRATE_SPAWN_POSITION_MEM_NAME 	= "storagecratepos";
	protected const string STORAGE_BOX_CLASSNAME 			= "StorageBox_Wooden";

	private EntityAI m_StorageBox;

	// --------------------------------------------------------------------------------


	override void DeferredInit()
	{
	    super.DeferredInit();

		ResolveCodeLock();
	}

	// --------------------------------------------------------------------------------

	protected void ResolveCodeLock()
	{
	    int slotId = InventorySlots.GetSlotIdFromString(CODE_LOCK_ATTACHMENT_NAME);
	    EntityAI attachment = GetInventory().FindAttachment(slotId);
	    if (attachment)
	    {
	        m_CodeLock = DigitalCodeLock_Bunker.Cast(attachment);
	        return;
	    }

		if (g_Game.IsServer())
		{
			m_CodeLock = DigitalCodeLock_Bunker.Cast(GetInventory().CreateAttachmentEx("DigitalCodeLock_Bunker", slotId));
			m_CodeLock.SetAnimationPhase(CodeLockVisualManager.CODELOCK_ANIMATED_ZIP, 1.0);
			return;
		}

		//! client re-sync in case it is not up to date
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ResolveCodeLock, RESOLVE_CODELOCK_RETRY_DELAY, false);
	}

	// --------------------------------------------------------------------------------
	
	override DigitalCodeLock_Bunker GetCodeLock()
	{
		if (!m_CodeLock)
		{
			int slotId = InventorySlots.GetSlotIdFromString(CODE_LOCK_ATTACHMENT_NAME);
			m_CodeLock = DigitalCodeLock_Bunker.Cast(GetInventory().FindAttachment(slotId));
		}

		return m_CodeLock;
	}
	
	// --------------------------------------------------------------------------------
	
	override protected void OnBunkerActivated(bool loadedAsActive, string codeLockCode)
	{
		bool wasInitialized = IsInitialized();

		if (!wasInitialized)
			return;

		super.OnBunkerActivated(loadedAsActive, codeLockCode);

		if (!m_StorageBox)
			m_StorageBox = FindOrSpawnStorageBox(loadedAsActive);
	}

	// --------------------------------------------------------------------------------
	
	override protected void OnBunkerDeactivated()
	{
		super.OnBunkerDeactivated();

		if (m_StorageBox)
		{
			#ifndef SERVER
			m_StorageBox.DeleteSafe();
			#else
			m_StorageBox.SetLifetime(5.0);
			#endif
		}
	}

	// --------------------------------------------------------------------------------

	private EntityAI FindOrSpawnStorageBox(bool preferExisting)
	{
		if (preferExisting)
		{
			EntityAI existing = FindExistingStorageBox();
			if (existing)
				return existing;
		}

		return SpawnStorageBox();
	}

	// --------------------------------------------------------------------------------

	private EntityAI FindExistingStorageBox()
	{
		if (!MemoryPointExists(CRATE_SPAWN_POSITION_MEM_NAME))
			return null;

		vector spawnPosWorld = ModelToWorld(GetMemoryPointPos(CRATE_SPAWN_POSITION_MEM_NAME));

		array<Object> objects = new array<Object>();
		array<CargoBase> proxyCargos = new array<CargoBase>();
		g_Game.GetObjectsAtPosition3D(spawnPosWorld, 1.5, objects, proxyCargos);

		float bestDist 	= 999999;
		EntityAI bestBox = null;

		foreach (Object obj : objects)
		{
			if (StorageBox_Colorbase.Cast(obj))
			{
				float dist = vector.Distance(spawnPosWorld, obj.GetWorldPosition());
				if (dist < bestDist)
				{
					bestDist = dist;
					bestBox 	= EntityAI.Cast(obj);
				}
			}
		}

		return bestBox;
	}

	// --------------------------------------------------------------------------------

	private EntityAI SpawnStorageBox()
	{
		if (!MemoryPointExists(CRATE_SPAWN_POSITION_MEM_NAME))
			return null;

		vector positionModel = GetMemoryPointPos(CRATE_SPAWN_POSITION_MEM_NAME);
		vector positionWorld = ModelToWorld(positionModel);

		EntityAI storageBox = EntityAI.Cast(g_Game.CreateObjectEx(STORAGE_BOX_CLASSNAME, positionWorld, ECE_CREATEPHYSICS|ECE_UPDATEPATHGRAPH|ECE_KEEPHEIGHT|ECE_EQUIP_CARGO));
		if (storageBox)
		{
		    vector directionModel	= GetMemoryPointPos("storagecratedir");
		    vector directionWorld	= ModelToWorld(directionModel);
		
		    vector forward 	= vector.Direction(positionWorld, directionWorld); // normalized vector		
			
			float yaw = Math.Atan2(forward[0], forward[2]) * Math.RAD2DEG;
			vector orientation = Vector(yaw, 0.0, 0.0);

			storageBox.SetOrientation(orientation);
		}

		return storageBox;
	}
}

class Land_Bunker_Basement_Civilian : BroadcastedBunker {}
class Land_Bunker_Basement_Storage : BroadcastedBunker {}
class Land_Bunker_Basement_Medical : BroadcastedBunker {}
class Land_Bunker_Shelter_Office : BroadcastedBunker {}
class Land_Bunker_Shelter_Storage : BroadcastedBunker {}
class Land_Bunker_Shelter_Medical : BroadcastedBunker {}
