class WaterLevelSettings
{
	int WaterLevel;
	float Duration;
	
	void WaterLevelSettings(int pWaterLevel, float pDuration)
	{
		WaterLevel 	= pWaterLevel;
		Duration	= pDuration;
	}
}

class PressureLevelSettings
{
	int PressureLevel;
	float Duration;
	
	void PressureLevelSettings(int pPressureLevel, float pDuration)
	{
		PressureLevel 	= pPressureLevel;
		Duration		= pDuration;
	}
}

class WaterLevelSnapshot
{
	float WaterHeight;
	float RemainingDuration;
}

class Land_Underground_WaterReservoir : BuildingBase
{
	protected const string OBJECT_NAME_WATER_PLANE 				= "Land_Underground_WaterReservoir_Water";

	protected const int WL_MIN 									= 0;
	protected const int WL_ABOVE_PIPES							= 1;
	protected const int WL_AVERAGE 								= 2;
	protected const int WL_MAX 									= 3;
	
	protected const int PL_MIN 									= 0;
	protected const int PL_AVERAGE 								= 1;
	protected const int PL_MAX 									= 2;
	
	protected const int VALVES_COUNT							= 2;
	protected const int VALVE_INDEX_DRAIN 						= 0;
	protected const int VALVE_INDEX_FILL 						= 1;
	
	protected const int PIPES_BROKEN_COUNT						= 2;
	protected const int PIPE_INDEX_BROKEN1						= 0;	//! main broken pipe
	protected const int PIPE_INDEX_BROKEN2						= 1;	//! tighter broken pipe

	protected const string ANIM_PHASE_VALVE_GAUGE_DRAIN 		= "ValveGauge1";
	protected const string ANIM_PHASE_VALVE_GAUGE_FILL 			= "ValveGauge2";
	protected const string ANIM_PHASE_VALVE_DRAIN 				= "Valve1";
	protected const string ANIM_PHASE_VALVE_FILL 				= "Valve2";
	protected const string VALVE_NAME_DRAIN 					= "valve1";
	protected const string VALVE_NAME_FILL		 				= "valve2";
	protected const string PIPE_NAME_BROKEN1	 				= "pipe_broken_1";
	protected const string PIPE_NAME_BROKEN2	 				= "pipe_broken_2";
	protected const string PIPE_NAME_LEAKING_DRAIN 				= "pipe_leaking_1";
	protected const string PIPE_NAME_LEAKING_FILL				= "pipe_leaking_2";
	
	protected const string SOUND_NAME_PIPE_CREAKING				= "WaterObjectUndergroundPipeCreaking_SoundSet";
	protected const string SOUND_NAME_VALVE_MANIPULATION		= "WaterObjectUndergroundValve_SoundSet";
	protected const string SOUND_NAME_PIPE_SPRINKLING_START		= "WaterObjectUndergroundPipeSprinkling_Start_SoundSet";
	protected const string SOUND_NAME_PIPE_SPRINKLING_END		= "WaterObjectUndergroundPipeSprinkling_End_SoundSet";
	protected const string SOUND_NAME_PIPE_SPRINKLING_LOOP1		= "WaterObjectUndergroundPipeSprinkling_Loop1_SoundSet";
	protected const string SOUND_NAME_PIPE_SPRINKLING_LOOP2		= "WaterObjectUndergroundPipeSprinkling_Loop2_SoundSet";
	protected const string SOUND_NAME_UPIPE_SPRINKLING_START	= "WaterObjectUndergroundUnderwaterPipe_Start_SoundSet";
	protected const string SOUND_NAME_UPIPE_SPRINKLING_END		= "WaterObjectUndergroundUnderwaterPipe_End_SoundSet";
	protected const string SOUND_NAME_UPIPE_SPRINKLING_LOOP		= "WaterObjectUndergroundUnderwaterPipe_Loop_SoundSet";
	protected const string SOUND_NAME_WATER_FILL_LOOP			= "WaterObjectUndergroundUnderwaterFill_Loop_SoundSet";
	protected const string SOUND_NAME_WATER_DRAIN_LOOP			= "WaterObjectUndergroundUnderwaterEmpty_Loop_SoundSet";
	
	protected const int PARTICLE_DRAIN_PIPE_MAX_PRESSURE 		= ParticleList.WATER_SPILLING;
	protected const int PARTICLE_FILL_PIPE_MAX_PRESSURE 		= ParticleList.WATER_SPILLING;
	protected const int PARTICLE_FILL_PIPE_JET 					= ParticleList.WATER_JET;
	protected const int PARTICLE_FILL_PIPE_JET_WEAK				= ParticleList.WATER_JET_WEAK;
	
	//! valve/pipe stages for water and pressure levels 
	protected ref array<ref WaterLevelSettings> 				m_DrainValveWaterStageSettings;
	protected ref array<ref PressureLevelSettings> 				m_DrainValvePressureStageSettings;
	protected ref array<ref PressureLevelSettings>				m_DrainValvePressureDeanimationSettings; //! for deanimation purposes
	protected ref array<ref WaterLevelSettings> 				m_FillValveWaterStageSettings;
	protected ref array<ref PressureLevelSettings> 				m_FillValvePressureStageSettings;
	protected ref array<ref PressureLevelSettings>				m_FillValvePressureDeanimationSettings; //! for deanimation purposes
	protected ref WaterLevelSnapshot							m_WaterLevelSnapshot;
	
	//! pointing to specific stage for each valve/pipe
	protected int m_DrainValveWaterLevelStageIndex;
	protected int m_DrainValvePressureLevelStageIndex;
	protected int m_FillValveWaterLevelStageIndex;
	protected int m_FillValvePressureLevelStageIndex;

	protected static const string WATER_LEVEL_MAX				= "water_level_max";
	protected static const string WATER_LEVEL_AVERAGE			= "water_level_average";
	protected static const string WATER_LEVEL_ABOVE_PIPES		= "water_level_above_pipes";
	protected static const string WATER_LEVEL_MIN				= "water_level_min";
	
	protected const int WATER_LEVELS_COUNT						= 4;
	protected const string WATER_LEVELS[WATER_LEVELS_COUNT] 	= {
		WATER_LEVEL_MAX,
		WATER_LEVEL_AVERAGE,
		WATER_LEVEL_ABOVE_PIPES,
		WATER_LEVEL_MIN
	};

