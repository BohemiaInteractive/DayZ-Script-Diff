typedef Param1<int> DoorStartParams;
typedef Param2<int, bool> DoorFinishParams;
typedef Param1<int> DoorLockParams;

class Building extends EntityAI
{
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
	
	bool CanDoorBeOpened(int doorIndex, bool checkIfLocked = false)
	{
		if (IsDoorOpen(doorIndex))
			return false;

		if (checkIfLocked)
		{
			if (IsDoorLocked(doorIndex))
				return false;
		}
		else
		{
			if (!IsDoorLocked(doorIndex))
				return false;
		}
		return true;
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
		return 1 << EBuildingLockType.LOCKPICK; //all doors are lockpickable by default
	}
	
	override void GetDebugActions(out TSelectableActionInfoArrayEx outputList)
	{
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_OUTPUT_LOG, "Output Door Log", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, " --- ", FadeColors.LIGHT_GREY));

		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_PLAY_DOOR_SOUND, "Play Door Sound", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_OPEN_DOOR, "Open Door", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_CLOSE_DOOR, "Close Door", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_LOCK_DOOR, "Lock Door", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.BUILDING_UNLOCK_DOOR, "Unlock Door", FadeColors.LIGHT_GREY));
		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, " --- ", FadeColors.LIGHT_GREY));
		
		super.GetDebugActions(outputList);
	}
	
	override bool OnAction(int action_id, Man player, ParamsReadContext ctx)
	{
		if (super.OnAction(action_id, player, ctx))
			return true;

		switch (action_id)
		{
		case EActions.BUILDING_PLAY_DOOR_SOUND:
			PlayDoorSound(GetNearestDoorBySoundPos(player.GetPosition()));
			return true;
		}

		if (!GetGame().IsServer())
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
	
	override bool IsHealthVisible()
	{
		return false;
	}

	ref TIntArray m_InteractActions;
	
	void Building()
	{
		m_InteractActions = new TIntArray;
		g_Game.ConfigGetIntArray("cfgVehicles " +GetType() + " InteractActions", m_InteractActions);
	}

	override bool IsInventoryVisible()
	{
		return false;
	}

	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
};
