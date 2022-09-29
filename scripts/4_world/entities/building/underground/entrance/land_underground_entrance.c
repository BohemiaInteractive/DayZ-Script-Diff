enum EUndegroundEntranceState
{
	UNINITIALIZED,
	CLOSED,//fully closed
	//opening
	OPENING_A,
	OPENING_B,
	OPENING_C,
	OPENING_D,
	OPENING_E,//fully open
	OPENING_F,
	OPENING_G,
	//closing
	CLOSING_A,
	CLOSING_B,
	CLOSING_C,
	CLOSING_D,
	CLOSING_E,
	CLOSING_F,
	CLOSING_G
}

enum EUndegroundDoorType
{
	MAIN,
	SMALL,
}

class AlarmLight : SpotlightLight
{
	void AlarmLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(15);
		SetBrightnessTo(10);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.0, 0.0);
		SetDiffuseColor(1.0, 0.0, 0.0);
		SetLifetime(1000);
		SetDisableShadowsWithinRadius(-1);
		SetFadeOutTime(1);
		SetCastShadow(false);
		m_FadeInTime = 0.25;
	}
}

//---------------------------------------------------------------------------------------------------------
//------------------------------------ Land_Underground_EntranceBase --------------------------------------
//---------------------------------------------------------------------------------------------------------

class Land_Underground_EntranceBase : House
{
	EUndegroundEntranceState m_DoorState 		= EUndegroundEntranceState.CLOSED;
	EUndegroundEntranceState m_DoorStatePrev 	= EUndegroundEntranceState.UNINITIALIZED;
	float 									m_AnimPhase;
	ref AnimationTimer 						m_AnimTimerDoorServer;
	ref Timer								m_NavmeshTimer;
	ref array<Land_Underground_Panel> 		m_ConnectedPanels;
	EUndegroundDoorType						m_DoorType;

	EntranceLight 							m_InteriorLight1;
	EntranceLight 							m_InteriorLight2;
	EntranceLight 							m_InteriorLight3;
	
	void Land_Underground_EntranceBase()
	{
		m_DoorType = EUndegroundDoorType.MAIN;
		Land_Underground_Panel.RegisterEntrance(this);
		RegisterNetSyncVariableFloat("m_AnimPhase", 0,1,5);
		RegisterNetSyncVariableInt("m_DoorState", 0, EnumTools.GetLastEnumValue(EUndegroundEntranceState));
		
		#ifndef SERVER
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( CreateLights, 250);
		#endif
	}
	
	void CreateLights();
	
	void ~Land_Underground_EntranceBase()
	{
		Land_Underground_Panel.UnregisterEntrance(this);
		#ifndef SERVER
		CleanUpOnDeleteClient();
		#endif
	}
	//---------------------
	
		
	void CleanUpOnDeleteClient()
	{
		//anything we might have started playing when the door was activated
		CleanUpOnClosedClient();
	}
	
	
	void CleanUpOnClosedClient();
	
	bool CanManipulate(Param param = null)
	{
		return m_DoorState == EUndegroundEntranceState.CLOSED;
	}
	
	void Manipulate(Param param = null)
	{
		OpenServer();
	}
	
	void NavmeshUpdate()
	{
		GetGame().UpdatePathgraphRegionByObject(this);
	}
	