	protected int 						m_ValveStatesPacked;
	protected int 						m_ValveStatesPackedPrev;
	protected int 						m_ValveManipulatedIndex;
	protected int 						m_ValveManipulatedIndexPrev;
	protected ref array<string> 		m_ValveNames;
	protected ref array<bool> 			m_ValveStates;
	protected ref map<string, vector> 	m_WaterLevelsAvailable;
	protected int 						m_WaterLevelActual;
	protected int 						m_WaterLevelPrev;
	protected ref array<bool>			m_PressureAnimationRequests;
	protected ref array<bool>			m_PressureDeanimationRequests;
	protected ref array<float>			m_WaterLevelTimesAccumulated;
	protected ref array<float>			m_PressureTimesAccumulated;
	protected int 						m_LastActiveValve;

	protected Object					m_SpawnedWaterObject;
	protected vector 					m_WaterLevelDefault;
	protected float 					m_WaterLevelHeightActual;
	
	//! VFX/SFX
	protected ref EffectSound			m_ValveManipulationSound;
	protected ref array<EffectSound>	m_PipeSounds;

	protected bool 						m_PipeCreakingSoundRequested;
	protected ref EffectSound			m_PipeCreakingSounds;
	
	protected bool 						m_PipeUnderwaterSoundRunning
	protected ref EffectSound 			m_PipeUnderwaterSound;
	
	protected ref EffectSound 			m_WaterLevelMovementSound;
	
	protected const int PIPE_CREAKING_MIN_TIME_DELAY_MS		= 10000;
	protected const int PIPE_CREAKING_MAX_TIME_DELAY_MS		= 15000;
	protected const int PIPE_CREAKING_SOUND_LOCATIONS_COUNT = 4;
	protected const string PIPE_CREAKING_SOUND_LOCATIONS[PIPE_CREAKING_SOUND_LOCATIONS_COUNT] = {
		"pipe_creaking_sound_pos_1",
		"pipe_creaking_sound_pos_2",
		"pipe_creaking_sound_pos_3",
		"pipe_creaking_sound_pos_4"
	};
	
	protected ref ParticleSourceArray	m_ValveParticles;
	protected ref ParticleSourceArray	m_PipeBrokenParticles;	
	
	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		#ifndef SERVER
		HandleSoundEffects();
		HandleVisualEffects();
		#endif
		
		if (!GetGame().IsServer())
		{
			return;
		}
		
		int valve;
		float waterHeight, pressureLevel;
		WaterLevelSettings wlStageSettings, wlStageSettingsPrev;
		PressureLevelSettings plStageSettings, plStageSettingsPrev;

		//! In case where one of the valves is already running
		//! - snapshot values; stop animation of that valve and continue with newly requested
		for (valve = 0; valve < VALVES_COUNT; ++valve)
		{
			if (m_LastActiveValve != INDEX_NOT_FOUND && m_LastActiveValve != valve && m_ValveStates[valve] == true)
			{
				wlStageSettings 						= ActualWaterLevelStageSettings(valve);
				m_WaterLevelSnapshot 					= new WaterLevelSnapshot();
				m_WaterLevelSnapshot.WaterHeight 		= m_WaterLevelHeightActual;
				m_WaterLevelSnapshot.RemainingDuration	= Math.Clamp(wlStageSettings.Duration - m_WaterLevelTimesAccumulated[valve], 0, wlStageSettings.Duration);
				m_ValveStates[valve] 					= false;
				m_PressureDeanimationRequests[valve]	= true;	//! will force that valve to start deanimation of gauge
				break;
			}
		}
		
		//! water level course and animation
		for (valve = 0; valve < VALVES_COUNT; ++valve)
		{
			if (m_ValveStates[valve] == true)
			{
				wlStageSettingsPrev	= PreviousWaterLevelStageSettings(valve);
				wlStageSettings 	= ActualWaterLevelStageSettings(valve);

				//! stay on the same level
				if (wlStageSettings.Duration == -1.0)
				{
					m_ValveStates[valve] 				= false;
					m_WaterLevelTimesAccumulated[valve] = 0;
					m_WaterLevelSnapshot 				= null;
					SyncValveVariables();
					return;
				}
				
				float adjustedWaterHeight 	= WaterLevelToHeight(wlStageSettingsPrev.WaterLevel);
				float adjustedDuration		= wlStageSettings.Duration;
				if (m_WaterLevelSnapshot)
				{
					adjustedWaterHeight	= m_WaterLevelSnapshot.WaterHeight;
					adjustedDuration 	= wlStageSettings.Duration + m_WaterLevelSnapshot.RemainingDuration;
				}
				
				if (m_WaterLevelTimesAccumulated[valve] <= adjustedDuration)
				{
					float start 	= adjustedWaterHeight;
					float target 	= WaterLevelToHeight(wlStageSettings.WaterLevel);
					waterHeight 	= Math.Lerp(start, target, Easing.EaseInOutSine(m_WaterLevelTimesAccumulated[valve] / adjustedDuration));
		        	SetWaterLevelHeight(waterHeight);
				}
				else
				{
					m_WaterLevelTimesAccumulated[valve] = 0;
					m_WaterLevelSnapshot 				= null;
					AdvanceToNextWaterLevelStageSettings(valve);
					SetSynchDirty();
				}

				m_WaterLevelTimesAccumulated[valve] = m_WaterLevelTimesAccumulated[valve] + AdjustTime(timeSlice);
			}
			else
			{
				m_WaterLevelTimesAccumulated[valve] = 0;
				SyncValveVariables();
			}
		}
		
		int allValvesStates = 0;

		//! pressure course and animation
		for (valve = 0; valve < VALVES_COUNT; ++valve)
		{
			if (m_PressureAnimationRequests[valve] == true)
			{
				bool hasToBeDeanimated 	= m_PressureAnimationRequests[valve] == m_PressureDeanimationRequests[valve];
				plStageSettingsPrev		= PreviousPressureLevelStageSettings(valve, hasToBeDeanimated);
				plStageSettings 		= ActualPressureLevelStageSettings(valve, hasToBeDeanimated);
				
				//! stay on the same level
				if (plStageSettings.Duration == -1.0)
				{
					m_PressureTimesAccumulated[valve] 	= 0;
					m_PressureAnimationRequests[valve] 	= false;
					SyncValveVariables();
					return;
				}
				
				if (m_PressureTimesAccumulated[valve] <= plStageSettings.Duration)
				{
					float plStart 	= PressureLevelToValue(plStageSettingsPrev.PressureLevel);
					float plTarget 	= PressureLevelToValue(plStageSettings.PressureLevel);
					pressureLevel 	= Math.Lerp(plStart, plTarget, m_PressureTimesAccumulated[valve] / plStageSettings.Duration);
					SetValvePressureLevelGauge(valve, pressureLevel);
				}
				else
				{
					m_PressureTimesAccumulated[valve] = 0;
					//! end of deanimation - overlapped activation of valves
					if (hasToBeDeanimated && IsValvePressureLevelGaugeAtBase(valve))
					{
						m_PressureDeanimationRequests[valve] = false;
						SetDefaultPressureLevelStageSettings(valve);
					}
					else
					{
						AdvanceToNextPressureLevelStageSettings(valve);
						SetSynchDirty();
					}
				}
	
				m_PressureTimesAccumulated[valve] = m_PressureTimesAccumulated[valve] + AdjustTime(timeSlice);
			}
			else
			{
				m_PressureTimesAccumulated[valve] = 0;
			}
			
			
			allValvesStates += m_PressureAnimationRequests[valve];
			if (allValvesStates == false)
			{
				SyncValveVariables();
			}
		}
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		m_ValveStates = UnpackBitsToArrayOfBoolStates(m_ValveStatesPacked, VALVES_COUNT);
		
