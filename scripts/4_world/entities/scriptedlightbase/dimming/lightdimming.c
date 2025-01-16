enum ELightDimmingState
{
	PAUSED,
	DIMMING,
	PAUSED_DIMMED,
	BRIGHTENING
}

/*	Enable and configure light dimming

	Example setup in ScriptedLightBase constructor:
	
	DimmingConfig dimCfg = new DimmingConfig();	
	// randomized
	dimCfg.SetRandomBrigthnessLimits(10, 50);
	dimCfg.SetRandomDimSpeedLimits(0.1, 1);
	dimCfg.SetRandomDurationLimits(0.2, 1);
	dimCfg.SetRandomBrightenSpeedLimits(0.1, 1);
	dimCfg.SetRandomPauseDurationLimits(1, 2);
	
	// custom patterns 
	float f = LightDimming.FLICKER_TIME;
	dimCfg.AddDimmingPattern( {0, 0, 50}, {f, f, 0.1}, {f, f, 2}, {f, f, 0.1}, {0.1, 0.1, 4});
	dimCfg.AddDimmingPattern( {0, 50, 0}, {f, 0.1, f}, {f, 1, f}, {f, 0.1, f}, {0.1, 0.1, 3});
	dimCfg.SetPatternQueueRepeat(2);

	EnableDimming(6.5, dimCfg);
*/
class LightDimming
{
	const float FLICKER_TIME = 0.05; // duration of a flicker -> when this speed is set in phase, DIM / BRIGHTEN phases are skipped, producing a quick flicker
	
	protected float m_DimBrigthnessBase; 
	protected float m_DimBrigthnessSpeedCoef; 	// interp speed multiplier
	protected float m_DimBrigthnessTarget;		// brightness value we want to dim to
	protected float m_DimDelta;					// delta used for timing states
	
	protected int 	m_CurrentPatternID;
	protected int 	m_CurrentEntryID;
	
	protected bool m_ToBeDestroyed;
	protected ELightDimmingState m_DimState;
	protected ScriptedLightBase m_Light;
	protected DimmingConfig m_ActiveCfg;
	protected ref array<ref DimmingConfig> m_Configs = new array<ref DimmingConfig>();
	
	void LightDimming(ScriptedLightBase light, float baseBrightness, DimmingConfig dimCfg)
	{
		m_Light = light;
		m_DimBrigthnessBase = baseBrightness;
		m_Configs.Insert(dimCfg);
		m_ActiveCfg = dimCfg;
	}
	
	ELightDimmingState GetState()
	{
		return m_DimState;
	}
	
	// load additional config for dimming to allow switches between multiple configurations
	void AddConfig(DimmingConfig cfg)
	{
		m_Configs.Insert(cfg);
	}
			
	protected void SetDimmingVal(float val)
	{
		m_Light.SetBrightnessTo(val);
	}
	
