class PPERequester_TunnelVisionEffects extends PPERequester_GameplayBase
{
	protected float m_Intensity;
	
	override protected void OnStart(Param par = null)
	{
		super.OnStart();
		
		m_Intensity = Param1<float>.Cast(par).param1;
		
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTE,false,m_Intensity,PPEGlow.L_25_TUNNEL,PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTECOLOR,{0.0,0.0,0.0,0.0},PPEGlow.L_26_TUNNEL,PPOperators.LOWEST);
	}
}