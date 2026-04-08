class Land_WarheadStorage_Bunker_Facility : House
{
	protected const float DOOR_AUTOCLOSE_TIME = 12; //how long before the door auto-close after being opened

	protected ref map<int, ref Timer> m_AutoCloseTimers;
	
	override void DeferredInit()
	{
		g_Game.RegisterNetworkStaticObject(this);
		
		if (g_Game.IsServer())
		{
			for (int i = 0; i < GetDoorCount(); ++i)
				AutoCloseDoor(i);

			JsonUndergroundTriggers.SpawnParentedTriggers(this);
		}
	}

	override bool CanDoorBeOpened(int doorIndex, bool checkIfLocked = false)
	{
		if (!super.CanDoorBeOpened(doorIndex, checkIfLocked))
			return false;
		
		switch (doorIndex)
		{
			case 7:
				return IsDoorClosed(8);
			case 8:
				return IsDoorClosed(7);
		}

		return true;
	}
	
	override bool CanDoorBeClosed(int doorIndex)
	{
		if (IsPressureDoor(doorIndex))
			return IsDoorOpen(doorIndex) && !IsDoorOpening(doorIndex); 
		
		return super.CanDoorBeClosed(doorIndex);
	}
	
	override void OnDoorOpenStart(DoorStartParams params)
	{
		if (!m_AutoCloseTimers)
			m_AutoCloseTimers = new map<int, ref Timer>();
		
		if (IsPressureDoor(params.param1))
		{
			Timer doorTimer = new Timer();
			doorTimer.Run(DOOR_AUTOCLOSE_TIME, this, "AutoCloseDoor", new Param1<int>(params.param1));
			m_AutoCloseTimers.Set(params.param1, doorTimer);
		}
	}
	
	override void OnDoorCloseStart(DoorStartParams params)
	{
		RemoveDoorTimer(params.param1);
	}

	override void OnSpawnByObjectSpawner(ITEM_SpawnerObject item)
	{
		super.OnSpawnByObjectSpawner(item);
		
		if (item.customString == string.Empty)
			return;
		
		JsonUndergroundTriggers jsonData = UndergroundAreaLoader.GetData();
		if (jsonData && jsonData.Triggers)
		{
			foreach (int index, auto triggerData : jsonData.Triggers)
			{
				if (triggerData.CustomSpawn)
				{
					//JSON: "customString": "undergroundTriggerTag=TAG_NAME"
					TStringArray customStringData = new TStringArray();
					item.customString.Split(";", customStringData);
					
					foreach (string entry : customStringData)
					{
						TStringArray optionValuePair = new TStringArray();
						entry.Split("=", optionValuePair);
						if (optionValuePair[0] == "undergroundTriggerTag")
						{
							if (optionValuePair[1] == triggerData.Tag)
								JsonUndergroundTriggers.SpawnTriggerCarrier(this, index, triggerData);
						}
					}
				}
			}
		}
	}
	
	private void AutoCloseDoor(int doorIndex)
	{
		CloseDoor(doorIndex);
		RemoveDoorTimer(doorIndex);
	}
	
	private void RemoveDoorTimer(int doorIndex)
	{
		if (m_AutoCloseTimers)
			m_AutoCloseTimers.Remove(doorIndex);
	}
	
	private bool IsPressureDoor(int doorIndex)
	{
		switch (doorIndex)
		{
			case 7:
			case 8:
				return true;
		}
		
		return false;
	}
	
}