		PlayValveManipulationSound();
	}
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionTurnValveUndergroundReservoir);
	}
	
	override bool HasTurnableValveBehavior()
	{
		return true;
	}
	
	override bool IsValveTurnable(int pValveIndex)
	{
		return !IsValveActive(pValveIndex) && IsValvePressureLevelGaugeAtBase(pValveIndex);
	}
	
	protected bool IsAnyValveActive()
	{
		for (int i = 0; i < m_ValveStates.Count(); ++i)
		{
			if (m_ValveStates[i])
			{
				return true;
			}
		}
		
		return false;
	}
	
	protected bool IsValveActive(int pValveIndex)
	{
		return pValveIndex >= 0 && pValveIndex <= VALVES_COUNT && m_ValveStates[pValveIndex];
	}
	
	override int GetTurnableValveIndex(int pComponentIndex)
	{
		string targetedValveName = GetActionComponentName(pComponentIndex);
		for (int i = 0; i < m_ValveNames.Count(); ++i)
		{
			if (m_ValveNames[i] == targetedValveName)
			{
				return i;
			}
		}
		
		return INDEX_NOT_FOUND;
	}
	
	void OnValveManipulationStart(int pValveIndex)
	{
		if (GetGame().IsServer())
		{
			AnimateValve(pValveIndex, 1);
			m_ValveManipulatedIndex = pValveIndex;
			SetSynchDirty();
		}
	}
	
	void OnValveManipulationEnd(int pValveIndex)
	{
		if (GetGame().IsServer())
		{
			switch (pValveIndex)
			{
			case VALVE_INDEX_DRAIN:
				m_ValveStates[pValveIndex]					= true;
				m_PressureAnimationRequests[pValveIndex] 	= true;
				m_DrainValveWaterLevelStageIndex 			= 1;
				m_DrainValvePressureLevelStageIndex 		= 1;
				SetLastActiveValve(pValveIndex);
				AnimateValve(pValveIndex, 0);
			break;
			case VALVE_INDEX_FILL:
				m_ValveStates[pValveIndex]					= true;
				m_PressureAnimationRequests[pValveIndex] 	= true;
				m_FillValveWaterLevelStageIndex 			= 1;
				m_FillValvePressureLevelStageIndex 			= 1;
				AnimateValve(pValveIndex, 0);
				SetLastActiveValve(pValveIndex);
			break;
			}

			SyncValveVariables();
		}
	}
	
	void OnValveManipulationCanceled(int pValveIndex)
	{
		if (GetGame().IsServer())
		{
			AnimateValve(pValveIndex, 0);
			m_ValveManipulatedIndex = -1;
			SetSynchDirty();
		}	
	}
	
	//! for the animation aligning
	//! returns data in order: start point; direction point 
	array<vector> GetValveAligningPointsWS(int pValveIndex)
	{
		vector posPoint, dirPoint = vector.Zero;
		
		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			posPoint = GetMemoryPointPosition(string.Format("%1_align_pos", VALVE_NAME_DRAIN));
			dirPoint = GetMemoryPointPosition(string.Format("%1_align_dir", VALVE_NAME_DRAIN));
		break;
		case VALVE_INDEX_FILL:
			posPoint = GetMemoryPointPosition(string.Format("%1_align_pos", VALVE_NAME_FILL));
			dirPoint = GetMemoryPointPosition(string.Format("%1_align_dir", VALVE_NAME_FILL));
		break;
		}
		
		array<vector> valvePositions = new array<vector>();
		valvePositions.Insert(posPoint);
		valvePositions.Insert(dirPoint);

		return valvePositions;
	}

	protected void SetLastActiveValve(int pValveIndex)
	{
		m_LastActiveValve = pValveIndex;
	}
	
	protected void SetWaterLevelHeight(float pHeight)
	{
		if (GetGame() && GetGame().IsServer())
		{
			vector pos 					= Vector(m_WaterLevelDefault[0], pHeight, m_WaterLevelDefault[2]);
			m_WaterLevelHeightActual	= pHeight;
			m_WaterLevelActual			= HeightToWaterLevel(pHeight);

			if (m_SpawnedWaterObject)
			{
				m_SpawnedWaterObject.SetPosition(pos);
			}
		}
	}
	
	protected void RegisterValve(string pCompName, int pIndex)
	{
		m_ValveNames.Set(pIndex, pCompName);
		m_ValveStates.Set(pIndex, false);
		m_PressureAnimationRequests.Set(pIndex, 0);
		m_PressureDeanimationRequests.Set(pIndex, 0);
		m_WaterLevelTimesAccumulated.Set(pIndex, 0);
		m_PressureTimesAccumulated.Set(pIndex, false);
		m_PipeSounds.Set(pIndex, null);
	}
	
	protected void TranslateMemoryPointsToWaterLevels()
	{
		m_WaterLevelsAvailable.Clear();
		for (int i = 0; i < WATER_LEVELS_COUNT; ++i)
		{
			if (MemoryPointExists(WATER_LEVELS[i]))
			{
				if (!m_WaterLevelsAvailable.Contains(WATER_LEVELS[i]))
				{
					m_WaterLevelsAvailable.Insert(WATER_LEVELS[i], ModelToWorld(GetMemoryPointPos(WATER_LEVELS[i])));
				}
			}
		}
	}
	
	protected float WaterLevelToHeight(int pWaterLevel)
	{
		switch (pWaterLevel)
		{
		case WL_MIN:
			return m_WaterLevelsAvailable[WATER_LEVEL_MIN][1];
		case WL_ABOVE_PIPES:
			return m_WaterLevelsAvailable[WATER_LEVEL_ABOVE_PIPES][1];		
		case WL_AVERAGE:
			return m_WaterLevelsAvailable[WATER_LEVEL_AVERAGE][1];
		case WL_MAX:
			return m_WaterLevelsAvailable[WATER_LEVEL_MAX][1];
		}
		
		return 0;
	}
	
	protected int HeightToWaterLevel(float pHeight)
	{
		if (pHeight <= m_WaterLevelsAvailable[WATER_LEVEL_MIN][1])
		{
			return WL_MIN;
		}
		else if (pHeight > m_WaterLevelsAvailable[WATER_LEVEL_MIN][1] && pHeight <= m_WaterLevelsAvailable[WATER_LEVEL_ABOVE_PIPES][1])
		{
			return WL_ABOVE_PIPES;
		}
		else if (pHeight > m_WaterLevelsAvailable[WATER_LEVEL_ABOVE_PIPES][1] && pHeight <= m_WaterLevelsAvailable[WATER_LEVEL_AVERAGE][1])
		{
			return WL_AVERAGE;
		}
		else if (pHeight < m_WaterLevelsAvailable[WATER_LEVEL_MAX][1] && pHeight >= m_WaterLevelsAvailable[WATER_LEVEL_AVERAGE][1])
		{
			return WL_AVERAGE;
		}

		return WL_MAX;
	}
	
	protected float PressureLevelToValue(int pPressureLevel)
	{
		switch (pPressureLevel)
		{
		case PL_MIN:
			return 0.0;
		case PL_AVERAGE:
			return 0.5;
		case PL_MAX:
			return 1.0;
		}
		
		return 0.0;
	}
	
	protected WaterLevelSettings PreviousWaterLevelStageSettings(int pValveIndex)
	{
		WaterLevelSettings wlStageSettings;

		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			if (m_DrainValveWaterLevelStageIndex > 0)
			{
				wlStageSettings = m_DrainValveWaterStageSettings.Get(m_DrainValveWaterLevelStageIndex - 1);
			}
			else
			{
				wlStageSettings = m_DrainValveWaterStageSettings.Get(m_DrainValveWaterLevelStageIndex);
			}
		break;
		case VALVE_INDEX_FILL:
			if (m_FillValveWaterLevelStageIndex > 0)
			{
				wlStageSettings = m_FillValveWaterStageSettings.Get(m_FillValveWaterLevelStageIndex - 1);
			}
			else
			{
				wlStageSettings = m_FillValveWaterStageSettings.Get(m_FillValveWaterLevelStageIndex);
			}
		break;
		}
		
		m_WaterLevelPrev = wlStageSettings.WaterLevel;
		SetSynchDirty();
		
		return wlStageSettings;
	}
	
	protected WaterLevelSettings ActualWaterLevelStageSettings(int pValveIndex)
	{
		WaterLevelSettings wlStageSettings;

		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			wlStageSettings = m_DrainValveWaterStageSettings.Get(m_DrainValveWaterLevelStageIndex);
		break;
		case VALVE_INDEX_FILL:
			wlStageSettings = m_FillValveWaterStageSettings.Get(m_FillValveWaterLevelStageIndex);
		break;
		}

		return wlStageSettings;
	}

	protected void AdvanceToNextWaterLevelStageSettings(int pValveIndex)
	{
		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			if (m_DrainValveWaterLevelStageIndex < m_DrainValveWaterStageSettings.Count() - 1)
			{
				m_DrainValveWaterLevelStageIndex++;
			}
			else
			{
				m_DrainValveWaterLevelStageIndex = 0;
			}
		break;
		case VALVE_INDEX_FILL:
			if (m_FillValveWaterLevelStageIndex < m_FillValveWaterStageSettings.Count() - 1)
			{
				m_FillValveWaterLevelStageIndex++;
			}
			else
			{
				m_FillValveWaterLevelStageIndex = 0;
			}
		break;
		}
	}
	
	protected PressureLevelSettings PreviousPressureLevelStageSettings(int pValveIndex, bool pDeanimationRequest = false)
	{
		PressureLevelSettings plStageSettings;

		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			if (pDeanimationRequest)
			{
				plStageSettings = m_FillValvePressureDeanimationSettings[0];
				return plStageSettings;
			}
			
			if (m_DrainValvePressureLevelStageIndex > 0)
			{
				plStageSettings = m_DrainValvePressureStageSettings.Get(m_DrainValvePressureLevelStageIndex - 1);
			}
			else
			{
				plStageSettings = m_DrainValvePressureStageSettings.Get(m_DrainValvePressureLevelStageIndex);
			}
		break;
		case VALVE_INDEX_FILL:
			if (pDeanimationRequest)
			{
				plStageSettings = m_FillValvePressureDeanimationSettings[0];
				return plStageSettings;
			}

			if (m_FillValvePressureLevelStageIndex > 0)
			{
				plStageSettings = m_FillValvePressureStageSettings.Get(m_FillValvePressureLevelStageIndex - 1);
			}
			else
			{
				plStageSettings = m_FillValvePressureStageSettings.Get(m_FillValvePressureLevelStageIndex);
			}
		break;
		}
		
		return plStageSettings;
	}
	
	protected PressureLevelSettings ActualPressureLevelStageSettings(int pValveIndex, bool pDeanimationRequest = false)
	{
		PressureLevelSettings plStageSettings;

		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			if (pDeanimationRequest)
			{
				plStageSettings = m_DrainValvePressureDeanimationSettings[1];
				return plStageSettings;
			}

			plStageSettings = m_DrainValvePressureStageSettings.Get(m_DrainValvePressureLevelStageIndex);
		break;
		case VALVE_INDEX_FILL:
			if (pDeanimationRequest)
			{
				plStageSettings = m_FillValvePressureDeanimationSettings[1];
				return plStageSettings;
			}

			plStageSettings = m_FillValvePressureStageSettings.Get(m_FillValvePressureLevelStageIndex);
		break;
		}

		return plStageSettings;
	}

	protected void AdvanceToNextPressureLevelStageSettings(int pValveIndex)
	{
		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			if (m_DrainValvePressureLevelStageIndex < m_DrainValvePressureStageSettings.Count() - 1)
			{
				m_DrainValvePressureLevelStageIndex++;
			}
			else
			{
				m_DrainValvePressureLevelStageIndex = 0;
			}
		break;
		case VALVE_INDEX_FILL:
			if (m_FillValvePressureLevelStageIndex < m_FillValvePressureStageSettings.Count() - 1)
			{
				m_FillValvePressureLevelStageIndex++;
			}
			else
			{
				m_FillValvePressureLevelStageIndex = 0;
			}
		break;
		}
	}
	
	protected void SetDefaultPressureLevelStageSettings(int pValveIndex)
	{
		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			m_DrainValvePressureLevelStageIndex = 0;
		break;
		case VALVE_INDEX_FILL:
			m_FillValvePressureLevelStageIndex = 0;
		break;
		}
	}
	
	protected void AnimateValve(int pValveIndex, float pPhase)
	{
		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			SetAnimationPhase(ANIM_PHASE_VALVE_DRAIN, pPhase);
		break;
		case VALVE_INDEX_FILL:
			SetAnimationPhase(ANIM_PHASE_VALVE_FILL, pPhase);	
		break;
		}		
	}
	
	protected void SetValvePressureLevelGauge(int pValveIndex, float pValue)
	{
		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			SetAnimationPhase(ANIM_PHASE_VALVE_GAUGE_DRAIN, pValue);
		break;
		case VALVE_INDEX_FILL:
			SetAnimationPhase(ANIM_PHASE_VALVE_GAUGE_FILL, pValue);	
		break;
		}
	}
	
	protected float GetValvePressureLevelGauge(int pValveIndex)
	{
		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			return GetAnimationPhase(ANIM_PHASE_VALVE_GAUGE_DRAIN);
		break;
		case VALVE_INDEX_FILL:
			return GetAnimationPhase(ANIM_PHASE_VALVE_GAUGE_FILL);	
		break;
		}
		
		return 0;
	}
	
	protected bool IsValvePressureLevelGaugeAtBase(int pValveIndex)
	{
		switch (pValveIndex)
		{
		case VALVE_INDEX_DRAIN:
			return GetValvePressureLevelGauge(pValveIndex) <= 0.01;
		break;
		case VALVE_INDEX_FILL:
			return GetValvePressureLevelGauge(pValveIndex) >= 0.99;
		break;
		}
		
		return 0.0;
	}
	
	protected float AdjustTime(float originalTime)
	{
		#ifdef DEVELOPER
		float timeAccel = 1;
		if (FeatureTimeAccel.GetFeatureTimeAccelEnabled(ETimeAccelCategories.UNDERGROUND_RESERVOIR))
		{
			timeAccel = FeatureTimeAccel.GetFeatureTimeAccelValue();
			return originalTime * timeAccel;
		}
		#endif
		return originalTime;
	}

	protected vector GetMemoryPointPosition(string pMemoryPoint)
	{
		vector pos = vector.Zero;

		if (MemoryPointExists(pMemoryPoint))
		{
			pos = GetMemoryPointPos(pMemoryPoint);
			pos = ModelToWorld(pos);
		}
		else
		{
			ErrorEx(string.Format("Memory point %1 not found, falling back to vector.Zero", pMemoryPoint));
		}		
		
		return pos;
	}
	
	protected void HandleSoundEffects()
	{
		HandleSoundEffectsPipeCreaking();
		HandleSoundEffectsPipeSprinkling();
		HandleSoundEffectsUnderwaterPipeSounds();
		HandleSoundEffectsWaterLevelMovementSounds();
	}
	
	protected void PlayValveManipulationSound()
	{
		switch (m_ValveManipulatedIndex)
		{
		case VALVE_INDEX_DRAIN:
			PlaySoundSetAtMemoryPoint(m_ValveManipulationSound, SOUND_NAME_VALVE_MANIPULATION, VALVE_NAME_DRAIN);
		break;
		case VALVE_INDEX_FILL:
			PlaySoundSetAtMemoryPoint(m_ValveManipulationSound, SOUND_NAME_VALVE_MANIPULATION, VALVE_NAME_FILL);
		break;
		}
	}
	
	protected void HandleSoundEffectsPipeCreaking()
	{
		if (IsAnyValveActive())
		{
			if ((m_PipeCreakingSounds == null && !m_PipeCreakingSoundRequested) || !m_PipeCreakingSoundRequested)
			{
				m_PipeCreakingSoundRequested = true;
				int randomDelay = Math.RandomInt(PIPE_CREAKING_MIN_TIME_DELAY_MS, PIPE_CREAKING_MAX_TIME_DELAY_MS);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(PlayPipeCreakingSoundOnLocation, randomDelay, false);
			}
		}
		else
		{
			if (m_PipeCreakingSounds)
			{
				StopSoundSet(m_PipeCreakingSounds);
				m_PipeCreakingSounds = null;
			}
		}
	}
	
	protected void HandleSoundEffectsPipeSprinkling()
	{	
		if (!m_PipeSounds || m_PipeSounds.Count() == 0)
		{
			return;
		}

		EffectSound sndDrain, sndFill;
		float drainPressureLevel = GetValvePressureLevelGauge(VALVE_INDEX_DRAIN);
		if (drainPressureLevel >= 0.4 && drainPressureLevel < 0.8)
		{
			if (m_PipeSounds[VALVE_INDEX_DRAIN] == null)
			{
				PlaySoundSetAtMemoryPoint(sndDrain, SOUND_NAME_PIPE_SPRINKLING_START, VALVE_NAME_DRAIN, false, 0.0, 0.5);
				m_PipeSounds[VALVE_INDEX_DRAIN] = sndDrain;
				StopSoundSet(m_PipeSounds[VALVE_INDEX_DRAIN]);
				m_PipeSounds[VALVE_INDEX_DRAIN] = null;
				PlaySoundSetAtMemoryPointLooped(sndDrain, SOUND_NAME_PIPE_SPRINKLING_LOOP1, VALVE_NAME_DRAIN, 0.5, 0.5);
				m_PipeSounds[VALVE_INDEX_DRAIN] = sndDrain;
			}
		}
		
		if (drainPressureLevel < 0.4)
		{
			if (m_PipeSounds[VALVE_INDEX_DRAIN] != null)
			{
				StopSoundSet(m_PipeSounds[VALVE_INDEX_DRAIN]);
				m_PipeSounds[VALVE_INDEX_DRAIN] = null;
			}
		}

		//! --------------------

		float fillPressureLevel = GetValvePressureLevelGauge(VALVE_INDEX_FILL);
		if (fillPressureLevel >= 0.4)
		{
			if (m_PipeSounds[VALVE_INDEX_FILL] == null)
			{
				PlaySoundSetAtMemoryPoint(sndFill, SOUND_NAME_PIPE_SPRINKLING_START, VALVE_NAME_FILL, false, 0.0, 0.5);
				StopSoundSet(m_PipeSounds[VALVE_INDEX_FILL]);
				m_PipeSounds[VALVE_INDEX_FILL] = null;
				PlaySoundSetAtMemoryPointLooped(sndFill, SOUND_NAME_PIPE_SPRINKLING_LOOP2, VALVE_NAME_FILL, 0.5, 0.5);
				m_PipeSounds[VALVE_INDEX_FILL] = sndFill;
			}
		}

		if (fillPressureLevel < 0.4)
		{
			if (m_PipeSounds[VALVE_INDEX_FILL] != null)
			{
				StopSoundSet(m_PipeSounds[VALVE_INDEX_FILL]);
				m_PipeSounds[VALVE_INDEX_FILL] = null;
			}
		}
	}

	protected void HandleSoundEffectsUnderwaterPipeSounds()
	{
		//! strong water jet sound only when fill valve activated
		if (IsValveActive(VALVE_INDEX_FILL))
		{
			if ((m_WaterLevelActual == WL_ABOVE_PIPES) || (m_WaterLevelActual == WL_MAX && m_WaterLevelPrev == WL_AVERAGE))
			{
				if (m_PipeUnderwaterSound == null)
				{
					PlaySoundSetAtMemoryPoint(m_PipeUnderwaterSound, SOUND_NAME_UPIPE_SPRINKLING_START, PIPE_NAME_BROKEN1, false, 0.0, 0.5);
					StopSoundSet(m_PipeUnderwaterSound);
					m_PipeUnderwaterSound = null;
					m_PipeUnderwaterSoundRunning = PlaySoundSetAtMemoryPointLooped(m_PipeUnderwaterSound, SOUND_NAME_UPIPE_SPRINKLING_LOOP, PIPE_NAME_BROKEN1, 0.0, 0.5);
				}
			}
			else if (m_WaterLevelActual == m_WaterLevelPrev)
			{
				if (m_PipeUnderwaterSound)
				{
					StopSoundSet(m_PipeUnderwaterSound);
					if (m_PipeUnderwaterSoundRunning)
					{
						PlaySoundSetAtMemoryPoint(m_PipeUnderwaterSound, SOUND_NAME_UPIPE_SPRINKLING_END, PIPE_NAME_BROKEN1, false, 0.0, 0.5);
						m_PipeUnderwaterSoundRunning = false;
					}
				}
			}
		}
	}
		
	protected void HandleSoundEffectsWaterLevelMovementSounds()
	{
		if (m_WaterLevelActual < m_WaterLevelPrev)
		{
			if (m_WaterLevelMovementSound == null)
			{
				PlaySoundSetAtMemoryPointLooped(m_WaterLevelMovementSound, SOUND_NAME_WATER_DRAIN_LOOP, PIPE_NAME_BROKEN1, 0.0, 0.5);
			}
		}
		else if (m_WaterLevelActual > m_WaterLevelPrev)
		{
			if (m_WaterLevelMovementSound == null)
			{
				PlaySoundSetAtMemoryPointLooped(m_WaterLevelMovementSound, SOUND_NAME_WATER_FILL_LOOP, PIPE_NAME_BROKEN1, 0.0, 0.5);
			}	
		}
		else if (m_WaterLevelActual == m_WaterLevelPrev)
		{
			if (m_WaterLevelMovementSound)
			{
				StopSoundSet(m_WaterLevelMovementSound);
			}
		}
	}
	
	protected void CleanSoundEffects()
	{
		SEffectManager.DestroySound(m_PipeSounds[VALVE_INDEX_DRAIN]);
		SEffectManager.DestroySound(m_PipeSounds[VALVE_INDEX_FILL]);
	}
	
	protected void PlayPipeCreakingSoundOnLocation()
	{
		m_PipeCreakingSoundRequested = false;
		int index = Math.RandomInt(0, PIPE_CREAKING_SOUND_LOCATIONS_COUNT);
		PlaySoundSetAtMemoryPoint(m_PipeCreakingSounds, SOUND_NAME_PIPE_CREAKING, PIPE_CREAKING_SOUND_LOCATIONS[index]);
	}
	
	protected void HandleVisualEffects()
	{
		if (GetValvePressureLevelGauge(VALVE_INDEX_DRAIN) >= 0.4)
		{
			if (m_ValveParticles[VALVE_INDEX_DRAIN] == null)
			{
				m_ValveParticles[VALVE_INDEX_DRAIN] = ParticleManager.GetInstance().PlayInWorldEx(PARTICLE_DRAIN_PIPE_MAX_PRESSURE, null, GetMemoryPointPosition(PIPE_NAME_LEAKING_DRAIN), "0 90 165", true);
			}
		}
		
		if (GetValvePressureLevelGauge(VALVE_INDEX_DRAIN) <= 0.4)
		{
			if (m_ValveParticles[VALVE_INDEX_DRAIN] != null)
			{
				m_ValveParticles[VALVE_INDEX_DRAIN].StopParticle();
				m_ValveParticles[VALVE_INDEX_DRAIN] = null;
			}
		}
		
		if (GetValvePressureLevelGauge(VALVE_INDEX_FILL) >= 0.4)
		{
			if (m_ValveParticles[VALVE_INDEX_FILL] == null)
			{
				m_ValveParticles[VALVE_INDEX_FILL] = ParticleManager.GetInstance().PlayInWorldEx(PARTICLE_FILL_PIPE_MAX_PRESSURE, null, GetMemoryPointPosition(PIPE_NAME_LEAKING_FILL), "0 90 90", true);
			}
		}
		
		if (GetValvePressureLevelGauge(VALVE_INDEX_FILL) <= 0.4)
		{
			if (m_ValveParticles[VALVE_INDEX_FILL] != null)
			{
				m_ValveParticles[VALVE_INDEX_FILL].StopParticle();
				m_ValveParticles[VALVE_INDEX_FILL] = null;
			}
		}
		
		
		//! strong jet stream particle only when fill valve activated
		if (IsValveActive(VALVE_INDEX_FILL))
		{
			if ((m_WaterLevelActual == WL_ABOVE_PIPES) || (m_WaterLevelActual == WL_MAX && m_WaterLevelPrev == WL_AVERAGE))
			{
				if (m_PipeBrokenParticles[PIPE_INDEX_BROKEN1] == null)
				{
					m_PipeBrokenParticles[PIPE_INDEX_BROKEN1] = ParticleManager.GetInstance().PlayInWorldEx(PARTICLE_FILL_PIPE_JET, null, GetMemoryPointPosition(PIPE_NAME_BROKEN1), "0 0 0", true);
				}
				
				//! we don't need the weak jet stream
				if (m_PipeBrokenParticles[PIPE_INDEX_BROKEN2])
				{
					m_PipeBrokenParticles[PIPE_INDEX_BROKEN2].StopParticle();
					m_PipeBrokenParticles[PIPE_INDEX_BROKEN2] = null;
				}
			}
		}
		
		//! weak jet stream
		if ((m_WaterLevelActual == WL_ABOVE_PIPES && m_WaterLevelPrev == WL_AVERAGE))
		{
			if (m_PipeBrokenParticles[PIPE_INDEX_BROKEN2] == null)
			{
				m_PipeBrokenParticles[PIPE_INDEX_BROKEN2] = ParticleManager.GetInstance().PlayInWorldEx(PARTICLE_FILL_PIPE_JET_WEAK, null, GetMemoryPointPosition(PIPE_NAME_BROKEN2), "0 0 0", true);
			}
		}
		
		if (m_WaterLevelActual >= WL_AVERAGE)
		{
			for (int pipe = 0; pipe < m_PipeBrokenParticles.Count(); ++pipe)
			{
				if (m_PipeBrokenParticles[pipe])
				{
					m_PipeBrokenParticles[pipe].StopParticle();
					m_PipeBrokenParticles[pipe] = null;
				}
			}
		}
	}
	
	protected void CleanVisualEffects()
	{
		for (int valve = 0; valve < VALVES_COUNT; ++valve)
		{
			if (m_ValveParticles[valve])
			{
				m_ValveParticles[valve].StopParticle();
				m_ValveParticles[valve] = null;
			}
		}
		
		for (int pipe = 0; pipe < m_PipeBrokenParticles.Count(); ++pipe)
		{
			if (m_PipeBrokenParticles[pipe])
			{
				m_PipeBrokenParticles[pipe].StopParticle();
				m_PipeBrokenParticles[pipe] = null;
			}
		}
	}
	
	protected void LateInit()
	{
		RegisterValve(VALVE_NAME_DRAIN, VALVE_INDEX_DRAIN);
		RegisterValve(VALVE_NAME_FILL, VALVE_INDEX_FILL);

		TranslateMemoryPointsToWaterLevels();
		
		ConfigureValvesAndGaugesCourse();
		
		m_WaterLevelDefault	= m_WaterLevelsAvailable[WATER_LEVEL_AVERAGE];

		if (GetGame().IsServer())
		{
			m_SpawnedWaterObject = GetGame().CreateObjectEx(OBJECT_NAME_WATER_PLANE, m_WaterLevelDefault, ECE_CREATEPHYSICS);
			SetWaterLevelHeight(m_WaterLevelDefault[1]);
		}
		
		GetGame().RegisterNetworkStaticObject(this);
		SetSynchDirty();
	}
	
	protected void ResetState()
	{
		SetWaterLevelHeight(m_WaterLevelsAvailable[WATER_LEVEL_AVERAGE][1]);
		SetValvePressureLevelGauge(VALVE_INDEX_DRAIN, 0.0);
		SetValvePressureLevelGauge(VALVE_INDEX_FILL, 1.0);
		m_DrainValveWaterLevelStageIndex 			= 0;
		m_DrainValvePressureLevelStageIndex			= 0;
		m_FillValveWaterLevelStageIndex				= 0;
		m_FillValvePressureLevelStageIndex			= 0;
		m_ValveStates								= {0, 0};
		m_PressureAnimationRequests					= {0, 0};
		m_PressureDeanimationRequests				= {0, 0};
		m_WaterLevelTimesAccumulated				= {0, 0};
		m_PressureTimesAccumulated					= {0, 0};
		m_LastActiveValve							= -1;
		m_WaterLevelHeightActual					= WaterLevelToHeight(WL_AVERAGE);
		m_PipeCreakingSoundRequested				= false;
		m_ValveManipulatedIndex						= -1;
		m_ValveManipulatedIndexPrev					= -1;
		SetSynchDirty();
		CleanSoundEffects();
	}
	
	protected void ConfigureValvesAndGaugesCourse()
	{
		//! drain - water level
		m_DrainValveWaterStageSettings.Insert(new WaterLevelSettings(WL_AVERAGE, -1.0));
		m_DrainValveWaterStageSettings.Insert(new WaterLevelSettings(WL_MIN, 40.0));
		m_DrainValveWaterStageSettings.Insert(new WaterLevelSettings(WL_MIN, 200.0));
		m_DrainValveWaterStageSettings.Insert(new WaterLevelSettings(WL_AVERAGE, 360.0));
		
		//! drain - pressure
		m_DrainValvePressureStageSettings.Insert(new PressureLevelSettings(PL_MIN, -1.0));
		m_DrainValvePressureStageSettings.Insert(new PressureLevelSettings(PL_MAX, 40.0));
		m_DrainValvePressureStageSettings.Insert(new PressureLevelSettings(PL_MAX, 200.0));
		m_DrainValvePressureStageSettings.Insert(new PressureLevelSettings(PL_MIN, 360.0));
		
		//! for deanimation purposes only while valves activation overllaps
		m_DrainValvePressureDeanimationSettings.Insert(new PressureLevelSettings(PL_AVERAGE, -1));
		m_DrainValvePressureDeanimationSettings.Insert(new PressureLevelSettings(PL_MIN, 5.0));
		
		//! fill - water level
		m_FillValveWaterStageSettings.Insert(new WaterLevelSettings(WL_AVERAGE, -1.0));
		m_FillValveWaterStageSettings.Insert(new WaterLevelSettings(WL_MAX, 40.0));
		m_FillValveWaterStageSettings.Insert(new WaterLevelSettings(WL_MAX, 100.0));
		m_FillValveWaterStageSettings.Insert(new WaterLevelSettings(WL_AVERAGE, 40.0));

		//! fill - pressure
		m_FillValvePressureStageSettings.Insert(new PressureLevelSettings(PL_MAX, -1.0));
		m_FillValvePressureStageSettings.Insert(new PressureLevelSettings(PL_MIN, 40.0));
		m_FillValvePressureStageSettings.Insert(new PressureLevelSettings(PL_MIN, 100.0));
		m_FillValvePressureStageSettings.Insert(new PressureLevelSettings(PL_MAX, 40.0));
		
		//! for deanimation purposes only while valves activation overllaps
		m_FillValvePressureDeanimationSettings.Insert(new PressureLevelSettings(PL_AVERAGE, -1.0));
		m_FillValvePressureDeanimationSettings.Insert(new PressureLevelSettings(PL_MAX, 5.0));
	}
	
	protected void SyncValveVariables()
	{
		if (GetGame())
		{
			m_ValveStatesPacked = PackArrayOfBoolStatesIntoBits(m_ValveStates);
			if (m_ValveStatesPacked != m_ValveStatesPackedPrev)
			{
				m_ValveStatesPackedPrev = m_ValveStatesPacked;
				SetSynchDirty();
			}
		}
	}
	
	protected void Init()
	{
		m_ValveNames 							= new array<string>();
		m_ValveStates 							= new array<bool>();
		m_PressureAnimationRequests				= new array<bool>();
		m_PressureDeanimationRequests			= new array<bool>();
		m_WaterLevelTimesAccumulated			= new array<float>();
		m_PressureTimesAccumulated				= new array<float>();
		m_WaterLevelsAvailable					= new map<string, vector>();
		m_DrainValveWaterStageSettings 			= new array<ref WaterLevelSettings>();	
		m_DrainValvePressureStageSettings 		= new array<ref PressureLevelSettings>();		
		m_DrainValvePressureDeanimationSettings	= new array<ref PressureLevelSettings>();		
		m_FillValveWaterStageSettings 			= new array<ref WaterLevelSettings>();
	 	m_FillValvePressureStageSettings		= new array<ref PressureLevelSettings>();
	 	m_FillValvePressureDeanimationSettings	= new array<ref PressureLevelSettings>();
		m_WaterLevelSnapshot 					= null;
		
		m_WaterLevelActual						= WL_AVERAGE;
		m_WaterLevelPrev						= WL_AVERAGE;

		m_LastActiveValve						= INDEX_NOT_FOUND;
		m_ValveManipulatedIndex					= -1;
		m_ValveManipulatedIndexPrev				= -1;
		
		m_PipeSounds							= new array<EffectSound>();
		m_ValveParticles						= new ParticleSourceArray();
		m_PipeBrokenParticles					= new ParticleSourceArray();
		
		//! pre-init arrays
		int i = 0;
		for (i = 0; i < VALVES_COUNT; ++i)
		{
			m_ValveNames.Insert("none");
			m_ValveStates.Insert(false);
			m_PressureAnimationRequests.Insert(false);
			m_PressureDeanimationRequests.Insert(false);
			m_WaterLevelTimesAccumulated.Insert(-1);
			m_PressureTimesAccumulated.Insert(-1);
			m_PipeSounds.Insert(null);
			m_ValveParticles.Insert(null);
		}
		
		for (i = 0; i < PIPES_BROKEN_COUNT; ++i)
		{
			m_PipeBrokenParticles.Insert(null);
		}

		RegisterNetSyncVariableInt("m_ValveStatesPacked", 0);
		RegisterNetSyncVariableInt("m_ValveManipulatedIndex", -1, VALVES_COUNT - 1);
		RegisterNetSyncVariableInt("m_WaterLevelActual", WL_MIN, WL_MAX);
		RegisterNetSyncVariableInt("m_WaterLevelPrev", WL_MIN, WL_MAX);

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(LateInit, 250);
	}

	void Land_Underground_WaterReservoir()
	{
		SetEventMask(EntityEvent.POSTSIMULATE);
		
		Init();
	}
	
	void ~Land_Underground_WaterReservoir()
	{
		CleanSoundEffects();
		CleanVisualEffects();
	}
	
	protected int PackArrayOfBoolStatesIntoBits(array<bool> pStates)
	{
		int packedBits = 0;
   
    	for (int i = 0; i < pStates.Count(); ++i)
		{
			if (pStates[i] == true)
			{
        		packedBits |= 1 << i;
			}
		}
		
		return packedBits;
	}
	
	protected array<bool> UnpackBitsToArrayOfBoolStates(int pPackedBits, int pArrayLength)
	{
		array<bool> unpackedBools = new array<bool>();
		for (int i = 0; i < pArrayLength; ++i)
		{
			if ((pPackedBits & 1 << i) != 0)
			{
				unpackedBools.Insert(true);
			}
			else
			{
				unpackedBools.Insert(false);
			}
		}
		
		return unpackedBools;
	}
	
