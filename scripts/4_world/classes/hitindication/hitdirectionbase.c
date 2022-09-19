

class HitDirectionEffectBase
{
	const float DURATION_COEF_MIN = 0.6;
	const float INTENSITY_MIN = 0.6;
	
	float m_HitDirection;
	float m_Duration;
	float m_BreakPoint;
	float m_TimeActive;
	float m_IntensityMax;
	
	Widget m_LayoutRoot;
	Widget m_Image;
	
	DayZPlayer m_Player;
	
	bool m_Initialized;
	int m_SizeXEnf;
	int m_SizeYEnf;
	float m_PosX;
	float m_PosY;
	float m_PosXScreenEdge;
	float m_PosYScreenEdge;
	float m_AngleRad;
	float m_AngleRadPrev;
	float m_SmoothVel[1];
	
	ref HitDirectionImagesBase m_ImageData;
	
	void HitDirectionEffectBase()
	{
		m_Initialized = false;
		m_PosX = 0.0;
		m_PosY = 0.0;
		m_AngleRad = 0.0;
		m_SmoothVel[0] = 0.0;
		
		m_ImageData = GetImageData();
		m_ImageData.GetCurrentImageData(m_LayoutRoot,m_Image);
	}
	
	//! Called manually after object spawn
	void Init(DayZPlayer player, float hit_direction, float intensity_max)
	{
		m_Player = player;
		float duration_coef = Math.Clamp(intensity_max,DURATION_COEF_MIN,1);
		m_IntensityMax = Math.Clamp(intensity_max,INTENSITY_MIN,1);
		m_Duration = m_DurationMax * duration_coef;
		m_BreakPoint = Math.Clamp(m_BreakPointBase * m_Duration,0,m_Duration);
		m_Scatter = Math.Clamp(m_Scatter,0.0,180.0);
		m_HitDirection = hit_direction + (Math.RandomFloatInclusive(0,m_Scatter) * Math.Pow(-1.0,Math.RandomIntInclusive(0,1)));
		
		Widget w = m_LayoutRoot;
		w = w.GetChildren();
		while (w)
		{
			w.Show(m_Image == w);
			w = w.GetSibling();
		}
		
		m_Image.SetColor(m_Color);
		
		m_LayoutRoot.Show(true);
		
		CalculateArrowPosition();
		SetIndicatorRotation();
		SetIndicatorPositon();
		m_Initialized = true;
	}
	
	HitDirectionImagesBase GetImageData(){}
	
	void ~HitDirectionEffectBase()
	{
		m_LayoutRoot.Show(false);
		delete m_LayoutRoot;
	}
	
	bool DurationCheck()
	{
		if ( m_TimeActive >= m_Duration )
		{
			return true;
		}
		return false;
	}
	
	void Update( float timeslice )
	{
		float intensity;
		
		if ( m_TimeActive <= m_BreakPoint )
		{
			intensity = m_IntensityMax;
		}
		else
		{
			float tmp_value = Math.InverseLerp(m_BreakPoint, m_Duration, m_TimeActive);
			intensity = Math.Lerp(m_IntensityMax,0.0,tmp_value);
		}
		
		m_TimeActive += timeslice;
		intensity = Math.Clamp(intensity,0,1);
		
		if ( m_TimeActive >= m_Duration )
		{
			m_LayoutRoot.Show(false);
		}
		else
		{
			m_LayoutRoot.SetAlpha(intensity);
			if ( m_Mode == HitDirectionModes.DYNAMIC )
			{
				CalculateArrowPosition(timeslice);
				SetIndicatorPositon(timeslice);
				SetIndicatorRotation(timeslice);
			}
		}
	}
	
