class PPERequester_FlashbangEffects extends PPERequester_GameplayBase
{
	const float VAL_FACTOR = 0.85;
	const float EXPOSURE_MAX = 50;
	protected float m_Exposure;
	protected float m_Intensity;
	
	override protected void OnStart(Param par = null)
	{
		if (!m_IsRunning)
		{
			GetGame().GetMission().GetEffectWidgets().AddActiveEffects({EffectWidgetsTypes.COVER_FLASHBANG});
		}
		
		super.OnStart();
	}
	
	override protected void OnStop(Param par = null)
	{
		super.OnStop();
		
		m_Intensity = 0.0;
		m_Exposure = 0.0;
		
		GetGame().GetMission().GetEffectWidgets().RemoveActiveEffects({EffectWidgetsTypes.COVER_FLASHBANG});
	}
	
	override protected void OnUpdate( float delta )
	{
		super.OnUpdate( delta );
		
		if ( GetGame() && GetGame().GetMission() && GetGame().GetMission().GetEffectWidgets() )
		{
			Param1<float> par = new Param1<float>(1 - m_Intensity);
			GetGame().GetMission().GetEffectWidgets().UpdateWidgets(EffectWidgetsTypes.COVER_FLASHBANG,0,par,EffectWidgetHandles.FLASHBANG);
		}
	}
	
	void SetFlashbangIntensity(float intensity, float daytime_toggle)
	{
		m_Intensity = intensity;
		array<float> overlay_color = {1.0, 1.0, 1.0, intensity};
		if ( daytime_toggle <= 5.0 )
		{
			m_Exposure = Math.Clamp(intensity * daytime_toggle,0.0,EXPOSURE_MAX);
		}
		else
		{
			m_Exposure = Easing.EaseInQuart(intensity);
			m_Exposure = Math.Clamp(EXPOSURE_MAX * m_Exposure,0.0,EXPOSURE_MAX);
		}
		overlay_color[3] = intensity;
		
		//blur
		SetTargetValueFloat(PostProcessEffectType.GaussFilter,PPEGaussFilter.PARAM_INTENSITY,true,intensity,PPEGaussFilter.L_0_FLASHBANG,PPOperators.ADD_RELATIVE);
		//overlay
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_OVERLAYFACTOR,true,intensity,PPEGlow.L_20_FLASHBANG,PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_OVERLAYCOLOR,overlay_color,PPEGlow.L_21_FLASHBANG,PPOperators.SET);
		//exposure
		SetTargetValueFloat(PPEExceptions.EXPOSURE,PPEExposureNative.PARAM_INTENSITY,false,m_Exposure,PPEExposureNative.L_0_FLASHBANG,PPOperators.ADD);
	}
}