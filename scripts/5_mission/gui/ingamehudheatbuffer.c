//#define HEATBUFFER_INDICATOR_DEBUG // Uncomment for heat buffer indicator debug logs
class IngameHudHeatBuffer
{
	protected Widget m_Root;

	protected float	m_CurrentValue;
	protected bool m_IsActive;
	protected int m_CurrentDirection; // -1 = none | 0 = decreasing | 1 = increasing
	protected int m_PreviousDirection; // -1 = none | 0 = decreasing | 1 = increasing
	protected bool m_Update;
	protected PlayerBase m_Player;
	protected float m_EffectTime;
	protected float m_EffectDuration;
	protected bool m_FadeIn;
	protected float m_FlashingTime;
	protected int m_FlashingStage;
	protected ref map<int, float> m_FlashingThresholds;
	
	void IngameHudHeatBuffer(Widget root, notnull PlayerBase player)
	{
		m_Root = root;
		m_Player = player;
		
		player.GetOnDeathStart().Insert(OnPlayerNegativeCondition);
		player.GetOnUnconsciousStart().Insert(OnPlayerNegativeCondition);
		player.GetOnUnconsciousStop().Insert(OnPlayerConditionChanged);

		m_CurrentDirection = -1;
		m_PreviousDirection = -1;
		m_Update = true;
		m_FlashingStage = -1;
		
		m_FlashingThresholds = new map<int, float>;
		m_FlashingThresholds.Insert(1, 0.002);
		m_FlashingThresholds.Insert(2, 0.332);
		m_FlashingThresholds.Insert(3, 0.662);
	}
	
	void OnPlayerNegativeCondition(PlayerBase player)
	{
		#ifdef HEATBUFFER_INDICATOR_DEBUG
		Print("IngameHudHeatBuffer::OnPlayerNegativeCondition");
		#endif

		if (player == m_Player)
			m_Update = false;
	}
	
	void OnPlayerConditionChanged(PlayerBase player)
	{
		#ifdef HEATBUFFER_INDICATOR_DEBUG
		Print("IngameHudHeatBuffer::OnPlayerConditionChanged");
		#endif	
		
		if (player == m_Player)
			m_Update = true;
	}
	
	bool CanUpdate()
	{
		return m_Player && m_Update;
	}
	
