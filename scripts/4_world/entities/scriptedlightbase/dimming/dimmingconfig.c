class DimmingConfig
{
	//randomization
	float m_DimBrigthnessMin = 0.1;
	float m_DimBrigthnessMax = 1;
	float m_DimSpeedMin = 0.1;
	float m_DimSpeedMax = 1;
	float m_DimBrigthnessDurationMin = 0.1;
	float m_DimBrigthnessDurationMax = 1;
	float m_BrightenSpeedMin = 0.1;
	float m_BrightenSpeedMax = 1;
	float m_DimBrigthnessPauseMin = 1;
	float m_DimBrigthnessPauseMax = 4;
	
	// patterns
	bool 	m_UsePatterns;
	int 	m_PatternRepeatCount = -1; // -1 = infinite
	int 	m_CurrentPatternID;
	int		m_PatternCount;
	int		m_CurrentEntryID;
	ref array<ref array<int>> 	m_BrightnessPatterns = new array<ref array<int>>();
	ref array<ref array<float>>	m_DimSpeedPatterns = new array<ref array<float>>();
	ref array<ref array<float>>	m_DurationPatterns = new array<ref array<float>>();
	ref array<ref array<float>> m_BrightenSpeedPatterns = new array<ref array<float>>();
	ref array<ref array<float>> m_PausePatterns = new array<ref array<float>>();
	
	// Percentage of base brightness, how strong can the dim be 
	void SetRandomBrigthnessLimits(float min, float max)
	{
		min = Math.Clamp(min, 0, 100);
		max = Math.Clamp(max, 0, 100);
		
		m_DimBrigthnessMin = min * 0.01;
		m_DimBrigthnessMax = max * 0.01;
	}
	
	// Speed limits of of random dimming
	void SetRandomDimSpeedLimits(float min, float max)
	{
		min = Math.Clamp(min, 0.01, 10);
		max = Math.Clamp(max, 0.01, 10);
		
		m_DimSpeedMin = min;
		m_DimSpeedMax = max;
	}
	
	// Duration of paused dimmed state
	void SetRandomDurationLimits(float min, float max)
	{
		min = Math.Clamp(min, 0.01, 100);
		max = Math.Clamp(max, 0.01, 100);
		
		m_DimBrigthnessDurationMin = min;
		m_DimBrigthnessDurationMax = max;
	}
	
	// Speed limits of of random brightening
	void SetRandomBrightenSpeedLimits(float min, float max)
	{
		min = Math.Clamp(min, 0.01, 10);
		max = Math.Clamp(max, 0.01, 10);
		
		m_BrightenSpeedMin = min;
		m_BrightenSpeedMax = max;
	}
	
	// Duration of paused state without dimming in seconds
	void SetRandomPauseDurationLimits(float min, float max)
	{
		min = Math.Clamp(min, 0.01, 100);
		max = Math.Clamp(max, 0.01, 100);
		
		m_DimBrigthnessPauseMin = min;
		m_DimBrigthnessPauseMax = max;
	}
	
	// Adding a pattern will disable randomized dimming
	void AddDimmingPattern(array<int> brightnessTarget, array<float> dimSpeed, array<float> dimDuration, array<float> brightenSpeed, array<float> pause)
	{
		int count = brightnessTarget.Count();
		if ( count != dimSpeed.Count() || count != dimDuration.Count() || count != brightenSpeed.Count() || count != pause.Count())
		{
			ErrorEx(this.ToString() + ": Invalid dimming pattern");
			return;
		}
		
		m_UsePatterns = true;
				
		m_BrightnessPatterns.Insert(brightnessTarget);
		m_DimSpeedPatterns.Insert(dimSpeed);
		m_DurationPatterns.Insert(dimDuration);
		m_BrightenSpeedPatterns.Insert(brightenSpeed);
		m_PausePatterns.Insert(pause);
		
		m_PatternCount++;
	}
	
	// Set how many times should the patterns repeat before stopping, default -1 = infinite repeat
	void SetPatternQueueRepeat(int val)
	{
		m_PatternRepeatCount = val;
	}
}