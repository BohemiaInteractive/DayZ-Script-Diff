class Land_Underground_Stairs_Exit : Land_Underground_EntranceBase
{
	EffectSound 		m_LockingSound;
	
	EffectSound 		m_OpenSoundIn;
	//EffectSound 		m_OpenSoundLoop;
	EffectSound 		m_OpenSoundOut;

	EffectSound 		m_CloseSoundIn;
	EffectSound 		m_CloseSoundLoop;
	EffectSound 		m_CloseSoundOut;

	const string LOCKING_SOUNDSET 			= "UndergroundDoor_Lock_SoundSet";
	const string OPENING_SOUNDSET_LOOP_IN 	= "UndergroundSmallExitDoor_Open_Start_SoundSet";
	const string OPENING_SOUNDSET_LOOP 		= "UndergroundSmallExitDoor_Open_Loop_SoundSet";
	const string OPENING_SOUNDSET_LOOP_OUT 	= "UndergroundSmallExitDoor_Open_End_SoundSet";
	
	const string CLOSING_SOUNDSET_LOOP_IN 	= "UndergroundSmallExitDoor_Close_Start_SoundSet";
	const string CLOSING_SOUNDSET_LOOP 		= "UndergroundSmallExitDoor_Close_Loop_SoundSet";
	const string CLOSING_SOUNDSET_LOOP_OUT	= "UndergroundSmallExitDoor_Close_End_SoundSet";
	
	void Land_Underground_Stairs_Exit()
	{
		m_DoorType = EUndegroundDoorType.SMALL;
	}
	
	override void CleanUpOnClosedClient()
	{
		SEffectManager.DestroySound(m_LockingSound);
		
		SEffectManager.DestroySound(m_OpenSoundIn);
		//SEffectManager.DestroySound(m_OpenSoundLoop);
		SEffectManager.DestroySound(m_OpenSoundOut);
		
		SEffectManager.DestroySound(m_CloseSoundIn);
		SEffectManager.DestroySound(m_CloseSoundLoop);
		SEffectManager.DestroySound(m_CloseSoundOut);

	}
	
	override float GetOpeningTime()
	{
		return AdjustTime(3);
	}
	
	override void OnDoorStateChangedServer(EUndegroundEntranceState newState)
	{
		switch (newState)
		{
			case EUndegroundEntranceState.OPENING_A:
				RequestLatentTransition(AdjustTime(1));
				SetAnimationPhase("EntranceDoor",1);
			break;
			case EUndegroundEntranceState.OPENING_B:
				GetGame().UpdatePathgraphRegionByObject(this);
				RequestLatentTransition(AdjustTime(6), EUndegroundEntranceState.CLOSING_A);
			break;
			case EUndegroundEntranceState.CLOSING_A:
				m_AnimTimerDoorServer = new AnimationTimer();
				m_AnimTimerDoorServer.Run(0, this, "OnUpdateServer", "OnFinishedTimerServer", GetOpeningTime(), false,1/ AdjustTime(1));
				RequestLatentTransition(AdjustTime(3));
			break;
			case EUndegroundEntranceState.CLOSING_B:
				RequestLatentTransition(AdjustTime(1), EUndegroundEntranceState.CLOSED);
				GetGame().UpdatePathgraphRegionByObject(this);	
			break;
		}
	}
	void SoundEnded(Effect eff)
	{
		if (eff == m_CloseSoundIn)
		{
			PlaySoundSetLoop( m_CloseSoundLoop, CLOSING_SOUNDSET_LOOP, 0, 0 );
		}
	}
	
	override void HandleAudioPlayback(EUndegroundEntranceState newState, EUndegroundEntranceState prevState)
	{
		if (newState == EUndegroundEntranceState.OPENING_A)
		{
			PlaySoundSet( m_LockingSound, LOCKING_SOUNDSET, 0, 0 );
			PlaySoundSet( m_OpenSoundIn, OPENING_SOUNDSET_LOOP_IN, 0, 0 );
		}
		else if (newState == EUndegroundEntranceState.OPENING_B)
		{
			if (m_OpenSoundIn)
				m_OpenSoundIn.Stop();
			PlaySoundSet( m_OpenSoundOut, OPENING_SOUNDSET_LOOP_OUT, 0, 0 );
		}
		else if (newState == EUndegroundEntranceState.CLOSING_A)
		{
			PlaySoundSet( m_CloseSoundIn, CLOSING_SOUNDSET_LOOP_IN, 0, 0 );
			if(m_CloseSoundIn)
				m_CloseSoundIn.Event_OnEffectEnded.Insert(SoundEnded);
		}
		else if (newState == EUndegroundEntranceState.CLOSING_B)
		{
			if (m_CloseSoundLoop)
				m_CloseSoundLoop.Stop();
			PlaySoundSet( m_CloseSoundOut, CLOSING_SOUNDSET_LOOP_OUT, 0, 0 );
			PlaySoundSet( m_LockingSound, LOCKING_SOUNDSET, 0, 0 );
			
		}
	}
	
	override void CreateLights()
	{
		m_InteriorLight1 = EntranceLight.Cast(ScriptedLightBase.CreateLightAtObjMemoryPoint(EntranceLightStairs1, this, "Light1"));
		m_InteriorLight2 = EntranceLight.Cast(ScriptedLightBase.CreateLightAtObjMemoryPoint(EntranceLightStairs2, this, "Light2"));
	}
	
}