	void Update(float timeslice)
	{
		int heatBufferStage = m_Player.GetHeatBufferStage();
		float heatBufferVal = m_Player.GetStatHeatBuffer().Get();
		float heatBufferDynMax = m_Player.GetHeatBufferDynamicMax();
		float heatBufferPercent = heatBufferVal / m_Player.GetStatHeatBuffer().GetMax();
		float heatBufferDiff = heatBufferVal - m_CurrentValue;
		m_CurrentValue = heatBufferVal;
		
		#ifdef HEATBUFFER_INDICATOR_DEBUG
		Print("-----------------------------------------------------------------------");
		Print("Buff stage=" + heatBufferStage);
		Print("Buff value=" + heatBufferVal);
		Print("Buff percent=" + heatBufferPercent);
		Print("Buff dynamic max=" + heatBufferDynMax);
		Print("Effect active=" + m_IsActive);
		Print("Last buff value=" + m_CurrentValue);
		Print("Buff differance=" + heatBufferDiff);
		Print("Prev Heat buffer direction=" + m_PreviousDirection);
		Print("Flashing time=" + m_FlashingTime);
		Print("Flashing stage=" + m_FlashingStage);
		#endif
	
		for (int i = 1; i < HeatBufferMdfr.NUMBER_OF_STAGES; ++i)
		{
			Widget hbw = m_Root.FindAnyWidget("HeatBuffer" + i);
			if (!hbw)
				continue;

			float stageThreshold = HeatBufferMdfr.STAGE_THRESHOLDS[i];
			#ifdef HEATBUFFER_INDICATOR_DEBUG
			Print("Stage treshold=" + stageThreshold);
			#endif
		
			// Determine heat buffer direction. 
			// When heat buffer percentage value is higher the the heat buffer dynamic max value we prevent increasing for the indactor logic.
			// Heat buffer Dynamic max value determines the max value the character can currently reach with his current heat insolation
			if (heatBufferDiff > 0)
			{
				// If heat buffer percentage value is below or at current heat buffer dynamic max range value the direction cant change
				if (heatBufferPercent < heatBufferDynMax)
				{
					m_CurrentDirection = 1;
				}
				else
				{
					#ifdef HEATBUFFER_INDICATOR_DEBUG
					Print("HEAT BUFFER - DYNAMIC MAX REACHED - DONT CHANGE DIRECTION");
					#endif
					m_CurrentDirection = -1;
				}
      		}
			else if (heatBufferDiff < 0)
			{
				m_CurrentDirection = 0;
			}
		
			#ifdef HEATBUFFER_INDICATOR_DEBUG
			Print("Heat buffer direction=" + m_CurrentDirection);
			#endif
		
			// Hide visibility of flashing stages
			if (heatBufferStage < i && m_FlashingStage != i || m_FlashingStage == i && m_FlashingTime >= 2.9)
			{
				if (m_FlashingStage == i)
				{
					m_IsActive = false;
					m_FlashingTime = 0;
					m_FlashingStage = -1;
				}
				
				hbw.SetAlpha(0);
				hbw.Show(false);
			}
			
			// Handle widget visibility and alpha based on stage and buffer percent
			if (heatBufferStage >= i)
			{
				hbw.Show(true);
			
				if (heatBufferPercent < stageThreshold)
				{
					if (m_CurrentDirection == 1)
					{
						#ifdef HEATBUFFER_INDICATOR_DEBUG
						Print("HEAT BUFFER - STAGE " + i + " - INCREASING");
						hbw.SetColor(ARGB(hbw.GetAlpha() * 255, 220, 220, 0)); // COLOR SET ON INDICATOR IS ONLY HERE FOR DEBUG TESTING FOR NOW
						#endif
						
						SetBaseAlpha(hbw, heatBufferPercent, stageThreshold);
						
						if (m_PreviousDirection == 0 && m_IsActive)
						{
							m_IsActive = false;
							m_FlashingTime = 0;
							m_FlashingStage = -1;
						}

			    		UpdateEffect(hbw, 2.0, timeslice);
					}
					else if (m_CurrentDirection == 0)
					{
						#ifdef HEATBUFFER_IND	ICATOR_DEBUG
						Print("HEAT BUFFER - STAGE " + i + " - DECREASING");
						#endif
						
						SetBaseAlpha(hbw, heatBufferPercent, stageThreshold);

						if (m_PreviousDirection == 1 && m_IsActive)
						{
							m_IsActive = false;
						}

						float flashingThreshold = m_FlashingThresholds.Get(i);
						if (heatBufferDynMax < 0.5)
						{
							flashingThreshold = flashingThreshold + 0.002;
						}
						
						#ifdef HEATBUFFER_INDICATOR_DEBUG
						Print("HEAT BUFFER - STAGE " + i + " - DECREASING - Flashing threshold=" + flashingThreshold);
						hbw.SetColor(ARGB(hbw.GetAlpha() * 255, 0, 206, 209)); // COLOR SET ON INDICATOR IS ONLY HERE FOR DEBUG TESTING FOR NOW
						#endif

						if (heatBufferPercent <= flashingThreshold)
						{
							#ifdef HEATBUFFER_INDICATOR_DEBUG
							Print("HEAT BUFFER - STAGE " + i + " - FLASHING");
							#endif
							
							#ifdef HEATBUFFER_INDICATOR_DEBUG
							hbw.SetColor(ARGB(hbw.GetAlpha() * 255, 255, 0, 0)); // COLOR SET ON INDICATOR IS ONLY HERE FOR DEBUG TESTING FOR NOW
							#endif

							UpdateEffect(hbw, 0.25, timeslice);
							
							m_FlashingTime += timeslice;
							if (m_FlashingTime >= 2.9)
							{
								m_IsActive = false;
								m_FlashingTime = 0;
								m_FlashingStage = -1;
								hbw.SetAlpha(0);
			   					hbw.Show(false);
							}
						}
					}
					else if (m_CurrentDirection == -1)
					{
						#ifdef HEATBUFFER_INDICATOR_DEBUG
						Print("HEAT BUFFER - STAGE " + i + " - STAL");
						#endif	
						
						SetBaseAlpha(hbw, heatBufferPercent, stageThreshold);
					}
				}
				else
				{
					#ifdef HEATBUFFER_INDICATOR_DEBUG
					Print("HEAT BUFFER - STAGE " + i + " - MAXED");
					#endif
				
					hbw.SetAlpha(1.0);				
					#ifdef HEATBUFFER_INDICATOR_DEBUG
					hbw.SetColor(ARGB(hbw.GetAlpha() * 255, 220, 220, 220)); // COLOR SET ON INDICATOR IS ONLY HERE FOR DEBUG TESTING FOR NOW
					#endif
				}
			}
   		}
		
		if (m_PreviousDirection != m_CurrentDirection)
			m_PreviousDirection = m_CurrentDirection;
	}

