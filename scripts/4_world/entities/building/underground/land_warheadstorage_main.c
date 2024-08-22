class BunkerStorageDoorLight extends PointLightBase
{
	void BunkerStorageDoorLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(5);
		SetBrightnessTo(6.5);
		FadeIn(1);
		SetFadeOutTime(2);
		SetFlareVisible(false);
		SetCastShadow(false);
		SetAmbientColor(1, 0.7, 0.3);
		SetDiffuseColor(1, 0.7, 0.3);
	}
}

class Land_WarheadStorage_Main : House
{
	protected const float DOOR_AUTOCLOSE_TIME = 12;//how long before the outer door auto-close after being opened
	
	protected ref map<int, ref Timer> m_AutoCloseTimers;
	protected BunkerStorageDoorLight m_StorageDoorLights[4];
	
	protected EffectSound m_SoundDoorLoop;
	protected ref array<EffectSound> m_PoweredSoundEffects = new array<EffectSound>();
	
	protected bool m_HasPower;
	protected bool m_HasPowerPrev;
		
	protected const int SOURCES_COUNT = 4;	// storage lights, side vents, lamps
	
	protected const string WARHEAD_BUNKER_TRIGGER = "UndergroundBunkerTrigger";
	
	protected const string ALARM_POS_MEMPOINT 		= "underground";
	protected const string VENT_MAIN_POS_MEMPOINT 	= "Ventilation_main";
	protected const string VENT_POS_MEMPOINT 		= "Ventilation";

	protected const string ALARM_DOOR_OPEN_SOUND 		= "UndergroundDoor_Alarm_Start_SoundSet";
	protected const string ALARM_DOOR_OPEN_LOOP_SOUND 	= "UndergroundDoor_Alarm_Loop_SoundSet";
	protected const string ALARM_DOOR_CLOSE_SOUND		= "UndergroundDoor_Alarm_End_SoundSet";
	protected const string BIG_DOOR_OPEN_SOUND			= "Bunker_Airlock_Door_Open_SoundSet";
	protected const string BIG_DOOR_CLOSE_SOUND			= "Bunker_Airlock_Door_Close_SoundSet";
	protected const string STORAGE_DOOR_OPEN_SOUND 		= "Bunker_doorMetalStorageOpen_SoundSet";
	protected const string STORAGE_DOOR_CLOSE_SOUND 	= "Bunker_doorMetalStorageClose_SoundSet";
	protected const string FENCE_DOOR_OPEN_SOUND 		= "Bunker_doorMetalJailSlide_Open_SoundSet";
	protected const string FENCE_DOOR_CLOSE_SOUND 		= "Bunker_doorMetalJailSlide_Close_SoundSet";
	protected const string METAL_DOOR_OPEN_SOUND 		= "Bunker_doorMetal_Open_SoundSet";
	protected const string METAL_DOOR_CLOSE_SOUND 		= "Bunker_doorMetal_Close_SoundSet";
	protected const string VENTILATION_SOUND			= "Bunker_Ventilation_SoundSet";
	protected const string LAMPS_SOUND					= "Bunker_Lamp_Hum_SoundSet";
	protected const string ELECTRICITY_ON_SOUND			= "Bunker_bunker_electricity_on_SoundSet";
	protected const string ELECTRICITY_OFF_SOUND		= "Bunker_bunker_electricity_off_SoundSet";
	
	protected const string LAMP_SELECTION 			= "GlassLamp";
	protected const string MAIN_DOOR_SELECTION1 	= "maindoor1_outer";
	protected const string MAIN_DOOR_SELECTION2 	= "maindoor2_outer";
	
	protected const string COLOR_LAMP_OFF 	= "#(argb,8,8,3)color(0.5,0.5,0.5,1,co)";
	protected const string COLOR_LAMP_ON	= "#(argb,8,8,3)color(1,0.8,0,1.0,co)";
	