	void OnUpdateClient(float timeSlice);
	
	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		#ifndef SERVER
		OnUpdateClient(timeSlice);
		#endif
	}
	
	void OnUpdateServer()
	{
		m_AnimPhase = m_AnimTimerDoorServer.GetValue() / AdjustTime(GetOpeningTime());// make 0..1
		SetAnimationPhaseNow("EntranceDoor",m_AnimPhase);
		SetSynchDirty();
	}
	
	void GetConnectedPanels(array<Land_Underground_Panel> panels)
	{
		if (!Land_Underground_Panel.m_Panels)
		{
			return;
		}
		foreach (Land_Underground_Panel p:Land_Underground_Panel.m_Panels)
		{
			if (p.GetClosestDoor() == this)
			{
				panels.Insert(p);
			}
		}
	}
	
	// checks whether we want to play this effect even when we are at a different(more advanced) state, as this effect is supposed to be playing over multiple states and was supposed to start during some earlier state switch
	// param 'state' is the state this effect is supposed to be played in, and 'lastValidState' is the latests state where we still want to play this effect provided previous state for the client is UNINITIALIZED
	// meaning the client just connected in/the item entered their multiplayer bubble
	bool CheckShouldPlayPersistent(EUndegroundEntranceState state, EUndegroundEntranceState lastValidState)
	{
		return m_DoorState == state || ( IsInitDoorStateSync() && m_DoorState > state && m_DoorState <= lastValidState);
	}
	
	float AdjustTime(float originalTime, float adjustedTime = -1)
	{
		#ifdef DEVELOPER
		float timeAccel = 1;
		
		if (adjustedTime != -1)
		{
			return adjustedTime;
		}
		if (FeatureTimeAccel.GetFeatureTimeAccelEnabled(ETimeAccelCategories.UNDERGROUND_ENTRANCE))
		{
			timeAccel = FeatureTimeAccel.GetFeatureTimeAccelValue();
			return originalTime / timeAccel;
		}
		#endif
		return originalTime;
	}
	
	float GetOpeningTime()
	{
		return 30;
	}
	
	void RequestLatentTransition(float time, EUndegroundEntranceState targetState = EUndegroundEntranceState.UNINITIALIZED)
	{
		if (targetState == EUndegroundEntranceState.UNINITIALIZED)
		{
			targetState = m_DoorState + 1;
		}
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( SetDoorStateServer, time * 1000, false, targetState);
	}
	
	void SetDoorStateServer(EUndegroundEntranceState newState)
	{
		m_DoorState = newState;
		OnDoorStateChangedServer(newState);
		SetSynchDirty();
	}
	
	void OnDoorStateChangedServer(EUndegroundEntranceState newState);
	
	void OnDoorStateChangedClient(EUndegroundEntranceState newState, EUndegroundEntranceState prevState)
	{
		if (newState > EUndegroundEntranceState.CLOSED)
		{
			SetEventMask(EntityEvent.POSTSIMULATE);
		}
		else
		{
			ClearEventMask(EntityEvent.POSTSIMULATE);
			CleanUpOnClosedClient();
		}
		
		HandleAudioPlayback(newState, prevState);
		HandleVisualPlayback(newState, prevState);
		
		if (!m_ConnectedPanels)
		{
			m_ConnectedPanels = new array<Land_Underground_Panel>();
			GetConnectedPanels(m_ConnectedPanels);
		}
		foreach (Land_Underground_Panel p:m_ConnectedPanels)
		{
			p.OnDoorStateChangedClient(newState, prevState);
		}
	}
	
	void OpenServer(bool force = false)
	{
		GetGame().RegisterNetworkStaticObject(this);
		if (m_DoorState == EUndegroundEntranceState.CLOSED || force)
		{
			SetDoorStateServer(EUndegroundEntranceState.OPENING_A);
		}
	}
	
	void HandleVisualPlayback(EUndegroundEntranceState newState, EUndegroundEntranceState prevState);
	void HandleAudioPlayback(EUndegroundEntranceState newState, EUndegroundEntranceState prevState);
	
	void OnFinishedTimerServer();

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (m_DoorState != m_DoorStatePrev)
		{
			OnDoorStateChangedClient(m_DoorState, m_DoorStatePrev);
			m_DoorStatePrev = m_DoorState;
		}
	}
	
	bool IsInitDoorStateSync()
	{
		return m_DoorStatePrev == EUndegroundEntranceState.UNINITIALIZED;
	}
	
	
	#ifdef DEVELOPER
	override string GetDebugText()
	{
		string debug_output;
		
		if (GetGame().IsDedicatedServer())
		{
			debug_output +=  "current state: " +  typename.EnumToString(EUndegroundEntranceState, m_DoorState) + "\n";
			/*
			if(GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ))
				debug_output +=  "next stage timer: " + GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).GetRemainingTimeByName(this, "SetDoorStateServer").ToString();
			*/
		}
		else
		{
			debug_output +=  "current state: " +  typename.EnumToString(EUndegroundEntranceState, m_DoorState) + "\n";
		}
		return debug_output;
	}
	
	override void OnDebugButtonPressServer(int button_index)
	{
		if (button_index == 1)
		{
			if (m_DoorState == EUndegroundEntranceState.CLOSED)
				OpenServer(true);
		}
		
		if (button_index == 2)
		{
			// just for debug controls
			if (!m_AnimTimerDoorServer)
				m_AnimTimerDoorServer = new AnimationTimer();
	
			if (m_DoorState == EUndegroundEntranceState.OPENING_G)
			{
				GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).Remove(SetDoorStateServer);
				SetDoorStateServer(EUndegroundEntranceState.CLOSING_A);
			}
		}
	}
	
	override void GetDebugButtonNames(out string button1, out string button2, out string button3, out string button4)
	{
		button1 = "Open";
		button2 = "Close";
	}
	#endif
}


