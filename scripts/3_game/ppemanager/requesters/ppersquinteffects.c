class PPERequester_SquintEffects : PPERequester_GameplayBase
{
	//! manually requested on demand
	void SetBlur(float intensity)
	{
		SetTargetValueFloat(PostProcessEffectType.GaussFilter,PPEGaussFilter.PARAM_INTENSITY,true,intensity,PPEGaussFilter.L_0_FEVER,PPOperators.ADD_RELATIVE);
	}
	
	//! manually requested on demand
	void ClearBlur()
	{
		SetTargetValueFloat(PostProcessEffectType.GaussFilter,PPEGaussFilter.PARAM_INTENSITY,true,0.0,PPEGaussFilter.L_0_FEVER,PPOperators.ADD_RELATIVE);
	}
}