	void CalculateArrowPosition(float timeslice = -1.0)
	{
		m_HitDirection = Math.NormalizeAngle(m_HitDirection);
		
		float angle_direction = m_Player.GetOrientation()[0];
		angle_direction = Math.NormalizeAngle(angle_direction);
		float camera_angle = GetGame().GetCurrentCameraDirection().VectorToAngles()[0];
		camera_angle = Math.NormalizeAngle(camera_angle);
		
		float angle_camera_diff = angle_direction - camera_angle;
		m_AngleRad = m_HitDirection + angle_camera_diff;
		m_AngleRad = Math.NormalizeAngle(m_AngleRad);
		m_AngleRad = m_AngleRad * Math.DEG2RAD;
		
		m_PosX = 0.0;
		m_PosY = 0.0;
		
		GetScreenSize(m_SizeXEnf,m_SizeYEnf);
		
		if ( m_Initialized && timeslice != -1.0 )
		{
			float val = m_AngleRadPrev - m_AngleRad + Math.PI;
			val = Math.ModFloat(val, Math.PI2);
			val -= Math.PI;
			m_AngleRad = m_AngleRadPrev - Math.SmoothCD(0, val, m_SmoothVel, 0.1, 1000, timeslice);
		}
		m_AngleRadPrev = m_AngleRad;
		
		m_PosXScreenEdge = Math.Clamp(Math.Sin(m_AngleRad)/Math.Sin(Math.PI/4),-1,1) * ( m_SizeXEnf/2 + m_DistanceAdjust * m_SizeXEnf);
		m_PosYScreenEdge = Math.Clamp(-1 * Math.Cos(m_AngleRad)/Math.Cos(Math.PI/4),-1,1) * ( m_SizeYEnf/2 + m_DistanceAdjust * m_SizeYEnf);
		
		FinalizePositionCalculation();
	}
	
	//! specific handling on individual indicator type
	void FinalizePositionCalculation(){}
	void SetIndicatorRotation(float timeslice = -1.0){}
	
	void SetIndicatorPositon(float timeslice = -1.0)
	{
		m_LayoutRoot.SetPos(m_PosX,m_PosY,true);
	}
	
	//-----------------------------------------------------------------------
	//Static stuff below
	//-----------------------------------------------------------------------
	static bool m_ServerOverrideEnabled;
	static int m_Mode;
	static int m_ID;
	static int m_Color;
	static protected typename m_Type;
	static float m_DurationMax;
	static float m_BreakPointBase; //! range 0..1, a point where the fading starts
	static float m_DistanceAdjust;
	static int m_RotationOverride;
	static float m_Scatter; //! range 0..180, randomized offset of direction to make it less acurate
	
	//!sets override values, or defaults
	static void CheckValues()
	{
		m_ServerOverrideEnabled = CfgGameplayHandler.GetHitIndicationOverrideEnabled();
		if (m_ServerOverrideEnabled)
		{
			m_Mode = CfgGameplayHandler.GetHitIndicationMode();
			m_ID = CfgGameplayHandler.GetHitIndicationTypeID();
			m_Color = CfgGameplayHandler.GetHitIndicationIndicatorColor();
			m_DurationMax = CfgGameplayHandler.GetHitIndicationMaxDuration();
			m_BreakPointBase = CfgGameplayHandler.GetHitIndicationBreakPoint();
			m_Scatter = CfgGameplayHandler.GetHitIndicationScatter();
		}
		else
		{
			m_Mode = HitDirectionModes.STATIC;
			m_ID = HitIndicatorType.SPLASH;
			m_Color = HitDirectionConstants.COLOR_DEFAULT;
			m_DurationMax = HitDirectionConstants.DURATION_BASE;
			m_BreakPointBase = HitDirectionConstants.BREAKPOINT_BASE;
			m_Scatter = HitDirectionConstants.SCATTER;
		}
		m_DistanceAdjust = HitDirectionConstants.DISTANCE_ADJUST;
		m_RotationOverride = HitDirectionConstants.ROTATION_DEFAULT;
	}
	
	static typename GetCurrentType()
	{
		switch (m_ID)
		{
			case HitIndicatorType.SPLASH:
				m_Type = HitDirectionEffectSplash;
			break;
			
			case HitIndicatorType.SPIKE:
				m_Type = HitDirectionEffectSpike;
			break;
			
			case HitIndicatorType.ARROW:
				m_Type = HitDirectionEffectArrow;
			break;
			
			default:
				ErrorEx("Unknown HitDirection mode, using HitIndicatorType.SPLASH",ErrorExSeverity.INFO);
				m_Type = HitDirectionEffectSplash;
			break;
		}
		
		return m_Type;
	}
}