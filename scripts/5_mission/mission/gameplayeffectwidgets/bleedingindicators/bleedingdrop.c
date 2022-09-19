
class BleedingIndicatorDropData
{
	protected ImageWidget 	m_Widget;
	protected int 			m_Severity;
	protected float 		m_TimeTotal;
	protected float 		m_ProgressBreakpointTime;
	protected float 		m_ProgressFadingDuration; //remaining duration AFTER breakpoint
	protected float 		m_ProgressBreakpoint;
	protected float 		m_Duration;
	protected float 		m_SpeedCoef;
	protected int 			m_ScatterPx;
	protected float 		m_SlideDistance;
	
	protected float 		m_ColorAlphaStart;
	protected float 		m_ColorAlphaEnd;
	protected float 		m_ColorAlphaCurrent;
	
	protected float 		m_ImageBaseSizeX;
	protected float 		m_ImageBaseSizeY;
	protected float 		m_ImageStartingSizeX; //adjusted by percentage
	protected float 		m_ImageStartingSizeY; //adjusted by percentage
	protected float 		m_ImageEndSizeX; //adjusted by percentage
	protected float 		m_ImageEndSizeY; //adjusted by percentage
	protected float 		m_ImageMaxSizeX;
	protected float 		m_ImageMaxSizeY;
	protected float 		m_ImageBaseRotation;
	protected bool 			m_IsRunning;
	protected vector 		m_BasePosition;
	protected int 			m_ScreenSizeX;
	protected int 			m_ScreenSizeY;
	protected float 		m_PosX, m_PosY;
	protected float 		m_StartSizeCoef;
	protected float 		m_EndSizeCoef;
	protected float 		m_RandomSizeMin;
	protected float 		m_RandomSizeMax;
	
	//Written with relative positioning in mind
	void BleedingIndicatorDropData(ImageWidget image, int severity)
	{
		m_Widget = image;
		m_Severity = severity;
		m_TimeTotal = 0;
		m_IsRunning = false;
		
#ifdef DIAG_DEVELOPER
		if (DbgBleedingIndicationStaticInfo.m_DbgUseOverrideValues)
		{
			m_Duration = DbgBleedingIndicationStaticInfo.m_DbgDropDurationBase;
			m_ProgressBreakpointTime = m_Duration * DbgBleedingIndicationStaticInfo.m_DbgDropProgressTreshold;
			m_ProgressBreakpoint = DbgBleedingIndicationStaticInfo.m_DbgDropProgressTreshold;
			m_StartSizeCoef = DbgBleedingIndicationStaticInfo.m_DbgDropStartSize;
			m_EndSizeCoef = DbgBleedingIndicationStaticInfo.m_DbgDropEndSize;
			m_RandomSizeMin = DbgBleedingIndicationStaticInfo.m_DbgDropSizeVariationMin;
			m_RandomSizeMax = DbgBleedingIndicationStaticInfo.m_DbgDropSizeVariationMax;
			m_ScatterPx = DbgBleedingIndicationStaticInfo.m_DbgDropScatter;
			m_SlideDistance = DbgBleedingIndicationStaticInfo.m_DbgDropSlideDistance;
		}
		else
#endif
		{
			switch (m_Severity)
			{
				case BleedingIndicationConstants.INDICATOR_SEVERITY_LOW:
				{
					m_Duration = BleedingIndicationConstants.DROP_DURATION_LOW;
					m_StartSizeCoef = BleedingIndicationConstants.DROP_SIZE_START_LOW;
					m_EndSizeCoef = BleedingIndicationConstants.DROP_SIZE_END_LOW;
					m_RandomSizeMin = BleedingIndicationConstants.DROP_SIZE_VARIATION_MIN_LOW;
					m_RandomSizeMax = BleedingIndicationConstants.DROP_SIZE_VARIATION_MAX_LOW;
					m_ScatterPx = BleedingIndicationConstants.DROP_SCATTER_LOW;
					m_SlideDistance = BleedingIndicationConstants.DROP_SLIDE_DISTANCE_LOW;
					break;
				}
				case BleedingIndicationConstants.INDICATOR_SEVERITY_MEDIUM:
				{
					m_Duration = BleedingIndicationConstants.DROP_DURATION_MEDIUM;
					m_StartSizeCoef = BleedingIndicationConstants.DROP_SIZE_START_MEDIUM;
					m_EndSizeCoef = BleedingIndicationConstants.DROP_SIZE_END_MEDIUM;
					m_RandomSizeMin = BleedingIndicationConstants.DROP_SIZE_VARIATION_MIN_MEDIUM;
					m_RandomSizeMax = BleedingIndicationConstants.DROP_SIZE_VARIATION_MAX_MEDIUM;
					m_ScatterPx = BleedingIndicationConstants.DROP_SCATTER_MEDIUM;
					m_SlideDistance = BleedingIndicationConstants.DROP_SLIDE_DISTANCE_MEDIUM;
					break;
				}
				case BleedingIndicationConstants.INDICATOR_SEVERITY_HIGH:
				{
					m_Duration = BleedingIndicationConstants.DROP_DURATION_HIGH;
					m_StartSizeCoef = BleedingIndicationConstants.DROP_SIZE_START_HIGH;
					m_EndSizeCoef = BleedingIndicationConstants.DROP_SIZE_END_HIGH;
					m_RandomSizeMin = BleedingIndicationConstants.DROP_SIZE_VARIATION_MIN_HIGH;
					m_RandomSizeMax = BleedingIndicationConstants.DROP_SIZE_VARIATION_MAX_HIGH;
					m_ScatterPx = BleedingIndicationConstants.DROP_SCATTER_HIGH;
					m_SlideDistance = BleedingIndicationConstants.DROP_SLIDE_DISTANCE_HIGH;
					break;
				}
			}
			
			m_ProgressBreakpointTime = m_Duration * BleedingIndicationConstants.DROP_PROGRESS_THRESHOLD;
			m_ProgressBreakpoint = BleedingIndicationConstants.DROP_PROGRESS_THRESHOLD;
		}
		m_ProgressFadingDuration = m_Duration - m_ProgressBreakpointTime;
		
		m_SpeedCoef = 1.0; //TODO ??
#ifdef DIAG_DEVELOPER
		if (DbgBleedingIndicationStaticInfo.m_DbgUseOverrideValues)
		{
			m_ColorAlphaStart = DbgBleedingIndicationStaticInfo.m_DbgDropColorAlphaStart / 255;
			m_ColorAlphaEnd = DbgBleedingIndicationStaticInfo.m_DbgDropColorAlphaEnd / 255;
		}
		else
#endif
		{
			m_ColorAlphaStart = BleedingIndicationConstants.DROP_COLOR_ALPHA_START / 255;
			m_ColorAlphaEnd = BleedingIndicationConstants.DROP_COLOR_ALPHA_END / 255;
		}
		InitImageScale();
		GetScreenSize(m_ScreenSizeX,m_ScreenSizeY);
		AdjustColorSaturation();
	}
	