//---------------------------------------------------------------------------------------------------------
//-------------------------------------- Land_Underground_Entrance ----------------------------------------
//---------------------------------------------------------------------------------------------------------

class Land_Underground_Entrance : Land_Underground_EntranceBase
{
	EffectSound 		m_SirenSound;
	EffectSound 		m_DoorEngineSoundIn;
	EffectSound 		m_DoorEngineSoundLoop;
	EffectSound 		m_DoorEngineSoundOut;
	EffectSound 		m_LockingSound;
	EffectSound 		m_DoorMovementSoundIn;
	EffectSound 		m_DoorMovementSoundLoop;
	EffectSound 		m_DoorMovementSoundOut;

	
	AlarmLight			m_AlarmLight;
	
	const string SIREN_SOUNDSET 					= "UndergroundDoor_Alarm_Loop_SoundSet";
	const string ENGINE_SOUNDSET_LOOP_IN			= "UndergroundDoor_ElectricMotor_Start_SoundSet";
	const string ENGINE_SOUNDSET_LOOP				= "UndergroundDoor_ElectricMotor_Loop_SoundSet";
	const string ENGINE_SOUNDSET_LOOP_OUT			= "UndergroundDoor_ElectricMotor_End_SoundSet";
	const string LOCKING_SOUNDSET 					= "UndergroundDoor_Lock_SoundSet";
	const string DOORMOVING_SOUNDSET_LOOP 			= "UndergroundDoor_DoorOpen_Loop_SoundSet";
	const string DOORMOVING_SOUNDSET_LOOP_OUT 		= "UndergroundDoor_DoorOpen_End_SoundSet";
	const string DOORMOVING_SOUNDSET_LOOP_IN 		= "UndergroundDoor_DoorOpen_Start_SoundSet";
	
	const float LIGHT_ROT_SPEED = -400;
	
	override void CreateLights()
	{
		m_InteriorLight1 = EntranceLight.Cast(ScriptedLightBase.CreateLightAtObjMemoryPoint(EntranceLightMain1, this, "InteriorLightPos1"));
		m_InteriorLight2 = EntranceLight.Cast(ScriptedLightBase.CreateLightAtObjMemoryPoint(EntranceLightMain2, this, "InteriorLightPos2"));
	}
	
	override void CleanUpOnClosedClient()
	{
		SEffectManager.DestroySound(m_SirenSound);
		SEffectManager.DestroySound(m_LockingSound);
		SEffectManager.DestroySound(m_DoorMovementSoundIn);
		SEffectManager.DestroySound(m_DoorMovementSoundOut);
		SEffectManager.DestroySound(m_DoorMovementSoundLoop);
		SEffectManager.DestroySound(m_DoorEngineSoundIn);
		SEffectManager.DestroySound(m_DoorEngineSoundOut);
		SEffectManager.DestroySound(m_DoorEngineSoundLoop);
		
		if (m_AlarmLight && GetGame())
		{
			m_AlarmLight.Destroy();
		}
	}
	