#ifdef DEVELOPER
	override protected string GetDebugText()
	{
		string debug_output = "";
		debug_output += string.Format("\nm_WaterLevelActual: %1", m_WaterLevelActual);
		debug_output += string.Format("\nm_WaterLevelPrev: %1", m_WaterLevelPrev);
		debug_output += string.Format("\nm_ValveManipulatedIndex: %1", m_ValveManipulatedIndex);

		int valve;
		for (valve = 0; valve < VALVES_COUNT; ++valve)
		{
			debug_output += string.Format("\nvalve %1 state:%2 | pressure req: %2", valve, m_ValveStates[valve], m_PressureAnimationRequests[valve]);
		}

		return debug_output;	
	}

	override void GetDebugButtonNames(out string button1, out string button2, out string button3, out string button4)
	{
		button1 = "Reset state";
		button2 = "Drain";
		button3 = "Fill";
	}
	
	override void OnDebugButtonPressServer(int button_index)
	{
		switch (button_index)
		{
		case 1:
			ResetState();
			ActualWaterLevelStageSettings(VALVE_INDEX_DRAIN);
			PreviousWaterLevelStageSettings(VALVE_INDEX_DRAIN);
			ActualWaterLevelStageSettings(VALVE_INDEX_FILL);
			PreviousWaterLevelStageSettings(VALVE_INDEX_FILL);
		break;
		case 2:
			ResetState();
			m_ValveStates[VALVE_INDEX_DRAIN]				= 1;
			m_PressureAnimationRequests[VALVE_INDEX_DRAIN]	= 1;
			m_DrainValveWaterLevelStageIndex 				= 1;
			m_DrainValvePressureLevelStageIndex 			= 1;
			SetLastActiveValve(VALVE_INDEX_DRAIN);
			SetSynchDirty();
		break;
		case 3:
			ResetState();
			m_ValveStates[VALVE_INDEX_FILL]					= 1;
			m_PressureAnimationRequests[VALVE_INDEX_FILL] 	= 1;
			m_FillValveWaterLevelStageIndex 				= 1;
			m_FillValvePressureLevelStageIndex 				= 1;
			SetLastActiveValve(VALVE_INDEX_FILL);
			SetSynchDirty();
		break;
		}
	}
#endif
}

class Land_Underground_WaterReservoir_Water : BuildingBase {}