	void ~BleedingIndicatorDropData()
	{
	}
	
	protected void InitImageScale()
	{
		m_Widget.GetSize(m_ImageBaseSizeX,m_ImageBaseSizeY);
		float randomScaleCoef =  Math.RandomFloatInclusive(m_RandomSizeMin,m_RandomSizeMax);
		
		m_ImageStartingSizeX = m_ImageBaseSizeX * m_StartSizeCoef * randomScaleCoef;
		m_ImageEndSizeX = m_ImageBaseSizeX * m_EndSizeCoef * randomScaleCoef;
		m_ImageStartingSizeY = m_ImageBaseSizeY * m_StartSizeCoef * randomScaleCoef;
		m_ImageEndSizeY = m_ImageBaseSizeY * m_EndSizeCoef * randomScaleCoef;
	}
	
	void ScatterPosition(vector pos)
	{
#ifdef DIAG_DEVELOPER
		if (DbgBleedingIndicationStaticInfo.m_DbgUseOverrideValues)
		{
			m_ScatterPx = DbgBleedingIndicationStaticInfo.m_DbgDropScatter;
		}
#endif
		float rndRadius = Math.RandomFloatInclusive(0.0,m_ScatterPx);
		float rndPos = Math.RandomFloatInclusive(0.0,Math.PI2);
		m_PosX = pos[0];
		m_PosX = m_PosX + rndRadius * Math.Sin(rndPos);
		
		m_PosY = pos[1];
		m_PosY = m_PosY + rndRadius * Math.Cos(rndPos);
	}
	
	void StartDrop()
	{
		m_TimeTotal = 0;
		
		ScatterPosition(m_BasePosition);
		m_Widget.SetPos(m_PosX,m_PosY);
#ifdef DIAG_DEVELOPER
		if (DbgBleedingIndicationStaticInfo.m_DbgUseOverrideValues && !DbgBleedingIndicationStaticInfo.m_DbgDropRotationRandom)
		{
			m_Widget.SetRotation(0,0,0);
		}
		else
#endif
		{
			m_Widget.SetRotation(0,0,Math.RandomFloatInclusive(0.0,360.0));
		}
		
		m_Widget.Show(true);
		m_IsRunning = true;
	}
	