	override void OnDoorStateChangedServer(EUndegroundEntranceState newState)
	{
		switch (newState)
		{
			case EUndegroundEntranceState.OPENING_A:
				RequestLatentTransition(AdjustTime(3));
			break;
			case EUndegroundEntranceState.OPENING_B:
				RequestLatentTransition(AdjustTime(2));
			break;
			case EUndegroundEntranceState.OPENING_C:
				RequestLatentTransition(AdjustTime(1));
			break;
			case EUndegroundEntranceState.OPENING_D:
				m_AnimTimerDoorServer = new AnimationTimer();
				m_AnimTimerDoorServer.Run(AdjustTime(GetOpeningTime()), this, "OnUpdateServer", "OnFinishedTimerServer",0, false,/*1/ AdjustTime(1)*/ 1);
				m_NavmeshTimer = new Timer();
				m_NavmeshTimer.Run(3, this, "NavmeshUpdate", NULL, true);
				RequestLatentTransition(AdjustTime(GetOpeningTime()));
			break;
			case EUndegroundEntranceState.OPENING_E:
				m_AnimTimerDoorServer.Stop();
				NavmeshUpdate();
				m_NavmeshTimer = null;
				RequestLatentTransition(AdjustTime(3));
			break;
			case EUndegroundEntranceState.OPENING_F:
				RequestLatentTransition(AdjustTime(3));
			break;		
			case EUndegroundEntranceState.OPENING_G:
				RequestLatentTransition(AdjustTime(300));
			break;
			case EUndegroundEntranceState.CLOSING_A:
				RequestLatentTransition(AdjustTime(3));
			break;
			case EUndegroundEntranceState.CLOSING_B:
				RequestLatentTransition(AdjustTime(3));
			break;
			case EUndegroundEntranceState.CLOSING_C:
				m_NavmeshTimer = new Timer();
				m_NavmeshTimer.Run(3, this, "NavmeshUpdate", NULL, true);
				m_AnimTimerDoorServer.Run(0, this, "OnUpdateServer", "OnFinishedTimerServer", AdjustTime(GetOpeningTime()),false, /*1/ AdjustTime(1)*/ 1);
				RequestLatentTransition(AdjustTime(GetOpeningTime()));
			break;
			case EUndegroundEntranceState.CLOSING_D:
				NavmeshUpdate();
				m_NavmeshTimer = null;
				RequestLatentTransition(AdjustTime(2));
			break;
			case EUndegroundEntranceState.CLOSING_E:
				RequestLatentTransition(AdjustTime(1));
			break;
			case EUndegroundEntranceState.CLOSING_F:
				RequestLatentTransition(AdjustTime(3));
			break;
			case EUndegroundEntranceState.CLOSING_G:
				RequestLatentTransition(0.25, EUndegroundEntranceState.CLOSED);
			break;
		}
	}

	vector GetLightPosition()
	{
		vector pos;
		if (MemoryPointExists("SirenLightPos"))
		{
			pos = GetMemoryPointPos("SirenLightPos");
			pos = ModelToWorld(pos);
		}
		else
		{
			ErrorEx("GetLightPosition could not locate memory point 'SirenLightPos'");
		}
		return pos;
	}
	
	void SoundEnded(Effect eff)
	{
		if (eff == m_DoorMovementSoundIn)
		{
			PlaySoundSetAtMemoryPointLooped(m_DoorMovementSoundLoop, DOORMOVING_SOUNDSET_LOOP, "DoorEngineSoundPos");
			m_DoorMovementSoundIn = null;
		}
		else if (eff == m_DoorEngineSoundIn)
		{
			PlaySoundSetAtMemoryPointLooped(m_DoorEngineSoundLoop, ENGINE_SOUNDSET_LOOP, "DoorEngineSoundPos");
			m_DoorEngineSoundIn = null;
		}
	}

