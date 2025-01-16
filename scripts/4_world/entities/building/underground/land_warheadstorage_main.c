class Land_WarheadStorage_Main : House
{
	protected bool m_HasPowerPrev;
	protected bool m_IsLowEnergy;
	protected WarheadStorageLight m_StorageDoorLights[4];
	protected EffectSound m_SoundDoorLoop[4];
	protected ref array<EffectSound> m_PoweredSoundEffects = new array<EffectSound>();
	protected ref map<int, ref Timer> m_AutoCloseTimers;
	
	protected const float DOOR_AUTOCLOSE_TIME = 12;//how long before the outer door auto-close after being opened
	protected const int SOURCES_COUNT = 4;	// storage lights, side vents, lamps
	
	protected const string WARHEAD_BUNKER_TRIGGER = "UndergroundBunkerTrigger";
	
	protected const string ALARM_POS_MEMPOINT 		= "underground";
	protected const string VENT_MAIN_POS_MEMPOINT 	= "Ventilation_main";
	protected const string VENT_POS_MEMPOINT 		= "Ventilation";

	protected const string ALARM_DOOR_OPEN_SOUND 		= "UndergroundDoor_Alarm_Start_SoundSet";
	protected const string ALARM_DOOR_OPEN_LOOP_SOUND 	= "UndergroundDoor_Alarm_Loop_SoundSet";
	protected const string ALARM_DOOR_CLOSE_SOUND		= "UndergroundDoor_Alarm_End_SoundSet";
	protected const string VENTILATION_SOUND			= "Bunker_Ventilation_SoundSet";
	protected const string LAMPS_SOUND					= "Bunker_Lamp_Hum_SoundSet";
	protected const string ELECTRICITY_ON_SOUND			= "Bunker_bunker_electricity_on_SoundSet";
	protected const string ELECTRICITY_OFF_SOUND		= "Bunker_bunker_electricity_off_SoundSet";
	
	protected const string LAMP_SELECTION 			= "GlassLamp";
	protected const string MAIN_DOOR_SELECTION1 	= "maindoor1_outer";
	protected const string MAIN_DOOR_SELECTION2 	= "maindoor2_outer";
	
	protected const string COLOR_LAMP_OFF 	= "DZ\\structures_sakhal\\military\\storage\\data\\Warhead_Storage_Lamp_Glass_int.rvmat";
	protected const string COLOR_LAMP_ON	= "DZ\\structures_sakhal\\military\\storage\\data\\Warhead_Storage_Lamp_Glass_e_int.rvmat";
	
	// sync
	protected bool m_HasPower;
	protected bool m_LeverStatesBits;
	
	void Land_WarheadStorage_Main()
	{
		RegisterNetSyncVariableBool("m_HasPower");
		RegisterNetSyncVariableInt("m_LeverStatesBits");
		RegisterNetSyncVariableInt("m_IsLowEnergy");
		
		Land_WarheadStorage_PowerStation.RegisterBunker(this);
		#ifndef SERVER
		SpawnTriggers(5);
		#endif
	}
	
	void ~Land_WarheadStorage_Main()
	{
		Land_WarheadStorage_PowerStation.UnregisterBunker(this);
		
		for (int i; i < 4; i++)
		{
			if (m_StorageDoorLights[i])
				m_StorageDoorLights[i].Destroy();
		}	
	}
		
	override void DeferredInit()
	{
		GetGame().RegisterNetworkStaticObject(this);
		
		if (GetGame().IsServer())
		{
			UpdateDoorStateServer();	// init closed state - code randomization? of opened state messes with doors in general
			for (int i = 0; i < 12; i++)
			{
				if (IsBunkerDoor(i))
					AutoCloseDoor(i);
			}
		}
	}
	
	void SetPowerServer(bool hasPower)
	{
		m_HasPower = hasPower;
		SetSynchDirty();
	}
	
	void SetLeverStatesServer(int leverBits)
	{
		m_LeverStatesBits = leverBits;
		SetSynchDirty();
		
		UpdateDoorStateServer();
	}
	
	void SetLowEnergyStateServer(bool state)
	{
		m_IsLowEnergy = state;
		SetSynchDirty();
	}
	
	bool IsLowEnergy()
	{
		return m_IsLowEnergy;
	}
	
	protected void OnPowerOnClient()
	{
		EffectSound soundEff;
		PlaySoundSetAtMemoryPoint(soundEff, VENTILATION_SOUND, VENT_MAIN_POS_MEMPOINT, true, 0.5, 0.5);
		m_PoweredSoundEffects.Insert(soundEff);
		soundEff.SetAutodestroy(true);
		
		PlaySoundSetAtMemoryPoint(soundEff, ELECTRICITY_ON_SOUND, VENT_MAIN_POS_MEMPOINT, false);
		soundEff.SetAutodestroy(true);
		
		for (int i; i < SOURCES_COUNT; i++)
		{
			PlaySoundSetAtMemoryPoint(soundEff, VENTILATION_SOUND, VENT_POS_MEMPOINT + (i+1).ToString(), true, 0.5, 0.5);
			m_PoweredSoundEffects.Insert(soundEff);
			soundEff.SetAutodestroy(true);
			PlaySoundSetAtMemoryPoint(soundEff, LAMPS_SOUND, "lamp" + (i+1).ToString() + "_pos", true, 0.1, 0.1);
			m_PoweredSoundEffects.Insert(soundEff);
			soundEff.SetAutodestroy(true);
		}
	}
	
	protected void OnPowerOffClient()
	{				
		foreach (EffectSound soundEff : m_PoweredSoundEffects)
		{
			if (soundEff)
				soundEff.Stop();
		}
		
		PlaySoundSetAtMemoryPoint(soundEff, ELECTRICITY_OFF_SOUND, VENT_MAIN_POS_MEMPOINT, false);
		soundEff.SetAutodestroy(true);
		
		m_PoweredSoundEffects.Clear();
	}
	
	
	override void OnDoorOpenStart(DoorStartParams params)
	{		
		#ifndef SERVER
		if (IsBunkerDoor(params.param1))
		{
			EffectSound sound;
			PlaySoundSetAtMemoryPoint(sound, ALARM_DOOR_OPEN_SOUND, ALARM_POS_MEMPOINT, false, 0, 0);
			sound.SetAutodestroy(true);
			
			int effectID = GetBunkerEffectIndexByDoor(params.param1);
			PlaySoundSetAtMemoryPoint(sound, ALARM_DOOR_OPEN_LOOP_SOUND, ALARM_POS_MEMPOINT, true, 0, 0);
			m_SoundDoorLoop[effectID] = sound;
			m_SoundDoorLoop[effectID].SetAutodestroy(true);
		}
		#endif

		if (!IsBunkerDoor(params.param1))
			return;
		
		if (!m_AutoCloseTimers)
			m_AutoCloseTimers = new map<int, ref Timer>();
		
		Timer doorTimer = new Timer();
		doorTimer.Run(DOOR_AUTOCLOSE_TIME, this, "AutoCloseDoor", new Param1<int>(params.param1));
		m_AutoCloseTimers.Set(params.param1, doorTimer);
	}
	
	override void OnDoorOpenFinish(DoorFinishParams params)
	{
		#ifndef SERVER
		if (IsBunkerDoor(params.param1))
		{
			int effectID = GetBunkerEffectIndexByDoor(params.param1);
			if (m_SoundDoorLoop[effectID] && m_SoundDoorLoop[effectID].IsPlaying())
			{
				EffectSound sound = m_SoundDoorLoop[effectID];
				StopSoundSet(sound);
			}
		}
		#endif
	}
	
	protected void RemoveDoorTimer(int doorIndex)
	{
		if (!IsBunkerDoor(doorIndex))
			return;
		
		if (m_AutoCloseTimers)
		{
			m_AutoCloseTimers.Remove(doorIndex);
		}
	}
	
	override void OnDoorCloseStart(DoorStartParams params)
	{
		#ifndef SERVER
		if (IsBunkerDoor(params.param1))
		{
			EffectSound sound;
			int effectID = GetBunkerEffectIndexByDoor(params.param1);
			if (m_SoundDoorLoop[effectID] && m_SoundDoorLoop[effectID].IsPlaying())	// in case events fire in wrong order
			{
				sound = m_SoundDoorLoop[effectID];
				StopSoundSet(sound);
			}
			
			PlaySoundSetAtMemoryPoint(sound, ALARM_DOOR_OPEN_LOOP_SOUND, ALARM_POS_MEMPOINT, true, 0, 0);
			m_SoundDoorLoop[effectID] = sound;
			m_SoundDoorLoop[effectID].SetAutodestroy(true);
		}
		#endif

		RemoveDoorTimer(params.param1);
	}
	
	override void OnDoorCloseFinish(DoorFinishParams params)
	{
		#ifndef SERVER
		if (IsBunkerDoor(params.param1))
		{
			EffectSound sound;
			PlaySoundSetAtMemoryPoint(sound, ALARM_DOOR_CLOSE_SOUND, ALARM_POS_MEMPOINT, false, 0, 0);
			sound.SetAutodestroy(true);
			
			int effectID = GetBunkerEffectIndexByDoor(params.param1);
			if (m_SoundDoorLoop[effectID] && m_SoundDoorLoop[effectID].IsPlaying())
			{
				sound = m_SoundDoorLoop[effectID];
				StopSoundSet(sound);
			}
		}
		#endif
	}
	
	void AutoCloseDoor(int doorIndex)
	{
		CloseDoor(doorIndex);
		RemoveDoorTimer(doorIndex);
	}
	
	void UpdateDoorStateServer()
	{
		for (int index = 1; index <= 4; index++)
		{
			int bit = 1 << (index - 1);
			int doorIndex = GetDoorIndexByLeverIndex(index);
			
			if ( ((bit & m_LeverStatesBits) != 0) && m_HasPower )
			{
				if (!IsDoorOpen(doorIndex))
					OpenDoor(doorIndex);
			}
			else
			{
				if (IsDoorOpen(doorIndex))
					CloseDoor(doorIndex);
			}
		}
	}
	
	void UpdateDoorStateClient()
	{		
		for (int index = 1; index <= 4; index++)
		{
			int bit = 1 << (index - 1);
			int doorIndex = GetDoorIndexByLeverIndex(index);
			int lightId = GetStorageLightIndexByDoor(doorIndex);
			
			if ( ((bit & m_LeverStatesBits) != 0) && m_HasPower )
			{
				if (!m_StorageDoorLights[lightId])
				{
					m_StorageDoorLights[lightId] = WarheadStorageLight.Cast(ScriptedLightBase.CreateLightAtObjMemoryPoint(WarheadStorageLight, this, "lamp" + (lightId + 1).ToString() + "_pos"));
					m_StorageDoorLights[lightId].SetSelectionID(GetHiddenSelectionIndex(LAMP_SELECTION + (lightId + 1).ToString()));
					m_StorageDoorLights[lightId].UpdateLightSourceMaterial(WarheadStorageLight.LIGHT_MAT_ON);
				}
			}
			else
			{
				if (m_StorageDoorLights[lightId])
				{
					m_StorageDoorLights[lightId].UpdateLightSourceMaterial(WarheadStorageLight.LIGHT_MAT_OFF);
					m_StorageDoorLights[lightId].Destroy();
				}
			}
		}
	}
	
	protected int GetStorageLightIndexByDoor(int doorIndex)
	{
		return doorIndex - 8;
	}
		
	protected int GetDoorIndexByLeverIndex(int LeverIndex)
	{
		switch (LeverIndex)
		{
			case 1:
				return 8;

			case 2:
				return 9;

			case 3:
				return 10;

			case 4:
				return 11;
		}
		return -1;
	}

	override bool CanDoorBeOpened(int doorIndex, bool checkIfLocked = false)
	{
		if (!super.CanDoorBeOpened(doorIndex, checkIfLocked))
			return false;
		
		switch (doorIndex)
		{
			case 4:
				return IsDoorClosed(4) && IsDoorClosed(5);
			case 5:
				return IsDoorClosed(4) && IsDoorClosed(5);
			case 6:
				return IsDoorClosed(6) && IsDoorClosed(7);
			case 7:
				return IsDoorClosed(6) && IsDoorClosed(7);
			case 8:
			case 9:
			case 10:
			case 11:
				return false;
		}
		return true;
	}
	
	override bool CanDoorBeLocked(int doorIndex)
	{
		if (IsStorageDoor(doorIndex) || IsBunkerDoor(doorIndex))
			return false;
		return super.CanDoorBeLocked(doorIndex);
	}
	
	protected int GetBunkerEffectIndexByDoor(int doorIndex)
	{
		return doorIndex - 4;
	}
	
	protected bool IsBunkerDoor(int doorIndex)
	{
		return (doorIndex >= 4 && doorIndex <= 7);
	}
	
	protected bool IsStorageDoor(int doorIndex)
	{
		return (doorIndex >= 8 && doorIndex <= 11);
	}
	
	override bool CanDoorBeClosed(int doorIndex)
	{
		if (IsStorageDoor(doorIndex))
			return false;
		
		if (IsBunkerDoor(doorIndex))
			return IsDoorOpen(doorIndex) && !IsDoorOpening(doorIndex); 
		
		return super.CanDoorBeClosed(doorIndex);
	}
	
	protected array<ref UndergroundBunkerTriggerData> GetTriggersData(int count)
	{
		array<ref UndergroundBunkerTriggerData> dataArr = new array<ref UndergroundBunkerTriggerData>();
		//memory point naming start at 1
		for (int i = 1; i < count + 1; i++)
		{
			UndergroundBunkerTriggerData data = new UndergroundBunkerTriggerData();
			string memoryPointNameMin = "darkness_" +i+"_min";
			string memoryPointNameMax = "darkness_" +i+"_max";
			vector memPosMin = GetMemoryPointPos(memoryPointNameMin);
			vector memPosMax = GetMemoryPointPos(memoryPointNameMax);
			vector triggerSize = memPosMax - memPosMin;
			vector triggerPosLocal = memPosMin + (triggerSize * 0.5);
			EUndergroundTriggerType type = EUndergroundTriggerType.INNER;
			string linkeDoorSelection = "";
			if (i == 4)
			{
				type = EUndergroundTriggerType.TRANSITIONING;
				linkeDoorSelection = MAIN_DOOR_SELECTION1;
			}
			else if (i == 5)
			{
				type = EUndergroundTriggerType.TRANSITIONING;
				linkeDoorSelection = MAIN_DOOR_SELECTION2;
			}
			
			data.m_Position = triggerPosLocal;
			data.m_Size = triggerSize;
			data.m_Type = type;
			data.m_LinkedDoorSelection = linkeDoorSelection;
			
			dataArr.Insert(data);
		}
		return dataArr;
	}

	protected void SpawnTriggers(int count)
	{
		array<ref UndergroundBunkerTriggerData> dataArr 	= GetTriggersData(5);
		
		foreach (UndergroundBunkerTriggerData data:dataArr)
		{
			//Debug.DrawSphere(pos , 0.1,Colors.GREEN, ShapeFlags.NOZBUFFER);
			UndergroundBunkerTrigger trigger = UndergroundBunkerTrigger.Cast(GetGame().CreateObjectEx(WARHEAD_BUNKER_TRIGGER, data.m_Position, ECE_LOCAL ));
			if (trigger)
			{
				vector extMax = data.m_Size * 0.5;
				vector extMin = -extMax;
				#ifdef DIAG_DEVELOPER
				trigger.m_Local = true;
				#endif
				AddChild(trigger,-1);
				trigger.SetExtents(extMin, extMax);
				trigger.Init(data);
			}
		}
	}
	
	override string GetDebugText()
	{
		string debug_output;
		
		if( GetGame().IsServer())
		{
			debug_output = "IsDoorOpen(4) " + IsDoorOpen(4)+ "\n";
			debug_output +=  "IsDoorOpen(5) " + IsDoorOpen(5)+ "\n";
			debug_output +=  "IsDoorOpen(6) " + IsDoorOpen(6)+ "\n";
			debug_output +=  "IsDoorOpen(7) " + IsDoorOpen(7)+ "\n";
		}
		else
		{
			debug_output = "this is client";
		}
		return debug_output;
	}
	
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_HasPower && !m_HasPowerPrev)
			OnPowerOnClient();
		else if(!m_HasPower && m_HasPowerPrev)
			OnPowerOffClient();
		
		m_HasPowerPrev = m_HasPower;
		
		UpdateDoorStateClient();
	}
}