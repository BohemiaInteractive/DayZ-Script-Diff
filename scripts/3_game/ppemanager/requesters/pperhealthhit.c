class PPERequester_HealthHitReaction extends PPERequester_GameplayBase
{
	const float INTENSITY_MULT = 6.0;
	
	void SetHitIntensity(float intensity)
	{
		//color overlay
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_OVERLAYFACTOR,true,0.05,PPEGlow.L_20_HIT,PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_OVERLAYCOLOR,{intensity * INTENSITY_MULT, 0.0, 0.0, 1.0},PPEGlow.L_21_HIT,PPOperators.HIGHEST);
	}
}