	//do note that one-time effects are played even if the client connects and misses the state switch, ie. they are not connected the exact moment when the effect is supposed to be played(just after a state switch), but connect sometime later
	//we can prevent such effects from playing by checking for IsInitDoorStateSync(), but that seems unnecessary as the issue is really small
	override void HandleAudioPlayback(EUndegroundEntranceState newState, EUndegroundEntranceState prevState)
	{
		//Print("HandleAudioVisualPlayback " + newState + ", " + prevState);
		//opening
		if ( CheckShouldPlayPersistent(EUndegroundEntranceState.OPENING_A, EUndegroundEntranceState.OPENING_F))
		{
			PlaySoundSetAtMemoryPointLooped(m_SirenSound, SIREN_SOUNDSET, "SirenSoundPos",0.5,0.5);
		}
		if (newState == EUndegroundEntranceState.OPENING_B)
		{
			if (prevState == EUndegroundEntranceState.OPENING_A)//if they connected already during B, do not play the in
			{
				PlaySoundSetAtMemoryPoint(m_DoorEngineSoundIn, ENGINE_SOUNDSET_LOOP_IN, "DoorEngineSoundPos");
				if (m_DoorEngineSoundIn)
					m_DoorEngineSoundIn.Event_OnEffectEnded.Insert(SoundEnded);
			}
		}
		
		if (CheckShouldPlayPersistent(EUndegroundEntranceState.OPENING_B, EUndegroundEntranceState.OPENING_E))
		{
			if (!m_DoorEngineSoundLoop && !m_DoorEngineSoundIn)//missed playing of the IN soundset which automatically triggers playing of the loop(connected after ?)
			{
				PlaySoundSetAtMemoryPointLooped(m_DoorEngineSoundLoop, ENGINE_SOUNDSET_LOOP, "DoorEngineSoundPos");
			}
		}
		if (newState == EUndegroundEntranceState.OPENING_C)
		{
			PlaySoundSetAtMemoryPoint(m_LockingSound, LOCKING_SOUNDSET, "DoorEngineSoundPos");
		}
		if (newState == EUndegroundEntranceState.OPENING_D)
		{
			if (prevState == EUndegroundEntranceState.OPENING_C)//if they connected already during C, do not play the in
			{
				PlaySoundSetAtMemoryPoint(m_DoorMovementSoundIn, DOORMOVING_SOUNDSET_LOOP_IN, "DoorEngineSoundPos");
				if (m_DoorMovementSoundIn)
					m_DoorMovementSoundIn.Event_OnEffectEnded.Insert(SoundEnded);
			}
			else if (!m_DoorMovementSoundIn && !m_DoorMovementSoundLoop)//missed playing of the IN soundset which automatically triggers playing of the loop(connected after ?)
			{
				PlaySoundSetAtMemoryPointLooped(m_DoorMovementSoundLoop, DOORMOVING_SOUNDSET_LOOP, "DoorEngineSoundPos");
			}
		}
		
		if (newState == EUndegroundEntranceState.OPENING_E)
		{
			StopSoundSet(m_DoorMovementSoundLoop);
			PlaySoundSetAtMemoryPoint(m_DoorMovementSoundOut, DOORMOVING_SOUNDSET_LOOP_OUT, "DoorEngineSoundPos");
		}
		if (newState == EUndegroundEntranceState.OPENING_F)
		{
			StopSoundSet(m_DoorEngineSoundLoop);
			PlaySoundSetAtMemoryPoint(m_DoorEngineSoundOut, ENGINE_SOUNDSET_LOOP_OUT, "DoorEngineSoundPos");
		}
		if (newState == EUndegroundEntranceState.OPENING_G)
		{
			StopSoundSet(m_SirenSound);
		}
		
		//closing
		if (CheckShouldPlayPersistent(EUndegroundEntranceState.CLOSING_A, EUndegroundEntranceState.CLOSING_F))
		{
			PlaySoundSetAtMemoryPointLooped(m_SirenSound, SIREN_SOUNDSET, "SirenSoundPos",0.5,0.5);
		}
		if (CheckShouldPlayPersistent(EUndegroundEntranceState.CLOSING_B, EUndegroundEntranceState.CLOSING_E))
		{
			if (prevState == EUndegroundEntranceState.CLOSING_A)//if they connected already during B, do not play the in
			{
				PlaySoundSetAtMemoryPoint(m_DoorEngineSoundIn, ENGINE_SOUNDSET_LOOP_IN, "DoorEngineSoundPos");
				if (m_DoorEngineSoundIn)
					m_DoorEngineSoundIn.Event_OnEffectEnded.Insert(SoundEnded);
			}
		}		
		if ( newState == EUndegroundEntranceState.CLOSING_C)
		{
			if (prevState == EUndegroundEntranceState.CLOSING_B)//if they connected already during C, do not play the in
			{
				PlaySoundSetAtMemoryPoint(m_DoorMovementSoundIn, DOORMOVING_SOUNDSET_LOOP_IN, "DoorEngineSoundPos");
				if (m_DoorMovementSoundIn)
					m_DoorMovementSoundIn.Event_OnEffectEnded.Insert(SoundEnded);
			}
			else if (!m_DoorMovementSoundIn && !m_DoorMovementSoundLoop)//missed playing of the IN soundset which automatically triggers playing of the loop(connected after ?)
			{
				PlaySoundSetAtMemoryPointLooped(m_DoorMovementSoundLoop, DOORMOVING_SOUNDSET_LOOP, "DoorEngineSoundPos");
			}
		}
		if (newState == EUndegroundEntranceState.CLOSING_D)
		{
			StopSoundSet(m_DoorMovementSoundLoop);
			PlaySoundSetAtMemoryPoint(m_DoorMovementSoundOut, DOORMOVING_SOUNDSET_LOOP_OUT, "DoorEngineSoundPos");
		}
		if (newState == EUndegroundEntranceState.CLOSING_E)
		{
			PlaySoundSetAtMemoryPoint(m_LockingSound, LOCKING_SOUNDSET, "DoorEngineSoundPos");
		}
		if (newState == EUndegroundEntranceState.CLOSING_F)
		{
			StopSoundSet(m_DoorEngineSoundLoop);
			PlaySoundSetAtMemoryPoint(m_DoorEngineSoundOut, ENGINE_SOUNDSET_LOOP_OUT, "DoorEngineSoundPos");
		}
		if (newState == EUndegroundEntranceState.CLOSING_G || newState == EUndegroundEntranceState.CLOSED)
		{
			StopSoundSet(m_SirenSound);
		}
	}
	
	override void OnUpdateClient(float timeSlice)
	{
		SetAnimationPhaseNow("EntranceDoor",m_AnimPhase);
		if (m_AlarmLight)
		{
			vector newOri = m_AlarmLight.GetOrientation() + Vector(timeSlice * LIGHT_ROT_SPEED,0,0);
			m_AlarmLight.SetOrientation(newOri);
		}
	}

	override void HandleVisualPlayback(EUndegroundEntranceState newState, EUndegroundEntranceState prevState)
	{
		if ( CheckShouldPlayPersistent(EUndegroundEntranceState.OPENING_A, EUndegroundEntranceState.OPENING_F) || CheckShouldPlayPersistent(EUndegroundEntranceState.CLOSING_A, EUndegroundEntranceState.CLOSING_F))
		{
			m_AlarmLight = AlarmLight.Cast( ScriptedLightBase.CreateLight( AlarmLight, GetLightPosition()) );
		}
		if ( newState == EUndegroundEntranceState.CLOSING_G || newState == EUndegroundEntranceState.CLOSED || newState == EUndegroundEntranceState.OPENING_G )
		{
			if (m_AlarmLight)
			{
				m_AlarmLight.Destroy();
			}
		}
	}
	
}
