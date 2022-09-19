class PPERequester_ShockHitReaction extends PPERequester_GameplayBase
{
	protected float m_Gauss;
	protected float m_VignetteIntensity;
	protected float m_OverlayFactor;
	protected ref array<float> m_OverlayColor;
	
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		m_Gauss = Param3<float,float,float>.Cast(par).param1;
		m_VignetteIntensity = Param3<float,float,float>.Cast(par).param2;
		m_OverlayFactor = Param3<float,float,float>.Cast(par).param3;
		
		m_OverlayColor = {0.0, 0.0, 0.0, 1.0};
		if (m_OverlayFactor > 0.0)
			m_OverlayColor = {0.1, 0.1, 0.1, 1.0};
		
		//blur
		SetTargetValueFloat(PostProcessEffectType.GaussFilter,PPEGaussFilter.PARAM_INTENSITY,true,m_Gauss,PPEGaussFilter.L_0_SHOCK,PPOperators.ADD_RELATIVE);
		//vignette
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTE,false,m_VignetteIntensity,PPEGlow.L_25_SHOCK,PPOperators.ADD);
		//SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTECOLOR,{0.25,0.25,0.25,0.0},PPEGlow.L_26_SHOCK,PPOperators.ADD_RELATIVE);
		//color overlay
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_OVERLAYFACTOR,true,m_OverlayFactor,PPEGlow.L_20_SHOCK,PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_OVERLAYCOLOR,m_OverlayColor,PPEGlow.L_21_SHOCK,PPOperators.SET);
	}
}