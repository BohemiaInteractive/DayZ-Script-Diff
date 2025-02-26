class ContainerLockedBase : BuildingSuper
{
	protected int m_LockedMask = 0;
	protected int m_OpeningMask = 0;
	
	void ContainerLockedBase()
	{
		int count = GetDoorCount();
		for (int i = 0; i < count; i++)
		{
			LockDoor(i);
		}
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_LockedMask);
		ctx.Write(m_OpeningMask);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		ctx.Read(m_LockedMask);
		ctx.Read(m_OpeningMask);
		
		HandleDoorLoad();
		
		return true;
	}
	
	protected void HandleDoorLoad()
	{
		int count = GetDoorCount();
		int i;
		
		//locking
		for (i = 0; i < count; i++)
		{
			if (m_LockedMask & (1 << i))
			{
				LockDoor(i,true);
			}
			else
			{
				UnlockDoor(i);
			}
		}
		
		//then opening
		for (i = 0; i < count; i++)
		{
			if (m_OpeningMask & (1 << i) && !IsDoorLocked(i))
			{
				OpenDoor(i);
			}
		}
	}
	
	//////////////////////////////////////////
	//events
	
	override void OnDoorLocked(DoorLockParams params)
	{
		super.OnDoorLocked(params);
		
		m_LockedMask |= 1 << params.param1;
		
		
		string selectionName = string.Format("side%1_lock",(params.param1 + 1));
		SetAnimationPhase(selectionName,0);
	}

	override void OnDoorUnlocked(DoorLockParams params)
	{
		super.OnDoorUnlocked(params);
		
		int doorIdx = params.param1;
		
		m_LockedMask &= ~(1 << doorIdx);
		
		string selectionName = string.Format("side%1_lock",(doorIdx + 1));
		SetAnimationPhase(selectionName,1);
		
		if (!GetGame().IsDedicatedServer())
			SEffectManager.PlaySoundEnviroment("Land_ContainerLocked_lock_SoundSet",GetDoorSoundPos(doorIdx));
	}
	
	override void OnDoorCloseStart(DoorStartParams params)
	{
		super.OnDoorCloseStart(params);
		
		m_OpeningMask &= ~(1 << params.param1);
	}
	
	override void OnDoorOpenStart(DoorStartParams params)
	{
		super.OnDoorOpenStart(params);
		
		m_OpeningMask |= 1 << params.param1;
	}
}

class Land_ContainerLocked_Blue_DE : ContainerLockedBase
{
	override int GetLockCompatibilityType(int doorIdx)
	{
		return 1 << EBuildingLockType.SHIP_CONTAINER_0;
	}
}

class Land_ContainerLocked_Yellow_DE : ContainerLockedBase
{
	override int GetLockCompatibilityType(int doorIdx)
	{
		return 1 << EBuildingLockType.SHIP_CONTAINER_1;
	}
}

class Land_ContainerLocked_Orange_DE : ContainerLockedBase
{
	override int GetLockCompatibilityType(int doorIdx)
	{
		return 1 << EBuildingLockType.SHIP_CONTAINER_2;
	}
}

class Land_ContainerLocked_Red_DE : ContainerLockedBase
{
	override int GetLockCompatibilityType(int doorIdx)
	{
		return 1 << EBuildingLockType.SHIP_CONTAINER_3;
	}
}