	protected void AdvanceState(ELightDimmingState lastState)
	{
		if (lastState == ELightDimmingState.PAUSED)
		{
			m_DimState = ELightDimmingState.DIMMING;
			
			if (m_ActiveCfg.m_UsePatterns)
			{
				SelectPattern();
				if (m_ToBeDestroyed)
					return;
				
				m_DimBrigthnessTarget = m_ActiveCfg.m_BrightnessPatterns[m_CurrentPatternID][m_CurrentEntryID] * 0.01 * m_DimBrigthnessBase;
				
				m_DimDelta = m_ActiveCfg.m_DimSpeedPatterns[m_CurrentPatternID][m_CurrentEntryID];
				if (m_DimDelta == FLICKER_TIME) // flicker, skip dim interp
				{
					AdvanceState(ELightDimmingState.DIMMING);
					return;
				}
			}
			else 
			{
				m_DimBrigthnessTarget = Math.RandomFloat(m_ActiveCfg.m_DimBrigthnessMin * m_DimBrigthnessBase, m_ActiveCfg.m_DimBrigthnessMax * m_DimBrigthnessBase);
				m_DimDelta = Math.RandomFloat(m_ActiveCfg.m_DimSpeedMin, m_ActiveCfg.m_DimSpeedMax);
			}
			
			m_DimBrigthnessSpeedCoef = 1/m_DimDelta;
		}
		else if (lastState == ELightDimmingState.DIMMING)
		{
			m_DimState = ELightDimmingState.PAUSED_DIMMED;
			if (m_ActiveCfg.m_UsePatterns)
				m_DimDelta = m_ActiveCfg.m_DurationPatterns[m_CurrentPatternID][m_CurrentEntryID];
			else
				m_DimDelta = Math.RandomFloat(m_ActiveCfg.m_DimBrigthnessDurationMin,m_ActiveCfg. m_DimBrigthnessDurationMax);
			
			SetDimmingVal(m_DimBrigthnessTarget);
			
		}
		else if (lastState == ELightDimmingState.PAUSED_DIMMED)
		{
			m_DimState = ELightDimmingState.BRIGHTENING;
			
			if (m_ActiveCfg.m_UsePatterns)
			{
				m_DimDelta = m_ActiveCfg.m_BrightenSpeedPatterns[m_CurrentPatternID][m_CurrentEntryID];
				if (m_DimDelta == FLICKER_TIME) // flicker, skip brighten interp
				{
					AdvanceState(ELightDimmingState.BRIGHTENING);
					return;
				}
			}
			else 
				m_DimDelta = Math.RandomFloat(m_ActiveCfg.m_BrightenSpeedMin, m_ActiveCfg.m_BrightenSpeedMax);

			m_DimBrigthnessSpeedCoef = 1/m_DimDelta;
		}
		else if (lastState == ELightDimmingState.BRIGHTENING)
		{
			m_DimState = ELightDimmingState.PAUSED;
			
			if (m_ActiveCfg.m_UsePatterns)
			{
				m_DimDelta = m_ActiveCfg.m_PausePatterns[m_CurrentPatternID][m_CurrentEntryID];
				m_CurrentEntryID++;
			}
			else
				m_DimDelta = Math.RandomFloat(m_ActiveCfg.m_DimBrigthnessPauseMin, m_ActiveCfg.m_DimBrigthnessPauseMax);
			
			SetDimmingVal(m_DimBrigthnessBase);
		}
	}
	
	protected void SelectPattern()
	{
		if (m_CurrentEntryID != m_ActiveCfg.m_BrightnessPatterns[m_CurrentPatternID].Count())
			return; // pattern in progress
		
		m_CurrentPatternID++;
		m_CurrentEntryID = 0;
				
		if (m_CurrentPatternID == m_ActiveCfg.m_PatternCount)
		{
			if (m_ActiveCfg.m_PatternRepeatCount == -1)
				m_CurrentPatternID = 0;
			else if (m_ActiveCfg.m_PatternRepeatCount == 0)
			{
				m_ToBeDestroyed = true;
				m_Light.StopDimming();
			}
			else 
			{
				m_ActiveCfg.m_PatternRepeatCount--;
				m_CurrentPatternID = 0;
			}
		}
	}
	
	void SwapConfig(int index)
	{
		if (!m_Configs.IsValidIndex(index))
			return;
		
		m_ActiveCfg = m_Configs[index];
		
		m_CurrentPatternID = 0;
		m_CurrentEntryID = 0;
		m_DimState = ELightDimmingState.PAUSED;
	}
		
	void HandleDimming(float timeSlice)
	{
		if (m_ToBeDestroyed)
			return;
		
		m_DimDelta -= timeSlice;
		if (m_DimDelta < 0)
		{
			AdvanceState(m_DimState);
			return;
		}
		
		if (m_DimState == ELightDimmingState.DIMMING || m_DimState == ELightDimmingState.BRIGHTENING)
		{
			if (m_DimState == ELightDimmingState.DIMMING)
				SetDimmingVal(Math.Lerp(m_DimBrigthnessTarget, m_DimBrigthnessBase, m_DimDelta * m_DimBrigthnessSpeedCoef));
			else 
				SetDimmingVal(Math.Lerp(m_DimBrigthnessBase, m_DimBrigthnessTarget, m_DimDelta * m_DimBrigthnessSpeedCoef));
		}
	}
}