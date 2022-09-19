class PPERequester_MenuEffects extends PPERequester_MenuBase
{
	void SetBlurIntensity(float gauss)
	{
		SetTargetValueFloat(PostProcessEffectType.GaussFilter,PPEGaussFilter.PARAM_INTENSITY,true,gauss,PPEGaussFilter.L_0_MENU,PPOperators.SET);
	}
	
	void SetVignetteIntensity(float vignette)
	{
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTE,false,vignette,PPEGlow.L_25_MENU,PPOperators.SET);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTECOLOR,{0.0,0.0,0.0,0.0},PPEGlow.L_26_MENU,PPOperators.OVERRIDE);
	}
}