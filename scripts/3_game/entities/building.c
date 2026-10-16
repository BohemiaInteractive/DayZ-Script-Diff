typedef Param1<int> DoorStartParams;
typedef Param2<int, bool> DoorFinishParams;
typedef Param1<int> DoorLockParams;

class DoorManipulationParams
{
	int m_DoorIndex;
	bool m_CheckIfLocked;
	Object m_Caller;
}

class BuildingType : EntityAIType
{

};

class Building : EntityAI
{
	// !! Move this to BuildingType !!
	static ref map<string, ref AdditionalDoorInfosHolder> m_AdditionalDoorInfosMap = new map<string, ref AdditionalDoorInfosHolder>; //! Additional script info
	protected ref InstancedAttachmentGroupDataBase m_InstancedAttGroupData;
	
	proto native int GetLaddersCount();
	proto native vector GetLadderPosTop(int ladderIndex);
	proto native vector GetLadderPosBottom(int ladderIndex);

	//! Gets the index of the door based on the view geometry component index
	proto native int GetDoorIndex(int componentIndex);

	//! Returns the number of the doors in the building
	proto native int GetDoorCount();

	//! When the door is requested to be fully open (animation wanted phase is greater than 0.5)
	proto native bool IsDoorOpen(int index);
	
	//! When the wanted phase is at the open phase target (1.0)
	proto native bool IsDoorOpening(int index);
	
	//! When the wanted phase is at the ajar phase target (0.2)
	proto native bool IsDoorOpeningAjar(int index);
	
	//! When the wanted phase is at the close phase target (0.0)
	proto native bool IsDoorClosing(int index);
	
	//! When the phase is at the open phase target (1.0)
	proto native bool IsDoorOpened(int index);
	
	//! When the phase is at the ajar phase target (0.2)
	proto native bool IsDoorOpenedAjar(int index);
	
	//! When the phase is at the close phase target (0.0)
	proto native bool IsDoorClosed(int index);

	//! When the door is locked
	proto native bool IsDoorLocked(int index);

	//! Plays the appropriate sound at the door, based on animation phase and state
	proto native void PlayDoorSound(int index);

	//! Attempts to open the door
	proto native void OpenDoor(int index);

	//! Attempts to close the door
	proto native void CloseDoor(int index);

	//! Locks the door if not already locked, resets the door health. 'force = true' will close the door if open
	proto native void LockDoor(int index, bool force = false);

	//! Unlocks the door if locked, AJAR animation optional
	proto native void UnlockDoor(int index, bool animate = true);

	//! Position in world space for where the door sounds are played from
	proto native vector GetDoorSoundPos(int index);

	//! Audible distance for the door sound
	proto native float GetDoorSoundDistance(int index);

	//! Requires special build as logging is disabled even on internal builds due to memory usage. Used for checking of possible causes of navmesh desync
	proto native void OutputDoorLog();

	//! Gets the nearest door based on the sound position (quick and dirty function for debugging)
	int GetNearestDoorBySoundPos(vector position)
	{
		float smallestDist = float.MAX;
		int nearestDoor = -1;

		int count = GetDoorCount();
		for (int i = 0; i < count; i++)
		{
			float dist = vector.DistanceSq(GetDoorSoundPos(i), position);
			if (dist < smallestDist)
			{
				nearestDoor = i;
				smallestDist = dist;
			}
		}

		return nearestDoor;
	}

	//! Event for when the door starts opening
	void OnDoorOpenStart(DoorStartParams params)
	{
	}

	//! Event for when the door finishes opening
	void OnDoorOpenFinish(DoorFinishParams params)
	{
	}

	//! Event for when the door starts opening ajarred (usually after unlock)
	void OnDoorOpenAjarStart(DoorStartParams params)
	{
	}

	//! Event for when the door finishes opening ajarred (usually after unlock)
	void OnDoorOpenAjarFinish(DoorFinishParams params)
	{
	}

	//! Event for when the door starts closing
	void OnDoorCloseStart(DoorStartParams params)
	{
	}

	//! Event for when the door finishes closing
	void OnDoorCloseFinish(DoorFinishParams params)
	{
	}

	//! Event for when the door is locked
	void OnDoorLocked(DoorLockParams params)
	{
	}

	//! Event for when the door is unlocked
	void OnDoorUnlocked(DoorLockParams params)
	{
	}
	
	bool CanDoorBeOpened(notnull DoorManipulationParams params)
	{
		if (IsDoorOpen(params.m_DoorIndex))
			return false;

		if (params.m_CheckIfLocked)
		{
			if (IsDoorLocked(params.m_DoorIndex))
				return false;
		}
		else
		{
			if (!IsDoorLocked(params.m_DoorIndex))
				return false;
		}

		return true;
	}
	
	bool CanDoorBeOpened(int doorIndex, bool checkIfLocked = false)
	{
		DoorManipulationParams doorParams = new DoorManipulationParams();
		doorParams.m_DoorIndex = doorIndex;
		doorParams.m_CheckIfLocked = checkIfLocked;

		return CanDoorBeOpened(doorParams);
	}

	bool CanDoorBeClosed(int doorIndex, Object caller)
	{
		return IsDoorOpen(doorIndex);
	}
	