	// Function that calculates and sets the base alpha value for the current heat buffer widget depending on the given heat buffer percentage value and stage threshold 
	void SetBaseAlpha(Widget hbw, float valuePercent, float stageThreshold)
	{
		float baseAlpha = Math.Lerp(0.05, 1.00, (valuePercent / stageThreshold));
		baseAlpha = Math.Floor(baseAlpha * 100) / 100;

		#ifdef HEATBUFFER_INDICATOR_DEBUG
		Print("HEAT BUFFER - Set base alpha=" + baseAlpha + " | Widget=" + hbw.GetName());
		#endif

	    hbw.SetAlpha(baseAlpha); // Set the current alpha to the base alpha
	}
	
	void UpdateEffect(Widget hbw, float duration, float timeslice)
	{
		#ifdef HEATBUFFER_INDICATOR_DEBUG
	    Print("HEAT BUFFER - EFFECT - Widget=" + hbw.GetName());
		#endif
	
	    float baseAlpha = hbw.GetAlpha();
	    float opacity;

	    // Initialize the effect if it's not already active
	    if (!m_IsActive)
	    {
	        m_EffectDuration = duration;
	        m_EffectTime = 0;
	        m_FadeIn = true;
	        m_IsActive = true;
	    }
	
	    // Update the effect time
	   	m_EffectTime += timeslice;
	
	    // Calculate the time fraction
	    float timeFraction = m_EffectTime / m_EffectDuration;
	
	    // Calculate opacity
	    if (m_FadeIn)
	    {
	        opacity = Math.Lerp(0.0, 1.0, timeFraction);
	        if (timeFraction >= 1.0)
	        {
	            m_FadeIn = false;
	            m_EffectTime = 0; // Reset for fade-out
	        }
	    }
	    else
	    {
	        opacity = Math.Lerp(1.0, 0.0, timeFraction);
	        if (timeFraction >= 1.0)
	        {
	            m_FadeIn = true;
	            m_EffectTime = 0; // Reset for fade-in
	        }
	    }
		
		// Clamp the opacity to ensure it's within the valid range
	    opacity = Math.Clamp(opacity, 0.0, 1.0);
	
	    // Set the widget's alpha (opacity)
	    hbw.SetAlpha(opacity);
	
	    // Debug print statements
		#ifdef HEATBUFFER_INDICATOR_DEBUG
	    Print("HEAT BUFFER - EFFECT - Opacity=" + opacity + " | Time Fraction=" + timeFraction + " | FadeIn=" + m_FadeIn + " | Effect time=" + m_EffectTime + " | Effect duration=" + m_EffectDuration);
		#endif
	}
}