	void StopDrop()
	{
		m_IsRunning = false;
		m_Widget.SetSize(m_ImageBaseSizeX,m_ImageBaseSizeY); //resets image size
		m_Widget.Show(false);
	}
	
	void SetBasePosition(vector pos)
	{
		m_BasePosition = pos;
		m_BasePosition[0] = m_BasePosition[0] - m_ScreenSizeX/2;
		m_BasePosition[1] = m_BasePosition[1] - m_ScreenSizeY/2;
	}
	
	bool IsRunning()
	{
		return m_IsRunning;
	}
	
	ImageWidget GetImage()
	{
		return m_Widget;
	}
	
	void AdjustColorSaturation()
	{
		//color adjustment
		float r = BleedingIndicationConstants.DROP_COLOR_RED;
		float g = BleedingIndicationConstants.DROP_COLOR_GREEN;
		float b = BleedingIndicationConstants.DROP_COLOR_BLUE;
		float desaturationEnd = BleedingIndicationConstants.DROP_COLOR_DESATURATIONEND;
#ifdef DIAG_DEVELOPER
		if (DbgBleedingIndicationStaticInfo.m_DbgUseOverrideValues)
		{
			r = DbgBleedingIndicationStaticInfo.m_DbgDropColorRed;
			g = DbgBleedingIndicationStaticInfo.m_DbgDropColorGreen;
			b = DbgBleedingIndicationStaticInfo.m_DbgDropColorBlue;
			desaturationEnd = DbgBleedingIndicationStaticInfo.m_DbgDesaturationEnd;
		}
#endif
		
		//saturation adjustment
#ifdef DIAG_DEVELOPER
		if (!DbgBleedingIndicationStaticInfo.m_DbgUseOverrideValues || (DbgBleedingIndicationStaticInfo.m_DbgUseOverrideValues && DbgBleedingIndicationStaticInfo.m_DbgDropDesaturate) )
		{
#endif
			Param par = PPEManagerStatic.GetPPEManager().GetPostProcessCurrentValues(PostProcessEffectType.Glow,PPEGlow.PARAM_SATURATION);
			float saturationProgress = Param1<float>.Cast(par).param1;
			saturationProgress = Easing.EaseOutSine(saturationProgress);
			saturationProgress = Math.Lerp(desaturationEnd,1.0,saturationProgress);
			float lowest_channel = Math.Min(Math.Min(r,g),b);
			r = Math.Lerp(lowest_channel,r,saturationProgress);
			g = Math.Lerp(lowest_channel,g,saturationProgress);
			b = Math.Lerp(lowest_channel,b,saturationProgress);
#ifdef DIAG_DEVELOPER
		}
#endif
		
		int color = ARGB(0x00,r,g,b);
		m_Widget.SetColor(color);
	}
	
	void UpdateAlpha(float progress,float progressFade)
	{
		if (progress <= m_ProgressBreakpoint)
		{
			m_ColorAlphaCurrent = m_ColorAlphaStart;
		}
		else
		{
			m_ColorAlphaCurrent = Math.Lerp(m_ColorAlphaStart,m_ColorAlphaEnd,progressFade);
		}
		
		m_Widget.SetAlpha(m_ColorAlphaCurrent);
	}
	
	//! scaling and transformation
	void UpdateTransform(float progress, float progressFade)
	{
		float breakProgress = Math.Clamp(Math.InverseLerp(0.0,m_ProgressBreakpoint,progress),0,1);
		float sizeX = Math.Lerp(m_ImageStartingSizeX,m_ImageEndSizeX,breakProgress);
		float sizeY = Math.Lerp(m_ImageStartingSizeY,m_ImageEndSizeY,breakProgress);
		m_Widget.SetSize(sizeX,sizeY);
		
		if (progress <= m_ProgressBreakpoint)
		{
			//do stuff before breakpoint
		}
		else
		{
			//do stuff after breakpoint
			float posYTemp = Math.Lerp(m_PosY,m_PosY + m_SlideDistance, progressFade);
			m_Widget.SetPos(m_PosX,posYTemp);
		}
	}
	
	void Update(float timeSlice)
	{
		if (m_IsRunning)
		{
			float progress, progressFade;
			progress = m_TimeTotal/m_Duration;
			progressFade = (m_TimeTotal - m_ProgressBreakpointTime)/m_ProgressFadingDuration;
			
			//alpha
			UpdateAlpha(progress,progressFade);
			//transform + scaling
			UpdateTransform(progress,progressFade);
			
			m_TimeTotal += (timeSlice * m_SpeedCoef);
			
			if (m_TimeTotal >= m_Duration)
			{
				//deletes this;
				StopDrop();
			}
		}
	}
}
