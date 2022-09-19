class Building extends EntityAI
{
	proto native int GetLaddersCount();
	proto native vector GetLadderPosTop(int ladderIndex);
	proto native vector GetLadderPosBottom(int ladderIndex);

	proto native int GetDoorIndex(int componentIndex);
	proto native bool IsDoorOpen(int index);
	proto native bool IsDoorLocked(int index);
	proto native void PlayDoorSound(int index);
	proto native void OpenDoor(int index);
	proto native void CloseDoor(int index);
	proto native void LockDoor(int index);
	proto native void UnlockDoor(int index);
	proto native vector GetDoorSoundPos(int index);
	
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