	void Land_WarheadStorage_Main()
	{
		RegisterNetSyncVariableBool("m_HasPower");
		
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
	
	protected void UpdateLamp(string selection, string color)
	{
		int selectionIdx = GetHiddenSelectionIndex(selection);
		SetObjectTexture(selectionIdx, color);
	}
	
	override void DeferredInit()
	{
		GetGame().RegisterNetworkStaticObject(this);
	}
	
	
	void SetPowerServer(bool hasPower)
	{
		if (hasPower && !m_HasPower)
			OnPowerOnServer();
		else if(!hasPower && m_HasPower)
			OnPowerOffServer();
		m_HasPower = hasPower;
		SetSynchDirty();
		
	}
	
	protected void OnPowerOnServer()
	{
		//storagedoor1_light_pos
		for (int i; i < SOURCES_COUNT; i++)
		{
			UpdateLamp(LAMP_SELECTION + (i+i).ToString(), COLOR_LAMP_ON);
		}
	}
	
	protected void OnPowerOffServer()
	{
		for (int i; i < SOURCES_COUNT; i++)
		{
			UpdateLamp(LAMP_SELECTION + (i+i).ToString(), COLOR_LAMP_OFF);
		}
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
			m_StorageDoorLights[i] = BunkerStorageDoorLight.Cast(ScriptedLightBase.CreateLightAtObjMemoryPoint(BunkerStorageDoorLight, this, "lamp" + (i+1).ToString() + "_pos"));
			
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
		for (int i; i < SOURCES_COUNT; i++)
		{
			if (m_StorageDoorLights[i])
				m_StorageDoorLights[i].Destroy();
		}
		
		foreach (EffectSound soundEff : m_PoweredSoundEffects)
		{
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
			PlaySoundSetAtMemoryPoint(sound, BIG_DOOR_OPEN_SOUND, ALARM_POS_MEMPOINT, false, 0, 0);
			sound.SetAutodestroy(true);
			
			PlaySoundSetAtMemoryPoint(m_SoundDoorLoop, ALARM_DOOR_OPEN_LOOP_SOUND, ALARM_POS_MEMPOINT, true, 0, 0);
			m_SoundDoorLoop.SetAutodestroy(true);
		}
		else if (IsMetalDoor(params.param1))
		{
			sound = SEffectManager.PlaySoundEnviroment(METAL_DOOR_OPEN_SOUND, GetDoorSoundPos(params.param1), 0, 0, false);
			sound.SetAutodestroy(true);
		}
		else if (IsFenceDoor(params.param1))
		{
			sound = SEffectManager.PlaySoundEnviroment(FENCE_DOOR_OPEN_SOUND, GetDoorSoundPos(params.param1), 0, 0, false);
			sound.SetAutodestroy(true);
		}
		else if (IsStorageDoor(params.param1))
		{
			sound = SEffectManager.PlaySoundEnviroment(STORAGE_DOOR_OPEN_SOUND, GetDoorSoundPos(params.param1), 0, 0, false);
			sound.SetAutodestroy(true);
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
			if (m_SoundDoorLoop.IsPlaying())
				StopSoundSet(m_SoundDoorLoop);
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
			PlaySoundSetAtMemoryPoint(sound, BIG_DOOR_CLOSE_SOUND, ALARM_POS_MEMPOINT, false, 0, 0);
			sound.SetAutodestroy(true);
			
			PlaySoundSetAtMemoryPoint(m_SoundDoorLoop, ALARM_DOOR_OPEN_LOOP_SOUND, ALARM_POS_MEMPOINT, true, 0, 0);
			m_SoundDoorLoop.SetAutodestroy(true);
		}
		else if (IsMetalDoor(params.param1))
		{
			sound = SEffectManager.PlaySoundEnviroment(METAL_DOOR_CLOSE_SOUND, GetDoorSoundPos(params.param1), 0, 0, false);
			sound.SetAutodestroy(true);
		}
		else if (IsFenceDoor(params.param1))
		{
			sound = SEffectManager.PlaySoundEnviroment(FENCE_DOOR_CLOSE_SOUND, GetDoorSoundPos(params.param1), 0, 0, false);
			sound.SetAutodestroy(true);
		}
		else if (IsStorageDoor(params.param1))
		{
			sound = SEffectManager.PlaySoundEnviroment(STORAGE_DOOR_CLOSE_SOUND, GetDoorSoundPos(params.param1), 0, 0, false);
			sound.SetAutodestroy(true);
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
			
			if (m_SoundDoorLoop.IsPlaying())
				StopSoundSet(m_SoundDoorLoop);
		}
		#endif
	}
	
	void AutoCloseDoor(int doorIndex)
	{
		CloseDoor(doorIndex);
		RemoveDoorTimer(doorIndex);
	}
	
	
	
	void UpdateDoorState(int leverBits)
	{
		for (int index = 1; index <= 4; index++)
		{
			int bit = 1 << (index - 1);
			int doorIndex = GetDoorIndexByLeverIndex(index);
			
			if ((bit & leverBits) != 0)
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
	
	protected bool IsMetalDoor(int doorIndex)
	{
		return (doorIndex == 0 || doorIndex == 1);
	}
	
	protected bool IsFenceDoor(int doorIndex)
	{
		return (doorIndex == 2 || doorIndex == 3);
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
	}
}