	bool CanDoorBeClosed(int doorIndex)
	{
		return IsDoorOpen(doorIndex);
	}
	
	//! Check if the door is closed and if the door is unlocked
	bool CanDoorBeLocked(int doorIndex)
	{
		return (!IsDoorOpen(doorIndex) && !IsDoorLocked(doorIndex));
	}
	
	/**
	\brief Which door is compatible with which key (door idx supplied). Bitwise.
	@param doorIdx
	@return bitwise value of all compatible locks
	\note you can combine the bit values like so:
	\note return (1 << EBuildingLockType.LOCKPICK) | (1 << EBuildingLockType.SHIP_CONTAINER_1);
	\note you can also this for each individual door idx
	*/	
	int GetLockCompatibilityType(int doorIdx)
	{
		return GetDoorInfo(doorIdx).m_LockCompatibilityBitMask;
	}
	
	override void GetDebugActions(out TSelectableActionInfoArrayEx outputList)
	{
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_OUTPUT_LOG, "Output Door Log", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));

		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_PLAY_DOOR_SOUND, "Play Door Sound", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_OPEN_DOOR, "Open Door", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_CLOSE_DOOR, "Close Door", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_LOCK_DOOR, "Lock Door", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_UNLOCK_DOOR, "Unlock Door", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		
		if (Gizmo_IsSupported())
		{
			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.GIZMO_OBJECT, "Gizmo Object", FadeColors.LIGHT_GREY));
			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
		}
		
		super.GetDebugActions(outputList);
	}
	
	override bool OnAction(int action_id, Man player, ParamsReadContext ctx)
	{
		if (super.OnAction(action_id, player, ctx))
			return true;

		if (g_Game.IsClient() || !g_Game.IsMultiplayer())
		{
			switch (action_id)
			{
				case EActions.GIZMO_OBJECT:
					if (GetGizmoApi())
						GetGizmoApi().SelectObject(this);
					return true;
			}
		}
		
		switch (action_id)
		{
		case EActions.BUILDING_PLAY_DOOR_SOUND:
			PlayDoorSound(GetNearestDoorBySoundPos(player.GetPosition()));
			return true;
		}

		if (!g_Game.IsServer())
			return false;

		switch (action_id)
		{
		case EActions.BUILDING_OUTPUT_LOG:
			OutputDoorLog();
			return true;
		case EActions.BUILDING_OPEN_DOOR:
			OpenDoor(GetNearestDoorBySoundPos(player.GetPosition()));
			return true;
		case EActions.BUILDING_CLOSE_DOOR:
			CloseDoor(GetNearestDoorBySoundPos(player.GetPosition()));
			return true;
		case EActions.BUILDING_LOCK_DOOR:
			LockDoor(GetNearestDoorBySoundPos(player.GetPosition()));
			return true;
		case EActions.BUILDING_UNLOCK_DOOR:
			UnlockDoor(GetNearestDoorBySoundPos(player.GetPosition()));
			return true;
		}

		return false;
	}
	
	override bool IsBuilding()
	{
		return true;
	}
	
	override bool CanObstruct()
	{
		return true;
	}
	
	override bool DisableVicinityIcon()
	{
		return true;
	}
	
	override bool IsHealthVisible()
	{
		return CanUseConstruction();
	}
	
	override bool ShowZonesHealth()
	{
		return CanUseConstruction();
	}

	ref TIntArray m_InteractActions;

	void Building()
	{
		m_InteractActions = new TIntArray;
		g_Game.ConfigGetIntArray("cfgVehicles " +GetType() + " InteractActions", m_InteractActions);
		
		InitializeDoorInfos();
	}
	
	protected void InitializeDoorInfos()
	{
		if (m_AdditionalDoorInfosMap.Contains(GetType())) //skip if already initialized
			return;
		
		AdditionalDoorInfosHolder holder = new AdditionalDoorInfosHolder(GetType());
		holder.Init();
		m_AdditionalDoorInfosMap.Set(GetType(),holder);
	}
	
	AdditionalDoorInfo GetDoorInfo(int doorIdx)
	{
		return m_AdditionalDoorInfosMap.Get(GetType()).GetSingleDoorInfo(doorIdx);
	}
	
	AdditionalDoorInfosHolder GetDoorInfoHolder()
	{
		return m_AdditionalDoorInfosMap.Get(GetType());
	}
	
	override protected void InitializeInstanceAttachmentGroupsData()
	{
		m_InstancedAttGroupData = new InstancedAttachmentGroupDataBuilding(this);
		m_InstancedAttGroupData.Init();
	}
	
	override bool IsInventoryVisible()
	{
		return CanUseConstruction() && GetInventory().GetAttachmentSlotsCount() > 0; //TODO: some location checks here?
	}
	
	override bool IsDetectableByVicinityCone()
	{
		return CanUseConstruction() && GetInventory().GetAttachmentSlotsCount() > 0; //TODO: some location checks here?
	}

	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
};

//-----------------------------------------------------------------------------
class WindSockType : EntityType
{
};

//-----------------------------------------------------------------------------
/*
class WindSock : Entity
{
};
*/
